/****************************************************/
/* File: util.c                                     */
/* Utility function implementation                  */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include <stdbool.h>
#include "globals.h"
#include "util.h"

/* Procedure printToken prints a token
* and its lexeme to the listing file
*/

extern int lines_number;

import_file_t* imports[MAX_FILE_IMPORTS];
int import_amount = 0;
bool ImportErrors = false;

int add_import(char* file_name, int line_number) {

  // Backup first character
  char v = file_name[0];

  // Remove quotes
	file_name = file_name+1;
	file_name[strlen(file_name)-1] = 0;

	// Verify if it is double quotes
  char* new_filename = (char*) malloc(sizeof(char)*50);
  if (v == '<') {
    sprintf(new_filename, "%s/%s", DEFAULT_LIBRARIES_PATH, file_name);
  } else {
    sprintf(new_filename, "%s", file_name);
  }

  // Verify if there is any other import in the same line or any similar import
  for (int i = 0; i<import_amount; i++) {
    if (imports[i]->line == line_number) {
      fprintf(stderr, "> Unable to import '%s' at line %d, '%s' was already imported.\n", file_name, lines_number, imports[i]->filename);
      fprintf(stderr, ">> Consider moving '%s' to the next line.\n", new_filename);
      ImportErrors = true;
      return -1;
    }

    if (strcmp(imports[i]->filename, new_filename) == 0) {
      fprintf(stderr, "> Unable to import '%s' at line %d, '%s' was already imported.\n", file_name, lines_number, imports[i]->filename);
      ImportErrors = true;
      return -1;
    }
  }

  import_file_t* new_import = (import_file_t*) malloc(sizeof(import_file_t));
  new_import->filename = (char*) malloc(strlen(new_filename));

  memcpy(new_import->filename, new_filename, strlen(new_filename));
  new_import->line = lines_number;

  imports[import_amount] = new_import;
  
  import_amount++;

  return 0;

}

int generate_final_file(char* source_file) {

	if (ImportErrors) {
		return -1;
	}

	int curr_line = 1;

	FILE* source = fopen(source_file,"r");
	if (source==NULL)
	{
		fprintf(stderr,"File '%s' not found\n",source_file);
		return -1;
	}

	fseek(source, 0, SEEK_END);
	size_t eof = ftell(source);
	fseek(source, 0, SEEK_SET);

	FILE* dest = fopen(TEMP_FILE_NAME,"w");
	if (dest==NULL)
	{
		fprintf(stderr,"File '%s' could not be created\n",TEMP_FILE_NAME);
		return -1;
	}

	char aux = 0;
	size_t pos = 0;
	bool skip_line = false;
	while(pos < eof) {
		fread(&aux, sizeof(char), 1, source);

		if (!skip_line)
			fwrite(&aux, sizeof(char), 1, dest);

		pos++;

		if (aux == '\n') {
			skip_line = false;
			curr_line++;

			for (int i = 0; i<import_amount; i++) {
				if (imports[i]->line == curr_line) {
					FILE* temp = fopen(imports[i]->filename, "r");

					if (temp==NULL)
					{
						fprintf(stderr,"File '%s' could not be found\n", imports[i]->filename);
						return -1;
					}

					fseek(temp, 0, SEEK_END);
					size_t temp_size = ftell(temp);
					fseek(temp, 0, SEEK_SET);

					char* temp_content = (char*) malloc(sizeof(char)*temp_size);

					fread(temp_content, sizeof(char), temp_size, temp);
					fwrite(temp_content, sizeof(char), temp_size, dest);
					fflush(dest);
					free(temp_content);
					fclose(temp);

					skip_line = true;
				}
			}
		}
	}

	fflush(dest);
	fclose(dest);

	return 0;

}

