#include "ficheros.h"
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
    entrada _entrada;
    inode inodo_dir;
    bool found;
    char tipo;
    char inicial[sizeof(_entrada.nombre)];
    char final[strlen(camino_parcial)];
    int cant_entradas_inodo;
    int num_entrada_inodo;
    unsigned int offset = 0;

    //xpperror("inodo: %d\n", RED, DEFAULT, true, false, *p_inodo_dir);
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
    if(!inode_has_perms(&inodo_dir, PERM_READ)){
	return ERROR_PERMISO_LECTURA;
    }
    entrada buffer[BLOCKSIZE/sizeof(entrada)];
    memset(buffer, 0, sizeof(buffer));
    cant_entradas_inodo = inodo_dir.logicByteSize/sizeof(entrada);
    num_entrada_inodo = 0;
    if(cant_entradas_inodo>0){
	while((num_entrada_inodo<cant_entradas_inodo) && found==false){
	    if(offset%BLOCKSIZE==0)mi_read_f(*p_inodo_dir, buffer, offset, offset+BLOCKSIZE);
	    _entrada = buffer[num_entrada_inodo%(sizeof(buffer)/sizeof(entrada))];
	    //xpperror("offset: %d, ninodo: %d, nombre: %s, buffer[%d]\n", ORANGE, DEFAULT, true, false, offset, _entrada.ninodo, _entrada.nombre, num_entrada_inodo%(sizeof(buffer)/sizeof(entrada)));
	    if(strcmp(_entrada.nombre, inicial)==0){
		found = true;
		*p_entrada = num_entrada_inodo;
	    }
	    offset+=sizeof(entrada);
	    num_entrada_inodo++;
	}
    }
    //xpperror("entradas: %d | current: %d\n", PURPLE, DEFAULT, true, false,cant_entradas_inodo, num_entrada_inodo);
    if(found==false && num_entrada_inodo==cant_entradas_inodo){
	if (!reservar) return ERROR_NO_EXISTE_ENTRADA_CONSULTA;
	//xpperror("tipo: %c, camino: %s, inodo_dir.type: %c, if %d\n", PURPLE, DEFAULT, true, false, tipo, camino_parcial, inodo_dir.type, inodo_dir.type == 'f');
	if (inodo_dir.type == 'f') return -8;
	if(!inode_has_perms(&inodo_dir, PERM_WRITE)){
	    return ERROR_PERMISO_ESCRITURA;
	}
	memcpy(_entrada.nombre, inicial, sizeof(inicial));
	if(tipo == 'd'){
	    if(strcmp(final, "/")==0)_entrada.ninodo = reservar_inodo('d', permisos);
	    else return ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO;
	}
	else{
	    _entrada.ninodo = reservar_inodo('f', permisos);
	}
	
	buffer[0] = _entrada;
	//xpperror("offset: %d, ninodo: %d, nombre: %s, p_inodo_dir: %d\n", YELLOW, DEFAULT, true, false, offset, buffer[0].ninodo, buffer[0].nombre, *p_inodo_dir);
	mi_write_f(*p_inodo_dir, buffer, offset, sizeof(entrada));
    }
    int l = strlen(final);
    //xpperror("l: %d\n", BLUE, DEFAULT, true, false, l);
    if(l==0 || (l==1 && final[0] == '/')){
	if(found && reservar) return ERROR_ENTRADA_YA_EXISTENTE;
	*p_inodo = _entrada.ninodo;
	*p_entrada = num_entrada_inodo;
	return EXITO;
    }else{
	*p_inodo_dir = _entrada.ninodo;
	return buscar_entrada(final, p_inodo_dir, p_inodo, p_entrada, reservar, permisos);
    }
}

void print_dir_error(int error){
    switch(error){
	case -2:
	    xpperror("ERROR: Camino incorrecto\n", RED, DEFAULT, true, false);
	    break;
	case -3:
	    xpperror("ERROR: No hay permisos de lectura\n", RED, DEFAULT, true, false);
	    break;
	case -4:
	    xpperror("ERROR: El archivo o directorio no existe\n", RED, DEFAULT, true, false);
	    break;
	case -5:
	    xpperror("ERROR: No existe algún directorio intermedio\n", RED, DEFAULT, true, false);
	    break;
	case -6:
	    xpperror("ERROR: No hay permisos de escritura\n", RED, DEFAULT, true, false);
	    break;
	case -7:
	    xpperror("ERROR: El archivo ya existe\n", RED, DEFAULT, true, false);
	    break;
	case -8:
	    xpperror("ERROR: No es un directorio\n", RED, DEFAULT, true, false);
	    break;
    }
}
