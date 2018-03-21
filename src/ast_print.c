#include "mCc/ast_print.h"

#include <assert.h>

#include "mCc/ast_visit.h"

#define LABEL_SIZE 64

const char *mCc_ast_print_binary_op(enum mCc_ast_binary_op op)
{
	switch (op) {
        case MCC_AST_BINARY_OP_ADD: return "+";
        case MCC_AST_BINARY_OP_SUB: return "-";
        case MCC_AST_BINARY_OP_MUL: return "*";
        case MCC_AST_BINARY_OP_DIV: return "/";
        case MCC_AST_BINARY_OP_AND: return "&&";
        case MCC_AST_BINARY_OP_OR: return "||";
        case MCC_AST_BINARY_OP_EQ: return "==";
        case MCC_AST_BINARY_OP_NEQ: return "!=";
        case MCC_AST_BINARY_OP_GE: return ">=";
        case MCC_AST_BINARY_OP_GT: return ">";
        case MCC_AST_BINARY_OP_LE: return "<=";
        case MCC_AST_BINARY_OP_LT: return "<";
	}

	return "unknown op";
}

const char *mCc_ast_print_unary_op(enum mCc_ast_unary_op op)
{
    switch (op) {
        case MCC_AST_BINARY_OP_FAC: return "!";
        case MCC_AST_BINARY_OP_NEGATION: return "-";
    }

    return "unknown op";
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


/* ---------------------------------------------------------------- Literals */

static void print_dot_literal_int(struct mCc_ast_literal *literal, void *data)
{
	assert(literal);
	assert(data);

	char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), "%ld", literal->i_value);

	FILE *out = data;
	print_dot_node(out, literal, label);
}

static void print_dot_literal_float(struct mCc_ast_literal *literal, void *data)
{
	assert(literal);
	assert(data);

	char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), "%f", literal->f_value);

	FILE *out = data;
	print_dot_node(out, literal, label);
}

static void print_dot_literal_string(struct mCc_ast_literal *literal, void *data)
{
    assert(literal);
    assert(data);

    char label[LABEL_SIZE] = { 0 };
    snprintf(label, sizeof(label), "%s", literal->s_value);

    FILE *out = data;
    print_dot_node(out, literal, label);
}

static void print_dot_literal_bool(struct mCc_ast_literal *literal, void *data)
{
    assert(literal);
    assert(data);

    char label[LABEL_SIZE] = { 0 };
    snprintf(label, sizeof(label), "%s", literal->b_value == true ? "true" : "false");

    FILE *out = data;
    print_dot_node(out, literal, label);
}
/* ------------------------------------------------------------- Expressions */


static void print_dot_expression_single(struct mCc_ast_expression *expression,
                                        void *data)
{
    assert(expression);
    assert(data);

    FILE *out = data;
    print_dot_node(out, expression, "expr: single");
    print_dot_edge(out, expression, expression->single_expr, "single");
}

static void
print_dot_expression_binary(struct mCc_ast_expression *expression,
                            void *data) {
    assert(expression);
    assert(data);

    char label[LABEL_SIZE] = {0};
    snprintf(label, sizeof(label), "expr: %s",
             mCc_ast_print_binary_op(expression->op));

    FILE *out = data;
    print_dot_node(out, expression, label);
    print_dot_edge(out, expression, expression->lhs, "lhs");
    print_dot_edge(out, expression, expression->rhs, "rhs");
}

/* --single*/

static void print_dot_single_expression_literal(struct mCc_ast_single_expression *single_expression, void *data)
{
	assert(single_expression);
	assert(data);

	FILE *out = data;
	print_dot_node(out, single_expression, "single_expr: lit");
	print_dot_edge(out, single_expression, single_expression->literal, "literal");
}

static void print_dot_single_expression_identifier(struct mCc_ast_single_expression *single_expression, void *data)
{
	assert(single_expression);
	assert(data);

	FILE *out = data;
	print_dot_node(out, single_expression, "single_expr: ident");
	print_dot_edge(out, single_expression, single_expression->identifier, "identifier");
	print_dot_edge(out, single_expression, single_expression->identifier_expression, "identifier expression");
}

