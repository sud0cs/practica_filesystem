#include "utils.h"
#include "bloques.h"
#include "ficheros_basico.h"
#include <limits.h>
superblock SB;
int tamMB(int nblocks){
  int mapblocks = (nblocks/8)/BLOCKSIZE;
  return (nblocks/8)%BLOCKSIZE>0?mapblocks+1:mapblocks;
}

int tamAI(int ninodes){
  int inodesbsz = ninodes*INODESIZE;
  return inodesbsz%BLOCKSIZE>0?(inodesbsz/BLOCKSIZE)+1:inodesbsz/BLOCKSIZE;
}


int initSB(int nblocks, int ninodes){
  SB.startMB = SBPOS + SBSIZE;
  SB.endMB = SB.startMB + tamMB(nblocks) - 1;
  SB.startAI = SB.endMB + 1;
  SB.endAI = SB.startAI + tamAI(ninodes) - 1;
  SB.startData = SB.endAI + 1;
  SB.endData = nblocks - 1;
  SB.firstFreeInode = 0;
  SB.rootInode = 0;
  SB.freeBlocks = nblocks;
  SB.freeInodes = ninodes;
  SB.totalBlocks = nblocks;
  SB.totalInodes = ninodes;
  return bwrite(SBPOS, &SB);
}

int initMB(int nblocks){
    int bs = SB.startData/8;
    if(SB.startData%8>0){
	unsigned char buffer[bs+1];
	int l = 0;
	int i = 0;
	for(;i<SB.startData%8; i++){
	    l+=1;
	    l = l << 1;
	}
	l = l << (7-i);
	buffer[bs] = l;
	for(i=0;i<bs-1;i++){
	    buffer[i] = 255;
	}
	bwrite(SB.startMB, buffer);
    }
    else{
	unsigned char buffer[bs];
	for(int i=0;i<bs;i++){
	    buffer[i] = 255;
	}
	bwrite(SB.startMB, buffer);
    }
    return EXITO;
}

int initAI(int nblocks){
    int sz = BLOCKSIZE/INODESIZE;
    inode inodes[sz];
    int icount = 0;
    int bcount = 0;
    while(icount<SB.totalInodes){
	for(int i = 0; i<sz; i++){
	    icount++;
	    if(icount<SB.totalInodes){
		inodes[i].type = 'l';
		inodes[i].directPointers[0] = icount;
	    }
	    else{
		inodes[i].type = 'l';
		inodes[i].directPointers[0] = UINT_MAX;
		break;
	    }
	}
	bwrite(SB.startAI + bcount, inodes);
	bcount++;
    }

    return EXITO;
}
