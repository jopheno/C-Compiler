/****************************************************/
/* File: symtab.c                                   */
/* Symbol table implementation for the TINY compiler*/
/* (allows only one symbol table)                   */
/* Symbol table is implemented as a chained         */
/* hash table                                       */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

void printSymTab(FILE * listing);


/* SIZE is the size of the hash table */
#define SIZE 211

/* SHIFT is the power of two used as multiplier
   in hash function  */
#define SHIFT 4

/* the hash function */
static int hash ( char * key )
{ int temp = 0;
  int i = 0;
  while (key[i] != '\0')
  { temp = ((temp << SHIFT) + key[i]) % SIZE;
    ++i;
  }
  return temp;
}

/* the hash table */
static BucketList hashTable[SIZE];

/* Procedure st_insert inserts line numbers andi
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
void st_insert( char * name, StructureId stID, int DataType, int lineno, int loc, char * scopeID, int num_arg, int passbyref)
{ int h = hash(name);
  BucketList l =  hashTable[h];
  while ((l != NULL) && !((strcmp(name,l->name) == 0) && ((strcmp(l->scope,"global")== 0) || (strcmp(l->scope,scopeID) == 0))))
    l = l->next;
  if (l == NULL) /* variable not yet in table */
  {
    l = (BucketList) malloc(sizeof(struct BucketListRec));
    l->number_args = num_arg;
    l->passBYreference = passbyref;
    l->name = name;
    l->datatype = DataType;
    l->structureid = stID;
    l->scope = scopeID;
    l->lines = (LineList) malloc(sizeof(struct LineListRec));
    l->lines->line_number = lineno;
    l->memloc = loc;
    l->lines->next = NULL;
    l->next = hashTable[h];
    hashTable[h] = l; }

    else/* found in table, so just add line number*/
  { LineList t = l->lines;
    while (t->next != NULL) t = t->next;
    t->next = (LineList) malloc(sizeof(struct LineListRec));
    t->next->line_number = lineno;
    t->next->next = NULL;
  }
} /* st_insert */

/* Function st_lookup returns the node BucketList
 * location of a variable or NULL if not found
 */
BucketList st_lookup ( char * name, char * scope)
{ int h = hash(name);
  BucketList l =  hashTable[h];
  while ((l != NULL) && !((strcmp(name,l->name) == 0) && ((strcmp(l->scope,"global")== 0) || (strcmp(l->scope,scope) == 0))))
    l = l->next;

  if (l == NULL) return NULL;
  else return l;
}
/* Procedure printSymTab prints a formatted
 * listing of the symbol table contents
 * to the listing file
 */

char * getStructureType( StructureId stID ) {
 switch(stID) {
   case FunctionSt: return "Function";
   case ParameterSt: return "Parameter";
   case VectorParameterSt: return "VectorParameter";
   case CallFunctionSt: return "CallFunction";
   case VariableUsedSt: return "VariableUsed";
   case VariableDeclaredSt: return "VariableDeclared";
   case VectorDeclaredSt: return "VectorDeclared";
   case VectorUsedSt: return "VectorUsed";
 }

 return "ERROR";
}

void printSymTab(FILE * listing)
{ int i;
  fprintf(listing,"Variable Name   Scope Function   StructureID          DataType     Location  passByref  Line Numbers\n");
  fprintf(listing,"-------------   --------------   -----------         ----------    --------  --------   ------------\n");
  for (i=0;i<SIZE;++i)
  { 
  
    if (hashTable[i] != NULL)
    { 
      BucketList l = hashTable[i];
      while (l != NULL)
      { 
        LineList t = l->lines;
        fprintf(listing,"%-18s ",l->name);
	      fprintf(listing,"%-14s ",l->scope);
        char * structureType = getStructureType(l->structureid);
	      fprintf(listing,"%-18s ", structureType);
	      if(l->datatype==0)
          fprintf(listing,"%10s ","Void");
        if(l->datatype==1)
          fprintf(listing,"%10s ","Integer");
        if(l->datatype==2)
          fprintf(listing,"%10s ","Boolean");

        fprintf(listing,"%10d  ",l->memloc);
        fprintf(listing,"%7d  ",l->passBYreference);
        while (t != NULL)
        { fprintf(listing,"%4d ",t->line_number);
          t = t->next;
        }
        fprintf(listing,"\n");
        l = l->next;
      }
    }
  }
} /* printSymTab */
