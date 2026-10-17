/*
 * lexer.l - Analizador lexico del lenguaje Chapin (.gt)
 *
 * Patrones tomados literalmente del contrato del lenguaje en
 * "contexto de proyecto/Proyecto Compiladores (Grupo #5).pdf", seccion 3.
 */

%{
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "parser.tab.h"
%}

%option noyywrap
%option yylineno

DIGITO      [0-9]
LETRA       [a-zA-Z_]
ID          {LETRA}({LETRA}|{DIGITO})*
ENTERO      {DIGITO}+
REAL        {DIGITO}+"."{DIGITO}+
CADENA      \"([^\"\n])*\"
ESPACIO     [ \t\r]+

%%

"//".*                       { /* comentario de una linea: se descarta */ }
"/*"([^*]|\*+[^*/])*\*+"/"    { /* comentario multilinea: se descarta */ }

"va"        { return VA; }
"pisto"     { return PISTO; }
"quetzal"   { return QUETZAL; }
"aguas"     { return AGUAS; }
"clavar"    { return CLAVAR; }
"shute"     { return SHUTE; }
"brega"     { return BREGA; }
"cabal"     { return CABAL; }
"sino"      { return SINO; }

{ID}        { yylval.str = strdup(yytext); return ID; }
{REAL}      { yylval.dval = atof(yytext); return REAL; }
{ENTERO}    { yylval.ival = atoi(yytext); return ENTERO; }
{CADENA}    {
                /* quita las comillas antes de guardar la cadena */
                yytext[yyleng - 1] = '\0';
                yylval.str = strdup(yytext + 1);
                return CADENA;
            }

"<="        { return LE; }
">="        { return GE; }
"=="        { return EQ; }
"!="        { return NE; }
"="         { return ASSIGN; }
"+"         { return PLUS; }
"-"         { return MINUS; }
"*"         { return TIMES; }
"/"         { return DIVIDE; }
"<"         { return LT; }
">"         { return GT; }

"("         { return LPAREN; }
")"         { return RPAREN; }
"{"         { return LBRACE; }
"}"         { return RBRACE; }
";"         { return SEMI; }
","         { return COMMA; }

{ESPACIO}   { /* se ignora */ }
\n          { /* %option yylineno ya incrementa el contador de linea */ }

.           {
                fprintf(stderr, "Error lexico en linea %d: caracter inesperado '%s'\n", yylineno, yytext);
                errores_lexicos++;
            }

%%
