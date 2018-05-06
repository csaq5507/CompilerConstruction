
#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include "mCc/ast_semantic_checks.h"

static void ast_semantic_check_single_expression(
	struct mCc_ast_single_expression *expression, void *data);
static void ast_semantic_check_expression(struct mCc_ast_expression *expression,
					  void *data);

static void
ast_semantic_check_call_expression(struct mCc_ast_call_expr *expression,
				   void *data);

static void ast_semantic_check_ret_stmt(struct mCc_ast_ret_stmt *stmt,
					void *data);
static void ast_semantic_check_ass_stmt(struct mCc_ast_assignment *stmt,
					void *data);
static void ast_semantic_check_if_stmt(struct mCc_ast_if_stmt *stmt,
				       void *data);
static void ast_semantic_check_while_stmt(struct mCc_ast_while_stmt *stmt,
					  void *data);

static struct mCc_ast_visitor symbol_table_visitor(void *data)
{
	assert(data);

	return (struct mCc_ast_visitor){
		.traversal = MCC_AST_VISIT_DEPTH_FIRST,
		.order = MCC_AST_VISIT_POST_ORDER,

		.userdata = data,
		.single_expression = ast_semantic_check_single_expression,
		.expression = ast_semantic_check_expression,
		.call_expression = ast_semantic_check_call_expression,

		.ret_stmt = ast_semantic_check_ret_stmt,
		.ass_stmt = ast_semantic_check_ass_stmt,
		.if_stmt = ast_semantic_check_if_stmt,
		.while_stmt = ast_semantic_check_while_stmt,

	};
};

static void ast_semantic_check_single_expression(
	struct mCc_ast_single_expression *expression, void *data)
{
	assert(expression);
	assert(data);

	switch (expression->type) {
	case (MCC_AST_SINGLE_EXPRESSION_TYPE_LITERAL):
		switch (expression->literal->type) {
		case (MCC_AST_LITERAL_TYPE_INT):
			expression->d_type = MCC_AST_TYPE_INT;
			break;
		case (MCC_AST_LITERAL_TYPE_STRING):
			expression->d_type = MCC_AST_TYPE_STRING;
			break;
		case (MCC_AST_LITERAL_TYPE_BOOL):
			expression->d_type = MCC_AST_TYPE_BOOL;
			break;
		case (MCC_AST_LITERAL_TYPE_FLOAT):
			expression->d_type = MCC_AST_TYPE_FLOAT;
			break;
		}
		break;
	case (MCC_AST_SINGLE_EXPRESSION_TYPE_IDENTIFIER):
		expression->d_type = expression->only_identifier->d_type;
		break;
	case (MCC_AST_SINGLE_EXPRESSION_TYPE_IDENTIFIER_EX):
		//expression->d_type = expression->identifier->d_type;
        if (expression->identifier_expression->d_type != MCC_AST_TYPE_INT) {
            char error_msg[1024] = {0};
            snprintf(error_msg, sizeof(error_msg),
                     "Array numerator not of type int");
            struct mCc_parser_error *error =
                    malloc(sizeof(struct mCc_parser_error));
            strcpy(error->error_msg, error_msg);
            error->error_line = 0;
            g_result->errors = add_parse_error(g_result->errors, error);
            g_result->status = MCC_PARSER_STATUS_ERROR;
        } else {
            switch (expression->identifier->d_type) {
                case (MCC_AST_TYPE_VOID):
                case (MCC_AST_TYPE_INT):
                case (MCC_AST_TYPE_FLOAT):
                case (MCC_AST_TYPE_BOOL):
                case (MCC_AST_TYPE_STRING):
                    break;
                case (MCC_AST_TYPE_INT_ARRAY):
                    expression->d_type = MCC_AST_TYPE_INT;
                    break;
                case (MCC_AST_TYPE_FLOAT_ARRAY):
                    expression->d_type = MCC_AST_TYPE_FLOAT;
                    break;
                case (MCC_AST_TYPE_BOOL_ARRAY):
                    expression->d_type = MCC_AST_TYPE_BOOL;
                    break;
                case (MCC_AST_TYPE_STRING_ARRAY):
                    expression->d_type = MCC_AST_TYPE_STRING;
                    break;
            }
        }


		break;
	case (MCC_AST_SINGLE_EXPRESSION_TYPE_CALL_EXPR):
		expression->d_type = expression->call_expr->d_type;
		break;
	case (MCC_AST_SINGLE_EXPRESSION_TYPE_UNARY_OP):
		// TODO check if type compatible with operator (string not
		// compatible with -)
		expression->d_type = expression->unary_expression->d_type;
		break;
	case (MCC_AST_SINGLE_EXPRESSION_TYPE_PARENTH):
		expression->d_type = expression->expression->d_type;
		break;
	}
}

