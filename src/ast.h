#ifndef AST_H
#define AST_H

/* Contadores de errores, compartidos entre lexer.l, parser.y y main.c */
extern int errores_lexicos;
extern int errores_sintacticos;
extern int errores_semanticos;

typedef enum {
    NODO_PROGRAMA,
    NODO_BLOQUE,
    NODO_DECL,
    NODO_ASIGNACION,
    NODO_AGUAS,
    NODO_CLAVAR,
    NODO_SHUTE,
    NODO_IF,
    NODO_WHILE,
    NODO_OP_BINARIA,
    NODO_OP_RELACIONAL,
    NODO_CONCAT,
    NODO_NEGACION,
    NODO_ID,
    NODO_ENTERO,
    NODO_REAL,
    NODO_CADENA
} TipoNodo;

typedef enum { TIPO_PISTO, TIPO_QUETZAL } TipoDato;

typedef struct NodoAST {
    TipoNodo tipo;
    int linea;

    /* listas: NODO_PROGRAMA y NODO_BLOQUE */
    struct NodoAST **hijos;
    int num_hijos;
    int cap_hijos;

    /* NODO_DECL / NODO_ASIGNACION / NODO_CLAVAR / NODO_ID */
    char *nombre;
    TipoDato tipo_dato;      /* solo para NODO_DECL */

    /* literales */
    int val_entero;
    double val_real;
    char *val_cadena;        /* NODO_CADENA y NODO_AGUAS */

    /* operadores binarios / relacionales / concat */
    char *operador;
    struct NodoAST *izq;
    struct NodoAST *der;

    /* NODO_DECL (expresion de init) / NODO_ASIGNACION (valor) /
       NODO_SHUTE / NODO_NEGACION (operando) */
    struct NodoAST *expr;

    /* NODO_IF / NODO_WHILE */
    struct NodoAST *condicion;
    struct NodoAST *cuerpo;       /* rama "entonces" en if, cuerpo en while */
    struct NodoAST *cuerpo_sino;  /* rama "sino"; NULL si no existe o es while */
} NodoAST;

NodoAST *crear_lista(TipoNodo tipo, int linea);
void agregar_hijo(NodoAST *lista, NodoAST *hijo);

NodoAST *crear_decl(int linea, TipoDato tipo, char *nombre, NodoAST *expr);
NodoAST *crear_asignacion(int linea, char *nombre, NodoAST *expr);
NodoAST *crear_aguas(int linea, char *cadena);
NodoAST *crear_clavar(int linea, char *nombre);
NodoAST *crear_shute(int linea, NodoAST *expr);
NodoAST *crear_if(int linea, NodoAST *cond, NodoAST *cuerpo, NodoAST *cuerpo_sino);
NodoAST *crear_while(int linea, NodoAST *cond, NodoAST *cuerpo);
NodoAST *crear_op_binaria(int linea, const char *op, NodoAST *izq, NodoAST *der);
NodoAST *crear_op_relacional(int linea, const char *op, NodoAST *izq, NodoAST *der);
NodoAST *crear_concat(int linea, NodoAST *izq, NodoAST *der);
NodoAST *crear_negacion(int linea, NodoAST *expr);
NodoAST *crear_id(int linea, char *nombre);
NodoAST *crear_entero(int linea, int valor);
NodoAST *crear_real(int linea, double valor);
NodoAST *crear_cadena(int linea, char *valor);

void imprimir_ast(NodoAST *nodo, int nivel);
void liberar_ast(NodoAST *nodo);

#endif
