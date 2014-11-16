%{
#include "gram.h"
%}

%option reentrant
%option noyywrap
%option noinput
%option nounput

%%

[a-zA-Z][a-zA-Z0-9]*    return IDENTIFIER;
-?[0-9]+(\.[0-9]+)?     return NUMBER;
[ \t\n]+                ;
"["                     return LBRACKET;
"]"                     return RBRACKET;
"{"                     return RBRACE;
"}"                     return RBRACE;
":"                     return COLON;
","                     return COMMA;
%%
