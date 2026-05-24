#include "directorios.h"
#include "argparse.h"
#include <stdio.h>
#include <string.h>

/*
 * rm_recursive()
 * ----------------------------------------------------------
 * Elimina recursivamente un fichero o directorio.
 * 
 * Funcionament:
 *   Intenta listar el contenido con mi_dir().
 *   Si mi_dir() devuelve error-> es un fichero-> eliminar amb mi_unlink().
 *   Si és un directorio:
 *    - Tokeniza la salida de mi_dir()
 *    - Para cada entrada, construye la ruta completa
 *    - Si es directorio-> llama recursivamente rm_recursive()
 *    - Si es fichero-> mi_unlink()
 *   Finalment elimina el directorio vacio con mi_unlink()
 * 
 * Parémetros:
 *   path: ruta absoluta del fichero/directorio a eliminar
 * 
 * Devuelve:
 *   EXITO(0) si se ha eliminado correctamente
 *   Codigo de error si falla
*/
int rm_recursive(const char *path){
    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));

    //Intentar listar el contenido
    int r = mi_dir(path, buffer);

    //Si mi_dir falla: no es un directorio, eliminar directamente
    if(r<0) return mi_unlink(path);

    //Tokenizar salida de mi_dir()
    char temp[4096];
    strcpy(temp, buffer);

    char *token = strtok(temp, "|");
    int campo = 0;
    char tipo;
    char nombre[256];

    while(token!=NULL){
        if(campo%5 == 0) tipo = token[0]; //Tipos: 'd' o 'f'
        if(campo%5 == 4){ //Nombre de la entrada
            strcpy(nombre, token);

            //Construir ruta completa
            char fullpath[512];
            strcpy(fullpath, path);
            if(path[strlen(path)-1] != '/') strcat(fullpath, "/");
            strcat(fullpath, nombre);

            //Recursión si es directorio
            if(tipo=='d') rm_recursive(fullpath);
            else mi_unlink(fullpath);
        }
        token=strtok(NULL, "|");
        campo++;
    }
    //Eliminar el drectorio vacio
    return mi_unlink(path);
}

/*
 * main()
 * ----------------------------------------------------------
 * Elimina recursivamnte un fichero o directorio del sistema de ficheros.
 * 
 * Parámetros:
 *   disco  STRING  -> nombre del disco virtual
 *   path  STRING  -> ruta absoluta a eliminar
 * 
 * Devuelve:
 *   EXITO(0) si todo va bien
 *   FALLO(-1) si hay algun error
*/
int main(int argc, char **argv){
    //Inicializar parser con 2 argumentos maximos
    init_parser(2, argc, argv);
    add_arg("disco", true, STRING, "Nombre del disco virtual");
    add_arg("path", true, STRING, "Ruta a eliminar recursivament");

    //Validar argumentos
    if(parse_args()==MISSING_ARGS_ERROR){
        print_help();
        return FALLO;
    }

    char *disco = arg_value("disco");
    char *path = arg_value("path");

    //Montar disco
    if(bmount(disco)<0){
        fprintf(stderr, "Error: bmount\n");
        free_args();
        return FALLO;
    }

    //Eliminar recursivamente
    int r = rm_recursive(path);
    if(r<0) print_dir_error(r);

    //Desmontar disco
    bumount();
    free_args();
    return r<0 ? FALLO : EXITO;
}
