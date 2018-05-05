#ifndef MCC_AST_H
#define MCC_AST_H

#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif

/* Forward Declarations */
typedef struct mCc_ast_literal ast_literal;
typedef struct mCc_ast_identifier ast_identifier;
typedef struct mCc_ast_function_def ast_function_def;
typedef struct mCc_ast_function_def_array ast_function_def_array;
typedef struct mCc_ast_expression ast_expr;
typedef struct mCc_ast_single_expression ast_single_expr;
typedef struct mCc_ast_call_expr ast_call_expr;
typedef struct mCc_ast_declaration ast_declaration;
typedef struct mCc_ast_parameter ast_parameter;
typedef struct mCc_ast_assignment ast_assignment;
typedef struct mCc_ast_stmt ast_stmt;
typedef struct mCc_ast_if_stmt ast_if_stmt;
typedef struct mCc_ast_while_stmt ast_while_stmt;
typedef struct mCc_ast_ret_stmt ast_ret_stmt;
typedef struct mCc_ast_compound_stmt ast_compound_stmt;
typedef struct mCc_ast_argument ast_argument;

/* ###################### STRUCTS/ENUMS ###################### */


enum mCc_ast_type {
	MCC_AST_TYPE_INT,           // 0
	MCC_AST_TYPE_FLOAT,         // 1
	MCC_AST_TYPE_STRING,           // 2
	MCC_AST_TYPE_BOOL,           // 3
	MCC_AST_TYPE_INT_ARRAY,           // 4
	MCC_AST_TYPE_FLOAT_ARRAY,           // 5
	MCC_AST_TYPE_STRING_ARRAY,           // 6
	MCC_AST_TYPE_BOOL_ARRAY,           // 7
	MCC_AST_TYPE_VOID           // 8
};

/* ---------------------------------------------------------------- AST Node */
typedef struct mCc_ast_source_location {
	int start_line;
	int start_col;
	int end_line;
	int end_col;
} ast_source_location;

typedef struct mCc_ast_node {
	ast_source_location sloc;
} ast_node;

/* --------------------------------------------------------------- Operators */
enum mCc_ast_binary_op {
	MCC_AST_BINARY_OP_ADD,		// 0
	MCC_AST_BINARY_OP_SUB,		// 1
	MCC_AST_BINARY_OP_MUL,		// 2
	MCC_AST_BINARY_OP_DIV,		// 3
	MCC_AST_BINARY_OP_LT,		// 4
	MCC_AST_BINARY_OP_GT,		// 5
	MCC_AST_BINARY_OP_LE,		// 6
	MCC_AST_BINARY_OP_GE,		// 7
	MCC_AST_BINARY_OP_AND,		// 8
	MCC_AST_BINARY_OP_OR,		// 9
	MCC_AST_BINARY_OP_EQ,		// 10
	MCC_AST_BINARY_OP_NEQ		// 11
};

enum mCc_ast_unary_op { MCC_AST_UNARY_OP_NEGATION, MCC_AST_UNARY_OP_FAC };

/* ---------------------------------------------------------------- Literals */
enum mCc_ast_literal_type {
	MCC_AST_LITERAL_TYPE_INT,
	MCC_AST_LITERAL_TYPE_FLOAT,
	MCC_AST_LITERAL_TYPE_BOOL,
	MCC_AST_LITERAL_TYPE_STRING
};

typedef struct mCc_ast_literal {
	ast_node node;

	enum mCc_ast_literal_type type;
	union {
		/* MCC_AST_LITERAL_TYPE_INT */
		long i_value;

		/* MCC_AST_LITERAL_TYPE_FLOAT */
		double f_value;

		bool b_value;

		char *s_value;
	};
} ast_literal;

/* ---------------------------------------------------------------- Identifier
 */

typedef struct mCc_ast_identifier {
	ast_node node;

	char *name;
	char *renamed;

	enum mCc_ast_type d_type;
	enum mCc_ast_type *param_types;
} ast_identifier;

ast_identifier *mCc_ast_new_identifier(char *name, int line);

