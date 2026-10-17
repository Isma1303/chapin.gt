#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

int errores_lexicos = 0;
int errores_sintacticos = 0;
int errores_semanticos = 0;

static NodoAST *nodo_base(TipoNodo tipo, int linea) {
    NodoAST *n = calloc(1, sizeof(NodoAST));
    n->tipo = tipo;
    n->linea = linea;
    return n;
}

NodoAST *crear_lista(TipoNodo tipo, int linea) {
    NodoAST *n = nodo_base(tipo, linea);
    n->cap_hijos = 4;
    n->hijos = malloc(sizeof(NodoAST *) * n->cap_hijos);
    return n;
}

void agregar_hijo(NodoAST *lista, NodoAST *hijo) {
    if (lista->num_hijos == lista->cap_hijos) {
        lista->cap_hijos *= 2;
        lista->hijos = realloc(lista->hijos, sizeof(NodoAST *) * lista->cap_hijos);
    }
    lista->hijos[lista->num_hijos++] = hijo;
}

NodoAST *crear_decl(int linea, TipoDato tipo, char *nombre, NodoAST *expr) {
    NodoAST *n = nodo_base(NODO_DECL, linea);
    n->tipo_dato = tipo;
    n->nombre = nombre;
    n->expr = expr;
    return n;
}

NodoAST *crear_asignacion(int linea, char *nombre, NodoAST *expr) {
    NodoAST *n = nodo_base(NODO_ASIGNACION, linea);
    n->nombre = nombre;
    n->expr = expr;
    return n;
}

NodoAST *crear_aguas(int linea, char *cadena) {
    NodoAST *n = nodo_base(NODO_AGUAS, linea);
    n->val_cadena = cadena;
    return n;
}

NodoAST *crear_clavar(int linea, char *nombre) {
    NodoAST *n = nodo_base(NODO_CLAVAR, linea);
    n->nombre = nombre;
    return n;
}

NodoAST *crear_shute(int linea, NodoAST *expr) {
    NodoAST *n = nodo_base(NODO_SHUTE, linea);
    n->expr = expr;
    return n;
}

NodoAST *crear_if(int linea, NodoAST *cond, NodoAST *cuerpo, NodoAST *cuerpo_sino) {
    NodoAST *n = nodo_base(NODO_IF, linea);
    n->condicion = cond;
    n->cuerpo = cuerpo;
    n->cuerpo_sino = cuerpo_sino;
    return n;
}

NodoAST *crear_while(int linea, NodoAST *cond, NodoAST *cuerpo) {
    NodoAST *n = nodo_base(NODO_WHILE, linea);
    n->condicion = cond;
    n->cuerpo = cuerpo;
    return n;
}

NodoAST *crear_op_binaria(int linea, const char *op, NodoAST *izq, NodoAST *der) {
    NodoAST *n = nodo_base(NODO_OP_BINARIA, linea);
    n->operador = strdup(op);
    n->izq = izq;
    n->der = der;
    return n;
}

NodoAST *crear_op_relacional(int linea, const char *op, NodoAST *izq, NodoAST *der) {
    NodoAST *n = nodo_base(NODO_OP_RELACIONAL, linea);
    n->operador = strdup(op);
    n->izq = izq;
    n->der = der;
    return n;
}

NodoAST *crear_concat(int linea, NodoAST *izq, NodoAST *der) {
    NodoAST *n = nodo_base(NODO_CONCAT, linea);
    n->izq = izq;
    n->der = der;
    return n;
}

NodoAST *crear_negacion(int linea, NodoAST *expr) {
    NodoAST *n = nodo_base(NODO_NEGACION, linea);
    n->expr = expr;
    return n;
}

NodoAST *crear_id(int linea, char *nombre) {
    NodoAST *n = nodo_base(NODO_ID, linea);
    n->nombre = nombre;
    return n;
}

NodoAST *crear_entero(int linea, int valor) {
    NodoAST *n = nodo_base(NODO_ENTERO, linea);
    n->val_entero = valor;
    return n;
}

NodoAST *crear_real(int linea, double valor) {
    NodoAST *n = nodo_base(NODO_REAL, linea);
    n->val_real = valor;
    return n;
}

NodoAST *crear_cadena(int linea, char *valor) {
    NodoAST *n = nodo_base(NODO_CADENA, linea);
    n->val_cadena = valor;
    return n;
}

static void sangria(int nivel) {
    for (int i = 0; i < nivel; i++) printf("  ");
}

