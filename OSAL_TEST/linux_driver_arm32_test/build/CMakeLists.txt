
set(KERNEL_SOURCE_DIR "/home/ding/linux-5.15.24")

set(TAGET_NAME "MakeLinuxKo")

add_custom_target(${TAGET_NAME}
    COMMAND make -C ${KERNEL_SOURCE_DIR}  M=${CMAKE_CURRENT_SOURCE_DIR} modules ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabihf-
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Creating linux ko..."
    VERBATIM
)

add_custom_target(clean_module
    COMMAND make -C ${KERNEL_SOURCE_DIR} M=${CMAKE_CURRENT_SOURCE_DIR} clean ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabihf-
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Cleaning module"
)

# run ${TAGET_NAME} target in cmake build
add_custom_target(default_target ALL
    DEPENDS ${TAGET_NAME}
)











