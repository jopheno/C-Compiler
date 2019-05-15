/****************************************************/
/* File: analyze.c                                  */
/* Semantic analyzer implementation                 */
/* for the Cminus compiler                          */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "symtab.h"
#include "analyze.h"

/* counter for variable memory locations */
int location = 0;
static char *scope ="global";
static int function_main = 0;
static int search_for_main = 1;
int typeVoid;
int returnVoid;
int add_in_out = 0;
BucketList bl;
BucketList funct_arg;
static void typeError(TreeNode * t, char * message);


/* Procedure traverse is a generic recursive
 * syntax tree traversal routine:
 * it applies preProc in preorder and postProc
 * in postorder to tree pointed to by t
 */
static void traverse( TreeNode * t,
                      void (* preProc) (TreeNode *),
                      void (* postProc) (TreeNode *) )
{
    if (t != NULL)
    {
        preProc(t);
        {
            int i;
            for (i=0; i < MAXCHILDREN; i++)
                traverse(t->child[i],preProc,postProc);
        }
        postProc(t);
        traverse(t->sibling,preProc,postProc);
    }
}

int datatype (TreeNode * t)
{
    if(t->type==Void) return 0;
    if(t->type==Integer) return 1;
    else return 2;
}

/* nullProc is a do-nothing procedure to
 * generate preorder-only or postorder-only
 * traversals from traverse
 */
static void nullProc(TreeNode * t)
{
    if (t==NULL) return;
    else return;
}

void insert_input_output()
{
   st_insert("checkForInputs", FunctionSt, 1, -1, -2, "global",0,0);
   st_insert("input", FunctionSt, 1, -1, -2, "global",0,0);
   st_insert("output", FunctionSt, 0 , -1, -2, "global",1,0);
}

/* Procedure insertNode inserts
 * identifiers stored in t into
 * the symbol table
 */
