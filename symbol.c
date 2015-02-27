/***********************************************************************
 * **YOUR GROUP INFO SHOULD GO HERE**
 *
 * Team Member: 
 * Zhongxing Chen(998471283), Chong Zhu(997962940)
 **********************************************************************/

#include "symbol.h"

stack_frame *TOP = NULL;  //top of the stack

/* symbol table operations */
void symtbl_init(stack_frame *sp){
  int i;
  for (i=0; i<MAX_ENTRY_SIZE; i++){
    (sp->table)[i].var = (char *)"";
    (sp->table)[i].type = -1;
    (sp->table)[i].constness = -1;
    (sp->table)[i].qualifier = -1;
    (sp->table)[i].linenum = -1;
  }
}

int symtbl_current_size(stack_frame *sp){
  int i;
  for (i=0; i<MAX_ENTRY_SIZE; i++){
    if ((sp->table)[i].type == -1)
      return i; 
  }
  return MAX_ENTRY_SIZE; //stack full
}

/* Always insert into the current stack_frame pointed by TOP 
 * since the current stack_frame denotes the current scope */
int insert_symbol(char *_var, int _type, int _constness, int _qualifier, int _linenum){
  int index_to_insert = symtbl_current_size(TOP);
  if (index_to_insert != MAX_ENTRY_SIZE) {
    (TOP->table)[index_to_insert].var = _var;
    (TOP->table)[index_to_insert].type = _type;
    (TOP->table)[index_to_insert].constness = _constness;
    (TOP->table)[index_to_insert].qualifier = _qualifier;
    (TOP->table)[index_to_insert].linenum = _linenum;
    return 0;
  }
  return -1; //operation failed.
}

symtbl_entry *get_symbol_entry(char *_var){
  int i;
  stack_frame *scope = TOP;
  while(scope != NULL){
    for (i=0; i<MAX_ENTRY_SIZE; i++){
      if (!strcmp((scope->table)[i].var, _var))
        return &((scope->table)[i]); 
    }
    scope = scope->next;
  }
  return NULL; //symbol not defined anywhere in the program
}

symtbl_entry *get_first_def_of_dup(char *_var){
  int i, occurrence = 0;
  stack_frame *scope = TOP;
  symtbl_entry *first_def = NULL;
  for (i=0; i<MAX_ENTRY_SIZE; i++){
    if (!strcmp((scope->table)[i].var, _var)) {
      occurrence++;
      if (occurrence == 1)
      first_def = &((scope->table)[i]); 
    }
  }
  if (occurrence == 1)
    return NULL; //no duplicate symbol found in the current scope
  else
    return first_def;
}

int get_type(char *_var){
  symtbl_entry *sym_entry = get_symbol_entry(_var); 
  if (sym_entry != NULL)
    return sym_entry->type;
  return -1; //symbol doesn't have a type.(i.e. undefined var)
}

/* stack operations 
 * - top(): returns the top of the stack (current scope)
 * - push(): push a new stack_frame onto the stack(i.e. create new scope)
 * - pop(): pop the top of the stack (i.e. exit current scope) */
stack_frame *top(){
  return TOP;
}

void push(){
  if (TOP == NULL) {
    TOP = (stack_frame *)malloc(sizeof(stack_frame));
    TOP->next = NULL;
    symtbl_init(TOP);
    return;
  }
  stack_frame *new_frame = (stack_frame *)malloc(sizeof(stack_frame));
  new_frame->next = TOP;
  TOP = new_frame;
  symtbl_init(TOP);
}

void pop(){
  if (TOP == NULL)
    return;
  stack_frame *new_top = TOP->next;
  free(TOP);
  TOP = new_top;
  new_top = NULL;
}

