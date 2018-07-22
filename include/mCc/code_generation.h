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


/**********************************************STRUCT/VARS*/

int jump_cond;
int string_label_idx;
int float_label_idx;
int label_idx;
int builtin;
int skip;
int push_vars;
bool is_float_condition;

struct label_identification {
	char *key;
	char *value;
};

struct labels {
	int counter;
	struct label_identification *l;
};

struct labels *label;


struct variable {
    int stack_diff;
    int size;
    enum mCc_tac_literal_type type;
    char *identifier;
};

struct stack_helper {
    int counter;
    struct variable *variables;
};

struct stack_helper *stack;
struct stack_helper *param_stack;

struct my_reg{
char * identifier;
};

struct lost_regs {
    int counter;
    struct my_reg* reg;
};

struct lost_regs* lost_registers;

struct regs {
	char *eax;
	char *ebx;
	char *ecx;
	char *edx;
	char *st0;
	char *st1;
	char *st2;
	char *st3;
};

struct regs *registers;

enum instruction {
	MCC_ASSEMBLY_LABEL,
	MCC_ASSEMBLY_DIRECTIVE,
	MCC_ASSEMBLY_FILE,
	MCC_ASSEMBLY_MOV,
	MCC_ASSEMBLY_ADD,
	MCC_ASSEMBLY_DIV,
	MCC_ASSEMBLY_MUL,
	MCC_ASSEMBLY_SUB,
	MCC_ASSEMBLY_PUSH,
	MCC_ASSEMBLY_CALL,
	MCC_ASSEMBLY_LEAVE,
	MCC_ASSEMBLY_RET,
	MCC_ASSEMBLY_CMP,
	MCC_ASSEMBLY_JMP,
	MCC_ASSEMBLY_FSTP,
	MCC_ASSEMBLY_BUILTIN,
	MCC_ASSEMBLY_CONSTANT

};

struct mCc_assembly_line {
	struct mCc_assembly_line *prev;
	struct mCc_assembly_line *next;
	enum instruction type;
	char *instruction;
};

struct mCc_assembly {
	struct mCc_assembly_line *head;
};

enum copy { COPY_PUSHL, COPY_MOVL, COPY_PUSHL_ARRAY };

/**********************************************GENERATE*/

void *init_globals();

struct mCc_assembly *mCc_assembly_generate(struct mCc_tac_list *tac,
					   char *filename);

void mCc_assembly_print(FILE *out, struct mCc_assembly *ass);

void mCc_assembly_delete(struct mCc_assembly *assembly);

struct mCc_assembly_line *mcc_assembly_generate_labels(struct mCc_tac_list *tac, char * filename);

struct mCc_assembly_line *
mCc_assembly_function_return(struct mCc_tac_list *tac);

struct mCc_assembly_line *
mCc_assembly_create_string_label(struct mCc_tac_list *tac);

struct mCc_assembly_line *
mCc_assembly_create_float_label(struct mCc_tac_list *tac);

struct mCc_assembly_line *
mCc_assembly_create_builtin_func(struct mCc_tac_list *tac);

struct mCc_assembly_line *mCc_assembly_copy_literal(struct mCc_tac_list *tac);

struct mCc_assembly_line *
mCc_assembly_copy_identifier(struct mCc_tac_list *tac);

struct mCc_assembly_line *mCc_assembly_store(struct mCc_tac_list *tac);

struct mCc_assembly_line *mCc_assembly_create_label(struct mCc_tac_list *tac);

struct mCc_assembly_line *mCc_assembly_operation(struct mCc_tac_list *tac, struct mCc_assembly_line* current);

struct mCc_assembly_line *mCc_assembly_function_start(struct mCc_tac_list *tac);

struct mCc_assembly_line *mCc_assembly_function_end(struct mCc_tac_list *tac);

struct mCc_assembly_line *mCc_assembly_jump(struct mCc_tac_list *tac);

struct mCc_assembly_line *mCc_assembly_procedure_call(struct mCc_tac_list *tac);

struct mCc_assembly_line *mCc_assembly_call_param(struct mCc_tac_list *tac);

struct mCc_assembly_line *
mCc_assembly_conditional_jump(struct mCc_tac_list *tac);

struct mCc_assembly_line *mCc_assembly_condition(struct mCc_tac_list *tac, struct mCc_assembly_line* current);
/*
struct mCc_assembly_line *
mCc_assembly_nested_condition(struct mCc_tac_list *tac,struct mCc_assembly_line * as);
*/
struct mCc_assembly_line *mCc_assembly_load(struct mCc_tac_list *tac);

/***************************************/
/***************************************/
bool is_register(char *identifier);

int get_literal_size(enum mCc_ast_literal_type type);

bool is_builtin(char *function);

void set_label(char *key, char *value);

char *get_label(char *key);

void set_var(int size, char *identifier);

void set_param_var(int size, char *identifier);

int get_pos(char * identifier);

struct variable *get_var(char *identifier);

void new_stack();

void delete_stack();

int get_stack_size();

void free_all_registers();

void free_register(char *identifier);

char *get_register(char *identifier);

void set_float_register(char *identifier);

void update_register(char *old_identifier, char *new_identifier);

bool is_float(char *identifier);

void
move_2_lines_to_end(struct mCc_assembly_line *current, struct mCc_tac_list *tac, char *identifier, char *identifier2);


void move_line_to_end(struct mCc_assembly_line *current, struct mCc_tac_list * tac, char *identifier);

void add_lost_register(char* identifier);

bool has_register(char *identifier);

int negate_binary_op_type(enum mCc_tac_operation_type type);

char *float_binary_op(struct mCc_tac_list *tac, char * operation);

void swap_register(char *identifier1);


#ifdef __cplusplus
}
#endif
#endif // MINICOMPILER_CODE_GENERATION_H
