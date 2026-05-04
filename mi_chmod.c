#include "directorios.h"
int main(int argc, char **argv){
    if(argc<4){
	printf("mi_chmod <disco> <permisos> <path>\n");
	return EXITO;
    }
    bmount(argv[1]);
    unsigned char perms = atoi(argv[2]);
    if(perms>7){
	xpperror("Valor máximo para los permisos: 7", RED, DEFAULT, true, false);
    }
    mi_chmod(argv[3], perms);
    bumount();
}
