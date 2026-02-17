#include "bloques.h"
#include "utils.h"

static int descriptor = 0;

/*
 * bmount()
 * ----------------------------------------------------------
 * Abre(o crea) el fichero que hará de dispositivo virtual.
 * Parámetros:
 *   camino ->nombre del fichero/disco virtual
 * Devuelve:
 *   descriptor del fichero si es correcto
 *   FALLO (-1) si hay error
*/
int bmount(const char *camino){
    //Abrimos el fichero con permisos de lectura/escritura y creación
    descriptor = open(camino, O_RDWR | O_CREAT, 0666);

    if(descriptor == -1){
        pperror("Error en bmount(): %s\n", &ERROR_STYLE, strerror(errno));
        return FALLO;
    }
    return descriptor;
}

/*
 * bumount()
 * ----------------------------------------------------------
 * Cierra el fichero del dispositivo virtual.
 * Devuelve:
 *   EXITO(0) si es correcto
 *   FALLO(-1) si hay error
*/
int bumount(){
    if(descriptor == 0){
        xpperror("Advertencia: bumount() llamado sin bmount()\n", YELLOW, DEFAULT, true, false);
        return FALLO;
    }

    if(close(descriptor) == -1){
        pperror("Error en bumount(): %s\n", &ERROR_STYLE, strerror(errno));
        return FALLO;
    }

    descriptor = 0;
    return EXITO;
}

/*
 * bwrite()
 * ----------------------------------------------------------
 * Escribe un bloque completo al dispositivo virtual.
 * Parámetros:
 *   nbloque -> número de bloque físico a escribir
 *   buf -> buffer de memoria con BLOCKSIZE bytes
 * Devuelve:
 *   BLOCKSIZE si es correcto
 *   ERROR (-1) si hay error
*/
int bwrite(unsigned int nbloque, const void *buf){
    //Mover el puntero al bloque correcto
    if(lseek(descriptor, nbloque * BLOCKSIZE, SEEK_SET) == -1){
        pperror("Error en bwrite() -> lseek(): %s\n", &ERROR_STYLE, strerror(errno));
        return FALLO;
    }

    //Escribir el bloque
    int resp = write(descriptor, buf, BLOCKSIZE);

    if(resp == -1){
        pperror("Error en bwrite() -> write(): %s\n", &ERROR_STYLE, strerror(errno));
        return FALLO;
    }

    return resp; //Si todo va bien, será BLOCKSIZE
}


int bread(unsigned int nbloque, void *buf){
    //Movemos el puntero del fichero a la posición adequada
    if(lseek(descriptor, nbloque * BLOCKSIZE, SEEK_SET) == -1){
        pperror("Error en bread() -> lseek(): %s\n", &ERROR_STYLE, strerror(errno));
        return FALLO;
    }

    //Leemos BLOCKSIZE bytes
    int leidos = read(descriptor, buf, BLOCKSIZE);

    if(leidos == -1){
        pperror("Error en bread() -> read(): %s\n", &ERROR_STYLE, strerror(errno));
        return FALLO;
    }

    return leidos;
}
