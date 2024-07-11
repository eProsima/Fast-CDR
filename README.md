# Fast CDR

[![License](https://img.shields.io/github/license/eProsima/Fast-CDR.svg)](https://opensource.org/licenses/Apache-2.0)
[![Releases](https://img.shields.io/github/v/release/eProsima/Fast-CDR?sort=semver)](https://github.com/eProsima/Fast-CDR/releases)
[![Issues](https://img.shields.io/github/issues/eProsima/Fast-CDR.svg)](https://github.com/eProsima/Fast-CDR/issues)
[![Forks](https://img.shields.io/github/forks/eProsima/Fast-CDR.svg)](https://github.com/eProsima/Fast-CDR/network/members)
[![Stars](https://img.shields.io/github/stars/eProsima/Fast-CDR.svg)](https://github.com/eProsima/Fast-CDR/stargazers)
[![Fast CDR Ubuntu CI (nightly)](https://github.com/eProsima/Fast-CDR/actions/workflows/nightly-ubuntu-ci.yml/badge.svg)](https://github.com/eProsima/Fast-CDR/actions/workflows/nightly-ubuntu-ci.yml)
[![Linux arm64 ci](http://jenkins.eprosima.com:8080/job/nightly_fastcdr_master_linux_aarch64/badge/icon?subject=%20%20%20Linux-aarch64%20CI%20)](http://jenkins.eprosima.com:8080/view/Nightly/job/nightly_fastcdr_master_linux_aarch64/)
[![Fast CDR Windows CI (nightly)](https://github.com/eProsima/Fast-CDR/actions/workflows/nightly-windows-ci.yml/badge.svg)](https://github.com/eProsima/Fast-CDR/actions/workflows/nightly-windows-ci.yml)
[![Fast CDR MacOS CI (nightly)](https://github.com/eProsima/Fast-CDR/actions/workflows/nightly-mac-ci.yml/badge.svg)](https://github.com/eProsima/Fast-CDR/actions/workflows/nightly-mac-ci.yml)
[![Coverage](https://img.shields.io/jenkins/coverage/cobertura.svg?jobUrl=http%3A%2F%2Fjenkins.eprosima.com%3A8080%2Fjob%2Fnightly_fastcdr_coverage_linux)](http://jenkins.eprosima.com:8080/job/nightly_fastcdr_coverage_linux)

**eProsima Fast CDR** is a C++ library that provides two serialization mechanisms.
One is the standard CDR serialization mechanism, while the other is a faster implementation that modifies the standard.

## Build

**eProsima Fast CDR** provides [CMake][cmake] scripts to build and install it (please read the [installation guide](https://fast-dds.docs.eprosima.com/en/latest/installation/sources/sources_linux.html#cmake-installation) for more details).
Also, in [eProsima][eprosima] you can find packages for Linux using autotools and binaries for Windows.

[cmake]: http://www.cmake.org
[eprosima]: http://www.eprosima.com

## Quality Declaration

**eprosima Fast CDR** claims to be in the **Quality Level 1** category based on the guidelines provided by [ROS 2](https://ros.org/reps/rep-2004.html).
See the [Quality Declaration](QUALITY.md) for more details.
