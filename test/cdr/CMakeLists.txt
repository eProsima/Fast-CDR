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
# fixed_size_string tests
###############################################################################
add_executable(FixedSizeStringTests fixed_size_string.cpp)
set_common_compile_options(FixedSizeStringTests)
target_link_libraries(FixedSizeStringTests fastcdr GTest::gtest_main)
gtest_discover_tests(FixedSizeStringTests)

###############################################################################
# array_as_std_vector tests
###############################################################################
add_executable(ArrayAsSTDVectorTests array_as_std_vector.cpp)
set_common_compile_options(ArrayAsSTDVectorTests)
target_link_libraries(ArrayAsSTDVectorTests fastcdr GTest::gtest_main)
gtest_discover_tests(ArrayAsSTDVectorTests)

###############################################################################
# Old cdr unit tests
###############################################################################
set(UNITTESTS_SOURCE SimpleTest.cpp ResizeTest.cpp)
add_executable(UnitTests ${UNITTESTS_SOURCE})
set_common_compile_options(UnitTests)
target_link_libraries(UnitTests fastcdr GTest::gtest_main)
gtest_discover_tests(UnitTests)
