#include "directorios.h"
#include "argparse.h"
/*
 * main()
 * ----------------------------------------------------------
 * Crea un directorio en el sistema de ficheros.
 * 
 * Este programa imita el comportamiento de mkdir en Linux:
 *   Si el path no termina en '/', se añade automáticamente
 *   Solo crea directorios(no archivos)
 *   Para crear archivos debe usarse mi_touch
 * 
 * Parámetros:
 *   argv[1]: nombre del dispositivo(disco virtual)
 *   argv[2]: permisos en octal(0-7)
 *   argv[3]: ruta absoluta del directorio a crear
 * 
 * Devuelve:
 *   EXITO(0) si todo va bien
 *   FALLO(-1) si ocurre algún error
*/
int main(int argc, char **argv){
    //Comprobar número de argumentos
    
    init_parser(4, argc, argv);
    add_arg("disco", true, STRING, "Nombre del disco");
    add_arg("perms", true, INT, "Número del inodo a leer");
    add_arg("path", true, STRING, "Ruta del nuevo directorio");
    add_arg("p", false, NONE, "Crea todos los directorios inexistentes en la ruta");

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
    char perms = *(int*)arg_value("perms");

    //Si el path no termina en '/', añadirlo
    if(path[strlen(path)-1] != '/')strcat(path, "/");

    //Validar permisos
    if(perms>7 || perms<0){
	    xpperror("Los permisos deben estar entre 0 y 7", RED, DEFAULT, true, false);
	    return FALLO;
    }

    //Crear el directorio
    print_dir_error(mi_creat(path, perms));

    //Desmontar el disco
    bumount();
    free_args();
    return EXITO;
}
