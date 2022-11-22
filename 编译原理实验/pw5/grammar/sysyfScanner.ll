%{ /* -*- C++ -*- */
# include <cerrno>
# include <climits>
# include <cstdlib>
# include <cstring> // strerror
# include <string>
# include <algorithm>
# include <iostream>
# include <map>
std::map<char,int> ch2int = {
    {'0',0},
    {'1',1},
    {'2',2},
    {'3',3},
    {'4',4},
    {'5',5},
    {'6',6},
    {'7',7},
    {'8',8},
    {'9',9},
    {'a',10},
    {'b',11},
    {'c',12},
    {'d',13},
    {'e',14},
    {'f',15},
    {'A',10},
    {'B',11},
    {'C',12},
    {'D',13},
    {'E',14},
    {'F',15}
};
# include "sysyfDriver.h"
# include "sysyfParser.h"
# define RET_NOTYPE_TOKEN(name) \
    auto mytoken = yy::sysyfParser::make_##name(loc);\
    loc.step();\
    return mytoken;

# define RET_TYPE_TOKEN(name, value) \
    auto mytoken = yy::sysyfParser::make_##name(value, loc);\
    loc.step();\
    return mytoken;
%}

%{
#if defined __clang__
# define CLANG_VERSION (__clang_major__ * 100 + __clang_minor__)
#endif

// Clang and ICC like to pretend they are GCC.
#if defined __GNUC__ && !defined __clang__ && !defined __ICC
# define GCC_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)
#endif

// Pacify warnings in yy_init_buffer (observed with Flex 2.6.4)
// and GCC 6.4.0, 7.3.0 with -O3.
#if defined GCC_VERSION && 600 <= GCC_VERSION
# pragma GCC diagnostic ignored "-Wnull-dereference"
#endif

// This code uses Flex's C backend, yet compiles it as C++.
// So expect warnings about C style casts and NULL.
#if defined CLANG_VERSION && 500 <= CLANG_VERSION
# pragma clang diagnostic ignored "-Wold-style-cast"
# pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#elif defined GCC_VERSION && 407 <= GCC_VERSION
# pragma GCC diagnostic ignored "-Wold-style-cast"
# pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif

#define FLEX_VERSION (YY_FLEX_MAJOR_VERSION * 100 + YY_FLEX_MINOR_VERSION)

// Old versions of Flex (2.5.35) generate an incomplete documentation comment.
//
//  In file included from src/scan-code-c.c:3:
//  src/scan-code.c:2198:21: error: empty paragraph passed to '@param' command
//        [-Werror,-Wdocumentation]
//   * @param line_number
//     ~~~~~~~~~~~~~~~~~^
//  1 error generated.
#if FLEX_VERSION < 206 && defined CLANG_VERSION
# pragma clang diagnostic ignored "-Wdocumentation"
#endif

// Old versions of Flex (2.5.35) use 'register'.  Warnings introduced in
// GCC 7 and Clang 6.
#if FLEX_VERSION < 206
# if defined CLANG_VERSION && 600 <= CLANG_VERSION
#  pragma clang diagnostic ignored "-Wdeprecated-register"
# elif defined GCC_VERSION && 700 <= GCC_VERSION
#  pragma GCC diagnostic ignored "-Wregister"
# endif
#endif

#if FLEX_VERSION < 206
# if defined CLANG_VERSION
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wdocumentation"
#  pragma clang diagnostic ignored "-Wshorten-64-to-32"
#  pragma clang diagnostic ignored "-Wsign-conversion"
# elif defined GCC_VERSION
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wsign-conversion"
# endif
#endif
%}

%option noyywrap nounput noinput batch debug

%{
  // Code run each time a pattern is matched.
  # define YY_USER_ACTION  loc.columns(yyleng);
%}

/* Regex abbreviations: */
MultilineComment		"/*"([^\*]|(\*)*[^\*/])*(\*)*"*/"
Identifier		      [_a-zA-Z][a-zA-Z0-9_]*
OctConst            ("0"[0-7]*)
DecConst            ([1-9][0-9]*)
HexConst            ("0"[xX][0-9a-fA-F]+)
FloatConst          ((([0-9]*[.][0-9]+)|([0-9]+[.]))([eE][-+]?[0-9]+)?)|([0-9]+[eE][-+]?[0-9]+)
Blank               [ \t\r]
NewLine             [\n]
SingleLineComment	"//"[^\n\r]*[\n\r]
LRBRACKET          \[([ \r\t\n]|{MultilineComment}|{SingleLineComment})*\]

