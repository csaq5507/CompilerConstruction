
#include <assert.h>
#include <stdio.h>
#include "mCc/ast_visit.h"

#define visit(node, callback, visitor)                                         \
	do {                                                                   \
		if (callback) {                                                \
			(callback)(node, (visitor)->userdata);                 \
		}                                                              \
	} while (0)

#define visit_if(cond, node, callback, visitor)                                \
	do {                                                                   \
		if (cond) {                                                    \
			visit(node, callback, visitor);                        \
		}                                                              \
	} while (0)

#define visit_if_pre_order(node, callback, visitor)                            \
	visit_if((visitor)->order == MCC_AST_VISIT_PRE_ORDER, node, callback,  \
		 visitor)

#define visit_if_post_order(node, callback, visitor)                           \
	visit_if((visitor)->order == MCC_AST_VISIT_POST_ORDER, node, callback, \
		 visitor)

/*
void mCc_ast_visit_expression(struct mCc_ast_expression *expression,
			      struct mCc_ast_visitor *visitor)
{
	assert(expression);
	assert(visitor);

	visit_if_pre_order(expression, visitor->expression, visitor);

	switch (expression->type) {
	case MCC_AST_EXPRESSION_TYPE_LITERAL:
		visit_if_pre_order(expression, visitor->expression_literal, visitor);
		mCc_ast_visit_literal(expression->literal, visitor);
		visit_if_post_order(expression, visitor->expression_literal, visitor);
		break;

	case MCC_AST_EXPRESSION_TYPE_BINARY_OP:
		visit_if_pre_order(expression, visitor->expression_binary_op, visitor);
		mCc_ast_visit_expression(expression->lhs, visitor);
		mCc_ast_visit_expression(expression->rhs, visitor);
		visit_if_post_order(expression, visitor->expression_binary_op, visitor);
		break;

	case MCC_AST_EXPRESSION_TYPE_PARENTH:
		visit_if_pre_order(expression, visitor->expression_parenth, visitor);
		mCc_ast_visit_expression(expression->expression, visitor);
		visit_if_post_order(expression, visitor->expression_parenth, visitor);
		break;
	}

	visit_if_post_order(expression, visitor->expression, visitor);
}
*/

void mCc_ast_visit_function_def_array(struct mCc_ast_function_def_array *f,
                                      struct mCc_ast_visitor *visitor)
{
    assert(f);
    assert(visitor);


    for (int i = 0; i < f->counter; i++) {
        struct mCc_ast_function_def *func = &f->function_def[i];
        visit(func, visitor->function_def_identifier, visitor);
        switch (func->type) {
            case (MCC_AST_FUNCTION_DEF_TYPE_TYPE):
                visit_if_pre_order(func, visitor->function_def_type, visitor);
                mCc_ast_visit_function_def_type(func, visitor);
                visit_if_post_order(func, visitor->function_def_type, visitor);
                break;
            case (MCC_AST_FUNCTION_DEF_TYPE_VOID):
                visit_if_pre_order(func, visitor->function_def_void, visitor);
                mCc_ast_visit_function_def_void(func, visitor);
                visit_if_post_order(func, visitor->function_def_void, visitor);
                break;
        }
    }
}

void mCc_ast_visit_function_def_type(struct mCc_ast_function_def *f,
                                     struct mCc_ast_visitor *visitor){
    assert(f);
    assert(visitor);

    visit_if_pre_order(f, visitor->function_def_parameter, visitor);
    mCc_ast_visit_parameter(f->params, visitor);
    visit_if_post_order(f, visitor->function_def_parameter, visitor);
    visit_if_pre_order(f, visitor->function_def_stmt, visitor);
    mCc_ast_visit_stmt(f->c_stmt, visitor);
    visit_if_post_order(f, visitor->function_def_stmt, visitor);
}

void mCc_ast_visit_function_def_void(struct mCc_ast_function_def *f,
                                     struct mCc_ast_visitor *visitor){
    assert(f);
    assert(visitor);

    visit_if_pre_order(f, visitor->function_def_parameter, visitor);
    mCc_ast_visit_parameter(f->params, visitor);
    visit_if_post_order(f, visitor->function_def_parameter, visitor);
    visit_if_pre_order(f, visitor->function_def_stmt, visitor);
    mCc_ast_visit_stmt(f->c_stmt, visitor);
    visit_if_post_order(f, visitor->function_def_stmt, visitor);
}

void mCc_ast_visit_parameter(struct mCc_ast_parameter *param,
                             struct mCc_ast_visitor *visitor){
    assert(param);
    assert(visitor);

    //TODO
}

void mCc_ast_visit_stmt(struct mCc_ast_compound_stmt *stmt,
                        struct mCc_ast_visitor *visitor){
    assert(stmt);
    assert(visitor);

    visit_if_pre_order(stmt, visitor->c_stmt, visitor);
    mCc_ast_visit_compound_stmt(stmt, visitor);
    visit_if_post_order(stmt, visitor->c_stmt, visitor);

}

void mCc_ast_visit_compound_stmt(struct mCc_ast_compound_stmt *c_stmt,
                                 struct mCc_ast_visitor *visitor){
    assert(c_stmt);
    assert(visitor);
    for (int i = 0; i < c_stmt->counter; i++) {
        /* TYPE */
        switch (c_stmt->statements[i].type) {
            case (MCC_AST_IF_STMT):
                visit_if_pre_order(c_stmt->statements[i].if_stmt, visitor->if_stmt, visitor);
                mCc_ast_visit_if_stmt(c_stmt->statements[i].if_stmt, visitor);
                visit_if_post_order(c_stmt->statements[i].if_stmt, visitor->if_stmt, visitor);
                break;
            case (MCC_AST_WHILE_STMT):
                visit(c_stmt->statements[i].while_stmt, visitor->while_stmt, visitor);
                break;
            case (MCC_AST_RET_STMT):
                visit(c_stmt->statements[i].ret_stmt, visitor->ret_stmt, visitor);
                break;
            case (MCC_AST_DECL_STMT):
                visit(c_stmt->statements[i].declaration, visitor->decl_stmt, visitor);
                break;
            case (MCC_AST_ASS_STMT):
                visit(c_stmt->statements[i].assignment, visitor->ass_stmt, visitor);
                break;
            case (MCC_AST_EXPR_STMT):
                visit(c_stmt->statements[i].expression, visitor->expression, visitor);
                break;
            case (MCC_AST_COMPOUND_STMT):
                visit(c_stmt->statements[i].compound_stmt, visitor->c_stmt, visitor);
                break;
            default:
                break;
        }
    }
}

void mCc_ast_visit_if_stmt(struct mCc_ast_if_stmt *stmt,
                           struct mCc_ast_visitor *visitor){
    assert(stmt);
    assert(visitor);

    visit(stmt->expression, visitor->expression, visitor);

    visit(stmt->statement, visitor->statement, visitor);

    if(stmt->else_statement != NULL) {
        visit(stmt->else_statement, visitor->statement, visitor);
    }
}