%{
/***********************************************************************
 * --YOUR GROUP INFO SHOULD GO HERE--
 * 
 * Team Member: 
 * Zhongxing Chen(998471283), Chong Zhu(997962940)
 *   Interface to the parser module for CSC467 course project.
 * 
 *   Phase 2: Implement context free grammar for source language, and
 *            parse tracing functionality.
 *   Phase 3: Construct the AST for the source language program.
 ***********************************************************************/

/***********************************************************************
 *  C Definitions and external declarations for this module.
 *
 *  Phase 3: Include ast.h if needed, and declarations for other global or
 *           external vars, functions etc. as needed.
 ***********************************************************************/

#include <string.h>

#include "common.h"
#include "ast.h"
#include "symbol.h"
#include "semantic.h"

#define YYERROR_VERBOSE
#define yTRACE(x)    { if (traceParser) fprintf(traceFile, "%s\n", x); }

void yyerror(char const* s);    /* what to do in case of error            */
int yylex();              /* procedure for calling lexical analyzer */
extern int yyline;        /* variable holding current line number   */

enum {
  DP3 = 0, 
  LIT = 1, 
  RSQ = 2
};

%}

/***********************************************************************
 *  Yacc/Bison declarations.
 *  Phase 2:
 *    1. Add precedence declarations for operators (after %start declaration)
 *    2. If necessary, add %type declarations for some nonterminals
 *  Phase 3:
 *    1. Add fields to the union below to facilitate the construction of the
 *       AST (the two existing fields allow the lexical analyzer to pass back
 *       semantic info, so they shouldn't be touched).
 *    2. Add <type> modifiers to appropriate %token declarations (using the
 *       fields of the union) so that semantic information can by passed back
 *       by the scanner.
 *    3. Make the %type declarations for the language non-terminals, utilizing
 *       the fields of the union as well.
 ***********************************************************************/

%{
#define YYDEBUG 1
%}

// defines the yyval union
%union {
  int as_int;
  int as_vec;
  float as_float;
  char *as_str;
  int as_func;
  node *as_ast;
}

%token          FLOAT_T
%token		VEC_T2
%token		VEC_T3
%token		VEC_T4
%token		INT_T
%token          IVEC_T2
%token          IVEC_T3
%token          IVEC_T4
%token          BOOL_T
%token          BVEC_T2
%token          BVEC_T3
%token          BVEC_T4
%token          CONST
%token          FALSE_C TRUE_C
//%token          FUNC
%token          IF ELSE
%token          AND OR NEQ EQ LEQ GEQ

// links specific values of tokens to yyval
%token <as_float> FLOAT_C
%token <as_int>   INT_C
%token <as_str>   ID
%token <as_int>   FUNC
// operator precdence
%left     OR                        // 7
%left     AND                       // 6
%left     EQ NEQ '<' LEQ '>' GEQ    // 5
%left     '+' '-'                   // 4
%left     '*' '/'                   // 3
%right    '^'                       // 2
%right    '!' UMINUS                // 1
%left     '(' '['                   // 0

// resolve dangling else shift/reduce conflict with associativity
%left     WITHOUT_ELSE
%left     WITH_ELSE

// type declarations
// TODO: fill this out
%type <as_ast> program
%type <as_ast> scope
%type <as_ast> declarations
%type <as_ast> statements
%type <as_ast> declaration
%type <as_ast> statement
%type <as_ast> type
%type <as_ast> expression
%type <as_ast> variable
%type <as_ast> arguments
%type <as_ast> arguments_opt

// expect one shift/reduce conflict, where Bison chooses to shift
// the ELSE.
%expect 1

%start    program

%%

/***********************************************************************
 *  Yacc/Bison rules
 *  Phase 2:
 *    1. Replace grammar found here with something reflecting the source
 *       language grammar
 *    2. Implement the trace parser option of the compiler
 ***********************************************************************/
program
  : scope 
      { yTRACE("program -> scope\n");
        errorOccurred=!semantic_check(ast);
      }
  ;

