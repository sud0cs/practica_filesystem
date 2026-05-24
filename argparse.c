#include "argparse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

arg* args;
unsigned int arg_count = 0;
int argc;
char **argv;

//tipos que puede almacenar el parser
char* get_type_name(arg_type t){
  switch(t){
    case CHAR:
      return "CHAR";
    case STRING:
      return "STRING";
    case INT:
      return "INT";
    case FLOAT:
      return "FLOAT";
    case BOOL:
      return "BOOL";
    default:
      return NULL;
  }
}

/*
 * init_parser()
 * ----------------------------------------------------------
 * inicializa las variables del parser
 * 
 * Parámetros:
 *   max_args -> número de argumentos que puede leer el parser
 *   _argc -> argc original
 *   _argv -> argv original
 * 
*/
void init_parser(unsigned int max_args, int _argc, char **_argv){
  args = calloc(max_args, sizeof(arg));
  argc = _argc;
  argv = malloc(sizeof(_argv));
  for(int i = 0; i<_argc; i++){
    argv[i] = malloc(strlen(_argv[i])+1);
    strcpy(argv[i], _argv[i]);
  }
}

/*
 * find_arg()
 * ----------------------------------------------------------
 * encuentra el argumento basado en su nombre
 * 
 * Parámetros:
 *   name -> nombre del argumento
 * 
 * devuelve:
 *   el argumento encontrado o NULL
*/
arg* find_arg(char *name){
  for(int i = 0; i<arg_count; i++){
      if(strcmp(name, args[i].name)==0)return &args[i];
  }
  return NULL;
}

/*
 * add_arg()
 * ----------------------------------------------------------
 * añade el argumento para que el parser lo pueda leer
 * 
 * Parámetros:
 *   name -> nombre del argumento
 *   mandatory -> si el argumento es obligatorio o no
 *   type -> tipo de variable a leer
 *   description -> descripción del argumento
 * 
*/
void add_arg(char *name, bool mandatory, arg_type type, char* description){
  arg _arg;
  if(strcmp(name, "") == 0 || find_arg(name)!=NULL || (mandatory && type==NONE))return;
  _arg.name = name;
  _arg.mandatory = mandatory;
  _arg.content = NULL;
  _arg.type = type;
  _arg.description = description;
  args[arg_count] = _arg;
  arg_count++;
}

/*
 * print_help()
 * ----------------------------------------------------------
 * Imprime el mensaje de ayuda del programa
 * 
*/
void print_help(){
  xpprint("%s ", DEFAULT, DEFAULT, true, false, argv[0]);
  for(int i = 0; i<arg_count; i++){
    if(args[i].mandatory)printf("<%s> ", args[i].name);
  }
  printf("\n---------------------\n\n");
  for(int i = 0; i<arg_count; i++){
    if(args[i].mandatory){xpprint("<%s> [%s]", DEFAULT, DEFAULT, true, false, args[i].name, get_type_name(args[i].type));printf(": %s\n", args[i].description);}
    else {
      if(args[i].type==NONE){xpprint("-%s", DEFAULT, DEFAULT, true, false, args[i].name);printf(": %s\n", args[i].description);}
      else{xpprint("-%s [%s]", DEFAULT, DEFAULT, true, false, args[i].name, get_type_name(args[i].type));printf(": %s\n", args[i].description);}
    } 
  }
}

/*
 * arg_name_in_argv()
 * ----------------------------------------------------------
 * Busca el nombre de un argumento entre los argumentos pasados al programa (argv)
 * 
 * Parámetros:
 *   _arg -> argumento a buscar
 *   i -> índice a partir del cual buscar en argv
 * 
 * Devuelve:
 *   true si el argumento se encuentra, si no false.
*/
bool arg_name_in_argv(arg _arg, int i){
  for(;i<argc; i++){
    if(argv[i][0]=='-' && strcmp(strpl(argv[i], "-", "", 1), _arg.name)==0)return true;
  }
  return false;
}

/*
 * find_empty_mandatory_arg()
 * ----------------------------------------------------------
 * busca el primer argumento obligatorio sin ningún dato almacenado
 * 
 * Parámetros:
 *   argv_i -> índice para buscar en arg_name_in_argv
 * 
 * devuelve:
 *   el primer argumento vacío si se encuentra, si no null
*/
arg* find_empty_mandatory_arg(int argv_i){
  for(int i = 0; i<arg_count; i++){
    if(args[i].mandatory && args[i].content == NULL && !arg_name_in_argv(args[i], argv_i))return &args[i];
  }
  return NULL;
}

/*
 * parse_args()
 * ----------------------------------------------------------
 * busca los argumentos creados en los argumentos pasados al programa
 * y almacena sus datos con el tipo correspondiente de variable
 * 
 * devuelve:
 *   El número de argumentos encontrados o MISSING_ARGS_ERROR en caso
 *   de que haya argumentos obligatorios sin pasar al programa
*/
int parse_args(){
  arg* _arg;
  int offset;
  int ac = 0;
  for(int i = 1; i<argc; i++){
    offset = 1;
    _arg = find_arg(strpl(argv[i], "-", "", 1));
    if (_arg==NULL || argv[i][0]!='-'){
      _arg = find_empty_mandatory_arg(i);
      if(_arg == NULL)continue;
      offset = 0;
    }
    if (_arg -> type == NONE)offset = 0;
    if(i+offset>=argc)return ac;
    switch (_arg -> type){
      case STRING:
        char* _str;
        _str = malloc(strlen(argv[i+offset])+1);
        strcpy(_str, argv[i+offset]);
        _arg -> content = _str;
        break;
      case CHAR:
        char* _char = malloc(sizeof(char));
        *_char = argv[i+offset][0];
        _arg -> content = _char;
        break;
      case INT:
        int* _int = malloc(sizeof(int));
        *_int = atoi(argv[i+offset]);
        _arg -> content = _int;
        break;
      case FLOAT:
        float* _float = malloc(sizeof(float));
        *_float = atof(argv[i+offset]);
        _arg -> content = _float;
        break;
      case BOOL:
        bool *_bool = malloc(sizeof(bool));
        *_bool = false;
        strlower(argv[i+offset]);
        if(strcmp(argv[i+offset],"true")==0 || atoi(argv[i+offset]))*_bool = true;
        _arg -> content = _bool;
        break;
      case NONE:
        _arg -> content = _arg;
        break;
    }
    i+=offset;
    ac++;
  }
  for(int i = 0; i<arg_count; i++){
    if(args[i].mandatory && args[i].content == NULL)return MISSING_ARGS_ERROR;
  }
  return ac;
}

/*
 * add_arg()
 * ----------------------------------------------------------
 * Devuelve true si el argumento se ha pasado al programa
 * 
 * Parámetros:
 *   name -> nombre del argumento
 * 
*/
bool arg_exists(char* name){
    arg *_arg = find_arg(name);
    return (_arg!=NULL&&_arg->content!=NULL);
}

/*
 * add_value()
 * ----------------------------------------------------------
 * Devuelve el contenido de un argumento
 * 
 * Parámetros:
 *   name -> nombre del argumento
 * 
*/
void* arg_value(char *name){
    arg* _arg = find_arg(name);
    if(_arg==NULL)return NULL;
    return _arg -> content;
}

/*
 * free_args()
 * ----------------------------------------------------------
 * Libera todas las variables
 *
*/
void free_args(){
    for(int i = 0; i<arg_count; i++){
	    if(args[i].type == NONE){
        args[i].content = NULL;
      }
      free(args[i].content);
    }
    free(argv);
}
