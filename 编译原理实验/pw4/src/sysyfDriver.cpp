#include "sysyfDriver.h"
#include "../build/sysyfParser.h"

sysyfDriver::sysyfDriver()
    : trace_scanning(false), trace_parsing(false)
{
}

sysyfDriver::~sysyfDriver()
{
}

SyntaxTree::Node* sysyfDriver::parse(const std::string &f)
{
    file = f;

    // lexer begin
    scan_begin();
    yy::sysyfParser parser(*this);
    parser.set_debug_level(trace_parsing);
    // parser begin
    parser.parse();
    // lexer end
    scan_end();

    return this->root;
}

void sysyfDriver::error(const yy::location& l, const std::string& m)
{
    std::cerr << l << ": " << m << std::endl;
}

void sysyfDriver::error(const std::string& m)
{
    std::cerr << m << std::endl;
}


void sysyfDriver::scan_begin()
{
    lexer.set_debug(trace_scanning);

    // Try to open the file:
    instream.open(file);

    if(instream.good()) {
        lexer.switch_streams(&instream, 0);
    }
    else if(file == "-") {
        lexer.switch_streams(&std::cin, 0);
    }
    else {
        error("Cannot open file '" + file + "'.");
        exit(EXIT_FAILURE);
    }
}

void sysyfDriver::scan_end()
{
    instream.close();
}
