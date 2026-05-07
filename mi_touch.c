#include "directorios.h"
int main(int argc, char **argv){
    if(argc<4){
	printf("mi_touch <disco> <path> <permisos>");
	return EXITO;
    }
    bmount(argv[1]);
    char *path = argv[3];
    if(path[strlen(path)-1] == '/'){
	xpperror("Touch no puede crear directorios.", RED, DEFAULT, true, false);
	bumount();
	return FALLO;
    }
    int perms = atoi(argv[2]);
    if(perms>7 || perms<0){
	xpperror("Los permisos deben estar entre 0 y 7", RED, DEFAULT, true, false);
	bumount();
	return FALLO;
    }
    print_dir_error(mi_creat(path, perms));
    bumount();
    return EXITO;
}
