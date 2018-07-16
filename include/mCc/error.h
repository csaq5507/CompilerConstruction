//
// Created by ivan on 09.05.18.
//

#ifndef MINICOMPILER_ERROR_H
#define MINICOMPILER_ERROR_H

#include <stdio.h>

#include "mCc/ast.h"

#ifdef __cplusplus
extern "C" {
#endif


// SYMBOL_TABLE
extern const char *ERROR_MAIN_NOT_VOID;
extern const char *ERROR_NO_MAIN;
extern const char *ERROR_DUBLICATE_FUNCTION;
extern const char *ERROR_DUBLICATE_VARIABLE;
extern const char *ERROR_NO_RETURN;
extern const char *ERROR_MISSING_VARIABLE_DEF;
extern const char *ERROR_MISSING_FUNCTION_DEF;
extern const char *ERROR_NUM_ARGUMENTS;

// SEMANTIC_CHECKS
extern const char *ERROR_BINARY_EX_TYPE_MISSMATCH;
extern const char *ERROR_WRONG_PARAMETER_TYPE;
extern const char *ERROR_WRONG_RETURN_TYPE;
extern const char *ERROR_WRONG_ASSIGNMENT_TYPE;
extern const char *ERROR_WRONG_ASSIGNMENT_SIZE;
extern const char *ERROR_CONDITION_NOT_BOOLEAN;



void mCc_add_error(const char *error_msg, int error_line,
		   struct mCc_parser_result *result);


#ifdef __cplusplus
}
#endif

#endif // MINICOMPILER_ERROR_H
