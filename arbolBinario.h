

typedef struct nodo *abb; //tipo opaco del nodo del arbol


void crea(abb *A);  //Función para crear el arbol
void destruye(abb *A);  //Función para eliminar y liberar memoria del arbol
void inserta(abb *A, char* cadena, short tipo); //Función para insertar elementos en el arbol
short busca_e_inserta(abb *A, char* cadena);    //Función para buscar lexemas en el arbol y devolver su componente léxico
void imprimirArbol(abb *A); //Función para imprimir el arbol


