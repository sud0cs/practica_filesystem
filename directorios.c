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

int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos){
    entrada entrada;
    inode inodo_dir;
    bool found;
    char tipo;
    char inicial[sizeof(entrada.nombre)];
    char final[strlen(camino_parcial)];
    int cant_entradas_inodo;
    int num_entrada_inodo;


    found = false;
    if(strcmp(camino_parcial, "/") == 0){
	*p_inodo = 0;
	*p_entrada = 0;
	return 0;
    }

    if(extraer_camino(camino_parcial, inicial, final, &tipo)<0){
	return ERROR_CAMINO_INCORRECTO;
    }

    leer_inodo(*p_inodo_dir, &inodo_dir);
    int buffer[BLOCKSIZE/sizeof(entrada)];

    //add perms

    int cant_entradas_inodo = inodo_dir.logicByteSize/sizeof(entrada);
    int num_entradas_inodo = 0;
    if(cant_entradas_inodo>0){
	
    }
}
