#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "assembly.h"
#include "symtab.h"

// There is only four registers
#define WD_SIZE 22
#define INST_AMOUNT 1024
#define REGISTERS_AMOUNT 4

j_reg_manager_t* regm;

j_inst_t* header_inst;
j_inst_t* first_init_inst;
j_inst_t* last_init_inst;
j_inst_t* first_inst;
j_inst_t* last_inst;

j_reg_t* false_reg;
j_reg_t* true_reg;

FILE* ja_file;

extern regex_t _temporary_regex;


// Current line number
int curr;
bool to_init = false;

// For debugging purposes
char* curr_instr;

void add_instr();
void print_instr(j_inst_t* inst);
void import_IO();

// Initializing all the workspace.

void init_inst() {
    // Setting current line to zero !
    curr = 0;

    // Creating header instruction !
    header_inst = (j_inst_t*) malloc(sizeof(j_inst_t));
    header_inst->prox = NULL;

    header_inst->line_number = curr++;

    header_inst->type = PRG;
    header_inst->func = (char*) malloc(sizeof(char)*WD_SIZE);
    strcpy(header_inst->func, "NOP");
    header_inst->first = NULL;
    header_inst->second = NULL;
    header_inst->third = NULL;

    header_inst->label = "SYS_INIT";

    first_init_inst = NULL;
    last_init_inst = NULL;

    // Its a PRG_CALLi main

    j_inst_t* main_call_inst = (j_inst_t*) malloc(sizeof(j_inst_t));
    j_arg_t* main_label_arg = (j_arg_t*) malloc(sizeof(j_arg_t));
    main_label_arg->type = LABEL;
    main_label_arg->data.label = "main";

    main_call_inst->type = PRG;
    main_call_inst->func = (char*) malloc(sizeof(char)*WD_SIZE);
    strcpy(main_call_inst->func, "CALLi");
    main_call_inst->first = main_label_arg;
    main_call_inst->second = NULL;
    main_call_inst->third = NULL;

    main_call_inst->label = "START";

    first_inst = main_call_inst;

    last_inst = main_call_inst;

    add_instr(PRG, "HALT", NULL, NULL, NULL, NULL);

    import_IO();

}

void init_reg(j_reg_manager_t* regg, char* reg_name, j_reg_code_t code, char* var_name, char* var_scope, bool freed) {

    regg->reg.name = (char*) malloc(sizeof(char)*3);
    regg->name = (char*) malloc(sizeof(char)*WD_SIZE);
    regg->scope = (char*) malloc(sizeof(char)*WD_SIZE);

    strcpy(regg->reg.name, reg_name);
    strcpy(regg->name, var_name);
    strcpy(regg->scope, var_scope);
    regg->reg.code = code;
    regg->reg.memloc = -1;
    regg->reg.ref = false;
    regg->reg.pointer = false;
    regg->backup = true;
    regg->freed = freed;
    regg->locked = false;
}

void init_regm() {
    regm = (j_reg_manager_t*) malloc(sizeof(j_reg_manager_t)*REGISTERS_AMOUNT);

    init_reg(&regm[0], "eax", 10000000, "t0", "global", true);
    init_reg(&regm[1], "ebx", 10100000, "t0", "global", true);
    init_reg(&regm[2], "ecx", 11000000, "t0", "global", true);
    init_reg(&regm[3], "edx", 11100000, "t0", "global", true);

    false_reg = (j_reg_t*) malloc(sizeof(j_reg_t));
    false_reg->name = "false";
    false_reg->code = 0;

    true_reg = (j_reg_t*) malloc(sizeof(j_reg_t));
    true_reg->name = "true";
    true_reg->code = 0;
}

void init_assembly(FILE* file) {
    if (file == NULL)
    {
        fprintf(stderr, "Unable to open file, redirecting to stdout\n");
        ja_file = stdout;

    } else {
        ja_file = file;
    }

    printf(">> Running synthesis module...\n");
    init_regm();
    init_inst();
}

j_reg_manager_t* regm_lookup ( char * name, char * scope) {

    for (int i = 0; i<REGISTERS_AMOUNT; i++) {
        j_reg_manager_t* reg = &regm[i];
        if (strcmp(reg->name, name) == 0 && strcmp(reg->scope, scope) == 0) {
            return reg;
        }
    }

    return NULL;
}

void add_custom_instr(char* instr) {
    // Creating instruction !
    j_inst_t* new_inst = (j_inst_t*) malloc(sizeof(j_inst_t));
    new_inst->prox = NULL;

    new_inst->line_number = curr++;

    new_inst->custom = true;
    new_inst->custom_instr = (char*) malloc(sizeof(char)*WD_SIZE);
    strcpy(new_inst->custom_instr, instr);

    if (to_init) {
        fprintf(stderr, ">> [ERROR] Trying to add custom instruction to init.");
        return;
    }

    last_inst->prox = new_inst;
    last_inst = new_inst;
}

void add_instr(j_inst_type_t type, j_func_t func, j_arg_t *third, j_arg_t *second, j_arg_t *first, char* label_name) {
    // Creating instruction !
    j_inst_t* new_inst = (j_inst_t*) malloc(sizeof(j_inst_t));
    new_inst->prox = NULL;

    new_inst->line_number = curr++;

    new_inst->type = type;
    new_inst->func = (char*) malloc(sizeof(char)*WD_SIZE);
    strcpy(new_inst->func, func);
    new_inst->first = first;
    new_inst->second = second;
    new_inst->third = third;

    new_inst->label = label_name;
    new_inst->custom = false;
    new_inst->custom_instr = NULL;

    if (to_init) {
        if (first_init_inst == NULL) {
            first_init_inst = new_inst;
            last_init_inst = new_inst;
            return;
        }

        last_init_inst->prox = new_inst;
        last_init_inst = new_inst;

        return;
    }

    last_inst->prox = new_inst;
    last_inst = new_inst;
}

void add_label(char* label_name) {
    add_instr(PRG, "NOP", NULL, NULL, NULL, label_name);
}

