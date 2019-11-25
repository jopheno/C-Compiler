/****************************************************/
/* File: cgen.c                                     */
/* The intermediate code generator implementation   */
/* for the TINY compiler                            */
/* (generates code for the Cminu[s machine)          */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "symtab.h"
#include "code.h"
#include "cgen.h"

/*************************************************************/

TypeRegTemp ArrayTemp[11];
int i;
TypeRegTemp aux;
char *lreg;
extern int location;

/*****************************************************/
/* Auxiliary Variables */
Arg aux1, aux2, aux3, aux4, lastReg;
Arg opglobal, oplocal1, oplocal2;
Lno *L;
Lno *T;
int cont = 0;
char *aux_label, *aux_temp;
Lno *backup;
Tlist LIST;
Tstack STACK;
TstackOp STACKOP;
BucketList bucketl;
char *Funct_Ant =" ";
int first_function = 0;
BucketList bucketl_Ant;
FIFOArgs ARG;     // Lista de argumentos para geração do código intermediário
Tlabel LISTLABEL; // Lista de nós de código intermediário com labels (function, L0..., goto )

/*****************************************************/
/* prototype for internal recursive code generator */
static void cGen (TreeNode * tree);

/* Procedure genStmt generates code at a statement node */

void printStmt(TreeNode * tree) {
    switch (tree->kind.stmt)
    {
        case IfK:
            printf("[IF Node]\n");
        case WhileK:
            printf("[While Node]\n");
        case AssignK:
            printf("[Assign Node]\n");
        case FunctionK:
            printf("[Function Node] {attr = {name = %s}}\n", tree->attr.name);
        case CallK:
            printf("[Call Node] {attr = {name = %s}}\n", tree->attr.name);
        case ParamK:
            printf("[Param Node]\n");
        case ReturnK:
            printf("[Return Node]\n");
        case AssemblyK:
            printf("[Assembly Node]\n");
        case NumberK:
            printf("[Number Node]\n");
    }
}

