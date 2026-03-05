#include "utils.h"
#include "bloques.h"
#include "ficheros_basico.h"
#include <limits.h>
#include <string.h>
#include <time.h>

superblock SB;

int tamMB(int nblocks){
    int bytes = (nblocks+7)/8;
    int blocks = (bytes + BLOCKSIZE - 1) / BLOCKSIZE;
    return blocks;
}

int tamAI(int ninodes){
    int bytes = ninodes * INODESIZE;
    int blocks = (bytes + BLOCKSIZE - 1) / BLOCKSIZE;
    return blocks;
}


int initSB(int nblocks, int ninodes){
    SB.startMB = SBPOS + SBSIZE;
    SB.endMB = SB.startMB + tamMB(nblocks) - 1;
    SB.startAI = SB.endMB + 1;
    SB.endAI = SB.startAI + tamAI(ninodes) - 1;
    SB.startData = SB.endAI + 1;
    SB.endData = nblocks - 1;
    SB.rootInode = 0;
    SB.firstFreeInode = 1;
    SB.totalBlocks = nblocks;
    SB.totalInodes = ninodes;

    //Blocs ocupats pel sistema
    int systemBlocks = 1 + tamMB(nblocks) + tamAI(ninodes) + 1; //SB + MB + AI + arrel
    SB.freeBlocks = nblocks - systemBlocks;
    SB.freeInodes = ninodes - 1;

    return bwrite(SBPOS, &SB);
}

int initMB(int nblocks){
    int systemBlocks = 1 + tamMB(nblocks) + tamAI(SB.totalInodes) + 1;
    int totalMB = tamMB(nblocks);

    unsigned char buffer[BLOCKSIZE];
    int bitsPerBlock = BLOCKSIZE*8;
    int bitsLeft = systemBlocks;

    for (int b = 0; b < totalMB; b++){
        memset(buffer, 0, BLOCKSIZE);

        for(int bit = 0; bit < bitsPerBlock && bitsLeft > 0; bit++, bitsLeft--){
            int byte = bit/8;
            int bitPos = bit%8;
            buffer[byte] |= (128 >> bitPos);
        }
        bwrite(SB.startMB + b, buffer);
    }
    return EXITO;
}

int initAI(int ninodes){
    int sz = BLOCKSIZE/INODESIZE;
    inode inodes[sz];

    int icount = 0;
    int bcount = 0;
    
    while(icount<ninodes){
        memset(inodes, 0, sizeof(inodes));

	    for(int i = 0; i<sz && icount < ninodes; i++, icount++){
	        if(icount==0){
                //Inodo raíz
		        inodes[i].type = 'd';
                inodes[i].perms = 7;
                inodes[i].nlinks = 1;
                inodes[i].logicByteSize = 0;
                inodes[i].usedBlocks = 1;
    		    inodes[i].directPointers[0] = SB.startData;
                inodes[i].ctime = inodes[i].mtime = inodes[i].atime = time(NULL);
	        }
	        else{
                //Inodos libres enazados
    		    inodes[i].type = 'l';
	    	    inodes[i].directPointers[0] = (icount == ninodes-1) ? UINT_MAX : icount+1;
	        }
	    }
	    bwrite(SB.startAI + bcount, inodes);
	    bcount++;
    }
    return EXITO;
}