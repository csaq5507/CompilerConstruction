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
		"int func1() {192 + 3.14;} "
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

TEST(parser_lexer, literal)
{
	mCc_ast_literal *i_literal = mCc_ast_new_literal_int(10);
	mCc_ast_literal *f_literal = mCc_ast_new_literal_float(22.5);
    char bool_lit[] = "true";
	mCc_ast_literal *b_literal = mCc_ast_new_literal_bool(bool_lit);
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

	free(i_literal);
    free(f_literal);
    free(b_literal);
    free(s_literal->s_value);
    free(s_literal);
}

TEST(parser_lexer, identifier)
{
    char test[] = "var_name";
    char *name = (char *) malloc(sizeof(test));
    strcpy(name, test);
    mCc_ast_identifier *identifier= mCc_ast_new_identifier(name, 6);

    ASSERT_EQ(identifier->size, 0);
    ASSERT_EQ(identifier->node.sloc.start_line, 6);
    ASSERT_EQ(identifier->d_type, MCC_AST_TYPE_VOID);
    ASSERT_STREQ(identifier->name, test);
    ASSERT_STREQ(identifier->renamed, test);

    free(identifier->name);
    free(identifier->renamed);
    free(identifier);
}

TEST(parser_lexer, array_declaration)
{
    ast_identifier *identifier = (ast_identifier *) malloc(sizeof(ast_identifier));

    ast_declaration *declaration_int =
            mCc_ast_new_array_declaration(MCC_AST_LITERAL_TYPE_INT, 5, identifier);
    ast_declaration *declaration_string =
            mCc_ast_new_array_declaration(MCC_AST_LITERAL_TYPE_STRING, 10, identifier);
    ast_declaration *declaration_float =
            mCc_ast_new_array_declaration(MCC_AST_LITERAL_TYPE_FLOAT, 500, identifier);
    ast_declaration *declaration_bool =
            mCc_ast_new_array_declaration(MCC_AST_LITERAL_TYPE_BOOL, 0, identifier);

    ASSERT_EQ(declaration_int->type, MCC_AST_DECLARATION_TYPE_ARRAY);
    ASSERT_EQ(declaration_int->numerator, 5);
    ASSERT_EQ(declaration_int->literal, MCC_AST_LITERAL_TYPE_INT);
    ASSERT_TRUE(declaration_int->array_identifier == identifier);

    ASSERT_EQ(declaration_string->type, MCC_AST_DECLARATION_TYPE_ARRAY);
    ASSERT_EQ(declaration_string->numerator, 10);
    ASSERT_EQ(declaration_string->literal, MCC_AST_LITERAL_TYPE_STRING);
    ASSERT_TRUE(declaration_string->array_identifier == identifier);

    ASSERT_EQ(declaration_float->type, MCC_AST_DECLARATION_TYPE_ARRAY);
    ASSERT_EQ(declaration_float->numerator, 500);
    ASSERT_EQ(declaration_float->literal, MCC_AST_LITERAL_TYPE_FLOAT);
    ASSERT_TRUE(declaration_float->array_identifier == identifier);

    ASSERT_EQ(declaration_bool->type, MCC_AST_DECLARATION_TYPE_ARRAY);
    ASSERT_EQ(declaration_bool->numerator, 0);
    ASSERT_EQ(declaration_bool->literal, MCC_AST_LITERAL_TYPE_BOOL);
    ASSERT_TRUE(declaration_bool->array_identifier == identifier);

    free(identifier);
    free(declaration_int);
    free(declaration_bool);
    free(declaration_float);
    free(declaration_string);
}

