project(SYSYF)

cmake_minimum_required(VERSION 3.5)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -Wall -g -pedantic")

# add macros to generate ANTLR Cpp code from grammar
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

BISON_TARGET(Parser ${CMAKE_CURRENT_SOURCE_DIR}/grammar/sysyfParser.yy ${CMAKE_CURRENT_BINARY_DIR}/sysyfParser.cpp
             COMPILE_FLAGS "-Wall -Wcounterexamples --defines=${CMAKE_CURRENT_BINARY_DIR}/sysyfParser.h")
FLEX_TARGET(Scanner ${CMAKE_CURRENT_SOURCE_DIR}/grammar/sysyfScanner.ll  ${CMAKE_CURRENT_BINARY_DIR}/sysyfScanner.cpp
            COMPILE_FLAGS "--debug --c++")
ADD_FLEX_BISON_DEPENDENCY(Scanner Parser)

# include generated files in project environment
include_directories(${CMAKE_CURRENT_BINARY_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(executable sysYFParser)

add_executable(${executable}
  ${CMAKE_CURRENT_SOURCE_DIR}/src/sysyfDriver.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SyntaxTree.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SyntaxTreePrinter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ErrorReporter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SyntaxTreeChecker.cpp
  ${BISON_Parser_OUTPUTS}
  ${FLEX_Scanner_OUTPUTS})

add_executable(integrationTestWrapper
  ${CMAKE_CURRENT_SOURCE_DIR}/test/test.cpp)

# enable testing functionality
enable_testing()

set(TESTCASES 2/01 2/02 2/03 2/04 2/05 2/06 2/07 2/11 2/12 2/13 2/14 2/15 2/16 2/17 3/01 3/02 3/03 3/04 3/05 3/06 3/07)
FOREACH(testcase ${TESTCASES})
  set(testcase_name IntegrationTest_${testcase})
  add_test(NAME ${testcase_name} COMMAND integrationTestWrapper ${executable} ../test/task${testcase}.sy)
  file(READ test/task${testcase}.out expected_output LIMIT_INPUT 1)
  set_tests_properties(${testcase_name} PROPERTIES PASS_REGULAR_EXPRESSION ${expected_output})
ENDFOREACH(testcase)
