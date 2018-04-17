
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

#define visit_for(visitor,max) for(int i = ((visitor)->order == MCC_AST_VISIT_PRE_ORDER)? 0 : max-1; \
         ((visitor)->order == MCC_AST_VISIT_PRE_ORDER)? i < max : i >= 0; \
         ((visitor)->order == MCC_AST_VISIT_PRE_ORDER)? i++ : i--) {


void mCc_ast_visit_function_def_array(struct mCc_ast_function_def_array *f,
				      struct mCc_ast_visitor *visitor)
{
	assert(f);
	assert(visitor);

    visit_for(visitor,f->counter);
		struct mCc_ast_function_def *func = &f->function_def[i];
		visit(func, visitor->function_def_identifier, visitor);
		switch (func->type) {
		case (MCC_AST_FUNCTION_DEF_TYPE_TYPE):
			visit_if_pre_order(func, visitor->function_def_type,
					   visitor);
			mCc_ast_visit_function_def_type(func, visitor);
			visit_if_post_order(func, visitor->function_def_type,
					    visitor);
			break;
		case (MCC_AST_FUNCTION_DEF_TYPE_VOID):
			visit_if_pre_order(func, visitor->function_def_void,
					   visitor);
			mCc_ast_visit_function_def_void(func, visitor);
			visit_if_post_order(func, visitor->function_def_void,
					    visitor);
			break;
		}
	}
}

void mCc_ast_visit_function_def_type(struct mCc_ast_function_def *f,
				     struct mCc_ast_visitor *visitor)
{
	assert(f);
	assert(visitor);

	visit_if_pre_order(f, visitor->function_def_parameter, visitor);
	mCc_ast_visit_parameter(f->params, visitor);
	visit_if_post_order(f, visitor->function_def_parameter, visitor);
	visit_if_pre_order(f, visitor->function_def_stmt, visitor);
	mCc_ast_visit_stmt(f->c_stmt, visitor);
	visit_if_post_order(f, visitor->function_def_stmt, visitor);

	visit(f, visitor->close_function_def, visitor);
}

void mCc_ast_visit_function_def_void(struct mCc_ast_function_def *f,
				     struct mCc_ast_visitor *visitor)
{
	assert(f);
	assert(visitor);

	visit_if_pre_order(f, visitor->function_def_parameter, visitor);
	mCc_ast_visit_parameter(f->params, visitor);
	visit_if_post_order(f, visitor->function_def_parameter, visitor);
	visit_if_pre_order(f, visitor->function_def_stmt, visitor);
	mCc_ast_visit_stmt(f->c_stmt, visitor);
	visit_if_post_order(f, visitor->function_def_stmt, visitor);

	visit(f, visitor->close_function_def, visitor);
}

void mCc_ast_visit_parameter(struct mCc_ast_parameter *param,
			     struct mCc_ast_visitor *visitor)
{
	assert(param);
	assert(visitor);

    visit_for(visitor,param->counter);
		visit_if_pre_order(&param->declaration[i], visitor->parameter,
				   visitor);
		mCc_ast_visit_decl_stmt(&param->declaration[i], visitor);
		visit_if_post_order(&param->declaration[i], visitor->parameter,
				    visitor);
	}
}

void mCc_ast_visit_stmt(struct mCc_ast_compound_stmt *stmt,
			struct mCc_ast_visitor *visitor)
{
	assert(stmt);
	assert(visitor);

	visit_if_pre_order(stmt, visitor->c_stmt, visitor);
	mCc_ast_visit_compound_stmt(stmt, visitor);
	visit_if_post_order(stmt, visitor->c_stmt, visitor);
}

void mCc_ast_visit_stmt_statement(struct mCc_ast_stmt *stmt,
				  struct mCc_ast_visitor *visitor)
{
	assert(stmt);
	assert(visitor);

