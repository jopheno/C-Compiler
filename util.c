/****************************************************/
/* File: util.c                                     */
/* Utility function implementation                  */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"

/* Procedure printToken prints a token
* and its lexeme to the listing file
*/

extern int lines_number;

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
        default:
          fprintf(listing,"Unknown ExpNode kind\n");
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
      default:
      fprintf(listing,"Unknown ExpNode kind\n");
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
    case VectorIdK:
    fprintf(listing,"Array Used: %s\n",tree->attr.name);
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
