/*
 * codegen.c - Traduccion Chapin (.gt) -> Nova (.nova)
 *
 * La gramatica de Nova impone que TODAS las declaraciones aparezcan antes de
 * cualquier sentencia (programa : declaraciones sentencias) y prohibe
 * inicializar en la declaracion (declaracion : tipo ID ';'). Chapin, en
 * cambio, permite intercalar declaraciones (incluso dentro de bloques) y
 * acepta `va tipo id = expr;`. Como la tabla de simbolos de Chapin es global
 * (sin ambitos), la traduccion:
 *
 *   1. iza todas las declaraciones al inicio del programa Nova, y
 *   2. convierte las declaraciones inicializadas en una asignacion en su
 *      posicion original.
 *
 * Constructos sin equivalente directo en Nova:
 *   - `aguas "texto";`  -> `imprimir("texto");`
 *   - `clavar(x);`      -> comentario `// ...` + aviso en stderr (se omite),
 *                          porque Nova no tiene lectura por teclado.
 */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"

static const char *tipo_nova(TipoDato t) {
    return t == TIPO_PISTO ? "entero" : "decimal";
}

static void emitir_indent(FILE *f, int nivel) {
    for (int i = 0; i < nivel; i++) fputs("    ", f);
}

/* ---------- Pasada 1: recolectar e izar declaraciones ---------- */

static void recolectar_decls(NodoAST *nodo, FILE *f) {
    if (!nodo) return;
    switch (nodo->tipo) {
        case NODO_PROGRAMA:
        case NODO_BLOQUE:
            for (int i = 0; i < nodo->num_hijos; i++)
                recolectar_decls(nodo->hijos[i], f);
            break;
        case NODO_DECL:
            fprintf(f, "%s %s;\n", tipo_nova(nodo->tipo_dato), nodo->nombre);
            break;
        case NODO_IF:
            recolectar_decls(nodo->cuerpo, f);
            recolectar_decls(nodo->cuerpo_sino, f);
            break;
        case NODO_WHILE:
            recolectar_decls(nodo->cuerpo, f);
            break;
        default:
            break;
    }
}

/* ---------- Emision de expresiones ---------- */

static void emitir_expr(NodoAST *nodo, FILE *f);

/*
 * Emite un operando. El AST de Chapin descarta los parentesis originales, asi
 * que reconstruimos la precedencia envolviendo las subexpresiones compuestas
 * (operadores binarios/relacionales, concatenacion y negacion).
 */
static void emitir_operando(NodoAST *nodo, FILE *f) {
    if (!nodo) return;
    switch (nodo->tipo) {
        case NODO_OP_BINARIA:
        case NODO_OP_RELACIONAL:
        case NODO_CONCAT:
        case NODO_NEGACION:
            fputc('(', f);
            emitir_expr(nodo, f);
            fputc(')', f);
            break;
        default:
            emitir_expr(nodo, f);
            break;
    }
}

/* Nova exige decimales con punto: DIGITO+ "." DIGITO+ */
static void emitir_real(FILE *f, double v) {
    char buf[64];
    snprintf(buf, sizeof buf, "%.10g", v);
    if (!strchr(buf, '.') && !strchr(buf, 'e') && !strchr(buf, 'E'))
        strcat(buf, ".0");
    fputs(buf, f);
}

/* Nova reconoce cadenas con comillas y escapes \" y \\ */
static void emitir_cadena(FILE *f, const char *s) {
    fputc('"', f);
    for (const char *p = s; p && *p; p++) {
        if (*p == '"' || *p == '\\') fputc('\\', f);
        fputc(*p, f);
    }
    fputc('"', f);
}

