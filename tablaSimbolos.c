
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "arbolBinario.h"
#include "definiciones.h"

/* Tamaño de la cadena en que almaceno las lecturas de lexemas de la tabla de símbolos. Sé que el lexema más grande es de longitud 7
   porque el archivo de definiciones es un fichero estático que puedo comprobar en todo momento.
   Si en algún momento añadiese un lexema más grande, es fácil venir aquí y cambiar esta constante.*/
#define TAM 8

abb tabla; // Mi tabla de símbolos. Un árbol binario.
char *lexDEF[] = {"package","import", "func","chan",
                         "var", "for","range",  "go"};

short macros[] = {PACKAGE, IMPORT, FUNC, CHAN,
                 VAR, FOR, RANGE, GO};

/* Creo la tabla de símbolos y la relleno con las palabras reservadas del lenguaje. */
void iniciarTabla(){
    crea(&tabla);
    // Inicializo la tabla de símbolos.
    for (int i = 0; i < 8; i++) {
        inserta(&tabla, lexDEF[i], macros[i]);
    }
}

/* Función para buscar en la tabla e insertar cuando sea necesario: devuelve _IDENTIFICADOR o el componente léxico de aquello con lo que el argumento haya coincidido. */
short insertarEnTabla(char* cadena){
    return (busca_e_inserta(&tabla, cadena));
}

/* Función para imprimir la tabla de símbolos. */
void imprimirTabla(){
    printf("\n---------------------Tabla de simbolos---------------------\n");
    imprimirArbol(&tabla);
    printf("\n\n");
}

/* Función para liberar la memoria de la tabla de símbolos. */
void eliminarTabla(){
    destruye(&tabla);
}