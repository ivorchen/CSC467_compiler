/***********************************************************************
 * **YOUR GROUP INFO SHOULD GO HERE**
 *
 * Team Member: 
 * Zhongxing Chen(998471283), Chong Zhu(997962940)
 **********************************************************************/

#include "semantic.h"
#include <assert.h> 
#include "parser.tab.h"

#define GET_DECL_NUM(scope_node) scope_node->scope._decls->decls._decls_index
#define GET_DECL_VAR(scope_node, i)  scope_node->scope._decls->decls._decls[i]
#define GET_DECL_VAR_NAME(scope_node, i)  GET_DECL_VAR(scope_node, i)->decl._id 
#define GET_DECL_VAR_TYPE(scope_node, i)  GET_DECL_VAR(scope_node, i)->decl._type->type._type
#define GET_DECL_VAR_CONST(scope_node, i)  GET_DECL_VAR(scope_node, i)->decl.cnst
#define GET_DECL_VAR_LINENUM(scope_node, i)  GET_DECL_VAR(scope_node, i)->decl.linenum

#define GET_STMT(scope_node, i) scope_node->scope._stmts->stmts._stmts[i]
#define GET_STMT_NUM(scope_node) scope_node->scope._stmts->stmts._stmts_index

#define NUM_OF_PREDEF_VAR 13

static int scope_num = 0;

int BOOL_S_V_TYPE(int type) {
  switch(type) {
    case BOOL_T:
    case BVEC_T2:
    case BVEC_T3:
    case BVEC_T4: return 1; break;
    default: return 0; break;
  }
}

int ARITH_S_V_TYPE(int type) {
  switch(type) {
    case INT_T:
    case IVEC_T2:
    case IVEC_T3:
    case IVEC_T4:
    case FLOAT_T:
    case VEC_T2:
    case VEC_T3:
    case VEC_T4: return 1; break;
    default: return 0; break;
  }
}

char *reverse_opnum_to_op(int id){
  switch (id){
    case AND: return (char *)"&&"; break;
    case OR:  return (char *)"||"; break;
    case EQ:  return (char *)"=="; break;
    case NEQ: return (char *)"!="; break;
    case '<': return (char *)"<"; break;
    case LEQ: return (char *)"<="; break;
    case '>': return (char *)">"; break;
    case GEQ: return (char *)">="; break;
    case '+': return (char *)"+"; break;
    case '-': return (char *)"-"; break;
    case '*': return (char *)"*"; break;
    case '/': return (char *)"/"; break;
    case '^': return (char *)"^"; break;
    default: break;
  }
  return (char *)"";
}

char *reverse_typeid_to_type(int id){
  switch (id){
    case INT_T: return (char *)"int"; break;
    case IVEC_T2:  return (char *)"ivec2"; break;
    case IVEC_T3:  return (char *)"ivec3"; break;
    case IVEC_T4:  return (char *)"ivec4"; break;
    case FLOAT_T:  return (char *)"float"; break;
    case VEC_T2:  return (char *)"vec2"; break;
    case VEC_T3:  return (char *)"vec3"; break;
    case VEC_T4:  return (char *)"vec4"; break;
    case BOOL_T: return (char *)"bool"; break;
    case BVEC_T2:  return (char *)"bvec2"; break;
    case BVEC_T3:  return (char *)"bvec3"; break;
    case BVEC_T4:  return (char *)"bvec4"; break;
    default: return (char *)"ANY"; break;
  }
}

#if 1
enum qualifier_kind{
  RESULT,
  ATTRIBUTE,
  UNIFORM,
  NOT_PREDEF
};
#endif

typedef struct predef_var_info {
  char *var;
  int type;
  int constness;
  int qualifier;
}predef_var_info;

predef_var_info get_predef_var_info(int predef_var_index) {
  predef_var_info info;
  info.type = -1;
  info.qualifier = -1;
  switch(predef_var_index) {
    case 1: {
      info.type = BOOL_T; info.qualifier = 0; //qualifier: result
      return info;
    } break;
    case 0:
    case 2: {
      info.type = VEC_T4; info.qualifier = 0; //qualifier: result
      return info;
    } break;

    case 3:
    case 4:
    case 5:
    case 6: {
      info.type = VEC_T4; info.qualifier = 1; //qualifier: attribute
      return info;
    } break;

    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12: {
      info.type = VEC_T4; info.qualifier = 2; //qualifier: uniform
      return info;
    } break;
    default: break;
  }
  return info;
}

