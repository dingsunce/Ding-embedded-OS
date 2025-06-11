project(BEGOS)
cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if (MSVC)
        #disable warning of Unannotated fallthrough between switch labels
        add_compile_options(/wd26819)
        #disable warning of deprecated function call
        add_compile_options(/wd4996)
        #disable warning of Prefer 'enum class' over 'enum
        add_compile_options(/wd26812)
        message("Windows MSVC Compiler")
    endif ()
endif ()

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    if (GCC)
        message("Linux GCC Compiler")
    endif ()
endif ()



include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/OS)

add_subdirectory(${CMAKE_SOURCE_DIR}/OS)
add_subdirectory(${CMAKE_SOURCE_DIR}/UnitTest)

