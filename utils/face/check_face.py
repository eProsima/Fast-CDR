#!/usr/bin/env python3
# Copyright 2026 Proyectos y Sistemas de Mantenimiento SL (eProsima).
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
"""Checks Fast CDR against the C++11 General Purpose capability set of the FACE Technical Standard, Edition 3.2.

Fast CDR is compiled as the FACE Conformance Test Suite (CTS) compiles a Unit of Conformance: with the compiler flags
of its toolchain (-nostdinc -nostdinc++ ...) and only the gold standard headers of the profile, which declare just what
the profile allows, each in the form the C++ standard guarantees for its header. Then the functions the objects use
must be defined by the gold standard libraries, or by the allowed definitions of the toolchain.

* Each public header is compiled alone, so that it includes what it uses.
* Each source is compiled to an object, with the definitions of the library build.
* The undefined symbols of the objects must be defined by the gold standard libraries (built from their CTS sources),
  the default allowed definitions of the CTS, or allowed_symbols.txt.

Only the gold standard files of the capability set are fetched from the CTS archive published by The Open Group
(Distribution Statement A, unlimited), with HTTP range requests, and checked against GOLD_SHA256. A local CTS
installation can be given instead (--cts-dir).

The result is the list of issues: each compiler error, or function outside the profile, at the place in Fast CDR where
it is, with the checked files that run into it. The first error of a file is marked: the later ones may be its
consequences.

Exit code: 0 when there is no issue; 1 when there is any; 2 when the check could not run.
"""
import argparse
import collections
import concurrent.futures
import hashlib
import io
import os
import re
import shutil
import subprocess
import sys
import urllib.request
import zipfile

CTS_VERSION = '3.2.3'
CTS_URL = 'https://www.opengroup.org/sites/default/files/Linux_CTS_{}_Distribution.zip'.format(CTS_VERSION)
CTS_ROOT = 'FACEConformanceTestSuite_{}/'.format(CTS_VERSION)
# The gold standard headers and libraries of the C++11 General Purpose capability set, and the default allowed
# definitions of the CTS toolchains.
GOLD_DIR = 'goldStandardLibraries/C++/src/OSS/libstdc++_v11/general/'
ALLOWED_DEFINITIONS = 'datafiles/sourcefiles/default_allowed_definitions.cpp'
# SHA-256 of the fetched files (see files_sha256)
GOLD_SHA256 = '8e60fa509b5b4643d7632eaf6bc08b7af3bcca1d51bd6baa9320b1e8f29955fb'

# The compiler flags of the CTS C++11 General Purpose toolchain
FACE_FLAGS = ['-fno-builtin', '-nostdinc', '-nostdinc++', '-DFACE_GENERAL_PURPOSE_PROFILE', '-std=c++11']

# The headers the CTS generates for each toolchain, which the gold standard headers and the allowed definitions
# include: the exact-width types of the compiler, the null pointer constant, and the declarations of the allowed
# definitions (none with GCC on Linux: the CTS sample toolchain only declares atexit for MinGW and Cygwin).
TOOLCHAIN_HEADERS = {
    'allowed_definitions.hpp': '''\
#ifndef CTS_ALLOWED_DEFINITIONS_HPP
#define CTS_ALLOWED_DEFINITIONS_HPP
#endif
''',
    'CONFORMANCE_TEST_FACE_EXACT_TYPES.h': '''\
#ifndef CONFORMANCE_TEST_FACE_EXACT_TYPES_H
#define CONFORMANCE_TEST_FACE_EXACT_TYPES_H
typedef __INT8_TYPE__ FACE_int8_t;
typedef __INT16_TYPE__ FACE_int16_t;
typedef __INT32_TYPE__ FACE_int32_t;
typedef __INT64_TYPE__ FACE_int64_t;
typedef __UINT8_TYPE__ FACE_uint8_t;
typedef __UINT16_TYPE__ FACE_uint16_t;
typedef __UINT32_TYPE__ FACE_uint32_t;
typedef __UINT64_TYPE__ FACE_uint64_t;
typedef __SIZE_TYPE__ FACE_size_t;
#endif
''',
    'CONFORMANCE_TEST_FACE_NULL.h': '''\
#ifndef CONFORMANCE_TEST_FACE_NULL_H
#define CONFORMANCE_TEST_FACE_NULL_H
#ifdef __cplusplus
#define FACE_NULL 0
#else
#define FACE_NULL ((void*) 0)
#endif
#endif
''',
}

# A compiler error: file, line, column, message
ERROR = re.compile(r'^(.+?):(\d+):(\d+): (?:fatal )?error: (.*)$')


