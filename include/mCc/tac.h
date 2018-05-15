

#ifndef MINICOMPILER_TAC_H
#define MINICOMPILER_TAC_H

#include <stdio.h>

#include "mCc/ast.h"
#include "ast_visit.h"

#ifdef __cplusplus
extern "C" {
#endif


struct mCc_tac_list *mCc_tac_generate(struct mCc_ast_function_def_array *f);
void mCc_tac_delete(struct mCc_tac_list *head);
void mCc_tac_print(FILE *out, struct mCc_tac_list *head);

struct mCc_tac_list *head;
struct mCc_tac_list *tail;
int v_counter;
int l_counter;


enum mCc_tac_element_type {
	MCC_TAC_ELEMENT_TYPE_UNKNOWN,		 //                      0
	MCC_TAC_ELEMENT_TYPE_COPY,		 // x = y                1
	MCC_TAC_ELEMENT_TYPE_UNARY,		 // x = op y             2
	MCC_TAC_ELEMENT_TYPE_BINARY,		 // x = y op z           3
	MCC_TAC_ELEMENT_TYPE_UNCONDITIONAL_JUMP, // jump L                4
	MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP,   // jumpfalse x L        5
	MCC_TAC_ELEMENT_TYPE_LABEL,		 // label L              6
	MCC_TAC_ELEMENT_TYPE_PARAMETER_SETUP,    // param x              7
	MCC_TAC_ELEMENT_TYPE_PROCEDURE_CALL,     // call p,n             8
	MCC_TAC_ELEMENT_TYPE_LOAD,		 // x = y[i]             9
	MCC_TAC_ELEMENT_TYPE_STORE,		 // x[i] = y             10
	MCC_TAC_ELEMENT_TYPE_ADDRESS_ASSIGNMENT, // x = &y               11
	MCC_TAC_ELEMENT_TYPE_POINTER_ASSIGNMENT, // x = *y               12
	MCC_TAC_ELEMENT_TYPE_FUNCTION_START,
	MCC_TAC_ELEMENT_TYPE_FUNCTION_END,
	MCC_TAC_ELEMENT_TYPE_RETURN
};

enum mCc_tac_operation_type {
	MCC_TAC_OPERATION_TYPE_UNKNOWN,    // 0
	MCC_TAC_OPERATION_TYPE_ASSIGNMENT, // 1
	MCC_TAC_OPERATION_TYPE_PLUS,       // 2
	MCC_TAC_OPERATION_TYPE_MINUS,      // 3
	MCC_TAC_OPERATION_TYPE_MULTIPLY,   // 4
	MCC_TAC_OPERATION_TYPE_DIVISION,   // 5
	MCC_TAC_OPERATION_TYPE_FAC,	// 6
	MCC_TAC_OPERATION_TYPE_EQ,	 // 7
	MCC_TAC_OPERATION_TYPE_NE,	 // 8
	MCC_TAC_OPERATION_TYPE_LT,	 // 9
	MCC_TAC_OPERATION_TYPE_GT,	 // 10
	MCC_TAC_OPERATION_TYPE_LE,	 // 11
	MCC_TAC_OPERATION_TYPE_GE,	 // 12
	MCC_TAC_OPERATION_TYPE_AND,	// 13
	MCC_TAC_OPERATION_TYPE_OR	  // 14
};

enum mCc_tac_literal_type {
	MCC_TAC_LITERAL_TYPE_UNKNWON,
	MCC_TAC_LITERAL_TYPE_STRING,
	MCC_TAC_LITERAL_TYPE_INT,
	MCC_TAC_LITERAL_TYPE_BOOL,
	MCC_TAC_LITERAL_TYPE_FLOAT
};

typedef struct mCc_tac_list {
	struct mCc_tac_list *prev;
	struct mCc_tac_list *next;

	enum mCc_tac_element_type type;

	enum mCc_tac_operation_type operation_type;
	struct mCc_tac_list *jump;

	char *f_identifier;
	char *s_identifier;
	char *t_identifier;

	int num_function_param;

	enum mCc_tac_literal_type l_type;

	char *s_literal;
	long i_literal;
	bool b_literal;
	double f_literal;

} tac_list;

tac_list *tac_new_list();

#ifdef __cplusplus
}
#endif
#endif // MINICOMPILER_TAC_H
