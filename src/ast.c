#include "mCc/ast.h"

#include <assert.h>
#include <stdlib.h>

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
mCc_ast_new_void_function_def()
{
    struct mCc_ast_function_def *f = malloc(sizeof(*f));
    if (!f) {
        return NULL;
    }
    f->type = MCC_AST_FUNCTION_DEF_TYPE_VOID;

    f->void_value = "void";
    return f;
}

struct mCc_ast_function_def *
mCc_ast_new_type_function_def(struct mCc_ast_literal *type)
{
    struct mCc_ast_function_def *f = malloc(sizeof(*f));
    if (!f) {
        return NULL;
    }
    f->type = MCC_AST_FUNCTION_DEF_TYPE_VOID;

    f->literal = type;
    return f;
}

void mCc_ast_delete_function_def(struct mCc_ast_function_def *f)
{
    assert(f);

    switch (f->type)
    {
        case MCC_AST_FUNCTION_DEF_TYPE_VOID:
            free(f->void_value);
            break;
        case MCC_AST_FUNCTION_DEF_TYPE_TYPE:
            free(f->literal);
            break;
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
    }

    free(stmt);
}
