#include "directorios.h"
int main(int argc, char **argv){
    if(argc<3){
	fprintf(stderr, "mi_ls <disco> <path>\n");
	return FALLO;
    }
    int detailed = 0;
    detailed = strcmp(argv[1],"-l") == 0;
    if(argc==4 && !detailed){
	fprintf(stderr, "mi_ls <disco> <path>\n");
	return FALLO;
    }
    bmount(argv[1+detailed]);
    char *path = argv[2+detailed];
    char buffer[2048];
    memset(buffer, 0, 2048);
    int err = mi_dir(path, buffer);
    if(err<0){
	print_dir_error(err);
	bumount();
	return FALLO;
    }
    char* token = strtok(buffer, "|");
    unsigned int i = 0;
    int color;
    if(detailed)printf("Tipo\tPerms\tmTime\t\t\t\tTamaño\tNombre\n--------------------------------------------------------------\n");
    while (token != NULL) {
	if(i%5 == 0)color=strcmp(token, "d")==0?BLUE:DEFAULT;
	if(i%5 == 1 && color==DEFAULT)color=token[2]=='x'?GREEN:DEFAULT;
        if(i%5 == 4)xpprint("%s\n", color, DEFAULT, color!=DEFAULT, false, token);
	else{
	    if(detailed)printf("%s\t",token);
	}
	token = strtok(NULL, "|");
	i++;
    }
    bumount();
    return EXITO;
}
