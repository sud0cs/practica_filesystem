#include "directorios.h"
#include "argparse.h"
#include <stdio.h>

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

    if(bmount(disco)<0){
        fprintf(stderr, "Error: bmount\n");
        free_args();
        return FALLO;
    }

    int r = mi_link(origen, destino);
    if(r<0){
        print_dir_error(r);
        bumount();
        free_args();
        return FALLO;
    }

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