static void genStmt( TreeNode * tree)
{
    TreeNode * p1, * p2, * p3;
    Lno *saved_instruction;
    //printStmt(tree);
    switch (tree->kind.stmt)
    {

        case IfK :
            p1 = tree->child[0] ;
            p2 = tree->child[1] ;
            p3 = tree->child[2] ;
            if(p3==NULL)
            {
                /* generate code for test expression */
                cGen(p1);
                aux1.addr.string = "IF_FALSE";
                aux1.type=String;
                aux2=LIST.last->result;
                aux3.addr.string="_";
                aux3.type=String;
                aux4.addr.string="Label Undefined";
                aux4.type=String;
                L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);
                insert_Stack(&STACK,L);

                /* generate code for statemant */
                cGen(p2);
                backup=remove_Stack(&STACK);
                aux_label=newLabel();
                backup->result.addr.string=aux_label;
                backup->result.type = String;
                aux1.addr.string = "label";
                aux1.type=String;
                aux2.addr.string ="_";
                aux2.type=String;
                aux3.addr.string="_";
                aux3.type=String;
                aux4.addr.string=aux_label;
                aux4.type=String;
                L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);
            }
            else
            {
                /* generate code for test expression */
                cGen(p1);
                aux1.addr.string ="IF_FALSE";
                aux1.type=String;
                aux2=LIST.last->result;
                aux3.addr.string="_";
                aux3.type=String;
                aux4.addr.string="Label Undefined";
                aux4.type=String;
                L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);
                insert_Stack(&STACK,L);
                /* generate code for statemant */
                cGen(p2);
                saved_instruction=remove_Stack(&STACK);
                aux1.addr.string = "goto";
                aux1.type=String;
                aux2.addr.string = "_";
                aux2.type=String;
                aux3.addr.string = "_";
                aux3.type=String;
                aux4.addr.string="Label Undefined";
                aux4.type=String;
                L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);
                insert_Stack(&STACK,L);
                aux1.addr.string = "label";
                aux1.type=String;
                aux2.addr.string = "_";
                aux2.type=String;
                aux3.addr.string = "_";
                aux3.type=String;
                aux_label=newLabel();
                aux4.addr.string=aux_label;
                aux4.type=String;
                L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);
                saved_instruction->result.addr.string=aux_label;
                saved_instruction->result.type = String;
                /* generate code for else statemant */
                cGen(p3);
                aux1.addr.string = "label";
                aux1.type=String;
                aux2.addr.string = "_";
                aux2.type=String;
                aux3.addr.string = "_";
                aux3.type=String;
                aux_label=newLabel();
                aux4.addr.string=aux_label;
                aux4.type=String;
                L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);
                backup = remove_Stack(&STACK);
                backup->result.addr.string=aux_label;
                backup->result.type = String;
            }

            break; /* if_k */

        case WhileK:
            p1 = tree->child[0] ;
            p2 = tree->child[1] ;
            aux1.addr.string = "label";
            aux1.type=String;
            aux2.addr.string = "_";
            aux2.type=String;
            aux3.addr.string= "_";
            aux3.type=String;
            aux4.addr.string="Label Undefined1";
            aux4.type=String;
            L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);
            insert_Stack(&STACK,L);
            /* generate code for expression */
            cGen(p1);
            aux1.addr.string = "IF_FALSE";
            aux1.type=String;
            aux2=LIST.last->result;
            aux3.addr.string="_";
            aux3.type=String;
            aux4.addr.string="Label Undefined2";
            aux4.type=String;
            L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);
            insert_Stack(&STACK,L);

            /* generate code for statemant */
            cGen(p2);
            saved_instruction = remove_Stack(&STACK);
            aux1.addr.string = "goto";
            aux1.type=String;
            aux2.addr.string = "_";
            aux2.type=String;
            aux3.addr.string = "_";
            aux3.type=String;
            aux_label=newLabel();
            aux4.addr.string=aux_label;
            aux4.type=String;
            L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);
            backup = remove_Stack(&STACK);
            backup->result.addr.string = aux_label;
            backup->result.type = String;
            aux1.addr.string = "label";
            aux1.type=String;
            aux2.addr.string = "_";
            aux2.type=String;
            aux3.addr.string = "_";
            aux3.type=String;
            aux_label=newLabel();
            aux4.addr.string=aux_label;
            aux4.type=String;
            L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);
            saved_instruction->result.addr.string = aux_label;
            saved_instruction->result.type = String;
            break; /* while_k */

        case AssignK:

            p1 = tree->child[0];
            p2 = tree->child[1];

            if(p1->kind.exp==IdK)
            {
                if (p2->nodekind==ExpK)
                {
                    switch (p2->kind.exp)
                    {
                    case ConstK:
                        aux1.addr.string = "assign_id_const";
                        aux1.type=String;
                        aux2.addr.constant = p2->attr.val;
                        aux2.type=Constant;
                        aux3.addr.string = "_";
                        aux3.type=String;
                        aux4.addr.variable=st_lookup(p1->attr.name,p1->scope);
                        aux4.type=Bucket;
                        L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);
                        break;

                    case IdK:
                        aux1.addr.string = "assign_id_id";
                        aux1.type=String;
                        aux2.addr.variable=st_lookup(p2->attr.name,p2->scope);
                        aux2.type=Bucket;
                        aux3.addr.string = "_";
                        aux3.type=String;
                        aux4.addr.variable=st_lookup(p1->attr.name,p1->scope);
                        aux4.type=Bucket;
                        L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);

                        break;

                    case IdAddrK:
                        aux1.addr.string = "assign_id_const";
                        aux1.type=String;
                        //aux2.addr.variable=st_lookup(p2->attr.name,p2->scope)->memloc;
                        //aux2.type=Bucket;
                        aux2.addr.constant=st_lookup(p2->attr.name,p2->scope)->memloc;
                        aux2.type=ADDR_Constant;
                        aux3.addr.string = "_";
                        aux3.type=String;
                        aux4.addr.variable=st_lookup(p1->attr.name,p1->scope);
                        aux4.type=Bucket;
                        L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);

                        break;

                    case VectorIdK:
                        cGen(p2);
                        aux1.addr.string = "assign_id_a";
                        aux1.type=String;
                        aux2=LIST.last->result;
                        aux3.addr.string = "_";
                        aux3.type=String;
                        aux4.addr.variable=st_lookup(p1->attr.name,p1->scope);
                        aux4.type=Bucket;
                        L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);
                        //printf(">> [2] Here %s || %d\n", aux2.addr.string, aux2.type);
                        break;

                    default:
                        cGen(p2);
                        aux1.addr.string = "assign_id_exp";
                        aux1.type=String;
                        aux2=LIST.last->result;
                        aux3.addr.string = "_";
                        aux3.type=String;
                        aux4.addr.variable=st_lookup(p1->attr.name,p1->scope);
                        aux4.type=Bucket;
                        L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);
                        //printf(">> [1] Here %s || %d\n", aux2.addr.string, aux2.type);

                        break;
                    }
                }
                if(p2->nodekind==StmtK)
                {
                    cGen(p2);
                    aux1.addr.string = "assign_id_f";
                    aux1.type=String;
                    aux2=LIST.last->result;
                    aux3.addr.string = "_";
                    aux3.type=String;
                    aux4.addr.variable=st_lookup(p1->attr.name,p1->scope);
                    aux4.type=Bucket;
                    L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);
                }
            }

            else if (p1->kind.exp==VectorIdK)
            {
                cGen(p1);
                if (p2->nodekind==ExpK)
                {
                    switch (p2->kind.exp)
                    {
                    case ConstK:
                        aux1.addr.string = "assign_a_const";
                        aux1.type=String;
                        aux2.addr.constant = p2->attr.val;
                        aux2.type=Constant;
                        aux3.addr.string = "_";
                        aux3.type=String;
                        aux4=LIST.last->result;
                        L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);
                        break;

                    case IdK:
                        aux1.addr.string = "assign_a_id";
                        aux1.type=String;
                        aux2.addr.variable=st_lookup(p2->attr.name,p2->scope);
                        aux2.type=Bucket;
                        aux3.addr.string = "_";
                        aux3.type=String;
                        aux4=LIST.last->result;
                        L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);

                        break;

                    case IdAddrK:
                        aux1.addr.string = "assign_a_const";
                        aux1.type=String;
                        //aux2.addr.variable=st_lookup(p2->attr.name,p2->scope)->memloc;
                        //aux2.type=Bucket;
                        aux2.addr.constant = st_lookup(p2->attr.name,p2->scope)->memloc;
                        aux2.type=ADDR_Constant;
                        aux3.addr.string = "_";
                        aux3.type=String;
                        aux4=LIST.last->result;
                        L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);

                        break;

                    case VectorIdK:
                        insert_Stack(&STACK,LIST.last);
                        cGen(p2);
                        aux1.addr.string = "assign_a_a";
                        aux1.type=String;
                        aux2=LIST.last->result;
                        aux3.addr.string = "_";
                        aux3.type=String;
                        backup= remove_Stack(&STACK);
                        aux4=backup->result;
                        L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);
                        break;

                    default:
                        insert_Stack(&STACK,LIST.last);
                        cGen(p2);
                        aux1.addr.string = "assign_a_exp";
                        aux1.type=String;
                        aux2=LIST.last->result;
                        aux3.addr.string = "_";
                        aux3.type=String;
                        backup= remove_Stack(&STACK);
                        aux4=backup->result;
                        L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);

                        break;
                    }
                }
                if(p2->nodekind==StmtK)
                {
                    insert_Stack(&STACK,LIST.last);
                    cGen(p2);
                    aux1.addr.string = "assign_a_f";
                    aux1.type=String;
                    aux2=LIST.last->result;
                    aux3.addr.string = "_";
                    aux3.type=String;
                    backup= remove_Stack(&STACK);
                    aux4=backup->result;
                    L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);
                }
            }
            break; /* assign_k */

        case FunctionK:
            bucketl = st_lookup(tree->attr.name,tree->scope);
            if(first_function==0)
            {
                bucketl_Ant = st_lookup(tree->attr.name,tree->scope);
                first_function =1;
            }
            else
            {

                aux1.addr.string = "end_function";
                aux1.type=String;
                aux2.addr.string = "_";
                aux2.type=String;
                aux3.addr.string = "_";
                aux3.type=String;
                aux4.addr.variable=bucketl_Ant;
                aux4.type=Bucket;
                L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);

            }
            bucketl_Ant = bucketl;
            aux1.addr.string = "start_function";
            aux1.type=String;
            aux2.addr.string = "_";
            aux2.type=String;
            aux3.addr.string = "_";
            aux3.type=String;
            aux4.addr.variable=bucketl;
            aux4.type=Bucket;
            L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);
            cGen(tree->child[0]);
            cGen(tree->child[1]);

            break;

        case ParamK:
            p1 = tree->child[0];
            bucketl = st_lookup(tree->attr.name,tree->scope);
            if(bucketl!=NULL)
            {
                aux1.addr.string = "param";
                aux1.type=String;
                aux2.addr.string = "_";
                aux2.type=String;
                aux3.addr.string = "_";
                aux3.type=String;
                aux4.addr.variable=st_lookup(tree->attr.name,tree->scope);
                aux4.type=Bucket;
                if(p1==NULL)
                    L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);
                else
                {
                    aux1.addr.string = "param_array";
                    aux1.type=String;
                    L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);
                }
            }

            break;

        case CallK:
            lastReg = LIST.last->result;
            p1=tree->child[0];
            if(p1==NULL)
            {
                bucketl = st_lookup(tree->attr.name,tree->scope);
                if(bucketl->datatype != 0)
                {
                    aux1.addr.string = "call";
                    aux1.type=String;
                    aux2.addr.variable= bucketl;
                    aux2.type=Bucket;
                    aux3.addr.constant = 0;
                    aux3.type=Constant;
                    aux_temp = newTemp();
                    aux4.addr.string = aux_temp;
                    aux4.type = String;
                }
                else
                {
                    aux1.addr.string = "call";
                    aux1.type=String;
                    aux2.addr.variable= bucketl;
                    aux2.type=Bucket;
                    aux3.addr.constant = 0;
                    aux3.type=Constant;
                    aux4.addr.string = "void";
                    aux4.type = String;
                }

                L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);
            }

            else
            {
                int n_args = 0;
                TreeNode* p = p1;
                while(p!=NULL) {
                    n_args++;
                    p = p->sibling;
                }

                TreeNode** brothers = (TreeNode**) malloc(n_args*sizeof(TreeNode*));

                n_args = 0;
                p = p1;

                while(p!=NULL) {
                    brothers[n_args++] = p;
                    p = p->sibling;
                }

                //while(p1!=NULL)
                // Inverting arguments before calling a function.
                for(int i = n_args-1; i>=0; i--)
                {
                    p1 = brothers[i];
                    if (p1->nodekind==ExpK)
                    {
                        if(p1->kind.exp==ConstK)
                        {
                            aux1.addr.string = "arg";
                            aux1.type=String;
                            aux2.addr.string = "_";
                            aux2.type=String;
                            aux3.addr.string = "_";
                            aux3.type=String;
                            aux4.addr.constant=p1->attr.val;
                            aux4.type = Constant;
                            L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);
                        }
                        else if (p1->kind.exp==IdK)
                        {
                            bucketl = st_lookup(p1->attr.name,p1->scope);
                            if(strcmp(getStructureType(bucketl->structureid),"Array")==0 ||
                                    strcmp(getStructureType(bucketl->structureid),"Parameter Vector")==0)
                            {
                                aux1.addr.string = "arg_array";
                            }
                            else
                            {
                                aux1.addr.string = "arg";
                            }
                            aux1.type=String;
                            aux2.addr.string = "_";
                            aux2.type=String;
                            aux3.addr.string = "_";
                            aux3.type=String;
                            aux4.addr.variable = bucketl;
                            aux4.type = Bucket;
                            L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);

                        }
                        else if (p1->kind.exp==IdAddrK)
                        {
                            bucketl = st_lookup(p1->attr.name,p1->scope);

                            aux1.addr.string = "arg";
                            aux1.type=String;
                            aux2.addr.string = "_";
                            aux2.type=String;
                            aux3.addr.string = "_";
                            aux3.type=String;
                            aux4.addr.constant=bucketl->memloc;
                            aux4.type = ADDR_Constant;
                            L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);

                        }
                        else if(p1->kind.exp==VectorIdK)
                        {
                            if(p1->child[0]==NULL)
                            {
                                aux1.addr.string = "arg";
                                aux1.type=String;
                                aux2.addr.string = "_";
                                aux2.type=String;
                                aux3.addr.string = "_";
                                aux3.type=String;
                                aux4.addr.variable=st_lookup(p1->attr.name,p1->scope);
                                aux4.type = Bucket;
                                L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);

                            }
                            else
                            {
                                cGen(p1);
                                insert_Stack(&STACK,LIST.last);
                                aux1.addr.string = "arg_address";
                                aux1.type=String;
                                aux2.addr.string = "_";
                                aux2.type=String;
                                aux3.addr.string = "_";
                                aux3.type=String;
                                saved_instruction = remove_Stack(&STACK);
                                aux4=saved_instruction->result;
                                L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);
                            }
                        }
                        else if(p1->kind.exp==OpK)
                        {
                            cGen(p1);
                            insert_Stack(&STACK,LIST.last);
                            aux1.addr.string = "arg";
                            aux1.type=String;
                            aux2.addr.string = "_";
                            aux2.type=String;
                            aux3.addr.string = "_";
                            aux3.type=String;
                            saved_instruction = remove_Stack(&STACK);
                            aux4=saved_instruction->result;
                            L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);
                        }
                        //p1=p1->sibling;
                    }

                    else if(p1->nodekind==StmtK)
                    {
                        if(p1->kind.stmt==CallK )
                        {
                            cGen(p1);
                            insert_Stack(&STACK,LIST.last);
                            aux1.addr.string = "arg";
                            aux1.type=String;
                            aux2.addr.string = "_";
                            aux2.type=String;
                            aux3.addr.string = "_";
                            aux3.type=String;
                            saved_instruction = remove_Stack(&STACK);
                            aux4=saved_instruction->result;
                            L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);
                        }
                        //p1=p1->sibling;
                    }
                }

                free(brothers);

                bucketl = st_lookup(tree->attr.name,tree->scope);
                if( bucketl->datatype != 0)
                {
                    aux1.addr.string = "call";
                    aux1.type=String;
                    aux2.addr.variable=bucketl;
                    aux2.type=Bucket;
                    aux3.addr.constant = bucketl->number_args;
                    aux3.type=Constant;
                    aux_temp = newTemp();
                    aux4.addr.string = aux_temp;
                    aux4.type = String;
                }
                else
                {
                    aux1.addr.string = "call";
                    aux1.type=String;
                    aux2.addr.variable=bucketl;
                    aux2.type=Bucket;
                    aux3.addr.constant = bucketl->number_args;
                    aux3.type=Constant;
                    aux4.addr.string = "void";
                    aux4.type = String;
                }
                L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);
            }

            break;
        case AssemblyK:

            if (tree->child[0] == NULL)
                break;
            
            aux1.addr.string = "start_assembly";
            aux1.type=String;
            aux2.addr.string = "_";
            aux2.type=String;
            aux3.addr.string = "_";
            aux3.type=String;
            aux4.addr.string="_";
            aux4.type = String;
            L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);

            for (p1 = tree->child[0]; p1 != NULL; p1 = p1->sibling) {
                aux1.addr.string = "inst";
                aux1.type=String;
                aux2.addr.string = p1->attr.name;
                aux2.type=String;
                aux3.addr.string = tree->scope;
                aux3.type=String;
                aux4.addr.string="void";
                aux4.type = String;
                L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);
            }

            aux1.addr.string = "end_assembly";
            aux1.type=String;
            aux2.addr.string = "_";
            aux2.type=String;
            aux3.addr.string = "_";
            aux3.type=String;
            aux4.addr.string="_";
            aux4.type = String;
            L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);

            break; /* assembly_k */

        case ReturnK:

            p1=tree->child[0];
            if(p1==NULL)
            {
                aux1.addr.string = "return";
                aux1.type=String;
                aux2.addr.string = "_";
                aux2.type=String;
                aux3.addr.string = "_";
                aux3.type=String;
                aux4.addr.string="void";
                aux4.type = String;
                L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);
            }
            else if(p1->kind.exp==ConstK)
            {
                aux1.addr.string = "return";
                aux1.type=String;
                aux2.addr.string = "_";
                aux2.type=String;
                aux3.addr.string = "_";
                aux3.type=String;
                aux4.addr.constant=p1->attr.val;
                aux4.type = Constant;
                L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);
            }
            else if(p1->kind.exp==IdK)
            {
                aux1.addr.string = "return";
                aux1.type=String;
                aux2.addr.string = "_";
                aux2.type=String;
                aux3.addr.string = "_";
                aux3.type=String;
                aux4.addr.variable=st_lookup(p1->attr.name,p1->scope);
                aux4.type = Bucket;
                L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);
            }
            else if(p1->kind.exp==IdAddrK)
            {
                aux1.addr.string = "return";
                aux1.type=String;
                aux2.addr.string = "_";
                aux2.type=String;
                aux3.addr.string = "_";
                aux3.type=String;
                aux4.addr.constant=st_lookup(p1->attr.name,p1->scope)->memloc;
                aux4.type = ADDR_Constant;
                L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);
            }
            else if(p1->kind.exp==VectorIdK)
            {
                cGen(p1);
                aux1.addr.string = "return_array";
                aux1.type=String;
                aux2.addr.string = "_";
                aux2.type=String;
                aux3.addr.string = "_";
                aux3.type=String;
                aux4=LIST.last->result;
                L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);
            }
            else
            {
                cGen(p1);
                aux1.addr.string = "return";
                aux1.type=String;
                aux2.addr.string = "_";
                aux2.type=String;
                aux3.addr.string = "_";
                aux3.type=String;
                aux4=LIST.last->result;
                L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);
            }

            break;

        default:
            break;
    }
} /* genStmt */

