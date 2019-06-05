/****************************************************/
/* File: tiny.y                                     */
/* The TINY Yacc/Bison specification file           */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/
%{
#define YYPARSER /* distinguishes Yacc output from other code files */
#define YYERROR_VERBOSE 1
#define YYDEBUG 1

#include "globals.h"
#include "lex.h"
#include "util.h"

extern int lines_number;
extern char* yytext;
static int yyerrstatus = 1;

#define YYSTYPE TreeNode *
static char * savedName; /* for use in assignments */
static int savedLineNo;  /* ditto */
static TreeNode * savedTree; /* stores syntax tree for later return */

extern char tokenString[MAXTOKENLEN+1];

int STOP_PARSING = 0;

extern FILE* bison_output;

%}

%start program
%token ERROR
%token IF ELSE WHILE RETURN IMPORT
%token VOID INT
%token ID NUM
%token ASSIGN EQ DIFF LT LET GT GET PLUS MINUS TIMES OVER
%token LPAREN RPAREN LBRACKET RBRACKET LCBRACE RCBRACE SEMI COMMA
%expect 1

%% /* Grammar for TINY */

program           : decl_list {fprintf(bison_output, "[PROGRAM]\n"); savedTree = $1; }
                  ;

decl_list         : decl_list decl {fprintf(bison_output, "[decl_list->decl_list decl]\n");
                        YYSTYPE t = $1;

                        if(t != NULL) {
                          while(t->sibling != NULL)
                            t = t->sibling;
                          t->sibling = $2;
                          $$ = $1;
                        } else {
                          $$ = $2;
                        }
                  }
                  | decl {fprintf(bison_output, "[decl_list->decl]\n");
                      $$ = $1;
                  }
                  ;

decl              : var_decl {fprintf(bison_output, "[decl->var_decl]\n"); $$ = $1; }
                  | func_decl {fprintf(bison_output, "[decl->func_decl]\n"); $$ = $1; }
                  | ERROR {fprintf(bison_output, "[decl->ERROR]\n"); $$ = NULL; yyerrok; }
                  ;

tipo_espec        : INT { fprintf(bison_output, "[tipo_espec->INT]\n");
                      $$ = newExpNode(TypeK);
                      $$->type = Integer;
                  }
                  | VOID { fprintf(bison_output, "[tipo_espec->VOID]\n");
                    $$ = newExpNode(TypeK);
                    $$->type = Void;
                  }
                  ;

var_decl          : tipo_espec identificador SEMI { fprintf(bison_output, "[var_decl->tipo_espec identificador SEMI]\n");

                      $$ = newExpNode(VariableK);
                      $$->type = $1->type;
                      $$->attr.name = $2->attr.name;
                      //$$->child[0] = $1;
                  }
                  | tipo_espec identificador LBRACKET numero RBRACKET SEMI {fprintf(bison_output, "[var_decl->tipo_espec identificador LBRACKET number RBRACKET SEMI]");
                    $$ = newExpNode(VectorK);
                    $$->type = $1->type;
                    $$->attr.name = $2->attr.name;
                    $$->child[0] = $4;
                  }
                  ;

func_decl         : tipo_espec identificador LPAREN params RPAREN comp_decl { fprintf(bison_output, "[func_decl->tipo_espec identificador LPAREN params RPAREN comp_decl]\n");
                      $$ = newStmtNode(FunctionK);
                      $$->type = $1->type;
                      $$->attr.name = $2->attr.name;
                      $$->child[0] = $4;
                      $$->child[1] = $6;
                  }
                  ;

params            : lista_params { fprintf(bison_output, "[params->lista_params]\n"); $$ = $1; }
                  | VOID { fprintf(bison_output, "[params->VOID]\n"); $$ = newStmtNode(ParamK);
                           $$->attr.name="void";}
                  | { $$ = NULL;}
                  ;

lista_params      : lista_params COMMA param { fprintf(bison_output, "[lista_params->lista_params COMMA param]\n");
                    YYSTYPE t = $1;
                    if (t != NULL) {
                      while(t->sibling != NULL)
                        t = t->sibling;

                      t->sibling = $3;
                      $$=$1;
                    } else {
                      $$ = $3;
                    }
                  }
                  | param { $$ = $1; }
                  ;

param             : tipo_espec identificador { fprintf(bison_output, "[param->tipo_espec identificador]\n");
                    $$ = newStmtNode(ParamK);
                    $$->type = $1->type;
                    $$->attr.name = $2->attr.name;
                  }
                  | tipo_espec identificador LBRACKET RBRACKET { fprintf(bison_output, "[param->tipo_espec identificador LBRACKET RBRACKET]\n");
                    $$ = newStmtNode(ParamK);
                    $$->type = $1->type;
                    $$->attr.name = $2->attr.name;
                    $$->child[0] = newExpNode(VectorIdK);
                    $$->child[0]->type = $1->type;
                    $$->child[0]->attr.name = $2->attr.name;
                    //$$->passByReference = 1;
                  }
                  ;

