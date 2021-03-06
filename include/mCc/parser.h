#ifndef MCC_PARSER_H
#define MCC_PARSER_H

#include <stdio.h>
#include <stdlib.h>

#include "mCc/ast.h"

#ifdef __cplusplus
extern "C" {
#endif

enum mCc_parser_status {
	MCC_PARSER_STATUS_OK,
	MCC_PARSER_STATUS_ERROR,
	MCC_PARSER_STATUS_UNABLE_TO_OPEN_STREAM
};

struct mCc_parser_result {
	enum mCc_parser_status status;
	bool has_toplevel;
	struct mCc_parser_error_array *errors;
	struct mCc_ast_function_def_array *func_def;
};

struct mCc_parser_error_array {
	int counter;
	struct mCc_parser_error *errors;
};

struct mCc_parser_error {
	char error_msg[1024];
	int error_line;
};

struct mCc_parser_error_array *new_parse_error_array();

struct mCc_parser_error_array *
add_parse_error(struct mCc_parser_error_array *array,
		struct mCc_parser_error *error);

struct mCc_parser_result mCc_parser_parse_string(const char *input);

struct mCc_parser_result mCc_parser_parse_file(FILE *input);

void mCc_delete_result(struct mCc_parser_result *result);


#ifdef __cplusplus
}
#endif

#endif
