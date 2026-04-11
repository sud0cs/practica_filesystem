#include "ficheros.h"
#include "utils.h"

int main(int argc, char **argv){
    if(argc != 4){
	fprintf(stderr, "Sintaxis: truncar <disco> <ninodo> <nbytes>\n");
	return FALLO;
    }
    bmount(argv[1]);
    unsigned int ninodo = atoi(argv[2]);
    unsigned int nbytes = atoi(argv[3]);
    
    if (nbytes==0) liberar_inodo(ninodo);
    else mi_truncar_f(ninodo, nbytes);
    
    bumount();
}
