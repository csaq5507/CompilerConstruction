#include <gtest/gtest.h>
#include <stdlib.h>
#include <stdio.h>
#include "mCc/ast.h"
#include "mCc/parser.h"
#include "mCc/ast_symbol_table.h"
#include "mCc/ast_semantic_checks.h"
#include "mCc/tac.h"


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

TEST(tac_generation, empty_if1) {
    const char input[] = "void main() { int a; a=0; if(a<5){ } a = 10; }";

    auto result = mCc_parser_parse_string(input);

    result = *(mCc_ast_symbol_table(&result));
    result = *(mCc_ast_semantic_check(&result));

    struct mCc_tac_list *tac;
    tac = mCc_tac_generate(result.func_def);

    ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

    mCc_delete_result(&result);

    struct mCc_tac_list *tac1 = get_at(tac,7);
    struct mCc_tac_list *tac2 = get_at(tac,8);
    struct mCc_tac_list *tac3 = get_at(tac,9);

    ASSERT_EQ(MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP,tac1->type);
    ASSERT_STREQ("L1",tac1->jump->identifier1);

    ASSERT_EQ(MCC_TAC_ELEMENT_TYPE_LABEL,tac2->type);
    ASSERT_STREQ("L0",tac2->identifier1);

    ASSERT_EQ(MCC_TAC_ELEMENT_TYPE_LABEL,tac3->type);
    ASSERT_STREQ("L1",tac3->identifier1);

    mCc_tac_delete(tac);
}

TEST(tac_generation, empty_if2) {
    const char input[] = "void main() { int a; a=0; if(a<5){ } else {a = 10;} a = 10; }";

    auto result = mCc_parser_parse_string(input);

    result = *(mCc_ast_symbol_table(&result));
    result = *(mCc_ast_semantic_check(&result));

    struct mCc_tac_list *tac;
    tac = mCc_tac_generate(result.func_def);

    ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

    mCc_delete_result(&result);

    struct mCc_tac_list *tac0 = get_at(tac,6);
    struct mCc_tac_list *tac1 = get_at(tac,7);
    struct mCc_tac_list *tac2 = get_at(tac,8);

    struct mCc_tac_list *tac3 = get_at(tac,11);
    struct mCc_tac_list *tac4 = get_at(tac,12);
    struct mCc_tac_list *tac5 = get_at(tac,13);


    ASSERT_EQ(MCC_TAC_ELEMENT_TYPE_BINARY,tac0->type);
    ASSERT_EQ(MCC_TAC_OPERATION_TYPE_GE,tac0->binary_op_type);

    ASSERT_EQ(MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP,tac1->type);
    ASSERT_STREQ("L1",tac1->jump->identifier1);

    ASSERT_EQ(MCC_TAC_ELEMENT_TYPE_LABEL,tac2->type);
    ASSERT_STREQ("L0",tac2->identifier1);


    ASSERT_EQ(MCC_TAC_ELEMENT_TYPE_UNCONDITIONAL_JUMP,tac3->type);
    ASSERT_STREQ("L2",tac3->jump->identifier1);

    ASSERT_EQ(MCC_TAC_ELEMENT_TYPE_LABEL,tac4->type);
    ASSERT_STREQ("L1",tac4->identifier1);

    ASSERT_EQ(MCC_TAC_ELEMENT_TYPE_LABEL,tac5->type);
    ASSERT_STREQ("L2",tac5->identifier1);

    mCc_tac_delete(tac);
}

TEST(tac_generation, empty_if3) {
    const char input[] = "void main() { int a; a=0; if(a<5){ } else {} a = 10; }";

    auto result = mCc_parser_parse_string(input);

    result = *(mCc_ast_symbol_table(&result));
    result = *(mCc_ast_semantic_check(&result));

    struct mCc_tac_list *tac;
    tac = mCc_tac_generate(result.func_def);

    ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

    mCc_delete_result(&result);

    struct mCc_tac_list *tac_temp = tac;
    while (tac_temp != NULL) {
        ASSERT_NE(tac_temp->type, MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP);
        ASSERT_NE(tac_temp->type, MCC_TAC_ELEMENT_TYPE_UNCONDITIONAL_JUMP);
        tac_temp = tac_temp->next;
    }


    mCc_tac_delete(tac);
}

TEST(tac_generation, empty_while) {
    const char input[] = "void main() { int a; a=0; while(a<5){ } a = 10; }";

    auto result = mCc_parser_parse_string(input);

    result = *(mCc_ast_symbol_table(&result));
    result = *(mCc_ast_semantic_check(&result));

    struct mCc_tac_list *tac;
    tac = mCc_tac_generate(result.func_def);

    ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

    mCc_delete_result(&result);

    struct mCc_tac_list *tac_temp = tac;
    while (tac_temp != NULL) {
        ASSERT_NE(tac_temp->type, MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP);
        ASSERT_NE(tac_temp->type, MCC_TAC_ELEMENT_TYPE_UNCONDITIONAL_JUMP);
        tac_temp = tac_temp->next;
    }


    mCc_tac_delete(tac);
}