static void ast_semantic_check_expression(struct mCc_ast_expression *expression,
					  void *data)
{
	assert(expression);
	assert(data);

	switch (expression->type) {
	case (MCC_AST_EXPRESSION_TYPE_SINGLE):
		expression->d_type = expression->single_expr->d_type;
		break;
	case (MCC_AST_EXPRESSION_TYPE_BINARY):
		if (expression->lhs->d_type != expression->rhs->d_type) {
			char error_msg[1024] = {0};
			snprintf(error_msg, sizeof(error_msg),
				 "Expression types not compatible");
			struct mCc_parser_error *error =
				malloc(sizeof(struct mCc_parser_error));
			strcpy(error->error_msg, error_msg);
			error->error_line = 0;
			g_result->errors =
				add_parse_error(g_result->errors, error);
			g_result->status = MCC_PARSER_STATUS_ERROR;
		}
		switch (expression->op) {
			case (MCC_AST_BINARY_OP_ADD):
			case (MCC_AST_BINARY_OP_DIV):
			case (MCC_AST_BINARY_OP_MUL):
			case (MCC_AST_BINARY_OP_SUB):
				expression->d_type = expression->lhs->d_type;
				break;
			case (MCC_AST_BINARY_OP_AND):
			case (MCC_AST_BINARY_OP_EQ):
			case (MCC_AST_BINARY_OP_GE):
			case (MCC_AST_BINARY_OP_GT):
			case (MCC_AST_BINARY_OP_LE):
			case (MCC_AST_BINARY_OP_LT):
			case (MCC_AST_BINARY_OP_NEQ):
			case (MCC_AST_BINARY_OP_OR):
				expression->d_type = MCC_AST_TYPE_BOOL;
				break;
		}

		break;
	}
}

static void
ast_semantic_check_call_expression(struct mCc_ast_call_expr *expression,
				   void *data)
{
	assert(expression);
	assert(data);

	expression->d_type = expression->identifier->d_type;

	if (expression->arguments != NULL) {
		for (int i = 0; i < expression->arguments->counter; i++) {
            if (expression->identifier->param_types != NULL) {
                if (expression->arguments->expression[i].d_type != expression->identifier->param_types[i]) {
                    printf("%d -- %d", expression->arguments->expression[i].d_type, expression->identifier->param_types[i]);
                    char error_msg[1024] = {0};
                    snprintf(error_msg, sizeof(error_msg), "Wrong type of parameter for function %s", expression->identifier->name);
                    struct mCc_parser_error *error =
                            malloc(sizeof(struct mCc_parser_error));
                    strcpy(error->error_msg, error_msg);
                    error->error_line = expression->identifier->node.sloc.start_line;
                    g_result->errors = add_parse_error(g_result->errors, error);
                    g_result->status = MCC_PARSER_STATUS_ERROR;
                }
            }
		}
	}
}

static void ast_semantic_check_ret_stmt(struct mCc_ast_ret_stmt *stmt,
					void *data)
{
	assert(stmt);
	assert(data);

	if(stmt->expression == NULL) {
		if (stmt->d_type != MCC_AST_TYPE_VOID){
			char error_msg[1024] = {0};
			snprintf(error_msg, sizeof(error_msg), "Void function with return type");
			struct mCc_parser_error *error =
					malloc(sizeof(struct mCc_parser_error));
			strcpy(error->error_msg, error_msg);
			error->error_line = 0;
			g_result->errors = add_parse_error(g_result->errors, error);
			g_result->status = MCC_PARSER_STATUS_ERROR;
		}
	} else if (stmt->d_type != stmt->expression->d_type) {
		char error_msg[1024] = {0};
		snprintf(error_msg, sizeof(error_msg), "Wrong return type");
		struct mCc_parser_error *error =
			malloc(sizeof(struct mCc_parser_error));
		strcpy(error->error_msg, error_msg);
		error->error_line = 0;
		g_result->errors = add_parse_error(g_result->errors, error);
		g_result->status = MCC_PARSER_STATUS_ERROR;
	}
}

