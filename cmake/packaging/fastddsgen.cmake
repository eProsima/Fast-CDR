# Run fastddsgen to create header/source from .idl
# Preserve subdirectory structure.
# Generates type naming compatible with ROS 2.
#
# fastddsgen(
#     idl_root_path         # root directory for .idl files (.idl include directory)
#     idl_relative_path     # list of subfolder containing .idl files, or list of .idl files, relative to idl_root_path
#     output_path           # Directory for the generated files
#     generated_header      # List of generated header files (absolute path)
#     generated_source      # List of generated source files (absolute path)
#     generated_include_dir # List of include paths for compiling generated_source
# )
#
# This function runs fastddsgen using the following command:
#
#     fastddsgen -typeros2 -replace \
#                -d output_path \
#                -t CMAKE_CURRENT_BINARY_DIR/tmp/fastddsgen \
#                -I idl_root_path \
#                idl_root_path/idl_relative_path
#
# Example directory of input idl files:
#
#     /home/username/myproject/foo/msg/a.idl
#     /home/username/myproject/foo/msg/b.idl
#     /home/username/myproject/bar/msg/c.idl (contains #include <foo/msg/a.idl>)
#     /home/username/myproject/bar/msg/d.idl (contains #include <foo/msg/b.idl>)
#     /home/username/myproject/CMakeLists.txt
#
# Example function usage #1:
#
#     fastddsgen(
#         ${CMAKE_CURRENT_SOURCE_DIR}
#         foo
#         ${CMAKE_CURRENT_SOURCE_DIR}/generated
#         myproject_idl_header
#         myproject_idl_source
#         myproject_idl_include_directories
# 
# Example function usage #2:
#
#     set(myproject_idl
#         foo/msg/a.idl
#         foo/msg/b.idl)
#         bar/msg/c.idl
#         bar/msg/d.idl)
#     fastddsgen(
#         ${CMAKE_CURRENT_SOURCE_DIR}
#         "${myproject_idl}"
#         ${CMAKE_CURRENT_SOURCE_DIR}/generated
#         myproject_idl_header
#         myproject_idl_source
#         myproject_idl_include_directories
# 
# Example output directory:
#
#     /home/username/myproject/build/generated/foo/msg/a.h,*.cxx
#     /home/username/myproject/build/generated/foo/msg/b.h,*.cxx
#     /home/username/myproject/build/generated/bar/msg/c.h,*.cxx
#     /home/username/myproject/build/generated/bar/msg/d.h,*.cxx
#
# NOTE: if all idl are unchanged, then fastddsgen.cmake will skip generation

