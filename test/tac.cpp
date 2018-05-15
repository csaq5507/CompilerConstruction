#include <gtest/gtest.h>
#include <stdlib.h>
#include <stdio.h>
#include "mCc/ast.h"
#include "mCc/parser.h"
#include "mCc/ast_symbol_table.h"
#include "mCc/ast_semantic_checks.h"
#include "mCc/tac.h"

TEST(tac_generation, tac_generation)
{

    const char input[] =
            "void main() { print(\"Hello World\"); }";

    auto result = mCc_parser_parse_string(input);

    result = *(mCc_ast_symbol_table(&result));
    result = *(mCc_ast_semantic_check(&result));

    struct mCc_tac_list *tac;
    tac = mCc_tac_generate(result.func_def);
    struct mCc_ast_function_def_array *func_def_arr = result.func_def;

    ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

    mCc_ast_delete_function_def_array(func_def_arr);
    mCc_delete_result(&result);

   ASSERT_EQ(MCC_TAC_ELEMENT_TYPE_PROCEDURE_CALL,tac->next->next->next->type);
   ASSERT_EQ(MCC_TAC_ELEMENT_TYPE_COPY,tac->next->next->type);
   ASSERT_EQ(1,tac->next->next->next->num_function_param);
   ASSERT_STREQ("t0",tac->next->next->f_identifier);
   ASSERT_STREQ("Hello World",tac->next->next->s_literal);

}
