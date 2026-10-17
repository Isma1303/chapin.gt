# AGENTS.md

Compilador front-end del lenguaje Chapin (`.gt`): Flex + Bison + C, binario `./chapin`. No es repo git.

## Comandos

- `make` — compila: bison genera `build/parser.tab.{c,h}`, flex genera `build/lex.yy.c`, luego enlaza con `src/{ast,symtab,codegen,main}.c` → `./chapin`
- `make test` — corre `tests/valido.gt` (debe aceptar) y `tests/invalido.gt` (debe fallar; el `-` prefijo en el Makefile tolera su exit code 1)
- `make clean` — borra `build/`, `chapin`, `chapin.dSYM`
- Un solo test: `./chapin tests/valido.gt` (si no hay errores, además escribe `nova.nova` en el cwd)
- Validar la traducción con el compilador real de Nova: compilar `Assets/Proyecto-Nova` y correr `./nova ../../nova.nova`

## Gotchas

- `bison` reporta `conflicts: 10 reduce/reduce` — **esperado**, documentado en README; viene de `expr -> expr PLUS CADENA` vs `factor -> CADENA` y se resuelve después en validaciones semánticas, no en la gramática. No "arreglar".
- `build/` está versionado/presente en el árbol pero es artefacto generado; no editar a mano. `lexer.l` depende de `build/parser.tab.h`, así que tras tocar `parser.y` corre `make` completo.
- macOS: el `bison`/`flex` del sistema (2.3) alcanza. Si usas Homebrew, `brew install flex bison` instala versiones keg-only: hay que poner `/opt/homebrew/opt/{bison,flex}/bin` (o `/usr/local/...` en Intel) en `PATH` antes de que `make` las use.
- `main.c` corre `yyparse()` aunque haya errores y solo al final imprime AST + tabla; el exit code es 1 si `errores_lexicos + errores_sintacticos + errores_semanticos > 0`. Validaciones semánticas actuales: variable duplicada y variable no declarada. La traducción a `nova.nova` ocurre **después** del chequeo, así que no se genera archivo si hay errores.
- La gramática de Nova (`Assets/Proyecto-Nova/parser.y`) impone: declaraciones **antes** de cualquier sentencia, sin inicializador (`tipo id;`), y bloques cerrados con `entonces/sino/fin` o `hacer/fin` (sin llaves). `codegen.c` iza las declaraciones y separa la inicialización en una asignación. Nova **no tiene `clavar`** (se omite con aviso) ni `aguas` (se traduce a `imprimir`).
- `Assets/Proyecto-Nova` usa `%code requires`, que exige **bison 3.x**; el 2.3 del sistema falla al compilarlo. Para validar la traducción hace falta un bison moderno (Homebrew).

## Estructura

- `src/lexer.l`, `src/parser.y` — tokens y gramática; la gramática llena el AST y la tabla de símbolos directamente
- `src/ast.{h,c}`, `src/symtab.{h,c}`, `src/codegen.{h,c}`, `src/main.c`
- `src/codegen.c` — traduce el AST a Nova y escribe `nova.nova` (solo si no hay errores); ver README sección 4
- `tests/valido.gt`, `tests/invalido.gt`, `tests/traduccion.gt` — fixtures; cualquier cambio de gramática debe mantener el comportamiento de los dos primeros

## Notas

- README referencia `CLAUDE.md`, `documentos de apoyo/` y `contexto de proyecto/` — esos archivos **no existen** en este árbol; no buscar.
- Chequeo de tipos completo (p. ej. `pisto` + `CADENA`) es trabajo futuro, según el propio README; no implementarlo esperando que la gramática lo distinga (ver nota sobre `Operacion '+'` vs `Concat`).
