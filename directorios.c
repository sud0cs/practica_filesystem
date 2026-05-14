#include "ficheros.h"
#include "directorios.h"
#include <string.h>
#include <stdio.h>

path_cache cache = {0};

/*
 * extraer_camino()
 * ----------------------------------------------------------
 * Separa un camino absoluto en dos partes: "inicial" y "final".
 * Sirve para procesar rutas recursivamente en buscar_entrada().
 * 
 * Ejemplos:
 *   "/dir1/dir2/fich" -> inicial="dir1", final="/dir2/fich", tip='d'
 *   "/fichero" -> inicial="fichero", final="", tipo='f'
 * 
 * Parámetros:
 *   camino -> ruta absoluta a analizar
 *   inicial -> primer componente del camino(nombre de directorio o fichero)
 *   final -> resto del camino(incluye '/')
 *   tipo -> 'd' si inicial es drectorio, 'f' si es fichero
 * 
 * Devuelve:
 *   1 si inicial es un directorio
 *   0 si inicial es un fichero
 *   ERROR_CAMINO_INCORRECTO si el camino no es válido
 */
int extraer_camino(const char *camino, char *inicial, char *final, char *tipo){
    //Comprobar camino válido
    if(camino==NULL || strlen(camino)==0 || camino[0]!='/'){
        return ERROR_CAMINO_INCORRECTO;
    }

    //Si el camino es solo "/", es la raíz
    if(strcmp(camino, "/")==0){
        strcpy(inicial, "/");
        strcpy(final, "");
        *tipo = 'd';
        return 1; //directorio
    }

    //Copiar camino
    char copia[strlen(camino)+1];
    strcpy(copia, camino);

    //Buscar segunda barra
    char *segunda = strchr(copia+1, '/');

    if(segunda==NULL){
        //No hay más '/', es fichero
        strcpy(inicial, copia+1);
        strcpy(final, "");
        *tipo = 'f';
        return 0;
    }

    //Hay más '/', extraemos inicial
    *segunda='\0';
    strcpy(inicial, copia+1);

    //El resto del camino(incluyendo la barra)
    strcpy(final, camino+1+strlen(inicial));

    //Determinar tipo(si hay más camino, tratamos como directorio)
    *tipo='d';
    return 1;
}

