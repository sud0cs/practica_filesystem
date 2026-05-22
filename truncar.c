#include "ficheros.h"
#include "utils.h"
#include "argparse.h"
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
    init_parser(3, argc, argv);
    add_arg("disco", true, STRING, "Nombre del disco");
    add_arg("ninodo", true, INT, "Número del inodo a truncar");
    add_arg("bytes", true, INT, "Nuevo tamaño logico en bytes");
    if(parse_args()==MISSING_ARGS_ERROR){
      print_help();
      return FALLO;
    }

    //Montar el disco virtual
    if(bmount(arg_value("disco"))<0){
      return FALLO;
    }

    unsigned int ninodo = *(int*)arg_value("ninodo");
    unsigned int nbytes = *(int*)arg_value("bytes");
    
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
    
    //Desmontamos el disco
    bumount();
    free_args();
}
