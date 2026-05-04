#include "directorios.h"
int main(int argc, char **argv){
    if(argc<5){
	fprintf(stderr, "mi_escribir <disco> <path> <texto> <offset>\n");
	return FALLO;
    }
    if(bmount(argv[1]) == FALLO) return FALLO;
    int offset = atoi(argv[4]);
    int err = mi_write(argv[2], argv[3], offset, strlen(argv[3]));
    if(err<0){
	print_dir_error(err);
	bumount();
	return FALLO;
    }
    bumount();
}
