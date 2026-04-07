#include "ficheros.h"
#include <string.h>
#include <stdio.h>
#include <time.h>

int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes){
    inode in;
    leer_inodo(ninodo, &in);

    //Comprovar permisos de esccritura (bit 2)
    if((in.perms & 2) != 2){
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

    
    if(primerBL == ultimoBL){ //Caso 1: un solo bloque
        int bf = translate_inode_block(ninodo, primerBL, true);
        if(bf < 0) return FALLO;

        bread(bf, buf_bloque);

        memcpy(buf_bloque + desp1, buf_original, nbytes);
        bwrite(bf, buf_bloque);

        escritos = nbytes;
    } else{ //Caso 2: varios bloques
        //Primer bloque
        int bf = translate_inode_block(ninodo, primerBL, true);
        if(bf < 0) return FALLO;
        
        bread(bf, buf_bloque);
        int tamPrimer = BLOCKSIZE - desp1;
        memcpy(buf_bloque + desp1, buf_original, tamPrimer);
        bwrite(bf, buf_bloque);
        escritos += tamPrimer;

        //Bloques intermedios
        for(unsigned int bl = primerBL + 1; bl < ultimoBL; bl++){
            bf = translate_inode_block(ninodo, bl, true);
            if(bf < 0) return FALLO;

            bwrite(bf, (unsigned char *)buf_original + escritos);
            escritos += BLOCKSIZE;
        }

        //Último bloque
        bf = translate_inode_block(ninodo, ultimoBL, true);
        if(bf < 0) return FALLO;

        bread(bf, buf_bloque);
        int tamUltimo = desp2 + 1;
        memcpy(buf_bloque, (unsigned char *)buf_original + escritos, tamUltimo);
        bwrite(bf, buf_bloque);
        escritos += tamUltimo;
    }
    //Releer el inodo para no perder usedBlocks actualizado en translate_inode_block
    leer_inodo(ninodo, &in);

    //Actualizar metadatos
    unsigned int nuevoTam = offset + escritos;
    if(nuevoTam > in.logicByteSize){
        in.logicByteSize = nuevoTam;
    }

    in.mtime = time(NULL);
    in.ctime = time(NULL);

    escribir_inodo(ninodo, &in);

    return escritos;
}

int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes){
    inode in;
    leer_inodo(ninodo, &in);

    //Comprobar permisos de lectura
    if((in.perms & 4) != 4){
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

    //Calcular distancias
    unsigned int primerBL = offset / BLOCKSIZE;
    unsigned int ultimoBL = (offset + nbytes - 1) / BLOCKSIZE;
    unsigned int desp1 = offset % BLOCKSIZE;
    unsigned int desp2 = (offset + nbytes - 1 ) % BLOCKSIZE;

    unsigned char buf_bloque[BLOCKSIZE];
    int leidos = 0;
    unsigned int restantes = nbytes;

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

int mi_chmod_f(unsigned int ninodo, unsigned char permisos){
    inode in;
    leer_inodo(ninodo, &in);

    in.perms = permisos;
    in.ctime = time(NULL);

    return escribir_inodo(ninodo, &in);
}