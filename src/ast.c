#include "mCc/ast.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

/* ---------------------------------------------------------------- Literals */

struct mCc_ast_literal *mCc_ast_new_literal_int(long value)
{
	struct mCc_ast_literal *lit = malloc(sizeof(*lit));
	if (!lit) {
		return NULL;
	}

	lit->type = MCC_AST_LITERAL_TYPE_INT;
	lit->i_value = value;
	return lit;
}

struct mCc_ast_literal *mCc_ast_new_literal_float(double value)
{
	struct mCc_ast_literal *lit = malloc(sizeof(*lit));
	if (!lit) {
		return NULL;
	}

	lit->type = MCC_AST_LITERAL_TYPE_FLOAT;
	lit->f_value = value;
	return lit;
}

struct mCc_ast_literal *mCc_ast_new_literal_bool(bool value)
{
	struct mCc_ast_literal *lit = malloc(sizeof(*lit));
	if (!lit) {
		return NULL;
	}

	lit->type = MCC_AST_LITERAL_TYPE_BOOL;
	lit->b_value = value;
	return lit;
}

struct mCc_ast_literal *mCc_ast_new_literal_string(char* value)
{
	struct mCc_ast_literal *lit = malloc(sizeof(*lit));
	if (!lit) {
		return NULL;
	}

	lit->type = MCC_AST_LITERAL_TYPE_STRING;
	lit->s_value = value;
	return lit;
}

void mCc_ast_delete_literal(struct mCc_ast_literal *literal)
{
	assert(literal);
	free(literal);
}

/* ------------------------------------------------------------- Expressions */

struct mCc_ast_expression *
mCc_ast_new_expression_single(struct mCc_ast_single_expression*single_expr)
{
	assert(single_expr);

	struct mCc_ast_expression *expr = malloc(sizeof(*expr));
	if (!expr) {
		return NULL;
	}

	expr->type = MCC_AST_EXPRESSION_TYPE_SINGLE;
	expr->single_expr = single_expr;
	return expr;
}

struct mCc_ast_expression *
mCc_ast_new_expression_binary_op(enum mCc_ast_binary_op op,
								 struct mCc_ast_single_expression*lhs,
								 struct mCc_ast_expression *rhs)
{
	assert(lhs);
	assert(rhs);

	struct mCc_ast_expression *expr = malloc(sizeof(*expr));
	if (!expr) {
		return NULL;
	}

	expr->type = MCC_AST_EXPRESSION_TYPE_BINARY;
	expr->op = op;
	expr->lhs = lhs;
	expr->rhs = rhs;
	return expr;
}

void mCc_ast_delete_expression(struct mCc_ast_expression *expression)
{
	assert(expression);

	switch (expression->type) {
		case MCC_AST_EXPRESSION_TYPE_SINGLE:
            mCc_ast_delete_single_expression(expression->single_expr);
			break;

		case MCC_AST_EXPRESSION_TYPE_BINARY:
			mCc_ast_delete_single_expression(expression->lhs);
			mCc_ast_delete_expression(expression->rhs);
			break;
	}

	free(expression);
}

struct mCc_ast_single_expression*
mCc_ast_new_single_expression_literal(struct mCc_ast_literal *literal)
{
    assert(literal);

    struct mCc_ast_single_expression *expr = malloc(sizeof(*expr));
    if (!expr) {
        return NULL;
    }

    expr->type = MCC_AST_SINGLE_EXPRESSION_TYPE_LITERAL;
    expr->literal = literal;
    return expr;
}

struct mCc_ast_single_expression*
mCc_ast_new_single_expression_identifier(char* identifier,
                                             struct mCc_ast_expression *identifier_expression)
{
    assert(identifier);
    assert(identifier_expression);

    struct mCc_ast_single_expression *expr = malloc(sizeof(*expr));
    if (!expr) {
        return NULL;
    }
    expr->type = MCC_AST_SINGLE_EXPRESSION_TYPE_IDENTIFIER;
    expr->identifier = identifier;
    expr->identifier_expression = identifier_expression;
    return expr;
}

struct mCc_ast_single_expression*
mCc_ast_new_single_expression_call_expr(struct mCc_ast_call_expr *call_expr)
{
    assert(call_expr);

    struct mCc_ast_single_expression *expr = malloc(sizeof(*expr));
    if (!expr) {
        return NULL;
    }
    expr->type = MCC_AST_SINGLE_EXPRESSION_TYPE_CALL_EXPR;
    expr->call_expr = call_expr;
    return expr;
}

struct mCc_ast_single_expression*
mCc_ast_new_single_expression_unary_op(enum mCc_ast_unary_op unary_op,
                                       struct mCc_ast_expression *unary_expression)
{
    assert(unary_op);
    assert(unary_expression);

