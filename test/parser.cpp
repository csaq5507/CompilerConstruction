#include <gtest/gtest.h>
#include <stdlib.h>
#include <stdio.h>
#include "mCc/ast.h"
#include "mCc/parser.h"

TEST(Parser, BinaryOp_1)
{
	const char input[] = "/* Comment */ int func1() {192 + 3.14;} int func4() {192 + 3.14;} int func2() {192 + 3.14;}";
	auto result = mCc_parser_parse_string(input);
	
	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

struct mCc_ast_function_def_array *func_def_arr = result.func_def;

    printf("result");
    printf("%d",func_def_arr->counter);


    auto func_def=func_def_arr->function_def;


	ASSERT_EQ(func_def_arr->counter,3);

	// root
	ASSERT_EQ(MCC_AST_FUNCTION_DEF_TYPE_TYPE, func_def->type);
	//ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT, func_def->l_type);

	// root -> lhs
	//ASSERT_EQ(MCC_AST_SINGLE_EXPRESSION_TYPE_LITERAL, expr->lhs->type);

	// root -> lhs -> literal
	//ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT, expr->lhs->literal->type);
	//ASSERT_EQ(192, expr->lhs->literal->i_value);

	// root -> rhs
	//ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_SINGLE, expr->rhs->type);
    //ASSERT_EQ(MCC_AST_SINGLE_EXPRESSION_TYPE_LITERAL, expr->rhs->single_expr->type);

	// root -> rhs -> literal
	//ASSERT_EQ(MCC_AST_LITERAL_TYPE_FLOAT, expr->rhs->single_expr->literal->type);
	//ASSERT_EQ(3.14, expr->rhs->single_expr->literal->type);

	mCc_ast_delete_function_def(func_def);
}
/*
TEST(Parser, Example_File_1) {

	FILE * file;
	file = fopen( "../examples/example_1.mc" , "r");
	if (!file) {
		printf("I can't open a.snazzle.file!\n");
	} else {
		auto result = mCc_parser_parse_file(file);

		ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
	}
}

/*
TEST(Parser, NestedExpression_1)
{
	const char input[] = "42 * (-192 + 3.14)";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto expr = result.expression;

	// root
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_MUL, expr->op);

	// root -> lhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->lhs->type);

	// root -> lhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT, expr->lhs->literal->type);
	ASSERT_EQ(42, expr->lhs->literal->i_value);

	// root -> rhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_PARENTH, expr->rhs->type);

	auto subexpr = expr->rhs->expression;

	// subexpr
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, subexpr->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_ADD, subexpr->op);

	// subexpr -> lhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, subexpr->lhs->type);

	// subexpr -> lhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT, subexpr->lhs->literal->type);
	ASSERT_EQ(-192, subexpr->lhs->literal->i_value);

	// subexpr -> rhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, subexpr->rhs->type);

	// subexpr -> rhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_FLOAT, subexpr->rhs->literal->type);
	ASSERT_EQ(3.14, subexpr->rhs->literal->f_value);

	mCc_ast_delete_expression(expr);
}
*/
TEST(Parser, MissingClosingParenthesis_1)
{
	const char input[] = "(42";
	auto result = mCc_parser_parse_string(input);

	ASSERT_NE(MCC_PARSER_STATUS_OK, result.status);
}
