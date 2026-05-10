#include "directorios.h"
#define BUFFERSIZE 4096
#define PATHSIZE 1024
void tree(char *path, unsigned int depth){
    char buffer[BUFFERSIZE];
    char newpath[PATHSIZE];
    char *ptr = buffer;
    memset(buffer, 0, BUFFERSIZE);
    memset(newpath, 0, PATHSIZE);
    char space[depth*2+1];
    memset(space, ' ', sizeof(space));
    int err = mi_dir(path, buffer);
    if(err<0){
	print_dir_error(err);
	bumount();
	return;
    }
    char *token = strtok_r(buffer, "|", &ptr);
    unsigned int i = 0;
    bool isdir = false;
    while (token != NULL) {
	if(i%5 == 0){
	    isdir = token[0]=='d';
	}
	if(i%5==4){
	    printf("%s|-%s\n",space,token);
	    if(isdir){
		strcpy(newpath, path);
		strcat(newpath, token);
		strcat(newpath, "/");
		tree(newpath,depth+1);
		memset(newpath, 0, PATHSIZE);
	    }
	}
	token = strtok_r(NULL, "|", &ptr);
	i++;
    }
}
int main(int argc, char **argv){
    if(argc<3){
	fprintf(stderr, "mi_ls <disco> <path>\n");
	return FALLO;
    }
    bmount(argv[1]);
    tree(argv[2], 1);
    bumount();
    return EXITO;
}
