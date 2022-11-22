#include <iostream>
#include "sysyfDriver.h"
#include "SyntaxTreePrinter.h"
#include "ErrorReporter.h"
#include "SyntaxTreeChecker.h"


void print_help(const std::string& exe_name) {
  std::cout << "Usage: " << exe_name
            << " [ -h | --help ] [ -p | --trace_parsing ] [ -s | --trace_scanning ] [ -emit-ast ]"
            << " <input-file>"
            << std::endl;
}

int main(int argc, char *argv[])
{
    sysyfDriver driver;
    SyntaxTreePrinter printer;
    ErrorReporter reporter(std::cerr);
    SyntaxTreeChecker checker(reporter);

    bool print_ast = false;

    std::string filename = "testcase.sy";
    for (int i = 1; i < argc; ++i) {
        if (argv[i] == std::string("-h") || argv[i] == std::string("--help")) {
            print_help(argv[0]);
            return 0;
        }
        else if (argv[i] == std::string("-p") || argv[i] == std::string("--trace_parsing"))
            driver.trace_parsing = true;
        else if (argv[i] == std::string("-s") || argv[i] == std::string("--trace_scanning"))
            driver.trace_scanning = true;
        else if (argv[i] == std::string("-emit-ast"))
            print_ast = true;
        else {
            filename = argv[i];
        }
    }
    auto root = driver.parse(filename);
    if(print_ast)
        root->accept(printer);
    root->accept(checker);
    return 0;
}
