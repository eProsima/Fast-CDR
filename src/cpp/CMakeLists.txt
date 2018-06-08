# Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

include(${PROJECT_SOURCE_DIR}/cmake/dev/set_sources.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/dev/generate_msvc_libraries.cmake)

# Set source files
set_sources(
    Cdr.cpp
    FastCdr.cpp
    FastBuffer.cpp
    exceptions/Exception.cpp
    exceptions/NotEnoughMemoryException.cpp
    exceptions/BadParamException.cpp
    FastCdr.rc
)

configure_file(${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}/config.h.in
    ${PROJECT_BINARY_DIR}/include/${PROJECT_NAME}/config.h)

# Definitions
# If Windows, set define for export dll functions
add_definitions(-D${PROJECT_NAME_UPPER}_SOURCE)

if(APPLE)
    set(CMAKE_MACOSX_RPATH ON)
    set(CMAKE_SKIP_BUILD_RPATH FALSE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)
endif()

# Create libraries
if(MSVC OR MSVC_IDE)
    if(EPROSIMA_INSTALLER)
        generate_msvc_libraries(i86Win32VS2010)
        generate_msvc_libraries(x64Win64VS2010)
        generate_msvc_libraries(i86Win32VS2013)
        generate_msvc_libraries(x64Win64VS2013)
        generate_msvc_libraries(i86Win32VS2015)
        generate_msvc_libraries(x64Win64VS2015)
    else()
        add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_SOURCES})

        add_library(${PROJECT_NAME}_static STATIC ${${PROJECT_NAME}_SOURCES})
        # Rename the library to have a "lib" before.
        set_target_properties(${PROJECT_NAME}_static PROPERTIES OUTPUT_NAME lib${PROJECT_NAME})

        set_target_properties(${PROJECT_NAME} ${PROJECT_NAME}_static PROPERTIES RELEASE_POSTFIX -${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})
        set_target_properties(${PROJECT_NAME} ${PROJECT_NAME}_static PROPERTIES RELWITHDEBINFO_POSTFIX -${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})
        set_target_properties(${PROJECT_NAME} ${PROJECT_NAME}_static PROPERTIES DEBUG_POSTFIX d-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})

        # Export symbols in DLL libraries
        target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_NAME_UPPER}_DYN_LINK)

        # Define public headers
        target_include_directories(${PROJECT_NAME} PUBLIC
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include> $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
            $<INSTALL_INTERFACE:include>
            )

        # Define public headers for static library
        target_include_directories(${PROJECT_NAME}_static PUBLIC
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include> $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
            $<INSTALL_INTERFACE:include>
            )
    endif()
else()
    #Create library
    add_library(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES})
    set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
    set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION})

    # Define public headers
    target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include> $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>
        )
endif()

###############################################################################
# Packaging
###############################################################################

# Install public headers
set_public_headers_directory(${PROJECT_SOURCE_DIR}/include ${PROJECT_NAME}
    DESTINATION ${INCLUDE_INSTALL_DIR}
    COMPONENT headers
    )

# Install config.h header
set_public_header(${PROJECT_BINARY_DIR}/include ${PROJECT_NAME} config.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/${PROJECT_NAME}
    COMPONENT headers
    )

set(CPACK_COMPONENT_HEADERS_DISPLAY_NAME "C++ Headers" PARENT_SCOPE)
set(CPACK_COMPONENT_HEADERS_DESCRIPTION "eProsima ${PROJECT_NAME_LARGE} C++ Headers" PARENT_SCOPE)

# Install sources
if(UNIX AND EPROSIMA_INSTALLER)
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/cpp
        DESTINATION src
        COMPONENT sources
        FILES_MATCHING
        PATTERN "*.c"
        PATTERN "*.cpp"
        PATTERN "*.h"
        PATTERN "*.hpp"
        )

    install(DIRECTORY ${PROJECT_BINARY_DIR}/autotools/
        DESTINATION .
        USE_SOURCE_PERMISSIONS
        OPTIONAL
        COMPONENT sources
        )
