include(${PROJECT_SOURCE_DIR}/cmake/dev/set_sources.cmake)

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
set(VERSION_MAJOR ${PROJECT_MAJOR_VERSION})
set(VERSION_MINOR ${PROJECT_MINOR_VERSION})
set(VERSION_MICRO ${PROJECT_MICRO_VERSION})

configure_file(${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}/config.h.in
    ${PROJECT_BINARY_DIR}/include/${PROJECT_NAME}/config.h)

# Definitions
# If Windows, set define for export dll functions
add_definitions(-DFASTCDR_SOURCE)
if(WIN32)
    add_definitions(-DFASTCDR_DYN_LINK)
endif()

# Include public api
include_directories(${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}
    ${PROJECT_BINARY_DIR}/include/${PROJECT_NAME}
    )

# Create libraries
if(MSVC OR MSVC_IDE)
    add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_SOURCES})
    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME}-${PROJECT_VERSION})
    set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_OUTPUT_NAME ${PROJECT_NAME}d-${PROJECT_VERSION})

    add_library(${PROJECT_NAME}_static STATIC ${${PROJECT_NAME}_SOURCES})
    # Rename the library to have a "lib" before.
    set_target_properties(${PROJECT_NAME}_static PROPERTIES OUTPUT_NAME lib${PROJECT_NAME}-${PROJECT_VERSION})
    set_target_properties(${PROJECT_NAME}_static PROPERTIES DEBUG_OUTPUT_NAME lib${PROJECT_NAME}d-${PROJECT_VERSION})

    set_target_properties(${PROJECT_NAME} ${PROJECT_NAME}_static PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG
        ${PROJECT_BINARY_DIR}/lib)
    set_target_properties(${PROJECT_NAME} ${PROJECT_NAME}_static PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG
        ${PROJECT_BINARY_DIR}/lib)
    set_target_properties(${PROJECT_NAME} ${PROJECT_NAME}_static PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE
        ${PROJECT_BINARY_DIR}/lib)
    set_target_properties(${PROJECT_NAME} ${PROJECT_NAME}_static PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE
        ${PROJECT_BINARY_DIR}/lib)

    # Define public headers for static library
    target_include_directories(${PROJECT_NAME}_static PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_BINARY_DIR}/include)
else()
    #Create library
    add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_SOURCES})

    # Set the output directory for the library.
    set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/lib)
endif()

# Define public headers
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include> $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

###############################################################################
# Packaging
###############################################################################

# Install public headers
set_public_headers_directory(${PROJECT_SOURCE_DIR}/include ${PROJECT_NAME}
    DESTINATION ${INCLUDE_INSTALL_DIR}
    COMPONENT headers
    )

# Install config.h header
install(FILES ${PROJECT_BINARY_DIR}/include/${PROJECT_NAME}/config.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/${PROJECT_NAME}
    COMPONENT headers
    )

set(CPACK_COMPONENT_HEADERS_DISPLAY_NAME "C++ Headers" PARENT_SCOPE)
set(CPACK_COMPONENT_HEADERS_DESCRIPTION "eProsima ${PROJECT_NAME_LARGE} C++ Headers" PARENT_SCOPE)

