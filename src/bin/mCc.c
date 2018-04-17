#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mCc/ast_print.h>
#include <mCc/ast_symbol_table.h>

#include "mCc/ast.h"
#include "mCc/parser.h"

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
		if (result.status != MCC_PARSER_STATUS_OK) {
			printf("Parser_error:\n");
			for (int i = 0; i < result.errors->counter; i++) {
				printf("Error at line %d\n",
				       result.errors->errors[i].error_line);
				printf("%s\n",
				       result.errors->errors[i].error_msg);
			}
			return EXIT_FAILURE;
		}
        func = result.func_def;
	}

	/*    TODO
	 * - run semantic checks
	 * - create three-address code
	 * - do some optimisations
	 * - output assembly code
	 * - invoke backend compiler
	 */

	FILE *out1;
	FILE *out2;
	out1 = fopen("output1.txt", "w");
	out2 = fopen("output2.txt", "w");

	mCc_ast_print_dot_function_def(out1, func);

	func = mCc_ast_symbol_table(func);

	//mCc_ast_print_dot_function_def(out2, func);

	fclose(out1);
	fclose(out2);

	/* cleanup */
	mCc_ast_delete_function_def_array(func);

	return EXIT_SUCCESS;
 }
