#include "directorios.h"
#include "argparse.h"
#include <stdio.h>

/*
 * main()
 * ----------------------------------------------------------
 * Mueve un fichero o directorio dentro del sistema de ficheros.
 * 
 * Funcionamiento:
 *   Crear enlace duro al destino(mi_link)
 *   Eliminar la entrada original(mi_unlink)
 * 
 * Parámetros:
 *   disco  STRING
 *   og-path  STRING
 *   new-path  STRING
 * 
 * Devuelve:
 *   EXITO(0) si todo va bien
 *   FALLO(-1) si hay error
*/
int main(int argc, char **argv){
    init_parser(3, argc, argv);
    add_arg("disco",   true, STRING, "Nombre del disco virtual");
    add_arg("og-path",  true, STRING, "Ruta origen");
    add_arg("new-path", true, STRING, "Ruta destino");

    if(parse_args()==MISSING_ARGS_ERROR){
        print_help();
        return FALLO;
    }

    char *disco = arg_value("disco");
    char *origen = arg_value("og-path");
    char *destino = arg_value("new-path");

    //Montar disco
    if(bmount(disco)<0){
        fprintf(stderr, "Error: bmount\n");
        free_args();
        return FALLO;
    }

    //Crear enlace duro al destino
    int r = mi_link(origen, destino);
    if(r<0){
        print_dir_error(r);
        bumount();
        free_args();
        return FALLO;
    }

    //Eliminar la entrada original
    r = mi_unlink(origen);
    if(r<0){
        print_dir_error(r);
        bumount();
        free_args();
        return FALLO;
    }

    bumount();
    free_args();
    return EXITO;
}
