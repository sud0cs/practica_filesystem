#include "directorios.h"
#include <stdio.h>
#include "argparse.h"
/*
 * main()
 * ----------------------------------------------------------
 * Crea un enlace duro entre dos rutas del sistema de ficheros.
 * Internamente llama a mi_link(), que:
 *   Busca el inodo del ficehro original(camino1)
 *   Crea una nueva entrada de directorio(camino2)
 *   Hace que camino2 apunte al mismo inodo que camino1
 *   Incrementa el contador de enlaces(nlinks)
 * 
 * Parámetros(argv)
 *   argv[1]: nombre del dispositivo(disco virtual)
 *   argv[2]: ruta del fichero original
 *   argv[3]: ruta donde crea el enlace duro
 * 
 * Devuelve:
 *   EXITO(0) si se crea correctamente
 *   Código de error si falla
*/
int main(int argc, char **argv){
    init_parser(3, argc, argv);
    add_arg("disco", true, STRING, "Nombre del disco");
    add_arg("og-path", true, STRING, "Ruta del fichero original");
    add_arg("link-path", true, STRING, "Ruta del enlace a crear");
    
    if(parse_args()==MISSING_ARGS_ERROR){
      print_help();
      return FALLO;
    }  
    
    //Montar el disco virtual
    if(bmount(arg_value("disco"))<0){
        fprintf(stderr, "Error: bmount\n");
        return FALLO;
    }

    //Intentar crear el enlace duro
    int r = mi_link(arg_value("og-path"), arg_value("link-path"));
    if(r<0){
        print_dir_error(r); //Montar error descriptivo
        bumount();
        return FALLO;
    }

    //Desmontar el disco
    bumount();
    free_args();
    return EXITO;
}
