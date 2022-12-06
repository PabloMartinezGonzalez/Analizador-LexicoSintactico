#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "definiciones.h"
#include "sistemaEntrada.h"
#include "tablaSimbolos.h"
#include "analizadorLexico.h"
#include "errores.h"

#define tamLex 32    //Variable que define el tamaño del lexema

short flagTam=0;   //Flag para indicar cuando se ha superado el tamaño
// Función para inicializar la estructura del componente léxico
void creaLex(compLexico *lex) {
    if (lex->lexema != NULL) { // En caso que no sea nulo se debe liberar la memoria
        free(lex->lexema);
        lex->lexema = NULL;
    }
    //Reserva de memoria e inicialización para el componente lexico
    lex->lexema = (char *) malloc(tamLex * sizeof(char));
    memset(lex->lexema, 0, tamLex);   //Inicialización a null de los elementos para evitar aparición de basura previa en esas direcciones de memoria
    lex->tipo = -1; //Se pone el tipo a -1 para identificar en caso de error
}

// Función para introducir un caracter en el lexema del componente lexico
void almacenar(compLexico *lex, char c) {
    //Se concatena el contenido actual con el nuevo caracter


    //En el caso que el tamaño del lexema sea igual al tamaño de memoria, se debe reservar más para poder introducir más caracteres
    if(tamLex == strlen(lex->lexema)+1){
        if(flagTam==0) {
            error("El tamanho del lexema supera el tamanho maximo permitido.");
            flagTam=1;
        }

    }else{
        strncat(lex->lexema, &c, 1);
    }

}

//Funcion que realizará la funcion del automata que detecta carácteres alfanumericos
void alfanumerico(compLexico *lex, char actual) {
    actual = siguienteCaracter();
    almacenar(lex, actual);
    if (isalpha(actual) || isdigit(actual) || actual == '_') {  //Caso de detectar un caracter válido para una cadena alfanumerica, o el caracter empieza por _
        while (isalpha(actual) || isdigit(actual) || actual == '_') {   //Se van añadiendo caracteres al lexema mientras estos sean válidos
            actual = siguienteCaracter();
            almacenar(lex, actual);
        }
        //Cuando no se encuentran más caracteres válidos, se devuelve el último y se elimina del lexema
        devolverCaracter();
        lex->lexema[strlen(lex->lexema) - 1] = '\0';
        aceptarLexema();
        lex->tipo = insertarEnTabla(lex->lexema);   //Se manda el lexema a la tabla de simbolos para obtener el componente léxico de este, viendo si es una palabra reservada o un identificador

    } else {    //Caso donde se encuentra un único caracter válido
        devolverCaracter();
        lex->lexema[strlen(lex->lexema) - 1] = '\0';
        aceptarLexema();
        lex->tipo = insertarEnTabla(lex->lexema);   //Se manda a la tabla de símbolos para obtener su componente léxico
    }
}

