###############################################################################
# CMake build rules for FastCDR                                               #
###############################################################################
cmake_minimum_required(VERSION 2.8.11)

# Set CMAKE_BUILD_TYPE to Release by default.
if(DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
        )
else()
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
        )
endif()

project("fastcdr")
set(PROJECT_NAME_STYLED "FastCDR")
set(PROJECT_NAME_LARGE "Fast CDR")
string(TOUPPER "${PROJECT_NAME}" PROJECT_NAME_UPPER)
set(${PROJECT_NAME}_DESCRIPTION_SUMMARY "C++ library for serialize using CDR serialization")
set(${PROJECT_NAME}_DESCRIPTION "eProsima ${PROJECT_NAME_LARGE} library provides two serialization mechanisms. One is the standard CDR serialization mechanism, while the other is a faster implementation that modifies the standard.")

message(STATUS "Configuring ${PROJECT_NAME_LAGE}")

###############################################################################
# Version information                                                         #
###############################################################################

#If not set, copy over product version from configure.ac
if(NOT PROJECT_MAJOR_VERSION)
    file(STRINGS
        ${PROJECT_SOURCE_DIR}/configure.ac
        PROJECT_MAJOR_VERSION_TMP
        REGEX "^m4_define\\(\\[?version_major\\]?, *\\[?[0-9]+\\]?\\)"
        )
    file(STRINGS
        ${PROJECT_SOURCE_DIR}/configure.ac
        PROJECT_MINOR_VERSION_TMP
        REGEX "^m4_define\\(\\[?version_minor\\]?, *\\[?[0-9]+\\]?\\)"
        )
    file(STRINGS
        ${PROJECT_SOURCE_DIR}/configure.ac
        PROJECT_MICRO_VERSION_TMP
        REGEX "^m4_define\\(\\[?version_micro\\]?, *\\[?[0-9]+\\]?\\)"
        )

    string(REGEX REPLACE "m4_define\\(\\[?version_major\\]?, *\\[?([0-9]+)\\]?\\)"
        "\\1"
        PROJECT_MAJOR_VERSION
        ${PROJECT_MAJOR_VERSION_TMP}
        )
    string(REGEX REPLACE "m4_define\\(\\[?version_minor\\]?, *\\[?([0-9]+)\\]?\\)"
        "\\1"
        PROJECT_MINOR_VERSION
        ${PROJECT_MINOR_VERSION_TMP}
        )
    string(REGEX REPLACE "m4_define\\(\\[?version_micro\\]?, *\\[?([0-9]+)\\]?\\)"
        "\\1"
        PROJECT_MICRO_VERSION
        ${PROJECT_MICRO_VERSION_TMP}
        )
endif()

set(PROJECT_VERSION ${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_MICRO_VERSION})

message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "To change de version modify the file configure.ac")

###############################################################################
# Generation of windows installers.
###############################################################################
if(WIN32)
    option(EPROSIMA_INSTALLER "Activate the creation of a build to create windows installers" OFF)
endif()

###############################################################################
# Warning level
###############################################################################
if(MSVC OR MSVC_IDE)
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS] /W4")
    endif()
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")
endif()

###############################################################################
# Load external eProsima projects.
###############################################################################

option(EPROSIMA_BUILD "Activate internal building" OFF)

if(EPROSIMA_INSTALLER)
    set(EPROSIMA_BUILD ON)
endif()

###############################################################################
# Test system configuration
###############################################################################
include(${PROJECT_SOURCE_DIR}/cmake/dev/check_configuration.cmake)
check_stdcxx()
check_endianness()

###############################################################################
# Check MSVC architecture
###############################################################################
if(MSVC OR MSVC_IDE)
    check_msvc_arch()
endif()

###############################################################################
# Installation paths
###############################################################################
set(INCLUDE_INSTALL_DIR include/ CACHE PATH "Installation directory for C++ headers")
if((MSVC OR MSVC_IDE) AND EPROSIMA_BUILD)
    set(LIB_INSTALL_DIR lib/${MSVC_ARCH}/ CACHE PATH "Installation directory for libraries")
else()
    set(LIB_INSTALL_DIR lib/ CACHE PATH "Installation directory for libraries")
endif()
set(DATA_INSTALL_DIR share/ CACHE PATH "Installation directory for data")
if(WIN32)
    set(LICENSE_INSTALL_DIR . CACHE PATH "Installation directory for licenses")
else()
    set(LICENSE_INSTALL_DIR ${DATA_INSTALL_DIR}/${PROJECT_NAME} CACHE PATH "Installation directory for licenses")
endif()

###############################################################################
# Compile library.
###############################################################################
add_subdirectory(src/cpp)

###############################################################################
# Testing
###############################################################################
find_package(GTest)
if(GTEST_FOUND)
    enable_testing()
    include(CTest)
    add_subdirectory(test)
endif()

###############################################################################
# Packaging
###############################################################################
# Install licenses
install(FILES ${PROJECT_SOURCE_DIR}/FASTCDR_LIBRARY_LICENSE.txt
    ${PROJECT_SOURCE_DIR}/LGPLv3_LICENSE.txt
    DESTINATION ${LICENSE_INSTALL_DIR}
    COMPONENT licenses
    )
set(CPACK_COMPONENT_LICENSES_HIDDEN 1)

set(CPACK_COMPONENTS_ALL headers licenses)

if(MSVC OR MSVC_IDE)
    if(EPROSIMA_INSTALLER)
        set(CPACK_COMPONENTS_ALL ${CPACK_COMPONENTS_ALL}
            libraries_i86Win32VS2010
            libraries_x64Win64VS2010
            libraries_i86Win32VS2013
            libraries_x64Win64VS2013
            )
    else()
        set(CPACK_COMPONENTS_ALL ${CPACK_COMPONENTS_ALL} libraries_${MSVC_ARCH})
    endif()
else()
    set(CPACK_COMPONENTS_ALL ${CPACK_COMPONENTS_ALL} libraries)
endif()

include(${PROJECT_SOURCE_DIR}/cmake/packaging/eProsimaPackaging.cmake)
