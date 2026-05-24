#include "directorios.h"
#include "argparse.h"
#define BUFFERSIZE 8192
#define PATHSIZE 1024

/*
 * tree()
 * ----------------------------------------------------------
 * Función recursiva que muestra el contenido de un directorio
 * en formato de árbol, similar al comando 'tree' de Linux.
 * 
 * Para cada entrada del directorio:
 *   -Muestra su nombre con sangrado según la profundidad
 *   -Si es un directorio, desciende recursivamente
 * 
 * Parámetros:
 *   path -> ruta absoluta del directorio a listar
 *   depth -> nivel de profundidad(para generar sangrado visual)
*/
void tree(char *path, unsigned int depth){
    char buffer[BUFFERSIZE];
    char newpath[PATHSIZE];
    char *ptr = buffer;

    memset(buffer, 0, BUFFERSIZE);
    memset(newpath, 0, PATHSIZE);
    
    //Sangrado visual según profundidad
    char space[depth*2+1];
    memset(space, ' ', sizeof(space));
    space[depth*2]='\0';
    //Obtener listado del directorio
    int err = mi_dir(path, buffer);
    if(err<0){
	    print_dir_error(err);
	    bumount();
	    return;
    }

    //Tokenizar salida de mi_dir()
    char *token = strtok_r(buffer, "|", &ptr);
    unsigned int i = 0;
    bool isdir = false;
    bool isexec = false;
    while (token != NULL) {
      //Campo 0: tipo('d' o 'f')
	    if(i%5 == 0){
	        isdir = token[0]=='d';
	    }

      //campo 1: permisos
      if(i%5 == 1){
          isexec = token[2]=='x';
      }

      //Campo 4: nombre
	    if(i%5==4){
	        printf("%s|-",space);
	        //Si es directorio, descender recursivamente
            if(isdir){
            xpprint("%s\n", BLUE, DEFAULT, false, false, token);
		        strcpy(newpath, path);
		        strcat(newpath, token);
		        strcat(newpath, "/");
		        tree(newpath,depth+1);
		        memset(newpath, 0, PATHSIZE);
	        }
          else{
            if(isexec)xpprint("%s\n", GREEN, DEFAULT, false, false, token);
            else printf("%s\n", token);
          }
	    }
	    token = strtok_r(NULL, "|", &ptr);
	    i++;
    }
}


/*
 * main()
 * ----------------------------------------------------------
 * Muestra el árbol de directorios a partir de una ruta absoluta.
 * 
 * Monta el disco virtual, llama a tree() y finalmente desmonta del disco.
 * 
 * Parámetros:
 *   argv[1]: nombre del dispositivo(disco virtual)
 *   argv[2]: ruta absoluta del directorio a mostrar
 * 
 * Devuelve:
 *   EXITO(0) si todo va bien
 *   FALLO(-1) si ocurre algún error
*/
int main(int argc, char **argv){
    //Comprobar número de argumentos
    init_parser(2, argc, argv);
    add_arg("disco", true, STRING, "Nombre del disco a crear");
    add_arg("path", true, STRING, "Ruta a partir de la cual mostrar el árbol de contenidos");
    
    if(parse_args()==MISSING_ARGS_ERROR){
      print_help();
      return FALLO;
    }  

    //Montar el disco virtual
    bmount(arg_value("disco"));

    //Mostrar árbol
    tree(arg_value("path"), 1);

    //Desmontar disco
    bumount();
    free_args();
    return EXITO;
}
