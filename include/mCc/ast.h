#ifndef MCC_AST_H
#define MCC_AST_H

#include <jmorecfg.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int bool;
#define true 1
#define false 0


/* Forward Declarations */
struct mCc_ast_expression;
struct mCc_ast_literal;

/* ---------------------------------------------------------------- AST Node */

struct mCc_ast_source_location {
	int start_line;
	int start_col;
	int end_line;
	int end_col;
};

/* Data contained by every AST node. */
struct mCc_ast_node {
	struct mCc_ast_source_location sloc;
};

/* --------------------------------------------------------------- Operators */

enum mCc_ast_binary_op {
	MCC_AST_BINARY_OP_ADD,
	MCC_AST_BINARY_OP_SUB,
	MCC_AST_BINARY_OP_MUL,
	MCC_AST_BINARY_OP_DIV,
	MCC_AST_BINARY_OP_LT,
	MCC_AST_BINARY_OP_GT,
	MCC_AST_BINARY_OP_LE,
	MCC_AST_BINARY_OP_GE,
	MCC_AST_BINARY_OP_AND,
	MCC_AST_BINARY_OP_OR,
	MCC_AST_BINARY_OP_E,
	MCC_AST_BINARY_OP_NE,
};

enum mCc_ast_unary_op {
	MCC_AST_BINARY_OP_NEGATION,
	MCC_AST_BINARY_OP_FAC,
};

/* ---------------------------------------------------------------- Literals */

enum mCc_ast_literal_type {
	MCC_AST_LITERAL_TYPE_INT,
	MCC_AST_LITERAL_TYPE_FLOAT,
	MCC_AST_LITERAL_TYPE_BOOL,
	MCC_AST_LITERAL_TYPE_STRING,
};

struct mCc_ast_literal {
	struct mCc_ast_node node;

	enum mCc_ast_literal_type type;
	union {
		/* MCC_AST_LITERAL_TYPE_INT */
		long i_value;

		/* MCC_AST_LITERAL_TYPE_FLOAT */
		double f_value;

		bool b_value;

		char * s_value;
	};
};

/* ------------------------------------------------------------- Expressions */

enum mCc_ast_expression_type {
	MCC_AST_EXPRESSION_TYPE_SINGLE,
	MCC_AST_EXPRESSION_TYPE_BINARY
};

struct mCc_ast_expression {
	struct mCc_ast_node node;

	enum mCc_ast_expression_type type;

	union {

		/* MCC_AST_EXPRESSION_TYPE_LITERAL */
		struct mCc_ast_single_expr single_expr;

		/* MCC_AST_EXPRESSION_TYPE_BINARY_OP */ //optional
		struct {
			enum mCc_ast_binary_op op;
			struct mCc_ast_expression *lhs;
			struct mCc_ast_expression *rhs;
		};

	};

};

struct mCc_ast_expression *
mCc_ast_new_expression_single(struct mCc_ast_single_expr *single_expr);

struct mCc_ast_expression *
mCc_ast_new_expression_binary_op(enum mCc_ast_binary_op op,
                                 struct mCc_ast_expression *lhs,
                                 struct mCc_ast_expression *rhs);

void mCc_ast_delete_expression(struct mCc_ast_expression *expression);

enum mCc_ast_single_expression_type {
	MCC_AST_SINGLE_EXPRESSION_TYPE_LITERAL,
	MCC_AST_SINGLE_EXPRESSION_TYPE_BINARY
};

struct mCc_ast_single_expr{
	struct mCc_ast_node node;

	union {
		struct mCc_ast_literal literal;

		struct {
			char * identifier;
			struct mCc_ast_expression identifier_expression;
		};

		struct mCc_ast_call_expr call_expr;

		struct {
			enum mCc_ast_unary_op unary_operator;
			struct mCc_ast_expression unary_expression;
		};
		struct mCc_ast_expression expression;

	};
};

struct mCc_ast_call_expr {
	struct mCc_ast_node node;

	char * identifier;

	struct mCc_ast_argument * arguments;
};

/* ---------------------------------------------------------------- Argument */

struct mCc_ast_argument {
	struct mCc_ast_node node;

	struct mCc_ast_expression expression;
	struct mCc_ast_expression * optional_expressions;

};



struct mCc_ast_literal *mCc_ast_new_literal_int(long value);

struct mCc_ast_literal *mCc_ast_new_literal_float(double value);

void mCc_ast_delete_literal(struct mCc_ast_literal *literal);

/* ----------------------------------------------------------- Function Def */

struct mCc_ast_function_def {
	struct mCc_ast_node node;

	enum mCc_ast_literal_type type;
	char * identifier;
	struct mCc_ast_parameter * params;
	struct mCc_ast_compound_stmt c_stmt;

};

/* ----------------------------------------------------------- Parameter */

struct mCc_ast_parameter {
	struct mCc_ast_node node;

	struct mCc_ast_declaration * declarations;
};


/* ----------------------------------------------------------- Compount stmt */


struct mCc_ast_compound_stmt {
	struct mCc_ast_node node;

	struct mCc_ast_stmt * statements;
};


/* ----------------------------------------------------------- stmt */

struct mCc_ast_stmt {
	struct mCc_ast_node node;

	union {
		struct mCc_ast_if_stmt if_stmt;
		struct mCc_ast_while_stmt while_stmt;
		struct mCc_ast_ret_stmt ret_stmt;
		struct mCc_ast_declaration declaration;
		struct mCc_ast_assignment assignment;
		struct mCc_ast_expression expression;
		struct mCc_ast_compound_stmt compound_stmt;

	};
};


struct mCc_ast_if_stmt {
	struct mCc_ast_node node;

	struct mCc_ast_expression expression;
	struct mCc_ast_stmt statement;
	struct mCc_ast_stmt else_statement;

};

struct mCc_ast_while_stmt {
	struct mCc_ast_node node;

	struct mCc_ast_expression expression;
	struct mCc_ast_stmt statement;
};

struct mCc_ast_ret_stmt {
	struct mCc_ast_node node;

	struct mCc_ast_expression expression;

};

/* ----------------------------------------------------------- Assignment */

struct mCc_ast_assignment {
	struct mCc_ast_node node;

	char * identifier;

	struct mCc_ast_expression numerator;
	struct mCc_ast_expression expression;

};
/* ----------------------------------------------------------- Declaration */

struct mCc_ast_declaration {
	struct mCc_ast_node node;

	enum mCc_ast_literal_type type;
	int numerator;
	char * identifier;
};


#ifdef __cplusplus
}
#endif

#endif
