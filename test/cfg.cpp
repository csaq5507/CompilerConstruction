#include <gtest/gtest.h>
#include <stdlib.h>
#include <stdio.h>
#include "mCc/ast.h"
#include "mCc/parser.h"
#include "mCc/ast_symbol_table.h"
#include "mCc/ast_semantic_checks.h"
#include "mCc/tac.h"
#include "mCc/cfg.h"

/*
 * Above each test the graph is shown in some graphical way (If
 * the graph is not to complex).
 *
 * Each node gets simple a number. Afterwards in the test the
 * same number is given to a variable to make some tests.
 */

/*
 * HEAD -> 1
 */
TEST(cfg, SimpleFunction)
{
    const char input[] = ""
            "void main(){\n"
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

    cfg_list *cfg_head = mCc_cfg_generate(tac);

    ASSERT_EQ(cfg_head->num_next_nodes, 1);

    cfg_list *cfg_1 = &cfg_head->next_nodes[0];

    ASSERT_EQ(cfg_1->num_next_nodes, 0);
    ASSERT_EQ(cfg_1->num_prev_nodes, 1);

    mCc_cfg_delete(cfg_head);

    mCc_tac_delete(tac);
}

/*
 *      HEAD
 *       /\
 *      1 2
 */
TEST(cfg, DoubleFunction)
{
    const char input[] = ""
            "void f() {\n"
            "   int a;\n"
            "   int b;\n"
            "   int c;\n"
            "}\n"
            "void main(){\n"
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

    cfg_list *cfg_head = mCc_cfg_generate(tac);

    ASSERT_EQ(cfg_head->num_next_nodes, 2);

    cfg_list *cfg_1 = &cfg_head->next_nodes[0];
    ASSERT_EQ(cfg_1->num_next_nodes, 0);
    ASSERT_EQ(cfg_1->num_prev_nodes, 1);


    cfg_list *cfg_2 = &cfg_head->next_nodes[1];
    ASSERT_EQ(cfg_2->num_next_nodes, 0);
    ASSERT_EQ(cfg_2->num_prev_nodes, 1);


    mCc_cfg_delete(cfg_head);

    mCc_tac_delete(tac);
}

/*
 * HEAD -> 1 -> 2
 */
TEST(cfg, SingleIfClause1)
{
    const char input[] = ""
            "void main(){\n"
            "    int a;\n"
            "    int b;\n"
            "    int c;\n"
            "    a = 1;\n"
            "    b = 5;\n"
            "   if (a == 1) {\n"
            "       c = a + b;\n"
            "   }\n"
            "}";

    auto result = mCc_parser_parse_string(input);

    result = *(mCc_ast_symbol_table(&result));
    result = *(mCc_ast_semantic_check(&result));

    struct mCc_tac_list *tac;
    tac = mCc_tac_generate(result.func_def);

    ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

    mCc_delete_result(&result);

    cfg_list *cfg_head = mCc_cfg_generate(tac);

    ASSERT_EQ(cfg_head->num_next_nodes, 1);

    cfg_list *cfg_1 = &cfg_head->next_nodes[0];

    ASSERT_EQ(cfg_1->num_next_nodes, 1);
    ASSERT_EQ(cfg_1->num_prev_nodes, 1);

    cfg_list *cfg_2 = &cfg_1->next_nodes[0];

    ASSERT_EQ(cfg_2->num_next_nodes, 0);
    ASSERT_EQ(cfg_2->num_prev_nodes, 1);

    mCc_cfg_delete(cfg_head);

    mCc_tac_delete(tac);
}

/*
 *      HEAD
 *        |
 *        1
 *       / \
 *       | 3
 *       |/
 *       2
 */
