cmake_minimum_required(VERSION 3.14)
project(NamedPipeLab5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Executables
add_executable(server.exe
    src/server.cpp
    src/employee.cpp
    src/utils.cpp
)

add_executable(client.exe
    src/client.cpp
    src/utils.cpp
)

target_include_directories(server.exe PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(client.exe PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Thread support
find_package(Threads REQUIRED)
target_link_libraries(server.exe PRIVATE Threads::Threads)

# Windows support
if (MINGW)
    set_target_properties(server.exe PROPERTIES SUFFIX "")
    set_target_properties(client.exe PROPERTIES SUFFIX "")
endif()

# Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# Test runner
add_executable(test_runner
    tests/test_runner.cpp
    tests/test_employee.cpp
    tests/test_utils.cpp
    src/employee.cpp
    src/utils.cpp
)

target_link_libraries(test_runner
    gtest_main
    gmock
    pthread
)

target_include_directories(test_runner PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

enable_testing()
add_test(NAME all_tests COMMAND test_runner)
