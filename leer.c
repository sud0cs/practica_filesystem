#include "ficheros.h"
#include <string.h>
#include <unistd.h>

#define TAMBUFFER 1500

int main(int argc, char **argv){
    if(argc != 3){
        fprintf(stderr, "Sintaxis: leer <disco> <ninodo>\n");
        return FALLO;
    }
    if(bmount(argv[1]) == FALLO) return FALLO;

    unsigned int ninodo = atoi(argv[2]);
    char buffer[TAMBUFFER];
    int offset = 0;
    int leidos = 0;
    int total = 0;

    do{
        memset(buffer, 0, TAMBUFFER);
        leidos = mi_read_f(ninodo, buffer, offset, TAMBUFFER);

        if(leidos < 0){
            fprintf(stderr, "Error en mi_read_f\n");
            bumount();
            return FALLO;
        }

        if(leidos > 0){
            write(1, buffer, leidos);
            offset += leidos;
            total += leidos;
        }

    } while(leidos > 0);

    struct STAT st;
    mi_stat_f(ninodo, &st);

    fprintf(stderr, "\n\nTotal leidos: %d\nTamEnBytesLog: %d\n", total, st.tamEnBytesLog);
    
    bumount();
    return 0;
}