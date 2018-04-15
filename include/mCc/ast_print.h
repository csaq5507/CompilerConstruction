#ifndef MCC_AST_PRINT_H
#define MCC_AST_PRINT_H

#include <stdio.h>

#include "mCc/ast.h"
#include "ast_visit.h"

#ifdef __cplusplus
extern "C" {
#endif

void mCc_ast_print_dot_function_def(FILE *out, struct mCc_ast_function_def_array *f);



#ifdef __cplusplus
}
#endif

#endif
