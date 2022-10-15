CC       = g++ -std=c++14
LEX      = flex -i -I 

all: expr exprlex 

exprlex:   grammar/relop.lex
	$(LEX)  -o src/lex.yy.c grammar/relop.lex
	$(CC)   -DLEXERGEN src/labLexer-2.cpp -o labLexer-2

expr:   
	$(CC)   src/labLexer-2.cpp -o labLexer-2m

clean:
	rm labLexer-2
	rm labLexer-2m
