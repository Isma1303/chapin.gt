# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project status

This repository currently contains no source code — only planning/spec documents under
`documentos de apoyo/`. There is no build system, no Flex/Bison files, and no `git` history yet.
Everything below describes what needs to be built, based on the language contract the team
already wrote (`documentos de apoyo/contexto de proyecto/Proyecto Compiladores (Grupo #5).pdf`).
When source files start getting created, update this doc with real build/run/test commands.

This is the final project for Curso Compiladores (3CT0113), Universidad San Pablo de Guatemala,
Grupo #5 (Ismael Liquez, Luis Franco). Each group builds a compiler front-end (+ optional codegen)
for its own toy language; this group's language is **Chapin** (file extension `.gt`).

## Intended architecture

Per the project's own "regla de diseño", the pipeline must keep source analysis and output
generation strictly separated so the target language can change without touching the analyzer:

```
Código fuente (.gt) -> Flex (tokens) -> Bison (gramática + AST) -> Validaciones semánticas
                                                                          |
                                                                          v
                                                        IR -> Generador de código C (C11)
```

- **Front-end** (Flex → Bison → AST → symbol table → semantic checks) is the required near-term
  deliverable (see "Entrega parcial — Semana 11" in `entregable_1.JPG`): a working `.l` file, a
  working `.y` file, Flex+Bison integration, an initial AST for the main constructs, a basic
  symbol table, and one valid + one invalid test program, plus a README with build/run instructions.
- **Back-end** (IR → C code generator) is a later, independent module — do not couple codegen
  logic into the parser/AST layer.

## The Chapin (.gt) language contract

Chapin is a small imperative/structured educational language whose reserved words are Guatemalan
slang ("chapinismos"). Full contract: `documentos de apoyo/contexto de proyecto/Proyecto Compiladores (Grupo #5).pdf`.

### Reserved words

| Word | Meaning |
|---|---|
| `va` | variable declaration |
| `pisto` | int type |
| `quetzal` | real/float type |
| `aguas` | print a prompt string |
| `clavar` | read input into a variable |
| `shute` | print/show an expression |
| `brega` | `while` loop |
| `cabal` | `if` |
| `sino` | `else` |

Only two base types exist: `pisto` (integer) and `quetzal` (real). Identifiers can't start with a
digit or collide with a reserved word.

### Operators & precedence (lowest to highest)

1. Relational, non-associative: `< > <= >= == !=`
2. `+ -` (binary), left-associative
3. `* /`, left-associative
4. `-` (unary), right-associative
5. `( )`

String concatenation is done via `expr -> expr PLUS CADENA` (e.g. `shute("Vuelta: " + contador)`);
this is intentionally resolved by semantic/type checking in Bison actions, not baked into the pure
grammar.

### Grammar (BNF, from the contract)

```
programa    -> lista_stmt
lista_stmt  -> stmt lista_stmt | ε
stmt        -> decl_stmt | asig_stmt | io_stmt | if_stmt | while_stmt
tipo        -> PISTO | QUETZAL
decl_stmt   -> VA tipo ID SEMI | VA tipo ID ASSIGN expr SEMI
asig_stmt   -> ID ASSIGN expr SEMI
io_stmt     -> AGUAS CADENA SEMI | CLAVAR LPAREN ID RPAREN SEMI | SHUTE LPAREN expr RPAREN SEMI
if_stmt     -> CABAL LPAREN cond RPAREN bloque
             | CABAL LPAREN cond RPAREN bloque SINO bloque
while_stmt  -> BREGA LPAREN cond RPAREN bloque
bloque      -> LBRACE lista_stmt RBRACE
cond        -> expr op_rel expr
op_rel      -> LT | GT | LE | GE | EQ | NE
expr        -> expr PLUS termino | expr MINUS termino | expr PLUS CADENA | termino
termino     -> termino TIMES factor | termino DIVIDE factor | factor
factor      -> LPAREN expr RPAREN | ID | ENTERO | REAL | CADENA | MINUS factor
```

Note the grammar as written is left-recursive (`expr -> expr PLUS termino`, etc.) — that's fine
for Bison (LALR handles left recursion natively) and matches the associativity table above, so
don't "fix" it into right recursion when implementing the `.y` file.

### Lexical patterns (for the Flex `.l` file)

```
DIGITO       [0-9]
LETRA        [a-zA-Z_]
ID           {LETRA}({LETRA}|{DIGITO})*
ENTERO       {DIGITO}+
REAL         {DIGITO}+"."{DIGITO}+
CADENA       \"([^\"\n])*\"
ESPACIO      [ \t\r]+
COMENTARIO_1L   "//".*
COMENTARIO_ML   "/*"([^*]|\*+[^*/])*\*+"/"
```

Reserved words must be matched before the generic `ID` rule (longest match / rule order in Flex
handles this automatically since exact-string rules for keywords are listed first).

### Reference example program

```
va pisto contador = 0;
va pisto limite;
va quetzal promedio = 0.0;

aguas "¿Hasta qué número quieres bregar?";
clavar(limite);

brega (contador < limite) {
    shute("Vuelta: " + contador);
    promedio = promedio + contador;
    contador = contador + 1;
}

promedio = promedio / limite;

cabal (promedio > 5) {
    shute("Promedio alto, quedó cabal: " + promedio);
} sino {
    shute("Promedio bajo: " + promedio);
}
```

Use this (or a trimmed version) as the "valid program" test fixture required for the Semana 11
deliverable; pair it with a deliberately broken variant (missing `;`, unbalanced braces, `shute`
without parens, etc. — six such cases are cataloged in the contract PDF) as the "invalid program"
fixture.
