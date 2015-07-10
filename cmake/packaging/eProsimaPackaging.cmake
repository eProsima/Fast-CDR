# Set common CPACK variables.

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${CPACK_PACKAGE_NAME} - C++ library for serialize using CDR serialization")

set(CPACK_PACKAGE_DESCRIPTIONY "${PROJECT_NAME_LARGE} library provides two serialization mechanisms. One is the standard CDR serialization mechanism, while the other is a faster implementation of it.")

set(CPACK_PACKAGE_VENDOR http://www.eprosima.com)
set(CPACK_PACKAGE_CONTACT "eProsima Support <support@eprosima.com>")

set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_MAJOR_VERSION})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_MINOR_VERSION})
set(CPACK_PACKAGE_VERSION_PATH ${PROJECT_MICRO_VERSION})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})

#TODO Falta

if(WIN32)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set(CPACK_GENERATOR TGZ)
    set(CPACK_SOURCE_GENERATOR TGZ)

    # Prepare specific cmake scripts
    configure_file(${PROJECT_SOURCE_DIR}/cmake/packaging/linux/LinuxPackaging.cmake.in ${PROJECT_BINARY_DIR}/cmake/packaging/linux/LinuxPackaging.cmake @ONLY)
    configure_file(${PROJECT_SOURCE_DIR}/cmake/packaging/linux/AutotoolsPackaging.cmake.in ${PROJECT_BINARY_DIR}/cmake/packaging/linux/AutotoolsPackaging.cmake @ONLY)

    # Prepare scripts for autotools
    include(${PROJECT_SOURCE_DIR}/cmake/packaging/linux/autotools_generator_utility.cmake)
    generate_autotools_generator_script()

    # Ignore not source files
    set(CPACK_SOURCE_IGNORE_FILES "~$" ".txt$" ".swp$")
    # Define source directories
    set(CPACK_SOURCE_INSTALLED_DIRECTORIES "${PROJECT_SOURCE_DIR}/src/cpp;src/cpp;${PROJECT_SOURCE_DIR}/include/fastcdr;include/fastcdr")

    # Set cpack project config file.
    set(CPACK_PROJECT_CONFIG_FILE ${PROJECT_BINARY_DIR}/cmake/packaging/linux/LinuxPackaging.cmake)
endif()

include(CPack)
