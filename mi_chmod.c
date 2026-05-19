#include "directorios.h"

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
    //Comprbar número de argumentos
    if(argc<4){
        fprintf(stderr, "mi_chmod <disco> <permisos> <path>\n");
	    return EXITO;
    }

    //Montar disco virtual
    if(bmount(argv[1])<0){
        fprintf(stderr, "Error: bmount\n");
        return FALLO;
    }

    //Convertir permisos a número
    unsigned char perms = atoi(argv[2]);

    //Comprobar que los permisos no superan el valor máximo
    if(perms>7){
	    xpperror("Valor máximo para los permisos: 7", RED, DEFAULT, true, false);
    }

    //Aplicar permisos sobre la ruta indicada
    int r = mi_chmod(argv[3], perms);
    if(r<0){
        print_dir_error(r);
        bumount();
        return FALLO;
    }

    //Desmontar el disco
    bumount();
    return EXITO;
}
