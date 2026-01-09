
file(GLOB_RECURSE src_c *.c ../app_ipc_test.c ../app_msg_test.c)
file(GLOB_RECURSE src_cpp *.cpp)

add_executable(WINODWS_TEST ../main.c ${src_c} ${src_cpp})

target_compile_definitions(DING_OS PUBLIC DMEM_DEBUG=1)
target_compile_definitions(DING_OS PUBLIC DMESSAGE_DEBUG=1)
target_compile_definitions(DING_OS PUBLIC DTASK_DEBUG=1)
target_compile_definitions(DING_OS PUBLIC UNIT_TEST=1)
target_compile_definitions(DING_OS PUBLIC DMEM_ALIGMENT_4BYTES=0)
target_compile_definitions(DING_OS PUBLIC OS_DEBUG=1)

target_link_libraries(WINODWS_TEST PUBLIC DING_OS)
target_link_libraries(WINODWS_TEST PUBLIC OS_AL_LIB)
target_link_libraries(WINODWS_TEST PUBLIC winmm)