j_reg_manager_t* regm_alloc () {

    // Always uses EDX as the locked for const registers
    j_reg_manager_t* reg = &regm[3];
    if (reg->freed && !reg->locked) {
        reg->freed = false;
        reg->locked = true;
        strcpy(reg->name, "t0");
        strcpy(reg->scope, "global");
        reg->reg.memloc = -1;

        return reg;
    }

    fprintf(stderr, ">> [%d] Not enough registers, I will try to free EDX register and lock it.\n", curr);

    if (!reg->freed && reg->locked) {
        fprintf(stderr, ">> [%d] Not enough const registers, it will need at least two EDX registers.\n", curr);
        fprintf(stderr, ">>>> regm_alloc throws the exception.\n");
        fprintf(stderr, ">>>>> curr_instr = %s\n", curr_instr);
        exit(-1);

        return NULL;
    }

    if (!reg->freed && reg->name != NULL && reg->scope != NULL) {
        BucketList bl = st_lookup ( reg->name, reg->scope);

        if (bl != NULL) {

            if (reg->backup) {
                j_arg_t* first = (j_arg_t*) malloc(sizeof(j_arg_t));
                first->type = IMMEDIATE;
                first->data.immediate = reg->reg.memloc;

                j_arg_t* third = (j_arg_t*) malloc(sizeof(j_arg_t));
                third->type = REG;
                third->data.reg = reg;

                add_instr(DMA, "STOREi", third, NULL, first, NULL);
            }

            strcpy(reg->name, "t0");
            strcpy(reg->scope, "global");
            reg->reg.memloc = -1;
            reg->reg.ref = false;
            reg->reg.pointer = false;

            reg->freed = false;

            // Don't need to save const values
            reg->backup = false;


            return reg;
        } else {
            // It was already used for const storage.

            reg->freed = false;
            return reg;
        }

    } else {
        fprintf(stderr, ">>> [0x02] An error has been found !\n");
        fprintf(stderr, ">>>> regm_alloc throws the exception.\n");
        fprintf(stderr, ">>>>> curr_instr = %s\n", curr_instr);
        exit(-1);
        return NULL;
    }
}

j_reg_manager_t* regm_load ( char * name, char * scope, j_var_list* list) {

    for (int i = 0; i<REGISTERS_AMOUNT; i++) {
        j_reg_manager_t* reg = &regm[i];
        if (strcmp(reg->name, name) == 0 && strcmp(reg->scope, scope) == 0 && !reg->freed) {
            return reg;
        }
    }

    for (int i = 0; i<REGISTERS_AMOUNT; i++) {
        j_reg_manager_t* reg = &regm[i];
        if (reg->freed == true && !reg->locked) {
            BucketList bl = st_lookup ( name, scope);

            if (bl != NULL) {
                j_arg_t* first = (j_arg_t*) malloc(sizeof(j_arg_t));
                first->type = IMMEDIATE;
                first->data.immediate = bl->memloc;
                j_arg_t* third = (j_arg_t*) malloc(sizeof(j_arg_t));
                third->type = REG;
                third->data.reg = &reg->reg;
                add_instr(DMA, "LOADi", third, NULL, first, NULL);

                strcpy(reg->name, name);
                strcpy(reg->scope, scope);
                if (bl->passBYreference == 1)
                    reg->reg.ref = true;
                else
                    reg->reg.ref = false;

                reg->reg.memloc = bl->memloc;
                reg->freed = false;

                // There is no need to generate store for param variables
                // and vectors.
                
                /*if (bl->structureid == VectorDeclaredSt ||
                    bl->structureid == VectorParameterSt ||
                    bl->structureid == ParameterSt)
                    reg->backup = false;
                else
                    reg->backup = true;
                */

               // Params must be made a backup, vector declarations
               // are only used for obtaining it's memory address.
               if (bl->structureid == VectorDeclaredSt) {
                    reg->backup = false;
                    reg->reg.pointer = true;
                } else {
                    reg->backup = true;
                    reg->reg.pointer = false;
               }


                return reg;

            } else {
                printf(">>> [0x02] An error has been found !\n");
                printf(">>>> regm_load[1] throws the exception.");
                printf(">>>>> curr_instr = %s\n", curr_instr);
                exit(-1);
                return NULL;
            }
        }
    }
    printf(">> [%d] Not enough registers, I will try to free one of them.\n", curr);

    for (int i = 0; i<REGISTERS_AMOUNT; i++) {
        j_reg_manager_t* reg = &regm[i];
        /*if (list->n1 != NULL && list->s1 != NULL)
            printf("[%d] >>> list->n1 = %s && reg->name = %s && cmp = %d && cmp = %d\n", i, list->n1, reg->name, strcmp(reg->name, list->n1), strcmp(reg->scope, list->s1));

        if (list->n2 != NULL && list->s2 != NULL)
            printf("[%d] >>> list->n2 = %s && reg->name = %s && cmp = %d && cmp = %d\n", i, list->n2, reg->name, strcmp(reg->name, list->n2), strcmp(reg->scope, list->s2));

        if (list->n3 != NULL && list->s3 != NULL)
            printf("[%d] >>> list->n3 = %s && reg->name = %s && cmp = %d && cmp = %d\n", i, list->n3, reg->name, strcmp(reg->name, list->n3), strcmp(reg->scope, list->s3));*/

        if ( (list->n1 == NULL || (list->n1 != NULL && list->s1 != NULL && (strcmp(reg->name, list->n1) != 0 || strcmp(reg->scope, list->s1) != 0))) &&
            (list->n2 == NULL || (list->n2 != NULL && list->s2 != NULL && (strcmp(reg->name, list->n2) != 0 || strcmp(reg->scope, list->s2) != 0))) &&
            (list->n3 == NULL || (list->n3 != NULL && list->s3 != NULL && (strcmp(reg->name, list->n3) != 0 || strcmp(reg->scope, list->s3) != 0))) )
        {
            BucketList bl = st_lookup ( reg->name, reg->scope);

            if (bl != NULL) {

                if (reg->backup) {
                    j_arg_t* first = (j_arg_t*) malloc(sizeof(j_arg_t));
                    first->type = IMMEDIATE;
                    first->data.immediate = bl->memloc;

                    j_arg_t* third = (j_arg_t*) malloc(sizeof(j_arg_t));
                    third->type = LABEL;
                    third->data.label = reg->reg.name;
                    add_instr(DMA, "STOREi", third, NULL, first, NULL);
                }

                /*strcpy(reg->name, name);
                strcpy(reg->scope, scope);
                if (bl->passBYreference == 1)
                    reg->reg.ref = true;
                else
                    reg->reg.ref = false;*/

                reg->freed = true;

                if (bl->structureid == VectorDeclaredSt) {
                    reg->backup = false;
                    reg->reg.pointer = true;
                } else {
                    reg->backup = true;
                    reg->reg.pointer = false;
                }

                // Once I have an available register, I can now load and
                // and return the requested variable over this freed register.
                return regm_load ( name, scope, list);

            } else {
                printf(">>> [0x02] An error has been found !\n");
                printf(">>>> regm_load[2] throws the exception.");
                printf(">>>>> curr_instr = %s\n", curr_instr);
                exit(-1);
                return NULL;
            }
        }
    }

    // Was unable to free a register to use !
    printf(">>> [0x05] An error has been found !\n");
    printf(">>>> regm_load throws the exception.");
    printf(">>>>> curr_instr = %s\n", curr_instr);
    exit(-1);

    return NULL;
}