scope
  : '{' declarations statements '}'
      { yTRACE("scope -> { declarations statements }\n");
      	$$ = ast_allocate(SCOPE_NODE,inner_decl_head,inner_stmt_head);
	ast = $$;
	inner_decl_head=inner_decl_head->decls._outer_decls_head;
	inner_stmt_head=inner_stmt_head->stmts._outer_stmts_head;
      }
  ;

declarations
  : declarations declaration
      { yTRACE("declarations -> declarations declaration\n");
      	//$$ = ast_allocate (DECLS_NODE, $2);
	add_decl_node($2,inner_decl_head);
	//ast=$$;
      }
  | 
      { yTRACE("declarations -> \n");
      	if(inner_decl_head!=NULL)
        outer_decl_head=inner_decl_head;
      	$$ = ast_allocate (DECLS_NODE,outer_decl_head);
	inner_decl_head = $$;
      }
  ;

statements
  : statements statement
      { yTRACE("statements -> statements statement\n");
       	//$$ = ast_allocate (STMTS_NODE, $2);
      	add_stmt_node($2,inner_stmt_head);
	//ast=$$;
      }
  | 
      { yTRACE("statements -> \n");
      	if(inner_stmt_head!=NULL)
        outer_stmt_head=inner_stmt_head;
      	$$ = ast_allocate (STMTS_NODE,outer_stmt_head);
	inner_stmt_head = $$;
      }
  ;

declaration
  : type ID ';' 
      { yTRACE("declaration -> type ID ;\n");
      	$$ = ast_allocate (DECLARATION_NODE, 0, $1, $2,NULL, yyline);
	ast = $$;
      }
  | type ID '=' expression ';'
      { yTRACE("declaration -> type ID = expression ;\n");
	$$ = ast_allocate (DECLARATION_NODE, 0,$1, $2, $4, yyline);
	ast = $$;
	 }
  | CONST type ID '=' expression ';'
      { yTRACE("declaration -> CONST type ID = expression ;\n");
      	$$ = ast_allocate (DECLARATION_NODE, CONST, $2, $3, $5, yyline);
	ast = $$;
      }
  ;

statement
  : variable '=' expression ';'
      { yTRACE("statement -> variable = expression ;\n");
      	$$ = ast_allocate (ASSIGNMENT_NODE, $1, $3, yyline);
	ast = $$;
      }
  | IF '(' expression ')' statement ELSE statement %prec WITH_ELSE
      { yTRACE("statement -> IF ( expression ) statement ELSE statement \n");
        $$ = ast_allocate (IF_STATEMENT_NODE, IF, $3, $5, ELSE, $7,yyline);
	ast = $$;
      }
  | IF '(' expression ')' statement %prec WITHOUT_ELSE
      { yTRACE("statement -> IF ( expression ) statement \n");
      	$$ = ast_allocate (IF_STATEMENT_NODE, IF, $3, $5, 0, NULL,yyline);
	ast = $$;
      }
  | scope 
      { yTRACE("statement -> scope \n"); 
        $$ = ast_allocate (NESTED_SCOPE_NODE, $1);
	ast = $$;
      }
  | ';'
      { yTRACE("statement -> ; \n") }
  ;