	switch (stmt->type) {
	case (MCC_AST_IF_STMT):
		visit_if_pre_order(stmt->if_stmt, visitor->if_stmt, visitor);
		mCc_ast_visit_if_stmt(stmt->if_stmt, visitor);
		visit_if_post_order(stmt->if_stmt, visitor->if_stmt, visitor);
		break;
	case (MCC_AST_WHILE_STMT):
		visit_if_pre_order(stmt->while_stmt, visitor->while_stmt,
				   visitor);
		mCc_ast_visit_while_stmt(stmt->while_stmt, visitor);
		visit_if_post_order(stmt->while_stmt, visitor->while_stmt,
				    visitor);
		break;
	case (MCC_AST_RET_STMT):
		visit_if_pre_order(stmt->ret_stmt, visitor->ret_stmt, visitor);
		mCc_ast_visit_ret_stmt(stmt->ret_stmt, visitor);
		visit_if_post_order(stmt->ret_stmt, visitor->ret_stmt, visitor);
		break;
	case (MCC_AST_DECL_STMT):
		visit(stmt->declaration, visitor->decl_stmt, visitor);
		mCc_ast_visit_decl_stmt(stmt->declaration, visitor);
		visit(stmt->declaration, visitor->decl_stmt, visitor);
		break;
	case (MCC_AST_ASS_STMT):
		visit_if_pre_order(stmt->assignment, visitor->ass_stmt,
				   visitor);
		mCc_ast_visit_ass_stmt(stmt->assignment, visitor);
		visit_if_post_order(stmt->assignment, visitor->ass_stmt,
				    visitor);
		break;
	case (MCC_AST_EXPR_STMT):
		visit_if_pre_order(stmt->expression, visitor->expression,
				   visitor);
		mCc_ast_visit_expression(stmt->expression, visitor);
		visit_if_post_order(stmt->expression, visitor->expression,
				    visitor);
		break;
	case (MCC_AST_COMPOUND_STMT):
		visit_if_pre_order(stmt->compound_stmt, visitor->c_stmt,
				   visitor);
		mCc_ast_visit_compound_stmt(stmt->compound_stmt, visitor);
		visit(stmt->compound_stmt, visitor->close_c_stmt, visitor);
		visit_if_post_order(stmt->compound_stmt, visitor->c_stmt,
				    visitor);
		break;
	default:
		break;
	}
}

void mCc_ast_visit_compound_stmt(struct mCc_ast_compound_stmt *c_stmt,
				 struct mCc_ast_visitor *visitor)
{
	assert(c_stmt);
	assert(visitor);

    visit_for(visitor,c_stmt->counter);
		/* TYPE */
		switch (c_stmt->statements[i].type) {
		case (MCC_AST_IF_STMT):
			visit_if_pre_order(c_stmt->statements[i].if_stmt,
					   visitor->if_stmt, visitor);
			mCc_ast_visit_if_stmt(c_stmt->statements[i].if_stmt,
					      visitor);
			visit_if_post_order(c_stmt->statements[i].if_stmt,
					    visitor->if_stmt, visitor);
			break;
		case (MCC_AST_WHILE_STMT):
			visit_if_pre_order(c_stmt->statements[i].while_stmt,
					   visitor->while_stmt, visitor);
			mCc_ast_visit_while_stmt(
				c_stmt->statements[i].while_stmt, visitor);
			visit_if_post_order(c_stmt->statements[i].while_stmt,
					    visitor->while_stmt, visitor);
			break;
		case (MCC_AST_RET_STMT):
			visit_if_pre_order(c_stmt->statements[i].ret_stmt,
					   visitor->ret_stmt, visitor);
			mCc_ast_visit_ret_stmt(c_stmt->statements[i].ret_stmt,
					       visitor);
			visit_if_post_order(c_stmt->statements[i].ret_stmt,
					    visitor->ret_stmt, visitor);
			break;
		case (MCC_AST_DECL_STMT):
			visit_if_pre_order(c_stmt->statements[i].declaration,
					   visitor->decl_stmt, visitor);
			mCc_ast_visit_decl_stmt(
				c_stmt->statements[i].declaration, visitor);
			visit_if_post_order(c_stmt->statements[i].declaration,
					    visitor->decl_stmt, visitor);
			break;
		case (MCC_AST_ASS_STMT):
			visit_if_pre_order(c_stmt->statements[i].assignment,
					   visitor->ass_stmt, visitor);
			mCc_ast_visit_ass_stmt(c_stmt->statements[i].assignment,
					       visitor);
			visit_if_post_order(c_stmt->statements[i].assignment,
					    visitor->ass_stmt, visitor);
			break;
		case (MCC_AST_EXPR_STMT):
			visit_if_pre_order(c_stmt->statements[i].expression,
					   visitor->expression, visitor);
			mCc_ast_visit_expression(
				c_stmt->statements[i].expression, visitor);
			visit_if_post_order(c_stmt->statements[i].expression,
					    visitor->expression, visitor);
			break;
		case (MCC_AST_COMPOUND_STMT):
			visit_if_pre_order(c_stmt->statements[i].compound_stmt,
					   visitor->c_stmt, visitor);
			mCc_ast_visit_compound_stmt(
				c_stmt->statements[i].compound_stmt, visitor);
			visit(c_stmt->statements[i].compound_stmt,
			      visitor->close_c_stmt, visitor);
			visit_if_post_order(c_stmt->statements[i].compound_stmt,
					    visitor->c_stmt, visitor);
			break;
		default:
			break;
		}
	}
}

