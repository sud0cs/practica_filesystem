#include "directorios.h"
#include <stdio.h>

/*
 * main()
 * ----------------------------------------------------------
 * Elimina un directorio vació del sistema de ficheros.
 * Internamente utiliza mi_unlink(), que:
 *   Busca la entrada del directorio
 *   Compruba que esté vacío
 *   Elimina la entrada del directorio padre
 *   Libera el inodo si es necesario
 * 
 * Parámetros(argv):
 *   argv[1]: nombre del dispositivo(disco virtual)
 *   argv[2]: ruta absoluta del directorio a eliminar
 * 
 * Devuelve:
 *   EXITO(0) si se elimina correctamente
 *   Código de error si falla
*/
int main(int argc, char **argv){
    if(argc!=3){
        fprintf(stderr, "Sintaxis: mi_rmdir <disco> </ruta_directorio>\n");
    }

    //Montar el disco virtual
    if(bmount(argv[1])<0){
        fprintf(stderr, "Error: bmount\n");
        return -1;
    }

    //Intentar eliminar el directorio
    int r = mi_unlink(argv[2]);
    if(r==ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO){
        fprintf(stderr, "Error: El directorio %s no está vacío\n", argv[2]);
    }else if(r<0){
        print_dir_error(r); //Mostrar error descriptivo
    }

    //Desmontar el disco
    bumount();
    return r;
}