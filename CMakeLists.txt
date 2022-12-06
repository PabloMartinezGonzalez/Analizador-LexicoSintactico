cmake_minimum_required(VERSION 3.21)
project(untitled C)

set(CMAKE_C_STANDARD 99)

add_executable(untitled main.c errores.c errores.h definiciones.h sistemaEntrada.c sistemaEntrada.h tablaSimbolos.c tablaSimbolos.h arbolBinario.c arbolBinario.h analizadorLexico.c analizadorLexico.h analizadorSintactico.c analizadorSintactico.h)
