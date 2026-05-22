#include "utils.h"
#include "simulacion.h"
#include "directorios.h"
#include <time.h>
#include "argparse.h"
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
  init_parser(3, argc, argv);
  add_arg("disco", true, STRING, "Nombre del disco");
  add_arg("path", true, STRING, "Ruta de la simulación a verificar");
  if(parse_args()==MISSING_ARGS_ERROR){
    print_help();
    return FALLO;
  }
  if(bmount(arg_value("disco")) == FALLO){
    return FALLO;
  }
  char output_path[1024];
  char *sim_path = arg_value("path");
  struct STAT stat;
  mi_stat(sim_path, &stat);
  if(stat.tamEnBytesLog/sizeof(entrada)!=PROCESOS){
    xpperror("ERROR: el número de entradas no coincide con el número de procesos", RED, DEFAULT, true, false);
  }
  if(sim_path[strlen(sim_path)-1] == '/')sim_path[strlen(sim_path)-1]='\0';
  char dir_buffer[8192];
  char *ptr = dir_buffer;
  mi_dir(sim_path, dir_buffer);
  sprintf(output_path, "%s/informe.txt", sim_path);
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
  int process = 1;
  while (token != NULL) {
    if(i%5 == 4){
      info.pid = atoi(token);
      info.nEscrituras = 0;
      memset(data_path, 0, sizeof(data_path));
      sprintf(data_path, "%s/%s/data.dat", sim_path, token);
      buffer_offset = 0;
      while(buffer_offset<MAXOFFSET){
        memset(regbuffer, 0, sizeof(regbuffer));
        mi_read(data_path, regbuffer, buffer_offset, sizeof(regbuffer));
        for(int j = 0; j<256; j++){
          if(regbuffer[j].pid == info.pid){
            if(info.nEscrituras == 0){
              info.menorPosicion = regbuffer[j];
              info.primeraEscritura = regbuffer[j];
              info.ultimaEscritura = regbuffer[j];
            }
            
            if(regbuffer[j].nRegistro>info.mayorPosicion.nRegistro)info.mayorPosicion = regbuffer[j];
            if(regbuffer[j].nEscritura<info.primeraEscritura.nEscritura)info.primeraEscritura = regbuffer[j];
            if(regbuffer[j].nEscritura>info.ultimaEscritura.nEscritura)info.ultimaEscritura = regbuffer[j];
            info.nEscrituras++;
            if(info.nEscrituras == NUMESCRITURAS)break;
          }
        }
        if(info.nEscrituras == NUMESCRITURAS)break;
        buffer_offset+=256*sizeof(REGISTRO);
      }
      xpperror("[%d) %d escrituras validadas en %s]\n", GRAY, DEFAULT, false, false, process, info.nEscrituras, data_path);
      memset(output_buffer, 0, sizeof(output_buffer));
      new_offset = sprintf(output_buffer, "\nPID: %d\nNumero escrituras: %d\nMenor Posición\t\t%d\t%d\t%sMayor Posición\t\t%d\t%d\t%sPrimera Escritura\t%d\t%d\t%sÚltima Escritura\t%d\t%d\t%s\n", info.pid, info.nEscrituras, info.menorPosicion.nEscritura, info.menorPosicion.nRegistro, ctime(&info.menorPosicion.fecha), info.mayorPosicion.nEscritura, info.mayorPosicion.nRegistro, ctime(&info.mayorPosicion.fecha), info.primeraEscritura.nEscritura, info.primeraEscritura.nRegistro, ctime(&info.primeraEscritura.fecha), info.ultimaEscritura.nEscritura, info.ultimaEscritura.nRegistro, ctime(&info.ultimaEscritura.fecha));
      mi_write(output_path, output_buffer, output_offset, sizeof(output_buffer));
      output_offset+=new_offset;
      process++;
    }
    i++;
    token = strtok_r(NULL, "|", &ptr);
  }
  printf("Output file: ");
  xpprint("%s", BLUE, DEFAULT, false, false, output_path);
  bumount();
  free_args();
  return 0;
}
