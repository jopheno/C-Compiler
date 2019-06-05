/****************************************************/
/* File: code.c                                     */
/* Functions for generate code machine and IR       */
/* implementation for the Cminus compiler           */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "code.h"

/**Counters used for create labels, parameters and temporaries*****************/
int l=-1;
int p=-1;
int t=-1;

/******** Generate allocation saved register from Intermediate Code ***********/
void inicialize_List_Label(Tlabel *listLabel)
{
    listLabel->init=NULL;
    listLabel->end=NULL;
    listLabel->length=0;
}

void insert_List_Label(Tlabel *listLabel, char *label, int memoryloc)
{

    nodeLabel * no = (nodeLabel*)malloc(sizeof(nodeLabel));

    if(listLabel->init==NULL && listLabel->end==NULL)
    {
        no->prox = NULL;
        listLabel->init = no;
        listLabel->end = no;
        no->label_IR = label;
        no->memLoc = memoryloc;
        no->addrfinalfunct = 0;
        listLabel->length++;
    }
    else
    {
        no->prox = NULL;
        listLabel->end->prox = no;
        listLabel->end = no;
        no->label_IR = label;
        no->memLoc = memoryloc;
        no->addrfinalfunct = 0;
        listLabel->length++;
    }
}

void print_List_Label(Tlabel *listLabel)
{
    nodeLabel *aux;
    if(listLabel!=NULL)
    {
        for(aux =listLabel->init; aux!=NULL; aux=aux->prox)
        {
            printf("%s %d    %d\n",aux->label_IR, aux->memLoc, aux->addrfinalfunct);
        }
    }
}

nodeLabel * search_List_Label(Tlabel *listLabel, char *label)
{
    nodeLabel *p;
    if(listLabel->init!=NULL && listLabel->end!=NULL)
    {
        for(p=listLabel->init; p!=NULL; p=p->prox)
        {
            if(strcmp(p->label_IR, label)==0)
            {
                return p;
            }
        }
    }
    return NULL;
}


/*Inicialize, insert and remove functions for FIFO of Args for Code Machine Generation */
void inicialize_FIFO_Args(FIFOArgs *arg)
{
    arg->init = NULL;
    arg->end = NULL;
    arg->length = 0;
}

void insert_FIFO_Args(FIFOArgs *arg, Lno *nodeIR)
{
    nodeArgs * no = (nodeArgs*)malloc(sizeof(nodeArgs));

    if(arg->init==NULL && arg->end==NULL)
    {
        arg->init = no;
        arg->end = no;
        no->nodeIR = nodeIR;
        arg->length++;
        no->num = arg->length;
        no->prox = NULL;
    }
    else
    {
        arg->end->prox = no;
        arg->end = no;
        no->nodeIR = nodeIR;
        arg->length++;
        no->num = arg->length;
        no->prox = NULL;
    }
}

Lno * remove_FIFO_Args(FIFOArgs *arg)
{
    nodeArgs *temp;
    Lno *node;
    if(arg->init==arg->end &&  arg->init!=NULL)
    {
        temp=arg->init;
        node=arg->init->nodeIR;
        arg->end=NULL;
        arg->init=NULL;
        return node;
    }

    else
    {
        temp=arg->init;
        node=arg->init->nodeIR;
        arg->init=temp->prox;
        return node;
    }
    return NULL;
}

/***************************************************************************/

/******** Inicialize List of Nodes for Intermediate Code ***********/
/******** Function for Insertion in the List ***********************/

void inicialize_List(Tlist *list)
{
    list->first=NULL;
    list->last=NULL;
    list->length=0;

}