type
  : INT_T
      { yTRACE("type -> INT_T \n");
      	$$ = ast_allocate (TYPE_NODE, INT_T);
	ast = $$;
      }
  | IVEC_T2
      { yTRACE("type -> IVEC_T2 \n");
      	$$ = ast_allocate (TYPE_NODE, IVEC_T2);
	ast = $$;
      }
  | IVEC_T3
      { yTRACE("type -> IVEC_T3 \n");
      	$$ = ast_allocate (TYPE_NODE, IVEC_T3);
	ast = $$;
      }
  | IVEC_T4
      { yTRACE("type -> IVEC_T4 \n");
      	$$ = ast_allocate (TYPE_NODE, IVEC_T4);
	ast = $$;
      }
  | BOOL_T
      { yTRACE("type -> BOOL_T \n");
      	$$ = ast_allocate (TYPE_NODE, BOOL_T);
	ast = $$;
      }
  | BVEC_T2
      { yTRACE("type -> BVEC_T2 \n");
      	$$ = ast_allocate (TYPE_NODE, BVEC_T2);
	ast = $$;
      }
  | BVEC_T3
      { yTRACE("type -> BVEC_T3 \n");
      	$$ = ast_allocate (TYPE_NODE, BVEC_T3);
	ast = $$;
      }
  | BVEC_T4
      { yTRACE("type -> BVEC_T4 \n");
      	$$ = ast_allocate (TYPE_NODE, BVEC_T4);
	ast = $$;
      }
  | FLOAT_T
      { yTRACE("type -> FLOAT_T \n");
      	$$ = ast_allocate (TYPE_NODE, FLOAT_T);
	ast = $$;
      }
  | VEC_T2
      { yTRACE("type -> VEC_T2 \n");
      	$$ = ast_allocate (TYPE_NODE, VEC_T2);
	ast = $$;
      }
  | VEC_T3
      { yTRACE("type -> VEC_T3 \n");
      	$$ = ast_allocate (TYPE_NODE, VEC_T3);
	ast = $$;
      }
  | VEC_T4
      { yTRACE("type -> VEC_T4 \n");
      	$$ = ast_allocate (TYPE_NODE, VEC_T4);
	ast = $$;
      }
  ;

expression

  /* constructor operators */
  : type '(' arguments_opt ')' %prec '('
      { yTRACE("expression -> type ( arguments_opt ) \n");
        $$ = ast_allocate (CONSTRUCTOR_NODE, $1, $3,yyline);
	ast = $$;
      }

  /* function-like operators */
  | FUNC '(' arguments_opt ')' %prec '('
      { yTRACE("expression -> FUNC ( arguments_opt ) \n");
        $$ = ast_allocate (FUNCTION_NODE, $1, $3,yyline);
	ast = $$;
      }

  /* unary opterators */
  | '-' expression %prec UMINUS
      { yTRACE("expression -> - expression \n");
        $$ = ast_allocate (UNARY_EXPRESION_NODE, '-', $2, yyline);
	ast = $$;
      }
  | '!' expression %prec '!'
      { yTRACE("expression -> ! expression \n");
        $$ = ast_allocate (UNARY_EXPRESION_NODE, '!', $2, yyline);
	ast = $$;
      }

  /* binary operators */
  | expression AND expression %prec AND
      { yTRACE("expression -> expression AND expression \n");
        $$ = ast_allocate (BINARY_EXPRESSION_NODE, AND, $1 , $3, yyline);
	ast = $$;
      }
  | expression OR expression %prec OR
      { yTRACE("expression -> expression OR expression \n");
        $$ = ast_allocate (BINARY_EXPRESSION_NODE, OR, $1 , $3, yyline);
	ast = $$;
      }
  | expression EQ expression %prec EQ
      { yTRACE("expression -> expression EQ expression \n");
        $$ = ast_allocate (BINARY_EXPRESSION_NODE, EQ, $1 , $3, yyline);
	ast = $$;
      }
  | expression NEQ expression %prec NEQ
      { yTRACE("expression -> expression NEQ expression \n");
        $$ = ast_allocate (BINARY_EXPRESSION_NODE, NEQ, $1 , $3, yyline);
	ast = $$;
      }
  | expression '<' expression %prec '<'
      { yTRACE("expression -> expression < expression \n");
        $$ = ast_allocate (BINARY_EXPRESSION_NODE, '<', $1 , $3, yyline);
	ast = $$;
      }
  | expression LEQ expression %prec LEQ
      { yTRACE("expression -> expression LEQ expression \n");
        $$ = ast_allocate (BINARY_EXPRESSION_NODE, LEQ, $1 , $3, yyline);
	ast = $$;
      }
  | expression '>' expression %prec '>'
      { yTRACE("expression -> expression > expression \n");
        $$ = ast_allocate (BINARY_EXPRESSION_NODE, '>', $1 , $3, yyline);
	ast = $$;
      }
  | expression GEQ expression %prec GEQ
      { yTRACE("expression -> expression GEQ expression \n");
        $$ = ast_allocate (BINARY_EXPRESSION_NODE, GEQ, $1 , $3, yyline);
	ast = $$;
      }
  | expression '+' expression %prec '+'
      { yTRACE("expression -> expression + expression \n");
        $$ = ast_allocate (BINARY_EXPRESSION_NODE, '+', $1 , $3, yyline);
	ast = $$;
      }
  | expression '-' expression %prec '-'
      { yTRACE("expression -> expression - expression \n");
        $$ = ast_allocate (BINARY_EXPRESSION_NODE, '-', $1 , $3, yyline);
	ast = $$;
      }
  | expression '*' expression %prec '*'
      { yTRACE("expression -> expression * expression \n");
        $$ = ast_allocate (BINARY_EXPRESSION_NODE, '*', $1 , $3, yyline);
	ast = $$;
      }
  | expression '/' expression %prec '/'
      { yTRACE("expression -> expression / expression \n");
        $$ = ast_allocate (BINARY_EXPRESSION_NODE, '/', $1 , $3, yyline);
	ast = $$;
      }
  | expression '^' expression %prec '^'
      { yTRACE("expression -> expression ^ expression \n");
        $$ = ast_allocate (BINARY_EXPRESSION_NODE, '^', $1 , $3, yyline);
	ast = $$;
      }

  /* literals */
  | TRUE_C
      { yTRACE("expression -> TRUE_C \n");
        $$ = ast_allocate (BOOL_NODE, 1);
	ast = $$;
      }
  | FALSE_C
      { yTRACE("expression -> FALSE_C \n");
        $$ = ast_allocate (BOOL_NODE, 0);
	ast = $$;
      }
  | INT_C
      { yTRACE("expression -> INT_C \n");
        $$ = ast_allocate (INT_NODE, $1);
	ast = $$;
      }
  | FLOAT_C
      { yTRACE("expression -> FLOAT_C \n");
        $$ = ast_allocate (FLOAT_NODE, $1);
	ast = $$;
      }

  /* misc */
  | '(' expression ')'
      { yTRACE("expression -> ( expression ) \n");
      	$$ = ast_allocate (EXPRESSION_NODE, $2);
	ast = $$;
      }
  | variable { }
    { yTRACE("expression -> variable \n");
        $$ = ast_allocate (VAR_NODE, $1);
	ast = $$;
    }
  ;

