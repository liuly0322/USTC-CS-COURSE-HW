%{
#include <stdio.h>
#include <string.h>

extern char *yytext;
%}

%%
(<>)|(<=)|(>=)|<|>|= {
    printf("(relop,%s)", yytext);
}
[^<>=]*	{
    strlen(yytext) ? printf("(other,%d)", strlen(yytext)) : 0;
}
%%

int yywrap() {
    return 1;
}