void insertOP(char *op)
{
    aux1.addr.string = op;
    aux1.type=String;
    oplocal2=remove_StackOp(&STACKOP);
    oplocal1=remove_StackOp(&STACKOP);
    aux_temp = newTemp();
    aux4.addr.string = aux_temp;
    aux4.type = String;
    opglobal.addr.string = aux_temp;
    opglobal.type = String;
    insert_StackOp (&STACKOP, opglobal);
    L=insert_Node_List(&LIST, aux1, oplocal1, oplocal2, aux4);
}

/* Procedure genExp generates code at an expression node */
static void genExp( TreeNode * tree)
{
    int loc;
    TreeNode * p1, * p2;
    switch (tree->kind.exp)
    {

    case VectorIdK:
        p1=tree->child[0];
        if (p1->nodekind==ExpK)
        {
            if(p1->kind.exp==IdK)
            {
                aux1.addr.string = "vector_var";
                aux1.type=String;
                aux2.addr.variable=st_lookup(tree->attr.name,tree->scope);
                aux2.type = Bucket;
                aux3.addr.variable=st_lookup(p1->attr.name,p1->scope);
                aux3.type = Bucket;
                aux_temp = newTemp();
                aux4.addr.string = aux_temp;
                aux4.type = String;
                L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);

            }
            else if(p1->kind.exp==IdAddrK)
            {
                aux1.addr.string = "vector_const";
                aux1.type=String;
                aux2.addr.variable=st_lookup(tree->attr.name,tree->scope);
                aux2.type = Bucket;
                aux3.addr.constant=st_lookup(p1->attr.name,p1->scope)->memloc;
                aux3.type = ADDR_Constant;
                aux_temp = newTemp();
                aux4.addr.string = aux_temp;
                aux4.type = String;
                L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);

            }
            else if(p1->kind.exp==ConstK)
            {
                aux1.addr.string = "vector_const";
                aux1.type=String;
                aux2.addr.variable=st_lookup(tree->attr.name,tree->scope);
                aux2.type = Bucket;
                aux3.addr.constant = p1->attr.val;
                aux3.type=Constant;
                aux_temp = newTemp();
                aux4.addr.string = aux_temp;
                aux4.type = String;
                L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);

            }
            else
            {
                cGen(p1);
                aux1.addr.string = "vector_exp";
                aux1.type=String;
                aux2.addr.variable=st_lookup(tree->attr.name,tree->scope);
                aux2.type = Bucket;
                aux3=LIST.last->result;
                aux_temp = newTemp();
                aux4.addr.string = aux_temp;
                aux4.type = String;
                L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);

            }
        }
        else if (p1->nodekind==StmtK)
        {
            cGen(p1);
            aux1.addr.string = "vector_fun";
            aux1.type=String;
            aux2.addr.variable=st_lookup(tree->attr.name,tree->scope);
            aux2.type = Bucket;
            aux3=LIST.last->result;
            aux_temp = newTemp();
            aux4.addr.string = aux_temp;
            aux4.type = String;
            L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);
        }
        break;


    case OpK :

        p1=tree->child[0];
        p2=tree->child[1];

        if(p1->kind.exp==VectorIdK)
        {
            if(p1->child[0]->nodekind==ExpK && p1->child[0]->kind.exp==OpK)
            {
                cGen(p1);
                oplocal1=remove_StackOp(&STACKOP);
                opglobal=LIST.last->result;
                insert_StackOp (&STACKOP, opglobal);
            }
            else
            {
                cGen(p1);
                opglobal=LIST.last->result;
                insert_StackOp (&STACKOP, opglobal);
            }

            // There are some assignments that occurs inside IF structures that
            // do not add assign_id_a and compare pointers to values, in order
            // to solve this problem, this line was added !
            aux1.addr.string = "assign_id_a";
            aux1.type=String;
            aux2.addr.string = aux_temp;
            aux2.type = String;
            aux3.addr.string = "_";
            aux3.type=String;
            aux4.addr.string = aux_temp;
            aux4.type = String;
            L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);
    }
        else if(p1->kind.stmt==CallK)
        {
            cGen(p1);
            opglobal=LIST.last->result;
            insert_StackOp (&STACKOP, opglobal);
        }
        else if (p1->kind.exp==ConstK)
        {
            opglobal.addr.constant = p1->attr.val;
            opglobal.type = Constant;
            insert_StackOp (&STACKOP, opglobal);
        }
        else if(p1->kind.exp==IdK)
        {
            opglobal.addr.variable = st_lookup(p1->attr.name,p1->scope);
            opglobal.type = Bucket;
            insert_StackOp (&STACKOP, opglobal);

        }
        else if(p1->kind.exp==IdAddrK)
        {
            opglobal.addr.constant = st_lookup(p1->attr.name,p1->scope)->memloc;
            opglobal.type = ADDR_Constant;
            insert_StackOp (&STACKOP, opglobal);

        }
        else if(p1->kind.exp==OpK)
        {
            cGen(p1);
        }
        if(p2->kind.exp==VectorIdK)
        {
            if(p2->child[0]->nodekind==ExpK && p2->child[0]->kind.exp==OpK)
            {
                cGen(p2);
                oplocal1=remove_StackOp(&STACKOP);
                opglobal=LIST.last->result;
                insert_StackOp (&STACKOP, opglobal);
            }
            else
            {
                cGen(p2);
                opglobal=LIST.last->result;
                insert_StackOp (&STACKOP, opglobal);
            }

            // There are some assignments that occurs inside IF structures that
            // do not add assign_id_a and compare pointers to values, in order
            // to solve this problem, this line was added !
            
            aux1.addr.string = "assign_id_a";
            aux1.type=String;
            aux2.addr.string = aux_temp;
            aux2.type = String;
            aux3.addr.string = "_";
            aux3.type=String;
            aux4.addr.string = aux_temp;
            aux4.type = String;
            L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);
        }
        else if(p2->kind.stmt==CallK)
        {
            cGen(p2);
            opglobal=LIST.last->result;
            insert_StackOp (&STACKOP, opglobal);
        }
        else if (p2->kind.exp==ConstK)
        {
            opglobal.addr.constant = p2->attr.val;
            opglobal.type = Constant;
            insert_StackOp (&STACKOP, opglobal);
        }
        else if(p2->kind.exp==IdK)
        {
            opglobal.addr.variable = st_lookup(p2->attr.name,p2->scope);
            opglobal.type = Bucket;
            insert_StackOp (&STACKOP, opglobal);
        }
        else if(p2->kind.exp==IdAddrK)
        {
            opglobal.addr.constant = st_lookup(p2->attr.name,p2->scope)->memloc;
            opglobal.type = ADDR_Constant;
            insert_StackOp (&STACKOP, opglobal);
        }
        else if(p2->kind.exp==OpK)
        {
            cGen(p2);
        }

        switch (tree->attr.op)
        {
        case PLUS :
            insertOP("sum");
            break;
        case MINUS :
            insertOP("sub");
                    break;
        case TIMES :
        insertOP("mult");

            break;
        case OVER :
            insertOP("div");
            break;

        case LT :
            insertOP("LT");
            break;

        case LET :
            insertOP("LTEQ");
            break;

        case GT :
            insertOP("GT");
            break;

        case GET :
        insertOP ("GTEQ");
            break;

        case DIFF :
            insertOP("DIFFERENT");
            break;

        case EQ :
            insertOP("EQ");
            break;
        default:
            break;
        } /* case op */

        break; /* OpK */

    default:
        break;
    }
} /* genExp */