TEST(parser_lexer, single_declaration)
{
    ast_identifier *identifier = (ast_identifier *) malloc(sizeof(ast_identifier));

    ast_declaration *declaration_int =
            mCc_ast_new_single_declaration(MCC_AST_LITERAL_TYPE_INT, identifier);
    ast_declaration *declaration_string =
            mCc_ast_new_single_declaration(MCC_AST_LITERAL_TYPE_STRING, identifier);
    ast_declaration *declaration_float =
            mCc_ast_new_single_declaration(MCC_AST_LITERAL_TYPE_FLOAT, identifier);
    ast_declaration *declaration_bool =
            mCc_ast_new_single_declaration(MCC_AST_LITERAL_TYPE_BOOL, identifier);

    ASSERT_EQ(declaration_int->type, MCC_AST_DECLARATION_TYPE_SINGLE);
    ASSERT_EQ(declaration_int->literal, MCC_AST_LITERAL_TYPE_INT);
    ASSERT_TRUE(declaration_int->identifier == identifier);

    ASSERT_EQ(declaration_string->type, MCC_AST_DECLARATION_TYPE_SINGLE);
    ASSERT_EQ(declaration_string->literal, MCC_AST_LITERAL_TYPE_STRING);
    ASSERT_TRUE(declaration_string->identifier == identifier);

    ASSERT_EQ(declaration_float->type, MCC_AST_DECLARATION_TYPE_SINGLE);
    ASSERT_EQ(declaration_float->literal, MCC_AST_LITERAL_TYPE_FLOAT);
    ASSERT_TRUE(declaration_float->identifier == identifier);

    ASSERT_EQ(declaration_bool->type, MCC_AST_DECLARATION_TYPE_SINGLE);
    ASSERT_EQ(declaration_bool->literal, MCC_AST_LITERAL_TYPE_BOOL);
    ASSERT_TRUE(declaration_bool->identifier == identifier);

    free(identifier);
    free(declaration_int);
    free(declaration_bool);
    free(declaration_float);
    free(declaration_string);
}

TEST(parser_lexer, array_assignment)
{
    ast_identifier *identifier = (ast_identifier *) malloc(sizeof(ast_identifier));

    ast_expr *ex_1 = (ast_expr*) malloc(sizeof(ast_expr));
    ast_expr *ex_2 = (ast_expr*) malloc(sizeof(ast_expr));

    ast_assignment *assignment =
            mCc_ast_new_array_assignment(identifier, ex_1, ex_2);

    ASSERT_TRUE(assignment->identifier == identifier);
    ASSERT_TRUE(assignment->numerator == ex_1);
    ASSERT_TRUE(assignment->expression == ex_2);

    free(identifier);
    free(ex_1);
    free(ex_2);
    free(assignment);
}

TEST(parser_lexer, single_assignment)
{
    ast_identifier *identifier = (ast_identifier *) malloc(sizeof(ast_identifier));

    ast_expr *ex = (ast_expr*) malloc(sizeof(ast_expr));


    ast_assignment *assignment =
            mCc_ast_new_single_assignment(identifier, ex);

    ASSERT_TRUE(assignment->identifier == identifier);
    ASSERT_TRUE(assignment->expression == ex);
    ASSERT_TRUE(assignment->numerator == NULL);

    free(ex);
    free(identifier);
    free(assignment);
}

TEST(parser_lexer, single_expression_literal)
{

    ast_literal *literal = (ast_literal*) malloc(sizeof(ast_literal));

    ast_single_expr *single_expr = mCc_ast_new_single_expression_literal(literal);

    ASSERT_EQ(single_expr->type, MCC_AST_SINGLE_EXPRESSION_TYPE_LITERAL);
    ASSERT_TRUE(single_expr->literal == literal);

    free(literal);
    free(single_expr);
}

TEST(parser_lexer, single_expression_identifier_ex)
{

    ast_identifier *identifier = (ast_identifier *) malloc(sizeof(ast_identifier));
    ast_expr *exp = (ast_expr *) malloc(sizeof(ast_expr));

    ast_single_expr *single_expr =
            mCc_ast_new_single_expression_identifier_ex(identifier, exp);

    ASSERT_EQ(single_expr->type, MCC_AST_SINGLE_EXPRESSION_TYPE_IDENTIFIER_EX);
    ASSERT_TRUE(single_expr->identifier == identifier);
    ASSERT_TRUE(single_expr->identifier_expression == exp);

    free(identifier);
    free(exp);
    free(single_expr);
}