static const char *nombre_tipo_dato(TipoDato t) {
    return t == TIPO_PISTO ? "pisto" : "quetzal";
}

void imprimir_ast(NodoAST *nodo, int nivel) {
    if (!nodo) return;
    sangria(nivel);
    switch (nodo->tipo) {
        case NODO_PROGRAMA:
            printf("Programa\n");
            for (int i = 0; i < nodo->num_hijos; i++) imprimir_ast(nodo->hijos[i], nivel + 1);
            break;
        case NODO_BLOQUE:
            printf("Bloque\n");
            for (int i = 0; i < nodo->num_hijos; i++) imprimir_ast(nodo->hijos[i], nivel + 1);
            break;
        case NODO_DECL:
            printf("Declaracion (%s) %s [linea %d]\n", nombre_tipo_dato(nodo->tipo_dato), nodo->nombre, nodo->linea);
            if (nodo->expr) imprimir_ast(nodo->expr, nivel + 1);
            break;
        case NODO_ASIGNACION:
            printf("Asignacion %s [linea %d]\n", nodo->nombre, nodo->linea);
            imprimir_ast(nodo->expr, nivel + 1);
            break;
        case NODO_AGUAS:
            printf("Aguas \"%s\" [linea %d]\n", nodo->val_cadena, nodo->linea);
            break;
        case NODO_CLAVAR:
            printf("Clavar %s [linea %d]\n", nodo->nombre, nodo->linea);
            break;
        case NODO_SHUTE:
            printf("Shute [linea %d]\n", nodo->linea);
            imprimir_ast(nodo->expr, nivel + 1);
            break;
        case NODO_IF:
            printf("Cabal [linea %d]\n", nodo->linea);
            sangria(nivel + 1); printf("condicion:\n");
            imprimir_ast(nodo->condicion, nivel + 2);
            sangria(nivel + 1); printf("entonces:\n");
            imprimir_ast(nodo->cuerpo, nivel + 2);
            if (nodo->cuerpo_sino) {
                sangria(nivel + 1); printf("sino:\n");
                imprimir_ast(nodo->cuerpo_sino, nivel + 2);
            }
            break;
        case NODO_WHILE:
            printf("Brega [linea %d]\n", nodo->linea);
            sangria(nivel + 1); printf("condicion:\n");
            imprimir_ast(nodo->condicion, nivel + 2);
            sangria(nivel + 1); printf("cuerpo:\n");
            imprimir_ast(nodo->cuerpo, nivel + 2);
            break;
        case NODO_OP_BINARIA:
            printf("Operacion '%s' [linea %d]\n", nodo->operador, nodo->linea);
            imprimir_ast(nodo->izq, nivel + 1);
            imprimir_ast(nodo->der, nivel + 1);
            break;
        case NODO_OP_RELACIONAL:
            printf("Relacional '%s' [linea %d]\n", nodo->operador, nodo->linea);
            imprimir_ast(nodo->izq, nivel + 1);
            imprimir_ast(nodo->der, nivel + 1);
            break;
        case NODO_CONCAT:
            printf("Concat [linea %d]\n", nodo->linea);
            imprimir_ast(nodo->izq, nivel + 1);
            imprimir_ast(nodo->der, nivel + 1);
            break;
        case NODO_NEGACION:
            printf("Negacion [linea %d]\n", nodo->linea);
            imprimir_ast(nodo->expr, nivel + 1);
            break;
        case NODO_ID:
            printf("Id %s [linea %d]\n", nodo->nombre, nodo->linea);
            break;
        case NODO_ENTERO:
            printf("Entero %d [linea %d]\n", nodo->val_entero, nodo->linea);
            break;
        case NODO_REAL:
            printf("Real %g [linea %d]\n", nodo->val_real, nodo->linea);
            break;
        case NODO_CADENA:
            printf("Cadena \"%s\" [linea %d]\n", nodo->val_cadena, nodo->linea);
            break;
    }
}

void liberar_ast(NodoAST *nodo) {
    if (!nodo) return;
    for (int i = 0; i < nodo->num_hijos; i++) liberar_ast(nodo->hijos[i]);
    free(nodo->hijos);
    liberar_ast(nodo->expr);
    liberar_ast(nodo->izq);
    liberar_ast(nodo->der);
    liberar_ast(nodo->condicion);
    liberar_ast(nodo->cuerpo);
    liberar_ast(nodo->cuerpo_sino);
    free(nodo->nombre);
    free(nodo->val_cadena);
    free(nodo->operador);
    free(nodo);
}