/* Procedure cGen recursively generates code by
 * tree traversal
 */

static void cGen( TreeNode * tree)
{
    if (tree != NULL)
    {
        switch (tree->nodekind)
        {
        case StmtK:
            genStmt(tree);
            break;
        case ExpK:
            genExp(tree);
            break;
        default:
            break;
        }
        cGen(tree->sibling);
    }
}

/************************************************************/
/**********Generate a limited number of temporaries**********/

char *new(int num)
{
    char *newt=(char*)malloc(15*sizeof(char));
    sprintf(newt, "%s%d", "t", num);
    return newt;
}

char * get_new_temp(char * regtemp, int address)
{
    for(i=1; i < 11; i++)
    {
        if(ArrayTemp[i].state == 0)
        {
            ArrayTemp[i].usedBy = regtemp;
            ArrayTemp[i].state = 1;
            ArrayTemp[i].newReg = new(i);
            ArrayTemp[i].address = address;
            return ArrayTemp[i].newReg;
        }
    }
    return NULL;
}

char * getUltimateReg()
{

    for(i=1; i > 0 ; i--)
    {
        if(ArrayTemp[i].state == 1)
        {
            return ArrayTemp[i].newReg;
        }
    }
    return NULL;
}

 TypeRegTemp  set_temp_used(char *reg)
{
    for(i=1; i<11 ; i++)
    {
        if(ArrayTemp[i].state == 1)
        {
            if(strcmp(ArrayTemp[i].usedBy, reg)==0)
            {
                ArrayTemp[i].state = 0;
                char* t_name = new(i);
                BucketList bl = st_lookup(t_name, "global");

                if (bl == NULL)
                    st_insert(t_name, VariableDeclaredSt, Integer, -1, location++, "global", 0, 0);

                return ArrayTemp[i];
            }
        }
    }
    return ArrayTemp[10];
}


