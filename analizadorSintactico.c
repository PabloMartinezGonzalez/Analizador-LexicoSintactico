#include <stdio.h>
#include <stdlib.h>
#include "analizadorLexico.h"

//Única función del analizador Sintáctico. Realiza un bucle recibiendo componentes léxicos del analizador léxico y los imprime.
void analizar(){
    compLexico *lex; // Estructura donde se almacena el lexema y el componente léxico
    lex = (compLexico*) malloc(sizeof(compLexico)); // Reserva de memoria para el componente léxico
    lex->lexema = NULL; //Asignación a null del puntero del lexema para evitar errores

    //Bucle para ir recibiendo lexemas hasta que este retorne 0, es decir, se detecte el caracter EOF
    while(siguiente_Lexema(lex)){
        printf("< %s , %hd >\n", lex->lexema, lex->tipo);
    }

    printf("\nAnalisis lexico completado.\n");

    //Liberación de memoria de la estructura
    if(lex->lexema != NULL){
        free(lex->lexema);
        lex->lexema = NULL;
    }
    free(lex);
    lex = NULL;

}


