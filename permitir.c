#include "ficheros.h"
#include "argparse.h"

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
    init_parser(3, argc, argv);
    add_arg("disco", true, STRING, "Nombre del disco");
    add_arg("ninodo", true, INT, "Número del inodo al que cambiar los permiso");
    add_arg("perms", true, INT, "Nuevos permisos (0-7)");
    if(parse_args()==MISSING_ARGS_ERROR){
      print_help();
      return FALLO;
    }    
    //Montar el disco virtual
    if(bmount(arg_value("disco"))<0){
        fprintf(stderr, "Error: bmount\n");
        return FALLO;
    }
    
    unsigned int ninodo = *(int*)arg_value("ninodo");
    unsigned char permisos = *(int*)arg_value("perms");

    /*
     * mi_chmof_f()
     * ----------------------------------------------------------
     * Cambia los permisos del inodo:
     *  bit 4: lectura(r)
     *  bit 2: escritura(w)
     *  bit 1: ejecución(x)
    */
    int r = mi_chmod_f(ninodo, permisos);
    if(r<0) fprintf(stderr, "Error: no se pudeieron cambiar los permisos\n");

    //Desmontar el disco
    bumount();
    free_args();
    return r;
}
