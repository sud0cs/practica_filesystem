#include "ficheros.h"
#include "utils.h"
#include <string.h>

//Códigos de error específicados de operaciones sobre directorios
#define ERROR_CAMINO_INCORRECTO (-2) //Ruta inválida o mal formada
#define ERROR_PERMISO_LECTURA (-3) //No hay permisos de lectura
#define ERROR_NO_EXISTE_ENTRADA_CONSULTA (-4) //Entrada no encontrada(modo consulta)
#define ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO (-5) //Falta un directorio en el camino
#define ERROR_PERMISO_ESCRITURA (-6) //No hay permisos de escritura
#define ERROR_ENTRADA_YA_EXISTENTE (-7) //La entrada ya existe(modo creación)
#define ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO (-8) //Intento de crear dentro de un fichero


//Parámetros del sistema de directorios
#define TAMNOMBRE 60 //Tamaño del nombre de directorio o fichero, en Ext2 = 256
#define MAX_PATH_DEPTH 32 //Profundidad máxima de un camino
#define DBGLVL8 0 //Nivel de debug para buscar_entrada()
#define DBGLVL9 0 //Nivel de debug para operaciones con caché
#define CACHE_SIZE 64 //Tamaño de la caché de rutas

/*
 * Estructura de una entrada de directorio
 *   nombre -> nombre del fichero/directorio
 *   ninodo -> número de inodo asociado
*/
typedef struct{
    char nombre[TAMNOMBRE];
    unsigned int ninodo;
}entrada;

/*
 * Canché de rutas para acelerar búsquedas repetidas
 *   last_item_pos -> posición donde insertar el siguiente elemento
 *   items -> número de elementos almacenados
 *   path[] -> rutas completadas almacenadas
 *   p_inode[] -> inodos asociados a cada ruta
*/
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
int mi_link(const char *camino1, const char *camino2);
int mi_unlink(const char *camino);
