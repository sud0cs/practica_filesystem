#include "directorios.h"
#include "argparse.h"
/*
 * main()
 * ----------------------------------------------------------
 * Escribe texto en un fichero del sistema de ficheros.
 * 
 * Monta el disco virtual, convierte los argumento a valores numéricos,
 * llama a mi_write() para escribir el texto en la posición indicada
 * y desmonta el disco.
 * 
 * Parámetros(argv):
 *   argv[1]: nombre del dispositivo(disco virtual)
 *   argv[2]: ruta absoluta del fichero donde escribir
 *   argv[3]: texto a escribir
 *   argv[4]: offset dentro del fichero
 * 
 * Devuelve:
 *   EXITO(0) si todo va bien
 *   FALLO(-1) si ocurre algún error
*/
int main(int argc, char **argv){
    //Comprobar número de argumentos

    init_parser(4, argc, argv);
    add_arg("disco", true, STRING, "Nombre del disco");
    add_arg("path", true, STRING, "Ruta del archivo en el que escribir");
    add_arg("texto", true, STRING, "Texto a escribir");
    add_arg("offset", true, INT, "offset dentro del fichero");

    if(parse_args()==MISSING_ARGS_ERROR){
      print_help();
      return FALLO;
    }

    //Montar el disco virtual
    if(bmount(arg_value("disco")) == FALLO) return FALLO;

    //Convertir offset a entero
    int offset = *(int*)arg_value("offset");

    //Escribir el texto en el fichero indicado
    int escritos = mi_write(arg_value("path"), arg_value("texto"), offset, strlen(arg_value("texto")));
    printf("longitud texto: %d\n", (int)strlen(arg_value("texto")));
    printf("Bytes escritos: %d\n", escritos);
    
    //Comprobar errores de escritura
    if(escritos<0){
	    print_dir_error(escritos);
	    bumount();
	    return FALLO;
    }
    
    //Desmontar el disco
    bumount();
    free_args();
}
