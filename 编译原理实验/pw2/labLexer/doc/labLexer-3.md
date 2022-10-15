# Antlr

## usage

build:

```shell
mkdir build && cd build && cmake .. && make
```

## parser

```antlr
lexer grammar labLexer;

Relop: '<'|'>'|'='|'<='|'>='|'<>';
Other: ~[=><]*;
```

Same as flex version

## output

```cpp
// read one line from stdin
std::string line;
getline(std::cin, line);

// gen token stream via input string
ANTLRInputStream input(line);
labLexer lexer(&input);
CommonTokenStream tokens(&lexer);

tokens.fill();

// output
for (auto token : tokens.getTokens()) {
    if (token->getType() == labLexer::Relop) {
        std::cout << "(relop," << token->getText() << ")";
    } else if (token->getType() == labLexer::Other) {
        std::cout << "(other," << token->getText().size() << ")";
    }
}
```