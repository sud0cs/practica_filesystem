#include "directorios.h"

/*
 * main()
 * ----------------------------------------------------------
 * Escribe texto en un fichero del sistema de ficheros.
 * 
 * Monta el disco virtual, convierte los argumento a valores numéricos,
 * llama a mi_write() para escribir el texto en la posición indicada
 * y desmonta el disco.
 * 
 * Parámetros(argv):
 *   argv[1]: nombre del dispositivo(disco virtual)
 *   argv[2]: ruta absoluta del fichero donde escribir
 *   argv[3]: texto a escribir
 *   argv[4]: offset dentro del fichero
 * 
 * Devuelve:
 *   EXITO(0) si todo va bien
 *   FALLO(-1) si ocurre algún error
*/
int main(int argc, char **argv){
    //Comprobar número de argumentos
    if(argc<5){
	    fprintf(stderr, "mi_escribir <disco> <path> <texto> <offset>\n");
	    return FALLO;
    }

    //Montar el disco virtual
    if(bmount(argv[1]) == FALLO) return FALLO;

    //Convertir offset a entero
    int offset = atoi(argv[4]);

    //Escribir el texto en el fichero indicado
    int err = mi_write(argv[2], argv[3], offset, strlen(argv[3]));
    
    //Comprobar errores de escritura
    if(err<0){
	    print_dir_error(err);
	    bumount();
	    return FALLO;
    }
    
    //Desmontar el disco
    bumount();
}
