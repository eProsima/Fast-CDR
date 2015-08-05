option(GTEST_INDIVIDUAL "Activate the execution of GTest tests" OFF)

macro(add_gtest test)
    if(GTEST_INDIVIDUAL)
        foreach(GTEST_SOURCE_FILE ${ARGN})
            file(STRINGS ${GTEST_SOURCE_FILE} GTEST_NAMES REGEX ^TEST)
            foreach(GTEST_NAME ${GTEST_NAMES})
                string(REGEX REPLACE ["\) \(,"] ";" GTEST_NAME ${GTEST_NAME})
                list(GET GTEST_NAME 1 GTEST_GROUP_NAME)
                list(GET GTEST_NAME 3 GTEST_NAME)
                add_test(${GTEST_GROUP_NAME}.${GTEST_NAME}
                    ${test}
                    --gtest_filter=${GTEST_GROUP_NAME}.${GTEST_NAME})
            endforeach()
        endforeach()
    else()
        add_test(${test} ${test})
    endif()
endmacro()
