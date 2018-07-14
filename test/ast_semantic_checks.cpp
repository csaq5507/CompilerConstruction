#include <gtest/gtest.h>
#include <stdlib.h>
#include <stdio.h>
#include "mCc/ast.h"
#include "mCc/parser.h"
#include "mCc/ast_symbol_table.h"
#include "mCc/ast_semantic_checks.h"
#include "mCc/error.h"

TEST(semantic_check, binary_ex_type_missmatch)
{

	const char input[] = "void main() {int a; a = 1 + 2.0;}";

	auto result = mCc_parser_parse_string(input);

	result = *(mCc_ast_symbol_table(&result));
	result = *(mCc_ast_semantic_check(&result));


	ASSERT_EQ(MCC_PARSER_STATUS_ERROR, result.status);

	char error_msg[1024] = {0};
	snprintf(error_msg, sizeof(error_msg), ERROR_BINARY_EX_TYPE_MISSMATCH,
		 "int", "+", "float");

	ASSERT_STREQ(error_msg, result.errors->errors[0].error_msg);

	mCc_delete_result(&result);
}

TEST(semantic_check, wrong_parameter_type)
{

	const char input[] = " void func1(int a){} void main() {func1(2.0);}";

	auto result = mCc_parser_parse_string(input);

	result = *(mCc_ast_symbol_table(&result));
	result = *(mCc_ast_semantic_check(&result));


	ASSERT_EQ(MCC_PARSER_STATUS_ERROR, result.status);

	char error_msg[1024] = {0};
	snprintf(error_msg, sizeof(error_msg), ERROR_WRONG_PARAMETER_TYPE,
		 "func1", 1, "int", "float");

	ASSERT_STREQ(error_msg, result.errors->errors[0].error_msg);

	mCc_delete_result(&result);
}

TEST(semantic_check, wrong_return_type)
{

	const char input[] = "int func1(){int a; return;} void main() {}";

	auto result = mCc_parser_parse_string(input);

	result = *(mCc_ast_symbol_table(&result));
	result = *(mCc_ast_semantic_check(&result));


	ASSERT_EQ(MCC_PARSER_STATUS_ERROR, result.status);

	char error_msg[1024] = {0};
	snprintf(error_msg, sizeof(error_msg), ERROR_WRONG_RETURN_TYPE, "int",
		 "void");


	ASSERT_STREQ(error_msg, result.errors->errors[0].error_msg);

	mCc_delete_result(&result);
}

TEST(semantic_check, wrong_return_type2)
{

	const char input[] = "void func1(){int a; return a;} void main() {}";

	auto result = mCc_parser_parse_string(input);

	result = *(mCc_ast_symbol_table(&result));
	result = *(mCc_ast_semantic_check(&result));


	ASSERT_EQ(MCC_PARSER_STATUS_ERROR, result.status);

	char error_msg[1024] = {0};
	snprintf(error_msg, sizeof(error_msg), ERROR_WRONG_RETURN_TYPE, "void",
		 "int");


	ASSERT_STREQ(error_msg, result.errors->errors[0].error_msg);

	mCc_delete_result(&result);
}

TEST(semantic_check, wrong_assignment_type)
{

	const char input[] = "void main() { int a; a = 2.0;}";

	auto result = mCc_parser_parse_string(input);

	result = *(mCc_ast_symbol_table(&result));
	result = *(mCc_ast_semantic_check(&result));


	ASSERT_EQ(MCC_PARSER_STATUS_ERROR, result.status);

	char error_msg[1024] = {0};
	snprintf(error_msg, sizeof(error_msg), ERROR_WRONG_ASSIGNMENT_TYPE,
		 "int", "float");


	ASSERT_STREQ(error_msg, result.errors->errors[0].error_msg);

	mCc_delete_result(&result);
}

TEST(semantic_check, condition_not_boolean_if)
{

	const char input[] = "void main() { int a; if(a+1) {} }";

	auto result = mCc_parser_parse_string(input);

	result = *(mCc_ast_symbol_table(&result));
	result = *(mCc_ast_semantic_check(&result));


	ASSERT_EQ(MCC_PARSER_STATUS_ERROR, result.status);

	char error_msg[1024] = {0};
	snprintf(error_msg, sizeof(error_msg), ERROR_CONDITION_NOT_BOOLEAN,
		 "int");


	ASSERT_STREQ(error_msg, result.errors->errors[0].error_msg);

	mCc_delete_result(&result);
}

TEST(semantic_check, condition_not_boolean_while)
{

	const char input[] = "void main() { int a; while((a + 1) * 5) {} }";

	auto result = mCc_parser_parse_string(input);

	result = *(mCc_ast_symbol_table(&result));
	result = *(mCc_ast_semantic_check(&result));


	ASSERT_EQ(MCC_PARSER_STATUS_ERROR, result.status);

	char error_msg[1024] = {0};
	snprintf(error_msg, sizeof(error_msg), ERROR_CONDITION_NOT_BOOLEAN,
		 "int");


	ASSERT_STREQ(error_msg, result.errors->errors[0].error_msg);

	mCc_delete_result(&result);
}