//Función que se encarga del análisis de dígitos con exponente (Sirve para simplificar el automata de dígitos)
void gestionarExponente(compLexico *lex, char actual) {
    short barraB = 0;   //Variable para detectar si el último elemento fue una barra baja
    actual = siguienteCaracter();
    almacenar(lex, actual);
    if (actual == '+' || actual == '-') {   //Caso donde despues del exponente va un + o un -
        actual = siguienteCaracter();
        almacenar(lex, actual);
        if (isdigit(actual)) {  //Caso donde despues del + o - vienen digitos
            while (isdigit(actual) || actual == '_') {  //Se guardan los elementos hasta que se encuentre un elemento no válido
                actual = siguienteCaracter();
                almacenar(lex, actual);
                if (actual == '_') {    //Caso donde se detecta una barra baja
                    if (barraB == 1) {  //Caso donde son dos barra bajas seguidas. Se acepta el número previo y devuelven las barras bajas
                        devolverCaracter();
                        devolverCaracter();
                        lex->lexema[strlen(lex->lexema) - 2] = '\0';
                        aceptarLexema();
                        lex->tipo = FLOAT;
                    } else {    //Se pine a uno la variable para indicar que el último caracter fue una barra baja
                        barraB = 1;
                    }
                }
            }
            if (barraB == 0) {   //caso donde el último caracter antes del actual no sea barra baja. Solo puede separar dos digitos consecutivos
                if (actual == 'i') {    //Caso en el que se encuentre una i, indicando que es un numero imaginario
                    aceptarLexema();
                    lex->tipo = IMAGINARIO;
                } else {   //caso donde despues del exponente no haya una i, indica que el numero leido es un float
                    devolverCaracter();
                    lex->lexema[strlen(lex->lexema) - 1] = '\0';
                    aceptarLexema();
                    lex->tipo = FLOAT;
                }
            } else {  //caso donde el último caracter antes del actual sea barra baja. Solo puede separar dos digitos consecutivos, se deben eliminar el último caracter y la barra baja
                devolverCaracter();
                devolverCaracter();
                lex->lexema[strlen(lex->lexema) - 2] = '\0';
                aceptarLexema();
                lex->tipo = FLOAT;
            }
        } else {   //caso donde despues del "e+" no se encuentre un digito. Indica que los tres ultimos elementos (.e+X) no pertenecen al número, si no a otros componentes lexicos
            devolverCaracter();
            devolverCaracter();
            devolverCaracter();
            lex->lexema[strlen(lex->lexema) -  3] = '\0';  //eliminar los tres ultimos caracteres del componente lexico
            lex->tipo = FLOAT;
            aceptarLexema();
        }
    } else if (isdigit(actual)) {  //caso que después de un exponente vaya un numero directamente (Análogo al anterior)
        while (isdigit(actual) || actual == '_') {
            actual = siguienteCaracter();
            almacenar(lex, actual);
            if (actual == '_') {
                if (barraB == 1) {
                    devolverCaracter();
                    devolverCaracter();
                    lex->lexema[strlen(lex->lexema) - 2] = '\0';
                    aceptarLexema();
                    lex->tipo = FLOAT;
                } else {
                    barraB = 1;
                }
            }
        }
        if (barraB == 0) {
            if (actual == 'i') {
                aceptarLexema();
                lex->tipo = IMAGINARIO;
            } else {
                devolverCaracter();
                lex->lexema[strlen(lex->lexema) - 1] = '\0';
                aceptarLexema();
                lex->tipo = FLOAT;
            }
        } else {
            devolverCaracter();
            devolverCaracter();
            lex->lexema[strlen(lex->lexema) - 2] = '\0';
            aceptarLexema();
            lex->tipo = FLOAT;
        }
    } else {  //caso donde despues de e, vaya algo distinto (.eX), lo cual indica que la e pertenece a otro lexema, haciendo que el lexema actual sea un float
        devolverCaracter();
        devolverCaracter();
        lex->lexema[strlen(lex->lexema) - 2] = '\0';
        aceptarLexema();
        lex->tipo = FLOAT;
    }
}

