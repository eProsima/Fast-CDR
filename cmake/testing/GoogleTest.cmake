include(GoogleTest)

function(gtest_discover_tests TARGET)
    if (WIN32)
        add_custom_command(
            TARGET ${TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -DTARGET=${TARGET} -DCONFIG=$<CONFIG> -DRUNTIME_LIST=$<TARGET_RUNTIME_DLLS:${TARGET}> -P ${CMAKE_SOURCE_DIR}/cmake/testing/generate_google_test_win_wrapper.cmake
            COMMAND_EXPAND_LISTS
            VERBATIM
            )

        set(CMAKE_COMMAND "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_win_wrapper_$<CONFIG>.bat") 
    endif()
    _gtest_discover_tests(${TARGET} ${ARGN})
endfunction()
