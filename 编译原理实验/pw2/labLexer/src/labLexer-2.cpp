#include <iostream>
#include <string>

#ifdef LEXERGEN
#include "lex.yy.c"
#else
#include "labLexer-1-parser.hpp"
#endif

int main() {
    std::string line;
    getline(std::cin, line);

#ifdef LEXERGEN
    yy_scan_string(line.c_str());
    yylex();
#else
    Tokens tokens = parse_tokens(line);

    for (const auto& token : tokens) {
        if (std::get<0>(token) == relop) {
            RelopType type = std::get<1>(token).relop;
            switch (type) {
                case le:
                    std::cout << "(relop,<=)";
                    break;
                case lr:
                    std::cout << "(relop,<>)";
                    break;
                case l:
                    std::cout << "(relop,<)";
                    break;
                case e:
                    std::cout << "(relop,=)";
                    break;
                case re:
                    std::cout << "(relop,>=)";
                    break;
                case r:
                    std::cout << "(relop,>)";
            }
        } else {
            unsigned count = std::get<1>(token).count;
            std::cout << "(other," << count << ")";
        }
    }
#endif
}