#include "directorios.h"
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
    if(argc!=4){
        fprintf(stderr, "Sintaxis: mi_cp <disco> <origen> <destino>\n");
        return FALLO;
    }

    if(bmount(argv[1])<0){
        fprintf(stderr, "Error: bmount\n");
        return FALLO;
    }

    //Intentar copiar como fichero
    int r = copy_file(argv[2], argv[3]);

    //Si no existe como fichero → crear directorio vacío
    if(r<0){
        r = mi_creat(argv[3], 7);
        if(r < 0) print_dir_error(r);
    }

    bumount();
    return r<0 ? FALLO : EXITO;
}
