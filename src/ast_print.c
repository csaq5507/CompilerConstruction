#include "mCc/ast_print.h"

#include <assert.h>

#include "mCc/ast_visit.h"
#include "../include/mCc/ast.h"

#define LABEL_SIZE 64

static void print_dot_stmt_if(struct mCc_ast_if_stmt *stmt, void *data);
static void print_dot_stmt_while(struct mCc_ast_while_stmt *stmt, void *data);
static void print_dot_stmt_ret(struct mCc_ast_ret_stmt *stmt, void *data);
static void print_dot_stmt_compound(struct mCc_ast_compound_stmt *stmt, void *data);
static void print_dot_stmt_ass(struct mCc_ast_assignment *stmt, void *data);
static void print_dot_stmt_expr(struct mCc_ast_expression *stmt, void *data);
static void print_dot_stmt_decl(struct mCc_ast_declaration *stmt, void *data);

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


/* ---------------------------------------------------------------- Literals */

static void print_dot_literal_int(struct mCc_ast_literal *literal, void *data){
	assert(literal);
	assert(data);


	//FILE *out = data;

}

static void print_dot_literal_float(struct mCc_ast_literal *literal, void *data){
	assert(literal);
	assert(data);


	//FILE *out = data;

}

static void print_dot_literal_string(struct mCc_ast_literal *literal, void *data){
    assert(literal);
    assert(data);


   // FILE *out = data;

}

static void print_dot_literal_bool(struct mCc_ast_literal *literal, void *data){
    assert(literal);
    assert(data);


   // FILE *out = data;

}
/* ------------------------------------------------------------- Expressions */


static void print_dot_expression_single(struct mCc_ast_expression *expression, void *data){
    assert(expression);
    assert(data);

    FILE *out = data;

    char label[LABEL_SIZE] = { 0 };

    if (expression->lhs != NULL) {
        if (expression->lhs->literal != NULL) {
            snprintf(label, sizeof(label), "Literal : %s", expression->lhs->literal);
            print_dot_node(out, expression->lhs->literal, label);
            print_dot_edge(out, expression, expression->lhs->literal, "");
        }
        if (expression->lhs->identifier != NULL) {
            snprintf(label, sizeof(label), "Literal : %s", expression->lhs->identifier);
            print_dot_node(out, expression->lhs->identifier, label);
            print_dot_edge(out, expression, expression->lhs->identifier, "");
        }
    }
    if (expression->rhs != NULL) {
        print_dot_expression_single(expression->rhs, data);
    }
}

static void print_dot_expression_binary(struct mCc_ast_expression *expression, void *data) {
    assert(expression);
    assert(data);

    FILE *out = data;

    char label[LABEL_SIZE] = { 0 };

    if (expression->lhs != NULL) {
        if (expression->lhs->literal != NULL) {
            snprintf(label, sizeof(label), "Literal : %s", expression->lhs->literal);
            print_dot_node(out, expression->lhs->literal, label);
            print_dot_edge(out, expression, expression->lhs->literal, "");
        }
    }
    if (expression->rhs != NULL) {
        print_dot_expression_single(expression->rhs, data);
    }

}

/* --single*/

static void print_dot_single_expression_literal(struct mCc_ast_single_expression *single_expression, void *data) {
	assert(single_expression);
	assert(data);

	//FILE *out = data;

}

static void print_dot_single_expression_identifier(struct mCc_ast_single_expression *single_expression, void *data){
	assert(single_expression);
	assert(data);

	//FILE *out = data;

}

static void print_dot_single_expression_identifier_ex(struct mCc_ast_single_expression *single_expression, void *data){
    assert(single_expression);
    assert(data);

   // FILE *out = data;

}

static void print_dot_single_expression_call(struct mCc_ast_single_expression *single_expression, void *data){
	assert(single_expression);
	assert(data);

	//FILE *out = data;

}

static void print_dot_single_expression_unary_op(struct mCc_ast_single_expression *single_expression, void *data){
	assert(single_expression);
	assert(data);

	//FILE *out = data;

}

static void print_dot_single_expression_parenth(struct mCc_ast_single_expression *single_expression, void *data){
	assert(single_expression);
	assert(data);

	//FILE *out = data;

}

/* ----------------------------------------------------------- parameter */

static void print_dot_parameter(struct mCc_ast_parameter *param, void *data) {
    assert(param);
    assert(data);
    printf("print_dot_parameter\n");

    //FILE *out = data;

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
            print_dot_stmt_expr(stmt->expression, data);
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

    //FILE *out = data;
}

static void print_dot_stmt_compound(struct mCc_ast_compound_stmt *stmt, void *data) {
    assert(stmt);
    assert(data);

    //FILE *out = data;

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
}

