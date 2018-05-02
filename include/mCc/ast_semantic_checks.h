
#ifndef MINICOMPILER_AST_SEMANTIC_CHECKS_H
#define MINICOMPILER_AST_SEMANTIC_CHECKS_H

#include <stdio.h>

#include "mCc/ast.h"
#include "ast_visit.h"
#include "parser.h"

#ifdef __cplusplus
extern "C" {
#endif


struct mCc_parser_result *
mCc_ast_semantic_check(struct mCc_parser_result *result);


struct mCc_parser_result *g_result;

#ifdef __cplusplus
}
#endif
#endif // MINICOMPILER_AST_SEMANTIC_CHECKS_H
