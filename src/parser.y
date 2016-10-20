%{

#include "tokenhelper.h"
#include "parserhelper.h"
#include "tokenizer.h"

%}

%debug
%defines
%error-verbose
%token-table
%verbose

%token TOK_TYPE TOK_TYPEOP TOK_INT TOK_FLOAT TOK_CHAR
%token TOK_STR TOK_LET TOK_IF TOK_THEN TOK_TYPEDEC
%token TOK_ASSIGN TOK_FALSE TOK_TRUE
%right TOK_OPEN_PAREN TOK_IDENT
%token TOK_START_OBJ TOK_END_OBJ TOK_EMPTY_LIST
%token TOK_START_LIST TOK_END_LIST TOK_COMMA 
%token TOK_CLOSE_PAREN
%token TOK_NATIVE_START TOK_NATIVE_END
%right TOK_ELSE
%left TOK_CONCAT
%left TOK_OBJ_ACCESS
%right TOK_LIST_PREPEND
%right TOK_NOT
%left TOK_AND TOK_OR
%left TOK_EQUALS TOK_NOT_EQUALS TOK_LESS_THAN TOK_LESS_THAN_OR_EQ TOK_GREATER_THAN TOK_GREATER_THAN_OR_EQ 
%left TOK_ADD TOK_SUB
%left TOK_MUL TOK_DIV TOK_MOD
%left TOK_EXP
%left TOK_LIST_ACCESS


%start file

%%
file        : 
            | line file
            ;
line        : declaration
            | typeDecl
            | rootExpr
            ;
declaration : TOK_LET TOK_IDENT paramlist TOK_ASSIGN expression { insert_global($2, $3, $5); }
            | TOK_LET TOK_IDENT paramlist TOK_ASSIGN native     { insert_native($2, $3, $5); }
            ;
typeDecl    : TOK_TYPEDEC TOK_TYPE TOK_ASSIGN type              { declType($2,$4); }
            ;
rootExpr    : expression                                        { execute($1); }
            ;
native      : TOK_NATIVE_START TOK_IDENT TOK_NATIVE_END         { $$ = $2; }
            | TOK_NATIVE_START TOK_TYPE TOK_NATIVE_END          { $$ = $2; }
            ;
paramlist   : TOK_OPEN_PAREN params                             { $$ = adoptChildren($1,$2); }
            ;
params      : TOK_CLOSE_PAREN                                   { $$ = makeNode($1); }
            | identifier TOK_CLOSE_PAREN                        { $$ = makeNode($2); adopt($$,$1); }
            | identifier TOK_COMMA params                       { $$ = adopt($3,$1); }
            ;
identifier  : TOK_IDENT                                         { $$ = $1; }
            | TOK_IDENT TOK_TYPEOP type                         { $$ = adopt($1,$3); }
            ;
type        : TOK_TYPE                                          { $$ = $1; }
            | TOK_START_OBJ objTypes                            { $$ = adoptChildren($1,$2); }
            | TOK_START_LIST type TOK_END_LIST                  { $$ = adopt($1,$2); }
            | type TOK_OR type                                  { adopt($2,$1); $$ = adopt($2,$3); }
            ;
objTypes    : TOK_END_OBJ                                       { $$ = makeNode($1); }
            | identifier TOK_END_OBJ                            { $$ = makeNode($2); adopt($$,$1); }
            | identifier TOK_COMMA objTypes                     { $$ = adopt($3,$1); }
            ;
arglist     : TOK_OPEN_PAREN args                               { $$ = adoptChildren($1,$2); }
            ;
args        : TOK_CLOSE_PAREN                                   { $$ = makeNode($1); }
            | expression TOK_CLOSE_PAREN                        { $$ = makeNode($2); adopt($$,$1); }
            | expression TOK_COMMA args                         { $$ = adopt($3,$1); }
            ;
