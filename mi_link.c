#include "directorios.h"
#include <stdio.h>

int main(int argc, char **argv){
    if(argc!=4){
        fprintf(stderr, "Sintaxis: %s <disco> <//ruta_origial> </ruta_enlace\n", argv[0]);
        return -1;
    }

    if(bmount(argv[1])<0){
        fprintf(stderr, "Error: bmount\n");
        return -1;
    }

    int r = mi_link(argv[2], argv[3]);
    if(r<0) print_dir_error(r);

    bumount();
    return r;
}