class HttpRangeFile(io.RawIOBase):
    """A remote file read with HTTP range requests, so that zipfile reads only the entries it needs."""

    def __init__(self, url):
        self.url = url
        self.pos = 0
        with urllib.request.urlopen(urllib.request.Request(url, method='HEAD'), timeout=60) as response:
            if response.headers.get('Accept-Ranges') != 'bytes':
                raise RuntimeError('{} does not accept range requests'.format(url))
            self.size = int(response.headers['Content-Length'])

    def readable(self):
        return True

    def seekable(self):
        return True

    def tell(self):
        return self.pos

    def seek(self, offset, whence=io.SEEK_SET):
        self.pos = {io.SEEK_SET: 0, io.SEEK_CUR: self.pos, io.SEEK_END: self.size}[whence] + offset
        return self.pos

    def readinto(self, buffer):
        if self.pos >= self.size or len(buffer) == 0:
            return 0
        end = min(self.pos + len(buffer), self.size) - 1
        request = urllib.request.Request(self.url, headers={'Range': 'bytes={}-{}'.format(self.pos, end)})
        with urllib.request.urlopen(request, timeout=60) as response:
            data = response.read()
        buffer[:len(data)] = data
        self.pos += len(data)
        return len(data)


def files_sha256(directory):
    """SHA-256 of the relative paths and contents of every file under a directory, in a fixed order."""
    digest = hashlib.sha256()
    for root, dirs, files in os.walk(directory):
        dirs.sort()
        for name in sorted(files):
            path = os.path.join(root, name)
            digest.update(os.path.relpath(path, directory).replace(os.sep, '/').encode() + b'\0')
            with open(path, 'rb') as f:
                digest.update(hashlib.sha256(f.read()).digest())
    return digest.hexdigest()


def fetch_cts(destination):
    """Fetches the gold standard files and the allowed definitions into destination, unless they are already there."""
    if os.path.isdir(destination) and files_sha256(destination) == GOLD_SHA256:
        return
    shutil.rmtree(destination, ignore_errors=True)
    print('Fetching the gold standard files of FACE CTS {} from {}'.format(CTS_VERSION, CTS_URL), flush=True)
    with zipfile.ZipFile(io.BufferedReader(HttpRangeFile(CTS_URL), buffer_size=1 << 20)) as archive:
        for name in archive.namelist():
            relative = name[len(CTS_ROOT):] if name.startswith(CTS_ROOT) else None
            if name.endswith('/') or relative is None or not (
                    relative.startswith(GOLD_DIR) or relative == ALLOWED_DEFINITIONS):
                continue
            path = os.path.join(destination, *relative.split('/'))
            os.makedirs(os.path.dirname(path), exist_ok=True)
            with open(path, 'wb') as f:
                f.write(archive.read(name))
    actual = files_sha256(destination)
    if actual != GOLD_SHA256:
        raise RuntimeError('The fetched files do not match GOLD_SHA256 (got {}): the published CTS may have changed; '
                           'check {} and update GOLD_SHA256'.format(actual, CTS_URL))


def compile_file(cxx, flags, source, output):
    """Compiles a file; returns whether it compiled, and the compiler output."""
    command = [cxx] + flags + (['-fsyntax-only'] if output is None else ['-c', '-o', output]) + [source]
    result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, universal_newlines=True)
    return result.returncode == 0, result.stdout


def run(command, **kwargs):
    return subprocess.run(command, stdout=subprocess.PIPE, universal_newlines=True, check=True, **kwargs).stdout


def symbols(nm, objects, defined):
    """The symbols that objects define, or the undefined ones."""
    if not objects:
        return set()
    flag = '--defined-only' if defined else '--undefined-only'
    output = run([nm, flag, '--format=just-symbols'] + objects)
    return {line.strip() for line in output.splitlines() if line.strip() and not line.endswith(':')}


def demangle(name):
    try:
        return run(['c++filt', name]).strip()
    except (OSError, subprocess.CalledProcessError):
        return name


def call_sites(obj, names):
    """The source lines of an object (built with -g) that refer to each of the names: {name: {'file:line'}}."""
    sites = collections.defaultdict(set)
    try:
        relocations = run(['readelf', '-rW', obj])
    except (OSError, subprocess.CalledProcessError):
        return sites
    section = None
    offsets = collections.defaultdict(list)  # section: [(offset, name)]
    for line in relocations.splitlines():
        header = re.match(r"Relocation section '\.rela?(\S+)'", line)
        if header:
            section = header.group(1)
            continue
        fields = line.split()
        if section and len(fields) >= 5 and re.match(r'^[0-9a-f]+$', fields[0]):
            name = fields[4].split('@')[0]
            if name in names:
                offsets[section].append((fields[0], name))
    for section, entries in offsets.items():
        try:
            lines = run(['addr2line', '-e', obj, '-j', section] + [offset for offset, _ in entries]).splitlines()
        except (OSError, subprocess.CalledProcessError):
            continue
        for (_, name), location in zip(entries, lines):
            if not location.startswith('??'):
                sites[name].add(location.split(' ')[0])
    return sites