//Función que implementa el automata de detección de números
short digitos(compLexico *lex, char actual) {
    short barraB = 0;     //flag para indicar que el último caracter era una barra baja
    while (isdigit(actual) || actual == '_') {
        actual = siguienteCaracter();
        almacenar(lex, actual);

        if (actual == '_') {    //Caso donde se encuentre una barra baja
            if (barraB == 1) {  //Caso donde haya dos barra bajas seguidas. Se considera el número y devuelven las barras bajas
                devolverCaracter();
                devolverCaracter();
                lex->lexema[strlen(lex->lexema) - 2] = '\0';
                aceptarLexema();
                lex->tipo = ENTERO;
                return 1;
            } else {    //En el caso de que sea la primera barra baja, se pone el flag a 1
                barraB = 1;
            }
        } else {    //Si no es una barra baja y es un digito, se pone el flag a 0
            barraB = 0;
        }
    }
    if (barraB == 0) { //caso donde el último elemento no sea una barra baja.
        if (actual == '.') {    //Caso donde se detecte un punto -> números flotantes
            actual = siguienteCaracter();
            almacenar(lex, actual);
            if (isdigit(actual)) {
                while (isdigit(actual) || actual == '_') {
                    actual = siguienteCaracter();
                    almacenar(lex, actual);
                    if (actual == '_') {
                        if (barraB == 1) {  //dos barra bajas seguidas. Se considera el número y devuelven las barras bajas
                            devolverCaracter();
                            devolverCaracter();
                            lex->lexema[strlen(lex->lexema) - 2] = '\0';
                            aceptarLexema();
                            lex->tipo = FLOAT;
                            return 1;
                        } else {
                            barraB = 1;
                        }
                    }
                }
                if (barraB ==0) {   //Caso donde el último caracter no fue una _, esto no puede ser al solo poder esta separar dos digitos consecutivos
                    if (actual == 'e' || actual == 'E') {
                        gestionarExponente(lex, actual);
                        return 1;
                    } else if (actual == 'i') {    //Caso en el que se encuentre una i, indicando que es un numero imaginario
                        aceptarLexema();
                        lex->tipo = IMAGINARIO;
                        return 1;
                    } else {   //caso donde despues de los digitos no viene nada válido
                        devolverCaracter();
                        devolverCaracter();
                        lex->lexema[strlen(lex->lexema) - 2] = '\0';
                        aceptarLexema();
                        lex->tipo = FLOAT;
                        return 1;
                    }
                } else {   //El último caracter fue una _, esto no puede ser al solo poder esta separar dos digitos consecutivos
                    devolverCaracter();
                    devolverCaracter();
                    lex->lexema[strlen(lex->lexema) - 2] = '\0';
                    aceptarLexema();
                    lex->tipo = FLOAT;
                    return 1;
                }
            } else if (actual =='i') {    //caso donde despues de un punto va una i, es imaginario, analizando numeros como x.i
                if(strlen(lex->lexema)==2){    //Caso donde solo estén los caracteres .i -> no es un número imaginario. Se considera el . como opeador y se retorna 0
                    devolverCaracter();
                    lex->lexema[strlen(lex->lexema) - 1] = '\0';
                    aceptarLexema();
                    return 0;
                }else{  //Caso donde antes del putno haya digitos
                    aceptarLexema();
                    lex->tipo = IMAGINARIO;
                    return 1;
                }
            } else if (actual == 'e' || actual == 'E') {  //Caso donde despues del número esté el exponente
                if(strlen(lex->lexema)==2){    //Caso donde solo estén los caracteres .e -> no se considera valido, el punto es un operador. Se retorna 0
                    devolverCaracter();
                    lex->lexema[strlen(lex->lexema) - 1] = '\0';
                    aceptarLexema();
                    return 0;
                }else{  //Caso donde antes del punto haya digitos
                    gestionarExponente(lex, actual);
                    return 1;
                }
            } else {  //Caso donde despues del punto no haya más elementos del número
                if(strlen(lex->lexema)==2){    //Caso donde el primero sea el punto y despues otro caracter. El punto es un operador
                    devolverCaracter();
                    lex->lexema[strlen(lex->lexema) - 1] = '\0';
                    aceptarLexema();
                    return 0;
                }else{  //Caso donde antes del punto haya digitos
                    devolverCaracter();
                    lex->lexema[strlen(lex->lexema) - 1] = '\0';
                    aceptarLexema();
                    lex->tipo = FLOAT;
                    return 1;
                }
            }
        } else if (actual == 'e' || actual == 'E') {//caso donde despues de los digitos viene un exponente
            gestionarExponente(lex, actual);
            return 1;
        } else if (actual == 'i') {   //Caso donde despues de los digitos enteros se encuentre una i -> numero imaginario
            aceptarLexema();
            lex->tipo = IMAGINARIO;
            return 1;
        }
            //Caso para cuando se detectan números hexadecimales
        else if ((actual == 'x' || actual == 'X') && strlen(lex->lexema) == 2 && lex->lexema[0] == '0') { //se debe comprobar si el primer elemento es 0 y solo hay dos elementos (0x)
            actual = siguienteCaracter();
            almacenar(lex, actual);
            //Se comprueba el caso donde despues de la X vaya una barra baja, ya que una sola es válido en números hexadecimales despues de la x
            if (actual == '_') {
                barraB = 1;
            }
            if (isxdigit(actual) || actual == '_') {
                while (isxdigit(actual) || actual == '_') { //Se van guardando los digitos si son hexadecimales o barra baja
                    actual = siguienteCaracter();
                    almacenar(lex, actual);
                    if (actual == '_') {    //Caso donde se encuentre una barra baja
                        if (barraB == 1) {  //Caso donde haya dos barra bajas seguidas, no se considera válido. Se considera el número y devuelven las barras bajas
                            devolverCaracter();
                            devolverCaracter();
                            lex->lexema[strlen(lex->lexema) - 2] = '\0';
                            aceptarLexema();
                            lex->tipo = ENTERO;
                            return 1;
                        } else {
                            barraB = 1;
                        }
                    }
                }
                if (barraB == 0) {  //Caso donde se acepta el número hexadecimal al ser válido, viendo que el últomo elemento no es barra baja
                    devolverCaracter();
                    lex->lexema[strlen(lex->lexema) - 1] = '\0';
                    aceptarLexema();
                    lex->tipo = ENTERO;
                    return 1;
                } else {    //Caso donde el último elemento es barra baja. Se debe devolver al ser un lexema a parte
                    devolverCaracter();
                    devolverCaracter();
                    lex->lexema[strlen(lex->lexema) - 2] = '\0';
                    aceptarLexema();
                    lex->tipo = ENTERO;
                    return 1;
                }
            }
        } else { //Caso donde solo se encuentres digitos y despues un caracter no válido
            devolverCaracter();

            lex->lexema[strlen(lex->lexema) - 1] = '\0';
            aceptarLexema();
            lex->tipo = ENTERO;
            return 1;

        }
    } else {  //caso donde el penúltimo elemento sea una barra baja. Esta solo puede separar dos digitos. Se debe devolver la barra baja y el último elemento leido
        devolverCaracter();
        devolverCaracter();
        lex->lexema[strlen(lex->lexema) - 2] = '\0';
        aceptarLexema();
        lex->tipo = ENTERO;
        return 1;
    }
    return 0;
}

