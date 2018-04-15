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
//typedef void (*mCc_ast_visit_expression_cb)(struct mCc_ast_expression *,
//                                           void *);

typedef void (*mCc_ast_identifier_cb)(char *, void *);

typedef void (*mCc_ast_function_def_type_cb)(struct mCc_ast_function_def *,
											 void *);
typedef void (*mCc_ast_function_def_void_cb)(struct mCc_ast_function_def *,
											 void *);
typedef void (*mCc_ast_function_def_identifier_cb)(struct mCc_ast_function_def *,
												   void *);
typedef void (*mCc_ast_function_def_parameter_cb)(struct mCc_ast_function_def *,
                                                   void *);
typedef void (*mCc_ast_function_def_stmt_cb)(struct mCc_ast_function_def *,
                                                  void *);

typedef void(*mCc_ast_stmt_cb)(struct mCc_ast_compound_stmt *, void *);
typedef void(*mCc_ast_if_stmt_cb)(struct mCc_ast_if_stmt *, void *);
typedef void(*mCc_ast_while_stmt_cb)(struct mCc_ast_while_stmt *, void *);
typedef void(*mCc_ast_ret_stmt_cb)(struct mCc_ast_ret_stmt *, void *);
typedef void(*mCc_ast_decl_stmt_cb)(struct mCc_ast_declaration *, void *);
typedef void(*mCc_ast_ass_stmt_cb)(struct mCc_ast_assignment *, void *);
typedef void(*mCc_ast_ass_stmt_statement_cb)(struct mCc_ast_stmt*, void *);


typedef void(*mCc_ast_expression_cb)(struct mCc_ast_expression *, void *);


struct mCc_ast_visitor {
	enum mCc_ast_visit_traversal traversal;
	enum mCc_ast_visit_order order;

	void *userdata;

	mCc_ast_identifier_cb identifier;

	mCc_ast_function_def_type_cb function_def_type;
    mCc_ast_function_def_void_cb function_def_void;
    mCc_ast_function_def_identifier_cb function_def_identifier;
    mCc_ast_function_def_parameter_cb function_def_parameter;
    mCc_ast_function_def_stmt_cb function_def_stmt;

    mCc_ast_stmt_cb c_stmt;
    mCc_ast_if_stmt_cb if_stmt;
    mCc_ast_while_stmt_cb while_stmt;
    mCc_ast_ret_stmt_cb ret_stmt;
    mCc_ast_decl_stmt_cb decl_stmt;
    mCc_ast_ass_stmt_cb ass_stmt;
    mCc_ast_ass_stmt_statement_cb statement;

    mCc_ast_expression_cb expression;

};

void mCc_ast_visit_function_def_array(struct mCc_ast_function_def_array *f,
							  struct mCc_ast_visitor *visitor);
void mCc_ast_visit_function_def_type(struct mCc_ast_function_def *f,
									  struct mCc_ast_visitor *visitor);
void mCc_ast_visit_function_def_void(struct mCc_ast_function_def *f,
									  struct mCc_ast_visitor *visitor);

void mCc_ast_visit_parameter(struct mCc_ast_parameter *param,
                             struct mCc_ast_visitor *visitor);

void mCc_ast_visit_stmt(struct mCc_ast_compound_stmt *stmt,
                        struct mCc_ast_visitor *visitor);
void mCc_ast_visit_compound_stmt(struct mCc_ast_compound_stmt *c_stmt,
                        struct mCc_ast_visitor *visitor);
void mCc_ast_visit_if_stmt(struct mCc_ast_if_stmt *stmt,
                           struct mCc_ast_visitor *visitor);

#ifdef __cplusplus
}
#endif

#endif
