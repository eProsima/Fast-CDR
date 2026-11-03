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

# Fast CDR compiled as the FACE CTS compiles a Unit of Conformance, for the C++11 General Purpose capability set
# (utils/face/check_face.py). The flags of the CTS toolchain are GCC's.
if(NOT (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_SYSTEM_NAME STREQUAL "Linux"))
    message(WARNING "FASTCDR_FACE_TEST needs GCC on Linux: the FACE test is not added")
    return()
endif()

find_package(Python3 COMPONENTS Interpreter)
if(NOT Python3_Interpreter_FOUND)
    message(WARNING "FASTCDR_FACE_TEST needs Python 3: the FACE test is not added")
    return()
endif()

# The definitions the library sources are built with (src/cpp/CMakeLists.txt)
set(FACE_DEFINES --define ${PROJECT_NAME_UPPER}_SOURCE)
if(STRICT_BOOL)
    list(APPEND FACE_DEFINES --define ${PROJECT_NAME_UPPER}_STRICT_BOOL)
endif()

add_test(NAME FaceGeneralPurpose
    COMMAND ${Python3_EXECUTABLE} ${PROJECT_SOURCE_DIR}/utils/face/check_face.py
        --source-dir ${PROJECT_SOURCE_DIR}
        --build-dir ${PROJECT_BINARY_DIR}
        --work-dir ${CMAKE_CURRENT_BINARY_DIR}/work
        --cxx ${CMAKE_CXX_COMPILER}
        --nm ${CMAKE_NM}
        ${FACE_DEFINES}
    )
set_tests_properties(FaceGeneralPurpose PROPERTIES LABELS "face")
