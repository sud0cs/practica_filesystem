#include "directorios.h"
#include "argparse.h"
#include <stdio.h>

/*
 * main()
 * ----------------------------------------------------------
 * Renombra un fichero o directorio dentro del sistema de ficheros.
 * 
 * Parámetros:
 *   disco  STRING  -> nombre del disco virtual
 *   og-path  STRING  -> ruta original
 *   new-path  STRING -> nuevo nombre/ruta
 * 
 * Devuelve:
 *   EXITO(0) si todo va bien
 *   FALLO(-1) si hay algun error
*/
int main(int argc, char **argv){
    init_parser(3, argc, argv);
    add_arg("disco", true, STRING, "Nombre del disco virtual");
    add_arg("og-path", true, STRING, "Ruta original");
    add_arg("destino", true, STRING, "Nuevo nombre");

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

    //Crear enlace duro con el nuevo nombre
    int r = mi_link(origen, destino);
    if(r<0){
        print_dir_error(r);
        bumount();
        free_args();
        return FALLO;
    }

    //Eliminar entrada antigua
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