void reg_backup(int i) {
    if (regm[i].freed == false) {
        BucketList bl = st_lookup ( regm[i].name, regm[i].scope);

        if (bl != NULL) {
            if (regm[i].backup) {
                j_arg_t* first = (j_arg_t*) malloc(sizeof(j_arg_t));
                first->type = IMMEDIATE;
                first->data.immediate = bl->memloc;

                j_arg_t* third = (j_arg_t*) malloc(sizeof(j_arg_t));
                third->type = LABEL;
                third->data.label = regm[i].reg.name;
                add_instr(DMA, "STOREi", third, NULL, first, NULL);
            }

            regm[i].freed = true;


        }
    }

    return;
}

void regm_backup() {
    for (int i = 0; i<REGISTERS_AMOUNT; i++)
        reg_backup(i);
}

void reg_free(int i) {
    regm[i].freed = true;
    regm[i].locked = false;
}

void regm_free() {
    for (int i = 0; i<REGISTERS_AMOUNT; i++)
        reg_free(i);
}

void reg_unlock(int i) {
    regm[i].locked = false;
}

void regm_unlock() {
    // Only the EDX register will be locked !
    reg_unlock(3);
}

// This will print current instruction !
void print_instr(j_inst_t* inst) {
    //printf("\t");

    if (inst->custom) {
        fprintf(ja_file, "%s\n", inst->custom_instr);
        return;
    }

    switch (inst->type)
    {
        case PRG:
            fprintf(ja_file, "PRG_");
            break;
        case DMA:
            fprintf(ja_file, "DMA_");
            break;
        case ALU:
            fprintf(ja_file, "ALU_");
            break;
        case ALUi:
            fprintf(ja_file, "ALUi_");
            break;

        default:
            break;
    }

    fprintf(ja_file, "%s ", inst->func);

    bool comma = false;

    if (inst->third != NULL) {
        comma = true;
        switch (inst->third->type)
        {
            case REG:
                fprintf(ja_file, "%s", inst->third->data.reg->name);
                break;
            case IMMEDIATE:
                fprintf(ja_file, "%d", inst->third->data.immediate);
                break;
            case LABEL:
                fprintf(ja_file, "%s", inst->third->data.label);
                break;

            default:
                break;
        }
    }

    if (inst->second != NULL) {
        if (comma)
            fprintf(ja_file, " ");

        comma = true;

        switch (inst->second->type)
        {
            case REG:
                fprintf(ja_file, "%s", inst->second->data.reg->name);
                break;
            case IMMEDIATE:
                fprintf(ja_file, "%d", inst->second->data.immediate);
                break;
            case LABEL:
                fprintf(ja_file, "%s", inst->second->data.label);
                break;

            default:
                break;
        }
    }

    if (inst->first != NULL) {
        if (comma)
            fprintf(ja_file, " ");

        comma = true;

        switch (inst->first->type)
        {
            case REG:
                fprintf(ja_file, "%s", inst->first->data.reg->name);
                break;
            case IMMEDIATE:
                fprintf(ja_file, "%d", inst->first->data.immediate);
                break;
            case LABEL:
                fprintf(ja_file, "%s", inst->first->data.label);
                break;

            default:
                break;
        }
    }

    if (inst->label != NULL) {
        if (comma)
            fprintf(ja_file, " ");

        fprintf(ja_file, "::%s::", inst->label);
    }

    fprintf(ja_file, "\n");
}

j_var_list* create_var_list(char* name1, char* scope1, char* name2, char* scope2, char* name3, char* scope3) {
    j_var_list* l = (j_var_list*) malloc(sizeof(j_var_list));
    l->n1 = NULL;
    l->n2 = NULL;
    l->n3 = NULL;
    l->s1 = NULL;
    l->s2 = NULL;
    l->s3 = NULL;

    if (name1 != NULL && scope1 != NULL) {
        l->n1 = name1;
        l->s1 = scope1;
    }

    if (name2 != NULL && scope2 != NULL) {
        l->n2 = name2;
        l->s2 = scope2;
    }

    if (name3 != NULL && scope3 != NULL) {
        l->n3 = name3;
        l->s3 = scope3;
    }

    return l;
}

void destroy_var_list(j_var_list* list) {

    free(list);
}

void handle_var(Arg* arg, char** name, char** scope, j_reg_t** reg) {
    if (arg->type == Bucket) {
        // Buckets are variables that can be converted to registers.
        (*name) = arg->addr.variable->name;
        (*scope) = arg->addr.variable->scope;

    } else if (arg->type == Constant) {
        // Constants are numerical values that can be converted to registers as well.

        // regm_alloc is used to allocate a register for general purpose use, have in mind that once you
        // allocated a register, it must be freed before any jumper occurs. You can only allocate one
        // register at a time.
        (*reg) = regm_alloc();

        j_arg_t* reg_arg = (j_arg_t*) malloc(sizeof(j_arg_t));
        reg_arg->type = REG;
        reg_arg->data.reg = (*reg);

        j_arg_t* const_arg = (j_arg_t*) malloc(sizeof(j_arg_t));
        const_arg->type = IMMEDIATE;
        const_arg->data.immediate = arg->addr.constant;

        // XOR is used to empty the register.
        add_instr(ALU, "XOR", reg_arg, reg_arg, reg_arg, NULL);

        // Now we make a simple ADDi instruction.
        add_instr(ALUi, "ADD", reg_arg, const_arg, NULL, NULL);

    } else if (arg->type == String) {
        printf(">>> [0x04]  An error has been found !\n");
        printf(">>>> The string [%s] was not expected here.\n", arg->addr.string);
        return;
    }
}

