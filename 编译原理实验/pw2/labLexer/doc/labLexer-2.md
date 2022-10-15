# Flex

## usage

simply `make`

## string as flex input

There are several ways, one possible way is:

```c
yy_scan_string(line.c_str());
```

also it seems that flex can generate c++ class code

## relop.lex

This file is quite simple :)

```lex
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
```