variable
  : ID
      { yTRACE("variable -> ID \n");
        $$ = ast_allocate (IDENT_NODE, $1, yyline);
	ast = $$;
      }
  | ID '[' INT_C ']' %prec '['
      { yTRACE("variable -> ID [ INT_C ] \n");
        $$ = ast_allocate (INDEX_NODE, $1, $3, yyline);
	ast = $$;
      }
  ;

arguments
  : arguments ',' expression
      { yTRACE("arguments -> arguments , expression \n");
      	//$$ = ast_allocate (ARGU_WITH_EXPR_NODE, $1, $3);
	//ast = $$;
	add_argu_node($3,arguments);
      }
  | expression
      { yTRACE("arguments -> expression \n");
      	$$ = ast_allocate (ARGUMENTS_OPT_NODE,$1);
	ast = $$;
	arguments=ast;
      	//$$ = ast_allocate (, $1);
	//ast = $$;
      }
  ;

arguments_opt
  : arguments
      { yTRACE("arguments_opt -> arguments \n") }
  |
      { yTRACE("arguments_opt -> \n");
    //    $$ = ast_allocate (ARGUMENTS_OPT_NODE);
//	ast = $$;
      }
  ;

%%

/***********************************************************************ol
 * Extra C code.
 *
 * The given yyerror function should not be touched. You may add helper
 * functions as necessary in subsequent phases.
 ***********************************************************************/
void yyerror(char const* s) {
  if(errorOccurred) {
    return;    /* Error has already been reported by scanner */
  } else {
    errorOccurred = 1;
  }

  fprintf(errorFile, "\nPARSER ERROR, LINE %d", yyline);
  
  if(strcmp(s, "parse error")) {
    if(strncmp(s, "parse error, ", 13)) {
      fprintf(errorFile, ": %s\n", s);
    } else {
      fprintf(errorFile, ": %s\n", s+13);
    }
  } else {
    fprintf(errorFile, ": Reading token %s\n", yytname[YYTRANSLATE(yychar)]);
  }
}

