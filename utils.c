#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "utils.h"

#define MAX_MSG_SIZE 4096
//https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797

char *prettify(char *str,style *s){
    //return null if some argument is null
    if (!str || !s){
	    return NULL;
    }
    
    char *out;
    int ln = strlen(str);
    
    //allocate max string size and initialize to all 0
    out = calloc(ln+35, sizeof(char));
    if(!out)return NULL;
    int size = 0;
    if(s -> color == -1){
	    memcpy(out+size,"\x1b[39m",5);
	    size = 5;
    }
    else{
	    size = sprintf(out, "\x1b[38;5;%dm", s -> color);
    }

    if(s -> bgcolor == -1){
	    memcpy(out+size,"\x1b[49m",5);
	    size+= 5;
    }
    else{
	    size+= sprintf(out+size, "\x1b[48;5;%dm", s -> bgcolor);
    }

    if(s -> bold){
	    memcpy(out+size,"\x1b[1m",4);
	    size+=4;
    }
    if(s -> underline){
	    memcpy(out+size,"\x1b[4m",4);
	    size+=4;
    }
    
    memcpy(out+size, str, ln);
    memcpy(out+size+ln, "\x1b[0m\0", 5);
    return out;
}

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

/**
 * Funcion string replace.
 * str -> original string
 * fnd -> substring to replace
 * rpl -> substring to replace fnd with
 * lim -> maximum of times to replace (lim <=0 no limit)
 */
char *strpl(char *str, char *fnd, char *rpl, int lim){
    if(!str || !fnd || !rpl) return NULL;
    int lenstr = strlen(str);
    int lenfnd = strlen(fnd);
    int lenrpl = strlen(rpl);  
    char *out;
    if(lenstr == 0 || lenfnd == 0){
	    out = malloc(lenstr + 1);
	    memcpy(out, str, lenfnd+1);
	    return out;
    }
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
    out = calloc(sizeof(char), lenstr + ((lenrpl - lenfnd)*oc)+1);
    if (oc==0){
	    strcpy(out,str);
	    return out;
    }
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
    strncpy(out+pos, rplpospt[oc-1]+lenfnd, lenstr);
    return out;
}