expression  : arithmetic                                        { $$ = $1; }
            | value                                             { $$ = $1; }
            | call                                              { $$ = $1; }
            | if                                                { $$ = $1; }
            | TOK_OPEN_PAREN expression TOK_CLOSE_PAREN         { $$ = adopt($1,$2); }
            | TOK_IDENT                                         { $$ = $1; }
            ;
if          : TOK_IF expression TOK_THEN expression TOK_ELSE expression { adopt($1, $2); adopt($1,$4); $$=adopt($1,$6); }
            ;
call        : TOK_IDENT arglist                                 { $$ = adopt($1,$2); }
            ;
arithmetic  : expression TOK_OR expression                      { adopt($2,$1); $$ = adopt($2,$3); }
            | expression TOK_AND expression                     { adopt($2,$1); $$ = adopt($2,$3); }
            | expression TOK_EQUALS expression                  { adopt($2,$1); $$ = adopt($2,$3); }
            | expression TOK_NOT_EQUALS expression              { adopt($2,$1); $$ = adopt($2,$3); }
            | expression TOK_LESS_THAN expression               { adopt($2,$1); $$ = adopt($2,$3); }
            | expression TOK_LESS_THAN_OR_EQ expression         { adopt($2,$1); $$ = adopt($2,$3); }
            | expression TOK_GREATER_THAN expression            { adopt($2,$1); $$ = adopt($2,$3); }
            | expression TOK_GREATER_THAN_OR_EQ expression      { adopt($2,$1); $$ = adopt($2,$3); }
            | expression TOK_ADD expression                     { adopt($2,$1); $$ = adopt($2,$3); }
            | expression TOK_SUB expression                     { adopt($2,$1); $$ = adopt($2,$3); }
            | expression TOK_MUL expression                     { adopt($2,$1); $$ = adopt($2,$3); }
            | expression TOK_DIV expression                     { adopt($2,$1); $$ = adopt($2,$3); }
            | expression TOK_MOD expression                     { adopt($2,$1); $$ = adopt($2,$3); }
            | expression TOK_EXP expression                     { adopt($2,$1); $$ = adopt($2,$3); }
            | expression TOK_CONCAT expression                  { adopt($2,$1); $$ = adopt($2,$3); }
            | expression TOK_LIST_PREPEND expression            { adopt($2,$3); $$ = adopt($2,$1); }
            | expression TOK_LIST_ACCESS expression             { adopt($2,$1); $$ = adopt($2,$3); }
            | TOK_NOT expression                                { $$ = adopt($1, $2); }
            ;
value       : list                                              { $$ = $1; }
            | TOK_INT                                           { $$ = $1; }
            | TOK_CHAR                                          { $$ = $1; }
            | TOK_FLOAT                                         { $$ = $1; }
            | TOK_TRUE                                          { $$ = $1; }
            | TOK_FALSE                                         { $$ = $1; }
            | object                                            { $$ = $1; }
            | expression TOK_OBJ_ACCESS TOK_IDENT               { adopt($2,$1); $$ = adopt($2,$3); }
            ;
list        : TOK_STR                                           { $$ = makeString($1); }
            | TOK_EMPTY_LIST                                    { $$ = $1; }
            | TOK_START_LIST listLiteral                        { $$ = adopt($1,$2); }
            ;
listLiteral : TOK_END_LIST                                      { $$ = $1; }
            | expression TOK_END_LIST                           { $$ = makeList($2,$1); }
            | expression TOK_COMMA listLiteral                  { adopt($2,$3); $$ = adopt($2,$1);}
            ;
object      : TOK_START_OBJ fields                              { $$ = adoptChildren($1,$2); }
            ;
fields      : TOK_END_OBJ                                       { $$ = makeNode($1); }
            | field TOK_END_OBJ                                 { $$ = makeNode($2); adopt($$,$1); }
            | field TOK_COMMA fields                            { $$ = adopt($3,$1); }
            ;
field       : TOK_IDENT TOK_ASSIGN expression                   { $$ = adopt($1,$3); }
            ;
%%