TEST(cfg, SingleIfClause2)
{
    const char input[] = ""
            "void main(){\n"
            "    int a;\n"
            "    int b;\n"
            "    int c;\n"
            "    a = 1;\n"
            "    b = 5;\n"
            "   if (a == 1) {\n"
            "       c = a + b;\n"
            "   }\n"
            "   b = 4;\n"
            "}";

    auto result = mCc_parser_parse_string(input);

    result = *(mCc_ast_symbol_table(&result));
    result = *(mCc_ast_semantic_check(&result));

    struct mCc_tac_list *tac;
    tac = mCc_tac_generate(result.func_def);

    ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

    mCc_delete_result(&result);

    cfg_list *cfg_head = mCc_cfg_generate(tac);

    ASSERT_EQ(cfg_head->num_next_nodes, 1);

    cfg_list *cfg_1 = &cfg_head->next_nodes[0];

    ASSERT_EQ(cfg_1->num_next_nodes, 2);
    ASSERT_EQ(cfg_1->num_prev_nodes, 1);

    cfg_list *cfg_2 = &cfg_1->next_nodes[0];

    ASSERT_EQ(cfg_2->num_next_nodes, 0);
    ASSERT_EQ(cfg_2->num_prev_nodes, 2);
    ASSERT_EQ(cfg_2->node_num, 3);

    cfg_list *cfg_3 = &cfg_1->next_nodes[1];

    ASSERT_EQ(cfg_3->num_next_nodes, 0);
    ASSERT_EQ(cfg_3->num_prev_nodes, 1);
    ASSERT_EQ(cfg_3->node_num, 4);

    cfg_list *cfg_3_branch = cfg_3->branch;

    ASSERT_EQ(cfg_2->node_num, cfg_3_branch->node_num);

    mCc_cfg_delete(cfg_head);

    mCc_tac_delete(tac);
}

/*
 *      HEAD
 *        |
 *        1
 *       / \
 *       2 3
 */
TEST(cfg, IFWithElseClause1)
{
    const char input[] = ""
            "void main(){\n"
            "    int a;\n"
            "    int b;\n"
            "    int c;\n"
            "    a = 1;\n"
            "    b = 5;\n"
            "   if (a == 1) {\n"
            "       c = a + b;\n"
            "   } else {\n"
            "       c = a - b;\n"
            "   }\n"
            "}";

    auto result = mCc_parser_parse_string(input);

    result = *(mCc_ast_symbol_table(&result));
    result = *(mCc_ast_semantic_check(&result));

    struct mCc_tac_list *tac;
    tac = mCc_tac_generate(result.func_def);

    ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

    mCc_delete_result(&result);

    cfg_list *cfg_head = mCc_cfg_generate(tac);

    ASSERT_EQ(cfg_head->num_next_nodes, 1);

    cfg_list *cfg_1 = &cfg_head->next_nodes[0];

    ASSERT_EQ(cfg_1->num_next_nodes, 2);
    ASSERT_EQ(cfg_1->num_prev_nodes, 1);

    cfg_list *cfg_2 = &cfg_1->next_nodes[0];

    ASSERT_EQ(cfg_2->num_next_nodes, 0);
    ASSERT_EQ(cfg_2->num_prev_nodes, 1);
    ASSERT_EQ(cfg_2->node_num, 3);

    cfg_list *cfg_3 = &cfg_1->next_nodes[1];

    ASSERT_EQ(cfg_3->num_next_nodes, 0);
    ASSERT_EQ(cfg_3->num_prev_nodes, 1);
    ASSERT_EQ(cfg_3->node_num, 4);

    mCc_cfg_delete(cfg_head);

    mCc_tac_delete(tac);
}

/*
 *      HEAD
 *        |
 *        1
 *       / \
 *       2 3
 *       \ /
 *        4
 */
