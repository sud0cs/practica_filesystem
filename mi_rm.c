#include "directorios.h"
#include <stdio.h>
#include "argparse.h"
/*
 * main()
 * ----------------------------------------------------------
 * Elimina una entrada dels sistema de ficeros(fichero o directorio vacío).
 * Internamente llama a mi_unlink(), que:
 *   Busca la entrada indicada por la ruta
 *   Compruba si es un directorio vacío(si aplica)
 *   Elimina la entrada del directorio padre
 *   Actualiza nlinks y libera el inodo si es necesario
 * 
 * Parámetros(argv):
 *   argv[1]: nombre del dispositivo(disco virtual)
 *   argv[2]: ruta absoluta del fichero/directorio a eliminar
 * 
 * Devuleve:
 *   EXITO(0) si se elimina correctamente
 *   Código de error si falla
*/
int main(int argc, char **argv){
    init_parser(2, argc, argv);
    add_arg("disco", true, STRING, "Nombre del disco");
    add_arg("path", true, STRING, "Ruta del fichero/directorio a eliminar");
    
    if(parse_args()==MISSING_ARGS_ERROR){
      print_help();
      return FALLO;
    }   

    //Montar disco virtual
    if(bmount(arg_value("disco"))<0){
        fprintf(stderr, "Error: bmount\n");
        return FALLO;
    }

    //Intentar elminar la entrada
    int r = mi_unlink(arg_value("path"));
    if(r<0){
        print_dir_error(r); //Mostrar error descriptivo
        bumount();
        return FALLO;
    }

    //Desmontar el disco
    bumount();
    return EXITO;
}