%%
 /* keyword */
float       {RET_NOTYPE_TOKEN(FLOAT);}
int 		{RET_NOTYPE_TOKEN(INT);}
return 	    {RET_NOTYPE_TOKEN(RETURN);}
void 		{RET_NOTYPE_TOKEN(VOID);}
const		{RET_NOTYPE_TOKEN(CONST);}
break       {RET_NOTYPE_TOKEN(BREAK);}
continue    {RET_NOTYPE_TOKEN(CONTINUE);}
while       {RET_NOTYPE_TOKEN(WHILE);}
if          {RET_NOTYPE_TOKEN(IF);}
else        {RET_NOTYPE_TOKEN(ELSE);}

[<]         {RET_NOTYPE_TOKEN(LT);}
"<="        {RET_NOTYPE_TOKEN(LTE);}
[>]         {RET_NOTYPE_TOKEN(GT);}
">="        {RET_NOTYPE_TOKEN(GTE);}
"=="        {RET_NOTYPE_TOKEN(EQ);}
"!="        {RET_NOTYPE_TOKEN(NEQ);}
[!]         {RET_NOTYPE_TOKEN(NOT);}
"&&"        {RET_NOTYPE_TOKEN(LOGICAND);}
"||"        {RET_NOTYPE_TOKEN(LOGICOR);}
[+] 		{RET_NOTYPE_TOKEN(PLUS);}
[-] 		{RET_NOTYPE_TOKEN(MINUS);}
[*] 		{RET_NOTYPE_TOKEN(MULTIPLY);}
[/] 		{RET_NOTYPE_TOKEN(DIVIDE);}
[%]			{RET_NOTYPE_TOKEN(MODULO);}
[=] 		{RET_NOTYPE_TOKEN(ASSIGN);}
[;] 		{RET_NOTYPE_TOKEN(SEMICOLON);}
[,] 		{RET_NOTYPE_TOKEN(COMMA);}
[(] 		{RET_NOTYPE_TOKEN(LPARENTHESE);}
[)] 		{RET_NOTYPE_TOKEN(RPARENTHESE);}
[[] 		{RET_NOTYPE_TOKEN(LBRACKET);} 
[]] 		{RET_NOTYPE_TOKEN(RBRACKET);}
[{] 		{RET_NOTYPE_TOKEN(LBRACE);}
[}] 		{RET_NOTYPE_TOKEN(RBRACE);}


{LRBRACKET}               {RET_NOTYPE_TOKEN(LRBRACKET);}
{Blank}+                  {loc.step();}
{NewLine}+                {loc.lines(yyleng); loc.step();}
{MultilineComment}				{std::string s = yytext;
                          size_t n = std::count(s.begin(), s.end(), '\n');
                          for (size_t i = 0; i < n; i++) loc.lines(1);}
{SingleLineComment}				{loc.lines(1);loc.step();}
{DecConst} 				  {std::string dec = yytext;
                           unsigned sum = 0;
                           int len = dec.size();
                           for(int i = 0;i < len;i++){
                               auto a = dec[i];
                               sum = sum * 10 + a - 48;
                           }
                           RET_TYPE_TOKEN(INTCONST, int(sum));}
{HexConst}                {std::string hex = yytext;
                           unsigned sum = 0;
                           int len = hex.size();
                           for(int i = 2;i < len;i++){
                               auto a = hex[i];
                               sum  = sum * 16 + ch2int[a];
                           }
                           RET_TYPE_TOKEN(INTCONST, int(sum));}
{OctConst}                {std::string oct = yytext;
                           unsigned sum = 0;
                           int len = oct.size();
                           for(int i = 1;i < len;i++){
                               auto a = oct[i];
                               sum  = sum * 8 + ch2int[a];
                           }
                           RET_TYPE_TOKEN(INTCONST, int(sum));}
{Identifier} 			  {RET_TYPE_TOKEN(IDENTIFIER, yytext);}
{FloatConst}			  {RET_TYPE_TOKEN(FLOATCONST, std::stod(yytext));}
<<EOF>>                   {RET_NOTYPE_TOKEN(END);}
.			              {std::cout << "Error in scanner!" << '\n'; exit(1);}
%%

int yyFlexLexer::yylex() {
    std::cerr << "'int yyFlexLexer::yylex()' should never be called." << std::endl;
    exit(1);
}
