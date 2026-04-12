#include "ficheros.h"
#include <string.h>
#include <unistd.h>

#define TAMBUFFER 1500

/*
 * main()
 * ----------------------------------------------------------
 * Lee el contenido de un fichero representado por un inodo y lo muestra por pantalla.
 * 
 * Monta el disco virtual, lee el inodo indicado por el usuario, va leyendo el fichero
 * en bloques de 1500 bytes, muestra el contenido por stdount y al final muestra el 
 * total de bytes leídos y tamaño lógico del fichero.
 * 
 * Parámetros(argv):
 *  argv[1]: nombre del dispositivo(disco virtual)
 *  argv[2]: numero de inodo a leer
 * 
 * Devuelve:
 *  EXITO(0) si todo va bien
 *  FALLO(-1) si hay error
*/
int main(int argc, char **argv){
    if(argc != 3){
        fprintf(stderr, "Sintaxis: leer <disco> <ninodo>\n");
        return FALLO;
    }

    //Montar el disco virtual
    if(bmount(argv[1]) == FALLO) return FALLO;

    unsigned int ninodo = atoi(argv[2]);
    char buffer[TAMBUFFER];
    int offset = 0;
    int leidos = 0;
    int total = 0;
    
    /*
     * Bucle de lectura
     * 
     * Se lee el fichero en bloques de 1500 bytes hasta que mi_read_f() devuelve 0(EOF).
    */
    do{
        memset(buffer, 0, TAMBUFFER);

        //Leer desde offset
        leidos = mi_read_f(ninodo, buffer, offset, TAMBUFFER);

        if(leidos < 0){
            fprintf(stderr, "Error en mi_read_f\n");
            bumount();
            return FALLO;
        }

        //Si se han leido bytes, mostrarlos por pantalla
        if(leidos > 0){
            write(1, buffer, leidos); //stdout = 1
            offset += leidos;
            total += leidos;
        }

    } while(leidos > 0);

    //Mosrar informacion del fichero
    struct STAT st;
    mi_stat_f(ninodo, &st);

    fprintf(stderr, "\n\nTotal leidos: %d\nTamEnBytesLog: %d\n", total, st.tamEnBytesLog);
    
    bumount();
    return 0;
}