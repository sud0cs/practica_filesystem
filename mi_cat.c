#include "directorios.h"
#include "argparse.h"
#define TAMBUFFER BLOCKSIZE*4

/*
 * main()
 * ----------------------------------------------------------
 * Lee y muestra por pantalla el contenido de un fichero del sistema
 * de ficheros, utilizando mi_read().
 * 
 * Monta el disco virtual, valida la ruta, lee el fichero en bloques de tamaño
 * TAMBUFFER y escribe su contenido por stdout.
 * 
 * Parámetros(argv):
 *   argv[1]: nombre del dispositivo(disco virtual)
 *   argv[2]: ruta absoluta del fichero a leer
 * 
 * Devuleve:
 *   EXITO(0) si todo va bien
 *   FALLO(-1) si hay error
*/
int main(int argc, char **argv){
    //Comprobar número de argumentos
    
    init_parser(2, argc, argv);
    add_arg("disco", true, STRING, "Nombre del disco");
    add_arg("path", true, STRING, "Ruta del archivo del cual mostrar sus datos");
    
    if(parse_args()==MISSING_ARGS_ERROR){
      print_help();
      return FALLO;
    }

    //Montar el disco virtual
    if(bmount(arg_value("disco")) == FALLO){
        fprintf(stderr, "Error: bmount\n");
        return FALLO;
    }

    //Variables para almacenar resultados de buscar_entrada()
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;

    //Comprobar que la ruta existe antes de leer
    int r = buscar_entrada(arg_value("path"), &p_inodo_dir, &p_inodo, &p_entrada, 0, 0);
    if(r<0){
        print_dir_error(r);
        bumount();
        return FALLO;
    }

    //Lectura del fichero en bloques
    int offset = 0;
    char buffer[TAMBUFFER];
    unsigned int leidos, total_leidos = 0;

    //Leer mientras queden datos
    while((leidos=mi_read(arg_value("path"), buffer, offset, TAMBUFFER)) > 0){
	    write(1, buffer, leidos); //Escribir por stdout
	    offset += leidos; //Avanzar offset
        total_leidos += leidos;
    }

    fprintf(stderr,"\nTotal_leidos %d\n", total_leidos);

    //Si mi_read devolvió error
    if(leidos<0){
	    print_dir_error(leidos);
	    bumount();
	    return FALLO;
    }

    //Desmontar el disco
    bumount();
    free_args();
}