//Función que va a ir analizando los caracteres recibidos por el sistema de entrada para identificar los lexemas y mandarlos al analizador sintáctico, que es el que invoca esta función
short siguiente_Lexema(compLexico *lex) {
    char actual; // Variable para almacenar el caracter que se está leyendo
    short encontrado = 0;   //Variable que determina que ya se ha identificado el lexema y se debe salir del bucle para mandarselo al sintáctico
    flagTam=0;
    while (!encontrado) {   //Bucle para ir leyendo lexemas
        creaLex(lex);   //Inicialización del componente léxico (se libera memoria en el caso de que no sea nulo, es decir, se quiera eliminar el contenido del lexema)
        actual = siguienteCaracter();
        if (actual == EOF) {    //Se comprueba si el caracter leido es el fin de fichero, lo que indica que se ha terminado
            return 0;
        }
        almacenar(lex, actual);


        if (isalpha(actual) || actual == '_') { //Comprobacion para el caso de lexemas alfanumericos (Estos deben empezar por una letra o una barra baja)
            alfanumerico(lex, actual);
            encontrado = 1;
        } else if (isdigit(actual)) {   //Comprobación para el caso de lexemas numéricos (deben empezar por un dígito)
            digitos(lex, actual);
            encontrado = 1;
        } else if (actual == '\n' || actual == ' ') {   //Caso para espacios y saltos de linea (se ignora y se vuelve a comenzar el bucle

        } else {    //Caso para cuando se detecten símbolos
            //Para casos donde solo se detecte un símbolo de manera independiente, se devuelve el propio código ascii
            switch (actual) {
                case '+':   //Caso cuando se detecta un mas. Puede ser solo + o puede ser +=
                    actual = siguienteCaracter();
                    if (actual == '=') {
                        almacenar(lex, actual);
                        lex->tipo = OP_SUMA_COMP;   //Se asigna el componente léxico correspondeinte en el fichero definiciones.h
                        aceptarLexema();
                        encontrado = 1;
                    } else {
                        devolverCaracter();
                        lex->tipo = (short) lex->lexema[strlen(lex->lexema) - 1];
                        aceptarLexema();
                        encontrado = 1;
                    }
                    break;
                case ':':   //Caso cuando se detecta un mas. Puede ser solo : o puede ser :=
                    actual = siguienteCaracter();
                    if (actual == '=') {
                        almacenar(lex, actual);
                        lex->tipo = OP_ASIGNACION;
                        aceptarLexema();
                        encontrado = 1;
                    } else {
                        devolverCaracter();
                        lex->tipo = (short) lex->lexema[strlen(lex->lexema) - 1];
                        aceptarLexema();
                        encontrado = 1;
                    }
                    break;
                case '<':    //Caso cuando se detecta un <. Puede ser solo < o puede ser <-
                    actual = siguienteCaracter();
                    if (actual == '-') {
                        almacenar(lex, actual);
                        lex->tipo = OP_FLECHA;
                        aceptarLexema();
                        encontrado = 1;
                    } else {
                        devolverCaracter();
                        lex->tipo = (short) lex->lexema[strlen(lex->lexema) - 1];
                        aceptarLexema();
                        encontrado = 1;
                    }
                    break;
                case '/':   //Caso para cuando se detecte una barra. Se debe hacer comprobaciones para ver si es un comentario
                    actual = siguienteCaracter();
                    almacenar(lex, actual);
                    //Comprobaciones para ver si son comentarios. En este caso nunca se almacena el lexema, ya que se ignora
                    if (actual == '/') {    //Caso en el que sea un comentario de barras dobles
                        actual = siguienteCaracter();
                        while (actual != '\n') {
                            actual = siguienteCaracter();
                        }
                        lex->lexema[strlen(lex->lexema) - 2] = '\0';
                        aceptarLexema();
                    } else if (actual == '*') {   //Caso para un comentario con asterisco
                        short acabar = 0;
                        while (!acabar) {
                            actual = siguienteCaracter();
                            if (actual == '*') {    //Si llega otro asterisco, solo termina si inmediatamente despues llega una barra
                                actual = siguienteCaracter();
                                if (actual == '/') {
                                    acabar = 1;
                                }
                            }
                        }
                        //
                        lex->lexema[strlen(lex->lexema) - 2] = '\0';
                        aceptarLexema();
                    } else {    //Caso donde se detecte una barra de manera individual. Se debe eliminar el último caracter leido
                        devolverCaracter();
                        lex->lexema[strlen(lex->lexema) - 1] = '\0';
                        lex->tipo = (short) actual;
                        aceptarLexema();
                        encontrado = 1;
                    }
                    break;
                case '"':   //Caso para cuando se detecte comillas, lo cual indica comienzo de un string

                    actual = siguienteCaracter();
                    almacenar(lex, actual);
                    //Se realiza un bucle hasta que se detectan unas comillas dobles. Si se detectan comillas pero va precedido de barra, no corta el string. Si se detecta comillas precedida de barra, pero esta tiene otra barra entes, si se corta el string
                    //Se detecta "xx" -> se termina el string. Se detecta "xx\", no se corta el string. Se detecta "xx\\" -> Si se corta el string, al ser la barra el caracter escapado
                    while (actual != '"' || (lex->lexema[strlen(lex->lexema) - 2] == '\\' &&
                                            lex->lexema[strlen(lex->lexema) - 3] != '\\')) {
                        //caso donde se detecta unas comillas despues de una barra. La barra no se incluye dentro del string
                        //No sirve en el caso que antes de la barra vaya otra barra, ya que se debe incluir la barra antes de las comillas y no eliminarla
                        if (actual == '"' && lex->lexema[strlen(lex->lexema) - 2] == '\\' &&
                            lex->lexema[strlen(lex->lexema) - 3] != '\\') {
                            lex->lexema[strlen(lex->lexema) - 2] = '\0';
                            almacenar(lex, actual);
                        }
                        //Se van almacenando todos los elementos de la cadena que se encuentran entre "".
                        actual = siguienteCaracter();
                        almacenar(lex, actual);
                    }
                    //Una vez se corta el string, se acepta el lexema
                    aceptarLexema();
                    lex->tipo = STRING;
                    encontrado = 1;
                    break;
                case '.':   //Caso donde se detecte punto. Puede ser un punto separador o el comienzo de un digito float
                    if(digitos(lex, actual)==1){    //Se comprueba la funcion digitos, la cual devuelve 0 en el caso de que el lexema empieze por un punto y no sea un número. Devuelve 1 cuando si es un número válido
                        encontrado=1;
                    }else{      //Caso para cuando el punto sea un operador
                        lex->tipo=(short)lex->lexema[0];
                        encontrado=1;
                    }
                    break;
                    //Casos para el resto de símbolos que no tienen combinaciones con otros. Se devuelve su código ascii como su componente léxico
                case '(':
                case ')':
                case '[':
                case ']':
                case ',':
                case '{':
                case '}':
                case '=':
                case ';':
                case '-':
                case '*':
                    lex->tipo = (short) lex->lexema[strlen(lex->lexema) - 1];
                    aceptarLexema();
                    encontrado = 1;
                    break;
            }
        }
    }
    return 1;
}

