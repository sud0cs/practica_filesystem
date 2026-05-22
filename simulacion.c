#include "directorios.h"
#include "utils.h"
#include "simulacion.h"
#include "time.h"
#include <sys/wait.h>
#include <signal.h>
#include "argparse.h"
int acabados = 0;

void reaper(){
  pid_t ended;
  signal(SIGCHLD, reaper);
  while ((ended=waitpid(-1, NULL, WNOHANG))>0) {
     acabados++;
  }
}

int main(int argc, char** argv){
  init_parser(1, argc, argv);
  add_arg("disco", true, STRING, "Nombre del disco");
  if(parse_args()==MISSING_ARGS_ERROR){
    print_help();
    return FALLO;
  }

  time_t c_time = time(NULL);
  struct tm *tmctime = localtime(&c_time);
  char path_parent[1024];
  int pid = 0;
  int err;
  strftime(path_parent, sizeof(path_parent), "/simul_%Y%m%d%H%M%S/", tmctime);
char path_child[1024];
  //create directory
  if(bmount(arg_value("disco")) == FALLO){
    return FALLO;
  }
  err = mi_creat(path_parent, PERM_READ | PERM_WRITE);
  if(err<0){
    print_dir_error(err);
    return -1;
  }
  signal(SIGCHLD, reaper);
  for(int i = 0; i<PROCESOS; i++){
    pid = fork();
    srand(time(NULL) + getpid());
    if(pid==0){
      int id = i+1;
      if(bmount(arg_value("disco")) == FALLO){
        return FALLO;
      }
      memset(path_child, 0, sizeof(path_child));
      //se crean el directorio y el archivo en el que almacenar los datos
      strncpy(path_child, path_parent, 1024);
      sprintf(path_child+strlen(path_child), "%d/", getpid());
      err = mi_creat(path_child, PERM_READ | PERM_WRITE);
      if(err<0){
        print_dir_error(err);
        return -1;
      }
      sprintf(path_child+strlen(path_child), "%s", "data.dat");
      err = mi_creat(path_child, PERM_READ | PERM_WRITE);
      if(err<0){
        print_dir_error(err);
        return -1;
      }

      REGISTRO reg;
      int j;
      for(j = 0; j<NUMESCRITURAS; j++){
        
        //cargar datos en el registro
        reg.fecha = time(NULL);
        reg.pid = getpid();
        reg.nEscritura = j+1;
        reg.nRegistro = rand() % REGMAX;
        
        //escribir el registro
        err = mi_write(path_child, &reg, reg.nRegistro*sizeof(REGISTRO), sizeof(REGISTRO));
        if(err<0){
          print_dir_error(err);
          return -1;
        }
        //esperar 0.05 segundos
        usleep(0.05 * MICROSECOND);
      }
      xpperror("[Proceso %d -> Escritos %d/%d registros en %s]\n", GRAY, DEFAULT, false, false,id, j, NUMESCRITURAS, path_child);
      bumount();
      exit(0);
    }
    //esperar 0.15 segundos
    usleep(0.15 * MICROSECOND);
  }
  while(acabados<PROCESOS){
    pause();
  }
  bumount();
  free_args();
  return 0;
}