static void insertNode( TreeNode * t )
{
    if(add_in_out==0)
    {
        insert_input_output();
        add_in_out = 1;
    }
    switch (t->nodekind)
    {
    case StmtK:
        switch (t->kind.stmt)
        {
        case FunctionK:
            if(strcmp(t->attr.name,"main")==0)
                function_main = 1;
            bl = st_lookup(t->attr.name,"global");
            if (bl == NULL)
            {
                t->scope=scope;
                /* not yet in table, so treat as new definition */
                    returnVoid = datatype(t);
                    if(returnVoid==0){
                        st_insert(t->attr.name, FunctionSt, datatype(t), t->line_number, location++, "global",0, 0);
                     }
                     else{
                        st_insert(t->attr.name, FunctionSt, datatype(t), t->line_number, location, "global",0, 0);
                        location+=2;
                     }
                scope=t->attr.name;
                t->scope=scope;
            }
            else
                /* already in table, so error message must to be send */
                typeError(t,"Function already declared or yours name was used by other structure");
            break;

        case ParamK:
            bl = st_lookup(t->attr.name,scope);
            funct_arg = st_lookup(scope, scope);
            typeVoid = datatype(t);

            if (funct_arg != NULL && typeVoid!=0)
                funct_arg->number_args++;

            t->scope=scope;
            if (bl == NULL && typeVoid!=0)
            {
                if(t->child[0]==NULL){
                    /* not yet in table, so treat as new definition */
                    st_insert(t->attr.name, ParameterSt, datatype(t), t->line_number, location++, scope, 0, 0);
                }
                else{
                    /* not yet in table, so treat as new definition of vector */
                    st_insert(t->attr.name, VectorParameterSt, datatype(t), t->line_number, location++, scope, 0, 1);
                }
            }
            else if(typeVoid!=0)
            {
                /* already in table, so error message must to be send */
                typeError(t,"Parameter already declared");
            }
            break;

        case CallK:
            t->scope=scope;
            bl = st_lookup(t->attr.name,scope);
            if (bl == NULL)
                /* not in table, so message error must to be  */
                typeError(t,"Function must be declared first");
            else
            {
                /* already in table*/
                st_insert(t->attr.name, CallFunctionSt, datatype(t), t->line_number, -2, scope, 0, 0);
                t->type = bl->datatype;
            }
            break;

        default:
            break;
        }
        break;

    case ExpK:
        switch (t->kind.exp)
        {
        case IdK:
            t->scope=scope;
            bl = st_lookup(t->attr.name,scope);
            if (bl == NULL)
                /* not yet in table, so message error must be print*/
                typeError(t,"Variable used must be declared first");
            else
            {
                /*already in table, so ignore location,
                   add line number of use only */
                st_insert(t->attr.name, VariableUsedSt, datatype(t), t->line_number, -2, scope,0, 0);
                t->type = bl->datatype;
            }

            break;

        case VariableK:
            t->scope=scope;
            bl = st_lookup(t->attr.name,scope);
            if (bl == NULL)
                /* not yet in table, so treat as new definition */
                if (datatype(t) == 0) {
                  typeError(t,"Variable must be an Integer");
                } else {
                  st_insert(t->attr.name, VariableDeclaredSt, datatype(t), t->line_number, location++, scope, 0, 0);
                }
            else {
                /*already in table, so error message must be print */

                if (bl->structureid == FunctionSt) {
                  typeError(t,"Variable already declared as a function name");
                }
                else {
                  typeError(t,"Variable already declared");
                }
            }
            break;

        case VectorK:
            t->scope=scope;
            if (st_lookup(t->attr.name,scope) == NULL){
                /* not yet in table, so treat as new definition */
                st_insert(t->attr.name, VectorDeclaredSt, datatype(t), t->line_number, location, scope, 0, 0);
              location += t->child[0]->attr.val;}
            else
                /* already in table, so message error must be print */
                typeError(t,"Array already declared");
            break;
        case VectorIdK:
            t->scope=scope;
            bl = st_lookup(t->attr.name,scope);
            if ( bl == NULL)
                /* not in table, so message error must to be  */
                typeError(t,"Array must be declared first");
            else
            {
                /* already in table*/
                st_insert(t->attr.name, VectorUsedSt, datatype(t), t->line_number, -2, scope, 0, 0);
                t->type = bl->datatype;
            }
            break;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

/* Function buildSymtab constructs the symbol
 * table by preorder traversal of the syntax tree
 */
void buildSymtab(TreeNode * syntaxTree)
{
    traverse(syntaxTree,insertNode,nullProc);
    if (TraceAnalyze)
    {
        fprintf(listing,"\nSymbol table:\n\n");
        printSymTab(listing);
    }
}

static void typeError(TreeNode * t, char * message)
{
    if(t!=NULL)
    {
        fprintf(listing,"Type error at line %d: %s\n",t->line_number,message);
        Error = TRUE;
    }
}

/* Procedure checkNode performs
 * type checking at a single tree node
 */
static void checkNode(TreeNode * t)
{

    if(search_for_main==1 && function_main==0)
    {
        typeError(t,"Function main not declared");
        search_for_main=0;
    }
    switch (t->nodekind)
    {
    case ExpK:
        switch (t->kind.exp)
        {
        case OpK:

            if ((t->attr.op == EQ) || (t->attr.op == LT) || (t->attr.op == LET) || (t->attr.op == GT) || (t->attr.op == GET) || (t->attr.op == DIFF))
                t->type = Boolean;
            else
                t->type = Integer;
            break;
        case ConstK:
            if(t->type != Integer)
                typeError(t,"Data type VOID detected in constant value");
            break;
        case VariableK:
            if(t->type != Integer)
                typeError(t,"Data type VOID detected in variable declared");
            break;
        case VectorK:
            if(t->type != Integer)
                typeError(t,"Data type VOID detected in array declared");
            break;
        default:
            break;
        }
        break;
    case StmtK:
        switch (t->kind.stmt)
        {
        case IfK:
            if (t->child[0]->type != Boolean)
                typeError(t->child[0],"IF test is not Boolean");
            break;

        case AssignK:

            //printf(":: %s | %s ::\n", t->child[0]->attr.name, t->child[0]->scope);
            bl = st_lookup(t->child[0]->attr.name, t->child[0]->scope);

            if (bl == NULL)
                break;


            if (t->child[1] && t->child[1]->nodekind == StmtK && t->child[1]->kind.stmt != OpK) {
                //printf(":: [%d] %s | %s ::\n", t->line_number, t->child[1]->attr.name, t->child[1]->scope);

                bl = st_lookup(t->child[1]->attr.name, t->child[1]->scope);

                if (bl == NULL)
                    break;
            }

            //if (t->child[1])
            //    printf(":: %s | %s ::\n", t->child[1]->attr.name, "global");

            //bl = st_lookup(t->child[1]->attr.name, t->child[1]->scope);

            //if (bl == NULL)
                //break;

            if (t->child[0]->type != Integer)
                typeError(t->child[0],"Assignment of non-integer value");

            if (t->child[0]->type != t->child[1]->type)
                typeError(t->child[0],"Invalid assignment, divergent data type");
            break;

        case WhileK:
            if (t->child[0]->type != Boolean)
                typeError(t->child[0],"WHILE test is not Boolean");
            break;

        default:
            break;
        }
        break;
    default:
        break;

    }
}

/* Procedure typeCheck performs type checking
 * by a postorder syntax tree traversal
 */
void typeCheck(TreeNode * syntaxTree)
{
    traverse(syntaxTree,nullProc,checkNode);
}
