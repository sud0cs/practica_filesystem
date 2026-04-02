#include "utils.h"
#include "bloques.h"
#include "ficheros_basico.h"
#include <limits.h>

//Superbloque global del sistema de ficheros
//superblock SB;

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
	superblock SB;

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
	superblock SB;
	bread(SBPOS, &SB);

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
 *   nblocks-> No se usa realmente, pero se mentiene por coherencia
 * Devuelve:
 *   EXITO (0) si se incializa correctamente
*/
int initAI(int nblocks){
    superblock SB;
    bread(SBPOS, &SB);

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
	    	} else{
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

/*
 * escribir_bit()
 * ----------------------------------------------------------
 * Escribe en el mapa de bits el valor del parámetro bit sobre el bit
 * que representa nbloque
 *
 * parámetros
 *  nbloque -> número de bloque en el MB
 *  bit -> indica el nuevo valor del bit (0 o 1)
 *
 * devuelve:
 *  EXITO (0) en caso de que la escritura del bit se realice correctamente
 */
int escribir_bit(unsigned int nbloque, unsigned int bit){
	superblock SB;
	bread(SBPOS, &SB);

	// Encontrar posición del byte en el mapa de bits
	unsigned int posbyteMB = nbloque/8;

	// Encontrar posición del bit en el byte
	unsigned int posbit = nbloque%8;
	
	// Encontrar el número de bloque absoluto a leer
	unsigned int nbloqueMB = posbyteMB/BLOCKSIZE;
	unsigned int nbloqueabs = SB.startMB+nbloqueMB;

	unsigned char bufferMB[BLOCKSIZE];
	bread(nbloqueabs, bufferMB);

	unsigned int posbyte = posbyteMB%BLOCKSIZE;

	unsigned char mascara = 128;
	
	// Se desplaza el primer bit de la máscara para que afecte solo al bit a escribir
	mascara >>= posbit;

	// Modificar el bit utilizando la máscara
	if(bit == 1){
		bufferMB[posbyte] |= mascara;
	} else{
		bufferMB[posbyte] &= ~mascara;
	}

	return bwrite(nbloqueabs, bufferMB);
}

/**
 * leer_bit()
 * ----------------------------------------------------------
 * lee el valor del bit que representa el bloque nbloque en el mapa de bits
 *
 * parámetros:
 *  nbloque -> número de bloque a leer sobre MB
 *
 * devuelve:
 *  valor del bit leído
 */
char leer_bit(unsigned int nbloque){
	superblock SB;
	bread(SBPOS, &SB);

	// Encontrar posición del byte en el mapa de bits
	unsigned int posbyteMB = nbloque/8;

	// Encontrar posición del bit en el byte
	unsigned int posbit = nbloque%8;

	// Encontrar el número de bloque absoluto a leer
	unsigned int nbloqueMB = posbyteMB/BLOCKSIZE;
	unsigned int nbloqueabs = SB.startMB+nbloqueMB;

	unsigned char bufferMB[BLOCKSIZE];
	bread(nbloqueabs, bufferMB);

	unsigned int posbyte = posbyteMB%BLOCKSIZE;

	unsigned char mascara = 128;

	// Se desplaza el primer bit de la máscara y se mantiene solamente el bit a leer
	mascara >>= posbit;
	mascara &= bufferMB[posbyte];
	
	//Se desplaza el bit leído hasta la última posición del byte de forma que el resultado sea 0 o 1
	mascara >>= (7-posbit);

	return mascara;
}

/**
 * reservar_bloque()
 * ----------------------------------------------------------
 * Encuentra el primer bit en el MB cuyo valor esté a 0 (desocupado), 
 * lo cambia a 1 (ocupado) y decrementa la cantidad de bloques libres
 * 
 * parámetros:
 *
 * devuelve:
 *  Número de bloque reservado
 */
int reservar_bloque(){
	superblock SB;
	bread(SBPOS, &SB);

	// Devolver fallo si no hay bloques disponibles
	if(SB.freeBlocks == 0){
		return FALLO;
	}

	unsigned char bufferMB[BLOCKSIZE];
	unsigned char bufferAux[BLOCKSIZE];
	memset(bufferAux, 255, BLOCKSIZE);

	int nbloqueMB = 0;

	// Comparar el buffer leído de MB con un buffer con todos los bits a 1 e ir aumentando nbloqueMB
	while(nbloqueMB < (SB.endMB - SB.startMB + 1)){
		bread(SB.startMB+nbloqueMB, bufferMB);
		if(memcmp(bufferMB, bufferAux, BLOCKSIZE) != 0){
			break;
		}
		nbloqueMB++;
	}

	// Encuentra la posición del primer byte en el buffer que contenga un 0
	int posbyte = 0;
	while(bufferMB[posbyte] == 255){
		posbyte++;
	}

	// Encuentra la posición del bit a 0
	unsigned char mascara = 128;
	int posbit = 0;
	while(bufferMB[posbyte] & mascara){
		mascara >>= 1;
		posbit++;
	}

	// Calcula la posición absoluta del bloque y escribe el bit
	int nbloque = (nbloqueMB*BLOCKSIZE+posbyte)*8+posbit;
	escribir_bit(nbloque, 1);

	// Actualizar superbloque
	SB.freeBlocks--;
	bwrite(SBPOS, &SB);

	unsigned char buffer[BLOCKSIZE];
	memset(buffer, 0, BLOCKSIZE);
	bwrite(nbloque, buffer);

	return nbloque;
}

/**
 * liberar_bloque()
 * ----------------------------------------------------------
 * Pone a 0 el bit que representa el bloque nbloque en el mapa de bits
 * e incrementa la cantidad de bloques libres
 * 
 * parámetros:
 *  nbloque -> número de bloque a liberar
 *
 * devuelve:
 *  número de bloque liberado
 */
int liberar_bloque(unsigned int nbloque){
	superblock SB;
	bread(SBPOS, &SB);

	// Pone a 0 el bit de MB que representa nbloque
	escribir_bit(nbloque, 0);

	// Actualizar superbloque
	SB.freeBlocks++;
	bwrite(SBPOS, &SB);

	return nbloque;
}


/**
 * escribir_inodo()
 * ----------------------------------------------------------
 * Escribe el contenido de un inodo sobre la posición indicada
 * en el array de inodos
 * 
 * parámetros:
 *  ninodo -> posición en el array de inodos en la cual escribir
 *  inodo -> inodo a escribir
 *
 * devuelve:
 *  EXITO (0) en caso de que la escritura sea realizada correctamente
 *
 */
int escribir_inodo(unsigned int ninodo, inode *inodo){
	superblock SB;
	bread(SBPOS, &SB);

	// Calcula la posición del bloque en la que se encuentra el inodo en el array de inodos
	unsigned int nbloqueAI = (ninodo*INODESIZE)/BLOCKSIZE;

	// Calcula la posición absoluta del bloque
	unsigned int nbloqueabs = SB.startAI+nbloqueAI;

	// Actualiza el inodo y lo escribe
	inode inodos[BLOCKSIZE/INODESIZE];
	if(bread(nbloqueabs, inodos)==FALLO) return FALLO;
	unsigned int posinodo = ninodo%(BLOCKSIZE/INODESIZE);
	inodos[posinodo] = *inodo;
	return bwrite(nbloqueabs, inodos);
}

/**
 * leer_inodo()
 * ----------------------------------------------------------
 * Lee el inodo contenido en la posición indicada del array de inodos
 * 
 * parámetros:
 *  ninodo -> posición en el array de inodos de la cual leer
 *  inodo -> variable sobre la que almacenar la información leída
 *
 * devuelve:
 *  EXITO (0) si se ha realizado la lectura correctamente, en caso contrario FALLO;
 *
 */
int leer_inodo(unsigned int ninodo, inode *inodo){
	superblock SB;
	bread(SBPOS, &SB);

	// Calcula la posición del bloque en la que se encuentra el inodo en el array de inodos
	unsigned int nblqoueAI = (ninodo*INODESIZE)/BLOCKSIZE;

	// Calcula la posición absoluta del bloque
	unsigned int nbloqueabs = SB.startAI+nblqoueAI;

	// Lee el inodo y lo escribe sobre el puntero inodo
	inode inodos[BLOCKSIZE/INODESIZE];
	if(bread(nbloqueabs, inodos) == FALLO) return FALLO;
	unsigned int posinodo = ninodo%(BLOCKSIZE/INODESIZE);
	*inodo = inodos[posinodo];

	return EXITO;
}

/**
 * reservar_inodo()
 * ----------------------------------------------------------
 * Encuentra el primer inodo libre, escribe datos sobre este en el AI
 * y lo elimina de la lista de inodos enlazados
 * 
 * parámetros:
 *  tipo -> tipo de dato que representa el inodo:
 *      - 'l':libre
 *      - 'd':directorio
 *      - 'f':fichero
 *
 *  permisos -> permisos del inodo (0-7)
 *
 * devuelve:
 *  Número del inodo reservado. FALLO en caso de producire algún error (no inodos libres, error de lectura/escritura)
 *
 */
int reservar_inodo(unsigned char tipo, unsigned char permisos){
	superblock SB;
	bread(SBPOS, &SB);

	if(SB.freeInodes == 0){
		return FALLO;
	}

	// Lee el primer inodo libre
	unsigned int posInodoReservado = SB.firstFreeInode;
	inode inodo;
	leer_inodo(posInodoReservado, &inodo);

	// Actualiza el primer inodo libre en el superbloque
	SB.firstFreeInode = inodo.directPointers[0];

	// Actualiza los datos del inodo
	inodo.type = tipo;
	inodo.perms = permisos;
	inodo.nlinks = 1;
	inodo.logicByteSize = 0;
	inodo.usedBlocks = 0;

	time_t now = time(NULL);
	inodo.atime = now;
	inodo.mtime = now;
	inodo.ctime = now;
	inodo.btime = now;

	// Pone todos los punteros de datos a 0
	memset(inodo.directPointers, 0, sizeof(inodo.directPointers));
	memset(inodo.indirectPointers, 0, sizeof(inodo.indirectPointers));

	// Escribe el inodo
	escribir_inodo(posInodoReservado, &inodo);

	// Actualiza el superbloque
	SB.freeInodes--;
	bwrite(SBPOS, &SB);

	return posInodoReservado;
}

/**
 * get_block_rank()
 * ----------------------------------------------------------
 * Encuentra el rango en el cual se está el bloque lógico y guarda la dirección
 * a la que apunta
 * 
 * parámetros:
 *  ptrinode -> puntero al inodo al que pertenece el bloque lógico
 *  logicblock -> número de bloque lógico
 *  ptr -> puntero a un entero sin signo en el cual se guardará la dirección a la que se apunta
 *
 * devuelve:
 *  El rango en el cual se encuentra el bloque lógico.
 *
 */
int get_block_rank(inode *ptrinode, int logicblock, unsigned int *ptr){
    switch(logicblock){
	// logicblock se encuentra en el rango 0 - DIRECT
	// NOTA: No todos los compiladores pueden compilar esta sintaxi
	// GCC que es el que usamos en esta asignatura si lo acepta
	case 0 ... DIRECT:
	    *ptr = ptrinode -> directPointers[logicblock];
	    return 0;
	//logicblock se encuentra en el rango DIRECT + 1 - INDIRECT1
	case DIRECT+1 ... INDIRECT0:
	    *ptr = ptrinode -> indirectPointers[0];
	    return 1;
	case INDIRECT0+1 ... INDIRECT1:
	    *ptr = ptrinode -> indirectPointers[1];
	    return 2;
	case INDIRECT1+1 ... INDIRECT2:
	    *ptr = ptrinode -> indirectPointers[2];
	    return 3;
	default:
	    *ptr = 0;
	    xpperror("logic block does not exist", RED, DEFAULT, true, false);
	    return -1;
    }
}


/**
 * translate_inode_block()
 * ----------------------------------------------------------
 * Dada la dirección de un bloque lógico perteneciente a un inodo
 * devuelve la dirección global de este bloque
 * 
 * parámetros:
 *  ninodo -> identificador del inodo
 *  logicblock -> número de bloque lógico
 *  reserve -> true si se tienen que reservar los bloques que no existen
 *
 * devuelve:
 *  La dirección global del bloque lógico
 *
 */
int translate_inode_block(unsigned int ninode, unsigned int logicblock, bool reserve){
    inode ptrinode;
    unsigned int ptr, pptr;
    leer_inodo(ninode, &ptrinode);
    int rank = get_block_rank(&ptrinode, logicblock, &ptr);
    bool update_inode = false;
    if (rank == 0) {
	if(ptr == 0){
	    if(!reserve) return FALLO;
	    ptr = reservar_bloque();
	    xpperror("\n[ translate_inode_block() -> inode.directPointers[%d] = %d ]", GRAY, DEFAULT, false, false, logicblock, ptr);
	    ptrinode.directPointers[logicblock] = ptr;
	    ptrinode.usedBlocks++;
	    ptrinode.ctime = time(NULL);
	    update_inode = true;
	}
    }
    else{
	unsigned int buffer[NPOINTERS];
	memset(buffer, 0, BLOCKSIZE);
	unsigned int block;
	int blvl = rank;
	int arr = 0;
	while(rank>0){
	    switch(rank + arr){
		case 3:
		    block = (logicblock-INDIRECT1)/(256*256);
		    break;
		case 2:
		    block = ((logicblock-INDIRECT0)/256)%256;
		    break;
		case 1:
		    block = (logicblock-DIRECT)%256;
		    break;
	    }
	    if(ptr == 0){
		if(!reserve) return FALLO;
		ptr = reservar_bloque();
		if (rank==blvl && !arr) {
		    ptrinode.indirectPointers[rank-1] = ptr;
		    xpprint("\n[ translate_inode_block() -> inode.indirectPointers[%d] = %d ]", GRAY, DEFAULT, false, false, rank, ptr);
		}
		else{
		    buffer[block] = ptr;
		    xpprint("\n[ translate_inode_block() -> inode.rank_%d_pointer[%d] = %d ]", GRAY, DEFAULT, false, false, rank, block, ptr);
		    bwrite(pptr, buffer);
		}
		memset(buffer, 0, BLOCKSIZE);
		ptrinode.usedBlocks++;
		ptrinode.ctime = time(NULL);
		update_inode = true;
	    }
	    else{
		if (blvl == rank){rank++; arr=1;}
		bread(ptr, buffer);
	    }
	    pptr = ptr;
	    ptr = buffer[block];
	    rank--;
	}
	if (ptr==0 && !arr){
	    ptr = reservar_bloque();
	    buffer[block] = ptr;
	    xpprint("\n[ translate_inode_block() -> inode.rank_%d_pointer[%d] = %d ]", GRAY, DEFAULT, false, false, rank, block, ptr);
	    bwrite(pptr, buffer);
	    ptrinode.usedBlocks++;
	    ptrinode.ctime = time(NULL);
	    update_inode = true;
	}
    }
    if (update_inode) escribir_inodo(ninode, &ptrinode);
    return ptr;
}