static void ast_semantic_check_ass_stmt(struct mCc_ast_assignment *stmt,
					void *data)
{
	assert(stmt);
	assert(data);

    if(stmt->numerator != NULL) {
        if (stmt->numerator->d_type != MCC_AST_TYPE_INT) {
            char error_msg[1024] = {0};
            snprintf(error_msg, sizeof(error_msg),
                     "Array numerator not of type int");
            struct mCc_parser_error *error =
                    malloc(sizeof(struct mCc_parser_error));
            strcpy(error->error_msg, error_msg);
            error->error_line = 0;
            g_result->errors = add_parse_error(g_result->errors, error);
            g_result->status = MCC_PARSER_STATUS_ERROR;
        } else {
            switch (stmt->identifier->d_type) {
                case (MCC_AST_TYPE_VOID):
                case (MCC_AST_TYPE_INT):
                case (MCC_AST_TYPE_FLOAT):
                case (MCC_AST_TYPE_BOOL):
                case (MCC_AST_TYPE_STRING):
                    break;
                case (MCC_AST_TYPE_INT_ARRAY):
                    stmt->identifier->d_type = MCC_AST_TYPE_INT;
                    break;
                case (MCC_AST_TYPE_FLOAT_ARRAY):
                    stmt->identifier->d_type = MCC_AST_TYPE_FLOAT;
                    break;
                case (MCC_AST_TYPE_BOOL_ARRAY):
                    stmt->identifier->d_type = MCC_AST_TYPE_BOOL;
                    break;
                case (MCC_AST_TYPE_STRING_ARRAY):
                    stmt->identifier->d_type = MCC_AST_TYPE_STRING;
                    break;
            }
        }
    }
        if (stmt->identifier->d_type != stmt->expression->d_type) {
            char error_msg[1024] = {0};
            snprintf(error_msg, sizeof(error_msg),
                     "Wrong type for assignment");
            struct mCc_parser_error *error =
                    malloc(sizeof(struct mCc_parser_error));
            strcpy(error->error_msg, error_msg);
            error->error_line = 0;
            g_result->errors = add_parse_error(g_result->errors, error);
            g_result->status = MCC_PARSER_STATUS_ERROR;
        }

}

static void ast_semantic_check_if_stmt(struct mCc_ast_if_stmt *stmt, void *data)
{
	assert(stmt);
	assert(data);

	if (stmt->expression->d_type != MCC_AST_TYPE_BOOL) {
		char error_msg[1024] = {0};
		snprintf(error_msg, sizeof(error_msg),
			 "Not boolean expression");
		struct mCc_parser_error *error =
			malloc(sizeof(struct mCc_parser_error));
		strcpy(error->error_msg, error_msg);
		error->error_line = 0;
		g_result->errors = add_parse_error(g_result->errors, error);
		g_result->status = MCC_PARSER_STATUS_ERROR;
	}
}

static void ast_semantic_check_while_stmt(struct mCc_ast_while_stmt *stmt,
					  void *data)
{
	assert(stmt);
	assert(data);

	if (stmt->expression->d_type != MCC_AST_TYPE_BOOL) {
		char error_msg[1024] = {0};
		snprintf(error_msg, sizeof(error_msg),
			 "Not boolean expression");
		struct mCc_parser_error *error =
			malloc(sizeof(struct mCc_parser_error));
		strcpy(error->error_msg, error_msg);
		error->error_line = 0;
		g_result->errors = add_parse_error(g_result->errors, error);
		g_result->status = MCC_PARSER_STATUS_ERROR;
	}
}

struct mCc_parser_result *
mCc_ast_semantic_check(struct mCc_parser_result *result)
{
	assert(result);

	g_result = result;
	struct mCc_ast_function_def_array *f = result->func_def;

	struct mCc_ast_visitor visitor = symbol_table_visitor(result);
	mCc_ast_visit_function_def_array(f, &visitor);

	return g_result;
}