/*
 * buscar_entrada()
 * ----------------------------------------------------------
 * Función clave del sistema de ficheros: busca una entrada dentro de un directorio.
 * Puede trabajar en modo consulta o en modo creación (reservar=1)
 * 
 * Funcionamiento general:
 *   Divide el camino en "inicial" y "final" con extraer_camino()
 *   Busca la entrada "inicial" dentro del inodo directorio actual
 *   Si no existe:
 *     -Si reservar=0: error
 *     -Si reservar=1: crear la entrada y reservar un inodo
 *   Si existe:
 *     -Si final esta vacio: hemos llegado al destino
 *     -Si final no esta vacio: llamada recursiva
 * 
 * Parámetros:
 *   camino_parcial -> ruta a resolver
 *   p_inodo_dir -> inodo del directorio donde buscar
 *   p_inodo -> devuelve el inodo encontrado
 *   p_entrada -> devuelve el número de entrada dentro del directorio
 *   reservar -> 0 consulta, 1 crea si no existe
 *   permisos -> permisos para el nuevo inodo si se crea
 * 
 * Devuelve:
 *   EXITO(0) si encuentra o crea la entrada
 *   Código de error negativo si falla
*/
int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos){
    entrada _entrada;
    inode inodo_dir;
    bool found;
    char tipo;
    char inicial[sizeof(_entrada.nombre)];
    char final[strlen(camino_parcial)];
    int cant_entradas_inodo;
    int num_entrada_inodo;
    unsigned int offset = 0;

    found = false;

    //Caso especial: raíz
    if(strcmp(camino_parcial, "/") == 0){
	    *p_inodo = 0;
	    *p_entrada = 0;
	    return 0;
    }

    //Separar camino en inicial/final
    if(extraer_camino(camino_parcial, inicial, final, &tipo)<0){
	    return ERROR_CAMINO_INCORRECTO;
    }
    #if DBGLVL8
    xpperror("[ buscar_entrada() -> inicial: %s, final: %s, reservar: %d ]\n", GRAY, DEFAULT, false, false, inicial, final, reservar);
    #endif

    //Leer inodo del directorio actual
    leer_inodo(*p_inodo_dir, &inodo_dir);
    if(!has_perms(inodo_dir.perms, PERM_READ)){
	    return ERROR_PERMISO_LECTURA;
    }

    entrada buffer[BLOCKSIZE/sizeof(entrada)];
    memset(buffer, 0, sizeof(buffer));
    
    //Numero de entradas existentes en el directorio(tamaño lógico/tamaño entrada)
    cant_entradas_inodo = inodo_dir.logicByteSize/sizeof(entrada);
    num_entrada_inodo = 0;
    if(cant_entradas_inodo>0){
        //Recorremos todas las entradas del directorio buscando coincidencia con "inicial"
	    while((num_entrada_inodo<cant_entradas_inodo) && found==false){
	        if(offset%BLOCKSIZE==0)mi_read_f(*p_inodo_dir, buffer, offset, BLOCKSIZE);
	        _entrada = buffer[num_entrada_inodo%(sizeof(buffer)/sizeof(entrada))];
	        offset+=sizeof(entrada);
		if(strcmp(_entrada.nombre, inicial)==0){
		        found = true;
		        *p_entrada = num_entrada_inodo;
			break;
	        }
	        num_entrada_inodo++;
	    }
    }

    //Si no se ha encontrado la entrada y reservar=1, intentamos crearla
    if(found==false && num_entrada_inodo==cant_entradas_inodo){
        if (!reservar) return ERROR_NO_EXISTE_ENTRADA_CONSULTA;
	    if (inodo_dir.type == 'f') return ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO;
	    if(!has_perms(inodo_dir.perms, PERM_WRITE)){
	        return ERROR_PERMISO_ESCRITURA;
	    }
        //Inicializamos nueva entrada con el nombre "inicial"
	    memcpy(_entrada.nombre, inicial, sizeof(inicial));
        
        //Reservar inodo según tipo
	    if(tipo == 'd'){
	        if(strcmp(final, "/")==0)_entrada.ninodo = reservar_inodo('d', permisos);
	        else return ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO;
	    }
	    else{
	        _entrada.ninodo = reservar_inodo('f', permisos);
	    }
        //Escribir nueva entrada al final del directorio
	    mi_write_f(*p_inodo_dir, &_entrada, offset, sizeof(entrada));
    }
    int l = strlen(final);

    //si ya no queda más camino por resolver
    if(l==0 || (l==1 && final[0] == '/')){
	    if(found && reservar) return ERROR_ENTRADA_YA_EXISTENTE;
	    *p_inodo = _entrada.ninodo;
	    *p_entrada = num_entrada_inodo;
	    return EXITO;
    }else{
        //Si aun queda camino por resolver, continuamos recursivamente en el siguiente nivel
	    *p_inodo_dir = _entrada.ninodo;
	    return buscar_entrada(final, p_inodo_dir, p_inodo, p_entrada, reservar, permisos);
    }
}

/*
 * print_dir_error()
 * ----------------------------------------------------------
 * Imprime por pantalla un mensaje decriptivo según el código de error
 * devuelto por buscar_entrada() o funciones relacionada.
*/
void print_dir_error(int error){
    switch(error){
	case -2:
	    xpperror("ERROR: Camino incorrecto\n", RED, DEFAULT, true, false);
	    break;
	case -3:
	    xpperror("ERROR: No hay permisos de lectura\n", RED, DEFAULT, true, false);
	    break;
	case -4:
	    xpperror("ERROR: El archivo o directorio no existe\n", RED, DEFAULT, true, false);
	    break;
	case -5:
	    xpperror("ERROR: No existe algún directorio intermedio\n", RED, DEFAULT, true, false);
	    break;
	case -6:
	    xpperror("ERROR: No hay permisos de escritura\n", RED, DEFAULT, true, false);
	    break;
	case -7:
	    xpperror("ERROR: El archivo ya existe\n", RED, DEFAULT, true, false);
	    break;
	case -8:
	    xpperror("ERROR: No es un directorio\n", RED, DEFAULT, true, false);
	    break;
    }
}

/*
 * mi_creat()
 * ----------------------------------------------------------
 * Crea un fichero o directorio según el camino indicado.
 * Internamente llama a buscar_entrada() en modo creación.
 * 
 * Parámetros:
 *   path -> camino absoluto del fichero/directorio a crear
 *   perms -> permisos del nuevo inodo
 * 
 * Devuelve:
 *   EXITO(0) si se crea correctamente
 *   Código de error si falla
*/
int mi_creat(char *path, unsigned char perms){
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;

    mi_waitSem();

    int r = buscar_entrada(path, &p_inodo_dir, &p_inodo, &p_entrada, 1, perms);

    mi_signalSem();
    
    return r;
}

