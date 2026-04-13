#include "utils.h"
#include "bloques.h"
#include "ficheros_basico.h"
#include "directorios.h"

/*
 * main()
 * ----------------------------------------------------------
 * Crea y inicializa un disco virtual completo.
 * Convierte los argumentos de entrada, crea un fichero que acctuara como disco virtual,
 * escribe todos los blqoues a 0, incialitza superbloque, mapa de bits y array de inodos
 * y finalmente reserva el inodo raiz.
 * 
 * Paramétros:
 *  argc: numero de argumentos
 *  argv: vector de argumentos
 *           argv[1] = nombre del disco
 *           argv[2] = nombre de bloques
 * 
 * Devuelve:
 *  EXITO(0): si todo ha ido bien
 *  FALLO(-1): si hay algun error
 * 
*/
int main(int argc, char **argv){
  //Convertimos el segundo argumento a entero: numero de bloques del disco
  int nblocks = atoi(argv[2]);

  //Comprovación básica: no podemos crear un disco de 0 bloques
  if(nblocks==0){
    pperror("ERROR: can't create a disk with 0 blocks", &ERROR_STYLE);
    return FALLO;
  }

  //Mensaje informativo con colores
  printf("Creating disk ");
  xpprint("%s", BLUE, DEFAULT, true, false, argv[1]);
  printf(" with ");
  xpprint("%d", BLUE, DEFAULT, true, false, nblocks);
  printf(" blocks");

  //Buffer de un bloque (1024 bytes) inicializando a ceros
  char buffer[BLOCKSIZE];
  memset(buffer, 0, BLOCKSIZE);

  //Abrimos(o creamos) el disco virtual
  if (bmount(argv[1]) == FALLO)return FALLO;

  //Escribe todos los bloques del disco a 0
  //Esto deja el fichero completamente limpio
  for(int i = 0; i < nblocks; i++){
    bwrite(i, buffer);
  }
  
  /*
   * Inicializar el superbloque
   * - nblocks: total de bloques del disco
   * - nblocks/4: numero de inodos
  */
  initSB(nblocks, nblocks/4);
  
  /*
   * Inicializar el mapa de bits
   * Marca como ocupados los bloques de los metadatos (SB, MB, AI)
  */
  initMB(nblocks);

  /*
   * Inicializar el array de inodos
   * Crea la lista de inodos libres
  */
  initAI(nblocks);

  //reserar el inodo raíz
  reservar_inodo('d', 7);

  //Cerramos el disco virtual
  return bumount();
}
