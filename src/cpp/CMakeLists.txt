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
)

# Set version
set(${PROJECT_NAME_UPPER}_VERSION_MAJOR ${PROJECT_MAJOR_VERSION})
set(${PROJECT_NAME_UPPER}_VERSION_MINOR ${PROJECT_MINOR_VERSION})
set(${PROJECT_NAME_UPPER}_VERSION_MICRO ${PROJECT_MICRO_VERSION})

configure_file(${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}/config.h.in
    ${PROJECT_BINARY_DIR}/include/${PROJECT_NAME}/config.h)

# Definitions
# If Windows, set define for export dll functions
add_definitions(-D${PROJECT_NAME_UPPER}_SOURCE)

# Create libraries
if(MSVC OR MSVC_IDE)
    if(EPROSIMA_INSTALLER)
        generate_msvc_libraries(i86Win32VS2010)
        generate_msvc_libraries(x64Win64VS2010)
        generate_msvc_libraries(i86Win32VS2013)
        generate_msvc_libraries(x64Win64VS2013)
    else()
        add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_SOURCES})
        set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME}-${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION})
        set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_OUTPUT_NAME ${PROJECT_NAME}d-${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION})

        add_library(${PROJECT_NAME}_static STATIC ${${PROJECT_NAME}_SOURCES})
        # Rename the library to have a "lib" before.
        set_target_properties(${PROJECT_NAME}_static PROPERTIES OUTPUT_NAME lib${PROJECT_NAME}-${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION})
        set_target_properties(${PROJECT_NAME}_static PROPERTIES DEBUG_OUTPUT_NAME lib${PROJECT_NAME}d-${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION})

        set_target_properties(${PROJECT_NAME} ${PROJECT_NAME}_static PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG
            ${PROJECT_BINARY_DIR}/lib)
        set_target_properties(${PROJECT_NAME} ${PROJECT_NAME}_static PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG
            ${PROJECT_BINARY_DIR}/lib)
        set_target_properties(${PROJECT_NAME} ${PROJECT_NAME}_static PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE
            ${PROJECT_BINARY_DIR}/lib)
        set_target_properties(${PROJECT_NAME} ${PROJECT_NAME}_static PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE
            ${PROJECT_BINARY_DIR}/lib)

        # Export symbols in DLL libraries
        target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME_UPPER}_DYN_LINK)

        # Define public headers
        target_include_directories(${PROJECT_NAME} PUBLIC
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include> $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include/${PROJECT_NAME}>
            $<INSTALL_INTERFACE:include>
            )

        # Define public headers for static library
        target_include_directories(${PROJECT_NAME}_static PUBLIC
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include> $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include/${PROJECT_NAME}>
            $<INSTALL_INTERFACE:include>
            )
    endif()
else()
    #Create library
    add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_SOURCES})

    # Set the output directory for the library.
    set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/lib)

    # Define public headers
    target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include> $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include/${PROJECT_NAME}>
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
install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/cpp
    DESTINATION src
    COMPONENT sources
    FILES_MATCHING
    PATTERN "*.c"
    PATTERN "*.cpp"
    PATTERN "*.h"
    PATTERN "*.hpp"
    )
if(UNIX)
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
    else()
        install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}_static
            EXPORT ${PROJECT_NAME}Targets
            RUNTIME DESTINATION ${LIB_INSTALL_DIR}
            LIBRARY DESTINATION ${LIB_INSTALL_DIR}
            ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
            COMPONENT libraries_${MSVC_ARCH}
            )

        export(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}_static FILE ${PROJECT_BINARY_DIR}/cmake/config/${PROJECT_NAME}Targets.cmake)
        install(EXPORT ${PROJECT_NAME}Targets
            DESTINATION ${LIB_INSTALL_DIR}/${PROJECT_NAME}/cmake
            COMPONENT cmake
            )

        # Install pdb files
        install(DIRECTORY ${PROJECT_BINARY_DIR}/lib/
            DESTINATION ${LIB_INSTALL_DIR}
            COMPONENT libraries_${MSVC_ARCH}
            FILES_MATCHING
            PATTERN "*.pdb"
            )

        string(TOUPPER "${MSVC_ARCH}" MSVC_ARCH_UPPER)
        set(CPACK_COMPONENT_LIBRARIES_${MSVC_ARCH_UPPER}_DISPLAY_NAME "${MSVC_ARCH}" PARENT_SCOPE)
        set(CPACK_COMPONENT_LIBRARIES_${MSVC_ARCH_UPPER}_DESCRIPTION "eProsima ${PROJECT_NAME_LARGE} libraries for platform ${MSVC_ARCH}" PARENT_SCOPE)
        set(CPACK_COMPONENT_LIBRARIES_${MSVC_ARCH_UPPER}_GROUP "Libraries" PARENT_SCOPE)
    endif()

    set(CPACK_COMPONENT_GROUP_LIBRARIES_DESCRIPTION "eProsima ${PROJECT_NAME_LARGE} libraries" PARENT_SCOPE)
else()
    install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
        RUNTIME DESTINATION ${LIB_INSTALL_DIR}
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