int is_predef_var(char *var) {
  if (!strcmp("gl_FragColor", var)) return 0; 
  if (!strcmp("gl_FragDepth", var)) return 1; 
  if (!strcmp("gl_FragCoord", var)) return 2; 

  if (!strcmp("gl_TexCoord", var)) return 3; 
  if (!strcmp("gl_Color", var)) return 4; 
  if (!strcmp("gl_Secondary", var)) return 5; 
  if (!strcmp("gl_FogFragCoord", var)) return 6; 

  if (!strcmp("gl_Light_Half", var)) return 7; 
  if (!strcmp("gl_Light_Ambient", var)) return 8; 
  if (!strcmp("gl_Material_Shininess", var)) return 9; 
  
  if (!strcmp("env1", var)) return 10; 
  if (!strcmp("env2", var)) return 11; 
  if (!strcmp("env3", var)) return 12; 
  
  return -1;
}

int isUniform(char *var) {
  if (!strcmp("gl_Light_Half", var)) return 1; 
  if (!strcmp("gl_Light_Ambient", var)) return 1; 
  if (!strcmp("gl_Material_Shininess", var)) return 1; 
  
  if (!strcmp("env1", var)) return 1; 
  if (!strcmp("env2", var)) return 1; 
  if (!strcmp("env3", var)) return 1; 

  return 0;
}

int isResult(char *var) {
  if (!strcmp("gl_FragColor", var)) return 1; 
  if (!strcmp("gl_FragDepth", var)) return 1; 
  if (!strcmp("gl_FragCoord", var)) return 1; 

  return 0;
}

int isAttribute(char *var) {
  if (!strcmp("gl_TexCoord", var)) return 1; 
  if (!strcmp("gl_Color", var)) return 1; 
  if (!strcmp("gl_Secondary", var)) return 1; 
  if (!strcmp("gl_FogFragCoord", var)) return 1; 

  return 0;
}

static predef_var_info predef_var[NUM_OF_PREDEF_VAR] = {
  {(char *)"gl_FragColor",         VEC_T4, 0, RESULT},
  {(char *)"gl_FragDepth",         BOOL_T, 0, RESULT},
  {(char *)"gl_FragCoord",         VEC_T4, 0, RESULT},

  {(char *)"gl_TexCoord",          VEC_T4, 0, ATTRIBUTE},
  {(char *)"gl_Color",             VEC_T4, 0, ATTRIBUTE},
  {(char *)"gl_Secondary",         VEC_T4, 0, ATTRIBUTE},
  {(char *)"gl_FogFragCoord",      VEC_T4, 0, ATTRIBUTE},

  {(char *)"gl_Light_Half",        VEC_T4, 1, UNIFORM},
  {(char *)"gl_Light_Ambient",     VEC_T4, 1, UNIFORM},
  {(char *)"gl_Material_Shininess",VEC_T4, 1, UNIFORM},
  
  {(char *)"env1",                 VEC_T4, 1, UNIFORM},
  {(char *)"env2",                 VEC_T4, 1, UNIFORM},
  {(char *)"env3",                 VEC_T4, 1, UNIFORM}
};

void scope_symtbl_ctor(node *scope, int scope_num){
  push(); //create new symtbl when entering new scope
  int i;
  if (scope_num == 1) {
    for (i=0; i<NUM_OF_PREDEF_VAR; i++)
      insert_symbol(predef_var[i].var, predef_var[i].type, predef_var[i].constness, predef_var[i].qualifier, 0);
  }
  for (i=0; i<GET_DECL_NUM(scope); i++)
    insert_symbol(GET_DECL_VAR_NAME(scope, i), GET_DECL_VAR_TYPE(scope, i), GET_DECL_VAR_CONST(scope, i), NOT_PREDEF, GET_DECL_VAR_LINENUM(scope, i));
  return;
}

void scope_symtbl_dtor(node *scope) {
  pop();
}

void print_scope_symtbl(stack_frame *sp){
  if (sp == NULL)
    return;
  int i;
  printf("------------------------------------------------------\n");
  for (i=0; i<symtbl_current_size(sp); i++)
    printf("Entry %d: var:%s -> type_id:%d constness:%d qualifier:%d -> defined on LINE %d\n",i,sp->table[i].var,sp->table[i].type,sp->table[i].constness, sp->table[i].qualifier, sp->table[i].linenum);
  printf("------------------------------------------------------\n");
}

