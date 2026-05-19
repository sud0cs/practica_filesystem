#include "directorios.h"
#include "utils.h"
#include "simulacion.h"
#include "time.h"
#include <sys/wait.h>
#include <signal.h>

int acabados = 0;

void reaper(){
  pid_t ended;
  signal(SIGCHLD, reaper);
  while ((ended=waitpid(-1, NULL, WNOHANG))>0) {
     acabados++;
  }
  xpperror("acabados =  %d\n", DEFAULT, DEFAULT, false, false, acabados);
}

int main(int argc, char** argv){
  time_t c_time = time(NULL);
  struct tm *tmctime = localtime(&c_time);
  char path_parent[1024];
  int pid;
  int err;
  fprintf(stderr, "MY PID: %d", getpid());
  strftime(path_parent, sizeof(path_parent), "/simul_%Y%m%d%H%M%S/", tmctime);

  //create directory
  bmount(argv[1]);
  err = mi_creat(path_parent, PERM_READ | PERM_WRITE);
  if(err<0){
    print_dir_error(err);
    return -1;
  }
  fprintf(stderr, "simulation dir: %s\n", path_parent);
  signal(SIGCHLD, reaper);
  for(int i = 0; i<PROCESOS; i++){
    pid = fork();
    srand(time(NULL) + getpid());
    if(pid==0){
      xpperror("--- creating process %d with pid %d ---\n", PURPLE, DEFAULT, false, true, i, getpid());
      bmount(argv[1]);
      char path_child[1024];
      strncpy(path_child, path_parent, 1024);
      sprintf(path_child+strlen(path_child), "%d/", getpid());
      mi_creat(path_child, PERM_READ | PERM_WRITE);
      sprintf(path_child+strlen(path_child), "%s", "data.dat");
      mi_creat(path_child, PERM_READ | PERM_WRITE);
      REGISTRO reg;
      for(int j = 0; j<NUMESCRITURAS; j++){
        reg.fecha = time(NULL);
        reg.pid = getpid();
        reg.nEscritura = j;
        reg.nRegistro = rand() % REGMAX;
        //escribir registro
        xpperror("WRITE: %d/%d AT PROCESS %d (%s)\n", BLUE, DEFAULT, false, false,j+1,NUMESCRITURAS, getpid(), path_child);
        xpperror("CONTENT: \nfecha: %spid: %d\nnEscritura: %d\nnRegistro: %d\n", GREEN, DEFAULT, true, false, ctime(&reg.fecha), reg.pid, reg.nEscritura, reg.nRegistro);
        mi_write(path_child, &reg, reg.nRegistro*sizeof(REGISTRO), sizeof(REGISTRO));
        //esperar 0.05 segundos
        usleep(0.05 * MICROSECOND);
      }
      xpperror("EXITING PROCESS %d\n", DEFAULT, DEFAULT, false, false, getpid());
      bumount();
      xpperror("UMOUNT PROCESS %d\n", RED, DEFAULT, false, false, getpid());
      exit(0);
    }
    //esperar 0.15 segundos
    usleep(0.15 * MICROSECOND);
  }
  while(acabados<PROCESOS){
    pause();
  }
  bumount();
  return 0;
}