function(fastddsgen 
    idl_root_path
    idl_relative_path
    output_path
    generated_header
    generated_source
    generated_include_directories
)
    # create a list of all relative idl paths
    # create a list of all absolute idl paths
    set(idl_abs_files)
    set(idl_rel_files)
    foreach(item ${idl_relative_path})
        if(IS_DIRECTORY "${idl_root_path}/${idl_relative_path}")
            # find all idl in subfolder, and append to list
            file(GLOB_RECURSE temp                           ${idl_root_path}/${item}/*.idl)
            list(APPEND idl_abs_files ${temp})
            file(GLOB_RECURSE temp RELATIVE ${idl_root_path} ${idl_root_path}/${item}/*.idl)
            list(APPEND idl_rel_files ${temp})
        else()
            # verify that file has .idl extension
            get_filename_component(ext ${item} EXT)
            if (NOT ${ext} STREQUAL ".idl")
                message(WARNING "expected subfolder or idl, but got ${item}")
                continue()
            endif()
            # append to list
            list(APPEND idl_rel_files ${item})
            list(APPEND idl_abs_files ${idl_root_path}/${item})
        endif()
    endforeach()
    # message(STATUS "idl rel paths: ${idl_rel_files}")
    # message(STATUS "idl abs paths: ${idl_abs_files}")

    # create list of all idl subfoldersand filenames
    set(idl_subfolders)
    set(idl_filenames)
    foreach(idl ${idl_rel_files})
        get_filename_component(subfolder ${idl} DIRECTORY)
        get_filename_component(filename ${idl} NAME_WE)
        list(APPEND idl_subfolders ${subfolder})
        list(APPEND idl_filenames ${filename})
    endforeach()
    # message(STATUS "idl subfolder: ${idl_subfolders}")
    # message(STATUS "idl filenames: ${idl_filenames}")

    # check if any of the generated files are missing
    set(generated_files_up_to_date TRUE)
    foreach(subfolder filename IN ZIP_LISTS idl_subfolders idl_filenames)
        if((NOT EXISTS "${output_path}/${subfolder}/${filename}.h") OR
           (NOT EXISTS "${output_path}/${subfolder}/${filename}.cxx") OR
           (NOT EXISTS "${output_path}/${subfolder}/${filename}PubSubTypes.h") OR
           (NOT EXISTS "${output_path}/${subfolder}/${filename}PubSubTypes.cxx"))
            set(generated_files_up_to_date FALSE)
            # message(STATUS "generation required. not found: ${output_path}/${subfolder}/${filename}.h,.cxx")
        endif()
    endforeach()

    # check if any of the .idl files have changed, by calculating the MD5 on all files
    set(md5_accumulated "")
    foreach(idl_file ${idl_abs_files})
        file(MD5 ${idl_file} this_md5)
        string(JOIN "" md5_accumulated ${md5_accumulated} ${this_md5})
    endforeach()
    # message("md5 of all idl       : ${md5_accumulated}")
    # message("md5 from previous run: ${last_${idl_root_path}_md5_accumulated}")

    if(NOT "${md5_accumulated}" STREQUAL "${last_${idl_root_path}_md5_accumulated}")
        set(generated_files_up_to_date FALSE)
    endif()

    if(${generated_files_up_to_date})
        # no change so skip fastddsgen
        # do not return, because we still need to set the output variables
        message(STATUS "fastddsgen skipping generation.\n"
            "   Generated files already exists in: ${output_path}\n"
            "   and no changes to: ${idl_root_path}/*.idl")
    else()
        # find fastddsgen executable
        find_program(FASTDDSGEN NAMES fastddsgen REQUIRED)
        message(STATUS "Found fastddsgen: ${FASTDDSGEN}")

        # set tmp folder, that is not in the path of the generated files.
        # fastddsgen uses /tmp by default, which can cause problems
        # when developer trigger different builds with different authority.
        set(tmp_path ${CMAKE_CURRENT_BINARY_DIR}/tmp/fastddsgen)
        execute_process(COMMAND mkdir -p ${tmp_path})

        # create output folders
        foreach(subfolder ${idl_subfolders})
            execute_process(COMMAND mkdir -p ${output_path}/${subfolder})
        endforeach()

        # run fastddsgen.
        # fastrtps must be run in idl_root_path or a parent of idl_root_path
        # to generate the proper include paths: #include "relative-path/Bar.h"
        # instead of the include path:          #include "Bar.h""
        message(STATUS "Running: fastddsgen -typeros2 -replace "
            "-d ${output_path} "
            "-t ${tmp_path} "
            "-I ${idl_root_path} "
            "${idl_root_path}/*.idl")
        execute_process(
            COMMAND ${FASTDDSGEN} -typeros2 -replace -d ${output_path} -t ${tmp_path} -I ${idl_root_path} ${idl_abs_files}
            WORKING_DIRECTORY ${idl_root_path}
            RESULT_VARIABLE fastrtps_result
            OUTPUT_VARIABLE fastrtps_output
            ERROR_VARIABLE fastrtps_error
            OUTPUT_STRIP_TRAILING_WHITESPACE)

        # Print fastddsgen output.
        # Indent the output, because it is multi-line.
        # fastrtps_error is not a real error, so print as STATUS.
        string(REPLACE "\n" "\n   " fastrtps_output ${fastrtps_output})
        message(STATUS "${fastrtps_output}")

        if(NOT ${fastrtps_error} STREQUAL "")
            string(REPLACE "\n" "\n   " fastrtps_error ${fastrtps_error})
            message(STATUS "${fastrtps_error}")
        endif()

        # check for error, and print fastddsgen output
        if(${fastrtps_result} EQUAL "0")
            message(STATUS "fastddsgen generated files to: ${output_path}")
        else()
            message(FATAL_ERROR "fastrtspgen failed with return code: ${fastrtps_result}")
        endif()

        # if generation successful, then save (cache) the MD5 of all idl files
        # hide the variable from cmake-gui
        set(last_${idl_root_path}_md5_accumulated ${md5_accumulated} CACHE STRING "do not modify" FORCE)
        mark_as_advanced(FORCE last_${idl_root_path}_md5_accumulated)
    endif()

    # for all generated files, move to subfolder
    foreach(subfolder filename IN ZIP_LISTS idl_subfolders idl_filenames)
        # move the generated .h / .cxx to output_path/subfolder/
        # message(STATUS "moving ${output_path}/${filename}* to ${output_path}/${subfolder}/${filename}*")
        if (EXISTS ${output_path}/${filename}.h)
            file(RENAME ${output_path}/${filename}.h              ${output_path}/${subfolder}/${filename}.h)
        endif()
        if (EXISTS ${output_path}/${filename}.cxx)
            file(RENAME ${output_path}/${filename}.cxx            ${output_path}/${subfolder}/${filename}.cxx)
        endif()
        if(EXISTS ${output_path}/${filename}PubSubTypes.h)
            file(RENAME ${output_path}/${filename}PubSubTypes.h   ${output_path}/${subfolder}/${filename}PubSubTypes.h)
        endif()
        if(EXISTS ${output_path}/${filename}PubSubTypes.cxx)
            file(RENAME ${output_path}/${filename}PubSubTypes.cxx ${output_path}/${subfolder}/${filename}PubSubTypes.cxx)
        endif()
    endforeach()

    # for all generated files, add to the list of generated header/source
    set(generated_header_to_return)
    set(generated_source_to_return)
    set(generated_include_dirs_to_return ${output_path})
    foreach(subfolder filename IN ZIP_LISTS idl_subfolders idl_filenames)
        # search for the files, don't assume they were generated
        file(GLOB generated_h               ${output_path}/${subfolder}/${filename}.h)
        file(GLOB generated_cxx             ${output_path}/${subfolder}/${filename}.cxx)
        file(GLOB generated_PubSubTypes_h   ${output_path}/${subfolder}/${filename}PubSubTypes.h)
        file(GLOB generated_PubSubTypes_cxx ${output_path}/${subfolder}/${filename}PubSubTypes.cxx)

        # cmake function arguments are not normal variables. For simplicity:
        # 1. create local variable with return values
        # 2. set the function arguments = local variable variables at end of function
        list(APPEND generated_header_to_return ${generated_h}   ${generated_PubSubTypes_h})
        list(APPEND generated_source_to_return ${generated_cxx} ${generated_PubSubTypes_cxx})

        list(APPEND generated_include_dirs_to_return "${output_path}/${subfolder}")
        list(REMOVE_DUPLICATES generated_include_dirs_to_return)
    endforeach()

    # mark as generated
    set_source_files_properties(${generated_header_to_return} ${generated_source_to_return} PROPERTIES GENERATED TRUE)

    # message(STATUS "generated_header: ${generated_header_to_return}")
    # message(STATUS "generated_source: ${generated_source_to_return}")
    # message(STATUS "generated_include_directories: ${generated_include_dirs_to_return}")

    # copy result to function arguments
    set(${generated_header}              ${generated_header_to_return}       PARENT_SCOPE)
    set(${generated_source}              ${generated_source_to_return}       PARENT_SCOPE)
    set(${generated_include_directories} ${generated_include_dirs_to_return} PARENT_SCOPE)
endfunction()