int check_expr_type(node *subexpr) {
  switch (subexpr->kind) {
    case EXPRESSION_NODE: return check_expr_type( subexpr->expr.next); break;
    case INT_NODE: return INT_T; break; 
    case FLOAT_NODE: return FLOAT_T; break;
    case BOOL_NODE: return BOOL_T; break;
    case IDENT_NODE: {
#if 0 
      if (isResult(subexpr->id_expr.var)) {
        fprintf(errorFile,"LINE %d: The predefined variable '%s' is write-only.\n", subexpr->id_expr.linenum, subexpr->id_expr.var);
	return -1;
      }
#endif
      int id_type = get_type(subexpr->id_expr.var);
      if (id_type == -1)
        fprintf(errorFile,"LINE %d: Undefined variable '%s'\n", subexpr->id_expr.linenum, subexpr->id_expr.var);
      subexpr->id_expr.ret_type=id_type;
      return id_type;
    } break;
    case INDEX_NODE: {
#if 0
      if (isResult(subexpr->index_expr.var)) {
        fprintf(errorFile,"LINE %d: The predefined variable '%s' is write-only.\n", subexpr->index_expr.linenum, subexpr->index_expr.var);
	return -1;
      }
#endif
      int array_type = get_type(subexpr->index_expr.var);
      if (array_type == -1)
        fprintf(errorFile,"LINE %d: Undefined array variable '%s'\n", subexpr->index_expr.linenum, subexpr->index_expr.var);
      int require_index=0;
      if(array_type==VEC_T2||array_type==IVEC_T2||array_type==BVEC_T2)require_index=2;
      else if(array_type==VEC_T3||array_type==IVEC_T3||array_type==BVEC_T3)require_index=3;
      else if(array_type==VEC_T4||array_type==IVEC_T4||array_type==BVEC_T4)require_index=4;
      if((require_index<=subexpr->index_expr.index||subexpr->index_expr.index<0)){
        fprintf(errorFile,"LINE %d: Array indexing is out of boundary.\n", subexpr->index_expr.linenum);
	return 0;
      }

      if(array_type==VEC_T2||array_type==VEC_T3||array_type==VEC_T4)array_type=FLOAT_T;
      else if(array_type==IVEC_T2||array_type==IVEC_T3||array_type==IVEC_T4)array_type=INT_T;
      else if(array_type==BVEC_T2||array_type==BVEC_T3||array_type==BVEC_T4)array_type=BOOL_T;
      subexpr->index_expr.ret_type=(array_type);
      return array_type; 
    } break;
    case VAR_NODE: {
      return check_expr_type( subexpr->var_expr.next);
    } break;
    case FUNCTION_NODE: {
      //return get_type(subexpr->id_expr.var);
    int func=  subexpr->func_expr.func_name;
    int argu_num= subexpr->func_expr.func_argu->argu_opt._argus_index;
    node * argu_opt=subexpr->func_expr.func_argu;
    int ret_val=0;
    if(func==0){
    //dp3
    if(argu_num!=2){
    fprintf(errorFile,"LINE %d: Function dp3 must have two arguments.\n",subexpr->func_expr.linenum);
      return 0;
    }
    int argu_type[2];
    argu_type[0]=check_expr_type( argu_opt->argu_opt._argus[0]);
    argu_type[1]=check_expr_type( argu_opt->argu_opt._argus[1]);
    if(argu_type[0]!=argu_type[1]){
    fprintf(errorFile,"LINE %d: Function dp3's arguments are not the same type.\n",subexpr->func_expr.linenum);
      ret_val = 0;
    }
    else if(argu_type[0]!=VEC_T3&&argu_type[0]!=VEC_T4&&argu_type[0]!=IVEC_T3&&argu_type[0]!=IVEC_T4){
    fprintf(errorFile,"LINE %d: Function dp3's arguments are not supported, only support VEC(2|3|4) or IVEC(2|3|4).\n",subexpr->func_expr.linenum);
      ret_val = 0;  
    }
    else{
    //success
    if(argu_type[0]==VEC_T3||argu_type[0]==VEC_T4)ret_val=FLOAT_T;
    if(argu_type[0]==IVEC_T3||argu_type[0]==IVEC_T4)ret_val=INT_T;
    }

    }//dp3

    else if(func==2){
    //rsq
    if(argu_num!=1){
    fprintf(errorFile,"LINE %d: Function rsq must have one argument.\n",subexpr->func_expr.linenum);
      return 0;
    }
    int argu_type=check_expr_type( argu_opt->argu_opt._argus[0]);
    if(argu_type!=FLOAT_T&&argu_type!=INT_T){
    fprintf(errorFile,"LINE %d: Function rsq's argument is not int nor float.\n",subexpr->func_expr.linenum);
      return 0;
    }
    if(argu_type==FLOAT_T)ret_val=FLOAT_T;
    else ret_val=INT_T;
    }

    else if(func==1){
    //lit
    if(argu_num!=1){
    fprintf(errorFile,"LINE %d: Function lit must have one argument.\n",subexpr->func_expr.linenum);
      return 0;
    }
    int argu_type=check_expr_type( argu_opt->argu_opt._argus[0]);
    if(argu_type!=VEC_T4){
    fprintf(errorFile,"LINE %d: Function lit's argument is not vec4.\n",subexpr->func_expr.linenum);
      return 0;
    }
    ret_val=VEC_T4;
    }
    subexpr->func_expr.ret_type=(ret_val);
    return ret_val;
    } break;
    case CONSTRUCTOR_NODE: {
      //return get_type(subexpr->id_expr.var);
      int type=subexpr->cnst_expr.type->type._type;
#if 0
      if(type!=IVEC_T2&&type!=IVEC_T3&&type!=IVEC_T4&&type!=VEC_T2&&type!=VEC_T3&&type!=VEC_T4&&type!=BVEC_T2&&type!=BVEC_T3&&type!=BVEC_T4){
      printf("LINE %d: Vector Constructor only support vector type.\n",subexpr->cnst_expr.linenum);
      return 0;
      }
#endif
      int vec_index=0;
      if(type==IVEC_T2||type==VEC_T2||type==BVEC_T2)  vec_index=2;
      else if(type==IVEC_T3||type==VEC_T3||type==BVEC_T3)  vec_index=3;
      else if(type==IVEC_T4||type==VEC_T4||type==BVEC_T4)  vec_index=4;
      else vec_index=1;
      if(vec_index!=subexpr->cnst_expr.argu->argu_opt._argus_index && (vec_index==2||vec_index==3||vec_index==4) ){
        fprintf(errorFile,"LINE %d: Constructors for vector types must have as many arguments as there are elements in the vector.\n",subexpr->cnst_expr.linenum);
        return 0;
      }
      if(vec_index!=subexpr->cnst_expr.argu->argu_opt._argus_index && (vec_index==1) ){
        fprintf(errorFile,"LINE %d: Constructors for basic types (bool, int, float) must have one argument that exactly matches that type.\n",subexpr->cnst_expr.linenum);
        return 0;
      }

      int i,ret_val,require_type;
      node * tmp=subexpr->cnst_expr.argu;
      if(type==IVEC_T2||type==IVEC_T3||type==IVEC_T4)   require_type=INT_T;
      else if(type==VEC_T2||type==VEC_T3||type==VEC_T4)      require_type=FLOAT_T;
      else if(type==BVEC_T2||type==BVEC_T3||type==BVEC_T4)   require_type=BOOL_T;
      else require_type=type;
      for(i=0;i<tmp->argu_opt._argus_index;i++){
         ret_val=check_expr_type( tmp->argu_opt._argus[i]);
	 if(ret_val!=require_type){
	 fprintf(errorFile,"LINE %d: The type of the #%d argument doesn't match the return type of the constructor.\n",subexpr->cnst_expr.linenum,i+1);
	 }
      }
      subexpr->cnst_expr.ret_type=(type);
      return type;
    } break;
    case UNARY_EXPRESION_NODE: {
      int type = check_expr_type(subexpr->unary_expr.right);
      if (subexpr->unary_expr.op == '!' && !BOOL_S_V_TYPE(type)) {//logical op
         fprintf(errorFile,"LINE %d: Logical op '!' expects right operand to be of scalar or vector boolean type.\n", subexpr->unary_expr.linenum);
         return -1; //-1 denotes any type
      }
      else if (subexpr->unary_expr.op == '-' && !ARITH_S_V_TYPE(type)) {// '-'
         fprintf(errorFile,"LINE %d: Arithmetic op '-' expects right operand to be of scalar or vector arithmetic type.\n", subexpr->unary_expr.linenum);
         return -1; //-1 denotes any type
      }
      subexpr->unary_expr.ret_type=(type);
      return type;
    } break;
    case BINARY_EXPRESSION_NODE: {
      int left_type = check_expr_type( subexpr->binary_expr.left);
      int right_type = check_expr_type( subexpr->binary_expr.right);
      switch (subexpr->binary_expr.op) {
        case AND:
        case OR: {
	  if (left_type == BOOL_T && right_type == BOOL_T){
	    subexpr->binary_expr.ret_type=(BOOL_T);
            return BOOL_T;
		}
	  if (left_type == BVEC_T2 && right_type == BVEC_T2){
	    subexpr->binary_expr.ret_type=(BVEC_T2);
            return BVEC_T2;}
	  if (left_type == BVEC_T3 && right_type == BVEC_T3){
	    subexpr->binary_expr.ret_type=(BVEC_T3);
            return BVEC_T3;}
	  if (left_type == BVEC_T4 && right_type == BVEC_T4){
	    subexpr->binary_expr.ret_type=(BVEC_T3);
            return BVEC_T4;}
	  fprintf(errorFile,"LINE %d: Logical op '%s' expects both operands to be of scalar or vector boolean type with the same base type.\n",subexpr->binary_expr.linenum,reverse_opnum_to_op(subexpr->binary_expr.op));
	  return -1;
	} break;

        case EQ:
        case NEQ: {
	  if (left_type == INT_T && right_type == INT_T){
	    subexpr->binary_expr.ret_type=(BOOL_T);
            return BOOL_T;}
	  if (left_type == IVEC_T2 && right_type == IVEC_T2){
	    subexpr->binary_expr.ret_type=(BOOL_T);
            return BOOL_T;}
	  if (left_type == IVEC_T3 && right_type == IVEC_T3){
	    subexpr->binary_expr.ret_type=(BOOL_T);
            return BOOL_T;}
	  if (left_type == IVEC_T4 && right_type == IVEC_T4){
	    subexpr->binary_expr.ret_type=(BOOL_T);
            return BOOL_T;}
	  if (left_type == FLOAT_T && right_type == FLOAT_T){
	    subexpr->binary_expr.ret_type=(BOOL_T);
            return BOOL_T;}
	  if (left_type == VEC_T2 && right_type == VEC_T2){
	    subexpr->binary_expr.ret_type=(BOOL_T);
            return BOOL_T;}
	  if (left_type == VEC_T3 && right_type == VEC_T3){
	    subexpr->binary_expr.ret_type=(BOOL_T);
            return BOOL_T;}
	  if (left_type == VEC_T4 && right_type == VEC_T4){
	    subexpr->binary_expr.ret_type=(BOOL_T);
            return BOOL_T;}
	  fprintf(errorFile,"LINE %d: Comparison op '%s' expects both operands to be of scalar or vector arithmetic type with the same base type.\n",subexpr->binary_expr.linenum,reverse_opnum_to_op(subexpr->binary_expr.op));
	  return -1;
	} break;

        case '<':
        case LEQ:
        case '>':
        case GEQ: {
	  if (left_type == INT_T && right_type == INT_T){
	    subexpr->binary_expr.ret_type=(BOOL_T);
            return BOOL_T;}
	  if (left_type == FLOAT_T && right_type == FLOAT_T){
	    subexpr->binary_expr.ret_type=(BOOL_T);
            return BOOL_T;}
	  fprintf(errorFile,"LINE %d: Comparison op '%s' expects both operands to be of scalar arithmetic type with the same base type.\n",subexpr->binary_expr.linenum,reverse_opnum_to_op(subexpr->binary_expr.op));
	  return -1;
	} break;

        case '+':
        case '-': {
	  if (left_type == INT_T && right_type == INT_T){
	    subexpr->binary_expr.ret_type=(INT_T);
            return INT_T;}
	  if (left_type == IVEC_T2 && right_type == IVEC_T2){
	    subexpr->binary_expr.ret_type=(IVEC_T2);
            return IVEC_T2;}
	  if (left_type == IVEC_T3 && right_type == IVEC_T3){
	    subexpr->binary_expr.ret_type=(IVEC_T3);
            return IVEC_T3;}
	  if (left_type == IVEC_T4 && right_type == IVEC_T4){
	    subexpr->binary_expr.ret_type=(IVEC_T4);
            return IVEC_T4;}
	  if (left_type == FLOAT_T && right_type == FLOAT_T){
	    subexpr->binary_expr.ret_type=(FLOAT_T);
            return FLOAT_T;}
	  if (left_type == VEC_T2 && right_type == VEC_T2){
	    subexpr->binary_expr.ret_type=(VEC_T2); 
            return VEC_T2;}
	  if (left_type == VEC_T3 && right_type == VEC_T3){
	    subexpr->binary_expr.ret_type=(VEC_T3);
            return VEC_T3;}
	  if (left_type == VEC_T4 && right_type == VEC_T4){
	    subexpr->binary_expr.ret_type=(VEC_T4);
            return VEC_T4;}
	  fprintf(errorFile,"LINE %d: Arithmetic op '%s' expects both operands to be of scalar or vector arithmetic type with the same base type.\n",subexpr->binary_expr.linenum,reverse_opnum_to_op(subexpr->binary_expr.op));
	  return -1;
	} break;

        case '*': {
	  if (left_type == INT_T && right_type == INT_T){
	    subexpr->binary_expr.ret_type=(INT_T);
            return INT_T;}
	  if (left_type == INT_T && right_type == IVEC_T2){
	    subexpr->binary_expr.ret_type=(IVEC_T2);
            return IVEC_T2;}
	  if (left_type == INT_T && right_type == IVEC_T3){
	    subexpr->binary_expr.ret_type=(IVEC_T3);
            return IVEC_T3;}
	  if (left_type == INT_T && right_type == IVEC_T4){
	    subexpr->binary_expr.ret_type=(IVEC_T4);
            return IVEC_T4;}

	  if (left_type == IVEC_T2 && right_type == IVEC_T2) {
	    subexpr->binary_expr.ret_type=(IVEC_T2);
            return IVEC_T2;}
	  if (left_type == IVEC_T3 && right_type == IVEC_T3) {
	    subexpr->binary_expr.ret_type=(IVEC_T3);
            return IVEC_T3;}
	  if (left_type == IVEC_T4 && right_type == IVEC_T4) {
	    subexpr->binary_expr.ret_type=(IVEC_T4);
            return IVEC_T4;}
	  if (left_type == IVEC_T2 && right_type == INT_T) {
	    subexpr->binary_expr.ret_type=(IVEC_T2);
            return IVEC_T2;}
	  if (left_type == IVEC_T3 && right_type == INT_T){
	    subexpr->binary_expr.ret_type=(IVEC_T3);
            return IVEC_T3;}
	  if (left_type == IVEC_T4 && right_type == INT_T){
	    subexpr->binary_expr.ret_type=(IVEC_T4);
            return IVEC_T4;}

	  if (left_type == FLOAT_T && right_type == FLOAT_T){
	    subexpr->binary_expr.ret_type=(FLOAT_T);
            return FLOAT_T;}
	  if (left_type == FLOAT_T && right_type == VEC_T2){
	    subexpr->binary_expr.ret_type=(VEC_T2);
            return VEC_T2;}
	  if (left_type == FLOAT_T && right_type == VEC_T3){
	    subexpr->binary_expr.ret_type=(VEC_T3);
            return VEC_T3;}
	  if (left_type == FLOAT_T && right_type == VEC_T4){
	    subexpr->binary_expr.ret_type=(IVEC_T4);
            return VEC_T4;}

	  if (left_type == VEC_T2 && right_type == VEC_T2) {
	    subexpr->binary_expr.ret_type=(VEC_T2);
            return VEC_T2;}
	  if (left_type == VEC_T3 && right_type == VEC_T3) {
	    subexpr->binary_expr.ret_type=(VEC_T3);
            return VEC_T3;}
	  if (left_type == VEC_T4 && right_type == VEC_T4) {
	    subexpr->binary_expr.ret_type=(VEC_T4);
            return VEC_T4;}
	  if (left_type == VEC_T2 && right_type == FLOAT_T){
	    subexpr->binary_expr.ret_type=(VEC_T2);
            return VEC_T2;}
	  if (left_type == VEC_T3 && right_type == FLOAT_T){
	    subexpr->binary_expr.ret_type=(VEC_T3);
            return VEC_T3;}
	  if (left_type == VEC_T4 && right_type == FLOAT_T){
	    subexpr->binary_expr.ret_type=(VEC_T4);
            return VEC_T4;}
	  fprintf(errorFile,"LINE %d: Arithmetic op '%s' expects both operands to have exactly the same base arithmetic type and same order.\n",subexpr->binary_expr.linenum,reverse_opnum_to_op(subexpr->binary_expr.op));
	  return -1;
	} break;

        case '/':
        case '^': {
	  if (left_type == INT_T && right_type == INT_T){
	    subexpr->binary_expr.ret_type=(INT_T);
            return INT_T;}
	  if (left_type == FLOAT_T && right_type == FLOAT_T){
	    subexpr->binary_expr.ret_type=(FLOAT_T);
            return FLOAT_T;}
	  fprintf(errorFile,"LINE %d: Arithmetic op '%s' expects both operands to be of scalar arithmetic type with the same base type.\n",subexpr->binary_expr.linenum,reverse_opnum_to_op(subexpr->binary_expr.op));
	  return -1;
	} break;
        default: break;
      }

    } break;
    default: break;
  }
  return -1;
}

