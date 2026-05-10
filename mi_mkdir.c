#include "directorios.h"

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
    if(argc<4){
	    printf("mi_mkdir <disco> <permisos> <path>\n*nota: mkdir imita el comportamiento del programa de linux por lo tanto aunque el path no acabe en / creará un directorio.\nPara crear archivos utilizar mi_touch");
	    return EXITO;
    }

    //Montar el disco virtual
    bmount(argv[1]);
    
    char *path = argv[3];
    char perms = atoi(argv[2]);

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
    return EXITO;
}