void printToken( TokenType token, const char* tokenString )
{ 
  switch (token)
  {
    case ENDFILE: fprintf(listing,"EOF\n"); break;
    case ERROR: fprintf(listing,"ERROR: %s\n",tokenString); break;
    case IF:
    case ELSE:
    case WHILE:
    case RETURN:
    case IMPORT:
    case VOID:
    case INT:
      fprintf(listing, "reserved word: %s\n",tokenString);
      break;

    case NUM: fprintf(listing, "NUM, val= %s\n",tokenString); break;
    case ID: fprintf(listing, "ID, name= %s\n",tokenString); break;
    case INST: fprintf(listing, "INST, name= %s\n",tokenString); break;
    case ASSIGN: fprintf(listing,"=\n"); break;
    case EQ: fprintf(listing,"==\n"); break;
    case DIFF: fprintf(listing,"!=\n"); break;
    case LT: fprintf(listing,"<\n"); break;
    case LET: fprintf(listing,"<=\n"); break;
    case GT: fprintf(listing,">\n"); break;
    case GET: fprintf(listing,">=\n"); break;
    case PLUS: fprintf(listing,"+\n"); break;
    case MINUS: fprintf(listing,"-\n"); break;
    case TIMES: fprintf(listing,"*\n"); break;
    case OVER: fprintf(listing,"/\n"); break;
    case LPAREN: fprintf(listing,"(\n"); break;
    case RPAREN: fprintf(listing,")\n"); break;
    case LBRACKET: fprintf(listing,"[\n"); break;
    case RBRACKET: fprintf(listing,"]\n"); break;
    case LCBRACE: fprintf(listing,"{\n"); break;
    case RCBRACE: fprintf(listing,"}\n"); break;
    case SEMI: fprintf(listing,";\n"); break;
    case COMMA: fprintf(listing,",\n"); break;
    default: /* should never happen */
      fprintf(listing,"Unknown token: %d\n",token);
  }
}

/* Function newStmtNode creates a new statement
* node for syntax tree construction
*/

TreeNode * newStmtNode(StmtKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
  fprintf(listing,"Out of memory error at line %d\n",lines_number);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = StmtK;
    t->kind.stmt = kind;
    t->line_number = lines_number;
    t->scope = NULL;
  }
  return t;
}

/* Function newExpNode creates a new expression
* node for syntax tree construction
*/
TreeNode * newExpNode(ExpKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
  fprintf(listing,"Out of memory error at line %d\n",lines_number);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = ExpK;
    t->kind.exp = kind;
    t->line_number = lines_number;
    t->type = Void;
    t->scope = NULL;
  }
  return t;
}

/* Function copyString allocates and makes a new
* copy of an existing string
*/
char * copyString(char * s)
{ int n;
  char * t;
  if (s==NULL) return NULL;
  n = strlen(s)+1;
  t = malloc(n);
  if (t==NULL)
  fprintf(listing,"Out of memory error at line %d\n",lines_number);
  else strcpy(t,s);
  return t;
}

/* Variable indentno is used by printTree to
* store current number of spaces to indent
*/
static int indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno+=2
#define UNINDENT indentno-=2

/* printSpaces indents by printing spaces */
static void printSpaces(void)
{ int i;
  for (i=0;i<indentno;i++)
  fprintf(listing," ");
}

/* procedure printTree prints a syntax tree to the
* listing file using indentation to indicate subtrees
*/
void printTree( TreeNode * tree )
{ int i;
  INDENT;
  while (tree != NULL) {
    printSpaces();
    if (tree->nodekind==StmtK)
    { 
      switch (tree->kind.stmt) {
        case IfK:
          fprintf(listing,"If\n");
          break;
        case WhileK:
          fprintf(listing,"While\n");
          break;
        case AssignK:
          fprintf(listing,"Assign to: %s\n",tree->attr.name);
          break;
        case FunctionK:
          fprintf(listing,"Function %s\n",tree->attr.name);
          break;
        case AssemblyK:
          fprintf(listing,"Assembly %s\n",tree->attr.name);
          break;
        default:
          fprintf(listing,"Unknown StmtNode kind\n");
          break;
      }
    }
    else if (tree->nodekind==ExpK) {
      switch (tree->kind.exp) {
        case OpK:
        fprintf(listing,"Op: ");
        printToken(tree->attr.op,"\0");
        break;
        case ConstK:
        fprintf(listing,"Const: %d\n",tree->attr.val);
        break;
        case IdK:
        fprintf(listing,"Id: %s\n",tree->attr.name);
        break;
        case IdAddrK:
        fprintf(listing,"Id Addr: %s\n",tree->attr.name);
        break;
        case InstructionK:
        fprintf(listing,"Instruction: %s\n",tree->attr.name);
        break;
        default:
        fprintf(listing,"Unknown ExpNode kind\n");
        break;
      }
    }
    else
      fprintf(listing,"Unknown node kind\n");
    
    for (i=0;i<MAXCHILDREN;i++)
      printTree(tree->child[i]);

    tree = tree->sibling;
  }

  UNINDENT;
}

