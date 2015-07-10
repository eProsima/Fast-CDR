###############################################################################
# CMake build rules for FastCDR                                               #
###############################################################################
cmake_minimum_required(VERSION 2.8)

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
set(PROJECT_NAME_LARGE "eProsima Fast CDR")

message(STATUS "Configuring ${PROJECT_NAME_LAGE}")

###############################################################################
# Version information                                                         #
###############################################################################

#If not set, copy over FastCDR version from configure.ac
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
# Test system configuration
###############################################################################
include(${PROJECT_SOURCE_DIR}/cmake/dev/check_configuration.cmake)
check_stdcxx()
check_endianness()

###############################################################################
# Compile FastCDR library.
###############################################################################
add_subdirectory(src/cpp)

###############################################################################
# Packaging
###############################################################################
include(${PROJECT_SOURCE_DIR}/cmake/packaging/eProsimaPackaging.cmake)

#set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/packaging)
# For Linux generate TGZ with sourcecode.
#set(CPACK_GENERATOR TGZ)
#set(CPACK_PACKAGE_FILE_NAME eProsima_FastCDR-${PROJECT_VERSION}-Linux)
#set(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE 1)
#set(CPACK_COMPONENTS_ALL fastcdr_source fastcdr_headers)
#set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
#configure_file(${PROJECT_SOURCE_DIR}/cmake/packaging/linux/SourcePackage.cmake.in ${PROJECT_BINARY_DIR}/cmake/packaging/linux/SourcePackage.cmake @ONLY)
#set(CPACK_SOURCE_INSTALL_SCRIPT ${PROJECT_BINARY_DIR}/cmake/packaging/linux/SourcePackage.cmake)
#set(CPACK_SOURCE_GENERATOR TGZ)
#set(CPACK_PROJECT_CONFIG_FILE ${CMAKE_SOURCE_DIR}/cmake/packaging/linux/SourcePackage.cmake)

#set(CPACK_GENERATOR RPM)
#set(CPACK_PROJECT_CONFIG_FILE ${CMAKE_SOURCE_DIR}/cmake/packaging/linux/RPMPackage.cmake)

#include(CPack)
