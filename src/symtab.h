#ifndef SYMTAB_H
#define SYMTAB_H

#include "ast.h"

typedef struct Simbolo {
    char *nombre;
    TipoDato tipo;
    int linea_decl;
    struct Simbolo *siguiente;
} Simbolo;

void tabla_inicializar(void);

/* Inserta una variable nueva. Retorna 0 si el nombre ya existia (no inserta). */
int tabla_declarar(const char *nombre, TipoDato tipo, int linea);

Simbolo *tabla_buscar(const char *nombre);
void tabla_imprimir(void);
void tabla_liberar(void);

#endif
