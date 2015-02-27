/***********************************************************************
 * **YOUR GROUP INFO SHOULD GO HERE**
 *
 * Team Member: 
 * Zhongxing Chen(998471283), Chong Zhu(997962940)
 **********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "common.h"
#include "parser.tab.h"
#include "semantic.h"
#define DEBUG_PRINT_TREE 0
node *ast = NULL;
node *inner_decl_head=NULL;
node *inner_stmt_head=NULL;
node *outer_decl_head=NULL;
node *outer_stmt_head=NULL;

//static int decl_index=0;
//static int stmt_index=0;

//node * decl_nodes[ARRAY_SIZE];
//node * stmt_nodes[ARRAY_SIZE];
node * arguments; 

void add_decl_node(node * _new, node * head){
if(head->decls._decls_index>=ARRAY_SIZE){
printf("Exceed the max array size\n");
return;
}
head->decls._decls[head->decls._decls_index]= _new;
head->decls._decls_index++;
}

void add_stmt_node(node * _new, node * head){
if(head->stmts._stmts_index>=ARRAY_SIZE){
printf("Exceed the max array size\n");
return;
}
head->stmts._stmts[head->stmts._stmts_index]= _new;
head->stmts._stmts_index++;
}

void add_argu_node(node * _new, node * head){
if(head->argu_opt._argus_index>=10){
printf("Exceed the max array size\n");
return;
}
head->argu_opt._argus[head->argu_opt._argus_index]=_new;
head->argu_opt._argus_index++;
}


node *ast_allocate(node_kind kind, ...) {
  va_list args;

  // make the node
  node *AST = (node *) malloc(sizeof(node));
  memset(AST, 0, sizeof *AST);
  AST->kind = kind;

  va_start(args, kind); 

  switch(kind) {
  
  // ...

  //scope
  case SCOPE_NODE:
    AST->scope._decls = va_arg(args, node *);
    AST->scope._stmts = va_arg(args, node *);
    AST->scope.if_cond_reg_num = -1;
    break;

  //statment
  case STMTS_NODE:
    //AST->stmts.stmt = va_arg(args, node *);
    //AST->stmts._stmts[ARRAY_SIZE]={NULL};
    AST->stmts._stmts_index=0;
    AST->stmts._outer_stmts_head= va_arg(args, node *);
    AST->stmts.if_cond_reg_num = -1;
    break;
   
  //declariation
  case DECLS_NODE:
    //AST->decls.decl = va_arg(args, node *);
    //AST->decls.cur = va_arg(args, node *);
    //AST->decls._decls[ARRAY_SIZE] = {NULL};
    AST->decls._decls_index=0;
    AST->decls._outer_decls_head= va_arg(args, node *);
    AST->decls.if_cond_reg_num = -1;
    break;

  case DECLARATION_NODE:
    AST->decl.cnst = va_arg(args, int);
    AST->decl._type = va_arg(args, node *);
    AST->decl._id = va_arg(args, char *);
    AST->decl._expr = va_arg(args, node *);
    AST->decl.linenum = va_arg(args, int);
    AST->decl.if_cond_reg_num = -1;
    break;


  //statement
  case ASSIGNMENT_NODE:
    AST->assign_statement._var = va_arg(args, node *);
    AST->assign_statement._expr = va_arg(args, node *);
    AST->assign_statement.linenum = va_arg(args, int);
    AST->assign_statement.ret_type = -1;
    AST->assign_statement.if_cond_reg_num = -1;
    break;

  case IF_STATEMENT_NODE:
    AST->if_statement._if = va_arg(args, int);
    AST->if_statement._expr = va_arg(args, node *);
    AST->if_statement._if_statement = va_arg(args, node *);
    AST->if_statement._else = va_arg(args, int);
    AST->if_statement._else_statement = va_arg(args, node *);
    AST->if_statement.linenum=va_arg(args, int);
    AST->if_statement.if_cond_reg_num = -1;
    break;

  case NESTED_SCOPE_NODE:
    AST->nested_scope_statement._scope = va_arg(args, node *);
    AST->nested_scope_statement.if_cond_reg_num = -1; 
    break;

  case TYPE_NODE:
    AST->type._type = va_arg(args, int);
    break;
//EXPRESSION

//expr: expressions
  case EXPRESSION_NODE: 
    AST->expr.next = va_arg(args, node *);
    break;

  case BOOL_NODE: 
    AST->bool_expr.val = va_arg(args, int);
    break;

  case CONSTRUCTOR_NODE:
    AST->cnst_expr.type	     = va_arg(args, node *);
    AST->cnst_expr.argu	     = va_arg(args, node *);
    AST->cnst_expr.linenum   = va_arg(args, int);
    AST->cnst_expr.ret_type  = -1;
    break;

  case FUNCTION_NODE:
    AST->func_expr.func_name = va_arg(args, int);
    AST->func_expr.func_argu = va_arg(args, node *);
    AST->func_expr.linenum   = va_arg(args, int);
    AST->func_expr.ret_type  = -1;
    break;

  case UNARY_EXPRESION_NODE:
    AST->unary_expr.op 		= va_arg(args, int);
    AST->unary_expr.right 	= va_arg(args, node *);
    AST->unary_expr.linenum 	= va_arg(args, int);
    AST->unary_expr.ret_type	= -1;
    break;

  case BINARY_EXPRESSION_NODE:
    AST->binary_expr.op 	= va_arg(args, int);
    AST->binary_expr.left 	= va_arg(args, node *);
    AST->binary_expr.right 	= va_arg(args, node *);
    AST->binary_expr.linenum 	= va_arg(args, int);
    AST->binary_expr.ret_type	= -1;
    break;

//literals
/*
  case BOOL_NODE:
    AST->bool_expr.val = va_arg(args, int);
    break;
*/
  case INT_NODE:
    AST->int_expr.val = va_arg(args, int);
    break;

  case FLOAT_NODE:
    AST->float_expr.val = va_arg(args, double);
    break;

