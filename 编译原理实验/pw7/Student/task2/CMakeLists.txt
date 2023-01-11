project(task2)

cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -Wall -g -pedantic")

set(SYSYF_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# include generated files in project environment
include_directories(${SYSYF_SOURCE_DIR}include/Frontend)
include_directories(${SYSYF_SOURCE_DIR}include/AST)
include_directories(${SYSYF_SOURCE_DIR}include/ErrorReporter)
include_directories(${SYSYF_SOURCE_DIR}include/SysYFIRBuilder)
include_directories(${SYSYF_SOURCE_DIR}include/SysYFIR)
add_subdirectory(${SYSYF_SOURCE_DIR}src/AST src/AST)
add_subdirectory(${SYSYF_SOURCE_DIR}src/ErrorReporter src/ErrorReporter)
add_subdirectory(${SYSYF_SOURCE_DIR}src/SysYFIRBuilder src/SysYFIRBuilder)
add_subdirectory(${SYSYF_SOURCE_DIR}src/SysYFIR src/SysYFIR)

add_subdirectory(demo)
add_subdirectory(cpp)
