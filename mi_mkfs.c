#include "utils.h"
#include "bloques.h"
#include "ficheros_basico.h"
#include <string.h>

int main(int argc, char **argv){
  int nblocks = atoi(argv[2]);
  if(nblocks==0){
    pperror("ERROR: can't create a disk with 0 blocks", &ERROR_STYLE);
    return FALLO;
  }
  printf("Creating disk ");
  xpprint("%s", BLUE, DEFAULT, true, false, argv[1]);
  printf(" with ");
  xpprint("%d", BLUE, DEFAULT, true, false, nblocks);
  printf(" blocks");

  char buffer[BLOCKSIZE];
  memset(buffer, 0, BLOCKSIZE);

  if (bmount(argv[1]) == FALLO)return FALLO;
  for(int i = 0; i < nblocks; i++){
    bwrite(i, buffer);
  }

  initSB(nblocks, nblocks/4);
  initMB(nblocks);
  initAI(nblocks);

  return bumount();
}