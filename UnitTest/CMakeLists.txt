file(GLOB_RECURSE unit_test_src_c *.c)
file(GLOB_RECURSE unit_test_src_cpp *.cpp)

add_executable(UNIT_TEST main.cpp ${unit_test_src_c} ${unit_test_src_cpp})


target_compile_definitions(BEG_OS PUBLIC MEM_DEBUG=1)
target_compile_definitions(BEG_OS PUBLIC MESSAGE_DEBUG=1)
target_compile_definitions(BEG_OS PUBLIC TASK_DEBUG=1)
target_compile_definitions(BEG_OS PUBLIC QUEUE_DEBUG=1)
target_compile_definitions(BEG_OS PUBLIC PROCESS_DEBUG=1)

target_compile_definitions(BEG_OS PUBLIC UNIT_TEST=1)
target_compile_definitions(BEG_OS PUBLIC FAST_MEM=1)
target_compile_definitions(BEG_OS PUBLIC FAST_MEMB=1)
target_compile_definitions(BEG_OS PUBLIC MEM_HEAP=200)
target_compile_definitions(BEG_OS PUBLIC MEM_ALIGMENT_4BYTES=0)
target_compile_definitions(BEG_OS PUBLIC SYS_TICK_SUPPORT_CALLBACK_LIST=1)
target_compile_definitions(BEG_OS PUBLIC OS_ITEM_ALLOCATE_FROM_GLOBAL_MEM=0)


target_include_directories(UNIT_TEST PUBLIC ./TestMock)
target_include_directories(UNIT_TEST PUBLIC ./TestSrc)
target_include_directories(UNIT_TEST PUBLIC ./TestStub)


target_link_libraries(UNIT_TEST PUBLIC BEG_OS)

set(LIB_DIR ${CMAKE_SOURCE_DIR}/UnitTest)

target_include_directories(UNIT_TEST PUBLIC ${LIB_DIR}/CppUTest)
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if (MSVC)
        target_link_directories(UNIT_TEST PUBLIC ${LIB_DIR}/CppUTest/MSVC)
        target_link_libraries(UNIT_TEST PUBLIC CppUTestd)
    endif ()
endif ()


if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_directories(UNIT_TEST PUBLIC ${LIB_DIR}/CppUTest/GCC)
    target_link_libraries(UNIT_TEST PUBLIC libCppUTest.a libCppUTestExt.a)
endif ()


add_custom_command(TARGET UNIT_TEST POST_BUILD COMMAND UNIT_TEST)






