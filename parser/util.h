/****************************************************/
/* File: util.h                                     */
/* The interface of general tools for the           */
/* Pyc compiler                                 */
/*           */
/* MUST compiler  2024 Fall                                */
/****************************************************/

#ifndef _UTIL_H_
#define _UTIL_H_

#include "libs.h"


void clear_input_queue(void){
  int c;
  while((c = getchar()) != '\n' && c != EOF){
    // Do nothing
  };
};

/* Print the message msg, then wait for the user to hit the enter/return key.
 * The input queue is cleared before this function returns.
 */
void pause_msg(const char * msg){
  printf("%s", msg);
  getchar(); // Wait for the user to hit enter
  clear_input_queue();
  
  
};

/* <Parameter:>
 * str:  a character string.
 * <Return:>
 * A copy (a clone) of the input string str, including the ending '\0'. The space of the clone does not overlap with the space of str.
 * */
char * string_clone(const char* str){
  int i;
  int len = strlen(str);
  char * clone =(char *)malloc(len +1);
  while(i<len){
    clone[i] = str[i];
    i++;
  }
  clone[i]='\0';
  return clone;
};



void *checked_malloc(int len){
  void * p = malloc(len);
  if(!p){
    fprintf(stderr, "\nRan out of memory!\n");
    exit(1);
  }
  return p;
};


int read_file_to_char_array( char * array, int arrayLength, FILE * stream){
  int i = 0;
  int c;
  while((c=getc(stream))!=EOF && i<arrayLength){
    array[i]=c;
    i++;
  }
  array[i]='\0';
  return i;
};


char *  clone_string_section(const char * str, int begin, int end){
  int i;
  int len = end - begin;
  char * clone=(char *)malloc(len+1);
  for(i=0;i<len;i++){
    clone[i]=str[begin+i];
  }
  clone[i]='\0';
  return clone;
};


#endif