static void print_dot_single_expression_call(struct mCc_ast_single_expression *single_expression, void *data)
{
	assert(single_expression);
	assert(data);

	FILE *out = data;
	print_dot_node(out, single_expression, "single_expr: call expr");
	print_dot_edge(out, single_expression, single_expression->call_expr, "call_expr");
}

static void print_dot_single_expression_unary_op(struct mCc_ast_single_expression *single_expression, void *data)
{
	assert(single_expression);
	assert(data);


	char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), "expr: %s",
			 mCc_ast_print_binary_op(single_expression->unary_operator));

	FILE *out = data;
	print_dot_node(out, single_expression, label);
	print_dot_edge(out, single_expression, single_expression->unary_expression, "unary_expr");
}

static void print_dot_single_expression_parenth(struct mCc_ast_single_expression *single_expression, void *data)
{
	assert(single_expression);
	assert(data);

	FILE *out = data;

	print_dot_node(out, single_expression, "( )");
	print_dot_edge(out, single_expression, single_expression->expression, "expression");
}

/* ----------------------------------------------------------- Function Def / CALL */

static void print_dot_function_def_void(struct mCc_ast_function_def *f, void *data) {
    assert(f);
    assert(data);

    FILE *out = data;

    print_dot_node(out, f, "func_def");
    print_dot_edge(out, f, f->void_value, "void");
    print_dot_edge(out, f, f->identifier, "ident");
    print_dot_edge(out, f, f->params, "param");
    print_dot_edge(out, f, f->c_stmt, "compound_stmt");
}

static void print_dot_function_def_type(struct mCc_ast_function_def *f, void *data) {
    assert(f);
    assert(data);

    FILE *out = data;

    print_dot_node(out, f, "func_def");
    print_dot_edge(out, f, f->type, "type");
    print_dot_edge(out, f, f->identifier, "ident");
    print_dot_edge(out, f, f->params, "param");
    print_dot_edge(out, f, f->c_stmt, "compound_stmt");
}

static struct mCc_ast_visitor print_dot_visitor(FILE *out)
{
	assert(out);

	return (struct mCc_ast_visitor){
		.traversal = MCC_AST_VISIT_DEPTH_FIRST,
		.order = MCC_AST_VISIT_PRE_ORDER,

		.userdata = out,

		.expression_single = print_dot_expression_single,
		.expression_binary = print_dot_expression_binary,

		.literal_int = print_dot_literal_int,
		.literal_float = print_dot_literal_float,
		.literal_string = print_dot_literal_string,
		.literal_bool = print_dot_literal_bool,


        .single_expression_literal = print_dot_single_expression_literal,
        .single_expression_identifier = print_dot_single_expression_identifier,
        .single_expression_call = print_dot_single_expression_call,
        .single_expression_unary_op = print_dot_single_expression_unary_op,
        .single_expression_parenth = print_dot_single_expression_parenth,


        .function_def_void = print_dot_function_def_void,
        .function_def_type = print_dot_function_def_type,
	};
}

void mCc_ast_print_dot_expression(FILE *out,
                                  struct mCc_ast_expression *expression)
{
	assert(out);
	assert(expression);

	print_dot_begin(out);

	struct mCc_ast_visitor visitor = print_dot_visitor(out);
	mCc_ast_visit_expression(expression, &visitor);

	print_dot_end(out);
}

void mCc_ast_print_dot_literal(FILE *out, struct mCc_ast_literal *literal)
{
	assert(out);
	assert(literal);

	print_dot_begin(out);

	struct mCc_ast_visitor visitor = print_dot_visitor(out);
	mCc_ast_visit_literal(literal, &visitor);

	print_dot_end(out);
}

void mCc_ast_print_dot_single_expression(FILE *out, struct mCc_ast_single_expression * single_expression)
{
	assert(out);
	assert(single_expression);

	print_dot_begin(out);
	struct mCc_ast_visitor visitor = print_dot_visitor(out);
	mCc_ast_visit_single_expression(single_expression,&visitor);

	print_dot_end(out);
}

void mCc_ast_print_dot_function_def(FILE *out, struct mCc_ast_function_def *f)
{
    assert(out);
    assert(f);

    print_dot_begin(out);
    struct  mCc_ast_visitor visitor = print_dot_visitor(out);
    mCc_ast_visit_function_def(f,&visitor);

    print_dot_end(out);
}