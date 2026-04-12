#include "ficheros.h"
#include "utils.h"

int main(int argc, char **argv){
    if(argc != 4){
	fprintf(stderr, "Sintaxis: truncar <disco> <ninodo> <nbytes>\n");
	return FALLO;
    }
    bmount(argv[1]);
    unsigned int ninodo = atoi(argv[2]);
    unsigned int nbytes = atoi(argv[3]);
    
    if (nbytes==0) liberar_inodo(ninodo);
    else mi_truncar_f(ninodo, nbytes);
    
    struct STAT st; 
    mi_stat_f(ninodo, &st); 
    xpprint("###############", 240, DEFAULT, false, false);xpprint(" DATOS INODO: %d ", DEFAULT, DEFAULT, true, false, ninodo);xpprint("###############\n", 240, DEFAULT, false, false);
    xpprint("#", 240, DEFAULT, false, false);xpprint(" Tipo", 146, DEFAULT, true, false);xpprint(": %c\n", DEFAULT, DEFAULT, false, false, st.tipo);
    xpprint("#", 240, DEFAULT, false, false);xpprint(" Permisos", 146, DEFAULT, true, false);xpprint(": %d\n", DEFAULT, DEFAULT, false, false, st.permisos);
    xpprint("#", 240, DEFAULT, false, false);xpprint(" atime", 146, DEFAULT, true, false);xpprint(": %s", DEFAULT, DEFAULT, false, false, ctime(&st.atime));
    xpprint("#", 240, DEFAULT, false, false);xpprint(" mtime", 146, DEFAULT, true, false);xpprint(": %s", DEFAULT, DEFAULT, false, false, ctime(&st.mtime));
    xpprint("#", 240, DEFAULT, false, false);xpprint(" ctime", 146, DEFAULT, true, false);xpprint(": %s", DEFAULT, DEFAULT, false, false, ctime(&st.ctime));
    xpprint("#", 240, DEFAULT, false, false);xpprint(" btime", 146, DEFAULT, true, false);xpprint(": %s", DEFAULT, DEFAULT, false, false, ctime(&st.btime));
    xpprint("#", 240, DEFAULT, false, false);xpprint(" Tamano en Bytes logicos", 146, DEFAULT, true, false);xpprint(": %d\n", DEFAULT, DEFAULT, false, false, st.tamEnBytesLog);
    xpprint("#", 240, DEFAULT, false, false);xpprint(" Numero Bloques ocupados", 146, DEFAULT, true, false);xpprint(": %d\n", DEFAULT, DEFAULT, false, false, st.numBloquesOcupados);
    xpprint("##############################################", 240, DEFAULT, false, false);
    bumount();
}
