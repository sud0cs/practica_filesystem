#include "ficheros.h"
#include "utils.h"

/*
 * main()
 * ----------------------------------------------------------
 * Permite truncar(reducir) el tamaño logico de un fichero representado por un inodo.
 * También parmite liberar el inodo complero si nbytes=0.
 * 
 * Monta el disco virtual, convierte los argumentos, si nbytes=0: libera el inodo entero(liberar_inodo),
 * si nbytes>0: trunca el fichero(mi_truncr_f), muestra la infomación actualizada del inodo y
 * finalemente desmonta el disco.
 * 
 * Parámetros:
 *  argv[1]: nombre del disco virtual
 *  argv[2]: numero de inodo a truncar
 *  argv[3]: nuevo tamaño logico en bytes
 * 
 * Devuelve:
 *  EXITO(0) si todo va bien
 *  FALLO(-1) si ocurre algún error
 */
int main(int argc, char **argv){
    if(argc != 4){
	fprintf(stderr, "Sintaxis: truncar <disco> <ninodo> <nbytes>\n");
	return FALLO;
    }

    //Montar el disco virtual
    bmount(argv[1]);

    unsigned int ninodo = atoi(argv[2]);
    unsigned int nbytes = atoi(argv[3]);
    
    /*
     * Si nbytes==0: liberar el inodo completo.
     *  - Libera todos los bloques de datos
     *  - Marca el inodo como libre('l')
     *  - Lo añade a la lista enlazada de inodos libres
    */
    if (nbytes==0) liberar_inodo(ninodo);

    /*
     * Si nbytes>0: truncar el fichero.
     * mi_truncar_f():
     *  - Libera bloques desde sbl hasta el último
     *  - Ajusta logicByteSize
     *  - Actualiza tiempos
    */
    else mi_truncar_f(ninodo, nbytes);
    
    //Mostrar información del inodo tras trncar/liberar
    struct STAT st; 
    mi_stat_f(ninodo, &st); 

    xpprint("###############", 240, DEFAULT, false, false);xpprint(" DATOS INODO: %d ", DEFAULT, DEFAULT, true, false, ninodo);xpprint("###############\n", 240, DEFAULT, false, false);
    xpprint("#", 240, DEFAULT, false, false);xpprint(" Tipo", 146, DEFAULT, true, false);xpprint(": %c\n", DEFAULT, DEFAULT, false, false, st.tipo);
    xpprint("#", 240, DEFAULT, false, false);xpprint(" Permisos", 146, DEFAULT, true, false);xpprint(": %d\n", DEFAULT, DEFAULT, false, false, st.permisos);
    xpprint("#", 240, DEFAULT, false, false);xpprint(" atime", 146, DEFAULT, true, false);xpprint(": %s", DEFAULT, DEFAULT, false, false, ctime(&st.atime));
    xpprint("#", 240, DEFAULT, false, false);xpprint(" mtime", 146, DEFAULT, true, false);xpprint(": %s", DEFAULT, DEFAULT, false, false, ctime(&st.mtime));
    xpprint("#", 240, DEFAULT, false, false);xpprint(" ctime", 146, DEFAULT, true, false);xpprint(": %s", DEFAULT, DEFAULT, false, false, ctime(&st.ctime));
    xpprint("#", 240, DEFAULT, false, false);xpprint(" btime", 146, DEFAULT, true, false);xpprint(": %s", DEFAULT, DEFAULT, false, false, ctime(&st.btime));
    xpprint("#", 240, DEFAULT, false, false);xpprint(" Tamano en Bytes logicos", 146, DEFAULT, true, false);xpprint(": %d\n", DEFAULT, DEFAULT, false, false, st.tamEnBytesLog);
    xpprint("#", 240, DEFAULT, false, false);xpprint(" Numero Bloques ocupados", 146, DEFAULT, true, false);xpprint(": %d\n", DEFAULT, DEFAULT, false, false, st.numBloquesOcupados);
    xpprint("##############################################", 240, DEFAULT, false, false);
    
    bumount();
}
