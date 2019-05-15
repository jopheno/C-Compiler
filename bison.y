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
//#include "parse.h"
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

program           : decl_list {printf("[PROGRAM]\n"); savedTree = $1; }
                  ;

decl_list         : decl_list decl {printf("[decl_list->decl_list decl]\n");
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
                  | decl {printf("[decl_list->decl]\n");
                      $$ = $1;
                  }
                  ;

decl              : var_decl {printf("[decl->var_decl]\n"); $$ = $1; }
                  | func_decl {printf("[decl->func_decl]\n"); $$ = $1; }
                  | ERROR {printf("[decl->ERROR]\n"); $$ = NULL; yyerrok; }
                  ;

tipo_espec        : INT { printf("[tipo_espec->INT]\n");
                      $$ = newExpNode(TypeK);
                      $$->type = Integer;
                  }
                  | VOID { printf("[tipo_espec->VOID]\n");
                    $$ = newExpNode(TypeK);
                    $$->type = Void;
                  }
                  ;

var_decl          : tipo_espec identificador SEMI { printf("[var_decl->tipo_espec identificador SEMI]\n");

                      $$ = newExpNode(VariableK);
                      $$->type = $1->type;
                      $$->attr.name = $2->attr.name;
                      //$$->child[0] = $1;
                  }
                  | tipo_espec identificador LBRACKET numero RBRACKET SEMI {printf("[var_decl->tipo_espec identificador LBRACKET number RBRACKET SEMI]");
                    $$ = newExpNode(VectorK);
                    $$->type = $1->type;
                    $$->attr.name = $2->attr.name;
                    $$->child[0] = $4;
                  }
                  ;

func_decl         : tipo_espec identificador LPAREN params RPAREN comp_decl { printf("[func_decl->tipo_espec identificador LPAREN params RPAREN comp_decl]\n");
                      $$ = newStmtNode(FunctionK);
                      $$->type = $1->type;
                      $$->attr.name = $2->attr.name;
                      $$->child[0] = $4;
                      $$->child[1] = $6;
                  }
                  ;

params            : lista_params { printf("[params->lista_params]\n"); $$ = $1; }
                  | VOID { printf("[params->VOID]\n"); $$ = newStmtNode(ParamK);
                           $$->attr.name="void";}
                  | { $$ = NULL;}
                  ;

lista_params      : lista_params COMMA param { printf("[lista_params->lista_params COMMA param]\n");
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

param             : tipo_espec identificador { printf("[param->tipo_espec identificador]\n");
                    $$ = newStmtNode(ParamK);
                    $$->type = $1->type;
                    $$->attr.name = $2->attr.name;
                  }
                  | tipo_espec identificador LBRACKET RBRACKET { printf("[param->tipo_espec identificador LBRACKET RBRACKET]\n");
                    $$ = newStmtNode(ParamK);
                    $$->type = $1->type;
                    $$->attr.name = $2->attr.name;
                    $$->child[0] = newExpNode(VectorIdK);
                    $$->child[0]->type = $1->type;
                    $$->child[0]->attr.name = $2->attr.name;
                    //$$->passByReference = 1;
                  }
                  ;

comp_decl         : LCBRACE local_decl lista_statement RCBRACE { printf("[comp_decl->LCBRACE local_decl lista_statement RCBRACE]\n");
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
                  | LCBRACE local_decl RCBRACE { printf("[comp_decl->LCBRACE local_decl RCBRACE]\n"); $$ = $2; }
                  | LCBRACE lista_statement RCBRACE { printf("[comp_decl->LCBRACE lista_statement RCBRACE]\n"); $$ = $2; }
                  | LCBRACE RCBRACE { printf("[comp_decl->LCBRACE RCBRACE]\n"); $$ = NULL; }
                  ;

local_decl        : local_decl var_decl { printf("[local_decl->local_decl var_decl]\n");
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
                  | var_decl { printf("[local_decl->var_decl]\n"); $$ = $1; }
                  //| /* vazio */ {}
                  ;

exp_decl          : exp SEMI { printf("[exp_decl->exp SEMI]\n"); $$ = $1; }
                  | SEMI { printf("[exp_decl->SEMI]\n"); $$ = NULL; }
                  ;

select_decl       : IF LPAREN exp RPAREN statement { printf("[select_decl->IF LPAREN exp RPAREN statement]\n");
                    $$ = newStmtNode(IfK);
                    $$->type = $3->type;
                    $$->child[0] = $3;
                    $$->child[1] = $5;
                  }
                  | IF LPAREN exp RPAREN statement ELSE statement { printf("[select_decl->IF LPAREN exp RPAREN statement ELSE statement]\n");
                    $$ = newStmtNode(IfK);
                    $$->type = $3->type;
                    $$->child[0] = $3;
                    $$->child[1] = $5;
                    $$->child[2] = $7;
                  }
                  ;

iter_decl         : WHILE LPAREN exp RPAREN statement { printf("[iter_decl->WHILE LPAREN exp RPAREN statement]\n");
                    $$ = newStmtNode(WhileK);
                    $$->type = $3->type;
                    $$->child[0] = $3;
                    $$->child[1] = $5;
                  }
                  ;

ret_decl          : RETURN SEMI { printf("[ret_decl->RETURN SEMI]\n");
                    $$ = newStmtNode(ReturnK);
                    $$->type = Void;
                  }
                  | RETURN exp SEMI { printf("[ret_decl->RETURN exp SEMI]\n");
                    $$ = newStmtNode(ReturnK);
                    $$->type = $2->type;
                    $$->child[0] = $2;
                  }
                  ;

lista_statement   : lista_statement statement { printf("[lista_statement->lista_statement statement]\n");
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
                  | statement {  printf("[lista_statement->statement]\n"); $$ = $1; }
                  //| /* vazio */ {}
                  ;

statement         : exp_decl { printf("[statement->exp_decl]\n"); $$ = $1; }
                  | comp_decl { printf("[statement->comp_decl]\n"); $$ = $1; }
                  | select_decl { printf("[statement->select_decl]\n"); $$ = $1; }
                  | iter_decl { printf("[statement->iter_decl]\n"); $$ = $1; }
                  | ret_decl { printf("[statement->ret_decl]\n"); $$ = $1; }
                  ;

var               : identificador { printf("[var->identificador]\n"); $$ = $1; }
                  | identificador LBRACKET exp RBRACKET { printf("[var->identificador LBRACKET exp RBRACKET]\n");
                    $$ = newExpNode(VectorIdK);
                    $$->attr.name = $1->attr.name;
                    $$->child[0] = $3;
                  }
                  ;

exp               : var ASSIGN exp { printf("[exp->var ASSIGN exp]\n");
                    $$ = newStmtNode(AssignK);
                    $$->type = $3->type;
                    $$->attr.name = "=";
                    $$->child[0] = $1;
                    $$->child[1] = $3;
                  }
                  | simple_exp { printf("[exp->simple_exp]\n"); $$ = $1; }
                  ;

simple_exp        : soma_exp relational soma_exp { printf("[simple_exp->soma_exp relational soma_exp]\n");
                    $$ = $2;
                    $$->child[0] = $1;
                    $$->child[1] = $3;
                  }
                  | soma_exp { printf("[simple_exp->soma_exp]\n"); $$ = $1; }
                  ;

relational        : LT {printf("[LT]\n"); $$ = newExpNode(OpK); $$->attr.op = LT;}
                  | LET {printf("[LET]\n"); $$ = newExpNode(OpK); $$->attr.op = LET;}
                  | GT {printf("[GT]\n"); $$ = newExpNode(OpK); $$->attr.op = GT;}
                  | GET {printf("[GET]\n"); $$ = newExpNode(OpK); $$->attr.op = GET;}
                  | EQ {printf("[EQ]\n");$$ = newExpNode(OpK); $$->attr.op = EQ;}
                  | DIFF {printf("[DIFF]\n"); $$ = newExpNode(OpK); $$->attr.op = DIFF;}
                  ;

soma_exp          : soma_exp soma termo { printf("[soma_exp->soma_exp soma termo]\n");
                    $$ = $2;
                    $$->child[0] = $1;
                    $$->child[1] = $3;
                  }
                  | termo { printf("[soma_exp->termo]\n"); $$=$1; }

soma              : PLUS { printf("[+]\n");
                    $$ = newExpNode(OpK);
                    $$->attr.op = PLUS;
                  }
                  | MINUS { printf("[-]\n");
                    $$ = newExpNode(OpK);
                    $$->attr.op = MINUS;
                  }
                  ;

termo             : termo multi fator { printf("[termo->termo multi fator]\n");
                    $$ = $2;
                    $$->child[0] = $1;
                    $$->child[1] = $3;
                  }
                  | fator { $$ = $1; }
                  ;

multi             : TIMES { printf("[multi->TIMES]\n");
                    $$ = newExpNode(OpK);
                    $$->attr.op = TIMES;
                  }
                  | OVER { printf("[multi->OVER]\n");
                    $$ = newExpNode(OpK);
                    $$->attr.op = OVER;
                  }
                  ;

fator             : LPAREN exp RPAREN { printf("[fator->LPAREN exp RPAREN]\n");
                    $$ = $2;
                    $$->type = $2->type;
                  }
                  | var { printf("[fator->var]\n"); $$ = $1; }
                  | activation { printf("[fator->activation]\n"); $$ = $1; }
                  | numero { printf("[fator->numero]\n"); $$ = $1; }
                  | ERROR { printf("[fator->ERROR]\n"); $$ = NULL; yyerrok;}
                  ;

activation        : identificador LPAREN arg_list RPAREN { printf("[activation->identificador LPAREN arg_list RPAREN]\n");
                    $$ = newStmtNode(CallK);
                    $$->type = $1->type;
                    $$->attr.name = $1->attr.name;
                    $$->child[0] = $3;
                  }
                  | identificador LPAREN RPAREN { printf("[activation->identificador LPAREN RPAREN]\n");
                    $$ = newStmtNode(CallK);
                    $$->type = $1->type;
                    $$->attr.name = $1->attr.name;
                  }
                  ;

arg_list          : arg_list COMMA exp { printf("[arg_list->arg_list COMMA exp]\n");
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

numero            : NUM {printf("[NUM]\n");
                    $$ = newExpNode(ConstK);
                    $$->type = Integer;
                    $$->attr.val = atoi(yytext);
                  }

identificador     : ID {printf("[ID] = %s\n", yytext);$$ = newExpNode(IdK);
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

/*static int yylex(void)
{ Token tok = getToken();

  return tok.type;
}*/

/*static int yylex(void)
{ return getTokenByType(); }*/

TreeNode * parse(void)
{ 
  /*if (!STOP_PARSING) {
    if (yyparse() == 0) { // YYACCEPT
      return savedTree;
    } else {
      savedTree = NULL;
      //yyerrok;
      //yyerrok();
      //parse();
    }
  }*/

  yyparse();
  return savedTree;
}
