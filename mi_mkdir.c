#include "directorios.h"
int main(int argc, char **argv){
    if(argc<4){
	printf("mi_mkdir <disco> <permisos> <path>\n*nota: mkdir imita el comportamiento del programa de linux por lo tanto aunque el path no acabe en / creará un directorio.\nPara crear archivos utilizar mi_touch");
	return EXITO;
    }
    bmount(argv[1]);
    char *path = argv[3];
    char perms = atoi(argv[2]);
    if(path[strlen(path)-1] != '/')strcat(path, "/");
    if(perms>7 || perms<0)return FALLO;
    print_dir_error(mi_creat(path, perms));
    bumount();
    return EXITO;
}
