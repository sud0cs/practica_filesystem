#include "directorios.h"
#include <stdio.h>
#include <string.h>

int copy_file(const char *src, const char *dst){
    char buffer[BLOCKSIZE];
    int offset = 0;
    int leidos;

    while((leidos=mi_read(src, buffer, offset, BLOCKSIZE)) > 0){
        mi_write(dst, buffer, offset, leidos);
        offset += leidos;
    }
    return EXITO;
}

int cp_recursive(const char *src, const char *dst){
    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));

    int r = mi_dir(src, buffer);

    //Si no es directorio → copiar fichero
    if(r<0){
        return copy_file(src, dst);
    }

    // Crear directorio destino
    mi_creat(dst, 7);

    // Tokenizar
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

            char srcpath[512], dstpath[512];
            sprintf(srcpath, "%s/%s", src, nombre);
            sprintf(dstpath, "%s/%s", dst, nombre);

            if(tipo=='d'){
                cp_recursive(srcpath, dstpath);
            } else{
                copy_file(srcpath, dstpath);
            }
        }
        token = strtok(NULL, "|");
        campo++;
    }
    return EXITO;
}

int main(int argc, char **argv){
    if(argc!=4){
        fprintf(stderr, "Sintaxis: mi_cp_r <disco> <origen> <destino>\n");
        return FALLO;
    }

    if(bmount(argv[1])<0){
        fprintf(stderr, "Error: bmount\n");
        return FALLO;
    }

    cp_recursive(argv[2], argv[3]);

    bumount();
    return EXITO;
}