void tempOp(Lno *node_IR)
{
                if(node_IR->arg1.type == String)
                {
                    aux = set_temp_used(node_IR->arg1.addr.string);
                    node_IR->arg1.addr.string = aux.newReg;
                    node_IR->arg1.address = aux.address;

                }
                if(node_IR->arg2.type == String)
                {
                    aux = set_temp_used(node_IR->arg2.addr.string);
                    node_IR->arg2.addr.string = aux.newReg;
                    node_IR->arg2.address = aux.address;
                }
                if(node_IR->result.type == String)
                {
                    node_IR->result.addr.string = get_new_temp(node_IR->result.addr.string,0);
                }

}
void generate_limited_temporaries(Tlist *list)
{
    Lno *a1, *a2;
    if(list!=NULL)
    {
        a2 = list->last;
        for( a1=list->first; a1!=a2; a1=a1->prox)
        {
            if (strcmp(a1->op.addr.string,"IF_FALSE")==0)
            {
                aux = set_temp_used(a1->arg1.addr.string);
                a1->arg1.addr.string = aux.newReg;
                a1->arg1.address = aux.address;
            }
            else if (strcmp(a1->op.addr.string,"call")==0)
            {
                if(a1->arg1.addr.variable->datatype != 0 && a1->result.type == String)
                {
                    if(strcmp(a1->result.addr.string, "void")!=0)
                        a1->result.addr.string = get_new_temp(a1->result.addr.string,0);
                }
            }
            else if (strcmp(a1->op.addr.string,"vector_var")==0)
            {
                if(a1->result.type == String)
                {
                    a1->result.addr.string = get_new_temp(a1->result.addr.string , 0);
                }
            }
            else if (strcmp(a1->op.addr.string,"vector_const")==0)
            {
                if(a1->result.type == String)
                {
                    a1->result.addr.string = get_new_temp(a1->result.addr.string , 0);
                }
            }
            else if (strcmp(a1->op.addr.string,"vector_exp")==0)
            {
                if(a1->arg2.type == String)
                {
                    aux = set_temp_used(a1->arg2.addr.string);
                    a1->arg2.addr.string = aux.newReg;
                    a1->arg2.address = aux.address;
                }

                if(a1->result.type == String)
                {
                    a1->result.addr.string = get_new_temp(a1->result.addr.string , 0);
                }
            }
            else if (strcmp(a1->op.addr.string,"vector_fun")==0)
            {
                if(a1->arg2.type == String)
                {
                    aux = set_temp_used(a1->arg2.addr.string);
                    a1->arg2.addr.string = aux.newReg;
                    a1->arg2.address = aux.address;
                }

                if(a1->result.type == String)
                {
                    a1->result.addr.string = get_new_temp(a1->result.addr.string , 0);
                }
            }
            else if (strcmp(a1->op.addr.string,"assign_id_const")==0)
            {
                if(a1->arg1.type == String)
                {
                    aux = set_temp_used(a1->arg1.addr.string);
                    a1->arg1.addr.string = aux.newReg;
                    a1->arg1.address = aux.address;
                }
                if(a1->result.type == String)
                {
                    a1->result.addr.string = get_new_temp(a1->result.addr.string,0);
                }
            }
            else if (strcmp(a1->op.addr.string,"assign_id_id")==0)
            {
                if(a1->arg1.type == String)
                {
                    aux = set_temp_used(a1->arg1.addr.string);;
                    a1->arg1.addr.string = aux.newReg;
                    a1->arg1.address = aux.address;
                }
                if(a1->result.type == String)
                {
                    a1->result.addr.string = get_new_temp(a1->result.addr.string, 0);
                }

            }
            else if (strcmp(a1->op.addr.string,"assign_id_a")==0)
            {
                if(a1->arg1.type == String)
                {
                    aux = set_temp_used(a1->arg1.addr.string);
                    // Aqui estava o problema !!!
                    a1->arg1.addr.string = aux.newReg;
                    a1->arg1.address = aux.address;
                }
                if(a1->result.type == String)
                {
                    a1->result.addr.string = get_new_temp(a1->result.addr.string , 0);
                }
            }
            else if (strcmp(a1->op.addr.string,"assign_id_exp")==0)
            {
                if(a1->arg1.type == String)
                {
                    aux = set_temp_used(a1->arg1.addr.string);
                    a1->arg1.addr.string = aux.newReg;
                    a1->arg1.address = aux.address;
                }
                if(a1->result.type == String)
                {
                    a1->result.addr.string = get_new_temp(a1->result.addr.string, 0);
                }

            }
            else if (strcmp(a1->op.addr.string,"assign_id_f")==0)
            {
                if(a1->arg1.type == String)
                {
                    aux = set_temp_used(a1->arg1.addr.string);
                    a1->arg1.addr.string = aux.newReg;
                    a1->arg1.address = aux.address;
                }
                if(a1->result.type == String)
                {
                    a1->result.addr.string = get_new_temp(a1->result.addr.string,0);
                }

            }
            else if (strcmp(a1->op.addr.string,"assign_a_const")==0)
            {
                if(a1->arg1.type == String)
                {
                    aux = set_temp_used(a1->arg1.addr.string);
                    a1->arg1.addr.string = aux.newReg;
                    a1->arg1.address = aux.address;

                }
                if(a1->result.type == String)
                {
                    aux = set_temp_used(a1->result.addr.string);
                    a1->result.addr.string = aux.newReg;
                    a1->result.address = aux.address;
                }

            }
            else if (strcmp(a1->op.addr.string,"assign_a_id")==0)
            {
                if(a1->arg1.type == String)
                {
                    aux = set_temp_used(a1->arg1.addr.string);
                    a1->arg1.addr.string = aux.newReg;
                    a1->arg1.address = aux.address;
                }
                if(a1->result.type == String)
                {
                    aux = set_temp_used(a1->result.addr.string);
                    a1->result.addr.string = aux.newReg;
                    a1->result.address = aux.address;
                }

            }
            else if (strcmp(a1->op.addr.string,"assign_a_a")==0)
            {
                if(a1->arg1.type == String)
                {
                    aux = set_temp_used(a1->arg1.addr.string);
                    a1->arg1.addr.string = aux.newReg;
                    a1->arg1.address = aux.address;
                }
                if(a1->result.type == String)
                {
                    aux = set_temp_used(a1->result.addr.string);
                    a1->result.addr.string = aux.newReg;
                    a1->result.address = aux.address;
                }

            }
            else if (strcmp(a1->op.addr.string,"assign_a_exp")==0)
            {
                if(a1->arg1.type == String)
                {
                    aux = set_temp_used(a1->arg1.addr.string);
                    a1->arg1.addr.string = aux.newReg;
                    a1->arg1.address = aux.address;
                }
                if(a1->result.type == String)
                {
                    aux = set_temp_used(a1->result.addr.string);
                    a1->result.addr.string = aux.newReg;
                    a1->result.address = aux.address;
                }

            }
            else if (strcmp(a1->op.addr.string,"assign_a_f")==0)
            {
                if(a1->arg1.type == String)
                {
                    aux = set_temp_used(a1->arg1.addr.string);
                    a1->arg1.addr.string = aux.newReg;
                    a1->arg1.address = aux.address;
                }
                if(a1->result.type == String)
                {
                    aux = set_temp_used(a1->result.addr.string);
                    a1->result.addr.string = aux.newReg;
                    a1->arg1.address = aux.address;
                }

            }
            else if (strcmp(a1->op.addr.string,"arg")==0)
            {
                if(a1->result.type == String)
                {
                    if(strcmp(a1->result.addr.string, "void")!=0)
                        aux = set_temp_used(a1->result.addr.string);
                        a1->result.addr.string = aux.newReg;
                        a1->result.address = aux.address;
                }
            }
            else if (strcmp(a1->op.addr.string,"arg_address")==0)
            {
                aux = set_temp_used(a1->result.addr.string);
                a1->result.addr.string = aux.newReg;
                a1->result.address = aux.address;
            }
            else if (strcmp(a1->op.addr.string,"return")==0)
            {
                if(a1->result.type == String)
                {
                    if(strcmp(a1->result.addr.string,"void")!=0) {
                        aux = set_temp_used(a1->result.addr.string);
                        a1->result.addr.string = aux.newReg;
                        a1->result.address = aux.address;
                    }
                }
            }
            else if (strcmp(a1->op.addr.string,"return_array")==0)
            {
                if(a1->result.type == String)
                {
                    aux = set_temp_used(a1->result.addr.string);
                    a1->result.addr.string = aux.newReg;
                    a1->result.address = aux.address;
                }
            }
            else if (strcmp(a1->op.addr.string,"array_var")==0)
            {
                if(a1->result.type == String)
                {
                    a1->result.addr.string = get_new_temp(a1->result.addr.string,1);
                }
            }
            else if (strcmp(a1->op.addr.string,"array_const")==0)
            {
                if(a1->result.type == String)
                {
                    a1->result.addr.string = get_new_temp(a1->result.addr.string, 1);
                }
            }
            else if (strcmp(a1->op.addr.string,"array_exp")==0)
            {
                if(a1->arg2.type == String)
                {
                    aux = set_temp_used(a1->arg2.addr.string);
                    a1->arg2.addr.string = aux.newReg;
                    a1->arg2.address = aux.address;
                }
                if(a1->result.type == String)
                {
                    a1->result.addr.string = get_new_temp(a1->result.addr.string, 1);
                }
            }
            else if (strcmp(a1->op.addr.string,"array_fun")==0)
            {
                if(a1->arg2.type == String)
                {
                    aux = set_temp_used(a1->arg2.addr.string);
                    a1->arg2.addr.string = aux.newReg;
                    a1->arg2.address = aux.address;
                }
                if(a1->result.type == String)
                {
                    a1->result.addr.string = get_new_temp(a1->result.addr.string,1);
                }
            }
            else if ((strcmp(a1->op.addr.string,"sum")==0 || (strcmp(a1->op.addr.string,"sub")==0 ||
                     strcmp(a1->op.addr.string,"mult")==0 || strcmp(a1->op.addr.string,"div")==0)) ||
                     (strcmp(a1->op.addr.string,"LT")==0 || strcmp(a1->op.addr.string,"LTEQ")==0 ||
                     strcmp(a1->op.addr.string,"GT")==0 || strcmp(a1->op.addr.string,"GTEQ")==0 ||
                     strcmp(a1->op.addr.string,"DIFFERENT")==0 || strcmp(a1->op.addr.string,"EQ")==0))
            {
                tempOp(a1);
            }

        }
    }

}
/**********************************************/
/* the primary function of the code generator */
/**********************************************/
/* Procedure codeGen generates code to a code
 * file by traversal of the syntax tree. The
 * second parameter (codefile) is the file name
 * of the code file, and is used to print the
 * file name as a comment in the code file
 */

