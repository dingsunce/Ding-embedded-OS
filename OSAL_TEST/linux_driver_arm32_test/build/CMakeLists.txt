
set(KERNEL_SOURCE_DIR "/home/ding/linux-5.15.24")

set(TAGET_NAME "make_linux_ko")

add_custom_target(${TAGET_NAME}
    COMMAND make 
            -C ${KERNEL_SOURCE_DIR}  
                M=${CMAKE_CURRENT_SOURCE_DIR} 
                modules 
                ARCH=arm 
                CROSS_COMPILE=arm-none-linux-gnueabihf-

    COMMAND echo "remove .o .cmd files"    

    COMMAND ${CMAKE_COMMAND} -DDIR=${CMAKE_CURRENT_SOURCE_DIR} -P "${CMAKE_CURRENT_SOURCE_DIR}/cleanup.cmake"

    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Creating linux ko..."
    VERBATIM
)

add_custom_target(clean_module
    COMMAND make 
            -C ${KERNEL_SOURCE_DIR} 
            M=${CMAKE_CURRENT_SOURCE_DIR} 
            clean 
            ARCH=arm 
            CROSS_COMPILE=arm-none-linux-gnueabihf-
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Cleaning linux ko..."
)

# run ${TAGET_NAME} target in cmake build
add_custom_target(default_target ALL
    COMMAND echo "Running default target"
    DEPENDS  ${TAGET_NAME}
)