void decode_instr(char* IC_type, Lno* node) {
    if(strcmp(IC_type, "call") == 0) {
        regm_backup();

        j_arg_t* first = (j_arg_t*) malloc(sizeof(j_arg_t));
        first->type = LABEL;
        first->data.label = node->arg1.addr.variable->name;

        add_instr(PRG, "CALLi", NULL, NULL, first, NULL);

        regm_free();


        // If there is a return value for this call, we must include a pop instruction for it.
        if (node->result.type == Bucket && node->result.addr.variable != NULL) {
            char* third_name = node->result.addr.variable->name;
            char* third_scope = node->result.addr.variable->scope;

            j_var_list* l = create_var_list(third_name, third_scope, NULL, NULL, NULL, NULL);

            j_reg_t *first_reg = regm_load(third_name, third_scope, l);

            destroy_var_list(l);

            j_arg_t* third = (j_arg_t*) malloc(sizeof(j_arg_t));
            third->type = REG;
            third->data.reg = first_reg;

            add_instr(DMA, "POP", third, NULL, NULL, NULL);

            //regm_backup();

        }

        //regm_free();

    } else if (strcmp(IC_type, "start_function") == 0) {
        // start_function remembers to create a label with the name of the function to make call instructions
        // possible.
        //regm_backup();

        // param is the way you read function parameters, to do this, we usually uses POP on joph_arch.

        to_init = true;

        char* third_name = node->result.addr.variable->name;
        char* third_scope = node->result.addr.variable->scope;

        j_var_list* l = create_var_list(third_name, third_scope, NULL, NULL, NULL, NULL);

        j_reg_t *first_reg = regm_load(third_name, third_scope, l);

        destroy_var_list(l);

        j_arg_t* third = (j_arg_t*) malloc(sizeof(j_arg_t));
        third->type = REG;
        third->data.reg = first_reg;

        j_arg_t* first = (j_arg_t*) malloc(sizeof(j_arg_t));
        first->type = LABEL;
        first->data.label = node->result.addr.variable->name;

        // XOR is used to empty the register.
        add_instr(ALU, "XOR", third, third, third, NULL);

        // Now we make a simple ADDi instruction.
        add_instr(ALUi, "ADD", third, first, NULL, NULL);

        regm_backup();

        to_init = false;

        add_label(node->result.addr.variable->name);

        regm_free();

    } else if (strcmp(IC_type, "start_assembly") == 0) {

        regm_backup();

    } else if (strcmp(IC_type, "inst") == 0) {

        char* instr = node->arg1.addr.string;
        char* scope = node->arg2.addr.string;
        
        int n = 0;

        char** words = (char**) malloc(5*sizeof(char*));
        int* mem_locs = (int*) malloc(5*sizeof(int));

        // Initialize memory locations to -1
        for (int i = 0; i<5; i++) {
            mem_locs[i] = -1;
        }

        // Let's find all the words
        char * pch;
        pch = strtok (instr," ");
        while (pch != NULL)
        {
            if (n >= 5) {
                fprintf(stderr, "[ERROR] The instruction '%s' have more than five words.", curr_instr);
                return;
            }

            words[n] = pch;

            pch = strtok (NULL, " ");
            n++;
        }

        // For each word, let's try to find a memory location.

        char* variableName = NULL;

        for (int i = 0; i<n; i++) {
            if (words[i][0] == '&') {
                variableName = words[i]+1;

                BucketList bl = st_lookup ( variableName, scope);

                if (bl == NULL) {
                    fprintf(stderr, "[ERROR] Unable to find memory location from '%s', instruction '%s'.", variableName, curr_instr);
                    return;
                }

                mem_locs[i] = bl->memloc;
            }
        }

        // Now we create a whole new instruction.

        char* new_instr = (char*) malloc(sizeof(char)*WD_SIZE);
        char* aux_str = malloc(sizeof(char)*WD_SIZE);
        char* substring = new_instr;

        for (int i = 0; i<n; i++) {
            bzero(aux_str, WD_SIZE);

            if (mem_locs[i] != -1) {
                sprintf(aux_str, "%d", mem_locs[i]);
                memcpy(substring, aux_str, strlen(aux_str));
            } else {
                sprintf(aux_str, "%s", words[i]);
                memcpy(substring, aux_str, strlen(aux_str));
            }

            if (i < (n-1))
                substring[strlen(aux_str)] = ' ';

            substring = substring + strlen(aux_str)+1;
        }

        free(aux_str);
        free(mem_locs);
        free(words);

        //printf("> Final instruction: '%s'\n", new_instr);

        add_custom_instr(new_instr);

    } else if (strcmp(IC_type, "end_assembly") == 0) {

        regm_free();

    } else if (strcmp(IC_type, "end_function") == 0) {
        // usually the user inserts a return statement inside the function, however if thats not the case,
        // the compiler will include a ret instruction on the end of the function block.

        regm_backup();

        add_instr(PRG, "RET", NULL, NULL, NULL, NULL);


        regm_free();
    } else if (strcmp(IC_type, "param") == 0) {

        // param is the way you read function parameters, to do this, we usually uses POP on joph_arch.

        char* third_name = node->result.addr.variable->name;
        char* third_scope = node->result.addr.variable->scope;

        j_var_list* l = create_var_list(third_name, third_scope, NULL, NULL, NULL, NULL);

        j_reg_t *first_reg = regm_load(third_name, third_scope, l);

        destroy_var_list(l);

        j_arg_t* third = (j_arg_t*) malloc(sizeof(j_arg_t));
        third->type = REG;
        third->data.reg = first_reg;

        add_instr(DMA, "POP", third, NULL, NULL, NULL);

    } else if (strcmp(IC_type, "param_array") == 0) {

        // param is the way you read function parameters, to do this, we usually uses POP on joph_arch.

        char* third_name = node->result.addr.variable->name;
        char* third_scope = node->result.addr.variable->scope;

        j_var_list* l = create_var_list(third_name, third_scope, NULL, NULL, NULL, NULL);

        j_reg_t *first_reg = regm_load(third_name, third_scope, l);

        destroy_var_list(l);

        j_arg_t* third = (j_arg_t*) malloc(sizeof(j_arg_t));
        third->type = REG;
        third->data.reg = first_reg;

        add_instr(DMA, "POP", third, NULL, NULL, NULL);

    } else if (strcmp(IC_type, "IF_FALSE") == 0) {
        regm_backup();

        char* first_name = node->arg1.addr.variable->name;
        char* first_scope = node->arg1.addr.variable->scope;

        char* third_name = node->result.addr.string;

        j_var_list* l = create_var_list(first_name, first_scope, NULL, NULL, NULL, NULL);

        j_reg_t *first_reg = regm_load(first_name, first_scope, l);
        destroy_var_list(l);

        j_arg_t* third = (j_arg_t*) malloc(sizeof(j_arg_t));
        third->type = REG;
        third->data.reg = first_reg;

        j_arg_t* second = (j_arg_t*) malloc(sizeof(j_arg_t));
        second->type = LABEL;
        second->data.label = third_name;

        // Once it's an IF_FALSE statement, it must invert the boolean reply in order to
        // get the correct branch.
        add_instr(ALU, "NOT", third, NULL, third, NULL);
        add_instr(PRG, "JMPCi", third, second, NULL, NULL);


        regm_free();
    } else if (strcmp(IC_type, "return") == 0 ||
                strcmp(IC_type, "return_array") == 0) {

        if (node->result.type == Bucket && node->result.addr.variable != NULL) {
            char* first_name = node->result.addr.variable->name;
            char* first_scope = node->result.addr.variable->scope;

            j_var_list* l = create_var_list(first_name, first_scope, NULL, NULL, NULL, NULL);

            j_reg_t *first_reg = regm_load(first_name, first_scope, l);
            destroy_var_list(l);

            j_arg_t* third = (j_arg_t*) malloc(sizeof(j_arg_t));
            third->type = REG;
            third->data.reg = first_reg;
            
            if (strcmp(IC_type, "return_array") == 0)
                add_instr(DMA, "LOAD", third, NULL, third, NULL);

            add_instr(DMA, "PUSH", third, NULL, NULL, NULL);

        } else if (node->result.type == Constant && node->result.addr.constant) {

            char* first_name = NULL;
            char* first_scope = NULL;
            j_reg_t *first_reg = NULL;

            char* second_name = NULL;
            char* second_scope = NULL;
            j_reg_t *second_reg = NULL;

            char* third_name = NULL;
            char* third_scope = NULL;
            j_reg_t *third_reg = NULL;

            handle_var(&node->result, &third_name, &third_scope, &third_reg);

            j_var_list* l = create_var_list(first_name, first_scope, second_name, second_scope, third_name, third_scope);

            if (third_name != NULL && third_scope != NULL)
                third_reg = regm_load(third_name, third_scope, l);

            destroy_var_list(l);

            j_arg_t* third = (j_arg_t*) malloc(sizeof(j_arg_t));
            third->type = REG;
            third->data.reg = third_reg;

            add_instr(DMA, "PUSH", third, NULL, NULL, NULL);
        }
        regm_backup();

        add_instr(PRG, "RET", NULL, NULL, NULL, NULL);

        regm_free();
    } else if (strcmp(IC_type, "goto") == 0) {
        regm_backup();

        j_arg_t* third = (j_arg_t*) malloc(sizeof(j_arg_t));
        third->type = LABEL;
        third->data.label = node->result.addr.string;

        add_instr(PRG, "JMPi", third, NULL, NULL, NULL);


        regm_free();
    } else if (strcmp(IC_type, "label") == 0) {
        regm_backup();

        add_label(node->result.addr.string);


        regm_free();
    } else if (strcmp(IC_type, "arg") == 0) {

        char* first_name = NULL;
        char* first_scope = NULL;
        j_reg_t *first_reg = NULL;

        char* second_name = NULL;
        char* second_scope = NULL;
        j_reg_t *second_reg = NULL;

        char* third_name = NULL;
        char* third_scope = NULL;
        j_reg_t *third_reg = NULL;

        handle_var(&node->result, &third_name, &third_scope, &third_reg);

        j_var_list* l = create_var_list(first_name, first_scope, second_name, second_scope, third_name, third_scope);

        if (third_name != NULL && third_scope != NULL)
            third_reg = regm_load(third_name, third_scope, l);

        destroy_var_list(l);

        j_arg_t* third = (j_arg_t*) malloc(sizeof(j_arg_t));
        third->type = REG;
        third->data.reg = third_reg;

        if (!third_reg->pointer)
            add_instr(DMA, "PUSH", NULL, NULL, third, NULL);
        else {
            // If VectorDeclaredSt ...
            j_arg_t* immediate = (j_arg_t*) malloc(sizeof(j_arg_t));
            immediate->type = IMMEDIATE;
            immediate->data.immediate = third_reg->memloc;

            j_reg_t* reg = regm_alloc();

            j_arg_t* reg_arg = (j_arg_t*) malloc(sizeof(j_arg_t));
            reg_arg->type = REG;
            reg_arg->data.reg = reg;

            j_arg_t* const_arg = (j_arg_t*) malloc(sizeof(j_arg_t));
            const_arg->type = IMMEDIATE;
            const_arg->data.immediate = third_reg->memloc;

            // XOR is used to empty the register.
            add_instr(ALU, "XOR", reg_arg, reg_arg, reg_arg, NULL);

            // Now we make a simple ADDi instruction.
            add_instr(ALUi, "ADD", reg_arg, const_arg, NULL, NULL);
            add_instr(DMA, "PUSH", NULL, NULL, reg_arg, NULL);
        }

        regm_unlock();

    } else if (strcmp(IC_type, "arg_address") == 0) {

        if (node->result.type == Bucket && node->result.addr.variable != NULL) {
            char* first_name = node->result.addr.variable->name;
            char* first_scope = node->result.addr.variable->scope;

            j_var_list* l = create_var_list(first_name, first_scope, NULL, NULL, NULL, NULL);

            j_reg_t *first_reg = regm_load(first_name, first_scope, l);
            destroy_var_list(l);

            j_arg_t* third = (j_arg_t*) malloc(sizeof(j_arg_t));
            third->type = REG;
            third->data.reg = first_reg;

            add_instr(DMA, "LOAD", third, NULL, third, NULL);
            add_instr(DMA, "PUSH", third, NULL, NULL, NULL);

        }
    } else if (strcmp(IC_type, "EQ") == 0 ||
                strcmp(IC_type, "DIFFERENT") == 0 ||
                strcmp(IC_type, "sum") == 0 ||
                strcmp(IC_type, "sub") == 0 ||
                strcmp(IC_type, "mult") == 0 ||
                strcmp(IC_type, "div") == 0 ||
                strcmp(IC_type, "GT") == 0 ||
                strcmp(IC_type, "LT") == 0 ||
                strcmp(IC_type, "LTEQ") == 0 ||
                strcmp(IC_type, "GTEQ") == 0) {

        // Operations functions such as EQ | DIFFERENT | sum | sub | mult | div | GT | LT | LTEQ | GTEQ
        // usually receives three parameters that can be either registers or consts, to fix this problem
        // we will verify what kind of parameters we have and convert them all to registers, so we will be
        // able to freely use the instruction using three registers address type.

        char* first_name = NULL;
        char* first_scope = NULL;
        j_reg_t *first_reg = NULL;

        char* second_name = NULL;
        char* second_scope = NULL;
        j_reg_t *second_reg = NULL;

        char* third_name = NULL;
        char* third_scope = NULL;
        j_reg_t *third_reg = NULL;

        handle_var(&node->arg1, &first_name, &first_scope, &first_reg);
        handle_var(&node->arg2, &second_name, &second_scope, &second_reg);
        handle_var(&node->result, &third_name, &third_scope, &third_reg);

        j_var_list* l = create_var_list(first_name, first_scope, second_name, second_scope, third_name, third_scope);

        if (first_name != NULL && first_scope != NULL)
            first_reg = regm_load(first_name, first_scope, l);

        if (second_name != NULL && second_scope != NULL)
            second_reg = regm_load(second_name, second_scope, l);

        if (third_name != NULL && third_scope != NULL)
            third_reg = regm_load(third_name, third_scope, l);

        destroy_var_list(l);

        j_arg_t* first = (j_arg_t*) malloc(sizeof(j_arg_t));
        first->type = REG;
        first->data.reg = first_reg;

        j_arg_t* second = (j_arg_t*) malloc(sizeof(j_arg_t));
        second->type = REG;
        second->data.reg = second_reg;

        j_arg_t* third = (j_arg_t*) malloc(sizeof(j_arg_t));
        third->type = REG;
        third->data.reg = third_reg;

        if (strcmp(IC_type, "EQ") == 0)
            add_instr(ALU, "SET", third, second, first, NULL);
        else if (strcmp(IC_type, "DIFFERENT") == 0)
            add_instr(ALU, "SDT", third, second, first, NULL);
        else if (strcmp(IC_type, "sum") == 0)
            add_instr(ALU, "ADD", third, second, first, NULL);
        else if (strcmp(IC_type, "sub") == 0)
            add_instr(ALU, "SUB", third, second, first, NULL);
        else if (strcmp(IC_type, "mult") == 0)
            add_instr(ALU, "MULT", third, second, first, NULL);
        else if (strcmp(IC_type, "div") == 0)
            add_instr(ALU, "DIV", third, second, first, NULL);
        else if (strcmp(IC_type, "GT") == 0)
            add_instr(ALU, "SGT", third, second, first, NULL);
        else if (strcmp(IC_type, "LT") == 0)
            add_instr(ALU, "SLT", third, second, first, NULL);
        else if (strcmp(IC_type, "LTEQ") == 0) {
            //add_instr(ALU, "SLT", third, second, first, NULL);
            add_instr(ALU, "SLET", third, second, first, NULL);
            //printf(">>> NOT YET IMPLEMENTED -> USING < INSTEAD !!!\n");
        } else if (strcmp(IC_type, "GTEQ") == 0) {
            //add_instr(ALU, "SGT", third, second, first, NULL);
            add_instr(ALU, "SGET", third, second, first, NULL);
            //printf(">>> NOT YET IMPLEMENTED -> USING > INSTEAD !!!\n");
        }

        regm_unlock();
    } else if (strcmp(IC_type, "assign_id") == 0) {

        // Assign identifier operations are used to assign consts or register-data values to variables.

        char* first_name = NULL;
        char* first_scope = NULL;
        j_reg_t *first_reg = NULL;

        char* second_name = NULL;
        char* second_scope = NULL;
        j_reg_t *second_reg = NULL;

        char* third_name = NULL;
        char* third_scope = NULL;
        j_reg_t *third_reg = NULL;

        handle_var(&node->arg1, &first_name, &first_scope, &first_reg);
        handle_var(&node->result, &third_name, &third_scope, &third_reg);

        j_var_list* l = create_var_list(first_name, first_scope, second_name, second_scope, third_name, third_scope);

        if (first_name != NULL && first_scope != NULL)
            first_reg = regm_load(first_name, first_scope, l);

        if (third_name != NULL && third_scope != NULL)
            third_reg = regm_load(third_name, third_scope, l);

        destroy_var_list(l);

        j_arg_t* first = (j_arg_t*) malloc(sizeof(j_arg_t));
        first->type = REG;
        first->data.reg = first_reg;

        j_arg_t* third = (j_arg_t*) malloc(sizeof(j_arg_t));
        third->type = REG;
        third->data.reg = third_reg;

        add_instr(ALU, "XOR", third, third, third, NULL);
        add_instr(ALU, "ADD", third, third, first, NULL);

        regm_unlock();
    } else if (strcmp(IC_type, "assign_a_a") == 0) {

        // Assign identifier operations are used to assign consts or register-data values to variables.

        char* first_name = NULL;
        char* first_scope = NULL;
        j_reg_t *first_reg = NULL;

        char* second_name = NULL;
        char* second_scope = NULL;
        j_reg_t *second_reg = NULL;

        char* third_name = NULL;
        char* third_scope = NULL;
        j_reg_t *third_reg = NULL;

        handle_var(&node->arg1, &first_name, &first_scope, &first_reg);
        handle_var(&node->result, &third_name, &third_scope, &third_reg);

        j_var_list* l = create_var_list(first_name, first_scope, second_name, second_scope, third_name, third_scope);

        if (first_name != NULL && first_scope != NULL)
            first_reg = regm_load(first_name, first_scope, l);

        if (third_name != NULL && third_scope != NULL)
            third_reg = regm_load(third_name, third_scope, l);

        destroy_var_list(l);

        j_arg_t* first = (j_arg_t*) malloc(sizeof(j_arg_t));
        first->type = REG;
        first->data.reg = first_reg;

        j_arg_t* third = (j_arg_t*) malloc(sizeof(j_arg_t));
        third->type = REG;
        third->data.reg = third_reg;

        // Backup first reg that contains mem_address
        add_instr(DMA, "PUSH", first, NULL, NULL, NULL);

        // Get mem_address value and store in first
        add_instr(DMA, "LOAD", first, NULL, first, NULL);

        // Store into third reg mem_address the value available
        // in the first reg.
        add_instr(DMA, "STORE", first, NULL, third, NULL);

        // Retrieve the backup from the stack.
        add_instr(DMA, "POP", first, NULL, NULL, NULL);

        regm_unlock();
    } else if (strcmp(IC_type, "assign_a_id") == 0 ||
                strcmp(IC_type, "assign_a_const") == 0 ||
                strcmp(IC_type, "assign_a_f") == 0) {

        // Assign identifier operations are used to assign consts or register-data values to variables.

        char* first_name = NULL;
        char* first_scope = NULL;
        j_reg_t *first_reg = NULL;

        char* second_name = NULL;
        char* second_scope = NULL;
        j_reg_t *second_reg = NULL;

        char* third_name = NULL;
        char* third_scope = NULL;
        j_reg_t *third_reg = NULL;

        handle_var(&node->arg1, &first_name, &first_scope, &first_reg);
        handle_var(&node->result, &third_name, &third_scope, &third_reg);

        j_var_list* l = create_var_list(first_name, first_scope, second_name, second_scope, third_name, third_scope);

        if (first_name != NULL && first_scope != NULL)
            first_reg = regm_load(first_name, first_scope, l);

        if (third_name != NULL && third_scope != NULL)
            third_reg = regm_load(third_name, third_scope, l);

        destroy_var_list(l);

        j_arg_t* first = (j_arg_t*) malloc(sizeof(j_arg_t));
        first->type = REG;
        first->data.reg = first_reg;

        j_arg_t* third = (j_arg_t*) malloc(sizeof(j_arg_t));
        third->type = REG;
        third->data.reg = third_reg;

        // Store into third reg mem_address the value available
        // in the first reg.
        add_instr(DMA, "STORE", first, NULL, third, NULL);

        regm_unlock();
    } else if (strcmp(IC_type, "assign_id_a") == 0) {

        // Assign identifier operations are used to assign consts or register-data values to variables.

        char* first_name = NULL;
        char* first_scope = NULL;
        j_reg_t *first_reg = NULL;

        char* second_name = NULL;
        char* second_scope = NULL;
        j_reg_t *second_reg = NULL;

        char* third_name = NULL;
        char* third_scope = NULL;
        j_reg_t *third_reg = NULL;

        handle_var(&node->arg1, &first_name, &first_scope, &first_reg);
        handle_var(&node->result, &third_name, &third_scope, &third_reg);

        j_var_list* l = create_var_list(first_name, first_scope, second_name, second_scope, third_name, third_scope);

        if (first_name != NULL && first_scope != NULL)
            first_reg = regm_load(first_name, first_scope, l);

        if (third_name != NULL && third_scope != NULL)
            third_reg = regm_load(third_name, third_scope, l);

        destroy_var_list(l);

        j_arg_t* first = (j_arg_t*) malloc(sizeof(j_arg_t));
        first->type = REG;
        first->data.reg = first_reg;

        j_arg_t* third = (j_arg_t*) malloc(sizeof(j_arg_t));
        third->type = REG;
        third->data.reg = third_reg;

        // Store into third reg mem_address the value available
        // in the first reg.
        add_instr(DMA, "LOAD", third, NULL, first, NULL);

        regm_unlock();
    } else if (strcmp(IC_type, "vector_var") == 0 ||
               strcmp(IC_type, "vector_const") == 0 ||
               strcmp(IC_type, "vector_exp") == 0 ||
               strcmp(IC_type, "vector_fun") == 0) {

        // Assign a vector variable is used to assign to a temporary variable the value
        // found at a requested position from a vector.

        char* first_name = NULL;
        char* first_scope = NULL;
        j_reg_t *first_reg = NULL;

        char* second_name = NULL;
        char* second_scope = NULL;
        j_reg_t *second_reg = NULL;

        char* third_name = NULL;
        char* third_scope = NULL;
        j_reg_t *third_reg = NULL;

        handle_var(&node->arg1, &first_name, &first_scope, &first_reg);
        handle_var(&node->arg2, &second_name, &second_scope, &second_reg);
        handle_var(&node->result, &third_name, &third_scope, &third_reg);

        j_var_list* l = create_var_list(first_name, first_scope, second_name, second_scope, third_name, third_scope);

        if (first_name != NULL && first_scope != NULL)
            first_reg = regm_load(first_name, first_scope, l);

        if (second_name != NULL && second_scope != NULL)
            second_reg = regm_load(second_name, second_scope, l);

        if (third_name != NULL && third_scope != NULL)
            third_reg = regm_load(third_name, third_scope, l);

        destroy_var_list(l);

        j_arg_t* first = (j_arg_t*) malloc(sizeof(j_arg_t));
        first->type = REG;
        first->data.reg = first_reg;

        j_arg_t* second = (j_arg_t*) malloc(sizeof(j_arg_t));
        second->type = REG;
        second->data.reg = second_reg;

        j_arg_t* third = (j_arg_t*) malloc(sizeof(j_arg_t));
        third->type = REG;
        third->data.reg = third_reg;

        if (first_reg->ref) {
            // If first_reg ref value is true, it means that this was passed by
            // reference to a function, so the pointer is found on it's value.

            // We must add to the pointer value (the read value from register) and
            // then search on the new generated pointer value and load it from memory.

            add_instr(ALU, "ADD", third, first, second, NULL);

            //add_instr(DMA, "LOAD", third, NULL, third, NULL);

        } else {
            // If first_reg ref value is false, it means that this was not passed by
            // reference to a function, so the pointer is it's own memory location.

            j_arg_t* immediate = (j_arg_t*) malloc(sizeof(j_arg_t));
            immediate->type = IMMEDIATE;
            immediate->data.immediate = first_reg->memloc;

            add_instr(ALU, "XOR", third, third, third, NULL);

            add_instr(ALUi, "ADD", third, NULL, immediate, NULL);
            add_instr(ALU, "ADD", third, third, second, NULL);

            //add_instr(DMA, "LOAD", third, NULL, third, NULL);
        }

        regm_unlock();
    } else if (strcmp(IC_type, "halt") == 0) {
        regm_backup();

        regm_free();

        add_instr(PRG, "HALT", NULL, NULL, NULL, NULL);
    } else {
        printf(">>> [0x06] An error has been found !\n");
        printf(">>>> decode_instr throws the exception.\n");
        printf(">>>>> curr_instr = %s\n", curr_instr);
    }
}

