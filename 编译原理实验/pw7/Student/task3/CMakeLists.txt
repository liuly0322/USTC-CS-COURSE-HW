cmake_minimum_required(VERSION 3.10)

project(mClang)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -g")
find_package(LLVM REQUIRED)
find_package(Clang REQUIRED)

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)
llvm_map_components_to_libnames(llvm_libs 
    Core
    native
    mcjit
)

message(STATUS "Found LLVM_INCLUDE_DIRS ${LLVM_INCLUDE_DIRS}")
message(STATUS "Found LLVM_DEFINITIONS ${LLVM_DEFINITIONS}")

include_directories(${LLVM_INCLUDE_DIRS} include)
add_definitions(${LLVM_DEFINITIONS})

add_executable(${PROJECT_NAME}
    main.cpp
    src/Driver/driver.cpp
    src/runtime/io.cpp
    src/runtime/runtime.cpp
    )
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    clangBasic
    clangCodeGen
    clangDriver
    clangFrontend
    clangSerialization
    clangAnalysis
    clangStaticAnalyzerCore
    clangStaticAnalyzerFrontend
    ${llvm_libs}
)
