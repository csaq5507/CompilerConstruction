#ifndef MCC_AST_PRINT_H
#define MCC_AST_PRINT_H

#include <stdio.h>

#include "mCc/ast.h"

#ifdef __cplusplus
extern "C" {
#endif


static void print_dot_begin(FILE *out);
static void print_dot_end(FILE *out);
static void print_dot_node(FILE *out, const void *node, const char *label);
static void print_dot_edge(FILE *out, const void *src_node, const void *dst_node, const char *label);

static void print_dot_expression(struct mCc_ast_expression *expression, void *data);
static void print_dot_expression_single(struct mCc_ast_single_expression *expression, void *data);
static void print_dot_expression_binary(struct mCc_ast_expression *expression, void *data);
static void print_dot_parameter(struct mCc_ast_parameter *param, void *data);
static void print_dot_stmt_statement(struct mCc_ast_stmt *stmt, void *data);
static void print_dot_stmt_if(struct mCc_ast_if_stmt *stmt, void *data);
static void print_dot_stmt_while(struct mCc_ast_while_stmt *stmt, void *data);
static void print_dot_stmt_ret(struct mCc_ast_ret_stmt *stmt, void *data);
static void print_dot_stmt_compound(struct mCc_ast_compound_stmt *stmt, void *data);
static void print_dot_stmt_ass(struct mCc_ast_assignment *stmt, void *data);
static void print_dot_stmt_expr(struct mCc_ast_expression *stmt, void *data);
static void print_dot_stmt_decl(struct mCc_ast_declaration *stmt, void *data);
static void print_dot_compound_stmt(struct mCc_ast_compound_stmt * c_stmt, void *data);
static void print_dot_function_def_void(struct mCc_ast_function_def *f, void *data);
static void print_dot_function_def_type(struct mCc_ast_function_def *f, void *data);
static void print_dot_call_expr(struct mCc_ast_call_expr *expression, void *data);

void mCc_ast_print_dot_function_def(FILE *out, struct mCc_ast_function_def_array *f);


#ifdef __cplusplus
}
#endif

#endif