void prepare_assembly(Tlist* list) {
    Lno *p1;
    //printf(">> Preparing assembly code for a %i sized program !\n", list->length);
    if(list!=NULL)
    {
        list->last->prox = NULL;
        for( p1=list->first; p1!=NULL; p1=p1->prox)
        {
            switch (p1->arg1.type)
            {
                case 0: // Bucket
                    break;
                case 1: // String
                    // If a temporary is read as a string, change it to a variable type to make it easier to manage.
                    if (strcmp(p1->arg1.addr.string, "_") != 0 && regexec(&_temporary_regex, p1->arg1.addr.string, 0, (regmatch_t *)NULL, 0) == 0) {
                        //printf("ARG1_HERE !!! %s ||\n", p1->arg1.addr.string);
                        BucketList bl = st_lookup ( p1->arg1.addr.string, "global");

                        if (bl != NULL) {
                            p1->arg1.type = Bucket;
                            p1->arg1.addr.variable = bl;
                        } else {
                            printf(">>> [0x03] An error has been found !\n");
                            printf(">>>> prepare_assembly throws the exception.\n");
                            printf(">>>>> curr_instr = %s\n", curr_instr);
                            printf(">>>>> p1->arg1.addr.string = %s\n", p1->arg1.addr.string);
                        }
                    }
                    break;
                case 2: // Constant
                    break;
                default:
                    printf( ">>> [0x01] An error has been found !\n");
                    break;
            }
            switch (p1->arg2.type)
            {
                case 0: // Bucket
                    break;
                case 1: // String
                    // If a temporary is read as a string, change it to a variable type to make it easier to manage.
                    if (strcmp(p1->arg2.addr.string, "_") != 0 && regexec(&_temporary_regex, p1->arg2.addr.string, 0, (regmatch_t *)NULL, 0) == 0) {
                        //printf("ARG2_HERE !!! %s ||\n", p1->arg2.addr.string);
                        BucketList bl = st_lookup ( p1->arg2.addr.string, "global");

                        if (bl != NULL) {
                            p1->arg2.type = Bucket;
                            p1->arg2.addr.variable = bl;
                        } else {
                            printf(">>> [0x03] An error has been found !\n");
                            printf(">>>> prepare_assembly throws the exception.\n");
                            printf(">>>>> curr_instr = %s\n", curr_instr);
                            printf(">>>>> p1->arg2.addr.string = %s\n", p1->arg2.addr.string);
                        }
                    }
                    break;
                case 2: // Constant
                    break;
                default:
                    printf( ">>> [0x01] An error has been found !\n");
                    break;
            }
            switch (p1->result.type)
            {
                case 0: // Bucket
                    break;
                case 1: // String
                    // If a temporary is read as a string, change it to a variable type to make it easier to manage.
                    if (strcmp(p1->result.addr.string, "_") != 0 && regexec(&_temporary_regex, p1->result.addr.string, 0, (regmatch_t *)NULL, 0) == 0) {
                        //printf("ARG3_HERE !!! %s ||\n", p1->result.addr.string);

                        BucketList bl = st_lookup ( p1->result.addr.string, "global");

                        if (bl != NULL) {
                            p1->result.type = Bucket;
                            p1->result.addr.variable = bl;
                        } else {
                            printf(">>> [0x03] An error has been found !\n");
                            printf(">>>> prepare_assembly throws the exception.\n");
                            printf(">>>>> curr_instr = %s\n", curr_instr);
                            printf(">>>>> p1->result.addr.string = %s\n", p1->result.addr.string);
                        }
                    }
                    break;
                case 2: // Constant
                    break;
                default:
                    printf( ">>> [0x01] An error has been found !\n");
                    break;
            }
        }
    } else {

    }
}

