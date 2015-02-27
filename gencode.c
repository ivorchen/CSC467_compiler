#include "gencode.h"
#include "parser.tab.h"

//static int condtmp_index=0;
//static int exprtmp_index=0;

static register_info register_pool[MAX_EXPR_REG_NUM];

void print_reg_pool(){
  int i;
  for (i=0; i<MAX_EXPR_REG_NUM; i++){
    if (register_pool[i].num != -1) {
      printf("reg%d status:%d\n", register_pool[i].num, register_pool[i].status);
    }
  }
}
void reg_info_init(){
  int i;
  for (i=0; i<MAX_EXPR_REG_NUM; i++){
    register_pool[i].num = -1;
    register_pool[i].status = -1;
  }
}

int reg_alloc(){
  int i;
  for (i=0; i<MAX_EXPR_REG_NUM; i++){
    if (register_pool[i].num == -1) {
      register_pool[i].num = i;
      register_pool[i].status = 1;
      fprintf(genFile,"TEMP ExprTemp%d;\n",i);
      return i;
    }
  }
  return -1; // reg_pool is full.
}

void reg_free(){
  int i;
  for (i=0; i<MAX_EXPR_REG_NUM; i++){
    if (register_pool[i].status == 2)
      register_pool[i].status = 0;
  }
}

int reg_assign(){
  int i;
  for (i=0; i<MAX_EXPR_REG_NUM; i++){
    if (register_pool[i].num != -1 && register_pool[i].status == 0) {
      register_pool[i].status = 1;
      return i;
    }
  }
  return reg_alloc(); // reg_pool is full.
}

void reg_mark_to_be_freed(int i){
  register_pool[i].status = 2;
}

void gen_id_node (node *ast)
{ 
  if(!strcmp(ast->id_expr.var,"gl_FragColor"))strcpy(ast->arb_code,"result.color");                                  
  else if (!strcmp(ast->id_expr.var,"gl_FragDepth"))strcpy(ast->arb_code,"result.depth");                            
  else if (!strcmp(ast->id_expr.var,"gl_FragCoord"))strcpy(ast->arb_code,"fragment.position");                       
  else if (!strcmp(ast->id_expr.var,"gl_TexCoord"))strcpy(ast->arb_code,"fragment.texcoord");                        
  else if (!strcmp(ast->id_expr.var,"gl_Color"))strcpy(ast->arb_code,"fragment.color");                              
  else if (!strcmp(ast->id_expr.var,"gl_Secondary"))strcpy(ast->arb_code,"fragment.color.secondary");                
  else if (!strcmp(ast->id_expr.var,"gl_FogFragCoord"))strcpy(ast->arb_code,"fragment.fogcoord");                    
  else if (!strcmp(ast->id_expr.var,"gl_Light_Half"))strcpy(ast->arb_code,"state.light[0].half");                    
  else if (!strcmp(ast->id_expr.var,"gl_Light_Ambient"))strcpy(ast->arb_code,"state.lightmodel.ambient");            
  else if (!strcmp(ast->id_expr.var,"gl_Material_Shininess"))strcpy(ast->arb_code,"state.material.shininess");       
  else if (!strcmp(ast->id_expr.var,"env1"))strcpy(ast->arb_code,"program.env[1]");                                   
  else if (!strcmp(ast->id_expr.var,"env2"))strcpy(ast->arb_code,"program.env[2]");                                   
  else if (!strcmp(ast->id_expr.var,"env3"))strcpy(ast->arb_code,"program.env[3]");                                   
  else                                    
    strcpy(ast->arb_code, ast->id_expr.var);                                   
}

