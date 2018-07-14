#include <gtest/gtest.h>
#include <stdlib.h>
#include <stdio.h>
#include "mCc/ast.h"
#include "mCc/parser.h"
#include "mCc/ast_symbol_table.h"
#include "mCc/ast_semantic_checks.h"
#include "mCc/error.h"

TEST(parser_lexer, ast_generation)
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

	mCc_delete_result(&result);
}

TEST(parser_lexer, check_syntax_error_msg)
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
}

TEST(parser_lexer, literal) {
	mCc_ast_literal *i_literal = mCc_ast_new_literal_int(10);
	mCc_ast_literal *f_literal = mCc_ast_new_literal_float(22.5);
	mCc_ast_literal *b_literal = mCc_ast_new_literal_bool(true);
    char test[] = "String";
	mCc_ast_literal *s_literal = mCc_ast_new_literal_string(test);

	ASSERT_EQ(i_literal->type, MCC_AST_LITERAL_TYPE_INT);
	ASSERT_EQ(i_literal->i_value, 10);

	ASSERT_EQ(f_literal->type, MCC_AST_LITERAL_TYPE_FLOAT);
	ASSERT_EQ(f_literal->f_value, 22.5);

	ASSERT_EQ(b_literal->type, MCC_AST_LITERAL_TYPE_BOOL);
	ASSERT_EQ(b_literal->b_value, true);

	ASSERT_EQ(s_literal->type, MCC_AST_LITERAL_TYPE_STRING);
	ASSERT_STREQ(s_literal->s_value, "String");

	mCc_ast_delete_literal(i_literal, i_literal);
}

TEST(parser_lexer, identifier) {
    char test[] = "var_name";
    char *name = (char *) malloc(sizeof(test));
    strcpy(name, test);
    mCc_ast_identifier *identifier= mCc_ast_new_identifier(name, 6);

    ASSERT_EQ(identifier->size, 0);
    ASSERT_EQ(identifier->node.sloc.start_line, 6);
    ASSERT_EQ(identifier->d_type, MCC_AST_TYPE_VOID);
    ASSERT_STREQ(identifier->name, test);
    ASSERT_STREQ(identifier->renamed, test);

    mCc_ast_delete_identifier(identifier, identifier);
}

TEST(parser_lexer, array_declaration) {
    // mCc_ast_new_array_declaration
}

TEST(parser_lexer, single_declaration) {
    // mCc_ast_new_single_declaration
}

TEST(parser_lexer, array_assignment) {
    // mCc_ast_new_array_assignment
}

TEST(parser_lexer, single_assignment) {
    // mCc_ast_new_single_assignment
}

TEST(parser_lexer, single_expression_literal) {
    // mCc_ast_new_single_expression_literal
}

TEST(parser_lexer, single_expression_identifier_ex) {
    // mCc_ast_new_single_expression_identifier_ex
}

TEST(parser_lexer, single_expression_identifier) {
    // mCc_ast_new_single_expression_identifier
}

TEST(parser_lexer, single_expression_call_expr) {
    // mCc_ast_new_single_expression_call_expr
}

TEST(parser_lexer, single_expression_unary_op) {
    // mCc_ast_new_single_expression_unary_op
}

TEST(parser_lexer, single_expression_parenth) {
    // mCc_ast_new_single_expression_parenth
}

TEST(parser_lexer, single_argument) {
    // mCc_ast_new_single_argument
}

TEST(parser_lexer, argument_array) {
    // mCc_ast_new_argument_array
}

TEST(parser_lexer, empty_call_expr) {
    // mCc_ast_new_empty_call_expr
}

TEST(parser_lexer, call_expr) {
    // mCc_ast_new_call_expr
}

TEST(parser_lexer, expression_single) {
    // mCc_ast_new_expression_single
}

TEST(parser_lexer, expression_binary_op) {
    // mCc_ast_new_expression_binary_op
}

TEST(parser_lexer, single_assignment) {
    // mCc_ast_new_single_assignment
}

TEST(parser_lexer, array_assignment) {
    // mCc_ast_new_array_assignment
}

TEST(parser_lexer, single_declaration) {
    // mCc_ast_new_single_declaration
}

TEST(parser_lexer, array_declaration) {
    // mCc_ast_new_array_declaration
}

TEST(parser_lexer, ret) {
    // mCc_ast_new_ret
}

TEST(parser_lexer, empty_ret) {
    // mCc_ast_new_empty_ret
}

TEST(parser_lexer, while_stmt) {
    // mCc_ast_new_while
}

TEST(parser_lexer, if_else) {
    // mCc_ast_new_if_else
}

TEST(parser_lexer, if_stmt) {
    // mCc_ast_new_if
}

TEST(parser_lexer, compound_stmt) {
    // mCc_ast_new_compound_stmt
}

TEST(parser_lexer, expression) {
    // mCc_ast_new_expression
}

TEST(parser_lexer, declaration) {
    // mCc_ast_new_declaration
}

TEST(parser_lexer, assignment) {
    // mCc_ast_new_assignment
}

TEST(parser_lexer, while_stmt) {
    // mCc_ast_new_while_stmt
}

TEST(parser_lexer, ret_stmt) {
    // mCc_ast_new_ret_stmt
}

TEST(parser_lexer, if_stmt) {
    // mCc_ast_new_if_stmt
}

TEST(parser_lexer, empty_compound) {
    // mCc_ast_new_empty_compound
}

TEST(parser_lexer, compound_array) {
    // mCc_ast_new_compound_array
}

TEST(parser_lexer, single_compound) {
    // mCc_ast_new_single_compound
}

TEST(parser_lexer, parameter_array) {
    // mCc_ast_new_parameter_array
}

TEST(parser_lexer, empty_parameter_array) {
    // mCc_ast_new_empty_parameter_array
}

TEST(parser_lexer, single_parameter) {
    // mCc_ast_new_single_parameter
}

TEST(parser_lexer, type_function_def) {
    // mCc_ast_new_type_function_def
}

TEST(parser_lexer, void_function_def) {
    // mCc_ast_new_void_function_def
}

TEST(parser_lexer, function_def_array) {
    // mCc_ast_new_function_def_array
}

TEST(parser_lexer, function_def_to_array) {
    // mCc_ast_add_function_def_to_array
}

TEST(parser_lexer, gen_func_def) {
    // mCc_ast_gen_func_def
}