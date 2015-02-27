/***********************************************************************
 * **YOUR GROUP INFO SHOULD GO HERE**
 *
 * Team Member: 
 * Zhongxing Chen(998471283), Chong Zhu(997962940)
 **********************************************************************/
#ifndef AST_H_
#define AST_H_ 1

#include <stdarg.h>
#define ARRAY_SIZE 1000
// Dummy node just so everything compiles, create your own node/nodes
//
// The code provided below is an example ONLY. You can use/modify it,
// but do not assume that it is correct or complete.
//
// There are many ways of making AST nodes. The approach below is an example
// of a descriminated union. If you choose to use C++, then I suggest looking
// into inheritance.

// forward declare
struct node_;
typedef struct node_ node;
extern node *ast;
extern node *inner_decl_head;
extern node *inner_stmt_head;
extern node *outer_decl_head;
extern node *outer_stmt_head;
//extern node *decl_nodes[];
//extern node *stmt_nodes[];
extern node *arguments;

typedef enum {
  UNKNOWN               = 0,

  SCOPE_NODE            = (1 << 0),
  
  EXPRESSION_NODE       = (1 << 2),
  UNARY_EXPRESION_NODE  = (1 << 2) | (1 << 3),
  BINARY_EXPRESSION_NODE= (1 << 2) | (1 << 4),
  BOOL_NODE             = (1 << 2) | (1 << 5),
  INT_NODE              = (1 << 2) | (1 << 6), 
  FLOAT_NODE            = (1 << 2) | (1 << 7),
  IDENT_NODE            = (1 << 2) | (1 << 8),
  INDEX_NODE		= (1 << 2) | (1 << 9),
  VAR_NODE              = (1 << 2) | (1 << 10),
  FUNCTION_NODE         = (1 << 2) | (1 << 11),
  CONSTRUCTOR_NODE      = (1 << 2) | (1 << 12),

  STATEMENT_NODE        = (1 << 1),
  IF_STATEMENT_NODE     = (1 << 1) | (1 << 13),
  WHILE_STATEMENT_NODE  = (1 << 1) | (1 << 14),
  ASSIGNMENT_NODE       = (1 << 1) | (1 << 15),
  NESTED_SCOPE_NODE     = (1 << 1) | (1 << 16),
  STMTS_NODE		= (1 << 1) | (1 << 17),

  DECLARATION_NODE      = (1 << 18),
  DECLS_NODE 		= (1 << 18)| (1 << 19),

  TYPE_NODE		= (1 << 20),
  
  ARGUMENTS_OPT_NODE    = (1 << 21),
//  ARGUMENT_NODE		= (1 << 20)|(1 << 21),
  //DECL_PURE_NODE	= (1 << 16) | (1 << 17),
  //DECL_WITH_ASGN_NODE	= (1 << 16) | (1 << 18),
  //DECL_CNST_ASGN_NODE 	= (1 << 16) | (1 << 19),
  
} node_kind;

struct node_ {

  // an example of tagging each node with a type
  node_kind kind;
  char arb_code[80];
  union {
    struct {
      // declarations?
      node * _decls;
      node * _stmts;
      //int _decl_index;
      //int _stmt_index;
      // statements?
	int if_cond_reg_num;
    } scope;
    
    struct {
    	node * _stmts[ARRAY_SIZE];
        int _stmts_index;
	node * _outer_stmts_head;
	int if_cond_reg_num;
    } stmts;

    struct {
        node * _decls[ARRAY_SIZE];
        int _decls_index;
	node * _outer_decls_head;
	int if_cond_reg_num;
    } decls;

    struct {
      node *next;
    } expr;

    struct {
      int val; //1 for true, 0 for false
    } bool_expr;

    struct {
      int op;
      node *right;
      int linenum;
      int ret_type;
    } unary_expr;

    struct {
      int op;
      node *left;
      node *right;
      int linenum;
      int ret_type;
    } binary_expr;

    struct {
      int val;
    } int_expr;

    struct {
      float val;
    } float_expr;

    struct {
      char *var; //id name
      int linenum;
      int ret_type;
    } id_expr;

    struct {
      char *var; //array name
      int index; //array index
      int linenum;
      int ret_type;
    } index_expr;

    struct {
      node *next;
    } var_expr;

    struct {
      int func_name;
      node * func_argu;
      int linenum;
      int ret_type;
    } func_expr;

    struct {
      node *next;
    } ctor_expr;

//    struct {
//    } statement;
  
    struct {
	int _if;
	node * _expr;
 	node * _if_statement;
	int _else;
	node * _else_statement;
        int linenum;
	int if_cond_reg_num;
    } if_statement;
  
    struct {
    } while_statement;
  
    struct {
	node * _var;
	node * _expr;
        int linenum;
        int ret_type;
	int if_cond_reg_num;
    } assign_statement;
  
    struct {
	node * _scope;
	int if_cond_reg_num;
    } nested_scope_statement;
  
    struct {
    int cnst;
    node * _type;
    char * _id;
    node * _expr;
    int linenum;
	int if_cond_reg_num;
    } decl;
    
    struct {
    int _type;
    } type;
    
    struct {
    node * _argus[20];
    int _argus_index;
    } argu_opt;


    struct{
	node * type;
	node * argu;
	int linenum;
        int ret_type;
    } cnst_expr;

    struct{
    	node * next;
	node * expr;
    } expr_argu;
    // etc.
  };
};

node *ast_allocate(node_kind type, ...);
void ast_free(node *ast);
void ast_print(node * ast);
void add_stmt_node(node * _new, node * head);
void add_decl_node(node * _new, node * head);
void add_argu_node(node * _new, node * head);

#endif /* AST_H_ */
