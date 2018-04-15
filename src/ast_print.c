
#include <assert.h>
#include <stdlib.h>
#include "mCc/ast_print.h"

#define LABEL_SIZE 64

char *ptr;

static void print_dot_begin(FILE *out);
static void print_dot_end(FILE *out);
static void print_dot_node(FILE *out, const void *node, const char *label);
static void print_dot_edge(FILE *out, const void *src_node,
			   const void *dst_node, const char *label);

static void print_dot_expression(struct mCc_ast_expression *expression,
				 void *data);
static void
print_dot_expression_single(struct mCc_ast_single_expression *expression,
			    void *data);
static void print_dot_expression_binary(struct mCc_ast_expression *expression,
					void *data);

static void print_dot_identifier(char *identifier, void *data);

static void print_dot_literal(struct mCc_ast_literal *literal, void *data);
static void print_dot_literal_int(struct mCc_ast_literal *literal, void *data);
static void print_dot_literal_float(struct mCc_ast_literal *literal, void *data);
static void print_dot_literal_bool(struct mCc_ast_literal *literal, void *data);
static void print_dot_literal_string(struct mCc_ast_literal *literal, void *data);

static void print_dot_stmt_statement(struct mCc_ast_stmt *stmt, void *data);
static void print_dot_stmt_if(struct mCc_ast_if_stmt *stmt, void *data);
static void print_dot_stmt_while(struct mCc_ast_while_stmt *stmt, void *data);
static void print_dot_stmt_ret(struct mCc_ast_ret_stmt *stmt, void *data);
static void print_dot_stmt_compound(struct mCc_ast_compound_stmt *stmt,
				    void *data);
static void print_dot_stmt_ass(struct mCc_ast_assignment *stmt, void *data);
static void print_dot_stmt_decl(struct mCc_ast_declaration *stmt, void *data);
static void print_dot_compound_stmt(struct mCc_ast_compound_stmt *c_stmt,
				    void *data);

static void print_dot_function_def_void(struct mCc_ast_function_def *f,
					void *data);
static void print_dot_function_def_type(struct mCc_ast_function_def *f,
					void *data);
static void print_dot_func_def_identifier(struct mCc_ast_function_def *f,
										  void *data);
static void print_dot_function_def_parameter(struct mCc_ast_function_def *f,
                                             void *data);
static void print_dot_function_def_stmt(struct mCc_ast_function_def *f,
                                    void *data);

static void print_dot_call_expr(struct mCc_ast_call_expr *expression,
				void *data);

static struct mCc_ast_visitor print_dot_visitor(FILE *out)
{
	assert(out);

	return (struct mCc_ast_visitor){
			.traversal = MCC_AST_VISIT_DEPTH_FIRST,
			.order = MCC_AST_VISIT_PRE_ORDER,

			.userdata = out,

			.identifier = print_dot_identifier,

			.function_def_type = print_dot_function_def_type,
			.function_def_void = print_dot_function_def_void,
            .function_def_identifier = print_dot_func_def_identifier,
            .function_def_parameter = print_dot_function_def_parameter,
            .function_def_stmt = print_dot_function_def_stmt,

            .c_stmt = print_dot_compound_stmt,
            .if_stmt = print_dot_stmt_if,
            .while_stmt = print_dot_stmt_while,
            .ret_stmt = print_dot_stmt_ret,
            .decl_stmt = print_dot_stmt_decl,
            .ass_stmt = print_dot_stmt_ass,
            .statement = print_dot_stmt_statement,

            .expression = print_dot_expression,

	};
}

const char *mCc_ast_print_binary_op(enum mCc_ast_binary_op op)
{
	switch (op) {
	case MCC_AST_BINARY_OP_ADD:
		return "+";
	case MCC_AST_BINARY_OP_SUB:
		return "-";
	case MCC_AST_BINARY_OP_MUL:
		return "*";
	case MCC_AST_BINARY_OP_DIV:
		return "/";
	case MCC_AST_BINARY_OP_AND:
		return "&&";
	case MCC_AST_BINARY_OP_OR:
		return "||";
	case MCC_AST_BINARY_OP_EQ:
		return "==";
	case MCC_AST_BINARY_OP_NEQ:
		return "!=";
	case MCC_AST_BINARY_OP_GE:
		return ">=";
	case MCC_AST_BINARY_OP_GT:
		return ">";
	case MCC_AST_BINARY_OP_LE:
		return "<=";
	case MCC_AST_BINARY_OP_LT:
		return "<";
	default:
		return "unknown";
	}
}

