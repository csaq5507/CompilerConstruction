
#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include "mCc/ast_print.h"

#define LABEL_SIZE 4096

char *ptr;
int counter;

static void print_dot_begin(FILE *out);
static void print_dot_end(FILE *out);
static void print_dot_node(FILE *out, const void *node, const char *label);
static void print_dot_arrow(FILE *out, const void *node,const char * label);
static void print_dot_edge(FILE *out, const void *src_node,
			   const void *dst_node, const char *label);

static void print_dot_expression(struct mCc_ast_expression *expression,
				 void *data);
static void
print_dot_expression_single(struct mCc_ast_single_expression *expression,
			    void *data);

static void print_dot_literal_int(struct mCc_ast_literal *literal, void *data);
static void print_dot_literal_float(struct mCc_ast_literal *literal,
				    void *data);
static void print_dot_literal_bool(struct mCc_ast_literal *literal, void *data);
static void print_dot_literal_string(struct mCc_ast_literal *literal,
				     void *data);

static void print_dot_stmt_statement(struct mCc_ast_stmt *stmt, void *data);
static void print_dot_stmt_if(struct mCc_ast_if_stmt *stmt, void *data);
static void print_dot_stmt_while(struct mCc_ast_while_stmt *stmt, void *data);
static void print_dot_stmt_ret(struct mCc_ast_ret_stmt *stmt, void *data);
static void print_dot_stmt_ass(struct mCc_ast_assignment *stmt, void *data);
static void print_dot_stmt_decl(struct mCc_ast_declaration *stmt, void *data);
static void print_dot_compound_stmt(struct mCc_ast_compound_stmt *c_stmt,
				    void *data);

static void print_dot_function_def_void(struct mCc_ast_function_def *f,
					void *data);
static void print_dot_function_def_type(struct mCc_ast_function_def *f,
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

		.i_literal = print_dot_literal_int,
		.f_literal = print_dot_literal_float,
		.b_literal = print_dot_literal_bool,
		.s_literal = print_dot_literal_string,

		.function_def_type = print_dot_function_def_type,
		.function_def_void = print_dot_function_def_void,

		.c_stmt = print_dot_compound_stmt,
		.if_stmt = print_dot_stmt_if,
		.while_stmt = print_dot_stmt_while,
		.ret_stmt = print_dot_stmt_ret,
		.decl_stmt = print_dot_stmt_decl,
		.ass_stmt = print_dot_stmt_ass,
		.statement = print_dot_stmt_statement,

		.expression = print_dot_expression,
		.single_expression = print_dot_expression_single,
		.call_expression = print_dot_call_expr,

	};
}

