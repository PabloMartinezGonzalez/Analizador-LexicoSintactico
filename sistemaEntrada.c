#include <stdio.h>
#include <errno.h>
#include "sistemaEntrada.h"
#include "errores.h"

#define N 32 //Tamaño de los buffers

//Estructura que implementa un buffer, donde se tiene el contenido, y dos variables para saber la posición de inicio y de fin del lexema en ese buffer
typedef struct{
    char contenido[N + 1]; // Vector para almacenar los elementos del buffer. Inicializaco a N+1 para dejar sitio al caracter EOF
    short inicio; // Puntero al inicio del lexema.
    short delantero; // Puntero al carácter actual.
}centinela;

//Estructura para implementar el doble buffer
typedef struct{
    centinela A;
    centinela B;
}buffer;


FILE *fichero;   //Variable para el archivo del código a compilar
buffer buf;     //Variable para el doble centinela
short activo = 0;  //Variable para indicar qué buffer está activo (0=A, 1=B)
short flagLlenar = 1; // Variable para indicar que el siguiente buffer ya se cargó previamente pero se tuvo que devolver el caracter, por lo que no se tiene que cargar

//Función que se carga de la inicialización de los indices de los buffers
void creaBuffer(){
    buf.A.inicio = 0;
    buf.A.delantero = 0;
    buf.B.inicio = 0;
    buf.B.delantero = 0;
    activo = 0; // Inicialmente el buffer activo es el A
}

//Función para inicializar los buffers y rellenar el centinela A
void iniciarSE(char* f){
    creaBuffer(buf);
    if((fichero = fopen(f, "r")) == NULL){ //Se abre el fichero en modo lectura
        error("Fallo al abrir el fichero de entrada");
    }else{
        fread(buf.A.contenido, 1, N, fichero);  //Se leen los N primeros elementos del fichero y almacenan en el contenido del buffer A
        buf.A.contenido[N]=EOF; //Se mete al final del contenido del buffer el elemento EOF
    }
}

//Función para rellenar el siguiente buffer dependiendo del que se encuentre activo
void rellenarCentinela(){
    unsigned int bLeidos;
    if(activo == 0){ // Si A está activo, se rellena el buffer B

        bLeidos = fread(buf.B.contenido, 1, N, fichero);
        buf.B.contenido[bLeidos] = EOF;
        if(ferror(fichero)){ // Se comprueba si ha habido errores tras la lectura del fichero
            error("Fallo al leer cargar los elementos del fichero en el buffer");
        }
        else{
            //Se inicializan los punteros del buffer
            buf.B.inicio = 0;
            buf.B.delantero = 0;
        }
    }
    if(activo == 1){ // Si B activo, se rellena el buffer A. Exactamente igual al caso anterior

        bLeidos = fread(buf.A.contenido, 1, N, fichero);
        buf.A.contenido[bLeidos] = EOF;
        if(ferror(fichero)){
            error("Fallo al leer cargar los elementos del fichero en el buffer");
        }
        else{
            buf.A.inicio = 0;
            buf.A.delantero = 0;
        }
    }
}


//Función para comprobar qué buffer está activo y cambiar al otro
void cambiarBuffer(){
    if(activo == 0) activo = 1; // Si A está activo, marco B como activo
    else if(activo == 1) activo = 0; // Si B está activo, marco A como activo
}

//Función para realizar el cierre el fichero que se está leyendo
void cerrarSE(){
    if(fclose(fichero)) { // Se comprueba si se producen errores en el cierre
        error("Fallo al cerrar el fichero de entrada");
    }
}

//Función para devolver al analziador léxico el siguiente caracter
char siguienteCaracter(){
    char c; // Variable para almacenar el carácter leído.

    if(activo == 0){ // Caso donde el buffer activo sea el A
        if((c = buf.A.contenido[buf.A.delantero]) == EOF){ // Se extrae el caracter del contenido a partir del puntero delantero, comprobando si es el fin de fichero
            if(!feof(fichero)){ //Se comprueba si el fin de fichero es el del fichero de entrada y no el del buffer
                if(flagLlenar)
                    rellenarCentinela(); //En el caso que el flagCargar esté a 1, el buffer B no se había llenado, por lop que se debe hacer
                else
                    flagLlenar = 1; // En el caso que sea 0, ya se habia cargado previamente y no se debe volver a cargar para no eliminar el contenido que aun no ha sifdo analizado
                cambiarBuffer(); // Se cambia el buffer activo
                c = siguienteCaracter(); //Se vuelve a llamar a la función para obtener el caracter siguiente del nuevo buffer activo
            }
                // En el caso que el elemento EOF sea el de fin de fichero, se avanza y se devuelve el EOF al analizador léxico
            else
                buf.A.delantero++; // Avanzo el puntero de delantero.
        }
            // En el caso que no sea un EOF, se avanza normal
        else
            buf.A.delantero++; // Avanzo el puntero de delantero y lo devuelvo.
    }
        //Caso donde el buffer activo sea el B. Exactamente igual al caso anterior
    else {
        if((c = buf.B.contenido[buf.B.delantero]) == EOF){
            if(!feof(fichero)){
                if(flagLlenar)
                    rellenarCentinela();
                else
                    flagLlenar = 1;
                cambiarBuffer();
                c = siguienteCaracter();
            }
            else
                buf.B.delantero++;
        }
        else
            buf.B.delantero++;
    }
    return (c); //Se devuelve el caracter obtenido
}

//Función para retroceder el puntero delantero del buffer activo al eliminarse del lexema actual
void devolverCaracter(){
    //Caso donde el buffer activo sea el A
    if(activo == 0){
        if(buf.A.delantero == 0){ //Si el puntero delantero era se encontraba al inicio del buffer, se debe retroceder al buffer anterior
            cambiarBuffer(); // Se debe cambiar el centinela activo
            buf.B.delantero--; //Se disminuye el valor de delantero del buffer activo, para que no apunte al EOF
            flagLlenar = 0; // Se pone el flag rellenar a 0 para indiciar que el otro buffer ya se ya cargado y no debe llenarse otra vez para no eliminar ese contenido aun no analizado
        }
        else buf.A.delantero--; // En el caso de no encontrarse al principio, simplemente disminuye el valor de delantero
    }
        //Caso exactamente igual para cuando el centinela activo es el B
    else if(activo == 1){
        if(buf.B.delantero == 0){
            cambiarBuffer();
            buf.A.delantero--;
            flagLlenar = 0;
        }
        else buf.B.delantero--;
    }
}

//Función para cuando se acepta el lexema. Se debe reiniciar el valor del puntero inicio para que coincida con el de delantero
void aceptarLexema(){
    //Se comprueba qué centinela esá activo
    if(activo == 0) buf.A.inicio = buf.A.delantero;
    else if(activo == 1) buf.B.inicio = buf.B.delantero;
}