TEST(cfg, IFWithElseClause2)
{
    const char input[] = ""
            "void main(){\n"
            "    int a;\n"
            "    int b;\n"
            "    int c;\n"
            "    a = 1;\n"
            "    b = 5;\n"
            "   if (a == 1) {\n"
            "       c = a + b;\n"
            "   } else {\n"
            "       c = a + b;\n"
            "   }\n"
            "   a = 5;\n"
            "}";

    auto result = mCc_parser_parse_string(input);

    result = *(mCc_ast_symbol_table(&result));
    result = *(mCc_ast_semantic_check(&result));

    struct mCc_tac_list *tac;
    tac = mCc_tac_generate(result.func_def);

    ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

    mCc_delete_result(&result);

    cfg_list *cfg_head = mCc_cfg_generate(tac);

    ASSERT_EQ(cfg_head->num_next_nodes, 1);

    cfg_list *cfg_1 = &cfg_head->next_nodes[0];

    ASSERT_EQ(cfg_1->num_next_nodes, 2);
    ASSERT_EQ(cfg_1->num_prev_nodes, 1);

    cfg_list *cfg_2 = &cfg_1->next_nodes[0];

    ASSERT_EQ(cfg_2->num_next_nodes, 1);
    ASSERT_EQ(cfg_2->num_prev_nodes, 1);
    ASSERT_EQ(cfg_2->node_num, 3);

    cfg_list *cfg_3 = &cfg_1->next_nodes[1];

    ASSERT_EQ(cfg_3->num_next_nodes, 0);
    ASSERT_EQ(cfg_3->num_prev_nodes, 1);
    ASSERT_EQ(cfg_3->node_num, 5);

    cfg_list *cfg_4 = &cfg_2->next_nodes[0];

    ASSERT_EQ(cfg_4->num_next_nodes, 0);
    ASSERT_EQ(cfg_4->num_prev_nodes, 2);
    ASSERT_EQ(cfg_4->node_num, 4);

    cfg_list *cfg_3_branch = cfg_3->branch;

    ASSERT_EQ(cfg_4->node_num, cfg_3_branch->node_num);

    mCc_cfg_delete(cfg_head);

    mCc_tac_delete(tac);
}

/*
 *      HEAD
 *        |
 *        1
 *       / \
 *       2 3
 *       |
 *       4
 */
TEST(cfg, IfWithElseIfClause1)
{
    const char input[] = ""
            "void main(){\n"
            "    int a;\n"
            "    int b;\n"
            "    int c;\n"
            "    a = 1;\n"
            "    b = 5;\n"
            "   if (a == 1) {\n"
            "       c = a + b;\n"
            "   } else if (a == 2) {\n"
            "       c = a + b;\n"
            "   }\n"
            "}";

    auto result = mCc_parser_parse_string(input);

    result = *(mCc_ast_symbol_table(&result));
    result = *(mCc_ast_semantic_check(&result));

    struct mCc_tac_list *tac;
    tac = mCc_tac_generate(result.func_def);

    ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

    mCc_delete_result(&result);

    cfg_list *cfg_head = mCc_cfg_generate(tac);

    ASSERT_EQ(cfg_head->num_next_nodes, 1);

    cfg_list *cfg_1 = &cfg_head->next_nodes[0];

    ASSERT_EQ(cfg_1->num_next_nodes, 2);
    ASSERT_EQ(cfg_1->num_prev_nodes, 1);

    cfg_list *cfg_2 = &cfg_1->next_nodes[0];

    ASSERT_EQ(cfg_2->num_next_nodes, 1);
    ASSERT_EQ(cfg_2->num_prev_nodes, 1);
    ASSERT_EQ(cfg_2->node_num, 3);

    cfg_list *cfg_3 = &cfg_1->next_nodes[1];

    ASSERT_EQ(cfg_3->num_next_nodes, 0);
    ASSERT_EQ(cfg_3->num_prev_nodes, 1);
    ASSERT_EQ(cfg_3->node_num, 6);

    cfg_list *cfg_4 = &cfg_2->next_nodes[0];

    ASSERT_EQ(cfg_4->num_next_nodes, 0);
    ASSERT_EQ(cfg_4->num_prev_nodes, 1);
    ASSERT_EQ(cfg_4->node_num, 5);

    mCc_cfg_delete(cfg_head);

    mCc_tac_delete(tac);
}


/*
 *      HEAD
 *        |
 *        1
 *       / \
 *       2 5
 *     / | |
 *     | 3 |
 *     \ | /
 *       4
 */