Lno * insert_Node_List(Tlist* list, Arg a, Arg b, Arg c, Arg d)
{
    Lno* new = (Lno*)malloc(sizeof(Lno));

    if(list->first  == NULL && list->last == NULL)
    {
        new->ant = new;
        new->prox = new;
        list->first = new;
        list->last = new;
        new->op.addr=a.addr;
        new->op.type= a.type;
        new->op.address = 0;
        new->arg1.addr=b.addr;
        new->arg1.type= b.type;
        new->arg1.address = 0;
        new->arg2.addr=c.addr;
        new->arg2.type= c.type;
        new->arg2.address = 0;
        new->result.addr=d.addr;
        new->result.type= d.type;
        new->result.address = 0;
        list->length++;
        new->id = list->length;
        new->number_instm = 0;

    }
    else
    {
        new->prox = list->first;
        list->first->ant = new;
        new->ant = list->last;
        list->last->prox = new;
        list->last = list->last->prox;
        new->op.addr=a.addr;
        new->op.type= a.type;
        new->op.address = 0;
        new->arg1.addr=b.addr;
        new->arg1.type= b.type;
        new->arg1.address = 0;
        new->arg2.addr=c.addr;
        new->arg2.type= c.type;
        new->arg2.address = 0;
        new->result.addr=d.addr;
        new->result.type= d.type;
        new->result.address = 0;
        list->length++;
        new->id = list->length;
        new->number_instm = 0;

    }

    return new;
}


void fprint_Node_List(FILE* file, Tlist * list)
{
    Lno *p1, *p2;
    fprintf(file, "%i \n", list->length);
    if(list!=NULL)
    {
        p2=list->last;
        for( p1=list->first; p1!=p2; p1=p1->prox)
        {
            fprintf(file, "[%d] ", p1->id);
            switch (p1->op.type)
            {
            case 0:
                if(p1->op.addr.variable!=NULL)
                {
                    fprintf(file, "(var=%s ,", p1->op.addr.variable->name);
                }
                break;
            case 1:
                fprintf(file, "(str=%s ,", p1->op.addr.string);
                break;
            case 2:
                fprintf(file, "(const=%d ,", p1->op.addr.constant);
                break;
            default:
                fprintf(file, "(err=%s ,", "ERROR");
                break;
            }
            switch (p1->arg1.type)
            {
            case 0:
                if(p1->arg1.addr.variable!=NULL)
                {
                    fprintf(file, "var=%s ,", p1->arg1.addr.variable->name);
                }
                break;
            case 1:
                fprintf(file, "str=%s ,", p1->arg1.addr.string, p1->arg1.address);
                break;
            case 2:
                fprintf(file, "const=%d ,", p1->arg1.addr.constant);
                break;
            default:
                fprintf(file, "err=%s ,", "ERROR");
                break;
            }
            switch (p1->arg2.type)
            {
            case 0:
                if(p1->arg2.addr.variable!=NULL)
                {
                    fprintf(file, "var=%s ,", p1->arg2.addr.variable->name);
                }
                break;
            case 1:
                fprintf(file, "str=%s ,", p1->arg2.addr.string);
                break;
            case 2:
                fprintf(file, "const=%d ,", p1->arg2.addr.constant);
                break;
            default:
                fprintf(file, "err=%s ,", "ERROR");
                break;
            }
            switch (p1->result.type)
            {
            case 0:
                if(p1->result.addr.variable!=NULL)
                {
                    fprintf(file, "var=%s )\n", p1->result.addr.variable->name);
                }
                break;
            case 1:
                fprintf(file, "str=%s )\n", p1->result.addr.string);
                break;
            case 2:
                fprintf(file, "const=%d )\n", p1->result.addr.constant);
                break;
            default:
                fprintf(file, "err=%s )\n", "ERROR");
                break;
            }

        }

        /*Print list->last */
        if(p2!=NULL)
        {
            fprintf(file, "[%d] ", p2->id);
            switch (p2->op.type)
            {
            case 0:
                if(p2->op.addr.variable!=NULL)
                {
                    fprintf(file, "(%s ,", p2->op.addr.variable->name);
                }
                break;
            case 1:
                fprintf(file, "(%s ,", p2->op.addr.string);
                break;
            case 2:
                fprintf(file, "(%d ,", p2->op.addr.constant);
                break;
            default:
                fprintf(file, "(%s ,", "ERROR");
                break;
            }
            switch (p2->arg1.type)
            {
            case 0:
                if(p2->arg1.addr.variable!=NULL)
                {
                    fprintf(file, "%s ,", p2->arg1.addr.variable->name);
                }
                break;
            case 1:
                fprintf(file, "%s ,", p2->arg1.addr.string);
                break;
            case 2:
                fprintf(file, "%d ,", p2->arg1.addr.constant);
                break;
            default:
                fprintf(file, "%s ,", "ERROR");
                break;
            }
            switch (p2->arg2.type)
            {
            case 0:
                if(p2->arg2.addr.variable!=NULL)
                {
                    fprintf(file, "%s ,", p2->arg2.addr.variable->name);
                }
                break;
            case 1:
                fprintf(file, "%s ,", p2->arg2.addr.string);
                break;
            case 2:
                fprintf(file, "%d ,", p2->arg2.addr.constant);
                break;
            default:
                fprintf(file, "%s ,", "ERROR");
                break;
            }
            switch (p2->result.type)
            {
            case 0:
                if(p2->result.addr.variable!=NULL)
                {
                    fprintf(file, "%s )\n", p2->result.addr.variable->name);
                }
                break;
            case 1:
                fprintf(file, "%s )\n", p2->result.addr.string);
                break;
            case 2:
                fprintf(file, "%d )\n", p2->result.addr.constant);
                break;
            default:
                fprintf(file, "%s )\n", "ERROR");
                break;
            }
        }
    }
}

