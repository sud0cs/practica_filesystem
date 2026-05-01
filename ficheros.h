#ifndef FICHEROS_H
#define FICHEROS_H
#include "ficheros_basico.h"
#include <time.h>

//Estructura STAT(igual que inodo pero sin punteros)
struct STAT{
    unsigned char tipo; //'f' ficheros, 'd' directorio
    unsigned char permisos; //bits rwx(igual que en el inodo)

    time_t atime; //ultimo acceso(lectura)
    time_t mtime; //ultima modificación de datos
    time_t ctime; //ultimo cambio de metadatos
    time_t btime; //fecha de creacion

    unsigned int nlinks; //numero de enlaces duros
    unsigned int tamEnBytesLog; //tamaño logico del fichero en bytes
    unsigned int numBloquesOcupados; //numero de bloques fisicos usados
};

int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes);
int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes);
int mi_stat_f(unsigned int ninodo, struct STAT *p_stat);
int mi_chmod_f(unsigned int ninodo, unsigned char permisos);
int mi_truncar_f(unsigned int ninodo, unsigned int nbytes);
#endif