void generate_assembly(Tlist* list) {

    prepare_assembly(list);

    Lno *p1;
    //printf(">> Generating assembly code for a %i sized program !\n", list->length);
    if(list!=NULL)
    {
        list->last->prox = NULL;
        for( p1=list->first; p1!=NULL; p1=p1->prox)
        {
            switch (p1->op.type)
            {
            case 0: // Bucket
                if(p1->op.addr.variable!=NULL)
                {
                    curr_instr = p1->op.addr.variable->name;
                    decode_instr(p1->op.addr.variable->name, p1);
                }
                break;
            case 1: // String

                curr_instr = p1->op.addr.string;

                if (strcmp(curr_instr, "assign_id_const") == 0 ||
                    strcmp(curr_instr, "assign_id_id") == 0 ||
                    strcmp(curr_instr, "assign_id_exp") == 0 ||
                    strcmp(curr_instr, "assign_id_f") == 0)
                    curr_instr = "assign_id";

                decode_instr(curr_instr, p1);

                break;
            case 2: // Constant
                //itoa(p1->op.addr.constant, curr_instr, 10);
                curr_instr = "Constant value !";
                decode_instr(p1->op.addr.constant, p1);
                break;
            default:
                printf( ">>> [0x01] An error has been found !\n");
                break;
            }
        }
    } else {

    }

    j_inst_t* it = header_inst;

    header_inst->prox = first_init_inst;

    last_init_inst->prox = first_inst;

    for (it = header_inst; it != NULL; it = it->prox) {
        print_instr(it);
    }
}

