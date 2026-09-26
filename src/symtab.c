#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

static Simbolo *tabla = NULL;

void tabla_inicializar(void) {
    tabla = NULL;
}

int tabla_declarar(const char *nombre, TipoDato tipo, int linea) {
    if (tabla_buscar(nombre)) return 0;
    Simbolo *s = malloc(sizeof(Simbolo));
    s->nombre = strdup(nombre);
    s->tipo = tipo;
    s->linea_decl = linea;
    s->siguiente = tabla;
    tabla = s;
    return 1;
}

Simbolo *tabla_buscar(const char *nombre) {
    for (Simbolo *s = tabla; s; s = s->siguiente) {
        if (strcmp(s->nombre, nombre) == 0) return s;
    }
    return NULL;
}

static const char *nombre_tipo(TipoDato t) {
    return t == TIPO_PISTO ? "pisto" : "quetzal";
}

void tabla_imprimir(void) {
    if (!tabla) {
        printf("(tabla vacia, no se declararon variables)\n");
        return;
    }
    printf("%-20s %-10s %s\n", "Nombre", "Tipo", "Linea de declaracion");
    printf("%-20s %-10s %s\n", "------", "----", "---------------------");
    for (Simbolo *s = tabla; s; s = s->siguiente) {
        printf("%-20s %-10s %d\n", s->nombre, nombre_tipo(s->tipo), s->linea_decl);
    }
}

void tabla_liberar(void) {
    Simbolo *s = tabla;
    while (s) {
        Simbolo *sig = s->siguiente;
        free(s->nombre);
        free(s);
        s = sig;
    }
    tabla = NULL;
}
