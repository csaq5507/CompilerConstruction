

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
void mCc_tac_print_reverse(FILE *out, struct mCc_tac_list *head);
void mCc_tac_print(FILE *out, struct mCc_tac_list *head);
struct mCc_tac_list * get_at(struct mCc_tac_list* head, int index);
void print_tac_elem(FILE *out, struct mCc_tac_list *current);

struct mCc_tac_list *head;
struct mCc_tac_list *tail;
int v_counter;
int l_counter;


enum mCc_tac_element_type {
	MCC_TAC_ELEMENT_TYPE_UNKNOWN,		 		//                      0
	MCC_TAC_ELEMENT_TYPE_COPY_LITERAL,		 	// x = 4                1
	MCC_TAC_ELEMENT_TYPE_COPY_IDENTIFIER,		// x = y                2
	MCC_TAC_ELEMENT_TYPE_UNARY,		 			// x = op y             3
	MCC_TAC_ELEMENT_TYPE_BINARY,		 		// x = y op z           4
	MCC_TAC_ELEMENT_TYPE_UNCONDITIONAL_JUMP, 	// jump L               5
	MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP,   	// jumpfalse x L        6
	MCC_TAC_ELEMENT_TYPE_LABEL,		 			// label L              7
	MCC_TAC_ELEMENT_TYPE_PARAMETER_SETUP,    	// param x              8
	MCC_TAC_ELEMENT_TYPE_PARAMETER_SETUP_CALL,    	// param x              8
	MCC_TAC_ELEMENT_TYPE_PROCEDURE_CALL,     	// call p,n             9
	MCC_TAC_ELEMENT_TYPE_LOAD,		 			// x = y[i]             10
	MCC_TAC_ELEMENT_TYPE_STORE,		 			// x[i] = y             11
	MCC_TAC_ELEMENT_TYPE_ADDRESS_ASSIGNMENT, 	// x = &y               12
	MCC_TAC_ELEMENT_TYPE_POINTER_ASSIGNMENT, 	// x = *y               13
	MCC_TAC_ELEMENT_TYPE_FUNCTION_START,
	MCC_TAC_ELEMENT_TYPE_FUNCTION_END,
	MCC_TAC_ELEMENT_TYPE_RETURN
};

enum mCc_tac_operation_type {
	MCC_TAC_OPERATION_TYPE_PLUS,       // 1
	MCC_TAC_OPERATION_TYPE_MINUS,      // 2
	MCC_TAC_OPERATION_TYPE_MULTIPLY,   // 3
	MCC_TAC_OPERATION_TYPE_DIVISION,   // 4
	MCC_TAC_OPERATION_TYPE_FAC,	// 5
	MCC_TAC_OPERATION_TYPE_EQ,	 // 6
	MCC_TAC_OPERATION_TYPE_NE,	 // 7
	MCC_TAC_OPERATION_TYPE_LT,	 // 8
	MCC_TAC_OPERATION_TYPE_GT,	 // 9
	MCC_TAC_OPERATION_TYPE_LE,	 // 10
	MCC_TAC_OPERATION_TYPE_GE,	 // 11
	MCC_TAC_OPERATION_TYPE_AND,	// 12
	MCC_TAC_OPERATION_TYPE_OR	  // 13
};

enum mCc_tac_literal_type {
	MCC_TAC_LITERAL_TYPE_STRING,
	MCC_TAC_LITERAL_TYPE_INT,
	MCC_TAC_LITERAL_TYPE_BOOL,
	MCC_TAC_LITERAL_TYPE_FLOAT
};
typedef struct mCc_tac_list {
	struct mCc_tac_list *prev;
	struct mCc_tac_list *next;

	enum mCc_tac_element_type type;
    char *identifier1;

    union {
		//DECL
        struct {
            enum mCc_ast_literal_type decl_lit_type;
            // ARRAY PARAM
            int param_size;
        };
        //COPY_LITERAL
        struct{
            enum mCc_tac_literal_type literal_type;
            union{
                char *s_literal;
                long i_literal;
                bool b_literal;
                double f_literal;
            };
        };

        //COPY_IDENTIFIER
        char *copy_identifier;

        //LOAD / STORE
        struct {
            char *identifier2;
            char *identifier3;
        };


        //UNARY_OP
        struct {
            enum mCc_tac_operation_type unary_op_type;
            char *unary_identifier;
        };

        //BINARY
        struct {
            enum mCc_tac_operation_type binary_op_type;
            char * lhs;
            char * rhs;
        };

        //CALL
        int num_function_param;


        //JUMP
        struct mCc_tac_list *jump;

    };

} tac_list;

tac_list *tac_new_list();


#ifdef __cplusplus
}
#endif
#endif // MINICOMPILER_TAC_H
