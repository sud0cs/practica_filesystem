#include "ficheros.h"

int main(int argc, char **argv){
    if(argc != 4){
        fprintf(stderr, "Sintaxis: permitir <disco> <ninodo> <permisos>\n");
        return FALLO;
    }
    bmount(argv[1]);
    
    unsigned int ninodo = atoi(argv[2]);
    unsigned char permisos = atoi(argv[3]);

    mi_chmod_f(ninodo, permisos);

    bumount();
}