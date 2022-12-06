#include <stdlib.h>
#include "analizadorSintactico.h"
#include "sistemaEntrada.h"
#include "tablaSimbolos.h"

int main() {

    //Inicialización de los elementos necesarios para antes de comenzar a analizar
    iniciarSE("../concurrentSum.go"); // Inicialización del sistema de entrada a partir del fichero de entrada
    iniciarTabla(); // Inicialización de la tabla de símbolos

    //Parte principal. Analiza los lexemas del fichero y muestra la tabla de símbolos final
    analizar(); //Ejecución de la funcion del analizador sintáctico, para que se vayan analizando los lexemas desde el léxico e imprimiendolos
    imprimirTabla(); // Se imprime la tabla de símbolos final despues de la ejecución

    /* Cierre y limpieza del sistema. */
    cerrarSE(); // Libero memoria del sistema de entrada.
    eliminarTabla(); // Libero memoria de la tabla de símbolos.

    return (EXIT_SUCCESS);
}