/* ----------------------------------------------------------- Function Def */
enum mCc_ast_function_def_type {
	MCC_AST_FUNCTION_DEF_TYPE_VOID,
	MCC_AST_FUNCTION_DEF_TYPE_TYPE
};

typedef struct mCc_ast_function_def {
	ast_node node;

	enum mCc_ast_function_def_type type;

	union {
		enum mCc_ast_literal_type l_type;
		char *void_value;
	};
	ast_identifier *identifier;
	ast_parameter *params; // Optional
	ast_compound_stmt *c_stmt;

} ast_function_def;

typedef struct mCc_ast_function_def_array {
	int counter;
	ast_function_def *function_def;

} ast_function_def_array;

/* ------------------------------------------------------------- Expressions */
enum mCc_ast_expression_type {
	MCC_AST_EXPRESSION_TYPE_SINGLE,
	MCC_AST_EXPRESSION_TYPE_BINARY
};

typedef struct mCc_ast_expression {
	ast_node node;

	enum mCc_ast_type d_type;

	enum mCc_ast_expression_type type;

	union {

		/* MCC_AST_EXPRESSION_TYPE_LITERAL */
		ast_single_expr *single_expr;

		/* MCC_AST_EXPRESSION_TYPE_BINARY_OP */ // optional
		struct {
			enum mCc_ast_binary_op op;
			ast_single_expr *lhs;
			ast_expr *rhs;
		};
	};

} ast_expr;

/* Single Expression */
enum mCc_ast_single_expression_type {
	MCC_AST_SINGLE_EXPRESSION_TYPE_LITERAL,
	MCC_AST_SINGLE_EXPRESSION_TYPE_IDENTIFIER,
	MCC_AST_SINGLE_EXPRESSION_TYPE_IDENTIFIER_EX,
	MCC_AST_SINGLE_EXPRESSION_TYPE_CALL_EXPR,
	MCC_AST_SINGLE_EXPRESSION_TYPE_UNARY_OP,
	MCC_AST_SINGLE_EXPRESSION_TYPE_PARENTH
};

typedef struct mCc_ast_single_expression {
	ast_node node;

	enum mCc_ast_type d_type;

	enum mCc_ast_single_expression_type type;

	union {
		ast_literal *literal;

		ast_identifier *only_identifier;

		struct {
			ast_identifier *identifier;
			ast_expr *identifier_expression; // OPTIONAL
		};

		ast_call_expr *call_expr;

		struct {
			enum mCc_ast_unary_op unary_operator;
			ast_expr *unary_expression;
		};

		ast_expr *expression;
	};
} ast_single_expr;

/* Call Expression */
typedef struct mCc_ast_call_expr {
	ast_node node;

	enum mCc_ast_type d_type;

	ast_identifier *identifier;

	ast_argument *arguments; // OPTIONAL
} ast_call_expr;

/* ----------------------------------------------------------- Declaration */
enum mCc_ast_declaration_type {
	MCC_AST_DECLARATION_TYPE_ARRAY,
	MCC_AST_DECLARATION_TYPE_SINGLE

};

typedef struct mCc_ast_declaration {
	ast_node node;

	enum mCc_ast_declaration_type type;
	enum mCc_ast_literal_type literal;
	union {

		struct {
			int numerator;
			ast_identifier *array_identifier;
		};
		ast_identifier *identifier;
	};

} ast_declaration;

/* ----------------------------------------------------------- Statement */
enum mCc_ast_stmt_type {
	MCC_AST_IF_STMT,
	MCC_AST_WHILE_STMT,
	MCC_AST_RET_STMT,
	MCC_AST_DECL_STMT,
	MCC_AST_ASS_STMT,
	MCC_AST_EXPR_STMT,
	MCC_AST_COMPOUND_STMT,

};