def read_list(path):
    """The non-empty lines of a file, without comments."""
    lines = []
    with open(path) as f:
        for line in f:
            line = line.split('#', 1)[0].strip()
            if line:
                lines.append(line)
    return lines


def main():
    here = os.path.dirname(os.path.abspath(__file__))
    parser = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    parser.add_argument('--source-dir', required=True, help='Fast CDR sources')
    parser.add_argument('--build-dir', required=True, help='Fast CDR build, for its generated headers')
    parser.add_argument('--work-dir', required=True, help='Directory for the fetched files and the objects')
    parser.add_argument('--cts-dir', help='A local CTS installation, instead of fetching its files')
    parser.add_argument('--cxx', default='g++', help='GCC C++ compiler (default: g++)')
    parser.add_argument('--nm', default='nm', help='nm (default: nm)')
    parser.add_argument('--define', action='append', default=[], help='Definition of the library sources')
    parser.add_argument('--allowed-symbols', default=os.path.join(here, 'allowed_symbols.txt'),
                        help='Symbols allowed besides those of the CTS (default: allowed_symbols.txt)')
    parser.add_argument('--jobs', type=int, default=os.cpu_count() or 1)
    args = parser.parse_args()

    source_dir = os.path.abspath(args.source_dir)
    build_dir = os.path.abspath(args.build_dir)
    work_dir = os.path.abspath(args.work_dir)

    # The CTS files and the toolchain headers
    try:
        if args.cts_dir:
            cts_dir = os.path.abspath(args.cts_dir)
        else:
            cts_dir = os.path.join(work_dir, 'cts')
            fetch_cts(cts_dir)
    except (OSError, RuntimeError, zipfile.BadZipFile) as error:
        print('Could not get the CTS files: {}'.format(error), file=sys.stderr)
        return 2
    gold_dir = os.path.join(cts_dir, *GOLD_DIR.split('/'))
    toolchain_dir = os.path.join(work_dir, 'toolchain')
    os.makedirs(toolchain_dir, exist_ok=True)
    for name, contents in TOOLCHAIN_HEADERS.items():
        with open(os.path.join(toolchain_dir, name), 'w') as f:
            f.write(contents)
    face_flags = FACE_FLAGS + ['-I' + gold_dir, '-I' + toolchain_dir]

    # The gold standard libraries and the allowed definitions
    objects_dir = os.path.join(work_dir, 'objects')
    shutil.rmtree(objects_dir, ignore_errors=True)
    os.makedirs(os.path.join(objects_dir, 'gold'))
    os.makedirs(os.path.join(objects_dir, 'fastcdr'))
    gold_sources = sorted(os.path.join(root, name) for root, _, files in os.walk(gold_dir)
                          for name in files if name.endswith('.cpp'))
    gold_sources.append(os.path.join(cts_dir, *ALLOWED_DEFINITIONS.split('/')))
    gold_objects = [os.path.join(objects_dir, 'gold', '{}.o'.format(i)) for i in range(len(gold_sources))]

    # Fast CDR: each public header alone, and each source (with debug information, to locate the calls)
    headers = sorted(os.path.relpath(os.path.join(root, name), source_dir) for root, _, files in
                     os.walk(os.path.join(source_dir, 'include')) for name in files if name.endswith(('.h', '.hpp')))
    sources = sorted(os.path.relpath(os.path.join(root, name), source_dir) for root, _, files in
                     os.walk(os.path.join(source_dir, 'src', 'cpp')) for name in files if name.endswith('.cpp'))
    fastcdr_flags = face_flags + ['-I' + os.path.join(source_dir, 'include'), '-I' + os.path.join(build_dir, 'include')]
    source_flags = fastcdr_flags + ['-g', '-I' + os.path.join(source_dir, 'src', 'cpp')] + \
        ['-D' + d for d in args.define]
    units_dir = os.path.join(work_dir, 'units')
    os.makedirs(units_dir, exist_ok=True)

    jobs = []  # (checked file, compiler flags, file to compile, object or None)
    for header in headers:
        unit = os.path.join(units_dir, header.replace(os.sep, '_') + '.cpp')
        with open(unit, 'w') as f:
            f.write('#include <{}>\n'.format(os.path.relpath(header, 'include').replace(os.sep, '/')))
        jobs.append((header, fastcdr_flags, unit, None))
    for i, source in enumerate(sources):
        jobs.append((source, source_flags, os.path.join(source_dir, source),
                     os.path.join(objects_dir, 'fastcdr', '{}.o'.format(i))))

    with concurrent.futures.ThreadPoolExecutor(max(1, args.jobs)) as pool:
        gold_results = list(pool.map(lambda job: compile_file(args.cxx, face_flags, *job),
                                     zip(gold_sources, gold_objects)))
        results = list(pool.map(lambda job: compile_file(args.cxx, job[1], job[2], job[3]), jobs))
    gold_failures = [(source, output) for source, (ok, output) in zip(gold_sources, gold_results) if not ok]
    if gold_failures:
        for source, output in gold_failures:
            print('Gold standard source {} does not compile:\n{}'.format(source, output), file=sys.stderr)
        return 2

    def where(path):
        """A path as reported: relative to the Fast CDR sources or build, or to the CTS."""
        path = os.path.normpath(os.path.join(source_dir, path))
        for base, prefix in ((units_dir, ''), (source_dir, ''), (build_dir, '<build>/'), (cts_dir, '<CTS>/')):
            if path.startswith(base + os.sep):
                return prefix + os.path.relpath(path, base)
        return path

    # The issues: {(location, message): {checked file: whether it is its first error}}
    issues = collections.defaultdict(dict)
    logs_dir = os.path.join(work_dir, 'logs')
    shutil.rmtree(logs_dir, ignore_errors=True)
    os.makedirs(logs_dir)
    for (checked, _, _, _), (ok, output) in zip(jobs, results):
        if ok:
            continue
        with open(os.path.join(logs_dir, checked.replace(os.sep, '_') + '.log'), 'w') as f:
            f.write(output)
        errors = [ERROR.match(line) for line in output.splitlines()]
        errors = [e for e in errors if e]
        if not errors:
            issues[(checked, 'does not compile (see the log)')][checked] = True
        for index, error in enumerate(errors):
            location = '{}:{}:{}'.format(where(error.group(1)), error.group(2), error.group(3))
            key = (location, error.group(4))
            issues[key][checked] = issues[key].get(checked, False) or index == 0

    # The functions each object of Fast CDR uses from outside Fast CDR, against those of the CTS and
    # allowed_symbols.txt
    fastcdr_objects = [job[3] for job, (ok, _) in zip(jobs, results) if job[3] is not None and ok]
    allowed = (symbols(args.nm, gold_objects, True) | symbols(args.nm, fastcdr_objects, True) |
               set(read_list(args.allowed_symbols)))
    for (checked, _, _, obj), (ok, _) in zip(jobs, results):
        if obj is None or not ok:
            continue
        outside = symbols(args.nm, [obj], False) - allowed
        sites = call_sites(obj, outside)
        for name in sorted(outside):
            message = 'uses {}, which is not in the profile'.format(demangle(name))
            for site in sorted(sites.get(name)) or [checked]:
                issues[(where(site), message)][checked] = True

    # Report
    def sort_key(item):
        (location, _), _ = item
        parts = location.split(':')
        return (parts[0],) + tuple(int(p) if p.isdigit() else 0 for p in parts[1:])

    failed = {checked for found_in in issues.values() for checked in found_in}
    first_errors = [item for item in sorted(issues.items(), key=sort_key) if any(item[1].values())]
    if first_errors:
        print('First error of each failing file (later errors may be their consequences):')
        for (location, message), found_in in first_errors:
            print('  {}: {}'.format(location, message))
            print('      in: ' + ', '.join(sorted(c for c, is_first in found_in.items() if is_first)))
        print('\nAll issues:')
    for (location, message), found_in in sorted(issues.items(), key=sort_key):
        print('{}: {}'.format(location, message))
        first = sorted(checked for checked, is_first in found_in.items() if is_first)
        later = sorted(checked for checked, is_first in found_in.items() if not is_first)
        if first:
            print('    first error of: ' + ', '.join(first))
        if later:
            print('    also in: ' + ', '.join(later))
    print('\n{} issues in {} of {} checked files ({} headers, {} sources).'.format(
        len(issues), len(failed), len(jobs), len(headers), len(sources)))
    if failed:
        print('Compiler output: {}'.format(logs_dir))
    return 1 if issues else 0


if __name__ == '__main__':
    sys.exit(main())
