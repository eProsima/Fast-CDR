include(GoogleTest)

function(gtest_discover_tests TARGET)
    if (WIN32)
        add_custom_command(
            TARGET ${TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "set(_target_runtime_dlls $<TARGET_RUNTIME_DLLS:${TARGET}>)" > "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_runtime_dlls_$<CONFIG>.cmake"
            COMMAND ${CMAKE_COMMAND} -DTARGET=${TARGET} -DCONFIG=$<CONFIG> -DFILE_TO_PROCESS="${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_runtime_dlls_$<CONFIG>.cmake" -P ${CMAKE_SOURCE_DIR}/cmake/testing/generate_google_test_win_wrapper.cmake
            COMMAND_EXPAND_LISTS
            )

        set(CMAKE_COMMAND "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_win_wrapper_$<CONFIG>.bat") 
    endif()
    _gtest_discover_tests(${TARGET} ${ARGN})
endfunction()
