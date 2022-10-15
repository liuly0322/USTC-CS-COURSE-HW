#include "../src/labLexer-1-parser.hpp"

template <typename T>
void assert(T a, T b) {
    if (a != b) {
        std::cout << "Test failed!" << std::endl;
        exit(1);
    }
}

int main() {
    std::string line = std::string("if (a<=b) a=b");
    Tokens tokens = parse_tokens(line);

    assert(tokens.size(), (size_t)5);

    assert(std::get<0>(tokens[0]), other);
    assert(std::get<1>(tokens[0]).count, (unsigned)5);

    assert(std::get<0>(tokens[1]), relop);
    assert(std::get<1>(tokens[1]).relop, le);

    assert(std::get<0>(tokens[2]), other);
    assert(std::get<1>(tokens[2]).count, (unsigned)4);

    assert(std::get<0>(tokens[3]), relop);
    assert(std::get<1>(tokens[3]).relop, e);

    assert(std::get<0>(tokens[4]), other);
    assert(std::get<1>(tokens[4]).count, (unsigned)1);

    std::cout << "Test Passed for labLexer-1" << std::endl;
}