static void print_dot_stmt_expr(struct mCc_ast_expression *stmt, void *data) {
    assert(stmt);
    assert(data);

    //FILE *out = data;

}

static void print_dot_stmt_decl(struct mCc_ast_declaration *stmt, void *data) {
    assert(stmt);
    assert(data);

   // FILE *out = data;

}

/* ----------------------------------------------------------- compound stmt */
static void print_dot_compound_stmt(struct mCc_ast_compound_stmt * c_stmt, void *data){
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
                print_dot_stmt_expr(c_stmt->statements[i].expression, data);
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
    snprintf(label, sizeof(label), "type: VOID");


    snprintf(label, sizeof(label), "identifier: %s", f->identifier);

    //print_dot_node(out, &f->node, label);
    //print_dot_edge(out, f, &f->node, "");

    /* Identifier */
    snprintf(label, sizeof(label), "identifier: %s", f->identifier);
    print_dot_node(out, f, label);
    print_dot_edge(out, f->identifier, f, "");

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
    switch(f->l_type) {
        case (MCC_AST_LITERAL_TYPE_INT):
            snprintf(label, sizeof(label), "type: INT");
            break;
        case (MCC_AST_LITERAL_TYPE_STRING):
            snprintf(label, sizeof(label), "type: STRING");
            break;
        case (MCC_AST_LITERAL_TYPE_BOOL):
            snprintf(label, sizeof(label), "type: BOOL");
            break;
        case (MCC_AST_LITERAL_TYPE_FLOAT):
            snprintf(label, sizeof(label), "type: FLOAT");
            break;
        default:
            snprintf(label, sizeof(label), "type: Unknwon");
            break;
    }

    //print_dot_node(out, f->type, label);
    //print_dot_edge(out, f->identifier, f->type, "");

    /* Identifier */
    snprintf(label, sizeof(label), "identifier: %s", f->identifier);
    print_dot_node(out, f, label);
    print_dot_edge(out, f->identifier, f, "");

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

/* ----------------------------------------------------------- declaration */

static void print_dot_declaration_array(struct mCc_ast_declaration *decl, void *data) {
    assert(decl);
    assert(data);

  //  FILE *out = data;

}

static void print_dot_declaration_single(struct mCc_ast_declaration *decl, void *data) {
    assert(decl);
    assert(data);

   // FILE *out = data;


}

static void print_dot_argument(struct mCc_ast_argument *arg, void *data) {
    assert(arg);
    assert(data);

   // FILE *out = data;


}


static struct mCc_ast_visitor print_dot_visitor(FILE *out){
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
            .single_expression_identifier_ex = print_dot_single_expression_identifier_ex,
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

            .parameter = print_dot_parameter,

            .compound = print_dot_compound_stmt,

            .argument = print_dot_argument,
    };
}
void mCc_ast_print_dot_expression(FILE *out, struct mCc_ast_expression *expression){
	assert(out);
	assert(expression);

}

void mCc_ast_print_dot_literal(FILE *out, struct mCc_ast_literal *literal){
	assert(out);
	assert(literal);

}

void mCc_ast_print_dot_single_expression(FILE *out, struct mCc_ast_single_expression * single_expression){
	assert(out);
	assert(single_expression);

}

void mCc_ast_print_dot_function_def(FILE *out, struct mCc_ast_function_def_array *f){
    assert(out);
    assert(f);
    printf("mCc_ast_print_dot_function_def\n");

    print_dot_begin(out);

    char label[LABEL_SIZE] = { 0 };
    print_dot_node(out, f, "Toplevel");



	struct  mCc_ast_visitor visitor = print_dot_visitor(out);
	for(int i=0;i<f->counter;i++) {
        snprintf(label, sizeof(label), "func_def: %d", i);
        char* name = f->function_def[i].identifier;
        print_dot_node(out, name, label);
        print_dot_edge(out, f, name, "");
        struct mCc_ast_function_def func = f->function_def[i];
        mCc_ast_visit_function_def(&func,&visitor);
    }
    print_dot_end(out);

}

void mCc_ast_print_dot_parameter(FILE *out, struct mCc_ast_parameter *param){
    assert(out);
    assert(param);
    printf("mCc_ast_print_dot_parameter\n");


}

void mCc_ast_print_dot_compound_stmt(FILE *out, struct mCc_ast_compound_stmt *c_stmt){
    assert(out);
    assert(c_stmt);
    printf("mCc_ast_print_dot_compound_stmt\n");


}

void mCc_ast_print_dot_stmt(FILE *out, struct mCc_ast_stmt *stmt){
    assert(out);
    assert(stmt);
    printf("mCc_ast_print_dot_stmt\n");


}

void mCc_ast_print_dot_declaration(FILE *out, struct mCc_ast_declaration *decl){
    assert(out);
    assert(decl);
    printf("mCc_ast_print_dot_declaration\n");


}