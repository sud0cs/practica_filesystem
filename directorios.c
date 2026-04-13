#include "directorios.h"
#include <string.h>
#include <stdio.h>

int extraer_camino(const char *camino, char *inicial, char *final, char *tipo){
    //Comprobar camino válido
    if(camino==NULL || strlen(camino)==0 || camino[0]!='/'){
        return ERROR_CAMINO_INCORRECTO;
    }

    //Si el camino es solo "/", es la raíz
    if(strcmp(camino, "/")==0){
        strcpy(inicial, "/");
        strcpy(final, "");
        *tipo = 'd';
        return 1; //directorio
    }

    //Copiar camino
    char copia[strlen(camino)+1];
    strcpy(copia, camino);

    //Buscar segunda barra
    char *segunda = strchr(copia+1, '/');

    if(segunda==NULL){
        //No hay más '/', es fichero
        strcpy(inicial, copia+1);
        strcpy(final, "");
        *tipo = 'f';
        return 0;
    }

    //Hay más '/', extraemos inicial
    *segunda='\0';
    strcpy(inicial, copia+1);

    //Extraer final(incluyendo '/')
    strcpy(final, camino+1+strlen(inicial));

    //Determinar tipo
    *tipo='d';
    return 1;
}
