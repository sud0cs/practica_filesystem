#include "directorios.h"
#include "argparse.h"

/*
 * main()
 * ----------------------------------------------------------
 * Cambia los permisos de un fichero o directorio a partir de su ruta.
 * 
 * Monta el disco virtual, convierte los argumentos a valores numéricos,
 * llama a mi_chmod() para aplicar los permisos y desmonta el disco.
 * 
 * Parámetros(argv):
 *   argv[1]: nombre del dispositivo(disco virtual)
 *   argv[2]: permisos en octal(0-7)
 *   argv[3]: ruta absoluta del fichero/drectorio
 * 
 * Devuelve:
 *   EXITO(0) si todo va bien
 *   FALO(-1) si ocurre algún error
*/
int main(int argc, char **argv){
    init_parser(3, argc, argv);
    add_arg("disco", true, STRING, "Nombre del disco");
    add_arg("perms", true, INT, "Nuevos permisos (entre 0 y 7)");
    add_arg("path", true, STRING, "Ruta de la que cambiar los permisos");
    
    if(parse_args()==MISSING_ARGS_ERROR){
      print_help();
      return FALLO;
    }

    //Montar disco virtual
    if(bmount(arg_value("disco"))<0){
        fprintf(stderr, "Error: bmount\n");
        return FALLO;
    }

    //Convertir permisos a número
    unsigned char perms = *(int*)arg_value("perms");

    //Comprobar que los permisos no superan el valor máximo
    if(perms>7){
	    xpperror("Valor máximo para los permisos: 7", RED, DEFAULT, true, false);
      bumount();
      return FALLO;
    }

    //Aplicar permisos sobre la ruta indicada
    int r = mi_chmod(arg_value("path"), perms);
    if(r<0){
        print_dir_error(r);
        bumount();
        return FALLO;
    }

    //Desmontar el disco
    bumount();
    free_args();
    return EXITO;
}
