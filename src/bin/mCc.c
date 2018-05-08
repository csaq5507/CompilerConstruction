#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mCc/ast_print.h>
#include <mCc/ast_symbol_table.h>
#include <mCc/ast_semantic_checks.h>
#include <mCc/tac.h>

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
    char *string = malloc(sizeof(char*) * strlen(argv[1]));
    strcpy(string, argv[1]);
    char delimiter_slash[] = "/";
    char *splits;
    char *last;

    last = string;
    splits = strtok(string, delimiter_slash);

    while(splits != NULL) {
        splits = strtok(NULL, delimiter_slash);
        if (splits != NULL)
            last = splits;
    }


    char *file_name = last;

    FILE *file_std_err;
    file_std_err = fopen("std.err", "a");

    /* Create Error file */
    char delimiter_point[] = ".";
    splits = strtok(last, delimiter_point);

    if(splits != NULL) {
        file_name = splits;
    }


    FILE *file_error;
    strcat(file_name, ".error");

    file_error=fopen(file_name,"w");


    /* Create Graph file */
    splits = strtok(last, delimiter_point);

    if(splits != NULL) {
        file_name = splits;
    }

    FILE *file_graph;
    strcat(file_name, ".graph");

    file_graph=fopen(file_name,"w");


    /* Create Three Address Code file */
    splits = strtok(last, delimiter_point);

    if(splits != NULL) {
        file_name = splits;
    }

    FILE *file_tac;
    strcat(file_name, ".tac");

    file_tac=fopen(file_name,"w");


	if (argc < 2) {
		print_usage(argv[0]);
		return EXIT_FAILURE;
	}


    /* Print only file name at top of common error file */
    splits = strtok(last, delimiter_point);

    if(splits != NULL) {
        file_name = splits;
    }

    fprintf(file_std_err,"--------------------------------------%s--------------------------------------\n", file_name);


    /* free file name string */
    free(string);


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


	/* parsing phase */
	{
		struct mCc_parser_result result = mCc_parser_parse_file(in);
		fclose(in);
		if (result.status == MCC_PARSER_STATUS_ERROR) {
			if (DEBUG) {
				fprintf(file_error, "Parser_error:\n");
				for (int i = 0; i < result.errors->counter;
				     i++) {
                    fprintf(file_std_err, "Error at line %d: ",
                            result.errors->errors[i]
                                    .error_line);
                    fprintf(file_std_err, "%s\n", result.errors->errors[i]
                            .error_msg);

					fprintf(file_error, "Error at line %d: ",
					       result.errors->errors[i]
						       .error_line);
					fprintf(file_error, "%s\n", result.errors->errors[i]
							       .error_msg);
				}
				mCc_ast_delete_function_def_array(
					result.func_def);
				mCc_delete_result(&result);
                fclose(file_error);
                fclose(file_graph);
                fclose(file_tac);
                fclose(file_std_err);
				return EXIT_SUCCESS;
			}
		}


		result = *(mCc_ast_symbol_table(&result));

		mCc_ast_print_dot_function_def(file_graph, result.func_def);


		if (result.status == MCC_PARSER_STATUS_ERROR) {
			if (DEBUG) {
				fprintf(file_error, "Semantic_error:\n");
				for (int i = 0; i < result.errors->counter;
				     i++) {
                    fprintf(file_std_err, "Error at line %d: ",
                            result.errors->errors[i]
                                    .error_line);
                    fprintf(file_std_err, "%s\n", result.errors->errors[i]
                            .error_msg);

					fprintf(file_error, "Error at line %d: ",
					       result.errors->errors[i]
						       .error_line);
					fprintf(file_error, "%s\n", result.errors->errors[i]
							       .error_msg);
				}
				mCc_ast_delete_function_def_array(
					result.func_def);
				mCc_delete_result(&result);
                fclose(file_error);
                fclose(file_graph);
                fclose(file_tac);
                fclose(file_std_err);
				return EXIT_SUCCESS;
			}
		}

		result = *(mCc_ast_semantic_check(&result));

            if (result.status == MCC_PARSER_STATUS_ERROR) {
			if (DEBUG) {
				fprintf(file_error, "Semantic_error:\n");
				for (int i = 0; i < result.errors->counter;
				     i++) {
                    fprintf(file_std_err, "Error at line %d: ",
                            result.errors->errors[i]
                                    .error_line);
                    fprintf(file_std_err, "%s\n", result.errors->errors[i]
                            .error_msg);

					fprintf(file_error, "Error at line %d: ",
					       result.errors->errors[i]
						       .error_line);
					fprintf(file_error, "%s\n", result.errors->errors[i]
							       .error_msg);
				}
				mCc_ast_delete_function_def_array(
					result.func_def);
				mCc_delete_result(&result);
                fclose(file_error);
                fclose(file_graph);
                fclose(file_tac);
                fclose(file_std_err);
				return EXIT_SUCCESS;
			}
		}

        fprintf(file_std_err,"No Errors\n");

		struct mCc_tac_list *tac;
		tac = mCc_tac_generate(result.func_def);

        fclose(file_error);
        fclose(file_graph);
		mCc_ast_delete_function_def_array(result.func_def);
		mCc_delete_result(&result);

		 mCc_tac_print(file_tac, tac);
        //mCc_tac_print(stdout, tac);
		mCc_tac_delete(tac);

		fclose(file_tac);
		fclose(file_std_err);
	}

	/*    TODO
	 * - do some optimisations
	 * - output assembly code
	 * - invoke backend compiler
	 */


	/* cleanup */

	return EXIT_SUCCESS;
}
