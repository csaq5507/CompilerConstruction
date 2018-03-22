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
    print_dot_edge(out, f, f->literal, "type");
    print_dot_edge(out, f, f->identifier, "ident");
    print_dot_edge(out, f, f->params, "param");
    print_dot_edge(out, f, f->c_stmt, "compound_stmt");
}

/* ----------------------------------------------------------- stmt */

static void print_dot_stmt_if(struct mCc_ast_stmt *stmt, void *data) {
    assert(stmt);
    assert(data);

    FILE *out = data;

    print_dot_node(out, stmt, "if-stmt");
    print_dot_edge(out, stmt, stmt->if_stmt->expression, "expression");
    print_dot_edge(out, stmt, stmt->if_stmt->statement, "if stmt");
    print_dot_edge(out, stmt, stmt->if_stmt->else_statement, "else stmt");
}

static void print_dot_stmt_while(struct mCc_ast_stmt *stmt, void *data) {
    assert(stmt);
    assert(data);

    FILE *out = data;

    print_dot_node(out, stmt, "while-stmt");
    print_dot_edge(out, stmt, stmt->while_stmt->expression, "expression");
    print_dot_edge(out, stmt, stmt->while_stmt->statement, "while stmt");
}

static void print_dot_stmt_ret(struct mCc_ast_stmt *stmt, void *data) {
    assert(stmt);
    assert(data);

    FILE *out = data;

    print_dot_node(out, stmt, "return-stmt");
    print_dot_edge(out, stmt, stmt->ret_stmt->expression, "expression");
}

static void print_dot_stmt_compound(struct mCc_ast_stmt *stmt, void *data) {
    assert(stmt);
    assert(data);

    FILE *out = data;

    print_dot_node(out, stmt, "compound-stmt");
    print_dot_edge(out, stmt, stmt->compound_stmt->statements, "statements");
}

static void print_dot_stmt_ass(struct mCc_ast_stmt *stmt, void *data) {
    assert(stmt);
    assert(data);

    FILE *out = data;

    print_dot_node(out, stmt, "assignment-stmt");
    print_dot_edge(out, stmt, stmt->assignment->identifier, "identifier");
    print_dot_edge(out, stmt, stmt->assignment->numerator, "numerator");
    print_dot_edge(out, stmt, stmt->assignment->expression, "expression");
}

static void print_dot_stmt_expr(struct mCc_ast_stmt *stmt, void *data) {
    assert(stmt);
    assert(data);

    FILE *out = data;

    print_dot_node(out, stmt, "expression-stmt");
    print_dot_edge(out, stmt, stmt->expression, "expression");
}

static void print_dot_stmt_decl(struct mCc_ast_stmt *stmt, void *data) {
    assert(stmt);
    assert(data);

    FILE *out = data;

    print_dot_node(out, stmt, "decl-stmt");
    print_dot_edge(out, stmt, stmt->declaration, "declaration");
}



/* ----------------------------------------------------------- declaration */


static void print_dot_declaration_array(struct mCc_ast_declaration *decl, void *data) {
    assert(decl);
    assert(data);

    FILE *out = data;

    print_dot_node(out, decl, "array_declaration");
    print_dot_edge(out, decl, decl->literal, "literal");
    print_dot_edge(out, decl, decl->array_identifier, "identifier");
    print_dot_edge(out, decl, decl->numerator, "numerator");
}

static void print_dot_declaration_single(struct mCc_ast_declaration *decl, void *data) {
    assert(decl);
    assert(data);

    FILE *out = data;

    print_dot_node(out, decl, "declaration");
    print_dot_edge(out, decl, decl->literal, "literal");
    print_dot_edge(out, decl, decl->array_identifier, "identifier");
}

/* ----------------------------------------------------------- declaration */


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

        .stmt_ret = print_dot_stmt_ret,
        .stmt_if = print_dot_stmt_if,
        .stmt_while = print_dot_stmt_while,
        .stmt_decl = print_dot_stmt_decl,
        .stmt_ass = print_dot_stmt_ass,
        .stmt_expr = print_dot_stmt_expr,
        .stmt_compound = print_dot_stmt_compound,

        .declaration_array = print_dot_declaration_array,
        .declaration_single = print_dot_declaration_single,

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

void mCc_ast_print_dot_stmt(FILE *out, struct mCc_ast_stmt *stmt)
{
    assert(out);
    assert(stmt);

    print_dot_begin(out);
    struct  mCc_ast_visitor visitor = print_dot_visitor(out);
    mCc_ast_visit_stmt(stmt,&visitor);

    print_dot_end(out);
}


void mCc_ast_print_dot_declaration(FILE *out, struct mCc_ast_declaration *decl)
{
    assert(out);
    assert(decl);

    print_dot_begin(out);
    struct  mCc_ast_visitor visitor = print_dot_visitor(out);
    mCc_ast_visit_declaration(decl,&visitor);

    print_dot_end(out);
}