typedef struct var_variation {
  symtbl_entry *_sym_entry;
  int _type;
}var_variation;

var_variation check_var_def(node *var_node){
  symtbl_entry *sym_entry;
  var_variation var_info = {NULL, -1};
  switch(var_node->kind) {
    case IDENT_NODE: {
      if ((sym_entry = get_symbol_entry(var_node->id_expr.var)) == NULL) {
        fprintf(errorFile,"LINE %d: Undefined variable '%s'\n", var_node->id_expr.linenum, var_node->id_expr.var);
	return var_info;
      }
      var_info._sym_entry=sym_entry;
      var_info._type=sym_entry->type;
      return var_info;
    } break;

    case INDEX_NODE: {
      if ((sym_entry = get_symbol_entry(var_node->index_expr.var)) == NULL) {
        fprintf(errorFile,"LINE %d: Undefined array variable '%s'\n", var_node->index_expr.linenum, var_node->index_expr.var);
	return var_info;
      }
      int array_type = get_type(var_node->index_expr.var);
      int require_index=0;
      if(array_type==VEC_T2||array_type==IVEC_T2||array_type==BVEC_T2)require_index=2;
      else if(array_type==VEC_T3||array_type==IVEC_T3||array_type==BVEC_T3)require_index=3;
      else if(array_type==VEC_T4||array_type==IVEC_T4||array_type==BVEC_T4)require_index=4;
      if((require_index<=var_node->index_expr.index||var_node->index_expr.index<0)){
        fprintf(errorFile,"LINE %d: Array indexing is out of boundary.\n", var_node->index_expr.linenum);
	return var_info;
      }

      if(array_type==VEC_T2||array_type==VEC_T3||array_type==VEC_T4)array_type=FLOAT_T;
      else if(array_type==IVEC_T2||array_type==IVEC_T3||array_type==IVEC_T4)array_type=INT_T;
      else if(array_type==BVEC_T2||array_type==BVEC_T3||array_type==BVEC_T4)array_type=BOOL_T;
      var_node->index_expr.ret_type=(array_type);
      var_info._sym_entry=sym_entry;
      var_info._type=array_type;
      return var_info;
    } break;
    default: return var_info; break;
  }
}

