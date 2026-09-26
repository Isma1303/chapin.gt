#include <stdio.h>
#include "ast.h"
#include "symtab.h"

extern FILE *yyin;
extern int yylineno;
extern int yyparse(void);
extern NodoAST *raiz_ast;

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <archivo.gt>\n", argv[0]);
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (!yyin) {
        perror("No se pudo abrir el archivo de entrada");
        return 1;
    }

    tabla_inicializar();
    yyparse();
    fclose(yyin);

    int total_errores = errores_lexicos + errores_sintacticos + errores_semanticos;
    if (total_errores > 0) {
        fprintf(stderr, "\nCompilacion fallida: %d error(es) lexico(s), "
                        "%d error(es) sintactico(s), %d error(es) semantico(s).\n",
                errores_lexicos, errores_sintacticos, errores_semanticos);
        liberar_ast(raiz_ast);
        tabla_liberar();
        return 1;
    }

    printf("Programa aceptado: %s\n\n", argv[1]);

    printf("=== Arbol de sintaxis abstracta (AST) ===\n\n");
    imprimir_ast(raiz_ast, 0);

    printf("\n=== Tabla de simbolos ===\n\n");
    tabla_imprimir();

    liberar_ast(raiz_ast);
    tabla_liberar();
    return 0;
}
