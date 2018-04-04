#include <gtest/gtest.h>
#include <stdlib.h>
#include <stdio.h>
#include "mCc/ast.h"
#include "mCc/parser.h"

TEST(Parser, BinaryOp_1)
{
	const char input[] = "/* Comment */ int func1() {192 + 3.14;} int func1() {192 + 3.14;} void func1() {192 + 3.14;}";
	auto result = mCc_parser_parse_string(input);
	
	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

    struct mCc_ast_function_def_array *func_def_arr = result.func_def;
    auto func_def = func_def_arr->function_def;

    for(int i=0;i<func_def_arr->counter-1;i++)
    {
        ASSERT_EQ(MCC_AST_FUNCTION_DEF_TYPE_TYPE, func_def[i].type);
    }
    ASSERT_EQ(MCC_AST_FUNCTION_DEF_TYPE_VOID, func_def[func_def_arr->counter-1].type);

	mCc_ast_delete_function_def(func_def_arr);
}

TEST(Parser, Wrong_Input) {
    const char input[] = "/* Comment */ int func1() {int a; int b int c; int d;}";
    auto result = mCc_parser_parse_string(input);

    ASSERT_EQ(result.errors->counter, 1);

    for(int i=0;i<result.errors->counter;i++)
    {
        ASSERT_NE("", result.errors->errors[i].error_msg);
    }
}