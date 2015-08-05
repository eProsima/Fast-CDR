if(NOT ((MSVC OR MSVC_IDE) AND EPROSIMA_INSTALLER))
    include(${PROJECT_SOURCE_DIR}/cmake/dev/gtest.cmake)

    # If Windows, link against export dll functions
    if(WIN32)
        add_definitions(-DFASTCDR_DYN_LINK)
    endif()


    ###############################################################################
    # Unit tests
    ###############################################################################
    set(UNITTESTS_SOURCE SimpleTest.cpp ResizeTest.cpp)
    add_executable(UnitTests ${UNITTESTS_SOURCE})
    add_gtest(UnitTests ${UNITTESTS_SOURCE})
    target_link_libraries(UnitTests fastcdr ${GTEST_BOTH_LIBRARIES})

    # Add environment
    if(WIN32)
        set_tests_properties(UnitTests PROPERTIES ENVIRONMENT
            "PATH=${PROJECT_BINARY_DIR}/lib\\;$ENV{PATH}")
    endif()
endif()
