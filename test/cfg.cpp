#include <gtest/gtest.h>
#include <stdlib.h>
#include <stdio.h>
#include "mCc/ast.h"
#include "mCc/parser.h"
#include "mCc/ast_symbol_table.h"
#include "mCc/ast_semantic_checks.h"
#include "mCc/tac.h"
#include "mCc/cfg.h"

TEST(cfg, Simplegeneration)
{
    const char input[] = "void main(){\n"
            "    int a;\n"
            "    int b;\n"
            "    int c;\n"
            "    a = 1;\n"
            "    b = 5;\n"
            "    c = a + b;\n"
            "}";

    auto result = mCc_parser_parse_string(input);

    result = *(mCc_ast_symbol_table(&result));
    result = *(mCc_ast_semantic_check(&result));

    struct mCc_tac_list *tac;
    tac = mCc_tac_generate(result.func_def);

    ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

    mCc_delete_result(&result);

    cfg_list *cfg = mCc_cfg_generate(tac);

    ASSERT_EQ(cfg->next_nodes[0].num_next_nodes, 0);

    mCc_cfg_delete(cfg);

    mCc_tac_delete(tac);
}

TEST(cfg, Complexgeneration)
{
    const char input[] = "void main(){\n"
            "    int a;\n"
            "    int b;\n"
            "    int c;\n"
            "    a = 1;\n"
            "    b = 5;\n"
            "    if (a == 1) {\n"
            "        c = a + b;\n"
            "    } else if (a == 1) {\n"
            "       c = a + b;\n"
            "    } else {\n"
            "        c = a - b;\n"
            "    }\n"
            "    int d;\n"
            "    d = 5;\n"
            "    while (d < 10) {\n"
            "        d = d + 1;\n"
            "    }\n"
            "\n"
            "}";

    auto result = mCc_parser_parse_string(input);

    result = *(mCc_ast_symbol_table(&result));
    result = *(mCc_ast_semantic_check(&result));

    struct mCc_tac_list *tac;
    tac = mCc_tac_generate(result.func_def);

    ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

    mCc_delete_result(&result);

    cfg_list *cfg = mCc_cfg_generate(tac);

    ASSERT_EQ(cfg->next_nodes[0].num_next_nodes, 2);
    ASSERT_EQ(cfg->next_nodes[0].next_nodes[0].next_nodes[0].next_nodes[0].next_nodes[0].num_next_nodes, 1);

    mCc_cfg_delete(cfg);

    mCc_tac_delete(tac);
}