int check_predef_result_in_if_statement(node *stmt){
  if (stmt!=NULL && stmt->kind == ASSIGNMENT_NODE) {
    if (stmt->assign_statement._var->kind == IDENT_NODE) {
      if(isResult(stmt->assign_statement._var->id_expr.var)) {
       fprintf(errorFile,"LINE %d: The predefined variable '%s' is of <Result> qualifier and cannot be assigned anywhere in the scope of an if or else statement.\n", stmt->assign_statement._var->id_expr.linenum, stmt->assign_statement._var->id_expr.var);
       return 0;
      }
    }
    else {
      if(isResult(stmt->assign_statement._var->index_expr.var)) {
        fprintf(errorFile,"LINE %d: The predefined variable '%s' is of <Result> qualifier and cannot be assigned anywhere in the scope of an if or else statement.\n", stmt->assign_statement._var->index_expr.linenum, stmt->assign_statement._var->index_expr.var);
	return 0;
      }
    }
  }
  return 1;
}
int semantic_check(node *ast) {
  if (ast == NULL)
    return 0;  //fail, nothing to check
  switch(ast->kind){
    case SCOPE_NODE: {
      scope_num++;
      //first pass: insert symbol table into SCOPE_NODE
      //if it is the program scope, insert predefined var
      //into symbol table as well.
      scope_symtbl_ctor(ast, scope_num);
      int i, ret_val=1;
      for (i=0; i<GET_DECL_NUM(ast); i++)
        ret_val *= semantic_check(GET_DECL_VAR(ast,i));
      for (i=0; i<GET_STMT_NUM(ast); i++)
	ret_val *= semantic_check(GET_STMT(ast,i));
      //destroy symbol table when exiting 
      scope_symtbl_dtor(ast);
      return ret_val;
    } break;

    case DECLARATION_NODE: {
      int ret_val = 1; 
      int expr_type; 

#if 1
      //first check if the variable name matches one of the predefined variables.
      if (is_predef_var(ast->decl._id)!= -1) {//redeclaring predefined variable
        fprintf(errorFile,"LINE %d: Name collision - Redeclaring predefined variable '%s' is forbidden.\n", ast->decl.linenum, ast->decl._id);
	return 0;
      }
#endif

      symtbl_entry *first_def = NULL;
      //check if the variable is already defined in the current scope
      if ((first_def = get_first_def_of_dup(ast->decl._id)) != NULL && ast->decl.linenum != first_def->linenum) {
        fprintf(errorFile,"LINE %d: Multiple definitions - Variable '%s' is already declared on LINE %d.\n", ast->decl.linenum, ast->decl._id, first_def->linenum);
       return 0;
      }

      if (ast->decl._expr != NULL) {
	#if 0
        //check const variable
	if (ast->decl.cnst) {
	  if (ast->decl._expr->kind!=BOOL_NODE && 
	     ast->decl._expr->kind!=INT_NODE &&
	     ast->decl._expr->kind!=FLOAT_NODE) {
	    if (ast->decl._expr->kind!=VAR_NODE || ast->decl._expr->var_expr.next->kind!=IDENT_NODE || !isUniform(ast->decl._expr->var_expr.next->id_expr.var)) {
	    fprintf(errorFile,"LINE %d: Const variables must be initialized with a literal value or with a uniform variable, and not an expression.\n", ast->decl.linenum);
	    return 0;
	    }
	  }
	}
	#endif
        expr_type = check_expr_type(ast->decl._expr); //check expr
        if (expr_type != ast->decl._type->type._type) {
          fprintf(errorFile,"LINE %d: The RHS of the declaration is of type <%s>, which doesn't match the declared variable type <%s>.\n", ast->decl.linenum, reverse_typeid_to_type(expr_type), reverse_typeid_to_type(ast->decl._type->type._type));
          ret_val = 0; 
        }
        return ret_val;
      }
      return 1; // success
    } break;

    case IF_STATEMENT_NODE: {
      int cond_type_match_bool = 0; 
      int predef_result_check = 1;
      int if_stmt_check = 1;
      int else_stmt_check = 1;
      int expr_type = check_expr_type(ast->if_statement._expr);
      if (expr_type == BOOL_T)
        cond_type_match_bool = 1;
      else{
	fprintf(errorFile,"The if cond expr is of type <%s>, which is not a BOOL_T.\n",reverse_typeid_to_type(expr_type));
      }
#if 1
      if (ast->if_statement._if_statement!=NULL && ast->if_statement._if_statement->kind == ASSIGNMENT_NODE) {
        predef_result_check = check_predef_result_in_if_statement(ast->if_statement._if_statement);
      }
      else if (ast->if_statement._if_statement!=NULL && ast->if_statement._if_statement->kind == NESTED_SCOPE_NODE) {
	node *next_scope = ast->if_statement._if_statement->nested_scope_statement._scope;
        if (next_scope!=NULL) {
          int i;
          for (i=0; i<GET_STMT_NUM(next_scope); i++)
	  {
       	  predef_result_check = check_predef_result_in_if_statement(GET_STMT(next_scope,i));
	  }
	}
      }
#endif 
      if_stmt_check = semantic_check(ast->if_statement._if_statement);
      if (ast->if_statement._else != 0) {
#if 1
      if (ast->if_statement._else_statement!=NULL && ast->if_statement._else_statement->kind == ASSIGNMENT_NODE) {
        predef_result_check = check_predef_result_in_if_statement(ast->if_statement._else_statement);
      }
      else if (ast->if_statement._else_statement!=NULL && ast->if_statement._else_statement->kind == NESTED_SCOPE_NODE) {
	node *next_scope = ast->if_statement._else_statement->nested_scope_statement._scope;
        if (next_scope!=NULL) {
          int i;
          for (i=0; i<GET_STMT_NUM(next_scope); i++)
	  {
       	  predef_result_check = check_predef_result_in_if_statement(GET_STMT(next_scope,i));
	  }
	}
      }
#endif 

        else_stmt_check = semantic_check(ast->if_statement._else_statement);
      }
      return (cond_type_match_bool == 1 && predef_result_check == 1 && if_stmt_check == 1 && else_stmt_check == 1)? 1 : 0;
    } break;

    case ASSIGNMENT_NODE: {
      int ret_val = 1; 
      int expr_type;
      var_variation var_info;

      var_info = check_var_def(ast->assign_statement._var);
      symtbl_entry *var_def = var_info._sym_entry;
      //check if the variable is defined
      if (var_def == NULL) {
        return 0; //var not defined.
      }
      //check if it's a predefined variable with uniform and attribute qualifier(read-only)
      if (var_def->qualifier==UNIFORM || var_def->qualifier==ATTRIBUTE) {
        fprintf(errorFile,"LINE %d: The predefined variable '%s' is read-only.\n", ast->assign_statement.linenum, var_def->var);
	return 0;
      }

      ast->assign_statement.ret_type=var_info._type;
      if (ast->assign_statement._expr != NULL) {
        //check const variable
	if (var_def->constness) {
	  fprintf(errorFile,"LINE %d: The value of const variable cannot be re-assigned.\n", ast->assign_statement.linenum);
          return 0;
	}
        else {
          expr_type = check_expr_type(ast->assign_statement._expr); //check expr
          if (var_def!= NULL && expr_type != var_info._type) {
	    fprintf(errorFile,"LINE %d: The RHS of the assignment is of type <%s>, which doesn't match the type <%s> of LHS.\n", ast->assign_statement.linenum, reverse_typeid_to_type(expr_type), reverse_typeid_to_type(var_info._type));
	    ret_val = 0; 
	  }
	  return ret_val;
	}
      }
      return 1; // success
      
    } break;


    case NESTED_SCOPE_NODE: {
      return semantic_check(ast->nested_scope_statement._scope);
    } break;

    default: break;

  }

  return 0; // failed checks
}