    struct mCc_ast_single_expression*expr = malloc(sizeof(*expr));
    if (!expr) {
        return NULL;
    }
    expr->type = MCC_AST_SINGLE_EXPRESSION_TYPE_UNARY_OP;
    expr->unary_operator = unary_op;
    expr->unary_expression = unary_expression;
    return expr;
}

struct mCc_ast_single_expression*
mCc_ast_new_single_expression_parenth(struct mCc_ast_expression *expression)
{
    assert(expression);

    struct mCc_ast_single_expression*expr = malloc(sizeof(*expr));
    if (!expr) {
        return NULL;
    }
    expr->type = MCC_AST_SINGLE_EXPRESSION_TYPE_PARENTH;
    expr->expression = expression;
    return expr;
}


void mCc_ast_delete_single_expression(struct mCc_ast_single_expression *expression)
{
    assert(expression);

    switch (expression->type) {
        case MCC_AST_SINGLE_EXPRESSION_TYPE_LITERAL:
            mCc_ast_delete_literal(expression->literal);
            break;

        case MCC_AST_SINGLE_EXPRESSION_TYPE_IDENTIFIER:
            free(expression->identifier);
            mCc_ast_delete_expression(expression->identifier_expression);
            break;
        case MCC_AST_SINGLE_EXPRESSION_TYPE_UNARY_OP:
            mCc_ast_delete_expression(expression->unary_expression);
            break;
        case MCC_AST_SINGLE_EXPRESSION_TYPE_PARENTH:
            mCc_ast_delete_expression(expression->expression);
            break;
        case MCC_AST_SINGLE_EXPRESSION_TYPE_CALL_EXPR:
            free(expression->call_expr);
            break;
    }

    free(expression);
}


/* ---------------------------------------------------Function def*/

struct mCc_ast_function_def *
mCc_ast_new_void_function_def(char * identifier, struct mCc_ast_parameter *params, struct mCc_ast_compound_stmt *c_stmt)
{
    struct mCc_ast_function_def *f = malloc(sizeof(*f));
    if (!f) {
        return NULL;
    }
    f->type = MCC_AST_FUNCTION_DEF_TYPE_VOID;

    f->void_value = "void";
    f->identifier = identifier;
    f->params = params;
    f->c_stmt = c_stmt;
    return f;
}


struct mCc_ast_function_def *
mCc_ast_new_type_function_def(enum mCc_ast_literal_type type, char * identifier, struct mCc_ast_parameter *params, struct mCc_ast_compound_stmt *c_stmt)
{
    struct mCc_ast_function_def *f = malloc(sizeof(*f));
    if (!f) {
        return NULL;
    }
    f->type = MCC_AST_FUNCTION_DEF_TYPE_VOID;

    f->l_type = type;
    f->identifier = identifier;
    f->params = params;
    f->c_stmt = c_stmt;
    return f;
}

void mCc_ast_delete_function_def(struct mCc_ast_function_def *f)
{
    assert(f);

    if (f->type == MCC_AST_FUNCTION_DEF_TYPE_VOID)
    {
            free(f->void_value);
    }

    free(f);
}

/* --------------------------------------------------- Statement*/


struct mCc_ast_stmt *
mCc_ast_new_if_stmt(struct mCc_ast_if_stmt *if_stmt)
{
    assert(if_stmt);

    struct mCc_ast_stmt *stmt = malloc(sizeof(*stmt));
    if (!stmt) {
        return NULL;
    }
    stmt->type = MCC_AST_IF_STMT;
    stmt->if_stmt = if_stmt;
    return stmt;
}

struct mCc_ast_stmt *
mCc_ast_new_while_stmt(struct mCc_ast_while_stmt *while_stmt)
{
    assert(while_stmt);

    struct mCc_ast_stmt *stmt = malloc(sizeof(*stmt));
    if (!stmt) {
        return NULL;
    }
    stmt->type = MCC_AST_WHILE_STMT;
    stmt->while_stmt = while_stmt;
    return stmt;
}

struct mCc_ast_stmt *
mCc_ast_new_ret_stmt(struct mCc_ast_ret_stmt *ret_stmt)
{
    assert(ret_stmt);

    struct mCc_ast_stmt *stmt = malloc(sizeof(*stmt));
    if (!stmt) {
        return NULL;
    }
    stmt->type = MCC_AST_RET_STMT;
    stmt->ret_stmt = ret_stmt;
    return stmt;
}

struct mCc_ast_stmt *
mCc_ast_new_declaration(struct mCc_ast_declaration *decl_stmt)
{
    assert(decl_stmt);

    struct mCc_ast_stmt *stmt = malloc(sizeof(*stmt));
    if (!stmt) {
        return NULL;
    }
    stmt->type = MCC_AST_DECL_STMT;
    stmt->declaration = decl_stmt;
    return stmt;
}

