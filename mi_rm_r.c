#include "directorios.h"
#include <stdio.h>
#include <string.h>

int rm_recursive(const char *path){
    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));

    //Intentar listar el contenido
    int r = mi_dir(path, buffer);

    //Si mi_dir falla: no es un directorio, eliminar directamente
    if(r<0) return mi_unlink(path);

    //Tokenizar solida
    char temp[4096];
    strcpy(temp, buffer);

    char *token = strtok(temp, "|");
    int campo = 0;
    char tipo;
    char nombre[256];

    while(token!=NULL){
        if(campo%5 == 0) tipo = token[0];
        if(campo%5 == 4){
            strcpy(nombre, token);

            char fullpath[512];
            strcpy(fullpath, path);
            if(path[strlen(path)-1] != '/') strcat(fullpath, "/");
            strcat(fullpath, nombre);

            if(tipo=='d'){
                rm_recursive(fullpath);
            } else{
                mi_unlink(fullpath);
            }
        }
        token=strtok(NULL, "|");
        campo++;
    }
    return mi_unlink(path);
}

int main(int argc, char **argv){
    if(argc != 3){
        fprintf(stderr, "Sintaxis: %s <disco> </ruta>\n"), argv[0];
        return FALLO;
    }

    if(bmount(argv[1])<0){
        fprintf(stderr, "Error: bmount\n");
        return FALLO;
    }

    int r = rm_recursive(argv[2]);
    if(r<0) print_dir_error(r);

    bumount();
    return r<0 ? FALLO : EXITO;
}
