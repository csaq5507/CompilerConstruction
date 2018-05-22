#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mCc/ast_print.h>
#include <mCc/ast_symbol_table.h>
#include <mCc/ast_semantic_checks.h>
#include <mCc/tac.h>
#include <mCc/utils.h>
#include <zconf.h>

#include "mCc/ast.h"
#include "mCc/parser.h"

#define VERSION 0.2

void print_usage(const char *prg)
{
	printf("usage: %s <FILE> \n\n", prg);
	printf("\t<FILE>\t\t\t\t\tInput filepath or - for stdin\n");
	printf("\t-t || -tac\t\t\t\tCreate TAC file\n");
	printf("\t-g || --graph\t\t\tCreate graph file in DOT format\n");
	printf("\t-o || --output <FILE>\tOutput filepath or - for stdout\n");
}

void print_error(const char *prg, const char *arg)
{
	printf("Usage Error on argument %s.\n", arg);
	print_usage(prg);
}

void compiler_error(const char * msg, FILE *error,
		    struct mCc_parser_result *result, ...)
{
    va_list args;
    va_start(args, msg);
	vfprintf(error, msg, args);
    va_end(args);
    for (int i = 0; i < result->errors->counter; i++) {
		fprintf(error, "Error at line %d: ",
			result->errors->errors[i].error_line);
		fprintf(error, "%s\n", result->errors->errors[i].error_msg);
	}
}

void clean_up(FILE * error, FILE * graph, FILE * tac, FILE * file_std_err,FILE * output,
            char * tacFileName,char * graphFileName,char * errorFileName,char * inputFileName,char * outputFileName){
    fclose(error);
    fclose(graph);
    fclose(tac);
    fclose(file_std_err);
    fclose(output);
    free(tacFileName);
    free(graphFileName);
    free(errorFileName);
    free(inputFileName);
    free(outputFileName);
}

int main(int argc, char *argv[])
{

	if (argc < 2) {
		print_usage(argv[0]);
		return EXIT_FAILURE;
	}
	char *outputFileName = "a.out";
	char *inputFileName = NULL;
	bool print_graph = false;
	bool print_tac = false;
	FILE *inputFile = NULL;
	FILE *output = NULL;
	FILE *tac = NULL;
	FILE *graph = NULL;
	FILE *error = NULL;
	FILE *file_std_err = NULL;
	file_std_err = fopen("std.err", "a");

	if (file_std_err == NULL) {
		perror("fopen std.err file");
		return EXIT_FAILURE;
	}
	for (int i = 1; i < argc; i++) {
		char *temp = argv[i];
		if (!strcmp(temp, "-h") || !strcmp(temp, "--help")) {
			print_usage(argv[0]);
			return EXIT_SUCCESS;
		} else if (!strcmp(temp, "-v") || !strcmp(temp, "--version")) {
			printf("%s Version: %f", argv[0], VERSION);
			return EXIT_SUCCESS;
		} else if (!strcmp(temp, "-g") || !strcmp(temp, "--graph"))
			print_graph = true;
		else if (!strcmp(temp, "-t") || !strcmp(temp, "--tac"))
			print_tac = true;
		else if (!strcmp(temp, "-o") || !strcmp(temp, "--output")) {
			i++;
			if (argc <= i)
				print_error(argv[0], temp);

			if(!strcmp(argv[i], "-"))
				output = stdout;
			else
				outputFileName = copy_string(argv[i]);
		} else if (!strcmp(temp, "-")) {
			inputFile = stdin;
		} else {
			if (inputFile != NULL) {
				print_error(argv[0], temp);
				return EXIT_FAILURE;
			}
			inputFileName = copy_string(temp);
			inputFile = fopen(temp, "r");
			if (!inputFile) {
				print_error(argv[0], temp);
				return EXIT_FAILURE;
			}
		}
	}

	if (inputFile != stdin && output == NULL) {
		char *string =
			malloc(sizeof(char) * (strlen(inputFileName) + 1));
		strcpy(string, inputFileName);
		char delimiter_slash[] = "/";
		char *splits;
		char *last;

		last = string;
		splits = strtok(string, delimiter_slash);

		while (splits != NULL) {
			splits = strtok(NULL, delimiter_slash);
			if (splits != NULL)
				last = splits;
		}

		/* Create Error file */
		char delimiter_point[] = ".";
		splits = strtok(last, delimiter_point);

		if (splits != NULL) {
			outputFileName = copy_string(splits);
		}
        free(string);
	}

	if(output==NULL) {
		output = fopen(outputFileName, "w");
		if (output == NULL) {
			perror("fopen output files");
			return EXIT_FAILURE;
		}
	}
	char *tacFileName = new_string("%s%s", output, ".tac");
	char *graphFileName = new_string("%s%s", output, ".graph");
	char *errorFileName = new_string("%s%s", output, ".error");
	tac = fopen(tacFileName, "w");
	graph = fopen(graphFileName, "w");
	error = fopen(errorFileName, "w");
	if (error == NULL || graph == NULL || tac == NULL) {
		perror("fopen output files");
		return EXIT_FAILURE;
	}


	/* parsing phase */
	struct mCc_parser_result result = mCc_parser_parse_file(inputFile);
	fclose(inputFile);
	if (result.status == MCC_PARSER_STATUS_ERROR) {
		compiler_error("Parse Error in file %s:\n", error, &result,inputFileName);
		compiler_error("Parse Error in file %s:\n", file_std_err, &result,inputFileName);

		mCc_ast_delete_function_def_array(result.func_def);
		mCc_delete_result(&result);
        clean_up(error,graph,tac,file_std_err,output,tacFileName,graphFileName,errorFileName,inputFileName,outputFileName);
		return EXIT_SUCCESS;
	}

	result = *(mCc_ast_symbol_table(&result));

	if (print_graph)
		mCc_ast_print_dot_function_def(graph, result.func_def);

	if (result.status == MCC_PARSER_STATUS_ERROR) {
		compiler_error("Semantic_error in file %s:\n", error, &result,inputFileName);
		compiler_error("Semantic_error in file %s:\n", file_std_err, &result,inputFileName);
		mCc_ast_delete_function_def_array(result.func_def);
		mCc_delete_result(&result);
        clean_up(error,graph,tac,file_std_err,output,tacFileName,graphFileName,errorFileName,inputFileName,outputFileName);

		return EXIT_SUCCESS;
	}


	result = *(mCc_ast_semantic_check(&result));

	if (result.status == MCC_PARSER_STATUS_ERROR) {
		compiler_error("Semantic_error in file %s:\n", error, &result,inputFileName);
		compiler_error("Semantic_error in file %s:\n", file_std_err, &result,inputFileName);
		mCc_ast_delete_function_def_array(result.func_def);
		mCc_delete_result(&result);
        clean_up(error,graph,tac,file_std_err,output,tacFileName,graphFileName,errorFileName,inputFileName,outputFileName);

		return EXIT_SUCCESS;
	}


	struct mCc_tac_list *_tac;
	_tac = mCc_tac_generate(result.func_def);


	mCc_ast_delete_function_def_array(result.func_def);
	mCc_delete_result(&result);

	if (print_tac)
		mCc_tac_print(tac, _tac);

	mCc_tac_delete(_tac);

    clean_up(error,graph,tac,file_std_err,output,tacFileName,graphFileName,errorFileName,inputFileName,outputFileName);

	//   printf("%d", sizeof(struct mCc_tac_list));

	/*    TODO
	 * - do some optimisations
	 * - output assembly code
	 * - invoke backend compiler
	 */


	/* cleanup */

	return EXIT_SUCCESS;
}