const char *mCc_ast_print_unary_op(enum mCc_ast_unary_op op)
{
	switch (op) {
	case MCC_AST_UNARY_OP_FAC:
		return "Operator: !";
	case MCC_AST_UNARY_OP_NEGATION:
		return "Operator: -";
	default:
		return "unknown";
	}
}

const char *mCc_ast_print_literal_type(enum mCc_ast_literal_type type)
{
	switch (type) {
	case MCC_AST_LITERAL_TYPE_STRING:
		return "type: STRING";
	case MCC_AST_LITERAL_TYPE_INT:
		return "type: INT";
	case MCC_AST_LITERAL_TYPE_FLOAT:
		return "type: FLOAT";
	case MCC_AST_LITERAL_TYPE_BOOL:
		return "type: BOOL";
	default:
		return "unknown";
	}
}
/* ------------------------------------------------------------- DOT Printer */

static void print_dot_begin(FILE *out)
{
	assert(out);

	fprintf(out, "digraph \"AST\" {\n");
	fprintf(out, "\tnodesep=0.6\n");
}

static void print_dot_end(FILE *out)
{
	assert(out);

	fprintf(out, "}\n");
}

static void print_dot_node(FILE *out, const void *node, const char *label)
{
	assert(out);
	assert(node);
	assert(label);

	fprintf(out, "\t\"%p\" [shape=box, label=\"%s\"];\n", node, label);
}

static void print_dot_edge(FILE *out, const void *src_node,
			   const void *dst_node, const char *label)
{
	assert(out);
	assert(src_node);
	assert(dst_node);
	assert(label);

	fprintf(out, "\t\"%p\" -> \"%p\" [label=\"%s\"];\n", src_node, dst_node,
		label);
}

/* ------------------------------------------------------------- Identifier */

static void print_dot_identifier(char *identifier, void *data){
	assert(identifier);
	assert(data);

	FILE *out = data;

	char label[LABEL_SIZE] = {0};
	snprintf(label, sizeof(label), "func_def: %s",
			 identifier);
	print_dot_node(out, identifier, label);
}

static void print_dot_func_def_identifier(struct mCc_ast_function_def *f,
												  void *data){
	assert(f);
	assert(data);

	FILE *out = data;

	char label[LABEL_SIZE] = {0};

    print_dot_node(out, data, "Toplevel");
	snprintf(label, sizeof(label), "func_def: %s",
			 f->identifier);
	print_dot_node(out, f->identifier, label);
	print_dot_edge(out, data, f->identifier, "");
}

/* ------------------------------------------------------------- Literal */

static void print_dot_literal(struct mCc_ast_literal *literal, void *data)
{
	assert(literal);
	assert(data);

	switch (literal->type) {
	case (MCC_AST_LITERAL_TYPE_INT):
		print_dot_literal_int(literal, data);
		break;
	case (MCC_AST_LITERAL_TYPE_STRING):
		print_dot_literal_string(literal, data);
		break;
	case (MCC_AST_LITERAL_TYPE_BOOL):
		print_dot_literal_bool(literal, data);
		break;
	case (MCC_AST_LITERAL_TYPE_FLOAT):
		print_dot_literal_float(literal, data);
		break;
	}
}

static void print_dot_literal_int(struct mCc_ast_literal *literal, void *data)
{
	assert(literal);
	assert(data);

	FILE *out = data;
	char label[LABEL_SIZE] = {0};

	snprintf(label, sizeof(label), "ILiteral: %ld",
					 literal->i_value);
	print_dot_node(out, literal, label);

}

static void print_dot_literal_float(struct mCc_ast_literal *literal, void *data)
{
	assert(literal);
	assert(data);

	FILE *out = data;
	char label[LABEL_SIZE] = {0};

	snprintf(label, sizeof(label), "FLiteral: %lf",
			 literal->f_value);
	print_dot_node(out, literal, label);

}

static void print_dot_literal_bool(struct mCc_ast_literal *literal, void *data)
{
	assert(literal);
	assert(data);

	FILE *out = data;
	char label[LABEL_SIZE] = {0};

	snprintf(label, sizeof(label), "BLiteral: %d",
			 literal->b_value);
	print_dot_node(out, literal, label);

}

