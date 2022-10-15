## Manually writing lexical parser

## usage

build:

```shell
mkdir build && cd build && cmake .. && make
```

test:

(Run in `build` folder)

```shell
ctest
```

run:

```
./labLexer-1
```

## doc

```
src
├── labLexer-1.cpp
├── labLexer-1-parser.hpp
```

`labLexer-1-parser.hpp` is a header only parser

it's only a demo so no namespace is used

```
test
└── labLexer-1-test.cpp
```

`labLexer-1-test.cpp` includes `labLexer-1-parser.hpp` to perform test