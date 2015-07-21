if(NOT ((MSVC OR MSVC_IDE) AND EPROSIMA_INSTALLER))

    # If Windows, link against export dll functions
    if(WIN32)
        add_definitions(-DFASTCDR_DYN_LINK)
    endif()


    ###############################################################################
    # SimpleTest
    ###############################################################################
    add_executable(SimpleTest SimpleTest.cpp)
    add_test(SimpleTest SimpleTest)
    target_link_libraries(SimpleTest fastcdr)

    # Add environment
    if(WIN32)
        set_tests_properties(SimpleTest PROPERTIES ENVIRONMENT
            "PATH=${PROJECT_BINARY_DIR}/lib\\;$ENV{PATH}")
    endif()

    ###############################################################################
    # ResizeTest
    ###############################################################################
    add_executable(ResizeTest ResizeTest.cpp)
    add_test(ResizeTest ResizeTest)
    target_link_libraries(ResizeTest fastcdr)

    # Add environment
    if(WIN32)
        set_tests_properties(ResizeTest PROPERTIES ENVIRONMENT
            "PATH=${PROJECT_BINARY_DIR}/lib\\;$ENV{PATH}")
    endif()

endif()
