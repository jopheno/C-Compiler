/****************************************************/
/* File: globals.h                                  */
/* Global types and vars for TINY compiler          */
/* must come before other include files             */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef YYPARSER

#include "bison.tab.h"
#define ENDFILE 0

#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* MAXRESERVED = the number of reserved words */
#define MAXTOKENLEN 100
#define MAXLEXICALUNITLEN 100
#define MAXRESERVED 8

#ifndef YYTOKENTYPE

typedef enum {
    /* book-keeping tokens */
    ENDFILE,ERROR,
    /* reserved words */
    IF,ELSE,WHILE,RETURN,IMPORT,
    /* variable types */
    VOID,INT,
    /* multicharacter tokens */
    ID,NUM,
    /* special symbols */
    ASSIGN,EQ,DIFF,LT,LET,GT,GET,PLUS,MINUS,TIMES,OVER,LPAREN,RPAREN,LBRACKET,RBRACKET,LCBRACE,RCBRACE,SEMI,COMMA
} TokenType;

#endif

#ifdef YYTOKENTYPE

typedef enum yytokentype TokenType;

#endif

typedef struct {
  TokenType type;
  char lexical_unit[MAXTOKENLEN];
} Token;

extern FILE* source; /* source code text file */
extern FILE* listing; /* listing output text file */
extern FILE* code; /* code text file for TM simulator */

extern int lines_number; /* source line number for listing */

/**************************************************/
/***********   Syntax tree for parsing ************/
/**************************************************/

typedef enum {
  StmtK,
  ExpK
} NodeKind;

typedef enum {
  IfK,
  WhileK,
  AssignK,
  FunctionK,
  CallK,
  ParamK,
  ReturnK,
  NumberK
} StmtKind;

typedef enum {
  OpK,
  ConstK,
  IdK,
  VariableK,
  VectorK,
  VectorIdK,
  TypeK
} ExpKind;

/* ExpType is used for type checking */
typedef enum {Void,Integer,Boolean} ExpType;

#define MAXCHILDREN 3

typedef struct treeNode
   { struct treeNode * child[MAXCHILDREN];
     struct treeNode * sibling;
     int line_number;
     NodeKind nodekind;
     char * scope;
     union { StmtKind stmt; ExpKind exp;} kind;
     union { TokenType op;
             int val;
             char * name; } attr;
     ExpType type; /* for type checking of exps */
   } TreeNode;

/* Estrutura de dados da tabela de símbolos */

typedef enum {
  FunctionSt,
  ParameterSt,
  VectorParameterSt,
  CallFunctionSt,
  AssignSt,
  VariableUsedSt,
  VariableDeclaredSt,
  VectorDeclaredSt,
  VectorUsedSt
} StructureId;

typedef struct LineListRec
{
  int line_number;
  struct LineListRec* next;
} * LineList;

typedef struct BucketListRec
{
    char * name;
    StructureId structureid;
    int  datatype;
    char *scope;
    int number_args;
    int passBYreference;
    LineList lines;
    int memloc ; /* memory location for variable */
    struct BucketListRec * next;
} * BucketList;


/**************************************************/
/****List for Intermediate Code Generation********/
typedef enum {Bucket, String, Constant} type_union;
typedef struct typeArg
{
    union
    {
        BucketList variable;
        char *string;
        int constant;
    } addr;
    int address;
    type_union type;
} Arg;

typedef struct no
{
    struct no *ant, *prox;
    Arg op, arg1, arg2, result;
    int id;
    int number_instm;
    int passBYreference;
} Lno;

typedef struct list
{
    Lno* first;
    Lno* last;
    int length;
} Tlist;

/******** Structure FIFO for arguments ************/
typedef struct nodeArgs{
    struct nodeArgs *prox;
    Lno *nodeIR;
    int num;
}nodeArgs;

typedef struct FIFOargs{
    nodeArgs *init, *end;
    int length;
}FIFOArgs;


/**************************************************/

/**************************************************/
/*Stack for Labels from the Intermediate Code Generation*/

typedef struct elem
{
    Lno *label;
    struct elem *prox;
} Sno;

typedef struct stack
{
    Sno *top;
} Tstack;
/**************************************************/

/*Stack for operands in Intermediate Code Generation*/

typedef struct elemOp
{
    Arg op;
    struct elemOp *prox;
} SOp;

typedef struct stackOp
{
    SOp *top;
} TstackOp;

/************************************************************************************************/
/*****Structure for labels from the first pass **************************************************/
typedef struct node
{
    struct node *prox;
    char *label_IR;
    int memLoc;
    int addrfinalfunct;
} nodeLabel;

typedef struct nodeL
{
    nodeLabel *init;
    nodeLabel *end;
    int length;
} Tlabel;

/**************************************************/
/***********   Flags for tracing       ************/
/**************************************************/


/* EchoSource = TRUE causes the source program to
 * be echoed to the listing file with line numbers
 * during parsing
 */
extern int EchoSource;

/* TraceScan = TRUE causes token information to be
 * printed to the listing file as each token is
 * recognized by the scanner
 */
extern int TraceScan;

/* TraceParse = TRUE causes the syntax tree to be
 * printed to the listing file in linearized form
 * (using indents for children)
 */
extern int TraceParse;

/* TraceAnalyze = TRUE causes symbol table inserts
 * and lookups to be reported to the listing file
 */
extern int TraceAnalyze;

/* TraceCode = TRUE causes comments to be written
 * to the TM code file as code is generated
 */
extern int TraceCode;

/* Error = TRUE prevents further passes if an error occurs */
extern int Error;

#endif