/*********************************************************************************/

/*********** Functions for create new labels and new temporaries variables *******/
char * newLabel()
{
    l++;
    char *str=(char*)malloc(15*sizeof(char));
    sprintf(str, "%s%d", "L", l);
    return str;
}

char * newTemp()
{
    t++;
    char *T=(char*)malloc(15*sizeof(char));
    sprintf(T, "%s%d", "t", t);
    return T;
}
char * NameReg(int Reg)
{
    char *R=(char*)malloc(15*sizeof(char));
    sprintf(R, "%s%d", "R", Reg);
    return R;
}
char * newParam()
{
    p++;
    char *P=(char*)malloc(15*sizeof(char));
    sprintf(P, "%s%d", "Param", p);
    return P;
}

/*********************************************************************************/

/******** Inicialize Stack of Labels for Intermediate Code ***********/
/******** Function for Insertion in the Stack ; Incialization and remove ******/

void inicialize_Stack (Tstack *stack)
{
    stack->top=NULL;
}

void insert_Stack (Tstack *stack, Lno *label)
{
    Sno *new_no;
    new_no = (Sno*)malloc(sizeof(Sno));
    new_no->label=label;
    if(stack->top==NULL)
    {
        stack->top=new_no;
        new_no->prox=NULL;
    }
    new_no->prox=stack->top;
    stack->top=new_no;
}

Lno *remove_Stack(Tstack *stack)
{
    Sno *temp;
    Lno *aux;
    if(stack->top==NULL)
        return NULL;
    else
    {
        temp=stack->top;
        stack->top=temp->prox;
        aux=temp->label;
        free(temp);
        return aux;
    }
}

/******** Inicialize Stack of operands for Arithmetic Expressions ***********/
/******** Function for Insertion in the Stack ; Incialization and remove ******/
void inicialize_Stack_Op (TstackOp *stackOp)
{
    stackOp->top=NULL;

}

void insert_StackOp (TstackOp *stackOp, Arg operand)
{

    SOp *new_op;
    new_op = (SOp*)malloc(sizeof(SOp));
    new_op->op=operand;
    if(stackOp->top==NULL)
    {
        stackOp->top=new_op;
        new_op->prox=NULL;
    }
    new_op->prox=stackOp->top;
    stackOp->top=new_op;
}

Arg remove_StackOp(TstackOp *stackOp)
{
    SOp *tempy;
    Arg aux;
    if(stackOp->top==NULL)
    {
        aux.addr.string = NULL;
        aux.type = String;
        return aux;
    }
    else
    {
        tempy=stackOp->top;
        stackOp->top=tempy->prox;
        aux=tempy->op;
        return aux;
    }
}


/**************************************************************************/
void emitComment( char * c )
{
    if (TraceCode) fprintf(code,"* %s\n",c);
}
