#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mCc/ast_print.h>

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

	struct mCc_ast_function_def_array *expr = NULL;

    printf("A\n\n");
	/* parsing phase */
	{
		struct mCc_parser_result result = mCc_parser_parse_file(in);
		fclose(in);
        printf("Zs\n\n");
		if (result.status != MCC_PARSER_STATUS_OK) {
            printf("Parser_error");
			return EXIT_FAILURE;
		}
		expr = result.func_def;
	}

	/*    TODO
	 * - run semantic checks
	 * - create three-address code
	 * - do some optimisations
	 * - output assembly code
	 * - invoke backend compiler
	 */

	printf("output");
    FILE *out;
    out = fopen("output.txt","w");
    
    mCc_ast_print_dot_function_def(out,expr);
    

	/* cleanup */
	mCc_ast_delete_function_def(expr);

	return EXIT_SUCCESS;
}
