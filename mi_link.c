#include "directorios.h"
#include <stdio.h>

/*
 * main()
 * ----------------------------------------------------------
 * Crea un enlace duro entre dos rutas del sistema de ficheros.
 * Internamente llama a mi_link(), que:
 *   Busca el inodo del ficehro original(camino1)
 *   Crea una nueva entrada de directorio(camino2)
 *   Hace que camino2 apunte al mismo inodo que camino1
 *   Incrementa el contador de enlaces(nlinks)
 * 
 * Parámetros(argv)
 *   argv[1]: nombre del dispositivo(disco virtual)
 *   argv[2]: ruta del fichero original
 *   argv[3]: ruta donde crea el enlace duro
 * 
 * Devuelve:
 *   EXITO(0) si se crea correctamente
 *   Código de error si falla
*/
int main(int argc, char **argv){
    if(argc!=4){
        fprintf(stderr, "Sintaxis: %s <disco> </ruta_original> </ruta_enlace\n", argv[0]);
        return FALLO;
    }

    //Montar el disco virtual
    if(bmount(argv[1])<0){
        fprintf(stderr, "Error: bmount\n");
        return FALLO;
    }

    //Intentar crear el enlace duro
    int r = mi_link(argv[2], argv[3]);
    if(r<0){
        print_dir_error(r); //Montar error descriptivo
        bumount();
        return FALLO;
    }

    //Desmontar el disco
    bumount();
    return EXITO;
}