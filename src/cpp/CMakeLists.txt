# Set source files
set(FASTCDR_SOURCES
    Cdr.cpp
    FastCdr.cpp
    FastBuffer.cpp
    exceptions/Exception.cpp
    exceptions/NotEnoughMemoryException.cpp
    exceptions/BadParamException.cpp
)

# Check C++11
include(CheckCXXCompilerFlag)
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)
    check_cxx_compiler_flag(--std=c++11 SUPPORTS_CXX11)
    if(SUPPORTS_CXX11)
        add_compile_options(--std=c++11)
        set(FASTCDR_SUPPORTS_CXX11 1)
        set(FASTCDR_SUPPORTS_CXX01 1)
    else()
        check_cxx_compiler_flag(--std=c++0x SUPPORTS_CXX01)
        if(SUPPORTS_CXX01)
            add_compile_options(--std=c++0x)
            set(FASTCDR_SUPPORTS_CXX01 1)
        endif()
    endif()
endif()

# Set version
set(VERSION_MAJOR ${PROJECT_MAJOR_VERSION})
set(VERSION_MINOR ${PROJECT_MINOR_VERSION})
set(VERSION_MICRO ${PROJECT_MICRO_VERSION})

# Test endianness
include(TestBigEndian)
test_big_endian(BIG_ENDIAN)
set(__BIG_ENDIAN__ ${BIG_ENDIAN})
set(__LITTLE_ENDIAN__ !${BIG_ENDIAN})

configure_file(config.h.cmake config.h)

# Definitions
# If Windows, set define for export dll functions
add_definitions(-DFASTCDR_SOURCE)
if(WIN32)
    add_definitions(-DFASTCDR_DYN_LINK)
endif()

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Create library
add_library(fastcdr SHARED ${FASTCDR_SOURCES})

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
    add_library(fastcdr_static STATIC ${SFC_SOURCES})

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

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION src
    COMPONENT fastcdr_source
    FILES_MATCHING
    PATTERN "*.cpp"
    PATTERN "*.h"
    )

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/fastcdr
    DESTINATION include
    COMPONENT fastcdr_headers
    FILES_MATCHING PATTERN "*.h"
    )
