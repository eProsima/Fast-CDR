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

###############################################################################
# ResizeTest
###############################################################################
add_executable(ResizeTest ResizeTest.cpp)
add_test(ResizeTest ResizeTest)
target_link_libraries(ResizeTest fastcdr)
