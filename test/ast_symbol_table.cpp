#include <gtest/gtest.h>
#include <stdlib.h>
#include <stdio.h>
#include "mCc/ast.h"
#include "mCc/parser.h"
#include "mCc/ast_symbol_table.h"
#include "mCc/ast_semantic_checks.h"
#include "mCc/error.h"

TEST(symbol_table, correct_symbol_table)
{
	const char input[] =
		"void func() {\n"
		"int a;\n"
		"a = 10;\n"
		"if (a == 10){\n"
		"int b;"
		"   return a;\n"
		"}\n"
		"}\n";
	auto result = mCc_parser_parse_string(input);


	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	struct mCc_ast_function_def_array *func_def_arr = result.func_def;
	auto func_def = func_def_arr->function_def;

	ASSERT_STREQ("a", func_def[func_def_arr->counter - 1]
				  .c_stmt[0]
				  .statements[0]
				  .declaration->identifier->name);

	auto result2 = mCc_ast_symbol_table(&result);
	func_def_arr = result2->func_def;
	ASSERT_STREQ("a1", func_def[func_def_arr->counter - 1]
				   .c_stmt[0]
				   .statements[0]
				   .declaration->identifier->renamed);


	mCc_delete_result(&result);
}


TEST(symbol_table, main_not_void)
{

	const char input[] = "int main() {}";

	auto result = mCc_parser_parse_string(input);

	result = *(mCc_ast_symbol_table(&result));

	ASSERT_EQ(MCC_PARSER_STATUS_ERROR, result.status);




	char error_msg[1024] = {0};
	snprintf(error_msg, sizeof(error_msg), ERROR_MAIN_NOT_VOID, "int");

	ASSERT_STREQ(error_msg, result.errors->errors[0].error_msg);

	mCc_delete_result(&result);
}


TEST(symbol_table, no_main)
{

	const char input[] = "void func1() {}";

	auto result = mCc_parser_parse_string(input);

	result = *(mCc_ast_symbol_table(&result));

	ASSERT_EQ(MCC_PARSER_STATUS_ERROR, result.status);



	ASSERT_STREQ(ERROR_NO_MAIN, result.errors->errors[0].error_msg);

	mCc_delete_result(&result);
}

TEST(symbol_table, dublicate_function)
{
	const char input[] = "void main() {} void func1(){} void func1(){}";

	auto result = mCc_parser_parse_string(input);

	result = *(mCc_ast_symbol_table(&result));

	ASSERT_EQ(MCC_PARSER_STATUS_ERROR, result.status);



	char error_msg[1024] = {0};
	snprintf(error_msg, sizeof(error_msg), ERROR_DUBLICATE_FUNCTION,
		 "func1");

	ASSERT_STREQ(error_msg, result.errors->errors[0].error_msg);

	mCc_delete_result(&result);
}
TEST(symbol_table, dublicate_variable)
{
	const char input[] =
		"void main() {\n"
		"int a;\n"
		"float a;"
		"a = 10;\n"
		"if (a == 10){\n"
		"int b;"
		"   return a;\n"
		"}\n"
		"}\n";

	auto result = mCc_parser_parse_string(input);

	result = *(mCc_ast_symbol_table(&result));

	ASSERT_EQ(MCC_PARSER_STATUS_ERROR, result.status);



	char error_msg[1024] = {0};
	snprintf(error_msg, sizeof(error_msg), ERROR_DUBLICATE_VARIABLE, "a");

	ASSERT_STREQ(error_msg, result.errors->errors[0].error_msg);

	mCc_delete_result(&result);
}


TEST(symbol_table, no_return)
{

	const char input[] = "void main() {int a; } int func1(){}";

	auto result = mCc_parser_parse_string(input);

	result = *(mCc_ast_symbol_table(&result));

	ASSERT_EQ(MCC_PARSER_STATUS_ERROR, result.status);



	char error_msg[1024] = {0};
	snprintf(error_msg, sizeof(error_msg), ERROR_NO_RETURN, "func1");

	ASSERT_STREQ(error_msg, result.errors->errors[0].error_msg);

	mCc_delete_result(&result);
}

TEST(symbol_table, missing_variable_def)
{

	const char input[] = "void main() {int a; b=10; } ";

	auto result = mCc_parser_parse_string(input);

	result = *(mCc_ast_symbol_table(&result));

	ASSERT_EQ(MCC_PARSER_STATUS_ERROR, result.status);



	char error_msg[1024] = {0};
	snprintf(error_msg, sizeof(error_msg), ERROR_MISSING_VARIABLE_DEF, "b");

	ASSERT_STREQ(error_msg, result.errors->errors[0].error_msg);

	mCc_delete_result(&result);
}


TEST(symbol_table, missing_function_def)
{

	const char input[] = "void main() {int a; func2(); }";

	auto result = mCc_parser_parse_string(input);

	result = *(mCc_ast_symbol_table(&result));

	ASSERT_EQ(MCC_PARSER_STATUS_ERROR, result.status);



	char error_msg[1024] = {0};
	snprintf(error_msg, sizeof(error_msg), ERROR_MISSING_FUNCTION_DEF,
		 "func2");

	ASSERT_STREQ(error_msg, result.errors->errors[0].error_msg);

	mCc_delete_result(&result);
}


TEST(symbol_table, error_num_arguments)
{

	const char input[] = " void func2(){} void main() {int a; func2(a); }";

	auto result = mCc_parser_parse_string(input);

	result = *(mCc_ast_symbol_table(&result));

	ASSERT_EQ(MCC_PARSER_STATUS_ERROR, result.status);



	char error_msg[1024] = {0};
	snprintf(error_msg, sizeof(error_msg), ERROR_NUM_ARGUMENTS, "func2", 0,
		 1);

	ASSERT_STREQ(error_msg, result.errors->errors[0].error_msg);

	mCc_delete_result(&result);
}
TEST(symbol_table, error_num_arguments2)
{

	const char input[] =
		" void func2(int a, int b){} void main() {int a; func2(a); }";

	auto result = mCc_parser_parse_string(input);

	result = *(mCc_ast_symbol_table(&result));

	ASSERT_EQ(MCC_PARSER_STATUS_ERROR, result.status);



	char error_msg[1024] = {0};
	snprintf(error_msg, sizeof(error_msg), ERROR_NUM_ARGUMENTS, "func2", 2,
		 1);

	ASSERT_STREQ(error_msg, result.errors->errors[0].error_msg);

	mCc_delete_result(&result);
}
