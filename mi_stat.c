#include "directorios.h"

/*
 * main()
 * ----------------------------------------------------------
 * Muestra por pantalla toda la información del inodo asociado
 * a una ruta del sistema de ficheros.
 * 
 * Utiliza mi_stat() para obtener los datos y los imprime con
 * formato y colores mediante xpprint().
 * 
 * Parámetros:
 *   argv[1]: nombre del dispositivo(disco virtual)
 *   argv[2]: ruta absoluta del fichero/directorio
 * 
 * Devuelve:
 *   EXITO(0) si todo va bien
 *   FALLO(-1) si ocurre algún error
*/
int main(int argc, char **argv){
    //Comprobar número de argumentos
    if(argc<3){
    	printf("mi_stat <disco> <path>\n");
	    return FALLO;
    }

    //Montar el disco virtual
    bmount(argv[1]);

    struct STAT st;
    
    //Obtener información del inodo asociado a la ruta
    int ninodo = mi_stat(argv[2], &st);
    if(ninodo<0){
    	print_dir_error(ninodo);
	    return FALLO;
    }

    //Crear una línea inferior de cierre hecha de '#'
    int len = strlen(argv[2]);
    char down[len+39];
    memset(down, '#', sizeof(down));
    down[len+38] = '\0';

    //Cabecera del bloque de infomación
    xpprint("###############", 240, DEFAULT, false, false);xpprint(" STAT '%s'", DEFAULT, DEFAULT, true, false, argv[2]);xpprint("###############\n", 240, DEFAULT, false, false);
    xpprint("#", 240, DEFAULT, false, false);xpprint(" Nº Inodo", 146, DEFAULT, true, false);xpprint(": %d\n", DEFAULT, DEFAULT, false, false, ninodo);
    xpprint("#", 240, DEFAULT, false, false);xpprint(" Tipo", 146, DEFAULT, true, false);xpprint(": %c\n", DEFAULT, DEFAULT, false, false, st.tipo);
    
    //Mostrar cada campo del STAT con formato
    xpprint("#", 240, DEFAULT, false, false);xpprint(" Permisos", 146, DEFAULT, true, false);xpprint(": %d\n", DEFAULT, DEFAULT, false, false, st.permisos);
    xpprint("#", 240, DEFAULT, false, false);xpprint(" atime", 146, DEFAULT, true, false);xpprint(": %s", DEFAULT, DEFAULT, false, false, ctime(&st.atime));
    xpprint("#", 240, DEFAULT, false, false);xpprint(" mtime", 146, DEFAULT, true, false);xpprint(": %s", DEFAULT, DEFAULT, false, false, ctime(&st.mtime));
    xpprint("#", 240, DEFAULT, false, false);xpprint(" ctime", 146, DEFAULT, true, false);xpprint(": %s", DEFAULT, DEFAULT, false, false, ctime(&st.ctime));
    xpprint("#", 240, DEFAULT, false, false);xpprint(" btime", 146, DEFAULT, true, false);xpprint(": %s", DEFAULT, DEFAULT, false, false, ctime(&st.btime));
    xpprint("#", 240, DEFAULT, false, false);xpprint(" Nº Enlaces", 146, DEFAULT, true, false);xpprint(": %d\n", DEFAULT, DEFAULT, false, false, st.nlinks);
    xpprint("#", 240, DEFAULT, false, false);xpprint(" Tamaño en Bytes logicos", 146, DEFAULT, true, false);xpprint(": %d\n", DEFAULT, DEFAULT, false, false, st.tamEnBytesLog);
    xpprint("#", 240, DEFAULT, false, false);xpprint(" Numero Bloques ocupados", 146, DEFAULT, true, false);xpprint(": %d\n", DEFAULT, DEFAULT, false, false, st.numBloquesOcupados);
    
    //Línea inferior de cierre
    xpprint("%s", 240, DEFAULT, false, false, down);

    //Desmontar el disco
    bumount();
    return EXITO;
}
