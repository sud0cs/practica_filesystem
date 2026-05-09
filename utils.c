#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "utils.h"

#define MAX_MSG_SIZE 4096
//https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797

/*
 * prettify()
 * ----------------------------------------------------------
 * Aplica estilos ANSI(color, fondo, negrita, subrayado) a una cadena.
 * Devuleve una nueva cadena con los códigos de escape aplicados.
 * 
 * Parámetros:
 *   str -> texto original
 *   s -> estructura con los estilos a plicar
 * 
 * Devuelve:
 *   Cadena con formato(reservada con calloc)
 *   NULL si hay error o argumentos inválidos
*/
char *prettify(char *str,style *s){
  //Devueleve null si algun argumento es null
  if (!str || !s){
	  return NULL;
  }
    
  char *out;
  int ln = strlen(str);
    
  //Asigna el tamaño máximo de cadena y lo inicializa a cero
  out = calloc(ln+35, sizeof(char));
  if(!out)return NULL;
  
  int size = 0;
  
  //Color de texto
  if(s -> color == -1){
	  memcpy(out+size,"\x1b[39m",5);
	  size = 5;
  }else{
    size = sprintf(out, "\x1b[38;5;%dm", s -> color);
  }

  //Color de fondo
  if(s -> bgcolor == -1){
    memcpy(out+size,"\x1b[49m",5);
    size+= 5;
  }else{
    size+= sprintf(out+size, "\x1b[48;5;%dm", s -> bgcolor);
  }

  //Negrita
  if(s -> bold){
    memcpy(out+size,"\x1b[1m",4);
    size+=4;
  }

  //Subrayado
  if(s -> underline){
    memcpy(out+size,"\x1b[4m",4);
    size+=4;
  }
  
  //Copiar texto y resetear estilos
  memcpy(out+size, str, ln);
  memcpy(out+size+ln, "\x1b[0m\0", 5);
  return out;
}

/*
 * pprint()
 * ----------------------------------------------------------
 * Imprime un mensaje con estilo utilizando una estructura style.
 * Acepta el formato printf mediante argumentos variables.
*/
void pprint(char *str, style *s, ...){
  char buffer [MAX_MSG_SIZE];
  va_list args;
  va_start(args, s);
  vsprintf(buffer, str, args);
  va_end(args);
  char *pstr = prettify(buffer, s);
  printf("%s", pstr);
  free(pstr);
}

/*
 * xpprint()
 * ----------------------------------------------------------
 * Versión simplificada de pprint() utilizando colores directos en lugar de una
 * estructura style extrema.
*/
void xpprint(char *str, Color fg, Color bg, bool bold, bool underline, ...){
  char buffer [MAX_MSG_SIZE];
  style s = {fg,bg,bold,underline};
  va_list args;
  va_start(args, underline);
  vsprintf(buffer, str, args);
  va_end(args);
  char *pstr = prettify(buffer, &s);
  printf("%s", pstr);
  free(pstr);
}

/*
 * pperror()
 * ----------------------------------------------------------
 * Igual que pprint(), pero imprime por stderr.
*/
void pperror(char *str, style *s, ...){
  char buffer [MAX_MSG_SIZE];
  va_list args;
  va_start(args, s);
  vsprintf(buffer, str, args);
  va_end(args);
  char *pstr = prettify(buffer, s);
  fprintf(stderr, "%s", pstr);
  free(pstr);
}

/*
 * xpperror()
 * ----------------------------------------------------------
 * Igual que xpprint(), pero imprime por stderr.
*/
void xpperror(char *str, Color fg, Color bg, bool bold, bool underline, ...){
  char buffer [MAX_MSG_SIZE];
  style s = {fg,bg,bold,underline};
  va_list args;
  va_start(args, underline);
  vsprintf(buffer, str, args);
  va_end(args);
  char *pstr = prettify(buffer, &s);
  fprintf(stderr, "%s", pstr);
  free(pstr);
}

/*
 * strpl()
 * ----------------------------------------------------------
 * Reemplaza todas(o un nñumero limitado) de apariciones de un substring dentro de una cadena.
 * 
 * Parámetros:
 *   str -> cadeana original
 *   fnd -> substring a buscar
 *   rpl -> substring por la que reemplazar
 *   lim -> numero máximo de reemplazos(<=0 = sin límite)
 * 
 * Devuelve:
 *   nueva cadena con los reemplazos aplicados
 *   NULL si hay error
 */
char *strpl(char *str, char *fnd, char *rpl, int lim){
  if(!str || !fnd || !rpl) return NULL;
  int lenstr = strlen(str);
  int lenfnd = strlen(fnd);
  int lenrpl = strlen(rpl);  
  char *out;

  //Casos triviales
  if(lenstr == 0 || lenfnd == 0){
	  out = malloc(lenstr + 1);
	  memcpy(out, str, lenfnd+1);
	  return out;
  }

  //Buscar posiciones donde aparece fnd
  int oc = 0;
  char *rplpospt[lenstr/lenfnd];

  for(int i = 0; i<=lenstr-lenfnd ;i++){
	  if (oc>=lim&&lim>0)break;
	  if(strncmp(str+i, fnd, lenfnd)==0){
	    rplpospt[oc] = str+i;
	    oc+=1;
	    i+=lenfnd;
	  }
  }

  //Reservar memoria para la cadena resultante
  out = calloc(sizeof(char), lenstr + ((lenrpl - lenfnd)*oc)+1);

  //Si no hay coincidencias, copiar original
  if (oc==0){
	  strcpy(out,str);
	  return out;
  }

  //Construcción de la cadena resultante
  size_t pos = 0;
  size_t sz = 0;

  for(int i = 0; i<oc; i++){
	  if(i==0){
	    strncpy(out,str,(rplpospt[0]-str));
	    strncpy(out+(rplpospt[0]-str),rpl,lenrpl);
	    pos += (rplpospt[0]-str)+lenrpl;
	  }
	  else{
	    sz = rplpospt[i]-(rplpospt[i-1]+lenfnd);
	    strncpy(out+pos, rplpospt[i-1]+lenfnd, sz);
	    strncpy(out+sz+pos, rpl, lenrpl);
	    pos += sz+lenrpl;
	  }
  }

  //Copiar el resto de la cadena
  size_t remaining = lenstr - (rplpospt[oc-1] - str) - lenfnd;
  if (remaining > 0) memcpy(out + pos, rplpospt[oc-1] + lenfnd, remaining);
  
  return out;
}