// I/O Functions must be implemented on assembly, in order to do that, we must generate the safe functions and
// insert them on the top of all the programs that will use input and output functions.

void import_IO() {

    j_reg_t* eax = &regm[0];
    j_reg_t* ebx = &regm[1];
    j_reg_t* ecx = &regm[2];
    j_reg_t* edx = &regm[3];

    //first->type = REG;
    //first->data.reg = eax;
    //first->type = IMMEDIATE;
    //first->data.immediate = 0;
    //first->type = LABEL;
    //first->data.label = "LABEL";

    j_arg_t* eax_arg = (j_arg_t*) malloc(sizeof(j_arg_t));
    eax_arg->type = REG;
    eax_arg->data.reg = eax;

    j_arg_t* ebx_arg = (j_arg_t*) malloc(sizeof(j_arg_t));
    ebx_arg->type = REG;
    ebx_arg->data.reg = ebx;

    j_arg_t* ecx_arg = (j_arg_t*) malloc(sizeof(j_arg_t));
    ecx_arg->type = REG;
    ecx_arg->data.reg = ecx;

    j_arg_t* edx_arg = (j_arg_t*) malloc(sizeof(j_arg_t));
    edx_arg->type = REG;
    edx_arg->data.reg = edx;

    j_arg_t* label_arg = (j_arg_t*) malloc(sizeof(j_arg_t));
    label_arg->type = LABEL;
    label_arg->data.label = "input";

    j_arg_t* read_immediate = (j_arg_t*) malloc(sizeof(j_arg_t));
    read_immediate->type = IMMEDIATE;
    read_immediate->data.immediate = 129; // 32'b10000001 means to read a 16 bits value

    j_arg_t* write_immediate = (j_arg_t*) malloc(sizeof(j_arg_t));
    write_immediate->type = IMMEDIATE;
    write_immediate->data.immediate = 131; // 32'b10000011 means to read a 16 bits value

    // First of all we create the start_function for input:
    //INPUT

    add_instr(PRG, "NOP", NULL, NULL, NULL, "input");

    // Once the input function does not require any further information, we will grab the amount of inputs
    // available in the FIFO hardware implemented and store at EAX.

    add_instr(DMA, "GIA", eax_arg, NULL, NULL, NULL);

    // Now we must verify if it exists at least one input value, in order to do that we will,
    // verify if EAX register equals zero, if so, we will go back to the start_function.

    add_instr(ALU, "XOR", ebx_arg, ebx_arg, ebx_arg, NULL);

    add_instr(ALU, "SET", ecx_arg, ebx_arg, eax_arg, NULL);

    // Now we must create a conditional jump using ECX register value that contains the boolean info from the
    // logical operation <.

    add_instr(PRG, "JMPCi", ecx_arg, label_arg, NULL, NULL);

    // If the condition is attended it will go back to the start_function of input and try everything again.
    // However, if the condition is not attended, it means there is values to be read, so, let's grab them.

    add_instr(DMA, "RI", edx_arg, read_immediate, NULL, NULL);

    // Once I already have the final read value, I must return this value back, so, let's trigger the stack.

    add_instr(DMA, "PUSH", NULL, NULL, edx_arg, NULL);

    add_instr(PRG, "RET", NULL, NULL, NULL, NULL);

    // Now we create the start_function for output:
    // OUTPUT FUNCTION

    add_instr(PRG, "NOP", NULL, NULL, NULL, "output");

    // Once the output function do require further information, we must POP from the hardware STACK the value
    // sent when calling the function.

    add_instr(DMA, "POP", eax_arg, NULL, NULL, NULL);

    // Now in order to show on the displays, we must use the instruction DMA_SO that will set the output value
    // from a specific device or led from the DE2-115 Development Kit.

    add_instr(DMA, "SO", write_immediate, NULL, eax_arg, NULL);

    // Once this function does not return any value, we must only use PRG_RET instruction.

    add_instr(PRG, "RET", NULL, NULL, NULL, NULL);

}
