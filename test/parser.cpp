#include <gtest/gtest.h>
#include <stdlib.h>
#include <stdio.h>
#include "mCc/ast.h"
#include "mCc/parser.h"
#include "mCc/ast_symbol_table.h"

TEST(Assignemt_1, Task1_2)
{
	const char input[] =
		"/* Comment */ int func1() {192 + 3.14;} "
		"int func2() {192 + 3.14;}"
		"void func3() {192 + 3.14;"
		"int a;"
		"a = 10;"
		"if (a == 10)"
		"   return 10;"
		"}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	struct mCc_ast_function_def_array *func_def_arr = result.func_def;
	auto func_def = func_def_arr->function_def;

	for (int i = 0; i < func_def_arr->counter - 1; i++) {
		ASSERT_EQ(MCC_AST_FUNCTION_DEF_TYPE_TYPE, func_def[i].type);
	}
	ASSERT_EQ(MCC_AST_FUNCTION_DEF_TYPE_VOID,
		  func_def[func_def_arr->counter - 1].type);

	ASSERT_EQ(4, func_def[func_def_arr->counter - 1].c_stmt->counter);

	ASSERT_EQ(
		MCC_AST_DECL_STMT,
		func_def[func_def_arr->counter - 1].c_stmt->statements[1].type);

	mCc_ast_delete_function_def_array(result.func_def);
	mCc_delete_result(&result);
}

TEST(Assignemt_1, Task3)
{
	const char input[] =
		"void func3() {\n192 + 3.14;\n"
		"int a\n"
		"a = 10;\n"
		"if (a == 10)\n"
		"   return 10;\n"
		"}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_ERROR, result.status);

	ASSERT_STREQ("syntax error, unexpected identifier, expecting ;",
		     result.errors->errors[0].error_msg);
	ASSERT_EQ(4, result.errors->errors[0].error_line);

	mCc_delete_result(&result);
	// TODO delete errors not working properly
}

TEST(Assignemt_2, Task1)
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
	ASSERT_STREQ("a0", func_def[func_def_arr->counter - 1]
				   .c_stmt[0]
				   .statements[0]
				   .declaration->identifier->renamed);


	mCc_ast_delete_function_def_array(result.func_def);
	mCc_delete_result(&result);
}

TEST(Assignemt_2, Task2)
{
	const char input[] =
		"void func() {\n"
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

	struct mCc_ast_function_def_array *func_def_arr = result.func_def;

	ASSERT_STREQ("Allready defined: a", result.errors->errors[0].error_msg);
}

TEST(Assignemt_2, Task3)
{
}

TEST(Assignemt_2, Task4)
{
}

TEST(Assignemt_2, Task5)
{
}