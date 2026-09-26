/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     VA = 258,
     PISTO = 259,
     QUETZAL = 260,
     AGUAS = 261,
     CLAVAR = 262,
     SHUTE = 263,
     BREGA = 264,
     CABAL = 265,
     SINO = 266,
     ASSIGN = 267,
     PLUS = 268,
     MINUS = 269,
     TIMES = 270,
     DIVIDE = 271,
     LT = 272,
     GT = 273,
     LE = 274,
     GE = 275,
     EQ = 276,
     NE = 277,
     LPAREN = 278,
     RPAREN = 279,
     LBRACE = 280,
     RBRACE = 281,
     SEMI = 282,
     COMMA = 283,
     ID = 284,
     CADENA = 285,
     ENTERO = 286,
     REAL = 287,
     UMINUS = 288
   };
#endif
/* Tokens.  */
#define VA 258
#define PISTO 259
#define QUETZAL 260
#define AGUAS 261
#define CLAVAR 262
#define SHUTE 263
#define BREGA 264
#define CABAL 265
#define SINO 266
#define ASSIGN 267
#define PLUS 268
#define MINUS 269
#define TIMES 270
#define DIVIDE 271
#define LT 272
#define GT 273
#define LE 274
#define GE 275
#define EQ 276
#define NE 277
#define LPAREN 278
#define RPAREN 279
#define LBRACE 280
#define RBRACE 281
#define SEMI 282
#define COMMA 283
#define ID 284
#define CADENA 285
#define ENTERO 286
#define REAL 287
#define UMINUS 288




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 34 "src/parser.y"
{
    int ival;
    double dval;
    char *str;
    NodoAST *nodo;
}
/* Line 1529 of yacc.c.  */
#line 122 "build/parser.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