static void print_dot_literal_string(struct mCc_ast_literal *literal, void *data)
{
	assert(literal);
	assert(data);

	FILE *out = data;
	char label[LABEL_SIZE] = {0};

	snprintf(label, sizeof(label), "SLiteral: %s",
			 literal->s_value);
	print_dot_node(out, literal, label);

}

/* ------------------------------------------------------------- Expressions */

static void print_dot_expression(struct mCc_ast_expression *expression,
				 void *data)
{
	assert(expression);
	assert(data);

	FILE *out = data;
	char label[LABEL_SIZE] = {0};

	switch (expression->type) {
	case (MCC_AST_EXPRESSION_TYPE_SINGLE):
		snprintf(label, sizeof(label), "Single expression");
		print_dot_node(out, expression->single_expr, label);
		print_dot_edge(out, expression, expression->single_expr, "");
		print_dot_expression_single(expression->single_expr, data);
		break;
	case (MCC_AST_EXPRESSION_TYPE_BINARY):
		print_dot_expression_binary(expression, data);
		break;
	default:
		break;
	}
}

static void
print_dot_expression_single(struct mCc_ast_single_expression *expression,
			    void *data)
{
	assert(expression);
	assert(data);

	FILE *out = data;

	char label[LABEL_SIZE] = {0};

	switch (expression->type) {
	case (MCC_AST_SINGLE_EXPRESSION_TYPE_LITERAL):
		print_dot_literal(expression->literal, data);
		print_dot_edge(out, expression, expression->literal, "");
		break;
	case (MCC_AST_SINGLE_EXPRESSION_TYPE_IDENTIFIER):
		snprintf(label, sizeof(label), "Identifier: %s",
			 expression->only_identifier);
		print_dot_node(out, expression->only_identifier, label);
		print_dot_edge(out, expression, expression->only_identifier,
			       "");
		break;
	case (MCC_AST_SINGLE_EXPRESSION_TYPE_IDENTIFIER_EX):
		snprintf(label, sizeof(label), "Identifier expression:");
		print_dot_node(out, expression->identifier_expression, label);
		print_dot_edge(out, expression,
			       expression->identifier_expression, "");
		print_dot_expression(expression->identifier_expression, data);
		break;
	case (MCC_AST_SINGLE_EXPRESSION_TYPE_CALL_EXPR):
		snprintf(label, sizeof(label), "Call expression:");
		print_dot_node(out, expression->call_expr, label);
		print_dot_edge(out, expression, expression->call_expr, "");
		print_dot_call_expr(expression->call_expr, data);
		break;
	case (MCC_AST_SINGLE_EXPRESSION_TYPE_UNARY_OP):
		snprintf(label, sizeof(label), "%s",
			 mCc_ast_print_unary_op(expression->unary_operator));
		print_dot_node(out, &expression->unary_operator, label);
		print_dot_edge(out, expression, &expression->unary_operator,
			       "");
		snprintf(label, sizeof(label), "Unary expression:");
		print_dot_node(out, expression->unary_expression, label);
		print_dot_edge(out, expression, expression->unary_expression,
			       "");
		print_dot_expression(expression->unary_expression, data);
		break;
	case (MCC_AST_SINGLE_EXPRESSION_TYPE_PARENTH):
		snprintf(label, sizeof(label), "Parenth expression:");
		print_dot_node(out, expression->expression, label);
		print_dot_edge(out, expression, expression->expression, "");
		print_dot_expression(expression->expression, data);
		break;
	default:
		break;
	}
}

static void print_dot_expression_binary(struct mCc_ast_expression *expression,
					void *data)
{
	assert(expression);
	assert(data);

	FILE *out = data;

	char label[LABEL_SIZE] = {0};

	snprintf(label, sizeof(label), "Left hand side:");
	print_dot_node(out, expression->lhs, label);
	print_dot_edge(out, expression, expression->lhs, "");
	print_dot_expression_single(expression->lhs, data);

	const char *text = mCc_ast_print_binary_op(expression->op);

	snprintf(label, sizeof(label), "Operator: %s", text);
	print_dot_node(out, ptr, label);
	print_dot_edge(out, expression, ptr++, "");

	snprintf(label, sizeof(label), "Rigth hand side");
	print_dot_node(out, expression->rhs, label);
	print_dot_edge(out, expression, expression->rhs, "");
	print_dot_expression(expression->rhs, data);
}

