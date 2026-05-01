#include "ficheros.h"
#include <string.h>
#include <stdio.h>
#include <time.h>


/*
 * mi_write_f()
 * ----------------------------------------------------------
 * Escribe datos en un fichero representado por un inodo
 * 
 * Combruba permisos de escritura, calcula que blqoues logiqcos intervienen, traduce cada blqoue logica a bloque físico(reservandolo si hace falta),
 * escribe los adtos en uno o varios blqoues y actualiza el tamaño lógico del fichero y las marcas de tiempo.
 * 
 * Parámetros:
 *  ninodo: numero de inodo deonde escribir
 *  buf_original_ buffer con los datos a escribir
 *  offset_ posición inicial dentro del fichero
 *  nbytes: numero de bytes a escribir
 * 
 * Devuelve:
 *  numero de bytes escritos
 *  FALLO(-1): si hay error
*/
int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes){
    inode in;
    leer_inodo(ninodo, &in);

    //Comprobar permisos de escritura (bit 2)
    if(!inode_has_perms(&in, PERM_WRITE)){
        fprintf(stderr, "No hay permisos de escritura\n");
        return FALLO;
    }

    //Cálculo de bloques lógicos y desplazamientos
    unsigned int primerBL = offset / BLOCKSIZE;
    unsigned int ultimoBL = (offset + nbytes - 1) / BLOCKSIZE;
    unsigned int desp1 = offset % BLOCKSIZE;
    unsigned int desp2 = (offset + nbytes - 1) % BLOCKSIZE;

    unsigned char buf_bloque[BLOCKSIZE];
    int escritos = 0;

    
    if(primerBL == ultimoBL){ //Caso 1: todo cabe en un solo bloque lógico
        int bf = translate_inode_block(ninodo, primerBL, true);
        if(bf < 0) return FALLO;

        bread(bf, buf_bloque);

        memcpy(buf_bloque + desp1, buf_original, nbytes);
        bwrite(bf, buf_bloque);

        escritos = nbytes;
    } else{ //Caso 2: varios bloques lógicos
        //Primer bloque parcial
        int bf = translate_inode_block(ninodo, primerBL, true);
        if(bf < 0) return FALLO;
        
        bread(bf, buf_bloque);
        int tamPrimer = BLOCKSIZE - desp1;
        memcpy(buf_bloque + desp1, buf_original, tamPrimer);
        bwrite(bf, buf_bloque);
        escritos += tamPrimer;

        //Bloques intermedios completos
        for(unsigned int bl = primerBL + 1; bl < ultimoBL; bl++){
            bf = translate_inode_block(ninodo, bl, true);
            if(bf < 0) return FALLO;

	bwrite(bf, (unsigned char *)buf_original + escritos);
            escritos += BLOCKSIZE;
        }

        //Último bloque parcial
        bf = translate_inode_block(ninodo, ultimoBL, true);
        if(bf < 0) return FALLO;

        bread(bf, buf_bloque);
        int tamUltimo = desp2 + 1;
        memcpy(buf_bloque, (unsigned char *)buf_original + escritos, tamUltimo);
        bwrite(bf, buf_bloque);
        escritos += tamUltimo;
    }
    //Releer el inodo para obtener usedBlocks actualizado en translate_inode_block
    leer_inodo(ninodo, &in);

    //Actualizar tamaño lógico
    unsigned int nuevoTam = offset + escritos;
    if(nuevoTam > in.logicByteSize){
        in.logicByteSize = nuevoTam;
    }

    //Actualizar tiempos
    in.mtime = time(NULL);
    in.ctime = time(NULL);

    escribir_inodo(ninodo, &in);

    return escritos;
}

