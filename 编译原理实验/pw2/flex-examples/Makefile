CC       = gcc -g
LEX      = flex -i -I 
INC      = -I../pl0/include 

all: exprlex 

exprlex:   expr.lex
	$(LEX)  expr.lex
	$(CC)  $(INC) -DLEXERGEN lex.yy.c expr.c -o expr -ll

expr:   
	$(CC)  $(INC) expr.c -o expr -ll

clean:
	rm -f *.BAK *.o core *~* *.a 
	rm -f *.tab.h *.tab.c
	rm -f expr *.out
