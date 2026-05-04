#include "ficheros.h"
#include "utils.h"
#include <string.h>

#define ERROR_CAMINO_INCORRECTO (-2)
#define ERROR_PERMISO_LECTURA (-3)
#define ERROR_NO_EXISTE_ENTRADA_CONSULTA (-4)
#define ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO (-5)
#define ERROR_PERMISO_ESCRITURA (-6)
#define ERROR_ENTRADA_YA_EXISTENTE (-7)
#define ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO (-8)

#define TAMNOMBRE 60 //Tamaño del nombre de directorio o fichero, en Ext2 = 256
#define MAX_PATH_DEPTH 32
#define DBGLVL8 1
#define DBGLVL9 1
#define CACHE_SIZE 64

typedef struct{
    char nombre[TAMNOMBRE];
    unsigned int ninodo;
}entrada;

typedef struct{
    unsigned int last_item_pos;
    unsigned int items;
    char path[CACHE_SIZE][TAMNOMBRE*MAX_PATH_DEPTH];
    unsigned int p_inode[CACHE_SIZE];
}path_cache;

int extraer_camino(const char *camino, char *inicial, char *final, char *tipo);
int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos);
void print_dir_error(int error);
int mi_creat(char *path, unsigned char perms);
int mi_dir(const char *camino, char *str);
int mi_chmod(const char *camino, unsigned char permisos);
int mi_stat(const char *camino, struct STAT *p_stat);
int mi_write(const char *camino, void *buffer, unsigned int offset, unsigned int nbytes);
int mi_read(const char *camino, void *buffer, unsigned int offset, unsigned int nbytes);