/*
 * mi_read_f()
 * ----------------------------------------------------------
 * Lee datos de un fichero representado por un inodo.
 * 
 * Comprueba permisos de lectura, ajusta nbytes para no leer más allá del EOF,
 * traduce bloques lógicos a físicos(sin reservar), copia los datos al buffer
 * del usuario y actualiza el atime.
 * 
 * Parámetros:
 *  ninodo: numero de indodo a leer
 *  buf_original: buffer donde guardar los datos leídos
 *  offset: posición incial dentro del ficheros
 *  nbytes: numero maximo de bytes a leer
 * 
 * Devuelve:
 *  numero de bytes leídos
 *  FALLO(-1) si no hay permisos
*/
int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes){
    inode in;
    leer_inodo(ninodo, &in);

    //Comprobar permisos de lectura (bit 4)
    if(!inode_has_perms(&in, PERM_READ)){
        fprintf(stderr, "No hay permisos de lectura\n");
        return FALLO;
    }

    //No leer más allá del EOF
    if(offset >= in.logicByteSize){
        return 0;
    }

    if(offset + nbytes > in.logicByteSize){
        nbytes = in.logicByteSize - offset;
    }

    //Calcular blqoues lógicos (distancias)
    unsigned int primerBL = offset / BLOCKSIZE;
    unsigned int ultimoBL = (offset + nbytes - 1) / BLOCKSIZE;
    unsigned int desp1 = offset % BLOCKSIZE;
    unsigned int desp2 = (offset + nbytes - 1 ) % BLOCKSIZE;

    unsigned char buf_bloque[BLOCKSIZE];
    int leidos = 0;
    unsigned int restantes = nbytes;
    
    //Leer bloque a bloque
    for(unsigned int bl = primerBL; bl <= ultimoBL; bl++){
        int bf = translate_inode_block(ninodo, bl, 0);

        if(bf != -1){
            bread(bf, buf_bloque);
        } else{
            memset(buf_bloque, 0, BLOCKSIZE);
        }

        unsigned int inicio = (bl == primerBL) ? desp1 : 0;
        unsigned int fin = (bl == ultimoBL) ? desp2 : BLOCKSIZE - 1;

        unsigned int trozo = fin - inicio + 1;
        if(trozo > restantes) trozo = restantes;

        memcpy((unsigned char*)buf_original + leidos, buf_bloque + inicio, trozo);

        leidos += trozo;
        restantes -= trozo;
    }
    //Actualizar atime
    in.atime = time(NULL);
    escribir_inodo(ninodo, &in);

    return leidos;
}

/*
 * mi_stat_f()
 * ----------------------------------------------------------
 * DEvueleve la infomación del inodo en una estructura STAT.
 * 
 * Parámetros:
 *  ninodo: numero de inodo
 *  p_stat: puntero a estructura STAT donde guardar los datos
 * 
 * Devuelve:
 *  EXITO(0)
*/
int mi_stat_f(unsigned int ninodo, struct STAT *p_stat){
    inode in;
    leer_inodo(ninodo, &in);

    p_stat->tipo = in.type;
    p_stat->permisos = in.perms;

    p_stat->atime = in.atime;
    p_stat->mtime = in.mtime;
    p_stat->ctime = in.ctime;
    p_stat->btime = in.btime;

    p_stat->nlinks = in.nlinks;
    p_stat->tamEnBytesLog = in.logicByteSize;
    p_stat->numBloquesOcupados = in.usedBlocks;

    return EXITO;
}

/*
 * mi_chmod_f()
 * ----------------------------------------------------------
 * Cambia los permisos de un inodo.
 * 
 * Parámetros:
 *  ninodo: numero de inodo
 *  permisos: nuevos permisos(bits rwx)
 * 
 * Devuelve:
 * EXITO o FALLO según escribir_inodo()
*/
int mi_chmod_f(unsigned int ninodo, unsigned char permisos){
    inode in;
    leer_inodo(ninodo, &in);

    in.perms = permisos;
    in.ctime = time(NULL);

    return escribir_inodo(ninodo, &in);
}

/*
 * mi_truncar_f()
 * ----------------------------------------------------------
 * Truncar un fichero a un tamaño dado.
 * 
 * Calcula cuanto blqoues deben quedar, llama a liberar_bloques_inodo() para liberar los sobrantes y
 * actualiza tamaño lógico y tiempos.
 * 
 * Parámetros:
 *  ninodo: numero de indod
 *  nbytes: nuevo tamaño lógico del fichero
 * 
 * Devuelve
 *  numero de bloques liberados
*/
int mi_truncar_f(unsigned int ninodo, unsigned int nbytes){
    inode inodo;
    leer_inodo(ninodo, &inodo);

    int sbl = nbytes%BLOCKSIZE==0?nbytes/BLOCKSIZE:nbytes/BLOCKSIZE + 1;
    
    int freed = liberar_bloques_inodo(sbl, &inodo);
    
    inodo.mtime = time(NULL);
    inodo.ctime = time(NULL);
    inodo.logicByteSize = nbytes;
    inodo.usedBlocks-=freed;
    
    escribir_inodo(ninodo, &inodo);
    return freed;
}
