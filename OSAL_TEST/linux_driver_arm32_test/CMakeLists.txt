

file(GLOB_RECURSE src_c *.c ../app_ipc_test.c ../app_msg_test.c)
file(GLOB_RECURSE src_cpp *.cpp)

#add_executable(LINUX_DRIVER_ARM32_TEST ../main.c ${src_c} ${src_cpp})

target_compile_definitions(DING_OS PUBLIC MEM_DEBUG=1)
target_compile_definitions(DING_OS PUBLIC MESSAGE_DEBUG=1)
target_compile_definitions(DING_OS PUBLIC TASK_DEBUG=1)
target_compile_definitions(DING_OS PUBLIC MEM_ALIGMENT_4BYTES=0)
target_compile_definitions(DING_OS PUBLIC BOOL_DEFINE=1)

#target_link_libraries(LINUX_DRIVER_ARM32_TEST PUBLIC DING_OS)
#target_link_libraries(LINUX_DRIVER_ARM32_TEST PUBLIC OS_AL_LIB)

set(KERNEL_SOURCE_DIR "/home/ding/linux-5.15.24")

set(TAGET_NAME "MakeLinuxKo")

# Set the source file for your driver module
set(DRIVER_SRCS chrdevbase.c)

add_custom_target(${TAGET_NAME}
    COMMAND make -C ${KERNEL_SOURCE_DIR}  M=${CMAKE_CURRENT_SOURCE_DIR} I=${CMAKE_CURRENT_SOURCE_DIR}/DING_OS modules ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabihf-
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Creating linux ko..."
    VERBATIM
)

add_custom_target(clean_module
    COMMAND make -C ${KERNEL_SOURCE_DIR} M=${CMAKE_CURRENT_SOURCE_DIR} MODDESTDIR=${CMAKE_BINARY_DIR}/bin clean ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabihf-
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Cleaning module"
)

# run ${TAGET_NAME} target in cmake build
add_custom_target(default_target ALL
    DEPENDS ${TAGET_NAME}
)