void gen_index_node (node *ast)
{ 
  if(!strcmp(ast->index_expr.var,"gl_FragColor"))strcpy(ast->arb_code,"result.color");                                  
  else if (!strcmp(ast->index_expr.var,"gl_FragDepth"))strcpy(ast->arb_code,"result.depth");                            
  else if (!strcmp(ast->index_expr.var,"gl_FragCoord"))strcpy(ast->arb_code,"fragment.position");                       
  else if (!strcmp(ast->index_expr.var,"gl_TexCoord"))strcpy(ast->arb_code,"fragment.texcoord");                        
  else if (!strcmp(ast->index_expr.var,"gl_Color"))strcpy(ast->arb_code,"fragment.color");                              
  else if (!strcmp(ast->index_expr.var,"gl_Secondary"))strcpy(ast->arb_code,"fragment.color.secondary");                
  else if (!strcmp(ast->index_expr.var,"gl_FogFragCoord"))strcpy(ast->arb_code,"fragment.fogcoord");                    
  else if (!strcmp(ast->index_expr.var,"gl_Light_Half"))strcpy(ast->arb_code,"state.light[0].half");                    
  else if (!strcmp(ast->index_expr.var,"gl_Light_Ambient"))strcpy(ast->arb_code,"state.lightmodel.ambient");            
  else if (!strcmp(ast->index_expr.var,"gl_Material_Shininess"))strcpy(ast->arb_code,"state.material.shininess");       
  else if (!strcmp(ast->index_expr.var,"env1"))strcpy(ast->arb_code,"program.env[1]");                                   
  else if (!strcmp(ast->index_expr.var,"env2"))strcpy(ast->arb_code,"program.env[2]");                                   
  else if (!strcmp(ast->index_expr.var,"env3"))strcpy(ast->arb_code,"program.env[3]");                                   
  else                                    
    strcpy(ast->arb_code, ast->index_expr.var);                                   
}