/*
 * mi_dir()
 * ----------------------------------------------------------
 * Lista la información de un fichero o de todas las entradas de un directorio.
 * El resultado se vuelca en la cadena 'str' con un formato tipo:
 *   tipo|permisos|fecha_modificación|tamaño|nombre|
 * 
 * Parámetros:
 *   camino -> retuta del fichero o directorio
 *   str -> buffer de salida donde se escribe la información
 * 
 * Devuelve:
 *   EXITO(0) si todo va bien
 *   Código de error si falla
*/
int mi_dir(const char *camino, char *str){
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    unsigned int offset = 0;
    superblock SB;
    bread(SBPOS, &SB);
    inode inodo;
    struct STAT stat_entrada;
    unsigned int p_inodo_dir = SB.rootInode;

    //Buscar la entrada correspondiete al camino
    int err = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 0);
    if(err<0)return err;

    leer_inodo(p_inodo, &inodo);
    if(!has_perms(inodo.perms, PERM_READ))return ERROR_PERMISO_ESCRITURA;

    entrada _entrada;
    char out[BLOCKSIZE];
    entrada buffer[BLOCKSIZE/sizeof(entrada)];

    //Si es un fichero, mostramos solo su información
    if(inodo.type == 'f'){
	    mi_stat_f(p_inodo, &stat_entrada);
	    mi_read_f(p_inodo_dir, buffer, sizeof(entrada)*p_entrada, BLOCKSIZE);
	    sprintf(str, "%c|%c%c%c|%s|%dB|%s|",
		stat_entrada.tipo,
		has_perms(stat_entrada.permisos, PERM_READ)?'r':'-',
		has_perms(stat_entrada.permisos, PERM_WRITE)?'w':'-',
		has_perms(stat_entrada.permisos, PERM_EXEC)?'x':'-',
		strpl(ctime(&stat_entrada.mtime),"\n", "", 0),
		stat_entrada.tamEnBytesLog,
		buffer[0].nombre);
	    return EXITO;
    }
    memset(out, 0, BLOCKSIZE);
    unsigned int strsize = 0;
    //Si es un directorio, recorremos todas sus entradas
    while(offset<inodo.logicByteSize){
	    if(offset%BLOCKSIZE == 0)mi_read_f(p_inodo, buffer, offset, BLOCKSIZE);
	    
        _entrada = buffer[(offset%BLOCKSIZE)/sizeof(entrada)];
	    mi_stat_f(_entrada.ninodo, &stat_entrada);
	    
        strsize += sprintf(out, "%c|%c%c%c|%s|%dB|%s|",
		stat_entrada.tipo,
		has_perms(stat_entrada.permisos, PERM_READ)?'r':'-',
		has_perms(stat_entrada.permisos, PERM_WRITE)?'w':'-',
		has_perms(stat_entrada.permisos, PERM_EXEC)?'x':'-',
		strpl(ctime(&stat_entrada.mtime),"\n", "", 0),
		stat_entrada.tamEnBytesLog,
		_entrada.nombre);
	    
        if(sizeof(buffer)<strsize)return FALLO;
	    strcat(str, out);
	    offset+=sizeof(entrada);
    }
    return EXITO;
}

/*
 * mi_chmod
 * ----------------------------------------------------------
 * Cambia los permisos de un ficehro o directorio a partir de su camino.
 * Internamente localiza el inodo con buscar_entrada() y llama a mi_chmod_f()
*/
int mi_chmod(const char *camino, unsigned char perms){
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    superblock SB;
    bread(SBPOS, &SB);
    unsigned int p_inodo_dir = SB.rootInode;
    int err = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 0);
    if (err<0)return err;
    return mi_chmod_f(p_inodo, perms);
}

/*
 * mi_stat()
 * ----------------------------------------------------------
 * Obtiene la metainformación (STAT) de un fichero o directorio a partir de su camino.
 * Internamente localiza el inodo con buscar_entrada() y llama a mi_stat_f().
 * 
 * Devuelve:
 *   Número de inodo si todo va bien
 *   Código de error si falla
*/
int mi_stat(const char *camino, struct STAT *p_stat){
    superblock SB;
    bread(SBPOS, &SB);
    unsigned int rootInode = SB.rootInode;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int err = buscar_entrada(camino, &rootInode, &p_inodo, &p_entrada, 0, 0);
    if(err<0)return err;
    mi_stat_f(p_inodo, p_stat);
    return p_inodo;
}

