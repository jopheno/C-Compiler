/****************************************************/
/* File: code.h                                     */
/* Code emitting utilities for the Cminus compiler  */
/* and interface to the TM machine                  */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/
#ifndef _CODE_H_
#define _CODE_H_

/* pc = program counter  */
#define  pc 7

/* mp = "memory pointer" points
 * to top of memory (for temp storage)
 */
#define  mp 6

/* gp = "global pointer" points
 * to bottom of memory for (global)
 * variable storage
 */
#define gp 5

/* accumulator */
#define  ac 0

/* 2nd accumulator */
#define  ac1 1
void inicialize_List(Tlist *list);
Lno * insert_Node_List(Tlist* list, Arg a, Arg b, Arg c, Arg d);
char * newLabel();
char * newTemp();
void inicialize_Stack (Tstack *stack);
void insert_Stack (Tstack *stack, Lno *label);
Lno *remove_Stack(Tstack *stack);
void inicialize_Stack_Op (TstackOp *stackOp);
void insert_StackOp (TstackOp *stackOp, Arg operand);
Arg  remove_StackOp(TstackOp *stackOp);
void fprint_Node_List(FILE* file, Tlist * list);
char * newParam();
void emitComment( char * c );
void inicialize_FIFO_Args(FIFOArgs *arg);
void insert_FIFO_Args(FIFOArgs *arg, Lno *nodeIR);
Lno * remove_FIFO_Args(FIFOArgs *arg);
void inicialize_List_Label(Tlabel *listLabel);
void insert_List_Label(Tlabel *listLabel, char *label, int memoryloc);
void print_List_Label(Tlabel *listLabel);
int convertRegStringToInt(char *machineReg);
nodeLabel * search_List_Label(Tlabel *listLabel, char *label);
char * NameReg(int Reg);

#endif