static void print_dot_call_expr(struct mCc_ast_call_expr *expression,
				void *data)
{
	assert(expression);
	assert(data);

	FILE *out = data;

	char label[LABEL_SIZE] = {0};

	snprintf(label, sizeof(label), "Operator: %s", expression->identifier);
	print_dot_node(out, expression->identifier, label);
	print_dot_edge(out, expression, expression->identifier, "");

    if(expression->arguments != NULL)
	for (int i = 0; i < expression->arguments->counter; i++) {
		snprintf(label, sizeof(label), "Arguments");
		print_dot_node(out, &expression->arguments->expression[i],
			       label);
		print_dot_edge(out, expression,
			       &expression->arguments->expression[i], "");
		print_dot_expression(&expression->arguments->expression[i],
				     data);
	}
}

/* ----------------------------------------------------------- stmt */

static void print_dot_stmt_statement(struct mCc_ast_stmt *stmt, void *data)
{
	assert(stmt);
	assert(data);

	FILE *out = data;

	char label[LABEL_SIZE] = {0};

	switch (stmt->type) {
	case (MCC_AST_IF_STMT):
		snprintf(label, sizeof(label), "If_Stmt");
		print_dot_node(out, stmt->if_stmt, label);
		print_dot_edge(out, stmt, stmt->if_stmt, "");
		print_dot_stmt_if(stmt->if_stmt, data);
		break;
	case (MCC_AST_WHILE_STMT):
		snprintf(label, sizeof(label), "While_Stmt");
		print_dot_node(out, stmt->while_stmt, label);
		print_dot_edge(out, stmt, stmt->while_stmt, "");
		print_dot_stmt_while(stmt->while_stmt, data);
		break;
	case (MCC_AST_RET_STMT):
		snprintf(label, sizeof(label), "Ret_Stmt");
		print_dot_node(out, stmt->ret_stmt, label);
		print_dot_edge(out, stmt, stmt->ret_stmt, "");
		print_dot_stmt_ret(stmt->ret_stmt, data);
		break;
	case (MCC_AST_DECL_STMT):
		snprintf(label, sizeof(label), "DECL_Stmt");
		print_dot_node(out, stmt->declaration, label);
		print_dot_edge(out, stmt, stmt->declaration, "");
		print_dot_stmt_decl(stmt->declaration, data);
		break;
	case (MCC_AST_ASS_STMT):
		snprintf(label, sizeof(label), "ASSIGNEMNT_Stmt");
		print_dot_node(out, stmt->assignment, label);
		print_dot_edge(out, stmt, stmt->assignment, "");
		print_dot_stmt_ass(stmt->assignment, data);
		break;
	case (MCC_AST_EXPR_STMT):
		snprintf(label, sizeof(label), "EXPRESSION_Stmt");
		print_dot_node(out, stmt->expression, label);
		print_dot_edge(out, stmt, stmt->expression, "");
		print_dot_expression(stmt->expression, data);
		break;
	case (MCC_AST_COMPOUND_STMT):
		snprintf(label, sizeof(label), "COMPOUND_Stmt");
		print_dot_node(out, stmt->compound_stmt, label);
		print_dot_edge(out, stmt, stmt->compound_stmt, "");
		print_dot_stmt_compound(stmt->compound_stmt, data);
		break;
	default:
		// tODO should not occur,
		break;
	}
}

static void print_dot_stmt_if(struct mCc_ast_if_stmt *stmt, void *data)
{
	assert(stmt);
	assert(data);

	FILE *out = data;

	char label[LABEL_SIZE] = {0};

	snprintf(label, sizeof(label), "Expression");

	print_dot_node(out, stmt->expression, label);
	print_dot_edge(out, stmt, stmt->expression, "");

	snprintf(label, sizeof(label), "Statement");

	print_dot_node(out, stmt->statement, label);
	print_dot_edge(out, stmt, stmt->statement, "");

	snprintf(label, sizeof(label), "Statement");
    if(stmt->else_statement != NULL) {
		print_dot_node(out, stmt->else_statement, label);
		print_dot_edge(out, stmt, stmt->else_statement, "");
	}

}

static void print_dot_stmt_while(struct mCc_ast_while_stmt *stmt, void *data)
{
	assert(stmt);
	assert(data);

	FILE *out = data;

	char label[LABEL_SIZE] = {0};

	snprintf(label, sizeof(label), "Expression");

	print_dot_node(out, stmt->expression, label);
	print_dot_edge(out, stmt, stmt->expression, "");
	print_dot_expression(stmt->expression, data);

	snprintf(label, sizeof(label), "Statement");

	print_dot_node(out, stmt->statement, label);
	print_dot_edge(out, stmt, stmt->statement, "");
	print_dot_stmt_statement(stmt->statement, data);
}