void printTreeC( TreeNode * tree )
{ int i;
  INDENT;
  while (tree != NULL) {
    printSpaces();

    if (tree->nodekind==StmtK)
    {switch (tree->kind.stmt) {
      case FunctionK:
      switch(tree->type){
        case 0:
        fprintf(listing,"Type: Void\n");
        break;
        case 1:
        fprintf(listing,"Type: Int\n");
        break;
      }
      fprintf(listing,"FunctionK: %s\n",tree->attr.name);
      break;
      case ParamK:

      switch(tree->type){
        case 0:
        fprintf(listing,"Type: Void\n");
        break;
        case 1:
        fprintf(listing,"Type: Int\n");
        break;
      }

      if(tree->child[0]==NULL)
      fprintf(listing,"Parameter: %s\n", tree->attr.name);
      else
      fprintf(listing,"Parameter Vector: %s\n", tree->attr.name);
      break;
      case CallK:
      fprintf(listing,"Call Function: %s\n", tree->attr.name);
      break;

      case IfK:
      fprintf(listing,"IF\n");
      break;
      case WhileK:
      fprintf(listing,"WHILE\n");
      break;
      case AssignK:
      fprintf(listing,"ASSIGN TO: %s\n",tree->attr.name);
      break;
      case ReturnK:
      fprintf(listing,"RETURN \n");
      break;
      case AssemblyK:
      fprintf(listing,"Assembly: \n");
      break;
      default:
      fprintf(listing,"Unknown StmtNode kind\n");
      break;
    }
  }
  else if (tree->nodekind==ExpK)
  {switch (tree->kind.exp) {
    case VectorK:
    fprintf(listing,"Array: %s [%d] \n",tree->attr.name, tree->child[0]->attr.val);
    break;
    case VariableK:
    fprintf(listing,"Variable Declared: %s\n",tree->attr.name);
    break;


    case OpK:
    fprintf(listing,"Op: ");
    printToken(tree->attr.op,"\0");
    break;
    case ConstK:
    fprintf(listing,"Const: %d\n",tree->attr.val);
    break;
    case IdK:
    fprintf(listing,"Variable Used: %s\n",tree->attr.name);
    break;
    case IdAddrK:
    fprintf(listing,"Variable Addr Used: %s\n",tree->attr.name);
    break;
    case VectorIdK:
    fprintf(listing,"Array Used: %s\n",tree->attr.name);
    break;
    case InstructionK:
    fprintf(listing,"Instruction: %s\n",tree->attr.name);
    break;
    case TypeK:
    switch(tree->type){
      case 0:
      fprintf(listing,"Type: Void\n");
      break;
      case 1:
      fprintf(listing,"Type: Int\n");
      break;
      default:
      fprintf(listing,"Unknown ExpNode kind\n");
      break;
    }
    break;

    default:
    fprintf(listing,"Unknown ExpNode kind\n");
    break;
  }
}
else fprintf(listing,"Unknown node kind\n");
for (i=0;i<MAXCHILDREN;i++)
printTreeC(tree->child[i]);
tree = tree->sibling;
}
UNINDENT;
}
