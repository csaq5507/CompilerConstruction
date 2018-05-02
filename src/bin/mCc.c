#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mCc/ast_print.h>
#include <mCc/ast_symbol_table.h>
#include <mCc/ast_semantic_checks.h>

#include "mCc/ast.h"
#include "mCc/parser.h"

#define DEBUG 1

void print_usage(const char *prg)
{
	printf("usage: %s <FILE>\n\n", prg);
	printf("  <FILE>        Input filepath or - for stdin\n");
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		print_usage(argv[0]);
		return EXIT_FAILURE;
	}

	/* determine input source */
	FILE *in;
	if (strcmp("-", argv[1]) == 0) {
		in = stdin;
	} else {
		in = fopen(argv[1], "r");
		if (!in) {
			perror("fopen");
			return EXIT_FAILURE;
		}
	}

	struct mCc_ast_function_def_array *func = NULL;

	/* parsing phase */
	{
		struct mCc_parser_result result = mCc_parser_parse_file(in);
		fclose(in);
		if (result.status == MCC_PARSER_STATUS_ERROR) {
			if (DEBUG) {
				printf("Parser_error:\n");
				for (int i = 0; i < result.errors->counter;
				     i++) {
					printf("Error at line %d: ",
					       result.errors->errors[i]
						       .error_line);
					printf("%s\n", result.errors->errors[i]
							       .error_msg);
				}
				mCc_ast_delete_function_def_array(
					result.func_def);
				mCc_delete_result(&result);
				return EXIT_SUCCESS;
			}
		}
		func = result.func_def;

		FILE *out1;
		FILE *out2;
		out1 = fopen("output1.txt", "w");
		out2 = fopen("output2.txt", "w");

		mCc_ast_print_dot_function_def(out1, func);

		result = *(mCc_ast_symbol_table(&result));

		mCc_ast_print_dot_function_def(out2, result.func_def);

		fclose(out1);
		fclose(out2);

		if (result.status == MCC_PARSER_STATUS_ERROR) {
			if (DEBUG) {
				printf("Parser_error:\n");
				for (int i = 0; i < result.errors->counter;
				     i++) {
					printf("Error at line %d: ",
					       result.errors->errors[i]
						       .error_line);
					printf("%s\n", result.errors->errors[i]
							       .error_msg);
				}
				mCc_ast_delete_function_def_array(
					result.func_def);
				mCc_delete_result(&result);
				return EXIT_SUCCESS;
			}
		}

		result = *(mCc_ast_semantic_check(&result));

		if (result.status == MCC_PARSER_STATUS_ERROR) {
			if (DEBUG) {
				printf("Parser_error:\n");
				for (int i = 0; i < result.errors->counter;
				     i++) {
					printf("Error at line %d: ",
					       result.errors->errors[i]
						       .error_line);
					printf("%s\n", result.errors->errors[i]
							       .error_msg);
				}
				mCc_ast_delete_function_def_array(
					result.func_def);
				mCc_delete_result(&result);
				return EXIT_SUCCESS;
			}
		}

		mCc_ast_delete_function_def_array(result.func_def);
		mCc_delete_result(&result);
	}

	/*    TODO
	 * - create three-address code
	 * - do some optimisations
	 * - output assembly code
	 * - invoke backend compiler
	 */


	/* cleanup */

	return EXIT_SUCCESS;
}
