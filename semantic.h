/***********************************************************************
 * **YOUR GROUP INFO SHOULD GO HERE**
 *
 * Team Member: 
 * Zhongxing Chen(998471283), Chong Zhu(997962940)
 **********************************************************************/
#ifndef _SEMANTIC_H
#define _SEMANTIC_H

#include "ast.h"
#include "symbol.h"
#include "common.h"
int semantic_check(node *ast);
char *reverse_typeid_to_type(int id);
void scope_symtbl_ctor(node *scope);
#endif