//misc
  case VAR_NODE:
    AST->var_expr.next = va_arg(args, node *);
    break;


//variableast.h
  case IDENT_NODE:
    AST->id_expr.var = va_arg(args, char *);
    AST->id_expr.linenum = va_arg(args, int);
    AST->id_expr.ret_type = -1; 
    break;

  case INDEX_NODE:
    AST->index_expr.var = va_arg(args, char *);
    AST->index_expr.index = va_arg(args, int);
    AST->index_expr.linenum = va_arg(args, int);
    AST->index_expr.ret_type = -1;
    break;

//arguments
  case ARGUMENTS_OPT_NODE:
     //AST->argu_opt._argus[20] = {NULL};
     AST->argu_opt._argus[0] = va_arg(args, node *);
     AST->argu_opt._argus_index = 1;
     break;

   
/*
  case ARGU_WITH_EXPR_NODE:
    AST->expr_argu.argus = va_arg(args, node *);
    AST->expr_argu.bool_expr = va_arg(args, node *);	

  // ...
*/
  default: break;
  }

  va_end(args);

  return AST;
}

void ast_free(node *ast) {
  if (ast == NULL)
    return;
   switch(ast->kind){
    case SCOPE_NODE: {
	if(ast->scope._decls!=NULL){
		ast_free(ast->scope._decls);
		}
	
	if(ast->scope._stmts!=NULL){
		ast_free(ast->scope._stmts);
		}
        free(ast);
	} break;

    case DECLS_NODE: {
 	if(ast->decls._decls_index!=0){
		int i=0;
		int tmp=ast->decls._decls_index;
		for(;i<tmp+1;i++){
			ast_free(ast->decls._decls[i]);
		}
		}
        free(ast);	
    	} break;
	
   case STMTS_NODE: {
 	if(ast->stmts._stmts_index!=0){
		int i=0;
		int tmp=ast->stmts._stmts_index;
		for(;i<tmp+1;i++){
			ast_free(ast->stmts._stmts[i]);
		}
		}
        free(ast);	
    	} break;

    case DECLARATION_NODE: {
	if(ast->decl._type!=NULL){
	ast_free(ast->decl._type);
	}
	
	if(ast->decl._expr!=NULL){
	ast_free(ast->decl._expr);
	}
        free(ast);
    } break;
   
    case IF_STATEMENT_NODE: {
    if(ast->if_statement._expr!=NULL)
    ast_free(ast->if_statement._expr);
    
    if(ast->if_statement._if_statement!=NULL)
    ast_free(ast->if_statement._if_statement);
    
    if(ast->if_statement._else_statement!=NULL)
    ast_free(ast->if_statement._else_statement);
    free(ast);
    } break;
    
    case ASSIGNMENT_NODE: {
    if(ast->assign_statement._var!=NULL)
    ast_free(ast->assign_statement._var);
    if(ast->assign_statement._expr!=NULL)
    ast_free(ast->assign_statement._expr);
    free(ast);
    } break; 

    case NESTED_SCOPE_NODE: {
	ast_free(ast->nested_scope_statement._scope);
        free(ast);
    } break;

    case EXPRESSION_NODE: {
      ast_free(ast->expr.next);
      free(ast);
    } break;

    case BOOL_NODE: {
      free(ast);
    } break;

    case FUNCTION_NODE: {
      if(ast->func_expr.func_argu!=NULL);
      ast_free(ast->func_expr.func_argu);
      free(ast);
    } break;

    case CONSTRUCTOR_NODE: {
    if(ast->cnst_expr.type!=NULL)
    ast_free(ast->cnst_expr.type);
    if(ast->cnst_expr.argu!=NULL)
    ast_free(ast->cnst_expr.argu);
    free(ast);
    } break;



    case ARGUMENTS_OPT_NODE: {
      if(ast->argu_opt._argus_index!=0){
		int i=0;
		int tmp=ast->argu_opt._argus_index;
		for(;i<tmp+1;i++){
			ast_free(ast->argu_opt._argus[i]);
		}
		}
      free(ast);	
    } break;

    case UNARY_EXPRESION_NODE: {
      if (ast->unary_expr.right != NULL) {
        
        ast_free(ast->unary_expr.right);
      }
     free(ast);
    } break;

    case BINARY_EXPRESSION_NODE: { 
      if (ast->binary_expr.left != NULL && 
          ast->binary_expr.right != NULL) {
        ast_free(ast->binary_expr.left);
        ast_free(ast->binary_expr.right);
      }
      free(ast);
    } break;

    case INT_NODE: {
      free(ast);
    } break;

    case FLOAT_NODE: {
      free(ast);
    } break;

    case IDENT_NODE: {
      free(ast);
    } break;

    case INDEX_NODE: {
      free(ast);
    } break;

    case VAR_NODE: {
      if (ast->var_expr.next != NULL)
        ast_free(ast->var_expr.next);
      free(ast);
    } break;
    
    case TYPE_NODE: {
	free(ast);	
          }
    
    default: break;
  }
}

