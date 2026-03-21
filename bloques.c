#include "bloques.h"
#include "utils.h"

//Descriptor del fichero que representa el dispositivo virtual
static int descriptor = 0;

/*
 * bmount()
 * ----------------------------------------------------------
 * Abre(o crea) el fichero que hará de disco virtual.
 * - O_RWD: Permite leer y escribir
 * - O_CREAT: Crea el fichero si no existe
 * - 0666: permisos rrw-rw-rw
 * 
 * Parámetros:
 *   camino ->nombre del fichero/disco virtual
 * Devuelve:
 *   descriptor del fichero si es correcto
 *   FALLO (-1) si hay error
*/
int bmount(const char *camino){
    //Abrimos el fichero con permisos de lectura/escritura y creación
    descriptor = open(camino, O_RDWR | O_CREAT, 0666);

    //Si open() falla, descriptor = -1
    if(descriptor == -1){
        pperror("Error en bmount(): %s\n", &ERROR_STYLE, strerror(errno));
        return FALLO;
    }
    return descriptor; //Devuelve el desccriptor del disco virtual
}

/*
 * bumount()
 * ----------------------------------------------------------
 * Cierra el fichero del dispositivo virtual.
 * Comprueba que se haya hecho bmount() antes
 * 
 * Devuelve:
 *   EXITO(0) si es correcto
 *   FALLO(-1) si hay error
*/
int bumount(){
    //Si descriptor es 0, significa que no no se ha abierto ningun disco
    if(descriptor == 0){
        xpperror("Advertencia: bumount() llamado sin bmount()\n", YELLOW, DEFAULT, true, false);
        return FALLO;
    }

    //Intentamos cerrar el fichero
    if(close(descriptor) == -1){
        pperror("Error en bumount(): %s\n", &ERROR_STYLE, strerror(errno));
        return FALLO;
    }

    descriptor = 0; //Marcamos que ya no hay ningun disco abierto
    return EXITO;
}

/*
 * bwrite()
 * ----------------------------------------------------------
 * Escribe un bloque completo (BLOCKSIZE byte) al dispositivo virtual.
 * Primero movemos el puntero del fichero con lseek(), después escribimos BLOCKSIZE bytes con write()
 * 
 * Parámetros:
 *   nbloque -> número de bloque físico a escribir
 *   buf -> buffer de memoria con con los datos(BLOCKSIZE bytes)
 * Devuelve:
 *   BLOCKSIZE si es correcto
 *   ERROR (-1) si hay error
*/
int bwrite(unsigned int nbloque, const void *buf){
    //Nos situamos en el byte incial del bloque nbloque
    if(lseek(descriptor, nbloque * BLOCKSIZE, SEEK_SET) == -1){
        pperror("Error en bwrite() -> lseek(): %s\n", &ERROR_STYLE, strerror(errno));
        return FALLO;
    }

    //Escribimos el bloque (CLOCKSIZE byte)
    int resp = write(descriptor, buf, BLOCKSIZE);

    if(resp == -1){
        pperror("Error en bwrite() -> write(): %s\n", &ERROR_STYLE, strerror(errno));
        return FALLO;
    }

    return resp; //Si todo va bien, será BLOCKSIZE
}

/*
 * bread()
 * ----------------------------------------------------------
 * Leemos un bloque completo(BLOCKSIZE bytes)
 * Igual que bwrite(), primero hacemos lseek() i despues read()
 * 
 * Parámetros
 *   nbloque -> número de bloque físico a escribir
 *   buf -> buffer de memoria con con los datos(BLOCKSIZE bytes)
 * Devuelve:
 *   BLOCKSIZE si es correcto
 *   ERROR (-1) si hay error
*/
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

    return leidos; //Si todo va bien, será BLOCKSIZE
}