static void print_dot_stmt_ret(struct mCc_ast_ret_stmt *stmt, void *data)
{
	assert(stmt);
	assert(data);

	FILE *out = data;

	char label[LABEL_SIZE] = {0};
    if(stmt->expression == NULL) return;
	snprintf(label, sizeof(label), "Expression");
	print_dot_node(out, stmt->expression, label);
	print_dot_edge(out, stmt, stmt->expression, "");
	print_dot_expression(stmt->expression, data);
}

static void print_dot_stmt_compound(struct mCc_ast_compound_stmt *stmt,
				    void *data)
{
	assert(stmt);
	assert(data);

	FILE *out = data;
	char label[LABEL_SIZE] = {0};

	for (int i = 0; i < stmt->counter; i++) {
		snprintf(label, sizeof(label), "Statment");
		print_dot_node(out, &stmt->statements[i], label);
		print_dot_edge(out, stmt, &stmt->statements[i], "");
		print_dot_stmt_statement(&stmt->statements[i], data);
	}
}

static void print_dot_stmt_ass(struct mCc_ast_assignment *stmt, void *data)
{
	assert(stmt);
	assert(data);

	FILE *out = data;

	char label[LABEL_SIZE] = {0};

	snprintf(label, sizeof(label), "Identifier: %s", stmt->identifier);
	print_dot_node(out, stmt->identifier, label);
	print_dot_edge(out, stmt, stmt->identifier, "");

	snprintf(label, sizeof(label), "Expression");
	print_dot_node(out, stmt->expression, label);
	print_dot_edge(out, stmt, stmt->expression, "");
	print_dot_expression(stmt->expression, data);
}

static void print_dot_stmt_decl(struct mCc_ast_declaration *stmt, void *data)
{
	assert(stmt);
	assert(data);

	FILE *out = data;

	char label[LABEL_SIZE] = {0};

	switch (stmt->literal) {
	case (MCC_AST_LITERAL_TYPE_INT):
		snprintf(label, sizeof(label), "Int");
		break;
	case (MCC_AST_LITERAL_TYPE_FLOAT):
		snprintf(label, sizeof(label), "Float");
		break;
	case (MCC_AST_LITERAL_TYPE_BOOL):
		snprintf(label, sizeof(label), "Bool");
		break;
	case (MCC_AST_LITERAL_TYPE_STRING):
		snprintf(label, sizeof(label), "String");
		break;
	default:
		break;
	}
	print_dot_node(out, &stmt->literal, label);
	print_dot_edge(out, stmt, &stmt->literal, "");

	switch (stmt->type) {
	case (MCC_AST_DECLARATION_TYPE_SINGLE):
		snprintf(label, sizeof(label), "Identifier: %s",
			 stmt->identifier);
		print_dot_node(out, stmt->identifier, label);
		print_dot_edge(out, stmt, stmt->identifier, "");
		break;
	case (MCC_AST_DECLARATION_TYPE_ARRAY):
		snprintf(label, sizeof(label), "Numerator: %d",
			 stmt->numerator);
		print_dot_node(out, &stmt->numerator, label);
		print_dot_edge(out, stmt, &stmt->numerator, "");
		snprintf(label, sizeof(label), "Identifier: %s",
			 stmt->array_identifier);
		print_dot_node(out, stmt->array_identifier, label);
		print_dot_edge(out, stmt, stmt->array_identifier, "");
		break;
	}
}

