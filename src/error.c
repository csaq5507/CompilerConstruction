//
// Created by ivan on 09.05.18.
//

#include <stdlib.h>
#include <memory.h>
#include <mCc/parser.h>
#include "error.h"


void mCc_add_error(const char *error_msg, int error_line,
		   struct mCc_parser_result *result)
{


	struct mCc_parser_error *error =
		malloc(sizeof(struct mCc_parser_error));
	strcpy(error->error_msg, error_msg);
	error->error_line = error_line;
	result->errors = add_parse_error(result->errors, error);
	result->status = MCC_PARSER_STATUS_ERROR;
}


// SYMBOL_TABLE
const char *ERROR_MAIN_NOT_VOID = "main function has return of type %s";
const char *ERROR_NO_MAIN = "missing main function";
const char *ERROR_DUBLICATE_FUNCTION = "function %s already defined";
const char *ERROR_DUBLICATE_VARIABLE = "variable %s already defined";
const char *ERROR_NO_RETURN = "missing return for function %s";
const char *ERROR_MISSING_VARIABLE_DEF = "missing definition of variable %s";
const char *ERROR_MISSING_FUNCTION_DEF = "missing definition of function %s";
const char *ERROR_NUM_ARGUMENTS = "function %s expects %d arguments, %d given";

// SEMANTIC_CHECKS
const char *ERROR_BINARY_EX_TYPE_MISSMATCH =
	"expression types not compatible: %s %s %s";
const char *ERROR_WRONG_PARAMETER_TYPE =
	"function %s expect %d. parameter of type %s, %s given";
const char *ERROR_WRONG_RETURN_TYPE =
	"wrong return of type %s for function, expected %s";
const char *NOT_ALLOWED_ASSIGNMENT =
	"array on left of assignment %s not allowed";
const char *ERROR_WRONG_ASSIGNMENT_TYPE =
	"assignment types not compatible: %s = %s";
const char *ERROR_CONDITION_NOT_BOOLEAN =
	"condition of type %s, expected boolean";
