#include "ficheros.h"
#include <string.h>
#include <stdio.h>

/*
 * main()
 * ----------------------------------------------------------
 * Escribe un texto en uno o varios inodos, en posiciones muy separadas (offsets grandes), para probar
 * la traducción de bloques lógicos a físicos.
 * 
 * Abre el disco virtual com bmount(), reserva uno o varios inodos según el parámetros, escribe el texto
 * en distintos offsets y finalmente mustra los bytes escritos, el tamaño lógico del fichero y el número
 * de bloques ocupados.
 * 
 * Parámetros (argv):
 *  argv[1]: nombre del dispositivo(disco virtual)
 *  argv[2]: texto a escribir(normalemnte "$(cat fichero)")
 *  argv[3]: 0 o 1:
 *             0: usar un solo inodo para todos los offsets
 *             1: reservar un inodo distinto para casa offset
 * 
 * Devuelve:
 *  EXITO(0) si todo va bien
 *  FALLO(-1) si hay error
*/
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

    //Montar el disco virtual
    if(bmount(disco) == FALLO) return FALLO;

    int longitud = strlen(texto);
    printf("longitud texto: %d\n\n", longitud);

    //Offsets muy grandes para forzar uso de indirectos
    unsigned int offsets[] = {9000, 209000, 30725000, 409605000, 480000000};
    unsigned int ninodo;
    struct STAT st;

    /*
     * Caso 1: usar un solo inodo para todos los offsets.
     * 
     * Esto permite ver como crece el fichero y como se van reservando bloques indirectos de distintos niveles.
    */
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
    }
    /*
     * Caso 2: reservar un inodo distinto para cada offset.
     * 
     * Esto permite ver como se comporta la reserva de bloques cuando cada escritura empieza desde un inodo vacío.
    */
    else{
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
    //Desmontar el disco
    bumount();
    return 0;
}