struct mCc_ast_stmt *
mCc_ast_new_assignment(struct mCc_ast_assignment *ass_stmt)
{
    assert(ass_stmt);

    struct mCc_ast_stmt *stmt = malloc(sizeof(*stmt));
    if (!stmt) {
        return NULL;
    }
    stmt->type = MCC_AST_ASS_STMT;
    stmt->assignment = ass_stmt;
    return stmt;
}

struct mCc_ast_stmt *
mCc_ast_new_expression(struct mCc_ast_expression *expr_stmt)
{
    assert(expr_stmt);

    struct mCc_ast_stmt *stmt = malloc(sizeof(*stmt));
    if (!stmt) {
        return NULL;
    }
    stmt->type = MCC_AST_EXPR_STMT;
    stmt->expression = expr_stmt;
    return stmt;
}

struct mCc_ast_stmt *
mCc_ast_new_compound_stmt(struct mCc_ast_compound_stmt *compound_stmt)
{
    assert(compound_stmt);

    struct mCc_ast_stmt *stmt = malloc(sizeof(*stmt));
    if (!stmt) {
        return NULL;
    }
    stmt->type = MCC_AST_COMPOUND_STMT;
    stmt->compound_stmt = compound_stmt;
    return stmt;
}

void mCc_ast_delete_stmt(struct mCc_ast_stmt *stmt) {
    assert(stmt);

    switch (stmt->type) {
        case MCC_AST_COMPOUND_STMT:
            free(stmt->compound_stmt);
            break;
        case MCC_AST_IF_STMT:
            free(stmt->if_stmt);
            break;
        case MCC_AST_RET_STMT:
            free(stmt->ret_stmt);
            break;
        case MCC_AST_WHILE_STMT:
            free(stmt->while_stmt);
            break;
        case MCC_AST_EXPR_STMT:
            mCc_ast_delete_expression(stmt->expression);
            break;
        case MCC_AST_DECL_STMT:
            free(stmt->declaration);
            break;
        case MCC_AST_ASS_STMT:
            free(stmt->assignment);
            break;
    }

    free(stmt);
}

struct mCc_ast_declaration *
mCc_ast_new_array_declaration(struct mCc_ast_literal *literal,int numerator, char * identifier)
{
    assert(literal);

    struct mCc_ast_declaration *decl = malloc(sizeof(*decl));
    if (!decl) {
        return NULL;
    }
    decl->type = MCC_AST_DECLARATION_TYPE_ARRAY;
    decl->literal = literal;
    decl->numerator = numerator;
    decl->array_identifier= identifier;
    return decl;
}

struct mCc_ast_declaration *
mCc_ast_new_single_declaration(struct mCc_ast_literal *literal, char * identifier)
{
    assert(literal);

    struct mCc_ast_declaration *decl = malloc(sizeof(*decl));
    if (!decl) {
        return NULL;
    }
    decl->type = MCC_AST_DECLARATION_TYPE_SINGLE;
    decl->literal = literal;
    decl->identifier= identifier;
    return decl;
}

void mCc_ast_delete_declaration(struct mCc_ast_declaration * decl)
{
    assert(decl);
    free(decl->literal);

    switch (decl->type) {
        case MCC_AST_DECLARATION_TYPE_SINGLE:
            free(decl->identifier);
            break;
        case MCC_AST_DECLARATION_TYPE_ARRAY:
            free(decl->array_identifier);
            break;
    }

    free(decl);
}


struct mCc_ast_parameter *
mCc_ast_new_parameter_array(struct mCc_ast_parameter * params, struct mCc_ast_declaration * decl)
{
    assert(decl);
    assert(params);
    struct mCc_ast_parameter * new_param = malloc( sizeof(*new_param));
    new_param->declaration = decl;

    int arr_size = sizeof(params) / sizeof(params[0]);

    struct mCc_ast_parameter* new_params = malloc(sizeof(new_params)*arr_size+1);
    memcpy(new_params,params,arr_size);
    memcpy(&(new_params[arr_size]),new_param,1);

    return new_params;
}

struct mCc_ast_parameter *
mCc_ast_new_empty_parameter_array()
{
    return NULL;
}

struct mCc_ast_parameter *
mCc_ast_new_single_parameter(struct mCc_ast_declaration * decl)
{
    assert(decl);

    struct mCc_ast_parameter * new_params = malloc(sizeof(*new_params));
    new_params->declaration=decl;
    return new_params;
}


struct mCc_ast_compound_stmt *
mCc_ast_new_compound_array(struct mCc_ast_compound_stmt* stmts, struct mCc_ast_stmt * stmt)
{
    assert(stmts);
    assert(stmt);

    struct mCc_ast_compound_stmt * new_stmt = malloc(sizeof(*new_stmt));
    new_stmt->statements = stmt;

    int arr_size = sizeof(stmts) / sizeof(stmts[0]);

    struct mCc_ast_compound_stmt *new_stmts = malloc(sizeof(new_stmts)*arr_size+1);
    memcpy(new_stmts,stmts,arr_size);
    memcpy(&(new_stmts[arr_size]),new_stmt,1);

    return new_stmts;
}