TEST(cfg, IfWithElseIfClause2)
{
    const char input[] = ""
            "void main(){\n"
            "    int a;\n"
            "    int b;\n"
            "    int c;\n"
            "    a = 1;\n"
            "    b = 5;\n"
            "   if (a == 1) {\n"
            "       c = a + b;\n"
            "   } else if (a == 2) {\n"
            "       c = a + b;\n"
            "   }\n"
            "   a = b;\n"
            "}";

    auto result = mCc_parser_parse_string(input);

    result = *(mCc_ast_symbol_table(&result));
    result = *(mCc_ast_semantic_check(&result));

    struct mCc_tac_list *tac;
    tac = mCc_tac_generate(result.func_def);

    ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

    mCc_delete_result(&result);

    cfg_list *cfg_head = mCc_cfg_generate(tac);

    ASSERT_EQ(cfg_head->num_next_nodes, 1);

    cfg_list *cfg_1 = &cfg_head->next_nodes[0];

    ASSERT_EQ(cfg_1->num_next_nodes, 2);
    ASSERT_EQ(cfg_1->num_prev_nodes, 1);

    cfg_list *cfg_2 = &cfg_1->next_nodes[0];

    ASSERT_EQ(cfg_2->num_next_nodes, 2);
    ASSERT_EQ(cfg_2->num_prev_nodes, 1);
    ASSERT_EQ(cfg_2->node_num, 3);

    cfg_list *cfg_3 = &cfg_1->next_nodes[1];

    ASSERT_EQ(cfg_3->num_next_nodes, 0);
    ASSERT_EQ(cfg_3->num_prev_nodes, 1);
    ASSERT_EQ(cfg_3->node_num, 7);

    cfg_list *cfg_3_branch = cfg_3->branch;

    cfg_list *cfg_4 = &cfg_2->next_nodes[0];

    ASSERT_EQ(cfg_3_branch->node_num, cfg_4->node_num);
    ASSERT_EQ(cfg_4->num_next_nodes, 0);
    ASSERT_EQ(cfg_4->num_prev_nodes, 3);
    ASSERT_EQ(cfg_4->node_num, 5);

    cfg_list *cfg_5 = &cfg_2->next_nodes[1];

    cfg_list *cfg_5_branch = cfg_5->branch;

    ASSERT_EQ(cfg_5->num_next_nodes, 0);
    ASSERT_EQ(cfg_5->num_prev_nodes, 1);
    ASSERT_EQ(cfg_5->node_num, 6);
    ASSERT_EQ(cfg_5_branch->node_num, cfg_4->node_num);

    mCc_cfg_delete(cfg_head);

    mCc_tac_delete(tac);
}

TEST(cfg, NestedIfClause)
{
    const char input[] = ""
            "void main(){\n"
            "   int a;\n"
            "   int b;\n"
            "   int c;\n"
            "   a = 1;\n"
            "   b = 5;\n"
            "   if (a == 1) {\n"
            "       c = a + b;\n"
            "       if (a == 2) {\n"
            "           a = 5;\n"
            "       }\n"
            "   } else {\n"
            "       c = a + b;\n"
            "   }\n"
            "   a = b;\n"
            "}";

    auto result = mCc_parser_parse_string(input);

    result = *(mCc_ast_symbol_table(&result));
    result = *(mCc_ast_semantic_check(&result));

    struct mCc_tac_list *tac;
    tac = mCc_tac_generate(result.func_def);

    ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);


    cfg_list *cfg_head = mCc_cfg_generate(tac);

    ASSERT_EQ(cfg_head->num_next_nodes, 1);

    cfg_list *cfg_1 = &cfg_head->next_nodes[0];

    ASSERT_EQ(cfg_1->num_next_nodes, 2);
    ASSERT_EQ(cfg_1->num_prev_nodes, 1);

    cfg_list *cfg_2 = &cfg_1->next_nodes[0];

    ASSERT_EQ(cfg_2->num_next_nodes, 1);
    ASSERT_EQ(cfg_2->num_prev_nodes, 2);
    ASSERT_EQ(cfg_2->node_num, 3);

    cfg_list *cfg_3 = &cfg_1->next_nodes[1];

    ASSERT_EQ(cfg_3->num_next_nodes, 1);
    ASSERT_EQ(cfg_3->num_prev_nodes, 1);
    ASSERT_EQ(cfg_3->node_num, 5);

    cfg_list *cfg_4 = &cfg_2->next_nodes[0];

    ASSERT_EQ(cfg_4->num_next_nodes, 0);
    ASSERT_EQ(cfg_4->num_prev_nodes, 2);
    ASSERT_EQ(cfg_4->node_num, 4);

    cfg_list *cfg_5 = &cfg_1->next_nodes[0];

    ASSERT_EQ(cfg_5->num_next_nodes, 1);
    ASSERT_EQ(cfg_5->num_prev_nodes, 2);
    ASSERT_EQ(cfg_5->node_num, 3);

    mCc_cfg_delete(cfg_head);

    mCc_tac_delete(tac);
    mCc_delete_result(&result);
}

