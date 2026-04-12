#include "ficheros.h"


/*
 * main()
 * ----------------------------------------------------------
 * Cambia los permisos de un inodo del sistema de ficheros.
 * 
 * Monta el dico virtual, convierte los argumentos a numeros, llama a mi_chmod_f()
 * para cambiar los permisos del inodo y desmonta el disco.
 * 
 * Parámetros(argv):
 *  argv[1]: nombre del dispositivo(disco virtual)
 *  argv[2]: numero de indodo al que cambiar permisos
 *  argv[3]: permisos en octal(0-7)
 * 
 * Devuelve:
 *  EXITO(0) si todo va bien
 *  FALLO(-1) si hay error
*/
int main(int argc, char **argv){
    if(argc != 4){
        fprintf(stderr, "Sintaxis: permitir <disco> <ninodo> <permisos>\n");
        return FALLO;
    }

    //Montar el disco virtual
    bmount(argv[1]);
    
    unsigned int ninodo = atoi(argv[2]);
    unsigned char permisos = atoi(argv[3]);

    /*
     * mi_chmof_f()
     * ----------------------------------------------------------
     * Cambia los permisos del inodo:
     *  bit 4: lectura(r)
     *  bit 2: escritura(w)
     *  bit 1: ejecución(x)
    */
    mi_chmod_f(ninodo, permisos);

    //Desmontar el disco
    bumount();
}