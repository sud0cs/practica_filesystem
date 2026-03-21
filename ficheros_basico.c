#include "utils.h"
#include "bloques.h"
#include "ficheros_basico.h"
#include <limits.h>

//Superbloque global del sistema de ficheros
superblock SB;

/*
 * tamMB()
 * ----------------------------------------------------------
 * Calcula cuántos bloques ocupa el mapa de bits(MB).
 * Cada bit representa un bloque del disco.
 * Cada bloque del MB contiene BLOCKSIZE * 8 bits.
 * 
 * Parámetro:
 *   nblocks-> numero total de bloques del disco
 * Devuelve:
 *   numero de bloques necesarios para almacenar el MB
*/
int tamMB(int nblocks){
  int mapblocks = (nblocks/8)/BLOCKSIZE;
  return (nblocks/8)%BLOCKSIZE>0?mapblocks+1:mapblocks;
}

/*
 * tamAI()
 * ----------------------------------------------------------
 * Calcula cuantos bloques ocupa el array de inodos(AI).
 * Cada inodo ocupa INODESIZE bytes.
 * 
 * Parámetros:
 *   ninodes-> numero total de inodos del sistema
 * Devuelve:
 *   numero de bloques necesarios para almacenar todos los inodos
*/
int tamAI(int ninodes){
  int inodesbsz = ninodes*INODESIZE;
  return inodesbsz%BLOCKSIZE>0?(inodesbsz/BLOCKSIZE)+1:inodesbsz/BLOCKSIZE;
}

/*
 * initSB()
 * ----------------------------------------------------------
 * Inicializa todos los campos del superbloque segun el enunciado.
 * define las posiciones del MB, AI y zona de datos.
 * Inicializa contadores de bloques e inodos libres.
 * Escribe el superbloque en el bloque 0 del disco.
 * 
 * Parámetros:
 *   nblocks-> numero total de bloques del disco
 *   ninodes-> numero total de inodos(normalmente nblocks/4)
 * Devuelve:
 *   BLOCKSIZE si bwrite() funciona correctamente
 *   FALLO (-1) si hay error
*/
int initSB(int nblocks, int ninodes){
	//Delimitación de las areas del disco
	SB.startMB = SBPOS + SBSIZE;
	SB.endMB = SB.startMB + tamMB(nblocks) - 1;

  	SB.startAI = SB.endMB + 1;
  	SB.endAI = SB.startAI + tamAI(ninodes) - 1;

  	SB.startData = SB.endAI + 1;
  	SB.endData = nblocks - 1;

	//Información del sistema
  	SB.firstFreeInode = 0;
 	SB.rootInode = 0;
  	SB.freeBlocks = nblocks;
 	SB.freeInodes = ninodes;
  	SB.totalBlocks = nblocks;
  	SB.totalInodes = ninodes;

	//Guardamos el superbloque en el bloque 0
  	return bwrite(SBPOS, &SB);
}

/*
 * initMB()
 * ----------------------------------------------------------
 * Inicializa el mapa de bits (MB) marcado como ocupados.
 * Todos los bloques de metadatos: SB + MB + AI.
 * Construye manualmente los bytes del MB.
 * 
 * Parámetros:
 *   nblocks-> numero total de bloques del disco
 * Devuelve:
 *   EXITO(0) si se inicializa correctamente
*/
int initMB(int nblocks){
	//Numero de bits que representen bloques de metadatos
	int bs = SB.startData/8; //Bytes completos que se nencesitan

	//CASO 1: hay bits sobrantes
    if(SB.startData%8>0){
		unsigned char buffer[bs+1];
		int l = 0;
		int i = 0;

		//Construcción del byte parcial
		for(;i<SB.startData%8; i++){
	    	l+=1;
	    	l = l << 1;
		}

		//Desplazamiento final para completar el byte
		l = l << (7-i);

		buffer[bs] = l; //Byte parcial
		
		//Rellenar bytes completos con 255(11111111)
		for(i=0;i<bs-1;i++){
	    	buffer[i] = 255;
		}

		//Escribe el bloque del MB
		bwrite(SB.startMB, buffer);
    }
	//CASO 2: no hay residuo -> todos los bytes son 255
    else{
		unsigned char buffer[bs];
		for(int i=0;i<bs;i++){
	    	buffer[i] = 255;
		}
		bwrite(SB.startMB, buffer);
    }
    return EXITO;
}

/*
 * initAI()
 * ----------------------------------------------------------
 * Inicializa el array de inodos creando la lista enlazada de inodos libres.
 * Para cada inodo:
 *   - type = 'l' (libre)
 *   - directPointers[0] = siguiente inodo libre
 * El último inodo apunta a UINT_MAX.
 * 
 * Parámetros:
 *   nblocks-> No se usa realmente, pero se mentiene poor coherencia
 * Devuelve:
 *   EXITO (0) si se incializa correctamente
*/
int initAI(int nblocks){
    int sz = BLOCKSIZE/INODESIZE; //Inodos por bloque
    inode inodes[sz];

    int icount = 0; //Contador global de inodos
    int bcount = 0; //Contador de bloques del AI
    
	//Recorremos todos los inodos del sistema
	while(icount<SB.totalInodes){
		for(int i = 0; i<sz; i++){
	    	icount++;
	    	if(icount<SB.totalInodes){
				//Inodo libre -> enlazado al siguiente
				inodes[i].type = 'l';
				inodes[i].directPointers[0] = icount;
	    	}
	    	else{
				//Último inodo -> apunta a NULL (UINT_MAX)
				inodes[i].type = 'l';
				inodes[i].directPointers[0] = UINT_MAX;
				break;
	    	}
		}

		//Escribe el bloque de inodos al disco
		bwrite(SB.startAI + bcount, inodes);
		bcount++;
    }

    return EXITO;
}