TEST(parser_lexer, single_expression_identifier)
{

    ast_identifier *identifier = (ast_identifier *) malloc(sizeof(ast_identifier));

    ast_single_expr *single_expr =
            mCc_ast_new_single_expression_identifier(identifier);

    ASSERT_EQ(single_expr->type, MCC_AST_SINGLE_EXPRESSION_TYPE_IDENTIFIER);
    ASSERT_TRUE(single_expr->identifier == identifier);

    free(identifier);
    free(single_expr);
}

TEST(parser_lexer, single_expression_call_expr)
{

    ast_call_expr *call_expr = (ast_call_expr *) malloc(sizeof(ast_call_expr));

    ast_single_expr *single_expr =
            mCc_ast_new_single_expression_call_expr(call_expr);

    ASSERT_EQ(single_expr->type, MCC_AST_SINGLE_EXPRESSION_TYPE_CALL_EXPR);
    ASSERT_TRUE(single_expr->call_expr == call_expr);

    free(call_expr);
    free(single_expr);
}

TEST(parser_lexer, single_expression_unary_op)
{

    ast_expr *unary_expr = (ast_expr *) malloc(sizeof(ast_expr));

    ast_single_expr *single_expr_neg =
            mCc_ast_new_single_expression_unary_op(MCC_AST_UNARY_OP_NEGATION,
                                                   unary_expr);
    ast_single_expr *single_expr_fac =
            mCc_ast_new_single_expression_unary_op(MCC_AST_UNARY_OP_FAC,
                                                   unary_expr);

    ASSERT_EQ(single_expr_neg->type, MCC_AST_SINGLE_EXPRESSION_TYPE_UNARY_OP);
    ASSERT_TRUE(single_expr_neg->unary_expression == unary_expr);
    ASSERT_EQ(single_expr_neg->unary_operator, MCC_AST_UNARY_OP_NEGATION);

    ASSERT_EQ(single_expr_fac->type, MCC_AST_SINGLE_EXPRESSION_TYPE_UNARY_OP);
    ASSERT_TRUE(single_expr_fac->unary_expression == unary_expr);
    ASSERT_EQ(single_expr_fac->unary_operator, MCC_AST_UNARY_OP_FAC);

    free(unary_expr);
    free(single_expr_neg);
    free(single_expr_fac);
}

TEST(parser_lexer, single_expression_parenth)
{

    ast_expr *expression = (ast_expr *) malloc(sizeof(ast_expr));

    ast_single_expr *single_expr =
            mCc_ast_new_single_expression_parenth(expression);

    ASSERT_EQ(single_expr->type, MCC_AST_SINGLE_EXPRESSION_TYPE_PARENTH);
    ASSERT_TRUE(single_expr->expression == expression);

    free(expression);
    free(single_expr);
}

TEST(parser_lexer, single_argument)
{

    ast_expr *expression = (ast_expr *) malloc(sizeof(ast_expr));

    ast_argument *argument = mCc_ast_new_single_argument(expression);

    ASSERT_TRUE(argument->expression == expression);
    ASSERT_EQ(argument->counter, 1);

    free(expression);
    free(argument);
}

TEST(parser_lexer, empty_call_expr)
{

    ast_identifier *identifier = (ast_identifier *) malloc(sizeof(ast_identifier));

    ast_call_expr *call_expr =
            mCc_ast_new_empty_call_expr(identifier);

    ASSERT_TRUE(call_expr->identifier == identifier);

    free(identifier);
    free(call_expr);
}

TEST(parser_lexer, call_expr)
{

    ast_identifier *identifier = (ast_identifier *) malloc(sizeof(ast_identifier));
    ast_argument *arguments = (ast_argument *) malloc(sizeof(ast_argument));

    ast_call_expr *call_expr = mCc_ast_new_call_expr(identifier, arguments);

    ASSERT_TRUE(call_expr->identifier == identifier);
    ASSERT_TRUE(call_expr->arguments == arguments);

    free(identifier);
    free(arguments);
    free(call_expr);
}

TEST(parser_lexer, expression_single)
{

    ast_single_expr *single_expr = (ast_single_expr *) malloc(sizeof(ast_single_expr));

    ast_expr *expr = mCc_ast_new_expression_single(single_expr);

    ASSERT_EQ(expr->type, MCC_AST_EXPRESSION_TYPE_SINGLE);
    ASSERT_TRUE(expr->single_expr == single_expr);

    free(single_expr);
    free(expr);
}