comp_decl         : LCBRACE local_decl lista_statement RCBRACE { fprintf(bison_output, "[comp_decl->LCBRACE local_decl lista_statement RCBRACE]\n");
                    YYSTYPE t = $2;

                    if (t != NULL) {
                      while(t->sibling != NULL)
                        t = t->sibling;

                      t->sibling = $3;
                      $$ = $2;
                    } else {
                      $$ = $3;
                    }
                  }
                  | LCBRACE local_decl RCBRACE { fprintf(bison_output, "[comp_decl->LCBRACE local_decl RCBRACE]\n"); $$ = $2; }
                  | LCBRACE lista_statement RCBRACE { fprintf(bison_output, "[comp_decl->LCBRACE lista_statement RCBRACE]\n"); $$ = $2; }
                  | LCBRACE RCBRACE { fprintf(bison_output, "[comp_decl->LCBRACE RCBRACE]\n"); $$ = NULL; }
                  ;

local_decl        : local_decl var_decl { fprintf(bison_output, "[local_decl->local_decl var_decl]\n");
                    YYSTYPE t = $1;
                    if (t != NULL) {
                      while(t->sibling != NULL)
                        t = t->sibling;

                      t->sibling = $2;
                      $$ = $1;
                    } else {
                      $$ = $2;
                    }
                  }
                  | var_decl { fprintf(bison_output, "[local_decl->var_decl]\n"); $$ = $1; }
                  //| /* vazio */ {}
                  ;

exp_decl          : exp SEMI { fprintf(bison_output, "[exp_decl->exp SEMI]\n"); $$ = $1; }
                  | SEMI { fprintf(bison_output, "[exp_decl->SEMI]\n"); $$ = NULL; }
                  ;

select_decl       : IF LPAREN exp RPAREN statement { fprintf(bison_output, "[select_decl->IF LPAREN exp RPAREN statement]\n");
                    $$ = newStmtNode(IfK);
                    $$->type = $3->type;
                    $$->child[0] = $3;
                    $$->child[1] = $5;
                  }
                  | IF LPAREN exp RPAREN statement ELSE statement { fprintf(bison_output, "[select_decl->IF LPAREN exp RPAREN statement ELSE statement]\n");
                    $$ = newStmtNode(IfK);
                    $$->type = $3->type;
                    $$->child[0] = $3;
                    $$->child[1] = $5;
                    $$->child[2] = $7;
                  }
                  ;

iter_decl         : WHILE LPAREN exp RPAREN statement { fprintf(bison_output, "[iter_decl->WHILE LPAREN exp RPAREN statement]\n");
                    $$ = newStmtNode(WhileK);
                    $$->type = $3->type;
                    $$->child[0] = $3;
                    $$->child[1] = $5;
                  }
                  ;

ret_decl          : RETURN SEMI { fprintf(bison_output, "[ret_decl->RETURN SEMI]\n");
                    $$ = newStmtNode(ReturnK);
                    $$->type = Void;
                  }
                  | RETURN exp SEMI { fprintf(bison_output, "[ret_decl->RETURN exp SEMI]\n");
                    $$ = newStmtNode(ReturnK);
                    $$->type = $2->type;
                    $$->child[0] = $2;
                  }
                  ;

lista_statement   : lista_statement statement { fprintf(bison_output, "[lista_statement->lista_statement statement]\n");
                    YYSTYPE t = $1;
                    if (t != NULL) {
                      while(t->sibling != NULL)
                        t = t->sibling;

                      t->sibling = $2;
                      $$ = $1;
                    } else {
                      $$ = $2;
                    }
                  }
                  | statement {  fprintf(bison_output, "[lista_statement->statement]\n"); $$ = $1; }
                  //| /* vazio */ {}
                  ;

statement         : exp_decl { fprintf(bison_output, "[statement->exp_decl]\n"); $$ = $1; }
                  | comp_decl { fprintf(bison_output, "[statement->comp_decl]\n"); $$ = $1; }
                  | select_decl { fprintf(bison_output, "[statement->select_decl]\n"); $$ = $1; }
                  | iter_decl { fprintf(bison_output, "[statement->iter_decl]\n"); $$ = $1; }
                  | ret_decl { fprintf(bison_output, "[statement->ret_decl]\n"); $$ = $1; }
                  ;

var               : identificador { fprintf(bison_output, "[var->identificador]\n"); $$ = $1; }
                  | identificador LBRACKET exp RBRACKET { fprintf(bison_output, "[var->identificador LBRACKET exp RBRACKET]\n");
                    $$ = newExpNode(VectorIdK);
                    $$->attr.name = $1->attr.name;
                    $$->child[0] = $3;
                  }
                  ;

exp               : var ASSIGN exp { fprintf(bison_output, "[exp->var ASSIGN exp]\n");
                    $$ = newStmtNode(AssignK);
                    $$->type = $3->type;
                    $$->attr.name = "=";
                    $$->child[0] = $1;
                    $$->child[1] = $3;
                  }
                  | simple_exp { fprintf(bison_output, "[exp->simple_exp]\n"); $$ = $1; }
                  ;

