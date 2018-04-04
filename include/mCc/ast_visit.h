#ifndef MCC_AST_VISIT_H
#define MCC_AST_VISIT_H

#include "mCc/ast.h"

#ifdef __cplusplus
extern "C" {
#endif

enum mCc_ast_visit_traversal {
	MCC_AST_VISIT_DEPTH_FIRST,
	/* TODO: MCC_AST_VISIT_BREADTH_FIRST, */
};

enum mCc_ast_visit_order {
	MCC_AST_VISIT_PRE_ORDER,
	MCC_AST_VISIT_POST_ORDER,
};

/* Callbacks */
typedef void (*mCc_ast_visit_expression_cb)(struct mCc_ast_expression *,
                                            void *);
typedef void (*mCc_ast_visit_literal_cb)(struct mCc_ast_literal *, void *);

typedef void (*mCc_ast_visit_single_expression_cb)(struct mCc_ast_single_expression *, void *);

typedef void (*mCc_ast_visit_function_def_cb)(struct mCc_ast_function_def *, void *);

typedef void (*mCc_ast_visit_stmt_cb)(struct mCc_ast_stmt *, void *);
typedef void (*mCc_ast_visit_stmt_if)(struct mCc_ast_if_stmt *, void *);
typedef void (*mCc_ast_visit_stmt_while)(struct mCc_ast_while_stmt *, void *);
typedef void (*mCc_ast_visit_stmt_ret)(struct mCc_ast_ret_stmt *, void *);
typedef void (*mCc_ast_visit_stmt_decl)(struct mCc_ast_declaration *, void *);
typedef void (*mCc_ast_visit_stmt_ass)(struct mCc_ast_assignment *, void *);
typedef void (*mCc_ast_visit_stmt_expr)(struct mCc_ast_expression *, void *);
typedef void (*mCc_ast_visit_stmt_cmp)(struct mCc_ast_compound_stmt *, void *);

typedef void (*mCc_ast_visit_declaration_cb)(struct mCc_ast_declaration *, void *);

typedef void (*mCc_ast_visit_parameter_cb)(struct mCc_ast_parameter *, void *);

typedef void (*mCc_ast_visit_compound_cb)(struct mCc_ast_compound_stmt *, void *);

typedef void (*mCc_ast_visit_argument_cb)(struct mCc_ast_argument *, void *);


struct mCc_ast_visitor {
	enum mCc_ast_visit_traversal traversal;
	enum mCc_ast_visit_order order;

	void *userdata;

	mCc_ast_visit_expression_cb expression;
	mCc_ast_visit_expression_cb expression_single;
	mCc_ast_visit_expression_cb expression_binary;

	mCc_ast_visit_literal_cb literal;
	mCc_ast_visit_literal_cb literal_int;
	mCc_ast_visit_literal_cb literal_float;
	mCc_ast_visit_literal_cb literal_bool;
	mCc_ast_visit_literal_cb literal_string;

	mCc_ast_visit_single_expression_cb single_expression;
	mCc_ast_visit_single_expression_cb single_expression_literal;
	mCc_ast_visit_single_expression_cb single_expression_identifier;
	mCc_ast_visit_single_expression_cb single_expression_identifier_ex;
	mCc_ast_visit_single_expression_cb single_expression_call;
	mCc_ast_visit_single_expression_cb single_expression_unary_op;
	mCc_ast_visit_single_expression_cb single_expression_parenth;

	mCc_ast_visit_function_def_cb function_def;
	mCc_ast_visit_function_def_cb function_def_void;
	mCc_ast_visit_function_def_cb function_def_type;

	mCc_ast_visit_stmt_cb stmt;
	mCc_ast_visit_stmt_if stmt_if;
	mCc_ast_visit_stmt_while stmt_while;
	mCc_ast_visit_stmt_ret stmt_ret;
	mCc_ast_visit_stmt_decl stmt_decl;
	mCc_ast_visit_stmt_ass stmt_ass;
	mCc_ast_visit_stmt_expr stmt_expr;
	mCc_ast_visit_stmt_cmp stmt_compound;

	mCc_ast_visit_declaration_cb declaration;
	mCc_ast_visit_declaration_cb declaration_array;
	mCc_ast_visit_declaration_cb declaration_single;

    mCc_ast_visit_parameter_cb parameter;

    mCc_ast_visit_compound_cb compound;

    mCc_ast_visit_argument_cb argument;
};

void mCc_ast_visit_expression(struct mCc_ast_expression *expression,
                              struct mCc_ast_visitor *visitor);

void mCc_ast_visit_literal(struct mCc_ast_literal *literal,
                           struct mCc_ast_visitor *visitor);

void mCc_ast_visit_single_expression(struct mCc_ast_single_expression *single_expression,
							  struct mCc_ast_visitor *visitor);

void mCc_ast_visit_function_def(struct mCc_ast_function_def *f,
							  struct mCc_ast_visitor *visitor);

void mCc_ast_visit_stmt(struct mCc_ast_stmt *stmt,
							  struct mCc_ast_visitor *visitor);

void mCc_ast_visit_declaration(struct mCc_ast_declaration *decl,
							   struct mCc_ast_visitor *visitor);

void mCc_ast_visit_parameter(struct mCc_ast_parameter * param,
                             struct mCc_ast_visitor * visitor);

#ifdef __cplusplus
}
#endif

#endif
