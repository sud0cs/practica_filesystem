#include "directorios.h"
#define TAMBUFFER BLOCKSIZE*4

int main(int argc, char **argv){
    if(argc<3){
	fprintf(stderr, "mi_escribir <disco> <path>\n");
	return FALLO;
    }
    if(bmount(argv[1]) == FALLO) return FALLO;
    int offset = 0;
    char buffer[TAMBUFFER];
    unsigned int leidos = 0;
    while((leidos=mi_read(argv[2], buffer, offset, TAMBUFFER)) > 0){
	write(1, buffer, leidos);
	offset+=leidos;
    }
    if(leidos<0){
	print_dir_error(leidos);
	bumount();
	return FALLO;
    }
    bumount();
}
