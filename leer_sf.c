#include "bloques.h"
#include "ficheros_basico.h"
#include "utils.h"
#include "directorios.h"

void mostrar_buscar_entrada(char *camino, char reservar){
  unsigned int p_inodo_dir = 0;
  unsigned int p_inodo = 0;
  unsigned int p_entrada = 0;
  int error;
  printf("\ncamino: %s, reservar: %d\n", camino, reservar);
  if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, reservar, 6)) < 0) {
    print_dir_error(error);
  }
  printf("**********************************************************************\n");
  return;
}


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

    if(argc != 2){
        fprintf(stderr, "Sintaxis: leer_sf <disco>\n");
        return FALLO;
    }

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

      mostrar_buscar_entrada("pruebas/", 1); //ERROR_CAMINO_INCORRECTO
  mostrar_buscar_entrada("/pruebas/", 0); //ERROR_NO_EXISTE_ENTRADA_CONSULTA
  mostrar_buscar_entrada("/pruebas/docs/", 1); //ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO
  mostrar_buscar_entrada("/pruebas/", 1); // creamos /pruebas/
  mostrar_buscar_entrada("/pruebas/docs/", 1); //creamos /pruebas/docs/
  mostrar_buscar_entrada("/pruebas/docs/doc1", 1); //creamos /pruebas/docs/doc1
  mostrar_buscar_entrada("/pruebas/docs/doc1/doc11", 1);  
  //ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO
  mostrar_buscar_entrada("/pruebas/", 1); //ERROR_ENTRADA_YA_EXISTENTE
  mostrar_buscar_entrada("/pruebas/docs/doc1", 0); //consultamos /pruebas/docs/doc1
  mostrar_buscar_entrada("/pruebas/docs/doc1", 1); //ERROR_ENTRADA_YA_EXISTENTE
  mostrar_buscar_entrada("/pruebas/casos/", 1); //creamos /pruebas/casos/
  mostrar_buscar_entrada("/pruebas/docs/doc2", 1); //creamos /pruebas/docs/doc2

    //Desmontamos el disco
    bumount();
}
