#include "directorios.h"
#include <stdio.h>

int main(int argc, char **argv){
    if(argc!=3){
        fprintf(stderr, "Sintaxis: mi_rmdir <disco> </ruta_directorio>\n");
    }

    if(bmount(argv[1]<0)){
        fprintf(stderr, "Error: bmount\n");
        return -1;
    }

    int r = mi_unlink(argv[2]);
    if(r<0){
        print_dir_error(r);
    }

    bumount();
    return r;
}