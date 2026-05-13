#include "directorios.h"
#define BUFFERSIZE 4096
#define PATHSIZE 1024

/*
 * tree()
 * ----------------------------------------------------------
 * Función recursiva que muestra el contenido de un directorio
 * en formato de árbol, similar al comando 'tree' de Linux.
 * 
 * Para cada entrada del directorio:
 *   -Muestra su nombre con sangrado según la profundidad
 *   -Si es un directorio, desciende recursivamente
 * 
 * Parámetros:
 *   path -> ruta absoluta del directorio a listar
 *   depth -> nivel de profundidad(para generar sangrado visual)
*/
void tree(char *path, unsigned int depth){
    char buffer[BUFFERSIZE];
    char newpath[PATHSIZE];
    char *ptr = buffer;

    memset(buffer, 0, BUFFERSIZE);
    memset(newpath, 0, PATHSIZE);
    
    //Sangrado visual según profundidad
    char space[depth*2+1];
    memset(space, ' ', sizeof(space));
    
    //Obtener listado del directorio
    int err = mi_dir(path, buffer);
    if(err<0){
	    print_dir_error(err);
	    bumount();
	    return;
    }

    //Tokenizar salida de mi_dir()
    char *token = strtok_r(buffer, "|", &ptr);
    unsigned int i = 0;
    bool isdir = false;

    while (token != NULL) {
        //Campo 0: tipo('d' o 'f')
	    if(i%5 == 0){
	        isdir = token[0]=='d';
	    }

        //Campo 4: nombre
	    if(i%5==4){
	        printf("%s|-%s\n",space,token);
	        //Si es directorio, descender recursivamente
            if(isdir){
		        strcpy(newpath, path);
		        strcat(newpath, token);
		        strcat(newpath, "/");
		        tree(newpath,depth+1);
		        memset(newpath, 0, PATHSIZE);
	        }
	    }
	    token = strtok_r(NULL, "|", &ptr);
	    i++;
    }
}


/*
 * main()
 * ----------------------------------------------------------
 * Muestra el árbol de directorios a partir de una ruta absoluta.
 * 
 * Monta el disco virtual, llama a tree() y finalmente desmonta del disco.
 * 
 * Parámetros:
 *   argv[1]: nombre del dispositivo(disco virtual)
 *   argv[2]: ruta absoluta del directorio a mostrar
 * 
 * Devuelve:
 *   EXITO(0) si todo va bien
 *   FALLO(-1) si ocurre algún error
*/
int main(int argc, char **argv){
    //Comprobar número de argumentos
    if(argc<3){
	    fprintf(stderr, "mi_ls <disco> <path>\n");
	    return FALLO;
    }

    //Montar el disco virtual
    bmount(argv[1]);

    //Mostrar árbol
    tree(argv[2], 1);

    //Desmontar disco
    bumount();
    return EXITO;
}