typedef struct mCc_ast_stmt {
	ast_node node;

	enum mCc_ast_stmt_type type;

	union {
		ast_if_stmt *if_stmt;
		ast_while_stmt *while_stmt;
		ast_ret_stmt *ret_stmt;
		ast_declaration *declaration;
		ast_assignment *assignment;
		ast_expr *expression;
		ast_compound_stmt *compound_stmt;
	};
} ast_stmt;

/* IF Statement */
typedef struct mCc_ast_if_stmt {
	ast_node node;

	ast_expr *expression;
	ast_stmt *statement;
	ast_stmt *else_statement; // OPTIONAL

} ast_if_stmt;

/* WHILE Statement */
typedef struct mCc_ast_while_stmt {
	ast_node node;

	ast_expr *expression;
	ast_stmt *statement;
} ast_while_stmt;

/* RETURN Statement */
typedef struct mCc_ast_ret_stmt {
	ast_node node;

	enum mCc_ast_type d_type;

	ast_expr *expression;

} ast_ret_stmt;

/* COMPOUND Statement */
typedef struct mCc_ast_compound_stmt {
	ast_node node;

	int counter;

	ast_stmt *statements; // OPTIONAL
} ast_compound_stmt;

/* ----------------------------------------------------------- Assignment */
typedef struct mCc_ast_assignment {
	ast_node node;

	ast_identifier *identifier;

	ast_expr *numerator; // OPTIONAL
	ast_expr *expression;

} ast_assignment;

/* ------------------------------------------------------------- Parameter */
typedef struct mCc_ast_parameter {
	ast_node node;

	int counter;

	ast_declaration *declaration;
} ast_parameter;

/* ------------------------------------------------------------- Argument */
typedef struct mCc_ast_argument {
	ast_node node;

	int counter;

	ast_expr *expression;
} ast_argument;

/* ###################### FUNCTIONS ###################### */
/* ---------------------------------------------------------------- Literals */
ast_literal *mCc_ast_new_literal_int(long value);

ast_literal *mCc_ast_new_literal_float(double value);

ast_literal *mCc_ast_new_literal_bool(bool value);

ast_literal *mCc_ast_new_literal_string(char *value);

void mCc_ast_delete_literal_value(ast_literal *literal, void *data);
void mCc_ast_delete_literal(ast_literal *literal, void *data);

/* ------------------------------------------------------------- Expressions */
ast_expr *mCc_ast_new_expression_single(ast_single_expr *single_expr);

ast_expr *mCc_ast_new_expression_binary_op(enum mCc_ast_binary_op op,
					   ast_single_expr *lhs, ast_expr *rhs);

void mCc_ast_delete_expression(ast_expr *expression, void *data);

/* Single Expression */
ast_single_expr *mCc_ast_new_single_expression_literal(ast_literal *literal);

ast_single_expr *
mCc_ast_new_single_expression_identifier(ast_identifier *identifier);

ast_single_expr *
mCc_ast_new_single_expression_identifier_ex(ast_identifier *identifier,
					    ast_expr *identifier_expression);
ast_single_expr *
mCc_ast_new_single_expression_call_expr(ast_call_expr *call_expr);

ast_single_expr *
mCc_ast_new_single_expression_unary_op(enum mCc_ast_unary_op unary_op,
				       ast_expr *unary_expression);

ast_single_expr *mCc_ast_new_single_expression_parenth(ast_expr *expression);

void mCc_ast_delete_single_expression(ast_single_expr *expression, void *data);

/* Call Expression */
ast_call_expr *mCc_ast_new_empty_call_expr(ast_identifier *identifier);

ast_call_expr *mCc_ast_new_call_expr(ast_identifier *identifier,
				     ast_argument *arguments);

void mCc_ast_delete_call_expr(ast_call_expr *call_expr, void *data);

/* ----------------------------------------------------------- Function Def  */
ast_function_def *mCc_ast_new_void_function_def(ast_identifier *identifier,
						ast_parameter *params,
						ast_compound_stmt *c_stmt);

ast_function_def *mCc_ast_new_type_function_def(enum mCc_ast_literal_type type,
						ast_identifier *identifier,
						ast_parameter *params,
						ast_compound_stmt *c_stmt);