void mCc_ast_visit_if_stmt(struct mCc_ast_if_stmt *stmt,
			   struct mCc_ast_visitor *visitor)
{
	assert(stmt);
	assert(visitor);

	visit_if_pre_order(stmt->expression, visitor->expression, visitor);
	mCc_ast_visit_expression(stmt->expression, visitor);
	visit_if_post_order(stmt->expression, visitor->expression, visitor);

	visit_if_pre_order(stmt->statement, visitor->statement, visitor);
	mCc_ast_visit_stmt_statement(stmt->statement, visitor);
	visit_if_post_order(stmt->statement, visitor->statement, visitor);

	if (stmt->else_statement != NULL) {
		visit_if_pre_order(stmt->else_statement, visitor->statement,
				   visitor);
		mCc_ast_visit_stmt_statement(stmt->statement, visitor);
		visit_if_post_order(stmt->else_statement, visitor->statement,
				    visitor);
	}
}

void mCc_ast_visit_while_stmt(struct mCc_ast_while_stmt *stmt,
			      struct mCc_ast_visitor *visitor)
{
	assert(stmt);
	assert(visitor);

	visit_if_pre_order(stmt->expression, visitor->expression, visitor);
	mCc_ast_visit_expression(stmt->expression, visitor);
	visit_if_post_order(stmt->expression, visitor->expression, visitor);

	visit_if_pre_order(stmt->statement, visitor->statement, visitor);
	mCc_ast_visit_stmt_statement(stmt->statement, visitor);
	visit_if_post_order(stmt->statement, visitor->statement, visitor);
}

void mCc_ast_visit_ret_stmt(struct mCc_ast_ret_stmt *stmt,
			    struct mCc_ast_visitor *visitor)
{
	assert(stmt);
	assert(visitor);
	if (stmt->expression == NULL)
		return;
	visit_if_pre_order(stmt->expression, visitor->expression, visitor);
	mCc_ast_visit_expression(stmt->expression, visitor);
	visit_if_post_order(stmt->expression, visitor->expression, visitor);
}

void mCc_ast_visit_ass_stmt(struct mCc_ast_assignment *stmt,
			    struct mCc_ast_visitor *visitor)
{
	assert(stmt);
	assert(visitor);

	visit(stmt->identifier, visitor->identifier, visitor);

	visit_if_pre_order(stmt->expression, visitor->expression, visitor);
	mCc_ast_visit_expression(stmt->expression, visitor);
	visit_if_post_order(stmt->expression, visitor->expression, visitor);
}

void mCc_ast_visit_decl_stmt(struct mCc_ast_declaration *stmt,
			     struct mCc_ast_visitor *visitor)
{
	assert(stmt);
	assert(visitor);

	switch (stmt->type) {
	case (MCC_AST_DECLARATION_TYPE_SINGLE):
		visit(stmt->identifier, visitor->identifier, visitor);
		break;
	case (MCC_AST_DECLARATION_TYPE_ARRAY):
		visit(stmt->array_identifier, visitor->identifier, visitor);
		break;
	}
}

void mCc_ast_visit_expression(struct mCc_ast_expression *expression,
			      struct mCc_ast_visitor *visitor)
{
	assert(expression);
	assert(visitor);

