#include "directorios.h"

/*
 * main()
 * ----------------------------------------------------------
 * Lista el contenido de un directorio del sistema de ficheros.
 * 
 * Soporta dos modos:
 *   Normal: mi_ls <disco> <path>
 *   Detallado(-l): mi_ls -l <disco> <path>
 * 
 * En modo detallado muestra tipo permisos, mtime, tamaño y nombre
 * 
 * Parámetros:
 *   argv[1]: [-l] o <disco>
 *   argv[2]: <disco> o <path>
 *   argv[3]: <path>(si se usa -1)
 * 
 * Devuelve:
 *    EXITO(0) si todo va bien
 *    FALLO(-1) si ocurre algún error
*/
int main(int argc, char **argv){
    //Comprobar número mínimo de argumentos
    if(argc<3){
	    printf("mi_ls <disco> <path>\n");
	    return FALLO;
    }

    //Comprobar si se usa el modo detallado(-l)
    int detailed = detailed = strcmp(argv[1],"-l") == 0;

    //Si hay 4 argumentos pero no se ha usado -l -> error
    if(argc==4 && !detailed){
	    printf("mi_ls <disco> <path>\n");
	    return FALLO;
    }

    //Montar el disco (el índice depende de si hay -l)
    if(bmount(argv[1+detailed])<0){
        fprintf(stderr, "Error: bmount\n");
        return FALLO;
    }

    //Ruta del directorio a listar
    char *path = argv[2+detailed];

    //Buffer donde mi_dir() devolverá la información
    char buffer[2048];
    memset(buffer, 0, sizeof(buffer));

    //Llamada a mi_dir()
    int err = mi_dir(path, buffer);
    if(err<0){
	    print_dir_error(err);
	    bumount();
	    return FALLO;
    }

    if(strlen(buffer)==0){
        printf("Total: 0\n");
        bumount();
        return FALLO;
    }

    //Tokenizar la salida separada por '|'
    char* token = strtok(buffer, "|");
    unsigned int i = 0;
    int color = DEFAULT;
    int total = 0;

    //Cabecera del modo detallado
    if(detailed)printf("Tipo\tPerms\tmTime\t\t\t\tTamaño\tNombre\n--------------------------------------------------------------\n");
    
    //Recorrer todos los tokens
    while (token != NULL) {
        //Determinar color según tipo o permisos
        if(i%5 == 0) color = DEFAULT;
    	  if(i%5 == 0)color=strcmp(token, "d")==0?BLUE:DEFAULT;
        if(i%5 == 1 && color==DEFAULT && strlen(token) >= 3) color=token[2]=='x'?GREEN:DEFAULT;
        
        //Nombre -> imprimir con color
        if(i%5 == 4){
            //nombre
            if(detailed){
                xpprint("%s\n", color, DEFAULT, color!=DEFAULT, false, token);
            }else{
                printf("%s\n", token);
            }
            total++;
	    }else if(detailed){
            //Otros campos -> imprimir solo si es modo detallado
	        //if(detailed)printf("%s\t",token);
            printf("%s\t", token);
	    }
	    token = strtok(NULL, "|");
	    i++;
    }
    //Imprimir total
    //if(detailed) printf("Total: %d\n, total");
    //else printf("Total: %d\n", total);
    printf("Total:%d\n", total);

    //Desmontar el disco
    bumount();
}
