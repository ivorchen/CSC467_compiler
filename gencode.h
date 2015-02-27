#ifndef GENCODE_H_
#define GENCODE_H_ 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "common.h"
#include "semantic.h"

#define MAX_EXPR_REG_NUM 100

typedef struct {
  int num;
  int status; // 0 for free, 1 for occupied. 
} register_info;

void reg_info_init();
int reg_alloc();
void reg_free();
int reg_assign();
void reg_mark_to_be_freed(int i);

void GenCode(node *);

#endif
