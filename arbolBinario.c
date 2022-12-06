//Clase de arbol binario inspirado en el realizado en la asigatura de Algoritmos y Estructuras de Datos en el segundo curso
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arbolBinario.h"
#include "definiciones.h"

//Estructura del elemento almacenado en cada nodo
typedef struct {
    char* lexema;
    short tipo;
}elemento;

//Estructura de los nodos del arbol
struct nodo {
    elemento info;
    struct nodo *izq, *der;
};


//Función para inicializar el arbol binario
void crea(abb *A) {
    *A = NULL;
}

//Función para liberar memoria y destruir el arbol
void destruye(abb *A) {
    if (*A != NULL) {
        destruye(&(*A)->izq);
        destruye(&(*A)->der);
        free(*A);
        *A = NULL;
    }
}

//Función para comprobar si el arbol es vacio
unsigned esVacio(abb A) {
    return A == NULL;
}

//Función para insertar un nuevo elemento en el arbol.
void inserta(abb *A, char* cadena, short tipo) {
    if (esVacio(*A)) {  //Caso para el cual el nodo actual del arbol está vacio
        *A = (abb) malloc(sizeof (struct nodo));    //Reserva de memoria para el arbol
        (*A)->info.lexema = (char*) malloc(strlen(cadena) * sizeof(char));  //Reserva de memoria para el lexema que se almacena en el nodo
        strcpy((*A)->info.lexema, "");  //Se inizan los elementos del lexema para evitar errores
        strncat((*A)->info.lexema, cadena, strlen(cadena)); //Se concatena el contenido del lexema con la cadena recibida
        (*A)->info.tipo = tipo; //Se asigna el tipo del elemento almacenado
        (*A)->izq = NULL;   //Se inicializan los punteros de izquierda y derecha
        (*A)->der = NULL;
    } else if (strcmp(cadena,(*A)->info.lexema)<0)  //Caso donde no esté vacio el arbol. Se va comprobando el orden alfabetico para introducirlo a la izquierda o derecha del actual
        inserta(&(*A)->izq, cadena, tipo);  //Si es menor alfabeticamente, se invoca a inserta al nodo de la izquierda
    else    //Si es menor alfabeticamente, se invoca a inserta al nodo de la derecha
        inserta(&(*A)->der, cadena, tipo);
}

//Función que buscará la cadena recibida en el arbol. En el caso de no encontrarlo se inserta
short busca_e_inserta(abb *A, char* cadena){
    if (esVacio(*A)){   //Cuando el nodo actual está vacio, indica que el elemento recibido no está en la tabla. Se debe introducir en el arbol y asignarlo como identificador
        *A = (abb) malloc(sizeof (struct nodo));
        (*A)->info.lexema = (char*) malloc(strlen(cadena) * sizeof(char));
        (*A)->info.lexema[0] = '\0';
        strncat((*A)->info.lexema, cadena, strlen(cadena));
        (*A)->info.tipo = IDENTIFICADOR;
        (*A)->izq = NULL;
        (*A)->der = NULL;
        return IDENTIFICADOR;  //Se devuelve identificador, que lo recibirá el analizador léxico a la hora de identificar cadenas alfanumericas
    }
    else if (strcmp(cadena,(*A)->info.lexema)==0){  //se realizan comprobaciones del orden alfabetico para buscar a la izquierda o la derecha
        return ((*A)->info.tipo);   //En el caso de que sea igual, es que se ha encontrado el elemento. Se retorna su tipo al analizador léxico
    }
    else if (strcmp(cadena,(*A)->info.lexema)<0){   //Si el elemento alfabeticamente es menor que el nodo actual, se invoca recursivamente a esta función al nodo de la izquierda
        return (busca_e_inserta(&(*A)->izq, cadena));
    }
    else{   //Si el elemento alfabeticamente es menor que el nodo actual, se invoca recursivamente a esta función al nodo de la derecha
        return (busca_e_inserta(&(*A)->der, cadena));
    }
}

//Función recursiva para imprimir el arbol en orden, según el código ascii
void imprimirArbol(abb *A) {

    if (!esVacio(*A)) {
        if (&(*A)->izq != NULL) {
            imprimirArbol(&(*A)->izq);
        }

        printf("Lexema: %s\t", (*A)->info.lexema);
        printf("---> Componente lexico: %hd\n", (*A)->info.tipo);

        if (&(*A)->der != NULL)
            imprimirArbol(&(*A)->der);
    }
}