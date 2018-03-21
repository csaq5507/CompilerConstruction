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
	mCc_ast_visit_single_expression_cb single_expression_call;
	mCc_ast_visit_single_expression_cb single_expression_unary_op;
	mCc_ast_visit_single_expression_cb single_expression_parenth;

	mCc_ast_visit_function_def_cb function_def;
	mCc_ast_visit_function_def_cb function_def_void;
	mCc_ast_visit_function_def_cb function_def_type;

	mCc_ast_visit_stmt_cb stmt;
	mCc_ast_visit_stmt_cb stmt_if;
	mCc_ast_visit_stmt_cb stmt_while;
	mCc_ast_visit_stmt_cb stmt_ret;
	mCc_ast_visit_stmt_cb stmt_decl;
	mCc_ast_visit_stmt_cb stmt_ass;
	mCc_ast_visit_stmt_cb stmt_expr;
	mCc_ast_visit_stmt_cb stmt_compound;

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

#ifdef __cplusplus
}
#endif

#endif
