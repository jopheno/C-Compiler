/****************************************************/
/* File: symtab.h                                   */
/* Symbol table interface for the TINY compiler     */
/* (allows only one symbol table)                   */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/
#include "globals.h"
#ifndef _SYMTAB_H_
#define _SYMTAB_H_


/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
void st_insert( char * name, StructureId stID, int DataType, int lineno, int loc, char * scopeID, int num_arg, int passbyref);

/* Function st_lookup returns the node BucketList
 * location of a variable or NULL if not found
 */
BucketList st_lookup ( char * name, char * scope);

/* Procedure printSymTab prints a formatted
 * listing of the symbol table contents
 * to the listing file
 */

char * getStructureType( StructureId stID);

void printSymTab(FILE * listing);


#endif
