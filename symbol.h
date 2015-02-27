/***********************************************************************
 * **YOUR GROUP INFO SHOULD GO HERE**
 *
 * Team Member: 
 * Zhongxing Chen(998471283), Chong Zhu(997962940)
 **********************************************************************/
#ifndef _SYMBOL_H
#define _SYMBOL_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_ENTRY_SIZE 1000

typedef struct {
  char *var; //variable name
  int type; //type of the variable
  int constness;
  int qualifier;
  int linenum;
} symtbl_entry;

typedef struct stack_frame{
  symtbl_entry table[MAX_ENTRY_SIZE]; 
  stack_frame *next;
} stack_frame; 

extern stack_frame *TOP;  //top of the stack

void symtbl_init(stack_frame *sp);
int symtbl_current_size(stack_frame *sp);
int insert_symbol(char *_var, int _type, int _constness, int _qualifier, int _linenum);
symtbl_entry *get_symbol_entry(char *_var);
symtbl_entry *get_first_def_of_dup(char *_var);
int get_type(char *_var);

stack_frame *top();
void push();
void pop();
#endif

