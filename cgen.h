/****************************************************/
/* File: cgen.h                                     */
/* The code generator interface to the TINY compiler*/
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _CGEN_H_
#define _CGEN_H_

/* Procedure codeGen generates code to a code
 * file by traversal of the syntax tree. The
 * second parameter (codefile) is the file name
 * of the code file, and is used to print the
 * file name as a comment in the code file
 */
/*Struct for map temporaries registers from Intermediate Code*/

typedef struct tempReg
{
    int state;
    char *usedBy;
    char *newReg;
    int address;
} TypeRegTemp;

void codeGen(TreeNode * syntaxTree, FILE* output);
char *new(int num);
char * get_new_temp(char * regtemp, int address);
void tempOp(Lno *node_IR);
void insertOP(char *op);
void insertSWLW(Arg a, char *lwsw);
char * getUltimateReg();
TypeRegTemp set_temp_used(char *reg);
void generate_limited_temporaries(Tlist *list);
Lno * insert_Node_List(Tlist* list, Arg a, Arg b, Arg c, Arg d);
#endif
