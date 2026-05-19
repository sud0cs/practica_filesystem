#include <time.h>
#ifndef SIMULACION_H
#define SIMULACION_H
#define REGMAX 500000 
#define PROCESOS 3
#define NUMESCRITURAS 5
#define MICROSECOND 1000000
typedef struct{
  time_t fecha;
  pid_t pid;
  int nEscritura;
  int nRegistro;
}REGISTRO;
#endif
