%{

#include "tokenhelper.h"

%}

%option 8bit
%option debug
%option nodefault
%option noinput
%option nounput
%option noyywrap
%option verbose
%option warn

WS              [ \t\r]
BLOCK           ((.|{WS}|\n)*)
LETTER          [a-zA-Z_']
DIGIT           [0-9]
INT             ({DIGIT}{DIGIT}*)
FLOAT           ({INT}\.{INT})
HEX             ([0-9a-fA-F])
CHAR            ('([^\\\n']|\\{HEX}{HEX}|\\[\\n'])')
STRING          (\"([^\\\n"]|\\{HEX}{HEX}|\\[\\n\"])*\")
IDENT           ([a-z]({LETTER}|{DIGIT})*)

%%

({WS}|\n)       {  }
"--"(.|{WS})*   {  }
"//"(.|{WS})*   {  }
"/*"{BLOCK}"*/" {  }
"/@"            { return tokenize (TOK_NATIVE_START); }
"@/"            { return tokenize (TOK_NATIVE_END); }

[A-Z][a-z]*     { return tokenize (TOK_TYPE); }
"::"            { return tokenize (TOK_TYPEOP); }
"{"             { return tokenize (TOK_START_OBJ); }
"}"             { return tokenize (TOK_END_OBJ); }

{INT}           { return tokenize (TOK_INT); }
{FLOAT}         { return tokenize (TOK_FLOAT); }
{CHAR}          { return tokenize (TOK_CHAR); }
{STRING}        { return tokenize (TOK_STR); }

"let"           { return tokenize (TOK_LET); }
"type"          { return tokenize (TOK_TYPEDEC); }

"if"            { return tokenize (TOK_IF); }
"then"          { return tokenize (TOK_THEN); }
"else"          { return tokenize (TOK_ELSE); }

"false"         { return tokenize (TOK_FALSE); }
"true"          { return tokenize (TOK_TRUE); }

{IDENT}         { return tokenize (TOK_IDENT); }

"++"            { return tokenize (TOK_CONCAT); }
"!!"            { return tokenize (TOK_LIST_ACCESS); }
"[]"            { return tokenize (TOK_EMPTY_LIST); }
"["             { return tokenize (TOK_START_LIST); }
"]"             { return tokenize (TOK_END_LIST); }
":"             { return tokenize (TOK_LIST_PREPEND); }
","             { return tokenize (TOK_COMMA); }

"&&"            { return tokenize (TOK_AND); }
"||"            { return tokenize (TOK_OR); }

"=="            { return tokenize (TOK_EQUALS); }
"!="            { return tokenize (TOK_NOT_EQUALS); }
"<"             { return tokenize (TOK_LESS_THAN); }
"<="            { return tokenize (TOK_LESS_THAN_OR_EQ); }
">"             { return tokenize (TOK_GREATER_THAN); }
">="            { return tokenize (TOK_GREATER_THAN_OR_EQ); }

"="             { return tokenize (TOK_ASSIGN); }
"("             { return tokenize (TOK_OPEN_PAREN); }
")"             { return tokenize (TOK_CLOSE_PAREN); }
"+"				{ return tokenize (TOK_ADD); }
"-"             { return tokenize (TOK_SUB); }
"*"             { return tokenize (TOK_MUL); }
"/"             { return tokenize (TOK_DIV); }
"%"             { return tokenize (TOK_MOD); }
"!"             { return tokenize (TOK_NOT); }
"^"             { return tokenize (TOK_EXP); }
"."             { return tokenize (TOK_OBJ_ACCESS); }


.               { invalid_token (yytext); }


%%

