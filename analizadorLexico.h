//Estructura para almacenar el componente léxico
typedef struct {
    short tipo;
    char* lexema;
}compLexico;

//Función para ir devolviendo lexemas a medida que se vayan detectando
short siguiente_Lexema(compLexico *lex);