# Chapin (.gt) — Entregable 1: Front-end (Flex + Bison)

Compilador front-end del lenguaje **Chapin** (extensión `.gt`), proyecto final del curso
Compiladores (3CT0113), Grupo #5. Este entregable corresponde a la **Entrega parcial — Semana 11**
descrita en `documentos de apoyo/entregable_1.JPG`:

- [x] Archivo Flex (`.l`) funcional — `src/lexer.l`
- [x] Archivo Bison (`.y`) funcional — `src/parser.y`
- [x] Integración Flex + Bison — `Makefile`
- [x] AST inicial para las construcciones principales — `src/ast.h`, `src/ast.c`
- [x] Tabla de símbolos básica — `src/symtab.h`, `src/symtab.c`
- [x] Un programa válido y uno inválido de prueba — `tests/valido.gt`, `tests/invalido.gt`
- [x] Este README con instrucciones de compilación y ejecución

El contrato completo del lenguaje (tokens, gramática BNF, precedencia) está en
`contexto de proyecto/Proyecto Compiladores (Grupo #5).pdf` y resumido en `CLAUDE.md`.

## 1. Instalación de Flex y Bison (macOS)

### Paso 0 — comprobar si ya los tienes

Abre una terminal y corre:

```bash
flex --version
bison --version
cc --version
```

Si los tres comandos responden con una versión (sin error de "command not found"), ya tienes lo
necesario y puedes saltar directo a la sección **2. Compilación**. En macOS, las Herramientas de
Línea de Comandos de Xcode ya incluyen `flex` y una versión antigua de `bison` (2.3), que **sí
alcanza para este proyecto** (la gramática de `src/parser.y` fue probada contra esa versión).

### Paso 1 — instalar las Herramientas de Línea de Comandos de Xcode (si el paso 0 falló)

```bash
xcode-select --install
```

Aparecerá una ventana del sistema pidiendo confirmar la instalación. Acéptala y espera a que
termine (puede tardar varios minutos). Al finalizar, repite las comprobaciones del Paso 0.

### Paso 2 (opcional, recomendado) — instalar una versión moderna de Bison con Homebrew

La `bison` que trae Xcode es la 2.3 (de 2006, por temas de licencia de Apple). Alcanza para este
entregable, pero si más adelante quieres mensajes de error más claros o funciones nuevas de Bison,
instala una versión moderna con [Homebrew](https://brew.sh):

```bash
# 2.1 Instalar Homebrew (si no lo tienes)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# 2.2 Instalar flex y bison actualizados
brew install flex bison
```

Homebrew instala `flex`/`bison` como *keg-only* (no reemplaza los del sistema automáticamente).
Al terminar, el propio instalador te muestra un mensaje como este — cópialo y agrégalo a tu shell:

```bash
# Apple Silicon (M1/M2/M3...):
echo 'export PATH="/opt/homebrew/opt/bison/bin:/opt/homebrew/opt/flex/bin:$PATH"' >> ~/.zshrc

# Intel:
echo 'export PATH="/usr/local/opt/bison/bin:/usr/local/opt/flex/bin:$PATH"' >> ~/.zshrc

# luego recarga la configuración:
source ~/.zshrc
```

Vuelve a correr `flex --version` y `bison --version`: ahora deberían mostrar la versión de
Homebrew (bison 3.x) en vez de la 2.3 del sistema.

## 2. Compilación

Desde la raíz del proyecto:

```bash
make
```

Esto genera:

1. `build/parser.tab.c` / `build/parser.tab.h` (Bison, a partir de `src/parser.y`)
2. `build/lex.yy.c` (Flex, a partir de `src/lexer.l`, ya integrado con los tokens de Bison)
3. El binario `./chapin`, enlazando lo anterior con `src/ast.c`, `src/symtab.c` y `src/main.c`

Al compilar `src/parser.y` verás un aviso de Bison:

```
src/parser.y: conflicts: 10 reduce/reduce
```

Esto es **esperado**: el propio contrato del lenguaje (sección 5, nota al pie) señala que la regla
`expr -> expr PLUS CADENA` (para permitir `shute("texto" + variable)`) genera un conflicto contra
`factor -> CADENA`, y que se resuelve fuera de la gramática pura, en la fase de validaciones
semánticas. No es un error de la gramática ni bloquea la compilación.

## 3. Ejecución

```bash
./chapin tests/valido.gt      # debe aceptar el programa e imprimir el AST + tabla de símbolos
./chapin tests/invalido.gt    # debe rechazarlo con un error sintáctico y línea exacta
```

O ambos de una vez:

```bash
make test
```

### Limpiar los artefactos generados

```bash
make clean
```

## 4. Estructura del proyecto

```
src/
  lexer.l       Analizador léxico (Flex): reconoce tokens, comentarios, cadenas, números
  parser.y      Gramática (Bison): construye el AST y llena la tabla de símbolos
  ast.h / ast.c Nodos del AST y su impresión en árbol indentado
  symtab.h/.c   Tabla de símbolos (lista enlazada: nombre, tipo, línea de declaración)
  main.c        Programa principal: abre el .gt, corre el parser, imprime AST y tabla
tests/
  valido.gt     Programa de ejemplo del contrato (sección 9) — debe compilar sin errores
  invalido.gt   Variante con un `;` faltante — debe rechazarse con error de sintaxis
```

## 5. Notas de diseño

- **Validaciones semánticas incluidas en este entregable:** declaración duplicada de una variable
  y uso de una variable no declarada (en asignaciones, `clavar(...)` y expresiones). Esto es lo
  mínimo que pide una "tabla de símbolos básica"; el chequeo de tipos completo (p. ej. no permitir
  sumar `pisto` con `CADENA` salvo en concatenación) queda para una fase posterior de validaciones
  semánticas, tal como indica el propio contrato del lenguaje.
- **Por qué `"texto" + variable` se imprime como `Operacion '+'` y no como `Concat` en el AST:**
  la regla `expr -> expr PLUS CADENA` de la gramática solo aplica cuando la cadena está a la
  *derecha* del `+` (p. ej. `variable + "texto"`). Cuando la cadena va a la izquierda (el caso más
  común, como en `shute("Vuelta: " + contador)`), la reduce por la regla aritmética genérica
  `expr -> expr PLUS termino`. Es el comportamiento correcto según el BNF del contrato; distinguir
  "suma numérica" de "concatenación" para *ambos* órdenes es, otra vez, trabajo de la fase de
  chequeo de tipos, no de la gramática.
