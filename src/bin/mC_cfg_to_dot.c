#include <stdio.h>
#include <stdlib.h>
#include <mCc/ast_print.h>
#include <mCc/ast_symbol_table.h>
#include <mCc/ast_semantic_checks.h>
#include <mCc/tac.h>
#include <mCc/utils.h>
#include <mCc/cfg.h>


void print_usage(const char *prg)
{
    printf("usage: %s <FILE> \n\n", prg);
    printf("\t<FILE>\t\t\t\t\tInput filepath\n");
}

void print_error(const char *prg, const char *arg)
{
    printf("Usage Error on argument %s.\n", arg);
    print_usage(prg);
}

int main(int argc, char *argv[])
{

    if (argc < 2) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }
    FILE *inputFile = fopen(argv[1], "r");
    if (!inputFile) {
        print_error(argv[0], argv[1]);
        return EXIT_FAILURE;
    }

    /* parsing phase */
    struct mCc_parser_result result = mCc_parser_parse_file(inputFile);
    fclose(inputFile);
    if (result.status == MCC_PARSER_STATUS_ERROR) {
        printf("Parse Error\n");
        mCc_delete_result(&result);
        return EXIT_FAILURE;
    }

    result = *(mCc_ast_symbol_table(&result));

    if (result.status == MCC_PARSER_STATUS_ERROR) {
        printf("Semantic Error\n");
        mCc_delete_result(&result);
        return EXIT_FAILURE;
    }

    result = *(mCc_ast_semantic_check(&result));

    if (result.status == MCC_PARSER_STATUS_ERROR) {
        printf("Semantic Error\n");
        mCc_delete_result(&result);
        return EXIT_FAILURE;
    }

    struct mCc_tac_list *_tac;
    _tac = mCc_tac_generate(result.func_def);

    cfg_list *cfg = mCc_cfg_generate(_tac);

    FILE *out;
    char *outputFileName;

    for (int i = 0; i < cfg->num_next_nodes; i++) {
        outputFileName = new_string("%s%s",
                                    cfg->next_nodes[i].tac_start->prev->identifier1, ".cfg");
        out = fopen(outputFileName, "w");
        mCc_cfg_print_single_function(out, &cfg->next_nodes[i]);
        free(outputFileName);
        fclose(out);
    }

    mCc_cfg_delete(cfg);
    mCc_tac_delete(_tac);
    mCc_delete_result(&result);


    return EXIT_SUCCESS;
}