	switch (expression->type) {
	case (MCC_AST_EXPRESSION_TYPE_SINGLE):
		visit_if_pre_order(expression->single_expr,
				   visitor->single_expression, visitor);
		mCc_ast_visit_expression_single(expression->single_expr,
						visitor);
		visit_if_post_order(expression->single_expr,
				    visitor->single_expression, visitor);
		break;
	case (MCC_AST_EXPRESSION_TYPE_BINARY):
		visit_if_pre_order(expression, visitor->binary_expression,
				   visitor);
		mCc_ast_visit_expression_binary(expression, visitor);
		visit_if_post_order(expression, visitor->binary_expression,
				    visitor);
		break;
	}
}

void mCc_ast_visit_expression_single(
	struct mCc_ast_single_expression *expression,
	struct mCc_ast_visitor *visitor)
{
	assert(expression);
	assert(visitor);

	switch (expression->type) {
	case (MCC_AST_SINGLE_EXPRESSION_TYPE_LITERAL):
		mCc_ast_visit_literal(expression->literal, visitor);
		break;
	case (MCC_AST_SINGLE_EXPRESSION_TYPE_IDENTIFIER):
		visit(expression->only_identifier, visitor->identifier,
		      visitor);
		break;
	case (MCC_AST_SINGLE_EXPRESSION_TYPE_IDENTIFIER_EX):
		visit(expression->identifier_expression, visitor->expression,
		      visitor);
		break;
	case (MCC_AST_SINGLE_EXPRESSION_TYPE_CALL_EXPR):
		visit_if_pre_order(expression->call_expr,
				   visitor->call_expression, visitor);
		mCc_ast_visit_call_expression(expression->call_expr, visitor);
		visit_if_post_order(expression->call_expr,
				    visitor->call_expression, visitor);
		break;
	case (MCC_AST_SINGLE_EXPRESSION_TYPE_UNARY_OP):
		visit(expression->unary_expression, visitor->expression,
		      visitor);
		break;
	case (MCC_AST_SINGLE_EXPRESSION_TYPE_PARENTH):
		visit(expression->expression, visitor->expression, visitor);
		break;
	default:
		printf("G\n");
		break;
	}
}

void mCc_ast_visit_expression_binary(struct mCc_ast_expression *expression,
				     struct mCc_ast_visitor *visitor)
{
	assert(expression);
	assert(visitor);

	visit_if_pre_order(expression->lhs, visitor->single_expression,
			   visitor);
	mCc_ast_visit_expression_single(expression->lhs, visitor);
	visit_if_post_order(expression->lhs, visitor->single_expression,
			    visitor);


	visit_if_pre_order(expression->rhs, visitor->expression, visitor);
	mCc_ast_visit_expression(expression->rhs, visitor);
	visit_if_post_order(expression->rhs, visitor->expression, visitor);
}

void mCc_ast_visit_argument(struct mCc_ast_argument *argument,
                           struct mCc_ast_visitor *visitor) {
    assert(argument);
    assert(visitor);

    visit_for(visitor, argument->counter)

        visit_if_pre_order(&argument->expression[i],visitor->expression,visitor);
        mCc_ast_visit_expression(&argument->expression[i],visitor);
        visit_if_post_order(&argument->expression[i],visitor->expression,visitor);

    }
}

void mCc_ast_visit_call_expression(struct mCc_ast_call_expr *expression,
				   struct mCc_ast_visitor *visitor)
{
	assert(expression);
	assert(visitor);

	visit(expression->identifier, visitor->identifier, visitor);

	if (expression->arguments != NULL)
    {
        visit_if_pre_order(expression->arguments,visitor->argument,visitor);
        mCc_ast_visit_argument(expression->arguments,visitor);
        visit_if_post_order(expression->arguments,visitor->argument,visitor);
    }
}

void mCc_ast_visit_literal(struct mCc_ast_literal *literal,
			   struct mCc_ast_visitor *visitor)
{
	assert(literal);
	assert(visitor);

	switch (literal->type) {
	case (MCC_AST_LITERAL_TYPE_INT):
		visit(literal, visitor->i_literal, visitor);
		break;
	case (MCC_AST_LITERAL_TYPE_STRING):
		visit(literal, visitor->s_literal, visitor);
		break;
	case (MCC_AST_LITERAL_TYPE_BOOL):
		visit(literal, visitor->b_literal, visitor);
		break;
	case (MCC_AST_LITERAL_TYPE_FLOAT):
		visit(literal, visitor->f_literal, visitor);
		break;
	}
}