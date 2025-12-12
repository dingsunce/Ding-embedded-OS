cmake_minimum_required(VERSION 3.15)
project(DING_OS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if(MSVC)
        # disable warning of Unannotated fallthrough between switch labels
        add_compile_options(/wd26819)

        # disable warning of deprecated function call
        add_compile_options(/wd4996)

        # disable warning of Prefer 'enum class' over 'enum
        add_compile_options(/wd26812)
        message("Windows MSVC Compiler")
    endif()
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    if(GCC)
        message("Linux GCC Compiler")
    endif()
endif()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

#1 : unit test environment
#2 : windows x86 test environment
#3 : linux x86 test environment
#4 : linux arm32 test environment
set(TEST_ENVN 4)

if(TEST_ENVN STREQUAL 4) # linux arm32 environment
    set(CMAKE_SYSTEM_PROCESSOR arm)
    set(CMAKE_C_COMPILER arm-none-linux-gnueabihf-gcc)
    set(CMAKE_CXX_COMPILER arm-none-linux-gnueabihf-g++)
    set(PROJECT_BINARY_DIR "${CMAKE_BINARY_DIR}")
    set(TARGET_NAME "bin/LINUX_ARM32_TEST")
    configure_file(${CMAKE_SOURCE_DIR}/.vscode/launch_connect_to_gdbserver.in.json ${CMAKE_SOURCE_DIR}/.vscode/launch.json @ONLY)
else()
    configure_file(${CMAKE_SOURCE_DIR}/.vscode/launch_none.in.json ${CMAKE_SOURCE_DIR}/.vscode/launch.json @ONLY)
endif()

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/DING_OS)

if(TEST_ENVN STREQUAL 1)
    include_directories(${CMAKE_SOURCE_DIR}/OSAL/non_preemptive_os)
    message("unit test Environment")
endif()

if(TEST_ENVN STREQUAL 2)
    include_directories(${CMAKE_SOURCE_DIR}/OSAL/windows)
    message("windows x86 test environment")
endif()

if(TEST_ENVN STREQUAL 3)
    include_directories(${CMAKE_SOURCE_DIR}/OSAL/linux)
    message("linux x86 test environment")
endif()

if(TEST_ENVN STREQUAL 4)
    include_directories(${CMAKE_SOURCE_DIR}/OSAL/linux)
    message("linux arm32 test environment")
endif()

add_subdirectory(${CMAKE_SOURCE_DIR}/DING_OS)

if(TEST_ENVN STREQUAL 1) #raw environment
    add_subdirectory(${CMAKE_SOURCE_DIR}/OSAL/non_preemptive_os)
    add_subdirectory(${CMAKE_SOURCE_DIR}/UnitTest)
endif()

if(TEST_ENVN STREQUAL 2) # windows x86 environment
    add_subdirectory(${CMAKE_SOURCE_DIR}/OSAL/windows)
    add_subdirectory(${CMAKE_SOURCE_DIR}/OSAL/windows_test)
endif()

if(TEST_ENVN STREQUAL 3) # linux x86 environment
    add_subdirectory(${CMAKE_SOURCE_DIR}/OSAL/linux)
    add_subdirectory(${CMAKE_SOURCE_DIR}/OSAL/linux_x86_test)
endif()

if(TEST_ENVN STREQUAL 4) # linux arm32 environment
    add_subdirectory(${CMAKE_SOURCE_DIR}/OSAL/linux)
    add_subdirectory(${CMAKE_SOURCE_DIR}/OSAL/linux_arm32_test)
endif()