# Install libraries
if(MSVC OR MSVC_IDE)
    if(EPROSIMA_INSTALLER)
        ## i86Win32VS2010
        install(DIRECTORY ${PROJECT_BINARY_DIR}/../i86Win32VS2010/lib/
            DESTINATION ${LIB_INSTALL_DIR}/i86Win32VS2010
            COMPONENT libraries_i86Win32VS2010
            FILES_MATCHING
            PATTERN "*${PROJECT_NAME}*-${PROJECT_VERSION}*"
            )
        set(CPACK_COMPONENT_LIBRARIES_I86WIN32VS2010_DISPLAY_NAME "i86Win32VS2010" PARENT_SCOPE)
        set(CPACK_COMPONENT_LIBRARIES_I86WIN32VS2010_DESCRIPTION "eProsima ${PROJECT_NAME_LARGE} libraries for platform i86Win32VS2010" PARENT_SCOPE)
        set(CPACK_COMPONENT_LIBRARIES_I86WIN32VS2010_GROUP "Libraries" PARENT_SCOPE)

        # x64Win64VS2010
        install(DIRECTORY ${PROJECT_BINARY_DIR}/../x64Win64VS2010/lib/
            DESTINATION ${LIB_INSTALL_DIR}/x64Win64VS2010
            COMPONENT libraries_x64Win64VS2010
            FILES_MATCHING
            PATTERN "*${PROJECT_NAME}*-${PROJECT_VERSION}*"
            )
        set(CPACK_COMPONENT_LIBRARIES_X64WIN64VS2010_DISPLAY_NAME "x64Win64VS2010" PARENT_SCOPE)
        set(CPACK_COMPONENT_LIBRARIES_X64WIN64VS2010_DESCRIPTION "eProsima ${PROJECT_NAME_LARGE} libraries for platform x64Win64VS2010" PARENT_SCOPE)
        set(CPACK_COMPONENT_LIBRARIES_X64WIN64VS2010_GROUP "Libraries" PARENT_SCOPE)

        # i86Win32VS2013
        install(DIRECTORY ${PROJECT_BINARY_DIR}/../i86Win32VS2013/lib/
            DESTINATION ${LIB_INSTALL_DIR}/i86Win32VS2013
            COMPONENT libraries_i86Win32VS2013
            FILES_MATCHING
            PATTERN "*${PROJECT_NAME}*-${PROJECT_VERSION}*"
            )
        set(CPACK_COMPONENT_LIBRARIES_I86WIN32VS2013_DISPLAY_NAME "i86Win32VS2013" PARENT_SCOPE)
        set(CPACK_COMPONENT_LIBRARIES_I86WIN32VS2013_DESCRIPTION "eProsima ${PROJECT_NAME_LARGE} libraries for platform i86Win32VS2013" PARENT_SCOPE)
        set(CPACK_COMPONENT_LIBRARIES_I86WIN32VS2013_GROUP "Libraries" PARENT_SCOPE)

        # x64Win64VS2013
        install(DIRECTORY ${PROJECT_BINARY_DIR}/../x64Win64VS2013/lib/
            DESTINATION ${LIB_INSTALL_DIR}/x64Win64VS2013
            COMPONENT libraries_x64Win64VS2013
            FILES_MATCHING
            PATTERN "*${PROJECT_NAME}*-${PROJECT_VERSION}*"
            )
        set(CPACK_COMPONENT_LIBRARIES_X64WIN64VS2013_DISPLAY_NAME "x64Win64VS2013" PARENT_SCOPE)
        set(CPACK_COMPONENT_LIBRARIES_X64WIN64VS2013_DESCRIPTION "eProsima ${PROJECT_NAME_LARGE} libraries for platform x64Win64VS2013" PARENT_SCOPE)
        set(CPACK_COMPONENT_LIBRARIES_X64WIN64VS2013_GROUP "Libraries" PARENT_SCOPE)
    else()
        install(TARGETS ${PROJECT_NAME}
            EXPORT ${PROJECT_NAME}Targets
            RUNTIME DESTINATION ${LIB_INSTALL_DIR}/${MSVC_ARCH}
            LIBRARY DESTINATION ${LIB_INSTALL_DIR}/${MSVC_ARCH}
            ARCHIVE DESTINATION ${LIB_INSTALL_DIR}/${MSVC_ARCH}
            COMPONENT libraries_${MSVC_ARCH}
            )

        install(TARGETS ${PROJECT_NAME}_static
            EXPORT ${PROJECT_NAME}Targets
            RUNTIME DESTINATION ${LIB_INSTALL_DIR}/${MSVC_ARCH}
            LIBRARY DESTINATION ${LIB_INSTALL_DIR}/${MSVC_ARCH}
            ARCHIVE DESTINATION ${LIB_INSTALL_DIR}/${MSVC_ARCH}
            COMPONENT libraries_${MSVC_ARCH}
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

    set(CPACK_COMPONENT_LIBRARIES_DISPLAY_NAME "Libraries" PARENT_SCOPE)
    set(CPACK_COMPONENT_LIBRARIES_DESCRIPTION "eProsima ${PROJECT_NAME_LARGE} libraries" PARENT_SCOPE)
endif()

# Install licenses
install(FILES ${PROJECT_SOURCE_DIR}/FASTCDR_LIBRARY_LICENSE.txt
    ${PROJECT_SOURCE_DIR}/LGPLv3_LICENSE.txt
    DESTINATION .
    )