TEST(parser_lexer, expression_binary_op)
{

    ast_single_expr *lhs = (ast_single_expr *) malloc(sizeof(ast_single_expr));
    ast_expr *rhs = (ast_expr *) malloc(sizeof(ast_expr));


    ast_expr *expr = mCc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_OR,
                                               lhs, rhs);

    ASSERT_EQ(expr->type, MCC_AST_EXPRESSION_TYPE_BINARY);
    ASSERT_EQ(expr->op, MCC_AST_BINARY_OP_OR);
    ASSERT_TRUE(expr->lhs == lhs);
    ASSERT_TRUE(expr->rhs == rhs);


    free(lhs);
    free(rhs);
    free(expr);
}

TEST(parser_lexer, ret)
{

    ast_expr *ex = (ast_expr *) malloc(sizeof(ast_expr));

    ast_ret_stmt *ret = mCc_ast_new_ret(ex);

    ASSERT_TRUE(ret->expression == ex);

    free(ex);
    free(ret);

}

TEST(parser_lexer, empty_ret)
{


    ast_ret_stmt *ret = mCc_ast_new_empty_ret();

    ASSERT_TRUE(ret->expression == NULL);

    free(ret);
}

TEST(parser_lexer, while_stmt)
{

    ast_while_stmt *while_st = (ast_while_stmt *) malloc(sizeof(ast_while_stmt));

    ast_stmt *while_stmt = mCc_ast_new_while_stmt(while_st);

    ASSERT_EQ(while_stmt->type, MCC_AST_WHILE_STMT);
    ASSERT_TRUE(while_stmt->while_stmt == while_st);

    free(while_st);
    free(while_stmt);

}

TEST(parser_lexer, if_stmt)
{

    ast_stmt *stmt = (ast_stmt *) malloc(sizeof(ast_stmt));
    ast_expr *ex = (ast_expr *) malloc(sizeof(ast_expr));

    ast_if_stmt *if_stmt = mCc_ast_new_if(ex, stmt);

    ASSERT_TRUE(if_stmt->statement == stmt);
    ASSERT_TRUE(if_stmt->expression == ex);

    free(stmt);
    free(ex);
    free(if_stmt);
}

TEST(parser_lexer, if_else)
{

    ast_expr *ex = (ast_expr *) malloc(sizeof(ast_expr));
    ast_stmt *stmt = (ast_stmt *) malloc(sizeof(ast_stmt));
    ast_stmt *else_stmt = (ast_stmt *) malloc(sizeof(ast_stmt));

    ast_if_stmt *if_stmt = mCc_ast_new_if_else(ex, stmt, else_stmt);

    ASSERT_TRUE(if_stmt->expression == ex);
    ASSERT_TRUE(if_stmt->statement == stmt);
    ASSERT_TRUE(if_stmt->else_statement == else_stmt);

    free(ex);
    free(stmt);
    free(else_stmt);
    free(if_stmt);
}

TEST(parser_lexer, compound_stmt)
{

    ast_compound_stmt *compound_stmt = (ast_compound_stmt *) malloc(sizeof(ast_compound_stmt));

    ast_stmt *cmp_stmt = mCc_ast_new_compound_stmt(compound_stmt);

    ASSERT_EQ(cmp_stmt->type, MCC_AST_COMPOUND_STMT);
    ASSERT_TRUE(cmp_stmt->compound_stmt == compound_stmt);

    free(compound_stmt);
    free(cmp_stmt);
}

TEST(parser_lexer, declaration)
{

    ast_declaration *decl_stmt = (ast_declaration *) malloc(sizeof(ast_declaration));

    ast_stmt *stmt = mCc_ast_new_declaration(decl_stmt);

    ASSERT_EQ(stmt->type, MCC_AST_DECL_STMT);
    ASSERT_TRUE(stmt->declaration == decl_stmt);

    free(decl_stmt);
    free(stmt);


}