static void emitir_expr(NodoAST *nodo, FILE *f) {
    if (!nodo) return;
    switch (nodo->tipo) {
        case NODO_OP_BINARIA:
        case NODO_OP_RELACIONAL:
            emitir_operando(nodo->izq, f);
            fprintf(f, " %s ", nodo->operador);
            emitir_operando(nodo->der, f);
            break;
        case NODO_CONCAT:
            emitir_operando(nodo->izq, f);
            fputs(" + ", f);
            emitir_operando(nodo->der, f);
            break;
        case NODO_NEGACION:
            fputc('-', f);
            emitir_operando(nodo->expr, f);
            break;
        case NODO_ID:
            fputs(nodo->nombre, f);
            break;
        case NODO_ENTERO:
            fprintf(f, "%d", nodo->val_entero);
            break;
        case NODO_REAL:
            emitir_real(f, nodo->val_real);
            break;
        case NODO_CADENA:
            emitir_cadena(f, nodo->val_cadena);
            break;
        default:
            break;
    }
}

/* ---------- Pasada 2: emitir sentencias ---------- */

static void emitir_sentencias(NodoAST *nodo, FILE *f, int nivel) {
    if (!nodo) return;
    switch (nodo->tipo) {
        case NODO_PROGRAMA:
        case NODO_BLOQUE:
            for (int i = 0; i < nodo->num_hijos; i++)
                emitir_sentencias(nodo->hijos[i], f, nivel);
            break;
        case NODO_DECL:
            /* La declaracion ya se izo; aqui solo va la inicializacion. */
            if (nodo->expr) {
                emitir_indent(f, nivel);
                fprintf(f, "%s = ", nodo->nombre);
                emitir_expr(nodo->expr, f);
                fputs(";\n", f);
            }
            break;
        case NODO_ASIGNACION:
            emitir_indent(f, nivel);
            fprintf(f, "%s = ", nodo->nombre);
            emitir_expr(nodo->expr, f);
            fputs(";\n", f);
            break;
        case NODO_AGUAS:
            emitir_indent(f, nivel);
            fputs("imprimir(", f);
            emitir_cadena(f, nodo->val_cadena);
            fputs(");\n", f);
            break;
        case NODO_CLAVAR:
            emitir_indent(f, nivel);
            fprintf(f, "// clavar(%s): no soportado en Nova\n", nodo->nombre);
            fprintf(stderr,
                    "Aviso: 'clavar(%s)' (linea %d) no tiene equivalente en Nova; se omitio.\n",
                    nodo->nombre, nodo->linea);
            break;
        case NODO_SHUTE:
            emitir_indent(f, nivel);
            fputs("imprimir(", f);
            emitir_expr(nodo->expr, f);
            fputs(");\n", f);
            break;
        case NODO_IF:
            emitir_indent(f, nivel);
            fputs("si ", f);
            emitir_expr(nodo->condicion, f);
            fputs(" entonces\n", f);
            emitir_sentencias(nodo->cuerpo, f, nivel + 1);
            if (nodo->cuerpo_sino) {
                emitir_indent(f, nivel);
                fputs("sino\n", f);
                emitir_sentencias(nodo->cuerpo_sino, f, nivel + 1);
            }
            emitir_indent(f, nivel);
            fputs("fin\n", f);
            break;
        case NODO_WHILE:
            emitir_indent(f, nivel);
            fputs("mientras ", f);
            emitir_expr(nodo->condicion, f);
            fputs(" hacer\n", f);
            emitir_sentencias(nodo->cuerpo, f, nivel + 1);
            emitir_indent(f, nivel);
            fputs("fin\n", f);
            break;
        default:
            break;
    }
}

int traducir_a_nova(NodoAST *raiz, const char *ruta) {
    FILE *f = fopen(ruta, "w");
    if (!f) return -1;

    /* 1. Declaraciones izadas al inicio del programa Nova. */
    recolectar_decls(raiz, f);

    /* Separador entre la seccion de declaraciones y la de sentencias. */
    fputc('\n', f);

    /* 2. Sentencias en su orden original. */
    emitir_sentencias(raiz, f, 0);

    if (fclose(f) != 0) return -1;
    return 0;
}
