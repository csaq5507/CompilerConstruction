#ifndef MCC_AST_H
#define MCC_AST_H

#include <jmorecfg.h>

#ifdef __cplusplus
extern "C" {
#else
typedef int bool;
#define true 1
#define false 0
#endif





/* Forward Declarations */
struct mCc_ast_expression;
struct mCc_ast_literal;
struct mCc_ast_single_expression;
struct mCc_ast_call_expr;
struct mCc_ast_parameter;
struct mCc_ast_declaration;
struct mCc_ast_compound_stmt;
struct mCc_ast_function_def;
struct mCc_ast_argument;


/* array_counter */

struct mCc_ast_function_def_array {
    int counter;
    struct mCc_ast_function_def * function_def;

};

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
	MCC_AST_BINARY_OP_EQ,
	MCC_AST_BINARY_OP_NEQ
};

enum mCc_ast_unary_op {
	MCC_AST_UNARY_OP_NEGATION,
	MCC_AST_UNARY_OP_FAC
};

/* ---------------------------------------------------------------- Literals */

enum mCc_ast_literal_type {
	MCC_AST_LITERAL_TYPE_INT,
	MCC_AST_LITERAL_TYPE_FLOAT,
	MCC_AST_LITERAL_TYPE_BOOL,
	MCC_AST_LITERAL_TYPE_STRING
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


struct mCc_ast_literal *mCc_ast_new_literal_int(long value);

struct mCc_ast_literal *mCc_ast_new_literal_float(double value);

struct mCc_ast_literal *mCc_ast_new_literal_bool(bool value);

struct mCc_ast_literal *mCc_ast_new_literal_string(char * value);

void mCc_ast_delete_literal(struct mCc_ast_literal *literal);

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
		struct mCc_ast_single_expression*single_expr;

		/* MCC_AST_EXPRESSION_TYPE_BINARY_OP */ //optional
		struct {
			enum mCc_ast_binary_op op;
			struct mCc_ast_single_expression*lhs;
			struct mCc_ast_expression *rhs;
		};

	};

};

struct mCc_ast_expression *
mCc_ast_new_expression_single(struct mCc_ast_single_expression*single_expr);

struct mCc_ast_expression *
mCc_ast_new_expression_binary_op(enum mCc_ast_binary_op op,
                                 struct mCc_ast_single_expression*lhs,
                                 struct mCc_ast_expression *rhs);

void mCc_ast_delete_expression(struct mCc_ast_expression *expression);


enum mCc_ast_single_expression_type {
	MCC_AST_SINGLE_EXPRESSION_TYPE_LITERAL,
	MCC_AST_SINGLE_EXPRESSION_TYPE_IDENTIFIER,
	MCC_AST_SINGLE_EXPRESSION_TYPE_IDENTIFIER_EX,
	MCC_AST_SINGLE_EXPRESSION_TYPE_CALL_EXPR,
	MCC_AST_SINGLE_EXPRESSION_TYPE_UNARY_OP,
	MCC_AST_SINGLE_EXPRESSION_TYPE_PARENTH
};

struct mCc_ast_single_expression {
    struct mCc_ast_node node;

    enum mCc_ast_single_expression_type type;

    union {
        struct mCc_ast_literal *literal;

		char * only_identifier;

        struct {
            char * identifier;
            struct mCc_ast_expression *identifier_expression; //OPTIONAL
        };

        struct mCc_ast_call_expr *call_expr;

        struct {
            enum mCc_ast_unary_op unary_operator;
            struct mCc_ast_expression * unary_expression;
        };

        struct mCc_ast_expression *expression;

    };
};

struct mCc_ast_single_expression*
mCc_ast_new_single_expression_literal(struct mCc_ast_literal *literal);

struct mCc_ast_single_expression*
mCc_ast_new_single_expression_identifier(char* identifier);
struct mCc_ast_single_expression*
mCc_ast_new_single_expression_identifier_ex(char* identifier,
										 struct mCc_ast_expression *identifier_expression);
struct mCc_ast_single_expression*
mCc_ast_new_single_expression_call_expr(struct mCc_ast_call_expr *call_expr);

