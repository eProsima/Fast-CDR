# Copyright 2023 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

###############################################################################
# Unit tests for XCDR extensions
###############################################################################
set(XCDR_TEST_SOURCE
    appendable.cpp
    basic_types.cpp
    external.cpp
    final.cpp
    mutable.cpp
    optional.cpp
    xcdrv1.cpp
    xcdrv2.cpp
    )
add_executable(xcdr_tests ${XCDR_TEST_SOURCE})
set_common_compile_options(xcdr_tests)
target_link_libraries(xcdr_tests fastcdr GTest::gtest_main)
gtest_discover_tests(xcdr_tests)