void codeGen(TreeNode * syntaxTree, FILE* output)
{

    /* Inicialize list of nodes for intermediate code generation */
    inicialize_List(&LIST);
    /* Inicialize list of labels for intermediate code generation */
    inicialize_Stack (&STACK);
    /* Inicialize stack of operands for intermediate code generation */
    inicialize_Stack_Op (&STACKOP);
    /* Inicialize list of arguments for machine code generation */
    inicialize_FIFO_Args(&ARG);
    /* Inicialize list for machine code generation */
    //inicialize_Code_List(&MCLIST);
    /* Inicialize list of labels for machine code generation jumps*/
    //inicialize_List_Label(&LISTLABEL);
    /* generate code for C minus program */
    //inicialize processor through function main
    
    // The Sythesis Module will be responsible for adding call to the main function.

    /*
        aux1.addr.string = "call";
        aux1.type=String;
        aux2.addr.variable=st_lookup("main", "global");
        aux2.type=Bucket;
        aux3.addr.constant = 0;
        aux3.type=Constant;
        aux4.addr.string = "void";
        aux4.type = String;
        L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);

    */

    /*************************************************************/

    cGen(syntaxTree);

    // Add end function for function main
        aux1.addr.string = "end_function";
        aux1.type=String;
        aux2.addr.string = "_";
        aux2.type = String;
        aux3.addr.string = "_";
        aux3.type = String;
        aux4.addr.variable = st_lookup("main", "global");
        aux4.type = Bucket;
        L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);

    /*************************************************************/
        /*aux1.addr.string = "halt";
        aux1.type=String;
        aux2.addr.string="_";
        aux2.type=String;
        aux3.addr.string = "_";
        aux3.type=String;
        aux4.addr.string = "_";
        aux4.type = String;
        L=insert_Node_List(&LIST, aux1, aux2, aux3, aux4);*/
    /*************************************************************/
    /* Incialize states for allocation register machine code generation */
    for(i=1; i<11; i++)
    {
        ArrayTemp[i].state = 0;
    }
    /*Generate new limmited temporararies for code machine generation*/
    generate_limited_temporaries(&LIST);

    /*Print nodes from Intermediate Code*/
    fprintf(output, "\n%s\n\n","******** INTERMEDIATE CODE **********");
    fprint_Node_List(output, &LIST);
    fprintf(output, "\n");

    //print_List_Label(&LISTLABEL);
    /* finish */
}