TEST(parser_lexer, assignment)
{

    ast_assignment *ass_stmt = (ast_assignment *) malloc(sizeof(ast_assignment));

    ast_stmt *stmt = mCc_ast_new_assignment(ass_stmt);

    ASSERT_EQ(stmt->type, MCC_AST_ASS_STMT);
    ASSERT_TRUE(stmt->assignment == ass_stmt);

    free(ass_stmt);
    free(stmt);
}

TEST(parser_lexer, empty_compound)
{

    ast_compound_stmt *cmp_stmt = mCc_ast_new_empty_compound();

    ASSERT_TRUE(cmp_stmt->statements == NULL);
    ASSERT_EQ(cmp_stmt->counter, 0);

    free(cmp_stmt);
}

TEST(parser_lexer, single_compound)
{
    ast_stmt *stmt = (ast_stmt *) malloc(sizeof(ast_stmt));

    ast_compound_stmt *cmp_stmt = mCc_ast_new_single_compound(stmt);

    ASSERT_EQ(cmp_stmt->counter, 1);
    ASSERT_TRUE(cmp_stmt->statements == stmt);

    free(stmt);
    free(cmp_stmt);
}

TEST(parser_lexer, parameter_array)
{

    ast_parameter *params = mCc_ast_new_empty_parameter_array();

    ASSERT_EQ(params->counter, 0);


    free(params);
}

TEST(parser_lexer, empty_parameter_array)
{

    ast_parameter *param = mCc_ast_new_empty_parameter_array();

    ASSERT_EQ(param->counter, 0);

    free(param);
}

TEST(parser_lexer, single_parameter)
{

    ast_declaration *decl = (ast_declaration *) malloc(sizeof(ast_declaration));

    ast_parameter *param = mCc_ast_new_single_parameter(decl);

    ASSERT_EQ(param->counter, 1);
    ASSERT_TRUE(param->declaration == decl);

    free(decl);
    free(param);
}

TEST(parser_lexer, type_function_def)
{

    ast_identifier *identifier = (ast_identifier *) malloc(sizeof(ast_identifier));
    ast_parameter *params = (ast_parameter *) malloc(sizeof(ast_parameter));
    ast_compound_stmt *c_stmt = (ast_compound_stmt *) malloc(sizeof(ast_compound_stmt));

    ast_function_def *func =
            mCc_ast_new_type_function_def(MCC_AST_LITERAL_TYPE_STRING,
                                          identifier, params, c_stmt);

    ASSERT_EQ(func->type, MCC_AST_FUNCTION_DEF_TYPE_TYPE);
    ASSERT_EQ(func->l_type, MCC_AST_LITERAL_TYPE_STRING);
    ASSERT_TRUE(func->identifier == identifier);
    ASSERT_TRUE(func->params == params);
    ASSERT_TRUE(func->c_stmt == c_stmt);

    free(identifier);
    free(params);
    free(c_stmt);
    free(func);
}

TEST(parser_lexer, void_function_def)
{

    ast_identifier *identifier = (ast_identifier *) malloc(sizeof(ast_identifier));
    ast_parameter *params = (ast_parameter *) malloc(sizeof(ast_parameter));
    ast_compound_stmt *c_stmt = (ast_compound_stmt *) malloc(sizeof(ast_compound_stmt));

    ast_function_def *func =
            mCc_ast_new_void_function_def(identifier, params, c_stmt);

    ASSERT_EQ(func->type, MCC_AST_FUNCTION_DEF_TYPE_VOID);
    ASSERT_STREQ(func->void_value, "void");
    ASSERT_TRUE(func->identifier == identifier);
    ASSERT_TRUE(func->params == params);
    ASSERT_TRUE(func->c_stmt == c_stmt);

    free(identifier);
    free(params);
    free(c_stmt);
    free(func);
}

TEST(parser_lexer, function_def_array)
{

    ast_function_def *f = (ast_function_def *) malloc(sizeof(ast_function_def));

    ast_function_def_array *func = mCc_ast_new_function_def_array(f);

    ASSERT_EQ(func->counter, 1);
    ASSERT_TRUE(func->function_def == f);

    free(f);
    free(func);
}