#include <iostream>
#include <string>
#include <tuple>
#include <unordered_set>
#include <vector>

enum TokenType { relop, other };
enum RelopType { le, lr, l, e, re, r };
union TokenData {
    RelopType relop;
    unsigned count;
    TokenData(unsigned count) : count(count){};
    TokenData(RelopType relop) : relop(relop){};
};
using Token = std::tuple<TokenType, TokenData>;
using Tokens = std::vector<Token>;

std::vector<Token> parse_tokens(std::string& line) {
    std::vector<Token> tokens{};

    unsigned state = 0;
    unsigned count_so_far = 0;

    auto clear_other = [&count_so_far, &tokens]() {
        if (count_so_far)
            tokens.push_back(Token(other, count_so_far));
        count_so_far = 0;
    };

    auto trans_from_inital_state = [&state, &count_so_far, &clear_other,
                                    &tokens](char c) {
        if (c == '=') {
            clear_other();
            tokens.push_back(Token(relop, e));
        } else if (c == '<') {
            clear_other();
            state = 1;
        } else if (c == '>') {
            clear_other();
            state = 2;
        } else {
            count_so_far++;
        }
    };

    for (const auto& c : line) {
        switch (state) {
            case 0:
                trans_from_inital_state(c);
                break;
            case 1:
                state = 0;
                if (c == '=') {
                    tokens.push_back(Token(relop, le));
                } else if (c == '>') {
                    tokens.push_back(Token(relop, lr));
                } else {
                    tokens.push_back(Token(relop, l));
                    trans_from_inital_state(c);
                }
                break;
            case 2:
                state = 0;
                if (c == '=') {
                    tokens.push_back(Token(relop, re));
                } else {
                    tokens.push_back(Token(relop, r));
                    trans_from_inital_state(c);
                }
        }
    }

    clear_other();
    if (state == 1)
        tokens.push_back(Token(other, l));
    if (state == 2)
        tokens.push_back(Token(relop, r));

    return tokens;
}