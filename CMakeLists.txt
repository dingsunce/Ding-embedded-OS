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

set(TEST_TYPE 1)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/DING_OS)

if(TEST_TYPE STREQUAL 1)
    include_directories(${CMAKE_SOURCE_DIR}/OSAL/non_preemptive_os)
endif()

if(TEST_TYPE STREQUAL 2)
    include_directories(${CMAKE_SOURCE_DIR}/OSAL/windows)
endif()

if(TEST_TYPE STREQUAL 3)
    include_directories(${CMAKE_SOURCE_DIR}/OSAL/linux)
endif()

add_subdirectory(${CMAKE_SOURCE_DIR}/DING_OS)

if(TEST_TYPE STREQUAL 1)
    add_subdirectory(${CMAKE_SOURCE_DIR}/OSAL/non_preemptive_os)
    add_subdirectory(${CMAKE_SOURCE_DIR}/UnitTest)
endif()

if(TEST_TYPE STREQUAL 2)
    add_subdirectory(${CMAKE_SOURCE_DIR}/OSAL/windows)
    add_subdirectory(${CMAKE_SOURCE_DIR}/OSAL/windows_test)
endif()

if(TEST_TYPE STREQUAL 3)
    add_subdirectory(${CMAKE_SOURCE_DIR}/OSAL/linux)
    add_subdirectory(${CMAKE_SOURCE_DIR}/OSAL/linux_test)
endif()
