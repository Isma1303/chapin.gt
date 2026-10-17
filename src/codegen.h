#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"

/*
 * codegen.h - Fase de traduccion Chapin (.gt) -> Nova (.nova)
 *
 * Recorre el AST ya validado y escribe un programa Nova equivalente en la
 * ruta indicada. El programa Nova resultante es consumible por el compilador
 * de Assets/Proyecto-Nova (`./nova <archivo>.nova`).
 */

/*
 * Traduce el AST `raiz` a codigo Nova y lo escribe en `ruta`.
 * Devuelve 0 si el archivo se escribio correctamente, -1 si no se pudo abrir.
 */
int traducir_a_nova(NodoAST *raiz, const char *ruta);

#endif
