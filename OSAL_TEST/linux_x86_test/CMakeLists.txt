find_package(Threads)

file(GLOB_RECURSE src_c *.c ../app_ipc_test.c ../app_msg_test.c)
file(GLOB_RECURSE src_cpp *.cpp)

add_executable(LINUX_X86_TEST ../main.c ${src_c} ${src_cpp})

target_compile_definitions(DING_OS PUBLIC MEM_DEBUG=1)
target_compile_definitions(DING_OS PUBLIC MESSAGE_DEBUG=1)
target_compile_definitions(DING_OS PUBLIC TASK_DEBUG=1)
target_compile_definitions(DING_OS PUBLIC MEM_ALIGMENT_4BYTES=0)
target_compile_definitions(DING_OS PUBLIC BOOL_DEFINE=1)

target_link_libraries(LINUX_X86_TEST PUBLIC DING_OS)
target_link_libraries(LINUX_X86_TEST PUBLIC OS_AL_LIB)
target_link_libraries(LINUX_X86_TEST PUBLIC Threads::Threads)