struct mCc_ast_single_expression*
mCc_ast_new_single_expression_unary_op(enum mCc_ast_unary_op unary_op,
                                       struct mCc_ast_expression *unary_expression);
struct mCc_ast_single_expression*
mCc_ast_new_single_expression_parenth(struct mCc_ast_expression *expression);

void mCc_ast_delete_single_expression(struct mCc_ast_single_expression *expression);

/* ----------------------------------------------------------- Function Def / CALL */

enum mCc_ast_function_def_type {
    MCC_AST_FUNCTION_DEF_TYPE_VOID,
    MCC_AST_FUNCTION_DEF_TYPE_TYPE
};



struct mCc_ast_function_def {
	struct mCc_ast_node node;

    enum mCc_ast_function_def_type type;


    union{
        enum mCc_ast_literal_type l_type;
        char * void_value;
    };
	char * identifier;
	struct mCc_ast_parameter *params; //Optional
	struct mCc_ast_compound_stmt *c_stmt;

};

struct mCc_ast_function_def *
mCc_ast_new_void_function_def(char * identifier, struct mCc_ast_parameter *params, struct mCc_ast_compound_stmt *c_stmt);

struct mCc_ast_function_def *
mCc_ast_new_type_function_def(enum mCc_ast_literal_type type, char * identifier, struct mCc_ast_parameter *params, struct mCc_ast_compound_stmt *c_stmt);

void mCc_ast_delete_function_def(struct mCc_ast_function_def_array *f);


struct mCc_ast_parameter {
	struct mCc_ast_node node;

    int counter;

    struct mCc_ast_declaration * declaration;
};


struct mCc_ast_argument {
    struct mCc_ast_node node;

    int counter;

    struct mCc_ast_expression * expression;
};


struct mCc_ast_call_expr {
    struct mCc_ast_node node;

    char * identifier;

    struct mCc_ast_argument *arguments; //OPTIONAL
};

/* ----------------------------------------------------------- Assignment / Declaration */

struct mCc_ast_assignment {
    struct mCc_ast_node node;

    char * identifier;

    struct mCc_ast_expression *numerator; //OPTIONAL
    struct mCc_ast_expression *expression;

};

enum mCc_ast_declaration_type{
	MCC_AST_DECLARATION_TYPE_ARRAY,
	MCC_AST_DECLARATION_TYPE_SINGLE

};

struct mCc_ast_declaration {
    struct mCc_ast_node node;

	enum mCc_ast_declaration_type type;
	enum mCc_ast_literal_type literal;
	union{

		struct{
			int numerator;
			char * array_identifier;

		};
		char * identifier;
	};

};


struct mCc_ast_declaration *
mCc_ast_new_array_declaration(enum mCc_ast_literal_type literal,int numerator, char * identifier);

struct mCc_ast_declaration *
mCc_ast_new_single_declaration(enum mCc_ast_literal_type literal, char * identifier);

void mCc_ast_delete_declaration(struct mCc_ast_declaration * decl);

/* ----------------------------------------------------------- stmt */

struct mCc_ast_stmt;


struct mCc_ast_if_stmt {
	struct mCc_ast_node node;

	struct mCc_ast_expression *expression;
	struct mCc_ast_stmt *statement;
	struct mCc_ast_stmt *else_statement; //OPTIONAL

};

struct mCc_ast_while_stmt {
	struct mCc_ast_node node;

	struct mCc_ast_expression *expression;
	struct mCc_ast_stmt *statement;
};

struct mCc_ast_ret_stmt {
	struct mCc_ast_node node;

	struct mCc_ast_expression *expression;

};


struct mCc_ast_compound_stmt {
    struct mCc_ast_node node;

    int counter;

    struct mCc_ast_stmt * statements; //OPTIONAL
};


enum mCc_ast_stmt_type {
    MCC_AST_IF_STMT	,
    MCC_AST_WHILE_STMT,
    MCC_AST_RET_STMT,
    MCC_AST_DECL_STMT,
    MCC_AST_ASS_STMT,
    MCC_AST_EXPR_STMT,
    MCC_AST_COMPOUND_STMT,

};