/*
 * push_cache()
 * ----------------------------------------------------------
 * Inserta un uevo camino y su inodoo asociado en la cache circular.
 * Si la caché está llea, sobreescribe la entrada más antigua.
*/
void push_cache(const char *camino, unsigned int p_inodo){
    memset(cache.path[cache.last_item_pos], 0, TAMNOMBRE*MAX_PATH_DEPTH);
    memcpy(cache.path[cache.last_item_pos], camino, TAMNOMBRE*MAX_PATH_DEPTH);
    cache.p_inode[cache.last_item_pos] = p_inodo;
    cache.last_item_pos = (cache.last_item_pos+1)%CACHE_SIZE;
    if(cache.items<CACHE_SIZE)cache.items++;
}

/*
 * find_canche()
 * ----------------------------------------------------------
 * Busca un camino en la caché.
 * Devuelve la posición si existe, o -1 si no está.
 */
int find_cache(const char *camino){
    if (cache.items<CACHE_SIZE){
	    for(int i = 0; i<cache.items; i++){
	        if(strcmp(cache.path[i%CACHE_SIZE], camino)==0)return i;
	    }
    }
    else{
	    for(int i = cache.last_item_pos; i<cache.last_item_pos-CACHE_SIZE; i--){
	        if(strcmp(cache.path[i%CACHE_SIZE], camino)==0)return i;
	    }
    }
    return -1;
}

//Maybe write a function to get p_inode since the same code is used in both functions?
/*
 * mi_write()
 * ----------------------------------------------------------
 * Escribe datos en un fichero a partir de un camino.
 * Utiliza la caché para acelerar búsquedas repetidas.
*/
int mi_write(const char *camino, void *buffer, unsigned int offset, unsigned int nbytes){
    int cache_pos = find_cache(camino);
    unsigned int p_inodo = 0;

    if(cache_pos>=0){
        //Utilizamos el inodo almacenado en caché
	    p_inodo = cache.p_inode[cache_pos];
	    #if DBGLVL9
	    xpperror("\n[ mi_write() -> Utilizamos datos almacenados en caché ]\n", BLUE, DEFAULT, false, false);
	    #endif
    }else{
        //No está en caché: buscamos la entrada y actualizamos caché
	    superblock SB;
	    bread(SBPOS, &SB);
	    unsigned int rootInode = SB.rootInode;
	    unsigned int p_entrada = 0;
	    int err = buscar_entrada(camino, &rootInode, &p_inodo, &p_entrada, 0, 0);
	    if(err<0)return err;
	    push_cache(camino, p_inodo);
	    #if DBGLVL9
	    xpperror("\n[ mi_write() -> Actualizar caché ]\n", ORANGE, DEFAULT, false, false);
	    #endif
    }
    return mi_write_f(p_inodo, buffer, offset, nbytes);
}

/*
 * mi_read()
 * ----------------------------------------------------------
 * Lee datos de un fichero a partir de un camino.
 * También utiliza la caché para evitar búsquedas repetidas.
 * 
 * Parámetros:
 *   camino -> ruta del fichero
 *   buffer -> buffer donde se guardan los datos leídos
 *   offset -> desplazamiento incial dentro del fichero
 *   nbytes -> número de bytes a leer
*/
int mi_read(const char *camino, void *buffer, unsigned int offset, unsigned int nbytes){
    int cache_pos = find_cache(camino);
    unsigned int p_inodo = 0;

    if(cache_pos>=0){
        //Utilizamos el inodo almacenado en caché
	    p_inodo = cache.p_inode[cache_pos];
	    #if DBGLVL9
	    xpperror("\n[ mi_read() -> Utilizamos datos almacenados en caché ]\n", BLUE, DEFAULT, false, false);
	    #endif
    }else{
        //No está en caché: buscamos la entrada y actualizamos caché
	    superblock SB;
	    bread(SBPOS, &SB);
	    unsigned int rootInode = SB.rootInode;
	    unsigned int p_entrada = 0;
	    int err = buscar_entrada(camino, &rootInode, &p_inodo, &p_entrada, 0, 0);
	    if(err<0)return err;
	    push_cache(camino, p_inodo);
	    #if DBGLVL9
	    xpperror("\n[ mi_read() -> Actualizar caché ]\n", ORANGE, DEFAULT, false, false);
	    #endif
    }
    return mi_read_f(p_inodo, buffer, offset, nbytes);
}