void GenCode(node * ast) {
#if 1
  if (ast == NULL)
    return;

  switch(ast->kind) {
    case SCOPE_NODE: {
        //fprintf(genFile,"TEMP ExprTemp%d;\n",exprtmp_index);
	//exprtmp_index++;
          
      if(ast->scope._decls!=NULL){
        if (ast->scope.if_cond_reg_num != -1)
	  ast->scope._decls->decls.if_cond_reg_num = ast->scope.if_cond_reg_num;
	GenCode(ast->scope._decls);
      }
	
      if(ast->scope._stmts!=NULL){
        if (ast->scope.if_cond_reg_num != -1)
          ast->scope._stmts->stmts.if_cond_reg_num = ast->scope.if_cond_reg_num;
        GenCode(ast->scope._stmts);
      }
    } break;

    case DECLS_NODE: {
      if(ast->decls._decls_index!=0){
        int i=0;
	int tmp=ast->decls._decls_index;
	for(;i<tmp+1;i++){
          if (ast->decls._decls[i]!= NULL && ast->decls.if_cond_reg_num != -1) {
	    ast->decls._decls[i]->decl.if_cond_reg_num = ast->decls.if_cond_reg_num;
	  }
	  GenCode(ast->decls._decls[i]);
	}
      }	
    } break;

    case STMTS_NODE: {
      if(ast->stmts._stmts_index!=0){
	int i=0;
	int tmp=ast->stmts._stmts_index;
	for(;i<tmp+1;i++){
          if (ast->stmts.if_cond_reg_num != -1) {
	    node *tmp = ast->stmts._stmts[i];
	    if (tmp != NULL) {
	      switch (tmp->kind) {
                case IF_STATEMENT_NODE:
	          tmp->if_statement.if_cond_reg_num = ast->stmts.if_cond_reg_num;
	  	break;
                case ASSIGNMENT_NODE:
	          tmp->assign_statement.if_cond_reg_num = ast->stmts.if_cond_reg_num;
		break;
                case NESTED_SCOPE_NODE:
	          tmp->nested_scope_statement.if_cond_reg_num = ast->stmts.if_cond_reg_num;
		break;
	        default: break;
	      }
	    }
	  }
   	  GenCode(ast->stmts._stmts[i]);
	}
      }	
    } break;

    case DECLARATION_NODE: {
      if(ast->decl._type!=NULL){
	GenCode(ast->decl._type);
        if(ast->decl._expr==NULL||ast->decl._expr->kind!=CONSTRUCTOR_NODE)
          fprintf(genFile,"TEMP %s;\n",ast->decl._id);
      }
      if(ast->decl._expr!=NULL){
	GenCode(ast->decl._expr);
        if (ast->decl.if_cond_reg_num == -1) { //not within if/else statement
	  if(ast->decl._expr->kind!=CONSTRUCTOR_NODE&&ast->decl._expr->kind!=FUNCTION_NODE){
	  fprintf(genFile,"MOV %s, %s;\n",
			  ast->decl._id, 
			  ast->decl._expr->arb_code);
	  }
	  else if (ast->decl._expr->kind==FUNCTION_NODE){
	    char * temp_string=ast->decl._expr->arb_code;
	    char func[30];
	    char argu[30];
	    sscanf(temp_string,"%s :%s",func,argu);
	    if(func!=NULL&&!strcmp(func,"RSQ")){
	      int reg =  reg_assign();
	      fprintf(genFile,"MOV ExprTemp%d, %s;\n",reg,argu);
	      fprintf(genFile,"%s %s, ExprTemp%d.x;\n",
		       func,
		       ast->decl._id,
		       reg);
              reg_mark_to_be_freed(reg);
	    }
	    else
	      fprintf(genFile,"%s %s, %s;\n",
		       func,
		       ast->decl._id,
		       argu);
	  }
	  else
	    fprintf(genFile,"PARAM %s = %s;\n",
	            ast->decl._id, 
		    ast->decl._expr->arb_code);
	}
        else { //within if/else statement
	  if(ast->decl._expr->kind!=CONSTRUCTOR_NODE&&ast->decl._expr->kind!=FUNCTION_NODE){
            fprintf(genFile,"CMP %s, ExprTemp%d, %s, %s;\n",
			    ast->decl._id, 
		            ast->decl.if_cond_reg_num,
			    ast->decl._id, 
			    ast->decl._expr->arb_code);
	  }
	  else if (ast->decl._expr->kind==FUNCTION_NODE){
	    char * temp_string=ast->decl._expr->arb_code;
	    char func[30];
	    char argu[30];
	    sscanf(temp_string,"%s :%s",func,argu);
	    //rsq
	    if(func!=NULL&&!strcmp(func,"RSQ")){
	      int reg =  reg_assign();
	      fprintf(genFile,"MOV ExprTemp%d, %s;\n",reg,argu);
	      fprintf(genFile,"%s ExprTemp%d, ExprTemp%d.x;\n",
		       func,
		       reg,
		       reg);
              fprintf(genFile,"CMP %s, ExprTemp%d, %s, ExprTemp%d;\n",
			      ast->decl._id, 
		              ast->decl.if_cond_reg_num,
			      ast->decl._id, 
			      reg);
              reg_mark_to_be_freed(reg);
	    }
	    //dp3,lit
	    else {
	      int reg =  reg_assign();
	      fprintf(genFile,"%s ExprTemp%d, %s;\n",
		       func,
		       reg,
		       argu);
              fprintf(genFile,"CMP %s, ExprTemp%d, %s, ExprTemp%d;\n",
			      ast->decl._id, 
		              ast->decl.if_cond_reg_num,
			      ast->decl._id, 
			      reg);
	      reg_mark_to_be_freed(reg);
	    }
	  }
	  //constructor node
	  else {
	      //int reg = reg_assign();
	      fprintf(genFile,"TEMP %s;\n",ast->decl._id);
	      fprintf(genFile,"PARAM ExprTemp%s = %s;\n",
	            		ast->decl._id, 
		    		ast->decl._expr->arb_code);
  	      fprintf(genFile,"CMP %s, ExprTemp%d, %s, ExprTemp%s;\n",
	    			ast->decl._id,
				ast->decl.if_cond_reg_num,
				ast->decl._id,
				ast->decl._id);
	  }
	}
      }
      reg_free();
    } break;
   
    case IF_STATEMENT_NODE: {
#if 1
      int reg = reg_assign();  //if cond
      int reg1 = reg_assign(); //else cond
      if(ast->if_statement._expr!=NULL) {
        GenCode(ast->if_statement._expr);
        fprintf(genFile,"MOV ExprTemp%d, %s; #if condition\n",
		    reg,
		    ast->if_statement._expr->arb_code);
	if (ast->if_statement.if_cond_reg_num != -1) { //nested if cond
	  int reg_restore = reg_assign(); 
	  //restore the unnormalized cond value.
          fprintf(genFile,"ADD ExprTemp%d, ExprTemp%d, %f; #restore the unnormalized condition value.\n",
		      reg_restore,
		      ast->if_statement.if_cond_reg_num,
		      0.5);
          fprintf(genFile,"MUL ExprTemp%d, ExprTemp%d, ExprTemp%d; #nested if condition\n",
		      reg,
		      reg,
		      reg_restore);
          reg_mark_to_be_freed(reg_restore);
	}
        //inverse the if condition
        fprintf(genFile,"SLT ExprTemp%d, ExprTemp%d, %f; #inverse if condition for else clause\n",
		    reg1,
		    reg,
		    1.0);
        fprintf(genFile,"SUB ExprTemp%d, ExprTemp%d, %f; #normalized if condition for CMP instruction\n",
		    reg,
		    reg,
		    0.5);
        fprintf(genFile,"SUB ExprTemp%d, ExprTemp%d, %f; #normalized else condition for CMP instruction\n",
		    reg1,
		    reg1,
		    0.5);
        reg_free();
      }
      if(ast->if_statement._if_statement!=NULL) {
        node *tmp = ast->if_statement._if_statement;
        switch (tmp->kind) {
          case ASSIGNMENT_NODE:
	    tmp->assign_statement.if_cond_reg_num = reg;
	    break;
          case NESTED_SCOPE_NODE:
	    tmp->nested_scope_statement.if_cond_reg_num = reg;
	    break;
          default: break;
        }
        GenCode(ast->if_statement._if_statement);
        reg_free();
      }
    
    //if(ast->if_statement._else!=0)
    //printf("\n    ELSE ");
      if(ast->if_statement._else_statement!=NULL) {
        node *tmp = ast->if_statement._else_statement;
        switch (tmp->kind) {
          case ASSIGNMENT_NODE:
	    tmp->assign_statement.if_cond_reg_num = reg1;
	    break;
          case NESTED_SCOPE_NODE:
	    tmp->nested_scope_statement.if_cond_reg_num = reg1; 
	    break;
          default: break;
        }
        GenCode(ast->if_statement._else_statement);
        reg_free();
      }
#endif
      reg_mark_to_be_freed(reg);
      reg_mark_to_be_freed(reg1);
      reg_free();
    } break;
    
    case ASSIGNMENT_NODE: {
      if(ast->assign_statement._var!=NULL)
        GenCode(ast->assign_statement._var);
      if(ast->assign_statement._expr!=NULL) {
        GenCode(ast->assign_statement._expr);
        if (ast->assign_statement.if_cond_reg_num == -1) {
          if(ast->assign_statement._expr->kind==FUNCTION_NODE){
    	    char * temp_string=ast->assign_statement._expr->arb_code;
	    char func[30];
	    char argu[30];
	    sscanf(temp_string,"%s :%s",func,argu);
	    if(func!=NULL&&!strcmp(func,"RSQ")){
	      int reg =  reg_assign();
	      fprintf(genFile,"MOV ExprTemp%d, %s;\n",reg,argu);
	      fprintf(genFile,"%s %s, ExprTemp%d.x;\n",
		      func,
		      ast->assign_statement._var->arb_code,
		      reg);
	      reg_mark_to_be_freed(reg);
	    }
	    //dp3,lit
	    else{
	      fprintf(genFile,"%s %s, %s;\n",
		      func,
		      ast->assign_statement._var->arb_code,
		      argu);
		}
          }
	  else if(ast->assign_statement._expr->kind==CONSTRUCTOR_NODE){
	  int reg =  reg_assign();
	  fprintf(genFile,"PARAM ExprTemp%d = %s;\n",
	            		reg, 
		    		ast->assign_statement._expr->arb_code);
	  fprintf(genFile,"MOV %s, ExprTemp%d;\n",ast->assign_statement._var->arb_code,reg);
	  }
          else{
            fprintf(genFile,"MOV %s, %s;\n",
		      ast->assign_statement._var->arb_code,
		      ast->assign_statement._expr->arb_code);
	  }
	}
        else{//within if statement scope
		if(ast->assign_statement._expr->kind!=CONSTRUCTOR_NODE&&ast->assign_statement._expr->kind!=FUNCTION_NODE){
		fprintf(genFile,"CMP %s, ExprTemp%d, %s, %s;\n",
		    ast->assign_statement._var->arb_code,
		    ast->assign_statement.if_cond_reg_num,
		    ast->assign_statement._var->arb_code,
		    ast->assign_statement._expr->arb_code);
	
		}

        	else if(ast->assign_statement._expr->kind==FUNCTION_NODE){
    	    	char * temp_string=ast->assign_statement._expr->arb_code;
	    	char func[30];
	    	char argu[30];
	    	sscanf(temp_string,"%s :%s",func,argu);
		//rsq
	    	if(func!=NULL&&!strcmp(func,"RSQ")){
	      		int reg =  reg_assign();
	      		fprintf(genFile,"MOV ExprTemp%d, %s;\n",reg,argu);
	      		fprintf(genFile,"%s ExprTemp%d, ExprTemp%d.x;\n",
		      		func,
		      		reg,
		      		reg);
	      		fprintf(genFile,"CMP %s, ExprTemp%d, %s, ExprTemp%d;\n",
		      		ast->assign_statement._var->arb_code,
		      		ast->assign_statement.if_cond_reg_num,
		      		ast->assign_statement._var->arb_code,
		      		reg);
	      		reg_mark_to_be_freed(reg);
	    		}
	    	else{//dp3 lit
	      		int reg =  reg_assign();
	      		fprintf(genFile,"%s ExprTemp%d, %s;\n",
		       		func,
		       		reg,
		       		argu);
              		fprintf(genFile,"CMP %s, ExprTemp%d, %s, ExprTemp%d;\n",
			      	ast->assign_statement._var->arb_code,
		              	ast->assign_statement.if_cond_reg_num,
			      	ast->assign_statement._var->arb_code,
			      	reg);
	      		reg_mark_to_be_freed(reg);
			}
          	}
		else{
	 	//constructor node
		//int reg = reg_assign();
	      	fprintf(genFile,"PARAM ExprTemp%s = %s;\n",
	            		ast->assign_statement._var->arb_code, 
		    		ast->assign_statement._expr->arb_code);
  	      	fprintf(genFile,"CMP %s, ExprTemp%d, %s, ExprTemp%s;\n",
	    			ast->assign_statement._var->arb_code,
				ast->assign_statement.if_cond_reg_num,
				ast->assign_statement._var->arb_code,
				ast->assign_statement._var->arb_code);
		}
	}
      reg_free();
      }
    } break; 

    case NESTED_SCOPE_NODE: {
      if (ast->nested_scope_statement.if_cond_reg_num != -1)
        ast->nested_scope_statement._scope->scope.if_cond_reg_num = ast->nested_scope_statement.if_cond_reg_num;
      GenCode(ast->nested_scope_statement._scope);
    } break;

    case EXPRESSION_NODE: {
        GenCode(ast->expr.next);
        strcpy(ast->arb_code,ast->expr.next->arb_code);
    } break;

    case BOOL_NODE: {
      //TODO: Not all bool_node usage involves in if condition.
        int reg=reg_assign();
     	fprintf(genFile, "MOV ExprTemp%d, %f;\n", reg, (ast->bool_expr.val) ? 1.0 : 0.0);
	sprintf(ast->arb_code,"ExprTemp%d",reg);
	reg_mark_to_be_freed(reg);
    } break;

    case FUNCTION_NODE: {
#if 1
      if(ast->func_expr.func_argu!=NULL) {
        GenCode(ast->func_expr.func_argu);
      
      
        if(ast->func_expr.func_name==0){
	  sprintf(ast->arb_code,"%s :%s","DP3",ast->func_expr.func_argu->arb_code);
        }
        else if(ast->func_expr.func_name==1){
      	  sprintf(ast->arb_code,"%s :%s","LIT",ast->func_expr.func_argu->arb_code);
        }
        else if(ast->func_expr.func_name==2){
      	  sprintf(ast->arb_code,"%s :%s","RSQ",ast->func_expr.func_argu->arb_code);
        }
#endif
      }
    } break;
#if 1
    case CONSTRUCTOR_NODE: {
      if(ast->cnst_expr.argu!=NULL){
        GenCode(ast->cnst_expr.argu);
        sprintf(ast->arb_code,"{%s}",ast->cnst_expr.argu->arb_code);
      }
    } break;
#endif
    case ARGUMENTS_OPT_NODE: {
      if(ast->argu_opt._argus_index!=0){
	int i=0;
	int tmp=ast->argu_opt._argus_index;
	for(;i<tmp+1;i++){
          if(ast->argu_opt._argus[i]!=NULL){
	    GenCode(ast->argu_opt._argus[i]);
	    if(strlen(ast->arb_code)==0&&strlen(ast->argu_opt._argus[i]->arb_code)!=0)
	      sprintf(ast->arb_code,"%s",ast->argu_opt._argus[i]->arb_code);
    	    else if(strlen(ast->argu_opt._argus[i]->arb_code)!=0)
   	      sprintf(ast->arb_code,"%s,%s", ast->arb_code,ast->argu_opt._argus[i]->arb_code );
	  }
	}
      }	
    } break;

    case UNARY_EXPRESION_NODE: {
      if (ast->unary_expr.right != NULL) {
	 int reg = reg_assign();
	 GenCode(ast->unary_expr.right);
	 if(ast->unary_expr.op=='-'){
	  fprintf(genFile,"MOV ExprTemp%d, %f;\n", reg, -1.0);
	  fprintf(genFile,"MUL ExprTemp%d, %s, ExprTemp%d;\n",reg, ast->unary_expr.right->arb_code, reg);
 	 }
	 else{
	  fprintf(genFile,"SGE ExprTemp%d, %s, %f;\n", reg, ast->unary_expr.right->arb_code, 1.0);
	  fprintf(genFile,"SLT ExprTemp%d, ExprTemp%d, %f;\n", reg, reg, 1.0);
	 }
      sprintf(ast->arb_code,"ExprTemp%d",reg);
      reg_mark_to_be_freed(reg);
      }
      
    } break;

    case BINARY_EXPRESSION_NODE: { 
      if (ast->binary_expr.left != NULL && 
          ast->binary_expr.right != NULL) {
	int reg = reg_assign();
	GenCode(ast->binary_expr.left);
	GenCode(ast->binary_expr.right);
#if 1
        switch(ast->binary_expr.op){
          case AND: {
	  //int tep_reg = reg_assign();
	  fprintf(genFile, "MUL "); 
	  fprintf(genFile,"ExprTemp%d, %s, %s;\n",
			    reg, 
			    ast->binary_expr.left->arb_code, 
			    ast->binary_expr.right->arb_code);
	  } break;
          case OR: {
	  fprintf(genFile, "ADD "); 
	  fprintf(genFile,"ExprTemp%d, %s, %s;\n",
			    reg, 
			    ast->binary_expr.left->arb_code, 
			    ast->binary_expr.right->arb_code);
	  } break;
          case EQ:  {
	  int tmp_reg = reg_assign();
	  fprintf(genFile, "SLT "); 
	  fprintf(genFile,"ExprTemp%d, %s, %s;\n",
			    reg, 
			    ast->binary_expr.left->arb_code, 
			    ast->binary_expr.right->arb_code);
          fprintf(genFile,"SUB ExprTemp%d, %s, %s;\n",
			    tmp_reg, 
			    ast->binary_expr.right->arb_code, 
			    ast->binary_expr.left->arb_code);
	  fprintf(genFile,"SLT ExprTemp%d, ExprTemp%d, %f;\n",
			    tmp_reg, 
			    tmp_reg,
			    0.0);
          fprintf(genFile,"ADD ExprTemp%d, ExprTemp%d, ExprTemp%d;\n",
			    reg, 
			    reg, 
			    tmp_reg);
	  fprintf(genFile,"SLT ExprTemp%d, ExprTemp%d, %f;\n",
			    reg, 
			    reg, 
			    1.0);
	  reg_mark_to_be_freed(tmp_reg);
	  } break;
          case NEQ: {
	  int tmp_reg = reg_assign();
	  fprintf(genFile, "SLT "); 
	  fprintf(genFile,"ExprTemp%d, %s, %s;\n",
			    reg, 
			    ast->binary_expr.left->arb_code, 
			    ast->binary_expr.right->arb_code);
          fprintf(genFile,"SUB ExprTemp%d, %s, %s;\n",
			    tmp_reg, 
			    ast->binary_expr.right->arb_code, 
			    ast->binary_expr.left->arb_code);
	  fprintf(genFile,"SLT ExprTemp%d, ExprTemp%d, %f;\n",
			    tmp_reg, 
			    tmp_reg,
			    0.0);
          fprintf(genFile,"ADD ExprTemp%d, ExprTemp%d, ExprTemp%d;\n",
			    reg, 
			    reg, 
			    tmp_reg);
	  reg_mark_to_be_freed(tmp_reg);
	  } break;
          case '<': {
	  fprintf(genFile, "SLT "); 
	  fprintf(genFile,"ExprTemp%d, %s, %s;\n",
			    reg, 
			    ast->binary_expr.left->arb_code, 
			    ast->binary_expr.right->arb_code);
	  }  break;
          case LEQ: {
	  fprintf(genFile,"SUB ExprTemp%d, %s, %s;\n",
			    reg, 
			    ast->binary_expr.right->arb_code, 
			    ast->binary_expr.left->arb_code);
	  fprintf(genFile,"SGE ExprTemp%d, ExprTemp%d, %f;\n",
			    reg, 
			    reg,
			    0.0);		  
	  } break;
          case '>': {
	  fprintf(genFile,"SUB ExprTemp%d, %s, %s;\n",
			    reg, 
			    ast->binary_expr.right->arb_code, 
			    ast->binary_expr.left->arb_code);
	  fprintf(genFile,"SLT ExprTemp%d, ExprTemp%d, %f;\n",
			    reg, 
			    reg,
			    0.0);	  
	  } break;
          case GEQ: {
	  fprintf(genFile, "SGE "); 
	  fprintf(genFile,"ExprTemp%d, %s, %s;\n",
			    reg, 
			    ast->binary_expr.left->arb_code, 
			    ast->binary_expr.right->arb_code);
	  } break;
          case '+': {
	    fprintf(genFile, "ADD "); 
	    fprintf(genFile,"ExprTemp%d, %s, %s;\n",
			    reg, 
			    ast->binary_expr.left->arb_code, 
			    ast->binary_expr.right->arb_code);
	  } break;
          case '-': {
	    fprintf(genFile, "SUB "); 
	    fprintf(genFile,"ExprTemp%d, %s, %s;\n",
			    reg,
			    ast->binary_expr.left->arb_code,
			    ast->binary_expr.right->arb_code);
	  } break;
          case '*': {
	    fprintf(genFile, "MUL "); 
	    fprintf(genFile, "ExprTemp%d, %s, %s;\n",
			    reg,
			    ast->binary_expr.left->arb_code,
			    ast->binary_expr.right->arb_code);
	  } break;
          case '/': {
	    int tmp_reg = reg_assign();
	    fprintf(genFile, "MOV ExprTemp%d, %s;\n",tmp_reg,ast->binary_expr.right->arb_code);
	    fprintf(genFile, "RCP ExprTemp%d, ExprTemp%d.x;\n",
			    reg,
			    tmp_reg); 
	    fprintf(genFile, "MUL ExprTemp%d, %s, ExprTemp%d;\n", 
			    reg, 
			    ast->binary_expr.left->arb_code, 
			    reg);
	    reg_mark_to_be_freed(reg);
	  } break;
          case '^': {
	    int tmp_reg = reg_assign();
	    fprintf(genFile, "MOV ExprTemp%d, %s;\n",tmp_reg,ast->binary_expr.right->arb_code);
	    fprintf(genFile, "MOV ExprTemp%d, %s;\n",reg,ast->binary_expr.left->arb_code);
	    fprintf(genFile, "POW ");
	    fprintf(genFile, "ExprTemp%d, ExprTemp%d.x, ExprTemp%d.x;\n", 
			    reg,
			    reg,
			    tmp_reg);
	    reg_mark_to_be_freed(reg);
	  } break;
          default: break;
	}
#endif
	sprintf(ast->arb_code,"ExprTemp%d", reg);
        reg_mark_to_be_freed(reg);
      }
    } break;

    case INT_NODE: {
      sprintf(ast->arb_code,"%f ", (float)ast->int_expr.val);
    } break;

    case FLOAT_NODE: {
    //  printf("<%f> ", ast->float_expr.val);
      sprintf(ast->arb_code,"%f ", ast->float_expr.val);
    } break;

    case IDENT_NODE: {
    //  printf("<%s> ", ast->id_expr.var);
    /*
    	gl_FragColor ­> result.color
 	gl_FragDepth ­> result.depth
	gl_FragCoord ­> fragment.position
        gl_TexCoord ­> fragment.texcoord
	gl_Color ­> fragment.color
	gl_Secondary ­> fragment.color.secondary
	gl_FogFragCoord ­> fragment.fogcoord
	gl_Light_Half ­> state.light[0].half
	gl_Light_Ambient ­> state.lightmodel.ambient
	gl_Material_Shininess ­> state.material.shininess
	env1 ­> program.env[1]
	env2 ­> program.env[2]
	env3 ­> program.env[3]
    */
      gen_id_node(ast);
    } break;

    case INDEX_NODE: {
    //  printf("(INDEX %s ", reverse_typeid_to_type(ast->index_expr.ret_type ));
      gen_index_node(ast);
      if(ast->index_expr.index==0)
      sprintf(ast->arb_code,"%s.x ", ast->arb_code);
      else if (ast->index_expr.index==1)
      sprintf(ast->arb_code,"%s.y ", ast->arb_code);
      else if (ast->index_expr.index==2)
      sprintf(ast->arb_code,"%s.z ", ast->arb_code);
      else if (ast->index_expr.index==3)
      sprintf(ast->arb_code,"%s.w ", ast->arb_code);
    } break;

    case VAR_NODE: {
      if (ast->var_expr.next != NULL){
        GenCode(ast->var_expr.next);
	strcpy(ast->arb_code,ast->var_expr.next->arb_code);
	}
    } break;
    
    case TYPE_NODE: {
#if 0  
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
#endif
	//fprintf(genFile,"TEMP ");
	  }
    
    default: break;
  }
#endif
}
