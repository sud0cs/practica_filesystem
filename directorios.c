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
    if(!has_perms(inodo_dir.perms, PERM_READ)){
	return ERROR_PERMISO_LECTURA;
    }
    entrada buffer[BLOCKSIZE/sizeof(entrada)];
    memset(buffer, 0, sizeof(buffer));
    cant_entradas_inodo = inodo_dir.logicByteSize/sizeof(entrada);
    num_entrada_inodo = 0;
    if(cant_entradas_inodo>0){
	while((num_entrada_inodo<cant_entradas_inodo) && found==false){
	    if(offset%BLOCKSIZE==0)mi_read_f(*p_inodo_dir, buffer, offset, BLOCKSIZE);
	    _entrada = buffer[num_entrada_inodo%(sizeof(buffer)/sizeof(entrada))];
	    if(strcmp(_entrada.nombre, inicial)==0){
		found = true;
		*p_entrada = num_entrada_inodo;
	    }
	    offset+=sizeof(entrada);
	    num_entrada_inodo++;
	}
    }
    if(found==false && num_entrada_inodo==cant_entradas_inodo){
	if (!reservar) return ERROR_NO_EXISTE_ENTRADA_CONSULTA;
	if (inodo_dir.type == 'f') return ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO;
	if(!has_perms(inodo_dir.perms, PERM_WRITE)){
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
	
	mi_write_f(*p_inodo_dir, &_entrada, offset, sizeof(entrada));
    }
    int l = strlen(final);
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

int mi_creat(char *path, unsigned char perms){
    superblock SB;
    bread(SBPOS, &SB);
    unsigned int rootInode = SB.rootInode;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    return buscar_entrada(path, &rootInode, &p_inodo, &p_entrada, 1, perms);
}

int mi_dir(const char *camino, char *str){
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    unsigned int offset = 0;
    superblock SB;
    bread(SBPOS, &SB);
    inode inodo;
    struct STAT stat_entrada;
    unsigned int p_inodo_dir = SB.rootInode;
    int err = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 0);
    if(err<0)return err;
    leer_inodo(p_inodo, &inodo);
    if(!has_perms(inodo.perms, PERM_READ))return ERROR_PERMISO_ESCRITURA;
    entrada _entrada;
    char out[BLOCKSIZE];
    entrada buffer[BLOCKSIZE/sizeof(entrada)];
    if(inodo.type == 'f'){
	mi_stat_f(p_inodo, &stat_entrada);
	mi_read_f(p_inodo_dir, buffer, sizeof(entrada)*p_entrada, BLOCKSIZE);
	sprintf(str, "%c|%c%c%c|%s|%dB|%s|",
		stat_entrada.tipo,
		has_perms(stat_entrada.permisos, PERM_READ)?'r':'-',
		has_perms(stat_entrada.permisos, PERM_WRITE)?'w':'-',
		has_perms(stat_entrada.permisos, PERM_EXEC)?'x':'-',
		strpl(ctime(&stat_entrada.mtime),"\n", "", 0),
		stat_entrada.tamEnBytesLog,
		buffer[0].nombre);
	return EXITO;
    }
    memset(out, 0, BLOCKSIZE);
    unsigned int strsize = 0;
    while(offset<inodo.logicByteSize){
	if(offset%BLOCKSIZE == 0)mi_read_f(p_inodo, buffer, offset, BLOCKSIZE);
	_entrada = buffer[(offset%BLOCKSIZE)/sizeof(entrada)];
	mi_stat_f(_entrada.ninodo, &stat_entrada);
	    strsize += sprintf(out, "%c|%c%c%c|%s|%dB|%s|",
		stat_entrada.tipo,
		has_perms(stat_entrada.permisos, PERM_READ)?'r':'-',
		has_perms(stat_entrada.permisos, PERM_WRITE)?'w':'-',
		has_perms(stat_entrada.permisos, PERM_EXEC)?'x':'-',
		strpl(ctime(&stat_entrada.mtime),"\n", "", 0),
		stat_entrada.tamEnBytesLog,
		_entrada.nombre);
	if(sizeof(buffer)<strsize)return FALLO;
	strcat(str, out);
	offset+=sizeof(entrada);
    }
    return EXITO;
}

int mi_chmod(const char *camino, unsigned char perms){
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    superblock SB;
    bread(SBPOS, &SB);
    unsigned int p_inodo_dir = SB.rootInode;
    int err = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 0);
    if (err<0)return err;
    return mi_chmod_f(p_inodo, perms);
}

int mi_stat(const char *camino, struct STAT *p_stat){
    superblock SB;
    bread(SBPOS, &SB);
    unsigned int rootInode = SB.rootInode;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int err = buscar_entrada(camino, &rootInode, &p_inodo, &p_entrada, 0, 0);
    if(err<0)return err;
    mi_stat_f(p_inodo, p_stat);
    return p_inodo;
}