ast_function_def_array *
mCc_ast_add_function_def_to_array(ast_function_def_array *f,
				  ast_function_def *f2);

ast_function_def_array *mCc_ast_new_function_def_array(ast_function_def *f);

void mCc_ast_delete_function_def_array(ast_function_def_array *f);
void mCc_ast_delete_function_def(ast_function_def *f, void *data);

/* ----------------------------------------------------------- Declaration */
ast_declaration *
mCc_ast_new_array_declaration(enum mCc_ast_literal_type literal, int numerator,
			      ast_identifier *identifier);

ast_declaration *
mCc_ast_new_single_declaration(enum mCc_ast_literal_type literal,
			       ast_identifier *identifier);

void mCc_ast_delete_declaration(ast_declaration *decl, void *data);

/* ----------------------------------------------------------- Statement */
ast_stmt *mCc_ast_new_if_stmt(ast_if_stmt *if_stmt);

ast_if_stmt *mCc_ast_new_if(ast_expr *ex, ast_stmt *stmt);

ast_if_stmt *mCc_ast_new_if_else(ast_expr *ex, ast_stmt *stmt,
				 ast_stmt *elsestmt);

void mCc_ast_delete_if_stmt(ast_if_stmt *if_stmt, void *data);

ast_stmt *mCc_ast_new_while_stmt(ast_while_stmt *while_stmt);

ast_while_stmt *mCc_ast_new_while(ast_expr *ex, ast_stmt *stmt);

void mCc_ast_delete_while_stmt(ast_while_stmt *while_stmt, void *data);

ast_stmt *mCc_ast_new_ret_stmt(ast_ret_stmt *ret_stmt);

ast_ret_stmt *mCc_ast_new_ret(ast_expr *ex);

ast_ret_stmt *mCc_ast_new_empty_ret();

void mCc_ast_delete_ret_stmt(ast_ret_stmt *ret_stmt, void *data);

ast_stmt *mCc_ast_new_declaration(ast_declaration *decl_stmt);

ast_stmt *mCc_ast_new_assignment(ast_assignment *ass_stmt);

ast_stmt *mCc_ast_new_expression(ast_expr *expr_stmt);

ast_stmt *mCc_ast_new_compound_stmt(ast_compound_stmt *compound_stmt);

ast_compound_stmt *mCc_ast_new_empty_compound();

ast_compound_stmt *mCc_ast_new_single_compound(ast_stmt *stmt);

ast_compound_stmt *mCc_ast_new_compound_array(ast_compound_stmt *stmts,
					      ast_stmt *stmt);

void mCc_ast_delete_compound_stmt(ast_compound_stmt *compound_stmt, void *data);

void mCc_ast_delete_stmt(ast_stmt *stmt, void *data);

/* ----------------------------------------------------------- Assignment */
ast_assignment *mCc_ast_new_single_assignment(ast_identifier *identifier,
					      ast_expr *ex);

ast_assignment *mCc_ast_new_array_assignment(ast_identifier *identifier,
					     ast_expr *ex, ast_expr *ex2);

void mCc_ast_delete_assignment(ast_assignment *assignment, void *data);

/* ------------------------------------------------------------- Argument */
ast_argument *mCc_ast_new_single_argument(ast_expr *ex);

ast_argument *mCc_ast_new_argument_array(ast_argument *arguments, ast_expr *ex);

void mCc_ast_delete_argument(ast_argument *argument, void *data);


/* ------------------------------------------------------------- Parameter */
ast_parameter *mCc_ast_new_parameter_array(ast_parameter *params,
					   ast_declaration *decl);

ast_parameter *mCc_ast_new_empty_parameter_array();

ast_parameter *mCc_ast_new_single_parameter(ast_declaration *decl);

void mCc_ast_delete_parameter(ast_parameter *params, void *data);


/* ---------------------------------------------------------------- VISITOR */

void mCc_ast_delete_identifier(ast_identifier *identifier, void *data);


#ifdef __cplusplus
}
#endif

#endif
