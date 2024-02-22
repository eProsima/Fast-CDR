# Copyright 2024 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

macro(find_or_add_gtest)
    # Check if building on an ament context, i.e. ROS 2
    # Thanks to https://github.com/facontidavide/PlotJuggler/blob/main/CMakeLists.txt#L66
    find_package(ament_cmake QUIET)

    # This is a ROS 2 build
    if(ament_cmake_FOUND)
        # Find all GTest vendor required packages
        find_package(ament_cmake REQUIRED)
        find_package(gtest_vendor REQUIRED)
        find_package(ament_cmake_gtest REQUIRED)

        # Find GTest
        ament_find_gtest()

        # Add aliases for GTest libraries so we can use them as targets independently of the context
        add_library(GTest::gtest ALIAS gtest)
        add_library(GTest::gtest_main ALIAS gtest_main)
        target_link_libraries(gtest_main gtest)

    # This is a non-ROS 2 build
    else()
        # Find GTest normally
        find_package(GTest CONFIG REQUIRED)
    endif()
endmacro()
