#include "ficheros.h"
#include <string.h>

int main(int argc, char **argv){
    if(argc != 4){
        fprintf(stderr,
            "Sintaxis: escribir <nombre_dispositivo> <\"$(cat fichero)\"> <diferentes_inodos>\n"
            "Offsets: 9000, 209000, 30725000, 409605000, 480000000\n"
            "Si diferentes_inodos=0 se reserva un solo inodo para todos los offsets\n");
        return FALLO;
    }
    char *disco = argv[1];
    char *texto = argv[2];
    int diferentes_inodos = atoi(argv[3]);

    if(bmount(disco) == FALLO) return FALLO;

    int longitud = strlen(texto);
    printf("longitud texto: %d\n\n", longitud);

    unsigned int offsets[] = {9000, 209000, 30725000, 409605000, 480000000};
    unsigned int ninodo;
    struct STAT st;

    if(diferentes_inodos == 0){
        ninodo = reservar_inodo('f', 6); //rw- rw- ---
        for(int i = 0; i < 5; i++){
            printf("Nº inodo resevado: %d\n", ninodo);
            printf("offset: %u\n", offsets[i]);

            int escritos = mi_write_f(ninodo, texto, offsets[i], longitud);
            mi_stat_f(ninodo, &st);
            
            printf("\nBytes escritos: %d\n", escritos);
            printf("stat.tamEnBytesLog=%d\n", st.tamEnBytesLog);
            printf("stat.numBloquesOcupados=%d\n\n", st.numBloquesOcupados);
        }
    } else{
        for(int i = 0; i < 5; i++){
            ninodo = reservar_inodo('f', 6);

            printf("Nº inodo reservado: %d\n", ninodo);
            printf("offset: %u\n", offsets[i]);

            int escritos = mi_write_f(ninodo, texto, offsets[i], longitud);
            mi_stat_f(ninodo, &st);

            printf("Bytes escritos: %d\n", escritos);
            printf("stat.tamEnBytesLog=%d\n", st.tamEnBytesLog);
            printf("stat.numBloquesOcupados=%d\n\n", st.numBloquesOcupados);
        }
    }
    bumount();
    return 0;
}
