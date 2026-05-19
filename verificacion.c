#include "utils.h"
#include "simulacion.h"
#include "directorios.h"
#include <time.h>
typedef struct {
  int pid;
  unsigned int nEscrituras;
  REGISTRO primeraEscritura;
  REGISTRO ultimaEscritura;
  REGISTRO menorPosicion;
  REGISTRO mayorPosicion;
} INFORMACION;

#define MAXOFFSET sizeof(REGISTRO)*REGMAX

int main(int argc, char **argv){
  bmount(argv[1]);
  char output_path[1024];
  struct STAT stat;
  mi_stat(argv[2], &stat);
  if(stat.tamEnBytesLog/sizeof(entrada)!=PROCESOS){
    xpperror("ERROR: el número de entradas no coincide con el número de procesos", RED, DEFAULT, true, false);
  }
  
  char dir_buffer[4096];
  char *ptr = dir_buffer;
  mi_dir(argv[2], dir_buffer);
  
  sprintf(output_path, "%s/informe.txt", argv[2]);
  mi_creat(output_path, PERM_READ | PERM_WRITE);
  
  char *token = strtok_r(dir_buffer, "|", &ptr);
  int i = 0;
  REGISTRO regbuffer[256];
  
  char data_path[1024];
  INFORMACION info;

  unsigned int buffer_offset = 0;

  unsigned int output_offset = 0;
  unsigned int new_offset;
  char output_buffer[1024];
  while (token != NULL) {
    if(i%5 == 4){
      fprintf(stderr, "%s\n", token);
      info.pid = atoi(token);
      info.nEscrituras = 0;
      sprintf(data_path, "%s/%s/data.dat", argv[2], token);
      buffer_offset = 0;
      while(buffer_offset<MAXOFFSET){
        memset(regbuffer, 0, sizeof(regbuffer));
        mi_read(data_path, regbuffer, buffer_offset, sizeof(regbuffer));
        for(int j = 0; j<256; j++){
          if(regbuffer[j].pid == info.pid){
            xpperror("READ: \t%d\t%d\t%d\t%s", RED, DEFAULT, false, false, regbuffer[j].pid, regbuffer[j].nEscritura, regbuffer[j].nRegistro, ctime(&regbuffer[j].fecha));
            if(info.nEscrituras == 0){
              info.menorPosicion = regbuffer[j];
              info.primeraEscritura = regbuffer[j];
              info.ultimaEscritura = regbuffer[j];
            }
            if(info.nEscrituras == NUMESCRITURAS-1){
              break;
            }
            if(regbuffer[j].nRegistro>info.mayorPosicion.nRegistro)info.mayorPosicion = regbuffer[j];
            if(regbuffer[j].nEscritura<info.primeraEscritura.nEscritura)info.primeraEscritura = regbuffer[j];
            if(regbuffer[j].nEscritura>info.ultimaEscritura.nEscritura)info.ultimaEscritura = regbuffer[j];
            info.nEscrituras++;
          }
        }
        if(info.nEscrituras == NUMESCRITURAS-1)break;
        buffer_offset+=256*sizeof(REGISTRO);
      }

      memset(output_buffer, 0, sizeof(output_buffer));
      new_offset = sprintf(output_buffer, "\nPID: %d\nNumero escrituras: %d\nMenor Posición\t\t%d\t%d\t%sMayor Posición\t\t%d\t%d\t%sPrimera Escritura\t%d\t%d\t%sÚltima Escritura\t\t%d\t%d\t%s\n", info.pid, info.nEscrituras, info.menorPosicion.nEscritura, info.menorPosicion.nRegistro, ctime(&info.menorPosicion.fecha), info.mayorPosicion.nEscritura, info.mayorPosicion.nRegistro, ctime(&info.mayorPosicion.fecha), info.primeraEscritura.nEscritura, info.primeraEscritura.nRegistro, ctime(&info.primeraEscritura.fecha), info.ultimaEscritura.nEscritura, info.ultimaEscritura.nRegistro, ctime(&info.ultimaEscritura.fecha));
      mi_write(output_path, output_buffer, output_offset, sizeof(output_buffer));
      output_offset+=new_offset;
    }
    i++;
    token = strtok_r(NULL, "|", &ptr);
  }
  bumount();
  return 0;
}