endif()

# Install libraries
if(MSVC OR MSVC_IDE)
    if(EPROSIMA_INSTALLER)
        install_msvc_libraries(i86Win32VS2010)
        install_msvc_libraries(x64Win64VS2010)
        install_msvc_libraries(i86Win32VS2013)
        install_msvc_libraries(x64Win64VS2013)
        install_msvc_libraries(i86Win32VS2015)
        install_msvc_libraries(x64Win64VS2015)
    else()
        set(DIR_EXTENSION "")
        if(EPROSIMA_INSTALLER_MINION)
            set(DIR_EXTENSION "/${MSVC_ARCH}")
        endif()

        install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}_static
            EXPORT ${PROJECT_NAME}Targets
            RUNTIME DESTINATION ${BIN_INSTALL_DIR}${DIR_EXTENSION}
            LIBRARY DESTINATION ${LIB_INSTALL_DIR}${DIR_EXTENSION}
            ARCHIVE DESTINATION ${LIB_INSTALL_DIR}${DIR_EXTENSION}
            COMPONENT libraries_${MSVC_ARCH}
            )

        export(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}_static FILE ${PROJECT_BINARY_DIR}/cmake/config/${PROJECT_NAME}Targets.cmake)
        install(EXPORT ${PROJECT_NAME}Targets
            DESTINATION ${LIB_INSTALL_DIR}${DIR_EXTENSION}/${PROJECT_NAME}/cmake
            COMPONENT cmake
            )

        # Install pdb files
        install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Debug/
            DESTINATION ${LIB_INSTALL_DIR}${DIR_EXTENSION}
            COMPONENT libraries_${MSVC_ARCH}
            CONFIGURATIONS Debug
            FILES_MATCHING
            PATTERN "*.pdb"
            )

        if(NOT (("${MSVC_ARCH}" STREQUAL "i86Win32VS2013") OR ("${MSVC_ARCH}" STREQUAL "x64Win64VS2013")))
            install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_static.dir/Debug/${PROJECT_NAME}_static.pdb
                DESTINATION ${LIB_INSTALL_DIR}${DIR_EXTENSION}
                COMPONENT libraries_${MSVC_ARCH}
                CONFIGURATIONS Debug
                )
        endif()

        string(TOUPPER "${MSVC_ARCH}" MSVC_ARCH_UPPER)
        set(CPACK_COMPONENT_LIBRARIES_${MSVC_ARCH_UPPER}_DISPLAY_NAME "${MSVC_ARCH}" PARENT_SCOPE)
        set(CPACK_COMPONENT_LIBRARIES_${MSVC_ARCH_UPPER}_DESCRIPTION "eProsima ${PROJECT_NAME_LARGE} libraries for platform ${MSVC_ARCH}" PARENT_SCOPE)
        set(CPACK_COMPONENT_LIBRARIES_${MSVC_ARCH_UPPER}_GROUP "Libraries" PARENT_SCOPE)
    endif()

    set(CPACK_COMPONENT_GROUP_LIBRARIES_DESCRIPTION "eProsima ${PROJECT_NAME_LARGE} libraries" PARENT_SCOPE)
else()
    install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
        RUNTIME DESTINATION ${BIN_INSTALL_DIR}
        LIBRARY DESTINATION ${LIB_INSTALL_DIR}
        ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
        COMPONENT libraries
        )

    export(TARGETS ${PROJECT_NAME} FILE ${PROJECT_BINARY_DIR}/cmake/config/${PROJECT_NAME}Targets.cmake)
    install(EXPORT ${PROJECT_NAME}Targets
        DESTINATION ${LIB_INSTALL_DIR}/${PROJECT_NAME}/cmake
        COMPONENT cmake
        )

    set(CPACK_COMPONENT_LIBRARIES_DISPLAY_NAME "Libraries" PARENT_SCOPE)
    set(CPACK_COMPONENT_LIBRARIES_DESCRIPTION "eProsima ${PROJECT_NAME_LARGE} libraries" PARENT_SCOPE)
endif()