/*
 * mi_link()
 * ----------------------------------------------------------
 * Crea un enlace duro: camino2 pasa a apuntar al mismo inodo que camino1.
 * 
 * Pasos:
 *   Buscar camino1(debe existir y ser un fichero)
 *   Crear entrada camino2
 *   Modificar la entrada para que apunte al mismo inodo
 *   Liberar el inodo reservado para camino2
 *   Incrementar nlinks del inodo original
*/
int mi_link(const char *camino1, const char *camino2){
    unsigned int p_inodo1, p_inodo2, p_inodo_dir1, p_inodo_dir2;
    unsigned int p_entrada1, p_entrada2;
    inode inodo1;
    entrada entrada2;
    superblock SB;

    bread(SBPOS, &SB);
    p_inodo_dir1 = SB.rootInode;
    p_inodo_dir2 = SB.rootInode;

    //Inicio sección critica
    mi_waitSem();

    //Buscar camino1 (tiene que existir)
    int r = buscar_entrada(camino1, &p_inodo_dir1, &p_inodo1, &p_entrada1, 0, 0);
    if(r<0) return r;

    //Leer inodo1
    leer_inodo(p_inodo1, &inodo1);

    //Comprobar que és un fichero y tiene permiso de lectura
    if(inodo1.type!='f') return ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO;
    if(!has_perms(inodo1.perms, PERM_READ)) return ERROR_PERMISO_LECTURA;

    //Crear entrada camino2
    r = buscar_entrada(camino2, &p_inodo_dir2, &p_inodo2, &p_entrada2, 1, PERM_READ | PERM_WRITE);
    if(r<0) return r;

    //Leer entrada creada
    mi_read_f(p_inodo_dir2, &entrada2, p_entrada2*sizeof(entrada), sizeof(entrada));

    //Modificar entrada: apuntar al mismo inodo que camino2
    entrada2.ninodo = p_inodo1;

    //Escribir entrada modificada
    mi_write_f(p_inodo_dir2, &entrada2, p_entrada2*sizeof(entrada), sizeof(entrada));

    //Liberar el inodo que se habia reservado para camino2
    liberar_inodo(p_inodo2);

    //Incrementar nlinks del inodo original
    inodo1.nlinks++;
    inodo1.ctime = time(NULL);
    escribir_inodo(p_inodo1, &inodo1);

    //Final sección critica
    mi_signalSem();
    
    return EXITO;
}

/*
 * mi_unlink()
 * ----------------------------------------------------------
 * Elimina una entrada de directorio.
 * Si el inodo queda sin enlaces, se libera.
 * 
 * Pasos:
 *   Buscar la entrada
 *   Comprobar si es directorio vacío
 *   Reorganizar entradas si no es la última
 *   Actualizar tamaño lógico del directorio
 *   Truncar si es necessario
 *   Decrementar nlinks y liberar si llega a 0
*/
int mi_unlink(const char *camino){
    unsigned int p_inodo, p_inodo_dir, p_entrada;
    inode inodo, inodo_dir;
    entrada ultima;
    superblock SB;

    bread(SBPOS, &SB);
    p_inodo_dir = SB.rootInode;

    //Inicio sección critica
    mi_waitSem();

    //Buscar entrada
    int r = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 0);
    if(r<0) return r;

    //Leer inodo a eliminar
    leer_inodo(p_inodo, &inodo);

    //Si es directorio y no esta vacío -> error
    if(inodo.type=='d' && inodo.logicByteSize>0) return ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO;

    //Leer inodo del directorio padre
    leer_inodo(p_inodo_dir, &inodo_dir);

    int nentradas = inodo_dir.logicByteSize/sizeof(entrada);

    //Si no es la última entrada -> copiar última
    if(p_entrada!=nentradas-1){
        mi_read_f(p_inodo_dir, &ultima, (nentradas-1)*sizeof(entrada), sizeof(entrada));
        mi_write_f(p_inodo_dir, &ultima, p_entrada*sizeof(entrada), sizeof(entrada));
    }

    //Actualitzar tamany lógic del directorio padre
    inodo_dir.mtime = time(NULL);
    inodo_dir.ctime = time(NULL);
    escribir_inodo(p_inodo_dir, &inodo_dir);

    //Truncar físicamente si es necesario
    mi_truncar_f(p_inodo_dir, inodo_dir.logicByteSize-sizeof(entrada));
    //Decrementar nlinks
    inodo.nlinks--;

    //Si nlinks=0 -> eliminar inodo
    if(inodo.nlinks==0){
        liberar_inodo(p_inodo);
    } else{
        inodo.ctime = time(NULL);
        escribir_inodo(p_inodo, &inodo);
    }

    //Final seccion critica
    mi_signalSem();

    return EXITO;
}
