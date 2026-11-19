include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://gitee.com/mirrors/googletest/repository/archive/v1.15.2.tar.gz
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE unit_test_src_c *.c)
file(GLOB_RECURSE unit_test_src_cpp *.cpp)
add_executable(UNIT_TEST main.cpp ${unit_test_src_c} ${unit_test_src_cpp})

target_compile_definitions(DING_OS PUBLIC DMEM_DEBUG=1)
target_compile_definitions(DING_OS PUBLIC DMESSAGE_DEBUG=1)
target_compile_definitions(DING_OS PUBLIC DTASK_DEBUG=1)

target_compile_definitions(DING_OS PUBLIC UNIT_TEST=1)
target_compile_definitions(DING_OS PUBLIC DMEM_ALIGMENT_4BYTES=0)

target_include_directories(UNIT_TEST PUBLIC ./TestMock)
target_include_directories(UNIT_TEST PUBLIC ./TestSrc)
target_include_directories(UNIT_TEST PUBLIC ./TestStub)

target_link_libraries(UNIT_TEST PUBLIC DING_OS)
target_link_libraries(UNIT_TEST PUBLIC OS_AL_LIB)
target_link_libraries(UNIT_TEST PUBLIC gtest)

# Connects your executable to CMake's built-in CTest runner
include(GoogleTest)
gtest_discover_tests(UNIT_TEST)

add_custom_command(TARGET UNIT_TEST POST_BUILD COMMAND UNIT_TEST)
