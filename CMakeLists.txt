cmake_minimum_required(VERSION 3.15)
project(DING_OS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message("running on Windows OS")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message("running on Linux OS")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

    message("Using GCC compiler")

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")

    message("Using Clang compiler")

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message("Using MSVC compiler")

    # disable warning of Unannotated fallthrough between switch labels
    add_compile_options(/wd26819)

    # disable warning of deprecated function call
    add_compile_options(/wd4996)

    # disable warning of Prefer 'enum class' over 'enum
    add_compile_options(/wd26812)
endif()


set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

#1 : unit test environment
#2 : windows x86 test environment
#3 : linux x86 test environment
#4 : linux arm32 test environment
set(TEST_ENVN 2)

if(TEST_ENVN STREQUAL 4) # linux arm32 test environment
    set(CMAKE_SYSTEM_PROCESSOR arm)
    set(CMAKE_C_COMPILER arm-none-linux-gnueabihf-gcc)
    set(CMAKE_CXX_COMPILER arm-none-linux-gnueabihf-g++)
    set(PROJECT_BINARY_DIR "${CMAKE_BINARY_DIR}")
    set(TARGET_NAME "bin/LINUX_ARM32_TEST")
    # @ONLY only replace @var@ in launch_connect_to_gdbserver.in.json
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

if(TEST_ENVN STREQUAL 1) # unit test environment
    add_subdirectory(${CMAKE_SOURCE_DIR}/OSAL/non_preemptive_os)
    add_subdirectory(${CMAKE_SOURCE_DIR}/UnitTest)
endif()

if(TEST_ENVN STREQUAL 2) # windows x86 test environment
    add_subdirectory(${CMAKE_SOURCE_DIR}/OSAL/windows)
    add_subdirectory(${CMAKE_SOURCE_DIR}/OSAL_TEST/windows_test)
endif()

if(TEST_ENVN STREQUAL 3) # linux x86 test environment
    add_subdirectory(${CMAKE_SOURCE_DIR}/OSAL/linux)
    add_subdirectory(${CMAKE_SOURCE_DIR}/OSAL_TEST/linux_x86_test)
endif()

if(TEST_ENVN STREQUAL 4) # linux arm32 test environment
    add_subdirectory(${CMAKE_SOURCE_DIR}/OSAL/linux)
    add_subdirectory(${CMAKE_SOURCE_DIR}/OSAL_TEST/linux_arm32_test)
endif()

