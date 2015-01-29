%{
#include "gram.h"
%}

%option reentrant
%option noyywrap
%option noinput
%option nounput

%%

\"(\\.|[^"])*\"         return STRING;
"null"                  return JNULL;
"true"                  return BOOLEAN;
"false"                 return BOOLEAN;
-?[0-9]+(\.[0-9]+)?     return NUMBER;
[ \t\n]+                ;
"["                     return LBRACKET;
"]"                     return RBRACKET;
"{"                     return LBRACE;
"}"                     return RBRACE;
":"                     return COLON;
","                     return COMMA;
.                       return -1;

%%
