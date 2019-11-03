#include <stdint.h>
#include <stdbool.h>
#include "symtab.h"
#include "globals.h"

typedef char* j_func_t;
typedef uint32_t j_immediate_t;
typedef uint8_t j_reg_code_t;
typedef char* j_label_t;

typedef enum joph_inst_type {
    NONE = 0, // USED FOR CUSTOM INSTRUCTIONS
    PRG = 111, // 111
    DMA = 100, // 100
    ALU = 10, // 010
    ALUi = 11, // 011
} j_inst_type_t;

typedef enum joph_arg_type {
    REG,
    IMMEDIATE,
    LABEL
} j_arg_type_t;

typedef struct joph_reg {
    char* name;
    j_reg_code_t code;
    bool ref;
    uint32_t memloc;
    bool pointer;
} j_reg_t;

typedef struct joph_arg {
    j_arg_type_t type;
    union {
        j_reg_t* reg;
        j_immediate_t immediate;
        j_label_t label;
    } data;
} j_arg_t;

typedef struct joph_inst
{
    struct joph_inst *prox;

    int line_number;

    j_inst_type_t type;
    j_func_t func;
    char* label;

    j_arg_t *first, *second, *third;

    // For custom instructions
    bool custom;
    char* custom_instr;
} j_inst_t;

typedef struct reg_manager {
    j_reg_t reg;
    char* name;
    char* scope;
    bool freed;
    bool locked;
    bool backup;
} j_reg_manager_t;

typedef struct joph_var_list {
    char* n1;
    char* s1;
    char* n2;
    char* s2;
    char* n3;
    char* s3;
} j_var_list;

j_reg_manager_t* regm_lookup ( char * name, char * scope);

void init_assembly(FILE* file);

void generate_assembly(Tlist* list);