struct mCc_ast_stmt{
    struct mCc_ast_node node;

    enum mCc_ast_stmt_type type;

    union {
        struct mCc_ast_if_stmt *if_stmt;
        struct mCc_ast_while_stmt *while_stmt;
        struct mCc_ast_ret_stmt *ret_stmt;
        struct mCc_ast_declaration *declaration;
        struct mCc_ast_assignment *assignment;
        struct mCc_ast_expression *expression;
        struct mCc_ast_compound_stmt *compound_stmt;

    };
};

struct mCc_ast_stmt *
mCc_ast_new_if_stmt(struct mCc_ast_if_stmt *if_stmt);

struct mCc_ast_stmt *
mCc_ast_new_while_stmt(struct mCc_ast_while_stmt *while_stmt);

struct mCc_ast_stmt *
mCc_ast_new_ret_stmt(struct mCc_ast_ret_stmt *ret_stmt);

struct mCc_ast_stmt *
mCc_ast_new_declaration(struct mCc_ast_declaration *decl_stmt);

struct mCc_ast_stmt *
mCc_ast_new_assignment(struct mCc_ast_assignment *ass_stmt);

struct mCc_ast_stmt *
mCc_ast_new_expression(struct mCc_ast_expression *expr_stmt);

struct mCc_ast_stmt *
mCc_ast_new_compound_stmt(struct mCc_ast_compound_stmt *compound_stmt);

void mCc_ast_delete_stmt(struct mCc_ast_stmt *stmt);






struct mCc_ast_if_stmt *
mCc_ast_new_if(struct mCc_ast_expression *ex, struct mCc_ast_stmt * stmt);

struct mCc_ast_if_stmt *
mCc_ast_new_if_else(struct mCc_ast_expression *ex, struct mCc_ast_stmt * stmt, struct mCc_ast_stmt * elsestmt);


struct mCc_ast_while_stmt *
mCc_ast_new_while(struct mCc_ast_expression *ex, struct mCc_ast_stmt * stmt);


struct mCc_ast_ret_stmt *
mCc_ast_new_ret(struct mCc_ast_expression *ex);

struct mCc_ast_ret_stmt *
mCc_ast_new_empty_ret();


struct mCc_ast_assignment *
mCc_ast_new_single_assignment(char * identifier, struct mCc_ast_expression *ex);

struct mCc_ast_assignment *
mCc_ast_new_array_assignment(char * identifier, struct mCc_ast_expression *ex,struct mCc_ast_expression *ex2);


struct mCc_ast_call_expr *
mCc_ast_new_empty_call_expr(char * identifier);

struct mCc_ast_call_expr *
mCc_ast_new_call_expr(char * identifier, struct mCc_ast_argument * arguments);

struct mCc_ast_argument *
mCc_ast_new_single_argument(struct mCc_ast_expression * ex);

struct mCc_ast_argument *
mCc_ast_new_argument_array(struct mCc_ast_argument * arguments, struct mCc_ast_expression * ex);



struct mCc_ast_parameter *
mCc_ast_new_parameter_array(struct mCc_ast_parameter * params, struct mCc_ast_declaration * decl);

struct mCc_ast_parameter *
mCc_ast_new_empty_parameter_array();

struct mCc_ast_parameter *
mCc_ast_new_single_parameter(struct mCc_ast_declaration * decl);

struct mCc_ast_compound_stmt *
mCc_ast_new_single_compound(struct mCc_ast_stmt * stmt);

struct mCc_ast_compound_stmt *
mCc_ast_new_compound_array(struct mCc_ast_compound_stmt* stmts, struct mCc_ast_stmt * stmt);

struct mCc_ast_function_def_array *
mCc_ast_add_function_def_to_array(struct mCc_ast_function_def_array *f, struct mCc_ast_function_def *f2);

struct mCc_ast_function_def_array *
mCc_ast_new_function_def_array(struct mCc_ast_function_def *f);


#ifdef __cplusplus
}
#endif

#endif
