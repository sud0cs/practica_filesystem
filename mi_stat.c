#include "directorios.h"
int main(int argc, char **argv){
    if(argc<3){
	printf("mi_stat <disco> <path>\n");
	return FALLO;
    }
    bmount(argv[1]);
    struct STAT st;
    int ninodo = mi_stat(argv[2], &st);
    if(ninodo<0){
	print_dir_error(ninodo);
	return FALLO;
    }
    char down[strlen(argv[2])+38];
    memset(down, '#', sizeof(down));
    xpprint("###############", 240, DEFAULT, false, false);xpprint(" STAT '%s'", DEFAULT, DEFAULT, true, false, argv[2]);xpprint("###############\n", 240, DEFAULT, false, false);
    xpprint("#", 240, DEFAULT, false, false);xpprint(" Nº Inodo", 146, DEFAULT, true, false);xpprint(": %d\n", DEFAULT, DEFAULT, false, false, ninodo);
    xpprint("#", 240, DEFAULT, false, false);xpprint(" Tipo", 146, DEFAULT, true, false);xpprint(": %c\n", DEFAULT, DEFAULT, false, false, st.tipo);
    xpprint("#", 240, DEFAULT, false, false);xpprint(" Permisos", 146, DEFAULT, true, false);xpprint(": %d\n", DEFAULT, DEFAULT, false, false, st.permisos);
    xpprint("#", 240, DEFAULT, false, false);xpprint(" atime", 146, DEFAULT, true, false);xpprint(": %s", DEFAULT, DEFAULT, false, false, ctime(&st.atime));
    xpprint("#", 240, DEFAULT, false, false);xpprint(" mtime", 146, DEFAULT, true, false);xpprint(": %s", DEFAULT, DEFAULT, false, false, ctime(&st.mtime));
    xpprint("#", 240, DEFAULT, false, false);xpprint(" ctime", 146, DEFAULT, true, false);xpprint(": %s", DEFAULT, DEFAULT, false, false, ctime(&st.ctime));
    xpprint("#", 240, DEFAULT, false, false);xpprint(" btime", 146, DEFAULT, true, false);xpprint(": %s", DEFAULT, DEFAULT, false, false, ctime(&st.btime));
    xpprint("#", 240, DEFAULT, false, false);xpprint(" Nº Enlaces", 146, DEFAULT, true, false);xpprint(": %d\n", DEFAULT, DEFAULT, false, false, st.nlinks);
    xpprint("#", 240, DEFAULT, false, false);xpprint(" Tamaño en Bytes logicos", 146, DEFAULT, true, false);xpprint(": %d\n", DEFAULT, DEFAULT, false, false, st.tamEnBytesLog);
    xpprint("#", 240, DEFAULT, false, false);xpprint(" Numero Bloques ocupados", 146, DEFAULT, true, false);xpprint(": %d\n", DEFAULT, DEFAULT, false, false, st.numBloquesOcupados);
    xpprint("%s", 240, DEFAULT, false, false, down);
    bumount();
    return EXITO;
}