struct mCc_ast_compound_stmt * mCc_ast_new_single_compound(struct mCc_ast_stmt * stmt)
{
    assert(stmt);

    struct mCc_ast_compound_stmt * new_stmts = malloc(sizeof(*new_stmts));
    new_stmts->statements = stmt;
    return new_stmts;
}


struct mCc_ast_if_stmt *
mCc_ast_new_if(struct mCc_ast_expression *ex, struct mCc_ast_stmt * stmt)
{
    assert(ex);
    assert(stmt);

    struct mCc_ast_if_stmt * if_stmt = malloc(sizeof(*if_stmt));
    if_stmt->expression = ex;
    if_stmt->statement = stmt;
    if_stmt->else_statement = NULL;

    return if_stmt;
}

struct mCc_ast_if_stmt *
mCc_ast_new_if_else(struct mCc_ast_expression *ex, struct mCc_ast_stmt * stmt, struct mCc_ast_stmt * elsestmt)
{
    assert(ex);
    assert(stmt);

    struct mCc_ast_if_stmt * if_stmt = malloc(sizeof(*if_stmt));
    if_stmt->expression = ex;
    if_stmt->statement = stmt;
    if_stmt->else_statement = elsestmt;

    return if_stmt;
}

struct mCc_ast_while_stmt *
mCc_ast_new_while(struct mCc_ast_expression *ex, struct mCc_ast_stmt * stmt)
{
    assert(ex);
    assert(stmt);

    struct mCc_ast_while_stmt * while_stmt = malloc(sizeof(*while_stmt));
    while_stmt->expression = ex;
    while_stmt->statement = stmt;

    return while_stmt;
}

struct mCc_ast_ret_stmt *
mCc_ast_new_ret(struct mCc_ast_expression *ex)
{
    assert(ex);

    struct mCc_ast_ret_stmt * ret_stmt = malloc(sizeof(*ret_stmt));
    ret_stmt->expression = ex;

    return ret_stmt;
}

struct mCc_ast_ret_stmt *
mCc_ast_new_empty_ret()
{
    struct mCc_ast_ret_stmt * ret_stmt = malloc(sizeof(*ret_stmt));
    ret_stmt->expression = NULL;

    return ret_stmt;
}


struct mCc_ast_assignment *
mCc_ast_new_single_assignment(char * identifier, struct mCc_ast_expression *ex)
{
    assert(ex);

    struct mCc_ast_assignment * ass = malloc(sizeof(*ass));
    ass->identifier = identifier;
    ass->expression = ex;
    ass->numerator = NULL;
    return ass;
}

struct mCc_ast_assignment *
mCc_ast_new_array_assignment(char * identifier, struct mCc_ast_expression *ex,struct mCc_ast_expression *ex2)
{
    assert(ex);
    assert(ex2);

    struct mCc_ast_assignment * ass = malloc(sizeof(*ass));
    ass->identifier = identifier;
    ass->expression = ex2;
    ass->numerator = ex;
    return ass;
}


struct mCc_ast_call_expr *
mCc_ast_new_empty_call_expr(char * identifier)
{
    struct mCc_ast_call_expr * call_expr = malloc(sizeof(*call_expr));
    call_expr->identifier=identifier;
    call_expr->arguments=NULL;
    return call_expr;
}

struct mCc_ast_call_expr *
mCc_ast_new_call_expr(char * identifier, struct mCc_ast_argument * arguments)
{
    assert(arguments);

    struct mCc_ast_call_expr * call_expr = malloc(sizeof(*call_expr));
    call_expr->identifier=identifier;
    call_expr->arguments=arguments;
    return call_expr;
}

struct mCc_ast_argument *
mCc_ast_new_single_argument(struct mCc_ast_expression * ex)
{
    assert(ex);

    struct mCc_ast_argument * argument = malloc(sizeof(*argument));
    argument->expression=ex;
    return argument;
}


struct mCc_ast_argument *
mCc_ast_new_argument_array(struct mCc_ast_argument * arguments, struct mCc_ast_expression * ex)
{
    assert(ex);

    struct mCc_ast_argument * new_argument = malloc( sizeof(*new_argument));
    new_argument->expression = ex;

    int arr_size = sizeof(arguments) / sizeof(arguments[0]);

    struct mCc_ast_argument *new_arguments=malloc(sizeof(new_arguments)*arr_size+1);
    memcpy(new_arguments,arguments,arr_size);
    memcpy(&(new_arguments[arr_size]),new_argument,1);

    return new_arguments;
}
