//
// Created by ivan on 23.05.18.
//

#ifndef MINICOMPILER_CODE_GENERATION_H
#define MINICOMPILER_CODE_GENERATION_H

#include <stdio.h>

#include "mCc/ast.h"
#include "ast_visit.h"
#include "tac.h"

#ifdef __cplusplus
extern "C" {
#endif

int jump_cond;



/**********************************************LABELS***/
int string_label_idx;
int float_label_idx;
int label_idx;
int stack_pointer;
int builtin;
struct label_identification{
    char * key;
    char * value;
};

struct labels{
    int counter;
    struct label_identification * l;
};

struct labels * label;

void set_label(char *key, char *value);
char * get_label(char * key);

struct variable{
    int stack_diff;
    char * identifier;
};

struct stack_helper{
    int counter;
    struct variable * variables;
};

struct stack_helper * stack;

void set_var(int size, char * identifier);
int get_var(char * identifier);

void new_stack();

void delete_stack();

int get_stack_size();


struct regs{
    char* eax;
    char* ebx;
    char* ecx;
    char* edx;
};

struct regs * registers;
void free_register(char* identifier);
char* get_register(char * identifier);
void update_register(char *old_identifier, char *new_identifier);
/**********************************************ASSEMBLY*/

struct mCc_assembly *mCc_generate_assembly(struct mCc_tac_list *tac);
void mCc_print_assembly(FILE * out, struct mCc_assembly *ass);
int get_literal_size(enum mCc_ast_literal_type type);
enum instruction{
    MCC_ASSEMBLY_LABEL,
    MCC_ASSEMBLY_DIRECTIVE,
    MCC_ASSEMBLY_FILE,
    MCC_ASSEMBLY_MOV,
    MCC_ASSEMBLY_ADD,
    MCC_ASSEMBLY_DIV,
    MCC_ASSEMBLY_MUL,
    MCC_ASSEMBLY_SUB,
    MCC_ASSEMBLY_PUSH,
    MCC_ASSEMBLY_POP,
    MCC_ASSEMBLY_CALL,
    MCC_ASSEMBLY_LEAVE,
    MCC_ASSEMBLY_RET,
    MCC_ASSEMBLY_CMP,
    MCC_ASSEMBLY_JMP,
    MCC_ASSEMBLY_BUILTIN,
    MCC_ASSEMBLY_CONSTANT

};

struct mCc_assembly_line{
    struct mCc_assembly_line * prev;
    struct mCc_assembly_line * next;
    enum instruction type;
    char * instruction;
};

struct mCc_assembly{
    int counter;
    struct mCc_assembly_line * head;
};

bool check_pushl_copy(struct mCc_tac_list *tac);

bool is_builtin(char *function);

struct mCc_assembly_line *mCc_assembly_function_return(struct mCc_tac_list *tac);

struct mCc_assembly_line *create_string_label(struct mCc_tac_list *tac);

struct mCc_assembly_line *create_float_label(struct mCc_tac_list *tac);

struct mCc_assembly_line *create_builtin_func(struct mCc_tac_list *tac);

struct mCc_assembly_line *mCc_assembly_copy_literal(struct mCc_tac_list *tac);

struct mCc_assembly_line *mCc_assembly_copy_identifier(struct mCc_tac_list *tac);

struct mCc_assembly_line *mCc_assembly_operation(struct mCc_tac_list *tac);

struct mCc_assembly_line *mCc_assembly_function_start(struct mCc_tac_list *tac);

struct mCc_assembly_line *mCc_assembly_function_end(struct mCc_tac_list *tac);

struct mCc_assembly_line *mCc_assembly_jump(struct mCc_tac_list *tac);

struct mCc_assembly_line *mCc_procedure_call(struct mCc_tac_list *tac);

struct mCc_assembly_line *mCc_assembly_param(struct mCc_tac_list *tac);

struct mCc_assembly_line *mCc_assembly_conditional_jump(struct mCc_tac_list *tac);

struct mCc_assembly_line *mCc_assembly_condition(struct mCc_tac_list *tac);

#ifdef __cplusplus
}
#endif
#endif //MINICOMPILER_CODE_GENERATION_H
