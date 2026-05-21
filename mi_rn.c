#include "directorios.h"
#include <stdio.h>

int main(int argc, char **argv){
    if(argc!=4){
        fprintf(stderr, "Sintaxis: mi_rn <disco> <origen> <destino>\n");
        return FALLO;
    }

    if(bmount(argv[1])<0){
        fprintf(stderr, "Error: bmount\n");
        return FALLO;
    }

    int r = mi_link(argv[2], argv[3]);

    if(r<0){
        print_dir_error(r);
        bumount();
        return FALLO;
    }

    r = mi_unlink(argv[2]);

    if(r<0){
        print_dir_error(r);
        bumount();
        return FALLO;
    }

    bumount();
    return EXITO;
}