void ast_print(node * ast) {
  if (ast == NULL)
    return;
  switch(ast->kind) {
    case SCOPE_NODE: {
	printf("(SCOPE ");
	if(ast->scope._decls!=NULL){
		ast_print(ast->scope._decls);
		}
	
	if(ast->scope._stmts!=NULL){
		ast_print(ast->scope._stmts);
		}
        printf(")");
	} break;

    case DECLS_NODE: {
	printf("\n  (DECLARATIONS ");
 	if(ast->decls._decls_index!=0){
		int i=0;
		int tmp=ast->decls._decls_index;
		for(;i<tmp+1;i++){
			ast_print(ast->decls._decls[i]);
		}
		}	
        printf(")");
    	} break;
	
   case STMTS_NODE: {
	printf("\n  (STATEMENTS ");
 	if(ast->stmts._stmts_index!=0){
		int i=0;
		int tmp=ast->stmts._stmts_index;
		for(;i<tmp+1;i++){
			ast_print(ast->stmts._stmts[i]);
		}
		}	
        printf(")");
    	} break;

    case DECLARATION_NODE: {
	printf("\n    (DECLARATION ");
	printf("<%s> ",ast->decl._id);
	if(ast->decl.cnst!=0){
	printf("const ");
	}
	if(ast->decl._type!=NULL){
	ast_print(ast->decl._type);
	}
	
	if(ast->decl._expr!=NULL){
	ast_print(ast->decl._expr);
	}
        printf(")");
    } break;
   
    case IF_STATEMENT_NODE: {
    printf("\n    (IF ");

    if(ast->if_statement._expr!=NULL)
    ast_print(ast->if_statement._expr);
    
    if(ast->if_statement._if_statement!=NULL)
    ast_print(ast->if_statement._if_statement);
    
    if(ast->if_statement._else!=0)
    printf("\n    ELSE ");
    
    if(ast->if_statement._else_statement!=NULL)
    ast_print(ast->if_statement._else_statement);
    printf(")");
    } break;
    
    case ASSIGNMENT_NODE: {
    printf("\n    (ASSIGN %s ",reverse_typeid_to_type(ast->assign_statement.ret_type));
    if(ast->assign_statement._var!=NULL)
    ast_print(ast->assign_statement._var);
    if(ast->assign_statement._expr!=NULL)
    ast_print(ast->assign_statement._expr);
    printf(")");
    } break; 

    case NESTED_SCOPE_NODE: {
	printf("SCOPE\n  ");
	ast_print(ast->nested_scope_statement._scope);
    } break;

    

    case EXPRESSION_NODE: {
      ast_print(ast->expr.next);
    } break;

    case BOOL_NODE: {
      printf("<%s> ", (ast->bool_expr.val) ? "true" : "false");
    } break;

    case FUNCTION_NODE: {
      printf("(CALL ");
      if(ast->func_expr.func_name==0)
      printf("dp3 ");
      if(ast->func_expr.func_name==1)
      printf("lit ");
      if(ast->func_expr.func_name==2)
      printf("rsq ");
      if(ast->func_expr.func_argu!=NULL);
      ast_print(ast->func_expr.func_argu);
      printf(")");
    } break;

    case CONSTRUCTOR_NODE: {
    printf("(CALL (CONSTRUCTOR ");
    if(ast->cnst_expr.type!=NULL)
    ast_print(ast->cnst_expr.type);
    if(ast->cnst_expr.argu!=NULL)
    ast_print(ast->cnst_expr.argu);
    printf("))");
    } break;



    case ARGUMENTS_OPT_NODE: {
       printf("(ARGUMENTS_OPT ");
      if(ast->argu_opt._argus_index!=0){
		int i=0;
		int tmp=ast->argu_opt._argus_index;
		for(;i<tmp+1;i++){
			ast_print(ast->argu_opt._argus[i]);
		}
		}	
       printf(")");
    } break;

    case UNARY_EXPRESION_NODE: {
      if (ast->unary_expr.right != NULL) {
        printf("(UNARY %s ",reverse_typeid_to_type(ast->unary_expr.ret_type));
        switch(ast->unary_expr.op){
          case '-': printf("- "); break;
          case '!':  printf("! "); break;
          default: break;
	}
        ast_print(ast->unary_expr.right);
        printf(")");
      }
    } break;

    case BINARY_EXPRESSION_NODE: { 
      if (ast->binary_expr.left != NULL && 
          ast->binary_expr.right != NULL) {
        printf("(BINARY %s ",reverse_typeid_to_type(ast->binary_expr.ret_type));
        switch(ast->binary_expr.op){
          case AND: printf("&& "); break;
          case OR:  printf("|| "); break;
          case EQ:  printf("== "); break;
          case NEQ: printf("!= "); break;
          case '<': printf("< ");  break;
          case LEQ: printf("<= "); break;
          case '>': printf("> ");  break;
          case GEQ: printf(">= "); break;
          case '+': printf("+ ");  break;
          case '-': printf("- ");  break;
          case '*': printf("* ");  break;
          case '/': printf("/ ");  break;
          case '^': printf("^ ");  break;
          default: break;
	}
        ast_print(ast->binary_expr.left);
        ast_print(ast->binary_expr.right);
        printf(")");
      }
    } break;

    case INT_NODE: {
      printf("<%d> ", ast->int_expr.val);
    } break;

    case FLOAT_NODE: {
      printf("<%f> ", ast->float_expr.val);
    } break;

    case IDENT_NODE: {
      printf("<%s> ", ast->id_expr.var);
    } break;

    case INDEX_NODE: {
      printf("(INDEX %s ", reverse_typeid_to_type(ast->index_expr.ret_type ));
      printf("<%s> <%d>)", ast->index_expr.var, ast->index_expr.index);
    } break;

    case VAR_NODE: {
      if (ast->var_expr.next != NULL)
        ast_print(ast->var_expr.next);
    } break;
    
    case TYPE_NODE: {
	if(ast->type._type==INT_T)
	  printf("int ");
	if(ast->type._type==IVEC_T2)
	  printf("ivec2 ");
	if(ast->type._type==IVEC_T3)
	  printf("ivec3 ");
	if(ast->type._type==IVEC_T4)
	  printf("ivec4 ");
	if(ast->type._type==BOOL_T)
	  printf("bool ");
	if(ast->type._type==BVEC_T2)
   	  printf("bvec2 ");
	if(ast->type._type==BVEC_T3)
   	  printf("bvec3 ");
	if(ast->type._type==BVEC_T4)
   	  printf("bvec4 ");
	if(ast->type._type==FLOAT_T)
	  printf("float ");
	if(ast->type._type==VEC_T2)
	  printf("vec2 ");
	if(ast->type._type==VEC_T3)
	  printf("vec3 ");
	if(ast->type._type==VEC_T4)
	  printf("vec4 ");
          }
    
    default: break;
  }

}


