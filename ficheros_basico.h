#ifndef FICHEROS_BASICO_H
#define FICHEROS_BASICO_H
#include "bloques.h"
#include <time.h>
#include <limits.h>
#include <stdbool.h>
#include <math.h>

#define SBPOS 0 //El superbloque se escribe en el primer bloque de nuestro FS
#define SBSIZE 1
#define INODESIZE 128 //tamaño en bytes de un inodo

#define NPOINTERS (BLOCKSIZE / sizeof(unsigned int))   // 256 punteros por bloque
#define DIRECT 12
#define INDIRECT0 (NPOINTERS + DIRECT)    // 268
#define INDIRECT1 (NPOINTERS * NPOINTERS + INDIRECT0)    // 65.804
#define INDIRECT2 (NPOINTERS * NPOINTERS * NPOINTERS + INDIRECT1) // 16.843.020
#define DBGLVL4 1
#define DBGLVL6 1

#define PERM_EXEC 1
#define PERM_WRITE 2
#define PERM_READ 4

typedef struct{
   unsigned int startMB; //Posición absoluta del primer bloque del mada de bits
   unsigned int endMB; //Posición absoluta del último bloque del mapa de bits
   unsigned int startAI; //Posición absoluta del primer bloque del array de inodos
   unsigned int endAI; //Posición absoluta del último bloque del array de inodos
   unsigned int startData; //Posición absoluta del primer bloque de datos
   unsigned int endData; //Posición absluta del último bloque de datos
   unsigned int rootInode; //Posición del inodo del directorio raíz(relativa al AI)
   unsigned int firstFreeInode; //Posición del primer inodo libre (relativa al AI)
   unsigned int freeBlocks; //PosiciónCantidad de bloques libres (en todo el disco)
   unsigned int freeInodes; //Cantidad de inodos libres(en la AI)
   unsigned int totalBlocks; //Cantidad total de bloques del disco
   unsigned int totalInodes; //Cantidad total de inodos(heurística)
   char padding[BLOCKSIZE - 12 * sizeof(unsigned int)]; //Relleno para ocupar el bloque completo
} superblock;

typedef struct{ //Comprobar que ocupa 128 bytes haciendo un sizeof(inodo)!!!
   unsigned char type; //Tipo ('l':libre, 'd':directorio o 'f':fichero)
   unsigned char perms; //Permisos (lectura y/o escritura y/o ejecución)

   /* Por cuestiones internas de alineación de estructuras, si se está utilizando
   un tamaño de palabra de 4 bytes (microprocesadores de 32 bits):
   unsigned char reservado_alineacion1 [2];
   en caso de que la palabra utilizada sea del tamaño de 8 bytes
(  microprocesadores de 64 bits): unsigned char reservado_alineacion1 [6]; */
   unsigned char alignment[6];

   time_t atime; //Fecha y hora del último acceso a datos(contenido)
   time_t mtime; //Fecha y hora de la última modificación de datos(contenido)
   time_t ctime; //Fecha y hora de la última modificación del inodo
   time_t btime; //Fecha y hora de creación del inodo (birth)

   /* comprobar que el tamaño del tipo time_t para vuestra plataforma/compilador es 8:
   printf ("sizeof time_t is: %ld\n", sizeof(time_t)); */

   unsigned int nlinks; //Cantidad de enlaces de entradas en directorio
   unsigned int logicByteSize; //Tamaño en bytes lógicos (EOF)
   unsigned int usedBlocks; //Cantidad de bloques ocupados zona de datos

   unsigned int directPointers[12]; //12 punteros a bloques directos
   unsigned int indirectPointers[3]; /*3 punteros a bloques indirectos:
   1 indirecto simple, 1 indirecto doble, 1 indirecto triple*/

   //Utilizar una variable de lineación si es necesario para vuestra plataforma/compilador
   char padding[INODESIZE - 2 * sizeof(unsigned char) - 4 * sizeof(time_t) - 18 * sizeof(unsigned int) - 6 * sizeof(unsigned char)];
   //Fijarse que también se resta lo que ocupen las variables de alineación utilizadas!!!
} inode;

int tamMB(int nblocks);
int tamAI(int ninodes);
int initMB(int nblocks);
int initSB(int nblocks, int ninodes);
int initAI(int nblocks);
int escribir_bit(unsigned int nbloque, unsigned int bit);
char leer_bit(unsigned int nbloque);
int reservar_bloque();
int liberar_bloque(unsigned int nbloque);
int escribir_inodo(unsigned int ninodo, inode *inodo);
int leer_inodo(unsigned int ninodo, inode *inodo);
int reservar_inodo(unsigned char tipo, unsigned char permisos);
int get_block_rank(inode *ptrinode, int logicblock, unsigned int *ptr);
int translate_inode_block(unsigned int ninode, unsigned int logicblock, bool reserve);
int liberar_bloques_inodo(unsigned int sbl, inode *inodo);
int liberar_inodo(unsigned int ninodo);
bool has_perms(char og_perms, char perms);
#endif
