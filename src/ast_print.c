#include "mCc/ast_print.h"

#include <assert.h>

#include "../include/mCc/ast.h"

#define LABEL_SIZE 64


const char *mCc_ast_print_binary_op(enum mCc_ast_binary_op op){
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

const char *mCc_ast_print_unary_op(enum mCc_ast_unary_op op){
    switch (op) {
        case MCC_AST_UNARY_OP_FAC: return "!";
        case MCC_AST_UNARY_OP_NEGATION: return "-";
    }

    return "unknown op";
}

const char *mCc_ast_print_literal_type(enum mCc_ast_literal_type type){
    switch (type) {
        case MCC_AST_LITERAL_TYPE_STRING: return "string";
        case MCC_AST_LITERAL_TYPE_INT: return "int";
        case MCC_AST_LITERAL_TYPE_FLOAT: return "float";
        case MCC_AST_LITERAL_TYPE_BOOL: return "bool";
    }

    return "unknown type";
}
/* ------------------------------------------------------------- DOT Printer */

static void print_dot_begin(FILE *out){
	assert(out);

	fprintf(out, "digraph \"AST\" {\n");
	fprintf(out, "\tnodesep=0.6\n");
}

static void print_dot_end(FILE *out){
	assert(out);

	fprintf(out, "}\n");
}

static void print_dot_node(FILE *out, const void *node, const char *label){
	assert(out);
	assert(node);
	assert(label);

	fprintf(out, "\t\"%p\" [shape=box, label=\"%s\"];\n", node, label);
}

static void print_dot_edge(FILE *out, const void *src_node, const void *dst_node, const char *label){
	assert(out);
	assert(src_node);
	assert(dst_node);
	assert(label);

	fprintf(out, "\t\"%p\" -> \"%p\" [label=\"%s\"];\n", src_node, dst_node,
	        label);
}

/* ------------------------------------------------------------- Expressions */



static void print_dot_expression(struct mCc_ast_expression *expression, void *data) {
    assert(expression);
    assert(data);

    FILE *out = data;
    char label[LABEL_SIZE] = { 0 };

    switch (expression->type) {
        case (MCC_AST_EXPRESSION_TYPE_SINGLE):
            snprintf(label, sizeof(label), "Single expression");
            print_dot_node(out, expression->single_expr, label);
            print_dot_edge(out, expression, expression->single_expr, "");
            print_dot_expression_single(expression->single_expr, data );
            break;
        case (MCC_AST_EXPRESSION_TYPE_BINARY):
            print_dot_expression_binary(expression, data );
            break;
        default:
            break;
    }
}

static void print_dot_expression_single(struct mCc_ast_single_expression *expression, void *data){
    assert(expression);
    assert(data);

    FILE *out = data;

    char label[LABEL_SIZE] = { 0 };


    switch (expression->type) {
        case (MCC_AST_SINGLE_EXPRESSION_TYPE_LITERAL):
            snprintf(label, sizeof(label), "Literal: %s", expression->literal);
            print_dot_node(out, expression->literal, label);
            print_dot_edge(out, expression, expression->literal, "");
            break;
        case(MCC_AST_SINGLE_EXPRESSION_TYPE_IDENTIFIER):
            snprintf(label, sizeof(label), "Identifier: %s", expression->only_identifier);
            print_dot_node(out, expression->only_identifier, label);
            print_dot_edge(out, expression, expression->only_identifier, "");
            break;
        case(MCC_AST_SINGLE_EXPRESSION_TYPE_IDENTIFIER_EX):
            snprintf(label, sizeof(label), "Identifier expression:");
            print_dot_node(out, expression->identifier_expression, label);
            print_dot_edge(out, expression, expression->identifier_expression, "");
            print_dot_expression(expression->identifier_expression, data);
            break;
        case(MCC_AST_SINGLE_EXPRESSION_TYPE_CALL_EXPR):
            snprintf(label, sizeof(label), "Call expression:");
            print_dot_node(out, expression->call_expr, label);
            print_dot_edge(out, expression, expression->call_expr, "");
            print_dot_call_expr(expression->call_expr, data);
            break;
        case(MCC_AST_SINGLE_EXPRESSION_TYPE_UNARY_OP):
            snprintf(label, sizeof(label), "Operator: %s", expression->unary_operator);
            print_dot_node(out, &expression->unary_operator, label);
            print_dot_edge(out, expression, &expression->unary_operator, "");
            snprintf(label, sizeof(label), "Unary expression:");
            print_dot_node(out, expression->unary_expression, label);
            print_dot_edge(out, expression, expression->unary_expression, "");
            print_dot_expression(expression->unary_expression, data);
            break;
        case(MCC_AST_SINGLE_EXPRESSION_TYPE_PARENTH):
            snprintf(label, sizeof(label), "Parenth expression:");
            print_dot_node(out, expression->expression, label);
            print_dot_edge(out, expression, expression->expression, "");
            print_dot_expression(expression->expression, data);
            break;
        default:
            break;
    }
}

static void print_dot_expression_binary(struct mCc_ast_expression *expression, void *data) {
    assert(expression);
    assert(data);

    FILE *out = data;

    char label[LABEL_SIZE] = { 0 };

    snprintf(label, sizeof(label), "Left hand side:");
    print_dot_node(out, expression->lhs, label);
    print_dot_edge(out, expression, expression->lhs, "");
    print_dot_expression_single(expression->lhs, data);

    char * text;
    switch (expression->op) {
        case (MCC_AST_BINARY_OP_ADD):
            text = "+";
            break;
        case (MCC_AST_BINARY_OP_SUB):
            text = "-";
            break;
        case (MCC_AST_BINARY_OP_MUL):
            text = "*";
            break;
        case (MCC_AST_BINARY_OP_DIV):
            text = "/";
            break;
        case (MCC_AST_BINARY_OP_LT):
            text = "<";
            break;
        case (MCC_AST_BINARY_OP_GT):
            text = ">";
            break;
        case (MCC_AST_BINARY_OP_LE):
            text = "<=";
            break;
        case (MCC_AST_BINARY_OP_GE):
            text = ">=";
            break;
        case (MCC_AST_BINARY_OP_AND):
            text = "&&";
            break;
        case (MCC_AST_BINARY_OP_OR):
            text = "||";
            break;
        case (MCC_AST_BINARY_OP_EQ):
            text = "==";
            break;
        case (MCC_AST_BINARY_OP_NEQ):
            text = "!=";
            break;
    }

    snprintf(label, sizeof(label), "Operator: %s", text);
    print_dot_node(out, text, label);
    print_dot_edge(out, expression, text, "");



    snprintf(label, sizeof(label), "Rigth hand side");
    print_dot_node(out, expression->rhs, label);
    print_dot_edge(out, expression, expression->rhs, "");
    print_dot_expression(expression->rhs, data);
}

static void print_dot_call_expr(struct mCc_ast_call_expr *expression, void *data) {
    assert(expression);
    assert(data);

    FILE *out = data;

    char label[LABEL_SIZE] = { 0 };

    snprintf(label, sizeof(label), "Operator: %s", expression->identifier);
    print_dot_node(out, expression->identifier, label);
    print_dot_edge(out, expression, expression->identifier, "");

    for(int i = 0; i < expression->arguments->counter; i++) {
        snprintf(label, sizeof(label), "Arguments");
        print_dot_node(out, &expression->arguments[i].expression, label);
        print_dot_edge(out, expression, &expression->arguments[i].expression, "");
        print_dot_expression(&expression->arguments[i].expression, data);
    }
}

/* ----------------------------------------------------------- parameter */

static void print_dot_parameter(struct mCc_ast_parameter *param, void *data) {
    assert(param);
    assert(data);

    FILE *out = data;

    char label[LABEL_SIZE] = { 0 };

    //FILE *out = data;
    for (int i = 0; i < param->counter; i++) {
        snprintf(label, sizeof(label), "Parameter");
        print_dot_node(out, param->declaration, label);
        print_dot_edge(out, param, param->declaration, "");
        print_dot_stmt_decl(param->declaration, data);
    }

}

/* ----------------------------------------------------------- stmt */

static void print_dot_stmt_statement(struct mCc_ast_stmt *stmt, void *data) {
    assert(stmt);
    assert(data);

    FILE *out = data;

    char label[LABEL_SIZE] = { 0 };


    switch(stmt->type) {
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
            // TODO ELSE STATEMENT
            break;
    }
}

static void print_dot_stmt_if(struct mCc_ast_if_stmt *stmt, void *data) {
    assert(stmt);
    assert(data);

    FILE *out = data;

    char label[LABEL_SIZE] = { 0 };

    snprintf(label, sizeof(label), "Expression");

    print_dot_node(out, stmt->expression, label);
    print_dot_edge(out, stmt, stmt->expression, "");
    print_dot_expression(stmt->expression, data);

    snprintf(label, sizeof(label), "Statement");

    print_dot_node(out, stmt->statement, label);
    print_dot_edge(out, stmt, stmt->statement, "");
    print_dot_stmt_statement(stmt->statement, data);

    snprintf(label, sizeof(label), "Statement");

    print_dot_node(out, stmt->else_statement, label);
    print_dot_edge(out, stmt, stmt->else_statement, "");
    print_dot_stmt_statement(stmt->else_statement, data);

}

static void print_dot_stmt_while(struct mCc_ast_while_stmt *stmt, void *data) {
    assert(stmt);
    assert(data);

    FILE *out = data;

    char label[LABEL_SIZE] = { 0 };

    snprintf(label, sizeof(label), "Expression");

    print_dot_node(out, stmt->expression, label);
    print_dot_edge(out, stmt, stmt->expression, "");
    switch (stmt->expression->type) {
        case (MCC_AST_EXPRESSION_TYPE_SINGLE):
            print_dot_expression_single(stmt->expression, data );
            break;
        case (MCC_AST_EXPRESSION_TYPE_BINARY):
            print_dot_expression_binary(stmt->expression, data );
            break;
        default:
            break;
    }

    snprintf(label, sizeof(label), "Statement");

    print_dot_node(out, stmt->statement, label);
    print_dot_edge(out, stmt, stmt->statement, "");
    print_dot_stmt_statement(stmt->statement, data);
}

static void print_dot_stmt_ret(struct mCc_ast_ret_stmt *stmt, void *data) {
    assert(stmt);
    assert(data);

    FILE *out = data;

    char label[LABEL_SIZE] = { 0 };

    snprintf(label, sizeof(label), "Expression");
    print_dot_node(out, stmt->expression, label);
    print_dot_edge(out, stmt, stmt->expression, "");
    print_dot_expression(stmt->expression, data);
}

static void print_dot_stmt_compound(struct mCc_ast_compound_stmt *stmt, void *data) {
    assert(stmt);
    assert(data);

    FILE *out = data;
    char label[LABEL_SIZE] = { 0 };

    for (int i = 0; i < stmt->counter; i++) {
        snprintf(label, sizeof(label), "Statment");
        print_dot_node(out, &stmt->statements[i], label);
        print_dot_edge(out, stmt, &stmt->statements[i], "");
        print_dot_stmt_statement(&stmt->statements[i], data);
    }

}

static void print_dot_stmt_ass(struct mCc_ast_assignment *stmt, void *data) {
    assert(stmt);
    assert(data);

    FILE *out = data;

    char label[LABEL_SIZE] = { 0 };

    snprintf(label, sizeof(label), "Identifier: %s", stmt->identifier);
    print_dot_node(out, stmt->identifier, label);
    print_dot_edge(out, stmt, stmt->identifier, "");

    snprintf(label, sizeof(label), "Expression");
    print_dot_node(out, stmt->expression, label);
    print_dot_edge(out, stmt, stmt->expression, "");
    print_dot_expression(stmt->expression, data );

}

static void print_dot_stmt_decl(struct mCc_ast_declaration *stmt, void *data) {
    assert(stmt);
    assert(data);

   // FILE *out = data;

}

/* ----------------------------------------------------------- compound stmt */
static void print_dot_compound_stmt(struct mCc_ast_compound_stmt * c_stmt, void *data) {
    assert(c_stmt);
    assert(data);

    printf("print_dot_compound_stmt\n");
    FILE *out = data;

    char label[LABEL_SIZE] = { 0 };

    for(int i=0;i<c_stmt->counter;i++) {
        /* TYPE */
        switch(c_stmt->statements[i].type) {
            case (MCC_AST_IF_STMT):
                snprintf(label, sizeof(label), "If_Stmt");
                print_dot_node(out, c_stmt->statements[i].if_stmt, label);
                print_dot_edge(out, c_stmt, c_stmt->statements[i].if_stmt, "");
                print_dot_stmt_if(c_stmt->statements[i].if_stmt, data);
                break;
            case (MCC_AST_WHILE_STMT):
                snprintf(label, sizeof(label), "While_Stmt");
                print_dot_node(out, c_stmt->statements[i].while_stmt, label);
                print_dot_edge(out, c_stmt, c_stmt->statements[i].while_stmt, "");
                print_dot_stmt_while(c_stmt->statements[i].while_stmt, data);
                break;
            case (MCC_AST_RET_STMT):
                snprintf(label, sizeof(label), "Ret_Stmt");
                print_dot_node(out, c_stmt->statements[i].ret_stmt, label);
                print_dot_edge(out, c_stmt, c_stmt->statements[i].ret_stmt, "");
                print_dot_stmt_ret(c_stmt->statements[i].ret_stmt, data);
                break;
            case (MCC_AST_DECL_STMT):
                snprintf(label, sizeof(label), "DECL_Stmt");
                print_dot_node(out, c_stmt->statements[i].declaration, label);
                print_dot_edge(out, c_stmt, c_stmt->statements[i].declaration, "");
                print_dot_stmt_decl(c_stmt->statements[i].declaration, data);
                break;
            case (MCC_AST_ASS_STMT):
                snprintf(label, sizeof(label), "ASSIGNEMNT_Stmt");
                print_dot_node(out, c_stmt->statements[i].assignment, label);
                print_dot_edge(out, c_stmt, c_stmt->statements[i].assignment, "");
                print_dot_stmt_ass(c_stmt->statements[i].assignment, data);
                break;
            case (MCC_AST_EXPR_STMT):
                snprintf(label, sizeof(label), "EXPRESSION_Stmt");
                print_dot_node(out, c_stmt->statements[i].expression, label);
                print_dot_edge(out, c_stmt, c_stmt->statements[i].expression, "");
                print_dot_expression(c_stmt->statements[i].expression, data);
                break;
            case (MCC_AST_COMPOUND_STMT):
                snprintf(label, sizeof(label), "COMPOUND_Stmt");
                print_dot_node(out, c_stmt->statements[i].compound_stmt, label);
                print_dot_edge(out, c_stmt, c_stmt->statements[i].compound_stmt, "");
                print_dot_stmt_compound(c_stmt->statements[i].compound_stmt, data);
                break;
            default:
                break;
        }
    }



}

/* ----------------------------------------------------------- Function Def / CALL */

static void print_dot_function_def_void(struct mCc_ast_function_def *f, void *data) {
    assert(f);
    assert(data);

    FILE *out = data;

    char label[LABEL_SIZE] = { 0 };

    /* TYPE */
    void *help1 ;
    snprintf(label, sizeof(label), "type: VOID");
    print_dot_node(out, &help1, label);
    print_dot_edge(out, f->identifier, &help1, "");

    /* Identifier */
    void *help2 = f->identifier;
    snprintf(label, sizeof(label), "identifier: %s", f->identifier);
    print_dot_node(out, &help2, label);
    print_dot_edge(out, f->identifier, &help2, "");

    /* Parameter */
    if (f->params->counter > 0){
        print_dot_node(out, f->params, "Params");
        print_dot_edge(out, f->identifier, f->params, "");
    }

    for(int i=0;i<f->params->counter;i++) {
        snprintf(label, sizeof(label), "params: %s", f->params->declaration[i].identifier);
        print_dot_node(out, &f->params->declaration[i], label);
        print_dot_edge(out, f->params, &f->params->declaration[i], "");
        print_dot_parameter(&f->params->declaration[i], data);
    }

    /* Statements */
    print_dot_node(out, f->c_stmt, "Statements");
    print_dot_edge(out, f->identifier, f->c_stmt, "");

    for(int i=0;i<f->c_stmt->counter;i++) {
        snprintf(label, sizeof(label), "statement: %s", f->c_stmt->statements[i]);
        print_dot_edge(out, f->c_stmt, &f->c_stmt->statements[i], "");
        print_dot_compound_stmt(f->c_stmt->statements, data);
    }
}

static void print_dot_function_def_type(struct mCc_ast_function_def *f, void *data) {
    assert(f);
    assert(data);

    FILE *out = data;

    char label[LABEL_SIZE] = { 0 };

    /* TYPE */

    char *help1;
    switch(f->l_type) {
        case (MCC_AST_LITERAL_TYPE_INT):
            help1 = "int";
            snprintf(label, sizeof(label), "type: INT");
            break;
        case (MCC_AST_LITERAL_TYPE_STRING):
            help1 = "string";
            snprintf(label, sizeof(label), "type: STRING");
            break;
        case (MCC_AST_LITERAL_TYPE_BOOL):
            help1 = "bool";
            snprintf(label, sizeof(label), "type: BOOL");
            break;
        case (MCC_AST_LITERAL_TYPE_FLOAT):
            help1 = "float";
            snprintf(label, sizeof(label), "type: FLOAT");
            break;
        default:
            help1 = "unknwon";
            snprintf(label, sizeof(label), "type: Unknwon");
            break;
    }

    print_dot_node(out, help1, label);
    print_dot_edge(out, f->identifier, help1, "");

    /* Identifier */
    char *help2 = f->identifier;
    snprintf(label, sizeof(label), "identifier: %s", f->identifier);
    print_dot_node(out, &help2, label);
    print_dot_edge(out, f->identifier, &help2, "");

    /* Parameter */
    if (f->params->counter > 0){
        print_dot_node(out, f->params, "Params");
        print_dot_edge(out, f->identifier, f->params, "");
    }

    for(int i=0;i<f->params->counter;i++) {
        snprintf(label, sizeof(label), "params: %s", f->params->declaration[i].identifier);
        print_dot_node(out, &f->params->declaration[i], label);
        print_dot_edge(out, f->params, &f->params->declaration[i], "");
        print_dot_parameter(&f->params->declaration[i], data);
    }

    /* Statements */
    print_dot_node(out, f->c_stmt, "Statements");
    print_dot_edge(out, f->identifier, f->c_stmt, "");
    print_dot_compound_stmt(f->c_stmt, data);


}

void mCc_ast_print_dot_function_def(FILE *out, struct mCc_ast_function_def_array *f){
    assert(out);
    assert(f);
    printf("mCc_ast_print_dot_function_def\n");

    print_dot_begin(out);

    char label[LABEL_SIZE] = { 0 };
    print_dot_node(out, f, "Toplevel");



	for(int i=0;i<f->counter;i++) {
        snprintf(label, sizeof(label), "func_def: %d", i);
        char* name = f->function_def[i].identifier;
        print_dot_node(out, name, label);
        print_dot_edge(out, f, name, "");
        struct mCc_ast_function_def func = f->function_def[i];
        switch (func.type) {
            case (MCC_AST_FUNCTION_DEF_TYPE_TYPE):
                print_dot_function_def_type(&func, out);
                break;
            case (MCC_AST_FUNCTION_DEF_TYPE_VOID):
                print_dot_function_def_void(&func, out);
                break;
        }
    }
    print_dot_end(out);

}