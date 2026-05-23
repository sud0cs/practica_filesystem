#include "directorios.h"
#include "argparse.h"
#include <stdio.h>
#include <string.h>

int copy_file(const char *src, const char *dst){
    char buffer[BLOCKSIZE];
    int offset = 0;
    int leidos;

    while((leidos = mi_read(src, buffer, offset, BLOCKSIZE)) > 0){
        mi_write(dst, buffer, offset, leidos);
        offset += leidos;
    }

    return EXITO;
}

int main(int argc, char **argv){
    init_parser(3, argc, argv);
    add_arg("disco", true, STRING, "Nombre del disco virtual");
    add_arg("src", true, STRING, "Ruta origen");
    add_arg("dst", true, STRING, "Ruta destino");

    if(parse_args()==MISSING_ARGS_ERROR){
        print_help();
        return FALLO;
    }

    char *disco = arg_value("disco");
    char *src = arg_value("src");
    char *dst = arg_value("dst");

    if(bmount(disco)<0){
        fprintf(stderr, "Error: bmount\n");
        free_args;
        return FALLO;
    }

    //Intentar copiar como fichero
    int r = copy_file(src, dst);

    //Si no existe como fichero → crear directorio vacío
    if(r<0){
        r = mi_creat(dst, 7);
        if(r < 0) print_dir_error(r);
    }

    bumount();
    free_args();
    return r<0 ? FALLO : EXITO;
}
