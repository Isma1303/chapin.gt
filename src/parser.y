/*
 * parser.y - Gramatica del lenguaje Chapin (.gt)
 *
 * Construye el AST y llena la tabla de simbolos mientras reconoce el
 * programa. La gramatica y las tablas de precedencia siguen el contrato
 * en documentos de apoyo/entregable_1.JPG y "contexto de proyecto/".
 */

%{
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "ast.h"
#include "symtab.h"

int yylex(void);
void yyerror(const char *msg);
extern int yylineno;

NodoAST *raiz_ast = NULL;

static void error_semantico(int linea, const char *fmt, ...) {
    va_list args;
    fprintf(stderr, "Error semantico en linea %d: ", linea);
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
    errores_semanticos++;
}
%}

%union {
    int ival;
    double dval;
    char *str;
    NodoAST *nodo;
}

%token VA PISTO QUETZAL AGUAS CLAVAR SHUTE BREGA CABAL SINO
%token ASSIGN PLUS MINUS TIMES DIVIDE
%token LT GT LE GE EQ NE
%token LPAREN RPAREN LBRACE RBRACE SEMI COMMA

%token <str> ID CADENA
%token <ival> ENTERO
%token <dval> REAL

%type <nodo> programa lista_stmt stmt decl_stmt asig_stmt io_stmt
%type <nodo> if_stmt while_stmt bloque cond expr termino factor
%type <ival> tipo

/* Precedencia de menor a mayor (misma tabla del contrato del lenguaje) */
%nonassoc LT GT LE GE EQ NE
%left PLUS MINUS
%left TIMES DIVIDE
%right UMINUS

%error-verbose

%%

programa:
      lista_stmt { $1->tipo = NODO_PROGRAMA; raiz_ast = $1; }
    ;

lista_stmt:
      lista_stmt stmt { agregar_hijo($1, $2); $$ = $1; }
    | /* vacio */      { $$ = crear_lista(NODO_BLOQUE, yylineno); }
    ;

stmt:
      decl_stmt
    | asig_stmt
    | io_stmt
    | if_stmt
    | while_stmt
    ;

tipo:
      PISTO   { $$ = TIPO_PISTO; }
    | QUETZAL { $$ = TIPO_QUETZAL; }
    ;

decl_stmt:
      VA tipo ID SEMI {
          if (!tabla_declarar($3, (TipoDato) $2, yylineno))
              error_semantico(yylineno, "la variable '%s' ya habia sido declarada", $3);
          $$ = crear_decl(yylineno, (TipoDato) $2, $3, NULL);
      }
    | VA tipo ID ASSIGN expr SEMI {
          if (!tabla_declarar($3, (TipoDato) $2, yylineno))
              error_semantico(yylineno, "la variable '%s' ya habia sido declarada", $3);
          $$ = crear_decl(yylineno, (TipoDato) $2, $3, $5);
      }
    ;

asig_stmt:
      ID ASSIGN expr SEMI {
          if (!tabla_buscar($1))
              error_semantico(yylineno, "la variable '%s' no ha sido declarada", $1);
          $$ = crear_asignacion(yylineno, $1, $3);
      }
    ;

io_stmt:
      AGUAS CADENA SEMI {
          $$ = crear_aguas(yylineno, $2);
      }
    | CLAVAR LPAREN ID RPAREN SEMI {
          if (!tabla_buscar($3))
              error_semantico(yylineno, "la variable '%s' no ha sido declarada", $3);
          $$ = crear_clavar(yylineno, $3);
      }
    | SHUTE LPAREN expr RPAREN SEMI {
          $$ = crear_shute(yylineno, $3);
      }
    ;

if_stmt:
      CABAL LPAREN cond RPAREN bloque {
          $$ = crear_if(yylineno, $3, $5, NULL);
      }
    | CABAL LPAREN cond RPAREN bloque SINO bloque {
          $$ = crear_if(yylineno, $3, $5, $7);
      }
    ;

while_stmt:
      BREGA LPAREN cond RPAREN bloque {
          $$ = crear_while(yylineno, $3, $5);
      }
    ;

bloque:
      LBRACE lista_stmt RBRACE { $$ = $2; }
    ;

cond:
      expr LT expr { $$ = crear_op_relacional(yylineno, "<",  $1, $3); }
    | expr GT expr { $$ = crear_op_relacional(yylineno, ">",  $1, $3); }
    | expr LE expr { $$ = crear_op_relacional(yylineno, "<=", $1, $3); }
    | expr GE expr { $$ = crear_op_relacional(yylineno, ">=", $1, $3); }
    | expr EQ expr { $$ = crear_op_relacional(yylineno, "==", $1, $3); }
    | expr NE expr { $$ = crear_op_relacional(yylineno, "!=", $1, $3); }
    ;

/*
 * NOTA sobre "expr PLUS CADENA": el propio contrato del lenguaje
 * (Proyecto Compiladores, seccion 5) advierte que esta regla introduce
 * un conflicto reduce/reduce contra "factor -> CADENA" y que se resuelve
 * fuera de la gramatica pura. Bison lo resuelve a favor de esta regla
 * (aparece primero en el archivo => menor numero de regla), por lo que
 * `shute("Vuelta: " + contador)` se etiqueta como NODO_CONCAT en el AST.
 * El conflicto reportado por bison -Wconflicts-sr/-Wconflicts-rr aqui es
 * esperado e intencional, no un error de la gramatica.
 */
expr:
      expr PLUS termino  { $$ = crear_op_binaria(yylineno, "+", $1, $3); }
    | expr MINUS termino { $$ = crear_op_binaria(yylineno, "-", $1, $3); }
    | expr PLUS CADENA   { $$ = crear_concat(yylineno, $1, crear_cadena(yylineno, $3)); }
    | termino            { $$ = $1; }
    ;

termino:
      termino TIMES factor  { $$ = crear_op_binaria(yylineno, "*", $1, $3); }
    | termino DIVIDE factor { $$ = crear_op_binaria(yylineno, "/", $1, $3); }
    | factor                { $$ = $1; }
    ;

factor:
      LPAREN expr RPAREN { $$ = $2; }
    | ID {
          if (!tabla_buscar($1))
              error_semantico(yylineno, "la variable '%s' no ha sido declarada", $1);
          $$ = crear_id(yylineno, $1);
      }
    | ENTERO { $$ = crear_entero(yylineno, $1); }
    | REAL   { $$ = crear_real(yylineno, $1); }
    | CADENA { $$ = crear_cadena(yylineno, $1); }
    | MINUS factor %prec UMINUS { $$ = crear_negacion(yylineno, $2); }
    ;

%%

void yyerror(const char *msg) {
    fprintf(stderr, "Error sintactico en linea %d: %s\n", yylineno, msg);
    errores_sintacticos++;
}
