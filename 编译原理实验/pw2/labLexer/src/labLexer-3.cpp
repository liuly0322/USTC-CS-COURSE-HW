#include <iostream>
#include <string>

#include "antlr4-runtime.h"
#include "labLexer.h"

using namespace antlr4;

int main(int argc, const char* argv[]) {
    std::string line;
    getline(std::cin, line);

    ANTLRInputStream input(line);
    labLexer lexer(&input);
    CommonTokenStream tokens(&lexer);

    tokens.fill();
    for (auto token : tokens.getTokens()) {
        if (token->getType() == labLexer::Relop) {
            std::cout << "(relop," << token->getText() << ")";
        } else if (token->getType() == labLexer::Other) {
            std::cout << "(other," << token->getText().size() << ")";
        }
    }

    return 0;
}