/*
 *      HEAD
 *        |
 *        1
 *        |
 *        2
 *       | |
 *        3

 */
TEST(cfg, WhileLoop1)
{
    const char input[] = ""
            "void main(){\n"
            "    int a;\n"
            "    int b;\n"
            "    int c;\n"
            "    a = 1;\n"
            "    b = 5;\n"
            "    while (a == 1) {\n"
            "       c = a + b;\n"
            "    }\n"
            "}";

     auto result = mCc_parser_parse_string(input);

    result = *(mCc_ast_symbol_table(&result));
    result = *(mCc_ast_semantic_check(&result));

    struct mCc_tac_list *tac;
    tac = mCc_tac_generate(result.func_def);

    ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

    mCc_delete_result(&result);

    cfg_list *cfg_head = mCc_cfg_generate(tac);

    ASSERT_EQ(cfg_head->num_next_nodes, 1);

    cfg_list *cfg_1 = &cfg_head->next_nodes[0];

    ASSERT_EQ(cfg_1->num_next_nodes, 1);
    ASSERT_EQ(cfg_1->num_prev_nodes, 1);

    cfg_list *cfg_2 = &cfg_1->next_nodes[0];

    ASSERT_EQ(cfg_2->num_next_nodes, 1);
    ASSERT_EQ(cfg_2->num_prev_nodes, 2);

    cfg_list *cfg_3 = &cfg_2->next_nodes[0];

    ASSERT_EQ(cfg_3->num_next_nodes, 0);
    ASSERT_EQ(cfg_3->num_prev_nodes, 1);

    cfg_list *cfg_3_loop_branch = &cfg_3->branch[0];

    ASSERT_EQ(cfg_3_loop_branch->node_num, cfg_2->node_num);

    mCc_cfg_delete(cfg_head);

    mCc_tac_delete(tac);
}

TEST(cfg, WhileLoop2)
{
    const char input[] = ""
            "void main(){\n"
            "    int a;\n"
            "    int b;\n"
            "    int c;\n"
            "    a = 1;\n"
            "    b = 5;\n"
            "    while (a == 1) {\n"
            "       c = a + b;\n"
            "    }\n"
            "   a = 4;\n"
            "}";

    auto result = mCc_parser_parse_string(input);

    result = *(mCc_ast_symbol_table(&result));
    result = *(mCc_ast_semantic_check(&result));

    struct mCc_tac_list *tac;
    tac = mCc_tac_generate(result.func_def);

    ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

    mCc_delete_result(&result);

    cfg_list *cfg_head = mCc_cfg_generate(tac);

    ASSERT_EQ(cfg_head->num_next_nodes, 1);

    cfg_list *cfg_1 = &cfg_head->next_nodes[0];

    ASSERT_EQ(cfg_1->num_next_nodes, 1);
    ASSERT_EQ(cfg_1->num_prev_nodes, 1);

    cfg_list *cfg_2 = &cfg_1->next_nodes[0];

    ASSERT_EQ(cfg_2->num_next_nodes, 2);
    ASSERT_EQ(cfg_2->num_prev_nodes, 2);

    cfg_list *cfg_4 = &cfg_2->next_nodes[1];

    ASSERT_EQ(cfg_4->num_next_nodes, 0);
    ASSERT_EQ(cfg_4->num_prev_nodes, 1);

    cfg_list *cfg_3 = &cfg_2->next_nodes[0];

    ASSERT_EQ(cfg_3->num_next_nodes, 0);
    ASSERT_EQ(cfg_3->num_prev_nodes, 1);

    mCc_cfg_delete(cfg_head);

    mCc_tac_delete(tac);
}

