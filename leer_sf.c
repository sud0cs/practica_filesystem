#include "bloques.h"
#include "ficheros_basico.h"
#include "utils.h"

/*
 * main()
 * ----------------------------------------------------------
 * Programa para leer y mostrar por pantalla:
 *   - El superbloque del sistema de ficheros
 *   - El tamaño de las estructuras (superblock e inode)
 *   - La lista enlazada de inodos libres
 * 
 * Este programa se usa para comprobar que mi_mks() ha inicializado
 * correctamente el sistema de ficheros.
 * 
 * Parámetros:
 *   argv[1]-> nombre del disco vrtual
 * Devuelve:
 *   EXITO (0) si todo es correcto
 *   FALLO (-1) si ocurre algún error
*/
int main(int argc, char **argv){
    superblock SB;

    //Intentamos montar el disco vitual
    if (bmount(argv[1]) == FALLO){
	    xpperror("Could not mount disk", RED, DEFAULT, true, false);
    }

    //Leemos el superbloque del bloque 0
    bread(SBPOS,&SB);

    //Se muestran todos los campos importantes del SB para verificar que initSB() funcionó correctamente.
    xpprint("###############", SALMON, DEFAULT, false, false);
    xpprint(" SUPERBLOCK ", DEFAULT, DEFAULT, true, false);
    xpprint("###############\n", SALMON, DEFAULT, false, false);
    
    xpprint("# ", SALMON, DEFAULT, false, false);xpprint("Start BitMap",PINK, DEFAULT, true, false);printf(": %d\n", SB.startMB);
    xpprint("# ", SALMON, DEFAULT, false, false);xpprint("End BitMap",PINK, DEFAULT, true, false);printf(": %d\n", SB.endMB);
    xpprint("# ", SALMON, DEFAULT, false, false);xpprint("Start Inode Array",PINK, DEFAULT, true, false);printf(": %d\n", SB.startAI);
    xpprint("# ", SALMON, DEFAULT, false, false);xpprint("End Inode Array",PINK, DEFAULT, true, false);printf(": %d\n", SB.endAI);
    xpprint("# ", SALMON, DEFAULT, false, false);xpprint("Start Data",PINK, DEFAULT, true, false);printf(": %d\n", SB.startData);
    xpprint("# ", SALMON, DEFAULT, false, false);xpprint("End Data",PINK, DEFAULT, true, false);printf(": %d\n", SB.endData);
    xpprint("# ", SALMON, DEFAULT, false, false);xpprint("Root Inode",PINK, DEFAULT, true, false);printf(": %d\n", SB.rootInode);
    xpprint("# ", SALMON, DEFAULT, false, false);xpprint("First Free Inode",PINK, DEFAULT, true, false);printf(": %d\n", SB.firstFreeInode);
    xpprint("# ", SALMON, DEFAULT, false, false);xpprint("Free Blocks",PINK, DEFAULT, true, false);printf(": %d\n", SB.freeBlocks);
    xpprint("# ", SALMON, DEFAULT, false, false);xpprint("FreeInodes",PINK, DEFAULT, true, false);printf(": %d\n", SB.freeInodes);
    xpprint("# ", SALMON, DEFAULT, false, false);xpprint("totalBlocks",PINK, DEFAULT, true, false);printf(": %d\n", SB.totalBlocks);
    xpprint("# ", SALMON, DEFAULT, false, false);xpprint("totalInodes",PINK, DEFAULT, true, false);printf(": %d\n", SB.totalInodes);

    xpprint("##########################################", SALMON, DEFAULT, false, false);

    /*
     * Impresión de tamaños de estructuras
     * Se muestra sizeof(superblock) y siceof(inode) para comprobar que coinciden con:
     *   - superblock = 1024 bytes
     *   - inode = 128 bytes
    */
    xpprint("\n\n###############", GREEN, DEFAULT, false, false);
    xpprint(" STRUCT SIZES ", DEFAULT, DEFAULT, true, false);
    xpprint("###############\n", GREEN, DEFAULT, false, false);

    xpprint("# ", GREEN, DEFAULT, false, false);xpprint("Sizeof superblock", LIGHT_GREEN, DEFAULT, true, false);printf(": %lu\n", sizeof(superblock));
    xpprint("# ", GREEN, DEFAULT, false, false);xpprint("Sizeof inode", LIGHT_GREEN, DEFAULT, true, false);printf( ": %lu\n", sizeof(inode));

    xpprint("############################################\n", GREEN, DEFAULT, false, false);
    
    /*
     * Recorrido de la lista de inodos libres.
     * Se recorre el array de inodos para comprobar que:
     *   - initAI() enlazo correctamente todos los indos libres
     *   - Cada inodo apunta al siguente
     *   - El último apunta a UINT_MAX
    */
    printf("\nChained inodes check\n\n");

    int ni = BLOCKSIZE/INODESIZE;
    bool dots = false;
    inode ibuff[ni];
    int last_inode = SB.rootInode;

    printf("%d -> ", SB.rootInode);
    
    //Recorremos todos los bloques del AI
    for(int i = SB.startAI;i<SB.startData;i++){
	    bread(i, ibuff);
	    for(int j = 0; j<ni; j++){
	        if(ibuff[j].directPointers[0]!=-1){
                //Comprobación estricta del encadenamiento
		        if(i<=SB.startAI||i==SB.endAI){
		            if(ibuff[j].directPointers[0]!=last_inode+1){
                        xpperror("\nERROR: inode %d points to %d", RED, DEFAULT, false, false, last_inode, ibuff[j].directPointers[0]);return FALLO;
                    }
		            printf("%d -> ", ibuff[j].directPointers[0]);
		        } else if(!dots){
                    dots = true; printf("... -> ");
                }
	        } else {
                //Último inodo apunta a UINT_MAX
                xpprint("%d", BLUE, DEFAULT, false, false, ibuff[j].directPointers[0]);
            }
	        last_inode+=1;
	    }
    }
    //Desmontamos el disco
    bumount();
}
