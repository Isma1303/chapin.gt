# Análisis técnico del compilador Nova — Semana 11

**Documento analizado:** `Compilador_Nova_Semana11.pdf`  
**Alcance:** Evaluación de lo presentado en las siete diapositivas. No se revisó el código fuente del proyecto.

## 1. Descripción general

Nova es un lenguaje de programación desarrollado como proyecto académico. La presentación se concentra en el **front-end del compilador**, que procesa archivos `.nova` para obtener un árbol de sintaxis abstracta (**AST**) y una **tabla de símbolos**.

El equipo indica que utiliza **Flex**, **Bison** y **C**:

- **Flex:** identifica los tokens del código fuente durante el análisis léxico.
- **Bison:** verifica la estructura del programa mediante reglas gramaticales.
- **C:** integra los componentes del proyecto y permite construir el ejecutable.

> **Distinción importante:** La presentación demuestra etapas de análisis de código fuente, pero no muestra ejecución de programas Nova ni generación de código objeto o ejecutable a partir de ellos.

## 2. Arquitectura presentada

En la diapositiva 3 se representa el siguiente flujo:

```text
Archivo .nova → Flex → Tokens → Bison → AST → Tabla de símbolos
```

El esquema explica correctamente la idea general de tokenización y análisis sintáctico. Sin embargo, la tabla de símbolos no tiene que producirse obligatoriamente después del AST: puede construirse durante el análisis sintáctico o en fases posteriores. La presentación no aclara el momento preciso en que se construye.

## 3. Compilación del proyecto

La diapositiva 4 ofrece estos pasos:

```bash
cd ProyectoNova
make clean
make
```

Se indica que, si no existen errores, se genera el ejecutable `nova` (`nova.exe` en Windows). Esto describe la **compilación del propio proyecto del compilador** y no la generación de ejecutables de programas escritos en Nova.

## 4. Pruebas mostradas

### 4.1. Programa válido — diapositiva 5

El equipo ejecuta:

```bash
./nova programa_valido.nova
```

La salida presentada informa que el análisis sintáctico es correcto y muestra, entre otros datos:

| Variable | Valor mostrado | Tipo en tabla de símbolos |
|---|---|---|
| `edad` | `20` | `entero` |
| `promedio` | `85.500000` | `decimal` |
| `nombre` | `"Wesley"` | `texto` |
| `activo` | No visible en el fragmento del AST | `booleano` |

La evidencia permite observar que existen registros de variables y tipos. No permite comprobar, por sí sola, cómo se estructura internamente el AST ni qué instrucciones soporta el lenguaje.

### 4.2. Programa inválido — diapositiva 6

El equipo ejecuta:

```bash
./nova programa_invalido.nova
```

El resultado incluye el mensaje:

```text
Error sintáctico: syntax error
La compilacion termino con errores.
```

La presentación explica que al archivo le falta un punto y coma (`;`) después de una asignación y que un bloque `si` no se cierra con `fin`.

La prueba ofrece evidencia de detección de errores sintácticos, aunque no muestra diagnósticos detallados por línea, columna o token inesperado, ni evidencia cómo se reportan varios errores dentro de un mismo archivo.

## 5. Evaluación de los componentes

| Componente | Evidencia de la presentación | Interpretación |
|---|---|---|
| Análisis léxico | Se reporta un lexer Flex funcional | No se muestran todas las reglas de tokens |
| Análisis sintáctico | Bison y pruebas válida/inválida | Se observa un caso aceptado y uno rechazado |
| AST | Se presenta una salida titulada «AST INICIAL» | No se ve un árbol jerárquico completo |
| Tabla de símbolos | Se listan cuatro variables con sus tipos | Hay evidencia de registro básico |
| Manejo de errores | Mensaje `syntax error` | Diagnóstico genérico |
| Análisis semántico | No se explica | No verificable a partir de las diapositivas |
| Ejecución del lenguaje | No se demuestra | No verificable |
| Generación de código | No se presenta | No verificable |

## 6. Fortalezas

1. El proyecto distingue los componentes léxico y sintáctico y adopta herramientas apropiadas para un compilador académico.
2. Utiliza `make` para automatizar la construcción del ejecutable del compilador.
3. Incluye pruebas con archivos válidos e inválidos.
4. Presenta una tabla de símbolos con nombres de variables y tipos.
5. Define un alcance de entrega parcial centrado en el front-end.

## 7. Aspectos por mejorar

### 7.1. Mostrar la sintaxis propia de Nova

Sería conveniente presentar el contenido real de un archivo `.nova` para comprender declaraciones, asignaciones, expresiones y estructuras de control. La presentación no incluye ese código fuente.

### 7.2. Demostrar la estructura del AST

La salida visible enumera variables y valores. Para demostrar mejor el AST, podrían mostrar nodos y relaciones jerárquicas, por ejemplo los correspondientes a una operación aritmética o una condición.

### 7.3. Mejorar los mensajes de error

El mensaje `syntax error` identifica que existe un problema, pero no ayuda suficientemente a localizarlo. Se recomienda mostrar línea, columna, token problemático y una explicación del error, si el proyecto contempla esa funcionalidad.

### 7.4. Documentar el análisis semántico

No se especifica si Nova valida tipos incompatibles, variables no declaradas, redeclaraciones o ámbitos. Estas verificaciones deberían explicarse o quedar claramente identificadas como trabajo pendiente.

### 7.5. Definir los siguientes pasos

No se presenta un plan de interpretación, generación de código o ejecución. Si el objetivo final es ejecutar programas escritos en Nova, conviene explicar qué estrategia se seguirá.

## 8. Preguntas para los expositores

1. **¿Cómo diferencian en Flex una palabra reservada de un identificador?**  
   *Objetivo:* Evaluar el manejo de reglas léxicas y prioridades.

2. **¿El AST almacena únicamente declaraciones o también expresiones, condiciones y ciclos?**  
   *Objetivo:* Delimitar el alcance de la representación sintáctica.

3. **¿Qué ocurre al asignar una cadena de texto a una variable declarada como entera?**  
   *Objetivo:* Comprobar si existen verificaciones semánticas de tipos.

4. **¿Cómo manejan variables declaradas en bloques distintos?**  
   *Objetivo:* Comprobar si la tabla de símbolos contempla ámbitos (*scopes*).

5. **¿Por qué los errores se reportan solo como `syntax error`?**  
   *Objetivo:* Conocer el estado del sistema de diagnósticos.

6. **¿Qué etapa implementarán para que Nova pueda ejecutar un programa?**  
   *Objetivo:* Conocer la estrategia de desarrollo posterior al front-end.

## 9. Conclusión

La presentación de **Nova — Semana 11** expone una arquitectura coherente para las primeras fases de un compilador académico. Muestra integración declarada de Flex y Bison, un caso de análisis válido, uno inválido, una salida identificada como AST y una tabla básica de símbolos.

No obstante, **la evidencia presentada no basta para confirmar la completitud del front-end**: faltan ejemplos de código fuente, detalles de la gramática, la estructura completa del AST y verificaciones semánticas. Tampoco se demuestra ejecución de programas Nova.

La evaluación debe entenderse como un **análisis de la presentación**, no como una auditoría del funcionamiento real del compilador.