const char *mCc_ast_print_binary_op(enum mCc_ast_binary_op op)
{
	switch (op) {
	case MCC_AST_BINARY_OP_ADD:
		return "Operator: +";
	case MCC_AST_BINARY_OP_SUB:
		return "Operator: -";
	case MCC_AST_BINARY_OP_MUL:
		return "Operator: *";
	case MCC_AST_BINARY_OP_DIV:
		return "Operator: /";
	case MCC_AST_BINARY_OP_AND:
		return "Operator: &&";
	case MCC_AST_BINARY_OP_OR:
		return "Operator: ||";
	case MCC_AST_BINARY_OP_EQ:
		return "Operator: ==";
	case MCC_AST_BINARY_OP_NEQ:
		return "Operator: !=";
	case MCC_AST_BINARY_OP_GE:
		return "Operator: >=";
	case MCC_AST_BINARY_OP_GT:
		return "Operator: >";
	case MCC_AST_BINARY_OP_LE:
		return "Operator: <=";
	case MCC_AST_BINARY_OP_LT:
		return "Operator: <";
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
		return "string";
	case MCC_AST_LITERAL_TYPE_INT:
		return "int";
	case MCC_AST_LITERAL_TYPE_FLOAT:
		return "float";
	case MCC_AST_LITERAL_TYPE_BOOL:
		return "bool";
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

static void print_dot_arrow(FILE *out, const void *node,const char*label)
{
    assert(out);
    assert(node);

    fprintf(out, "\t\"%p\" [shape=plaintext, label=\"%s\"];\n", node,label);
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

/* ------------------------------------------------------------- Literal */

static void print_dot_literal_int(struct mCc_ast_literal *literal, void *data)
{
	assert(literal);
	assert(data);

	FILE *out = data;
	char label[LABEL_SIZE] = {0};

	snprintf(label, sizeof(label), "ILiteral: %ld", literal->i_value);
	print_dot_node(out, literal, label);
}

static void print_dot_literal_float(struct mCc_ast_literal *literal, void *data)
{
	assert(literal);
	assert(data);

	FILE *out = data;
	char label[LABEL_SIZE] = {0};

	snprintf(label, sizeof(label), "FLiteral: %lf", literal->f_value);
	print_dot_node(out, literal, label);
}

static void print_dot_literal_bool(struct mCc_ast_literal *literal, void *data)
{
	assert(literal);
	assert(data);

	FILE *out = data;
	char label[LABEL_SIZE] = {0};

	snprintf(label, sizeof(label), "BLiteral: %d", literal->b_value);
	print_dot_node(out, literal, label);
}

static void print_dot_literal_string(struct mCc_ast_literal *literal,
				     void *data)
{
	assert(literal);
	assert(data);

	FILE *out = data;
	char label[LABEL_SIZE] = {0};
	snprintf(label, sizeof(label), "SLiteral: %s", literal->s_value);
	print_dot_node(out, literal, label);
}

/* ------------------------------------------------------------- Expressions */

static void print_dot_expression(struct mCc_ast_expression *expression,
				 void *data)
{
	assert(expression);
	assert(data);

	FILE *out = data;

	if (expression->type == MCC_AST_EXPRESSION_TYPE_SINGLE) {
		print_dot_node(out, expression,"Expression");
		print_dot_edge(out, expression, expression->single_expr, "");
	} else {
		print_dot_node(out, expression,
			       mCc_ast_print_binary_op(expression->op));
		print_dot_edge(out, expression, expression->lhs, "left");
		print_dot_edge(out, expression, expression->rhs, "right");
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
		print_dot_node(out, expression, "Literal_expression");
		print_dot_edge(out, expression, expression->literal, "");
		break;
	case (MCC_AST_SINGLE_EXPRESSION_TYPE_IDENTIFIER):
		snprintf(label, sizeof(label), "Identifier: %s",
			 expression->only_identifier->renamed);
		print_dot_node(out, expression, label);

		break;
	case (MCC_AST_SINGLE_EXPRESSION_TYPE_IDENTIFIER_EX):
		snprintf(label, sizeof(label), "Identifier: %s []",
			 expression->only_identifier->renamed);
		print_dot_node(out, expression, label);

		print_dot_edge(out, expression,
			       &expression->identifier_expression, "numerator");
		break;
	case (MCC_AST_SINGLE_EXPRESSION_TYPE_CALL_EXPR):
		print_dot_node(out, expression, "Call_expression");
		print_dot_edge(out, expression, expression->call_expr, "");
		break;
	case (MCC_AST_SINGLE_EXPRESSION_TYPE_UNARY_OP):
		print_dot_node(
			out, expression,
			mCc_ast_print_unary_op(expression->unary_operator));
		print_dot_edge(out, expression, expression->unary_expression,
			       "");
		break;
	case (MCC_AST_SINGLE_EXPRESSION_TYPE_PARENTH):
		print_dot_node(out, expression, "Parenth_expression");
		print_dot_edge(out, expression, expression->expression, "( )");
		break;
	}
}

static void print_dot_call_expr(struct mCc_ast_call_expr *expression,
				void *data)
{
	assert(expression);
	assert(data);

	FILE *out = data;

	char label[LABEL_SIZE] = {0};

	snprintf(label, sizeof(label), "%s( )",
		 expression->identifier->renamed);

	print_dot_node(out, expression, label);

	if (expression->arguments != NULL)
		for (int i = 0; i < expression->arguments->counter; i++) {
			snprintf(label, sizeof(label), "Argument %d", i);
			print_dot_edge(out, expression,
				       &expression->arguments->expression[i],
				       label);
		}
}

/* ----------------------------------------------------------- stmt */

static void print_dot_stmt_statement(struct mCc_ast_stmt *stmt, void *data)
{
	assert(stmt);
	assert(data);

    FILE *out = data;
    print_dot_node(out, stmt, "Statement");

    switch (stmt->type) {
		case (MCC_AST_IF_STMT):
            print_dot_edge(out, stmt,
				       stmt->if_stmt, "");
			break;
		case (MCC_AST_WHILE_STMT):
            print_dot_edge(out, stmt,
                           stmt->while_stmt, "");
			break;
		case (MCC_AST_RET_STMT):
            print_dot_edge(out, stmt,
                           stmt->ret_stmt, "");
			break;
		case (MCC_AST_DECL_STMT):
            print_dot_edge(out, stmt,
                           stmt->declaration, "");
			break;
		case (MCC_AST_ASS_STMT):
            print_dot_edge(out, stmt,
                           stmt->assignment, "");
			break;
		case (MCC_AST_EXPR_STMT):
            print_dot_edge(out, stmt,
                           stmt->expression, "");
			break;
		case (MCC_AST_COMPOUND_STMT):
            print_dot_edge(out, stmt,
                           stmt->compound_stmt, "");
			break;
		default:
			break;
		}
}

static void print_dot_stmt_if(struct mCc_ast_if_stmt *stmt, void *data)
{
	assert(stmt);
	assert(data);

	FILE *out = data;

	print_dot_node(out, stmt, "if");

	print_dot_edge(out, stmt, stmt->expression, "cond");


	print_dot_edge(out, stmt, stmt->statement, "true");

	if (stmt->else_statement != NULL) {
		print_dot_edge(out, stmt, stmt->else_statement, "false");
	}
}

static void print_dot_stmt_while(struct mCc_ast_while_stmt *stmt, void *data)
{
	assert(stmt);
	assert(data);

	FILE *out = data;

	print_dot_node(out, stmt, "while");

	print_dot_edge(out, stmt, stmt->expression, "cond");

	print_dot_edge(out, stmt, stmt->statement, "do");
}

static void print_dot_stmt_ret(struct mCc_ast_ret_stmt *stmt, void *data)
{
	assert(stmt);
	assert(data);

	FILE *out = data;
	print_dot_node(out, stmt, "return");

	if (stmt->expression == NULL)
		return;
	print_dot_edge(out, stmt, stmt->expression, "");
}

static void print_dot_stmt_ass(struct mCc_ast_assignment *stmt, void *data)
{
	assert(stmt);
	assert(data);

	FILE *out = data;
	print_dot_node(out, stmt, " = ");

	print_dot_edge(out, stmt, &stmt->identifier->renamed, "");

	if (stmt->numerator != NULL)
		print_dot_edge(out, stmt, stmt->numerator, "numerator");

	print_dot_edge(out, stmt, stmt->expression, "");


	char label[LABEL_SIZE] = {0};
	snprintf(label, sizeof(label), "%s", stmt->identifier->renamed);
	print_dot_node(out, &stmt->identifier->renamed, label);
}

static void print_dot_stmt_decl(struct mCc_ast_declaration *stmt, void *data)
{
	assert(stmt);
	assert(data);

	FILE *out = data;
	print_dot_node(out, stmt, "Declaration");

	char label[LABEL_SIZE] = {0};
	if (stmt->type == MCC_AST_DECLARATION_TYPE_SINGLE) {
		snprintf(label, sizeof(label), "%s %s;",
			 mCc_ast_print_literal_type(stmt->literal),
			 stmt->identifier->renamed);
		print_dot_edge(out, stmt, &stmt->identifier->renamed, "");
		print_dot_node(out, &stmt->identifier->renamed, label);
	} else {
		snprintf(label, sizeof(label), "%s [%d] %s;",
			 mCc_ast_print_literal_type(stmt->literal),
			 stmt->numerator, stmt->array_identifier->renamed);
		print_dot_edge(out, stmt, &stmt->array_identifier->renamed, "");
		print_dot_node(out, &stmt->array_identifier->renamed, label);
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
	print_dot_node(out, c_stmt, "{ }");

	for (int i = 0; i < c_stmt->counter; i++) {
        snprintf(label, sizeof(label), "%d", i);

        print_dot_edge(out,c_stmt,&c_stmt->statements[i],label);
	}
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
	snprintf(label, sizeof(label), "void %s( )", f->identifier->renamed);
	print_dot_node(out, f, label);
	print_dot_edge(out, f, f->c_stmt, "body");
    if(f->params!= NULL)
    for(int i=0;i<f->params->counter;i++) {
        snprintf(label, sizeof(label), "Parameter: %d",i);
        print_dot_edge(out, f, &f->params->declaration[i], label);
    }
}

static void print_dot_function_def_type(struct mCc_ast_function_def *f,
					void *data)
{
	assert(f);
	assert(data);

	FILE *out = data;

	char label[LABEL_SIZE] = {0};
    /* Type of the function */
	snprintf(label, sizeof(label), "%s %s( )",
		 mCc_ast_print_literal_type(f->l_type), f->identifier->renamed);
	print_dot_node(out, f, label);
    print_dot_edge(out, f, f->c_stmt, "body");
    if(f->params!= NULL)
        for(int i=0;i<f->params->counter;i++) {
        snprintf(label, sizeof(label), "Parameter: %d",i);
        print_dot_edge(out, f, &f->params->declaration[i], label);
    }
}

void mCc_ast_print_dot_function_def(FILE *out,
				    struct mCc_ast_function_def_array *f)
{
	assert(out);
	assert(f);
	ptr = malloc(1);
	counter = 0;
	print_dot_begin(out);


	struct mCc_ast_visitor visitor = print_dot_visitor(out);

	mCc_ast_visit_function_def_array(f, &visitor);

	print_dot_end(out);

	free(ptr);
}