#include "directorios.h"
#include "argparse.h"
/*
 * main()
 * ----------------------------------------------------------
 * Crea un fichero vacío en el sistema de ficheros(similar a 'touch' en Linux)
 * 
 * Este programa:
 *   Monta el dsco virtual
 *   Comprueba que la ruta no termina en '/'(touch no crea directorios)
 *   Valida los permisos(0-7)
 *   Llama a mi_creat() para crear el fichero
 *   Desmonta el disco
 * 
 * Parámetros:
 *   argv[1]: nombre del dispositivo(disco virtual)
 *   argv[2]: permisos en octal(0-7)
 *   argv[3]: ruta absoluta del fichero a crear
 * 
 * Devuelve:
 *   EXITO(0) si todo va ben
 *   FALLO(-1) si ocurre algún error
*/
int main(int argc, char **argv){
    //Comprobar número de argumentos
    init_parser(3, argc, argv);
    add_arg("disco", true, STRING, "Nombre del disco");
    add_arg("perms", true, INT, "Permisos del fichero");
    add_arg("path", true, STRING, "Ruta del fichero a crear");
    if(parse_args()==MISSING_ARGS_ERROR){
      print_help();
      return FALLO;
    }  
      //Montar el disco virtual
    if(bmount(arg_value("disco"))<0){
        fprintf(stderr, "Error: bmount\n");
        return FALLO;
    }

    char *path = arg_value("path");
    
    //Touch no puede crear directorios -> error si termina en '/'
    if(path[strlen(path)-1] == '/'){
    	xpperror("Touch no puede crear directorios.", RED, DEFAULT, true, false);
	    bumount();
	    return FALLO;
    }

    //Convertir permsos
    int perms = *(int*)arg_value("perms");

    //Validar permisos
    if(perms>7 || perms<0){
    	xpperror("Los permisos deben estar entre 0 y 7", RED, DEFAULT, true, false);
	    bumount();
	    return FALLO;
    }

    //Crear el fichero
    print_dir_error(mi_creat(path, perms));

    //Desmontar el disco
    bumount();
    free_args();
    return EXITO;
}