TEST(cfg, NestedWhileLoop)
{
    const char input[] = ""
            "void main(){\n"
            "    int a;\n"
            "    int b;\n"
            "    int c;\n"
            "    a = 1;\n"
            "    b = 5;\n"
            "    while (a == 1) {\n"
            "       c = a + b;\n"
            "       while ( a < 10) {\n"
            "            a = a + 1;\n"
            "       }\n"
            "       c = a + b;\n"
            "    }\n"
            "       c = a + b;\n"
            "}";

    auto result = mCc_parser_parse_string(input);

    result = *(mCc_ast_symbol_table(&result));
    result = *(mCc_ast_semantic_check(&result));

    struct mCc_tac_list *tac;
    tac = mCc_tac_generate(result.func_def);

    ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

    mCc_delete_result(&result);

    cfg_list *cfg_head = mCc_cfg_generate(tac);

    ASSERT_EQ(cfg_head->num_next_nodes, 1);

    cfg_list *cfg_1 = &cfg_head->next_nodes[0];

    ASSERT_EQ(cfg_1->num_next_nodes, 1);
    ASSERT_EQ(cfg_1->num_prev_nodes, 1);

    cfg_list *cfg_2 = &cfg_1->next_nodes[0];

    ASSERT_EQ(cfg_2->num_next_nodes, 2);
    ASSERT_EQ(cfg_2->num_prev_nodes, 2);

    cfg_list *cfg_3 = &cfg_2->next_nodes[0];

    ASSERT_EQ(cfg_3->num_next_nodes, 0);
    ASSERT_EQ(cfg_3->num_prev_nodes, 1);

    cfg_list *cfg_4 = &cfg_2->next_nodes[0];

    ASSERT_EQ(cfg_4->num_next_nodes, 0);
    ASSERT_EQ(cfg_4->num_prev_nodes, 1);

    cfg_list *cfg_5 = &cfg_2->next_nodes[1];

    ASSERT_EQ(cfg_5->num_next_nodes, 1);
    ASSERT_EQ(cfg_5->num_prev_nodes, 1);

    cfg_list *cfg_6 = &cfg_5->next_nodes[0];

    ASSERT_EQ(cfg_6->num_next_nodes, 2);
    ASSERT_EQ(cfg_6->num_prev_nodes, 2);

    cfg_list *cfg_7 = &cfg_6->next_nodes[0];

    ASSERT_EQ(cfg_7->num_next_nodes, 0);
    ASSERT_EQ(cfg_7->num_prev_nodes, 1);

    cfg_list *cfg_8 = &cfg_6->next_nodes[1];

    ASSERT_EQ(cfg_8->num_next_nodes, 0);
    ASSERT_EQ(cfg_8->num_prev_nodes, 1);

    mCc_cfg_delete(cfg_head);

    mCc_tac_delete(tac);
}

TEST(cfg, WhileIfCombination)
{
    const char input[] = ""
            "void main(){\n"
            "   int a;\n"
            "   int b;\n"
            "   int c;\n"
            "   a = 1;\n"
            "   b = 5;\n"
            "   while (a == 1) {\n"
            "       c = a + b;\n"
            "       if ( a < 10) {\n"
            "           a = a + 1;\n"
            "       }\n"
            "   c = a + b;\n"
            "   }\n"
            "}";

    auto result = mCc_parser_parse_string(input);

    result = *(mCc_ast_symbol_table(&result));
    result = *(mCc_ast_semantic_check(&result));

    struct mCc_tac_list *tac;
    tac = mCc_tac_generate(result.func_def);

    ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

    mCc_delete_result(&result);

    cfg_list *cfg_head = mCc_cfg_generate(tac);

    ASSERT_EQ(cfg_head->num_next_nodes, 1);

    cfg_list *cfg_1 = &cfg_head->next_nodes[0];

    ASSERT_EQ(cfg_1->num_next_nodes, 1);
    ASSERT_EQ(cfg_1->num_prev_nodes, 1);

    cfg_list *cfg_2 = &cfg_1->next_nodes[0];

    ASSERT_EQ(cfg_2->num_next_nodes, 1);
    ASSERT_EQ(cfg_2->num_prev_nodes, 2);

    cfg_list *cfg_3 = &cfg_2->next_nodes[0];

    ASSERT_EQ(cfg_3->num_next_nodes, 2);
    ASSERT_EQ(cfg_3->num_prev_nodes, 1);

    cfg_list *cfg_4 = &cfg_3->next_nodes[0];

    ASSERT_EQ(cfg_4->num_next_nodes, 0);
    ASSERT_EQ(cfg_4->num_prev_nodes, 2);

    cfg_list *cfg_5 = &cfg_3->next_nodes[1];

    ASSERT_EQ(cfg_5->num_next_nodes, 0);
    ASSERT_EQ(cfg_5->num_prev_nodes, 1);

    mCc_cfg_delete(cfg_head);

    mCc_tac_delete(tac);
}


