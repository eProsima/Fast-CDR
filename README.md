# Fast CDR

[![License](https://img.shields.io/github/license/eProsima/Fast-CDR.svg)](https://opensource.org/licenses/Apache-2.0)
[![Releases](https://img.shields.io/github/v/release/eProsima/Fast-CDR?sort=semver)](https://github.com/eProsima/Fast-CDR/releases)
[![Issues](https://img.shields.io/github/issues/eProsima/Fast-CDR.svg)](https://github.com/eProsima/Fast-CDR/issues)
[![Forks](https://img.shields.io/github/forks/eProsima/Fast-CDR.svg)](https://github.com/eProsima/Fast-CDR/network/members)
[![Stars](https://img.shields.io/github/stars/eProsima/Fast-CDR.svg)](https://github.com/eProsima/Fast-CDR/stargazers)
[![Linux ci](http://jenkins.eprosima.com:8080/job/nightly_fastcdr_master_linux/badge/icon?subject=%20%20%20Linux%20CI%20)](http://jenkins.eprosima.com:8080/job/nightly_fastcdr_master_linux)
[![Linux arm64 ci](http://jenkins.eprosima.com:8080/job/nightly_fastcdr_master_linux_aarch64/badge/icon?subject=%20%20%20Linux-aarch64%20CI%20)](http://jenkins.eprosima.com:8080/view/Nightly/job/nightly_fastcdr_master_linux_aarch64/)
[![Windows ci](http://jenkins.eprosima.com:8080/job/nightly_fastcdr_master_windows/label=windows-secure,platform=x64,toolset=v141/badge/icon?subject=%20%20%20%20Windows%20CI%20)](http://jenkins.eprosima.com:8080/job/nightly_fastcdr_master_windows/label=windows-secure,platform=x64,toolset=v141)
[![Mac ci](http://jenkins.eprosima.com:8080/job/nightly_fastcdr_master_mac/badge/icon?subject=%20%20%20%20%20%20%20Mac%20CI%20)](http://jenkins.eprosima.com:8080/job/nightly_fastcdr_master_mac)
[![Coverage](https://img.shields.io/jenkins/coverage/cobertura.svg?jobUrl=http%3A%2F%2Fjenkins.eprosima.com%3A8080%2Fjob%2Fnightly_fastcdr_coverage_linux)](http://jenkins.eprosima.com:8080/job/nightly_fastcdr_coverage_linux)

**eProsima Fast CDR** is a C++ library that provides two serialization mechanisms.
One is the standard CDR serialization mechanism, while the other is a faster implementation that modifies the standard.

## Build

**eProsima Fast CDR** provides [CMake][cmake] scripts to build and install it.
Also in [eProsima][eprosima] you can find packages for Linux using autotools and binaries for Windows.

[cmake]: http://www.cmake.org
[eprosima]: http://www.eprosima.com

## Quality Declaration

**eprosima Fast CDR** claims to be in the **Quality Level 1** category based on the guidelines provided by [ROS 2](https://ros.org/reps/rep-2004.html).
See the [Quality Declaration](QUALITY.md) for more details.
