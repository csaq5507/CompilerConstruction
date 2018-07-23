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
bool no_more_registers;

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
    int array_size;
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
	char *st4;
	char *st5;
	char *st6;
	char *st7;
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
	MCC_ASSEMBLY_FLD,
	MCC_ASSEMBLY_FXCH,
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
    int counter;
	struct mCc_assembly_line *head;
};


/**********************************************GENERATE*/
void init_globals();
struct mCc_assembly *mCc_assembly_generate(struct mCc_tac_list *tac,
					   char *filename);

void mCc_assembly_print(FILE *out, struct mCc_assembly *ass);

void mCc_assembly_delete(struct mCc_assembly *assembly);

struct mCc_assembly_line *mcc_assembly_generate_labels(struct mCc_tac_list *tac, char * filename);

struct mCc_assembly_line *
mCc_assembly_function_return(struct mCc_tac_list *tac, struct mCc_assembly_line * current);

struct mCc_assembly_line *
mCc_assembly_create_string_label(struct mCc_tac_list *tac, struct mCc_assembly_line * current);

struct mCc_assembly_line *
mCc_assembly_create_float_label(struct mCc_tac_list *tac, struct mCc_assembly_line * current);

struct mCc_assembly_line *
mCc_assembly_create_builtin_func(struct mCc_tac_list *tac, struct mCc_assembly_line * current);

struct mCc_assembly_line *mCc_assembly_copy_literal(struct mCc_tac_list *tac, struct mCc_assembly_line * current);

struct mCc_assembly_line *
mCc_assembly_copy_identifier(struct mCc_tac_list *tac, struct mCc_assembly_line * current);

struct mCc_assembly_line *mCc_assembly_store(struct mCc_tac_list *tac, struct mCc_assembly_line * current);

struct mCc_assembly_line *mCc_assembly_create_label(struct mCc_tac_list *tac, struct mCc_assembly_line * current);

struct mCc_assembly_line *mCc_assembly_operation(struct mCc_tac_list *tac, struct mCc_assembly_line * current);

struct mCc_assembly_line *mCc_assembly_function_start(struct mCc_tac_list *tac, struct mCc_assembly_line * current);

struct mCc_assembly_line *mCc_assembly_function_end(struct mCc_tac_list *tac, struct mCc_assembly_line * current);

struct mCc_assembly_line *mCc_assembly_jump(struct mCc_tac_list *tac, struct mCc_assembly_line * current);

struct mCc_assembly_line *mCc_assembly_procedure_call(struct mCc_tac_list *tac, struct mCc_assembly_line * current);

struct mCc_assembly_line *mCc_assembly_call_param(struct mCc_tac_list *tac, struct mCc_assembly_line * current);

struct mCc_assembly_line *
mCc_assembly_conditional_jump(struct mCc_tac_list *tac, struct mCc_assembly_line * current);

struct mCc_assembly_line *mCc_assembly_condition(struct mCc_tac_list *tac, struct mCc_assembly_line * current);

struct mCc_assembly_line *mCc_assembly_load(struct mCc_tac_list *tac, struct mCc_assembly_line * current);

struct mCc_assembly_line *consume_registers(struct mCc_tac_list *tc, struct mCc_assembly_line *current);


/***************************************/
/***************************************/
bool is_register(char *identifier);

int get_literal_size(enum mCc_ast_literal_type type);

bool is_builtin(char *function);

void set_label(char *key, char *value);

char *get_label(char *key);

void set_var(int size,int array_size, char *identifier);

void set_param_var(int size,int array_size, char *identifier);

int get_pos(char * identifier);

struct variable *get_var(char *identifier);

void new_stack();

void delete_stack();

int get_stack_size();

void free_all_registers();

void free_register(char *identifier);

char *get_register(char *identifier);

void push_float_register(char *identifier);

char * pop_float_register();

void update_register(char *old_identifier, char *new_identifier);

bool is_float(char *identifier);

void move_line_to_end(struct mCc_assembly_line *current, struct mCc_tac_list * tac, char *identifier);

void add_lost_register(char* identifier);

bool top_float_register(char *identifier, struct mCc_assembly_line* current);

char *float_binary_op(struct mCc_tac_list *tac, char* operation, struct mCc_assembly_line * current) ;

void fxch(char *identifier1);

void lose_all_registers();

#ifdef __cplusplus
}
#endif
#endif // MINICOMPILER_CODE_GENERATION_H
