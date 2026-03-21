#include <time.h>
#include <limits.h>

#define SBPOS 0
#define SBSIZE 1
#define INODESIZE 128

typedef struct{
   unsigned int startMB;
   unsigned int endMB;
   unsigned int startAI;
   unsigned int endAI;
   unsigned int startData;
   unsigned int endData;
   unsigned int rootInode;
   unsigned int firstFreeInode;
   unsigned int freeBlocks;
   unsigned int freeInodes;
   unsigned int totalBlocks;
   unsigned int totalInodes;
   char padding[BLOCKSIZE - 12 * sizeof(unsigned int)];
} superblock;

typedef struct{
   unsigned char type;    //'l':free/empty, 'd':directory, 'f':file
   unsigned char perms; //permissions
   unsigned char alignment[6];
   time_t atime; // last access
   time_t mtime; // last content modification
   time_t ctime; // last inode modification
   time_t btime; // inode "birth" / creation
   unsigned int nlinks;
   unsigned int logicByteSize;
   unsigned int usedBlocks;
   unsigned int directPointers[12];
   unsigned int indirectPointers[3];
   char padding[INODESIZE - 8 * sizeof(unsigned char) - 4 * sizeof(time_t) - 18 * sizeof(unsigned int)];
} inode;
int tamMB(int nblocks);
int tamAI(int ninodes);
int initMB(int nblocks);
int initSB(int nblocks, int ninodes);
int initAI(int nblocks);
