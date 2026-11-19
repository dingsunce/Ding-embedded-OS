cmake_minimum_required(VERSION 3.10)
project(LinuxKoBuild)

# 1. Allow environment variables to override your local paths and toolchains
set(KERNEL_SOURCE_DIR "$ENV{KERNEL_SRC}")
if(NOT KERNEL_SOURCE_DIR)
    set(KERNEL_SOURCE_DIR "/home/ding/linux-5.15.24")
endif()

set(TARGET_ARCH "$ENV{ARCH}")
if(NOT TARGET_ARCH)
    set(TARGET_ARCH "arm")
endif()

set(TOOLCHAIN_PREFIX "$ENV{CROSS_COMPILE}")
if(NOT TOOLCHAIN_PREFIX)
    set(TOOLCHAIN_PREFIX "arm-none-linux-gnueabihf-")
endif()

set(TAGET_NAME "make_linux_ko")

# 2. Use the dynamically resolved variables in your custom target
add_custom_target(${TAGET_NAME}
    COMMAND make 
            -C ${KERNEL_SOURCE_DIR}  
            M=${CMAKE_CURRENT_SOURCE_DIR} 
            modules 
            ARCH=${TARGET_ARCH} 
            CROSS_COMPILE=${TOOLCHAIN_PREFIX}

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
            ARCH=${TARGET_ARCH} 
            CROSS_COMPILE=${TOOLCHAIN_PREFIX}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Cleaning linux ko..."
)

add_custom_target(default_target ALL
    COMMAND echo "Running default target"
    DEPENDS ${TAGET_NAME}
)