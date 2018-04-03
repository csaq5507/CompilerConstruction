#include "mCc/ast_visit.h"

#include <assert.h>

#define visit(node, callback, visitor) \
	do { \
		if (callback) { \
			(callback)(node, (visitor)->userdata); \
		} \
	} while (0)

#define visit_if(cond, node, callback, visitor) \
	do { \
		if (cond) { \
			visit(node, callback, visitor); \
		} \
	} while (0)

#define visit_if_pre_order(node, callback, visitor) \
	visit_if((visitor)->order == MCC_AST_VISIT_PRE_ORDER, node, callback, \
	         visitor)

#define visit_if_post_order(node, callback, visitor) \
	visit_if((visitor)->order == MCC_AST_VISIT_POST_ORDER, node, callback, \
	         visitor)

void mCc_ast_visit_expression(struct mCc_ast_expression *expression,
                              struct mCc_ast_visitor *visitor)
{
	assert(expression);
	assert(visitor);

	visit_if_pre_order(expression, visitor->expression, visitor);

	switch (expression->type) {
	case MCC_AST_EXPRESSION_TYPE_SINGLE:
		visit_if_pre_order(expression, visitor->expression_single, visitor);
		mCc_ast_visit_single_expression(expression->single_expr, visitor);
		visit_if_post_order(expression, visitor->expression_single, visitor);
		break;

	case MCC_AST_EXPRESSION_TYPE_BINARY:
		visit_if_pre_order(expression, visitor->expression_binary, visitor);
		mCc_ast_visit_single_expression(expression->lhs, visitor);
		mCc_ast_visit_expression(expression->rhs, visitor);
		visit_if_post_order(expression, visitor->expression_binary, visitor);
		break;

	}

	visit_if_post_order(expression, visitor->expression, visitor);
}

void mCc_ast_visit_literal(struct mCc_ast_literal *literal,
                           struct mCc_ast_visitor *visitor)
{
	assert(literal);
	assert(visitor);

	visit_if_pre_order(literal, visitor->literal, visitor);

	switch (literal->type) {
        case MCC_AST_LITERAL_TYPE_INT:
            visit(literal, visitor->literal_int, visitor);
            break;

        case MCC_AST_LITERAL_TYPE_FLOAT:
            visit(literal, visitor->literal_float, visitor);
            break;

        case MCC_AST_LITERAL_TYPE_BOOL:
            visit(literal, visitor->literal_bool, visitor);
            break;

        case MCC_AST_LITERAL_TYPE_STRING:
            visit(literal, visitor->literal_string, visitor);
            break;
	}

	visit_if_post_order(literal, visitor->literal, visitor);
}

void mCc_ast_visit_single_expression(struct mCc_ast_single_expression *single_expression,
                           struct mCc_ast_visitor *visitor)
{
    assert(single_expression);
    assert(visitor);

    visit_if_pre_order(single_expression, visitor->single_expression, visitor);

    switch (single_expression->type) {
        case MCC_AST_SINGLE_EXPRESSION_TYPE_CALL_EXPR:
            visit(single_expression, visitor->single_expression_call, visitor);
            break;

        case MCC_AST_SINGLE_EXPRESSION_TYPE_IDENTIFIER:
            visit(single_expression, visitor->single_expression_identifier, visitor);
            break;

        case MCC_AST_SINGLE_EXPRESSION_TYPE_PARENTH:
            visit(single_expression, visitor->single_expression_parenth, visitor);
            break;

        case MCC_AST_SINGLE_EXPRESSION_TYPE_UNARY_OP:
            visit(single_expression, visitor->single_expression_unary_op, visitor);
            break;

        case MCC_AST_SINGLE_EXPRESSION_TYPE_LITERAL:
            visit(single_expression, visitor->single_expression_literal, visitor);
            break;
    }

    visit_if_post_order(single_expression, visitor->single_expression, visitor);
}

void mCc_ast_visit_function_def(struct mCc_ast_function_def *f,
                           struct mCc_ast_visitor *visitor)
{
    assert(f);
    assert(visitor);

    visit_if_pre_order(f, visitor->function_def, visitor);

    switch (f->type) {
        case MCC_AST_FUNCTION_DEF_TYPE_TYPE:
            visit(f, visitor->function_def_type, visitor);
            break;

        case MCC_AST_FUNCTION_DEF_TYPE_VOID:
            visit(f, visitor->function_def_void, visitor);
            break;
    }

    visit_if_post_order(f, visitor->function_def, visitor);
}

void mCc_ast_visit_stmt(struct mCc_ast_stmt *stmt,
                           struct mCc_ast_visitor *visitor)
{
    assert(stmt);
    assert(visitor);

    visit_if_pre_order(stmt, visitor->stmt, visitor);

    switch (stmt->type) {
        case MCC_AST_DECL_STMT:
            visit(stmt, visitor->stmt_decl, visitor);
            break;
        case MCC_AST_IF_STMT:
            visit(stmt, visitor->stmt_if, visitor);
            break;
        case MCC_AST_WHILE_STMT:
            visit(stmt, visitor->stmt_while, visitor);
            break;
        case MCC_AST_ASS_STMT:
            visit(stmt, visitor->stmt_ass, visitor);
            break;
        case MCC_AST_EXPR_STMT:
            visit(stmt, visitor->stmt_expr, visitor);
            break;
        case MCC_AST_RET_STMT:
            visit(stmt, visitor->stmt_ret, visitor);
            break;
        case MCC_AST_COMPOUND_STMT:
            visit(stmt, visitor->stmt_compound, visitor);
            break;
    }

    visit_if_post_order(stmt, visitor->stmt, visitor);
}

void mCc_ast_visit_declaration(struct mCc_ast_declaration *decl,
                        struct mCc_ast_visitor *visitor)
{
    assert(decl);
    assert(visitor);

    visit_if_pre_order(decl, visitor->declaration, visitor);

    switch (decl->type) {
        case MCC_AST_DECLARATION_TYPE_ARRAY:
            visit(decl, visitor->declaration_array, visitor);
            break;
        case MCC_AST_DECLARATION_TYPE_SINGLE:
            visit(decl, visitor->declaration_single, visitor);
            break;

    }

    visit_if_post_order(decl, visitor->declaration, visitor);
}