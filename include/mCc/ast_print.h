#ifndef MCC_AST_PRINT_H
#define MCC_AST_PRINT_H

#include <stdio.h>

#include "mCc/ast.h"

#ifdef __cplusplus
extern "C" {
#endif

const char *mCc_ast_print_binary_op(enum mCc_ast_binary_op op);
const char *mCc_ast_print_unary_op(enum mCc_ast_unary_op op);

/* ------------------------------------------------------------- DOT Printer */

void mCc_ast_print_dot_expression(FILE *out,
                                  struct mCc_ast_expression *expression);

void mCc_ast_print_dot_literal(FILE *out, struct mCc_ast_literal *literal);

void mCc_ast_print_dot_single_expression(FILE *out, struct mCc_ast_single_expression * single_expression);

void mCc_ast_print_dot_function_def(FILE *out, struct mCc_ast_function_def_array *f);

void mCc_ast_print_dot_declaration(FILE *out, struct mCc_ast_declaration *f);

#ifdef __cplusplus
}
#endif

#endif
