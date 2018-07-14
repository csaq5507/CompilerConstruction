#include <gtest/gtest.h>
#include <stdlib.h>
#include <stdio.h>
#include "mCc/ast.h"
#include "mCc/parser.h"
#include "mCc/ast_symbol_table.h"
#include "mCc/ast_semantic_checks.h"
#include "mCc/tac.h"


/*
 * Here we could test so many things that we decided to just look at the tac
 * output of the example programs.
 *
 */


TEST(tac_generation, tac_generation_func_call)
{

	const char input[] = "int func1(int b){return b+1;} void main() { int a; a=func1(2); print(\"Hello World\");}";

	auto result = mCc_parser_parse_string(input);

	result = *(mCc_ast_symbol_table(&result));
	result = *(mCc_ast_semantic_check(&result));

	struct mCc_tac_list *tac;
	tac = mCc_tac_generate(result.func_def);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	mCc_delete_result(&result);


    struct mCc_tac_list *tac2 = get_at(tac,12);
    ASSERT_NE(tac2, nullptr);
    ASSERT_EQ(MCC_TAC_ELEMENT_TYPE_PROCEDURE_CALL,tac2->type);
    ASSERT_STREQ("func10",tac2->identifier1);
    ASSERT_EQ(2,tac2->num_function_param);
    tac2=get_at(tac,16);
    ASSERT_EQ(MCC_TAC_ELEMENT_TYPE_PROCEDURE_CALL,tac2->type);
    ASSERT_STREQ("print",tac2->identifier1);
    ASSERT_EQ(1,tac2->num_function_param);

    mCc_tac_delete(tac);

}


TEST(tac_generation, tac_generation_conditional_jump)
{

    const char input[] = "int func1(int b){ if(b<5) return 0; else return 1;} void main() { int result; result=func1(5); }";

    auto result = mCc_parser_parse_string(input);

    result = *(mCc_ast_symbol_table(&result));
    result = *(mCc_ast_semantic_check(&result));

    struct mCc_tac_list *tac;
    tac = mCc_tac_generate(result.func_def);

    ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

    mCc_delete_result(&result);

    struct mCc_tac_list *tac2 = get_at(tac,5);
    ASSERT_NE(tac2, nullptr);
    ASSERT_EQ(MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP,tac2->type);
    ASSERT_STREQ("L1",tac2->jump->identifier1);
    tac2=get_at(tac,5);
    ASSERT_EQ(MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP,tac2->type);
    mCc_tac_delete(tac);

}



TEST(tac_generation, tac_generation_unconditional_jump)
{

    const char input[] = "void main() { int a; a=0; while(a<5){ a=a+1; }  }";

    auto result = mCc_parser_parse_string(input);

    result = *(mCc_ast_symbol_table(&result));
    result = *(mCc_ast_semantic_check(&result));

    struct mCc_tac_list *tac;
    tac = mCc_tac_generate(result.func_def);

    ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

    mCc_delete_result(&result);

    struct mCc_tac_list *tac2 = get_at(tac,14);
    ASSERT_NE(tac2, nullptr);
    ASSERT_EQ(MCC_TAC_ELEMENT_TYPE_UNCONDITIONAL_JUMP,tac2->type);
    ASSERT_STREQ("L1",tac2->jump->identifier1);
    tac2=get_at(tac,4);
    ASSERT_STREQ("L1",tac2->identifier1);
    ASSERT_EQ(MCC_TAC_ELEMENT_TYPE_LABEL,tac2->type);
    mCc_tac_delete(tac);

}
