if (NOT DEFINED TARGET)
    message(FATAL_ERROR "This scrips needs TARGET variable set")
endif()
if (NOT DEFINED CONFIG)
    message(FATAL_ERROR "This scrips needs CONFIG variable set")
endif()
if (NOT DEFINED FILE_TO_PROCESS)
    message(FATAL_ERROR "This scrips needs FILE_TO_PROCESS variable set")
endif()

include(${FILE_TO_PROCESS})

set(_path "")
foreach(_runtime_dll ${_target_runtime_dlls})
    cmake_path(GET _runtime_dll PARENT_PATH _runtime_dll_path)
    cmake_path(NATIVE_PATH _runtime_dll_path _runtime_dll_path_native)
    list(APPEND _path "${_runtime_dll_path_native}")
endforeach()

list(REMOVE_DUPLICATES _path)

cmake_path(NATIVE_PATH CMAKE_COMMAND _cmake_command)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_win_wrapper_${CONFIG}.bat" "
@ECHO OFF
set \"PATH=${_path};%PATH%\"
\"${_cmake_command}\" %*
")