simple_exp        : soma_exp relational soma_exp { fprintf(bison_output, "[simple_exp->soma_exp relational soma_exp]\n");
                    $$ = $2;
                    $$->child[0] = $1;
                    $$->child[1] = $3;
                  }
                  | soma_exp { fprintf(bison_output, "[simple_exp->soma_exp]\n"); $$ = $1; }
                  ;

relational        : LT {fprintf(bison_output, "[LT]\n"); $$ = newExpNode(OpK); $$->attr.op = LT;}
                  | LET {fprintf(bison_output, "[LET]\n"); $$ = newExpNode(OpK); $$->attr.op = LET;}
                  | GT {fprintf(bison_output, "[GT]\n"); $$ = newExpNode(OpK); $$->attr.op = GT;}
                  | GET {fprintf(bison_output, "[GET]\n"); $$ = newExpNode(OpK); $$->attr.op = GET;}
                  | EQ {fprintf(bison_output, "[EQ]\n");$$ = newExpNode(OpK); $$->attr.op = EQ;}
                  | DIFF {fprintf(bison_output, "[DIFF]\n"); $$ = newExpNode(OpK); $$->attr.op = DIFF;}
                  ;

soma_exp          : soma_exp soma termo { fprintf(bison_output, "[soma_exp->soma_exp soma termo]\n");
                    $$ = $2;
                    $$->child[0] = $1;
                    $$->child[1] = $3;
                  }
                  | termo { fprintf(bison_output, "[soma_exp->termo]\n"); $$=$1; }

soma              : PLUS { fprintf(bison_output, "[+]\n");
                    $$ = newExpNode(OpK);
                    $$->attr.op = PLUS;
                  }
                  | MINUS { fprintf(bison_output, "[-]\n");
                    $$ = newExpNode(OpK);
                    $$->attr.op = MINUS;
                  }
                  ;

termo             : termo multi fator { fprintf(bison_output, "[termo->termo multi fator]\n");
                    $$ = $2;
                    $$->child[0] = $1;
                    $$->child[1] = $3;
                  }
                  | fator { $$ = $1; }
                  ;

multi             : TIMES { fprintf(bison_output, "[multi->TIMES]\n");
                    $$ = newExpNode(OpK);
                    $$->attr.op = TIMES;
                  }
                  | OVER { fprintf(bison_output, "[multi->OVER]\n");
                    $$ = newExpNode(OpK);
                    $$->attr.op = OVER;
                  }
                  ;

fator             : LPAREN exp RPAREN { fprintf(bison_output, "[fator->LPAREN exp RPAREN]\n");
                    $$ = $2;
                    $$->type = $2->type;
                  }
                  | var { fprintf(bison_output, "[fator->var]\n"); $$ = $1; }
                  | activation { fprintf(bison_output, "[fator->activation]\n"); $$ = $1; }
                  | numero { fprintf(bison_output, "[fator->numero]\n"); $$ = $1; }
                  | ERROR { fprintf(bison_output, "[fator->ERROR]\n"); $$ = NULL; yyerrok;}
                  ;

activation        : identificador LPAREN arg_list RPAREN { fprintf(bison_output, "[activation->identificador LPAREN arg_list RPAREN]\n");
                    $$ = newStmtNode(CallK);
                    $$->type = $1->type;
                    $$->attr.name = $1->attr.name;
                    $$->child[0] = $3;
                  }
                  | identificador LPAREN RPAREN { fprintf(bison_output, "[activation->identificador LPAREN RPAREN]\n");
                    $$ = newStmtNode(CallK);
                    $$->type = $1->type;
                    $$->attr.name = $1->attr.name;
                  }
                  ;

arg_list          : arg_list COMMA exp { fprintf(bison_output, "[arg_list->arg_list COMMA exp]\n");
                    YYSTYPE t = $1;

                    if (t != NULL) {
                      while(t->sibling != NULL)
                        t = t->sibling;
                      t->sibling = $3;
                      $$ = $1;
                    } else {
                      $$ = $3;
                    }
                  }
                  | exp { $$ = $1; }
                  ;

numero            : NUM {fprintf(bison_output, "[NUM]\n");
                    $$ = newExpNode(ConstK);
                    $$->type = Integer;
                    $$->attr.val = atoi(yytext);
                  }

identificador     : ID {fprintf(bison_output, "[ID] = %s\n", yytext);$$ = newExpNode(IdK);
                        $$->attr.name = copyString(yytext);}
                  ;

%%

int yyerror(char * message)
{ fprintf(listing,"Syntax error at line %d: %s\n",lines_number,message);
  fprintf(listing,"Current token: ");
  printToken(yychar,yytext);
  Error = TRUE;

  if (yychar == 0) {
    STOP_PARSING = 1;
  }

  return 0;
}

/* yylex calls getToken to make Yacc/Bison output
 * compatible with ealier versions of the TINY scanner
 */

TreeNode * parse(void)
{ 

  yyparse();
  return savedTree;
}
