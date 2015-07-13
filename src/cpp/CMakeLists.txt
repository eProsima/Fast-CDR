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

configure_file(${PROJECT_SOURCE_DIR}/include/fastcdr/config.h.in
    ${PROJECT_SOURCE_DIR}/include/fastcdr/config.h)

# Definitions
# If Windows, set define for export dll functions
add_definitions(-DFASTCDR_SOURCE)
if(WIN32)
    add_definitions(-DFASTCDR_DYN_LINK)
endif()

# Include public api
include_directories(${PROJECT_SOURCE_DIR}/include/fastcdr)

# Create library
add_library(fastcdr SHARED ${${PROJECT_NAME}_SOURCES})

# Define public headers
target_include_directories(fastcdr PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(fastcdr PUBLIC ${PROJECT_BINARY_DIR}/include)

#Rename in Windows the debug library
if(WIN32)
    set_target_properties(fastcdr PROPERTIES DEBUG_OUTPUT_NAME fastcdrd)
endif()

# Set the output directory for the library.
# The runtime output directory is for Windows.
if(UNIX)
    set_target_properties(fastcdr PROPERTIES LIBRARY_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/lib)
elseif(WIN32)
    set_target_properties(fastcdr PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG
        ${PROJECT_BINARY_DIR}/lib)
    set_target_properties(fastcdr PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG
        ${PROJECT_BINARY_DIR}/lib)
    set_target_properties(fastcdr PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE
        ${PROJECT_BINARY_DIR}/lib)
    set_target_properties(fastcdr PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE
        ${PROJECT_BINARY_DIR}/lib)
endif()

# Create the static libraries in Windows
if(WIN32)
    # Create library
    add_library(fastcdr_static STATIC ${${PROJECT_NAME}_SOURCES})

    # Define public headers
    target_include_directories(fastcdr_static PUBLIC ${PROJECT_SOURCE_DIR}/include)

    # Rename the library to have a "lib" before.
    set_target_properties(fastcdr_static PROPERTIES DEBUG_OUTPUT_NAME libfastcdrd)
    set_target_properties(fastcdr_static PROPERTIES OUTPUT_NAME libfastcdr)

    # Force /MT for static VC runtimes
    if(MSVC)
        set_target_properties(fastcdr_static PROPERTIES COMPILE_FLAGS "/MT")
    endif()

    # Set the output directory for the library.
    # The runtime output directory is for Windows.
    set_target_properties(fastcdr_static PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG
        ${PROJECT_BINARY_DIR}/lib)
    set_target_properties(fastcdr_static PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE
        ${PROJECT_BINARY_DIR}/lib)
endif()

###############################################################################
# Packaging
###############################################################################

install(TARGETS fastcdr
    LIBRARY DESTINATION lib
    COMPONENT fastcdr_libraries
    )

if(WIN32)
    install(TARGETS fastcdr_static
        LIBRARY DESTINATION lib
        COMPONENT fastcdr_libraries
        )
endif()

set_public_headers_directory(${PROJECT_SOURCE_DIR}/include
    DESTINATION .
    COMPONENT fastcdr_headers
    )
