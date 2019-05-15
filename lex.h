
#include "globals.h"

#ifndef _LEX_H_
#define _LEX_H_

Token getToken(void);

char* getTokenName(TokenType t);

void openFile(char* file_name);

TreeNode* parse(void);

//void principal();

#endif