/* ----------------------------------------------------------- compound stmt */
static void print_dot_compound_stmt(struct mCc_ast_compound_stmt *c_stmt,
				    void *data)
{
	assert(c_stmt);
	assert(data);

	FILE *out = data;

	char label[LABEL_SIZE] = {0};

	for (int i = 0; i < c_stmt->counter; i++) {
		/* TYPE */
		switch (c_stmt->statements[i].type) {
		case (MCC_AST_IF_STMT):
			snprintf(label, sizeof(label), "If_Stmt");
			print_dot_node(out, c_stmt->statements[i].if_stmt,
				       label);
			print_dot_edge(out, c_stmt,
				       c_stmt->statements[i].if_stmt, "");
			break;
		case (MCC_AST_WHILE_STMT):
			snprintf(label, sizeof(label), "While_Stmt");
			print_dot_node(out, c_stmt->statements[i].while_stmt,
				       label);
			print_dot_edge(out, c_stmt,
				       c_stmt->statements[i].while_stmt, "");
			break;
		case (MCC_AST_RET_STMT):
			snprintf(label, sizeof(label), "Ret_Stmt");
			print_dot_node(out, c_stmt->statements[i].ret_stmt,
				       label);
			print_dot_edge(out, c_stmt,
				       c_stmt->statements[i].ret_stmt, "");
			break;
		case (MCC_AST_DECL_STMT):
			snprintf(label, sizeof(label), "DECL_Stmt");
			print_dot_node(out, c_stmt->statements[i].declaration,
				       label);
			print_dot_edge(out, c_stmt,
				       c_stmt->statements[i].declaration, "");
			break;
		case (MCC_AST_ASS_STMT):
			snprintf(label, sizeof(label), "ASSIGNEMNT_Stmt");
			print_dot_node(out, c_stmt->statements[i].assignment,
				       label);
			print_dot_edge(out, c_stmt,
				       c_stmt->statements[i].assignment, "");
			break;
		case (MCC_AST_EXPR_STMT):
			snprintf(label, sizeof(label), "EXPRESSION_Stmt");
			print_dot_node(out, c_stmt->statements[i].expression,
				       label);
			print_dot_edge(out, c_stmt,
				       c_stmt->statements[i].expression, "");
			break;
		case (MCC_AST_COMPOUND_STMT):
			snprintf(label, sizeof(label), "COMPOUND_Stmt");
			print_dot_node(out, c_stmt->statements[i].compound_stmt,
				       label);
			print_dot_edge(out, c_stmt,
				       c_stmt->statements[i].compound_stmt, "");
			break;
		default:
			break;
		}
	}
}

/* ----------------------------------------------------------- Parameter*/

static void print_dot_function_def_parameter(struct mCc_ast_function_def *f,
                                             void *data)
{
    assert(f);
    assert(data);

    FILE *out = data;

    char label[LABEL_SIZE] = {0};

    if (f->params->counter > 0) {
        print_dot_node(out, f->params, "Params");
        print_dot_edge(out, f->identifier, f->params, "");
    }

    for (int i = 0; i < f->params->counter; i++) {
        if (f->params->declaration[i].type
            == MCC_AST_DECLARATION_TYPE_SINGLE)
            snprintf(label, sizeof(label), "params: %s",
                     f->params->declaration[i].identifier);
        else
            snprintf(label, sizeof(label), "params: %s",
                     f->params->declaration[i].array_identifier);
        print_dot_node(out, &f->params->declaration[i], label);
        print_dot_edge(out, f->params, &f->params->declaration[i], "");
        print_dot_stmt_decl(&f->params->declaration[i], data);
    }
}


/* ----------------------------------------------------------- Statements*/

static void print_dot_function_def_stmt(struct mCc_ast_function_def *f,
                                        void *data)
{
    assert(f);
    assert(data);

    FILE *out = data;

    print_dot_node(out, f->c_stmt, "Statements");
    print_dot_edge(out, f->identifier, f->c_stmt, "");
}


/* ----------------------------------------------------------- Function Def /
 * CALL */

static void print_dot_function_def_void(struct mCc_ast_function_def *f,
					void *data)
{
	assert(f);
	assert(data);

	FILE *out = data;

	char label[LABEL_SIZE] = {0};

	/* TYPE */
	snprintf(label, sizeof(label), "type: VOID");
	print_dot_node(out, ptr, label);
	print_dot_edge(out, f->identifier, ptr++, "");
}

static void print_dot_function_def_type(struct mCc_ast_function_def *f,
					void *data)
{
	assert(f);
	assert(data);

	FILE *out = data;

	char label[LABEL_SIZE] = {0};

    /* Type of the function */
	snprintf(label, sizeof(label), "%s",
             mCc_ast_print_literal_type(f->l_type));
	print_dot_node(out, ptr, label);
	print_dot_edge(out, f->identifier, ptr++, "");
}

void mCc_ast_print_dot_function_def(FILE *out,
				    struct mCc_ast_function_def_array *f)
{
	assert(out);
	assert(f);
	ptr = malloc(1);
	print_dot_begin(out);


	struct mCc_ast_visitor visitor = print_dot_visitor(out);

	mCc_ast_visit_function_def_array(f, &visitor);

	print_dot_end(out);
}