
#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include "mCc/ast_semantic_checks.h"

static void ast_semantic_check_single_expression(struct mCc_ast_single_expression *expression,
                                    void *data);
static void ast_semantic_check_expression(struct mCc_ast_expression *expression,
                                                 void *data);

static void ast_semantic_check_call_expression(struct mCc_ast_call_expr *expression,
                                               void *data);

static void ast_semantic_check_decl_stmt(struct mCc_ast_declaration *stmt,
                                                 void *data);
static void ast_semantic_check_ret_stmt(struct mCc_ast_ret_stmt *stmt,
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

            .decl_stmt = ast_semantic_check_decl_stmt,
            .ret_stmt = ast_semantic_check_ret_stmt,

    };
};

static void ast_semantic_check_single_expression(struct mCc_ast_single_expression *expression,
                                                 void *data) {
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
            expression->d_type = expression->identifier->d_type;
            break;
        case (MCC_AST_SINGLE_EXPRESSION_TYPE_CALL_EXPR):
            expression->d_type = expression->call_expr->d_type;
            break;
        case (MCC_AST_SINGLE_EXPRESSION_TYPE_UNARY_OP):
            //TODO check if type compatible with operator (string not compatible with -)
            expression->d_type = expression->unary_expression->d_type;
            break;
        case (MCC_AST_SINGLE_EXPRESSION_TYPE_PARENTH):
            expression->d_type = expression->expression->d_type;
            break;
    }
}

static void ast_semantic_check_expression(struct mCc_ast_expression *expression,
                                          void *data) {
    assert(expression);
    assert(data);

    switch (expression->type) {
        case (MCC_AST_EXPRESSION_TYPE_SINGLE):
            expression->d_type = expression->single_expr->d_type;
            break;
        case (MCC_AST_EXPRESSION_TYPE_BINARY):
            if (expression->lhs->d_type == expression->rhs->d_type) {
                expression->d_type = expression->lhs->d_type;
            } else {
                printf("ERROR: %d -- %d\n", expression->lhs->d_type, expression->rhs->d_type);
                // TODO error message because type is not equal for lhs and rhs
            }
            break;
    }
}

static void ast_semantic_check_call_expression(struct mCc_ast_call_expr *expression,
                                               void *data) {
    assert(expression);
    assert(data);

    expression->d_type = expression->identifier->d_type;
}

static void ast_semantic_check_decl_stmt(struct mCc_ast_declaration *stmt,
                                         void *data){
    assert(stmt);
    assert(data);

    if (stmt->type == MCC_AST_DECLARATION_TYPE_SINGLE) {
        switch(stmt->literal){
            case (MCC_AST_LITERAL_TYPE_INT):
                stmt->identifier->d_type = MCC_AST_TYPE_INT;
                break;
            case (MCC_AST_LITERAL_TYPE_FLOAT):
                stmt->identifier->d_type = MCC_AST_TYPE_FLOAT;
                break;
            case (MCC_AST_LITERAL_TYPE_STRING):
                stmt->identifier->d_type = MCC_AST_TYPE_STRING;
                break;
            case (MCC_AST_LITERAL_TYPE_BOOL):
                stmt->identifier->d_type = MCC_AST_TYPE_BOOL;
                break;
        }
    } else if(stmt->type == MCC_AST_DECLARATION_TYPE_ARRAY) {
        switch(stmt->literal){
            case (MCC_AST_LITERAL_TYPE_INT):
                stmt->identifier->d_type = MCC_AST_TYPE_INT_ARRAY;
                break;
            case (MCC_AST_LITERAL_TYPE_FLOAT):
                stmt->identifier->d_type = MCC_AST_TYPE_FLOAT_ARRAY;
                break;
            case (MCC_AST_LITERAL_TYPE_STRING):
                stmt->identifier->d_type = MCC_AST_TYPE_STRING_ARRAY;
                break;
            case (MCC_AST_LITERAL_TYPE_BOOL):
                stmt->identifier->d_type = MCC_AST_TYPE_BOOL_ARRAY;
                break;
        }
    }
}

static void ast_semantic_check_ret_stmt(struct mCc_ast_ret_stmt *stmt,
                                        void *data) {
    assert(stmt);
    assert(data);

    if(stmt->d_type != stmt->expression->d_type) {
        // TODO wrong return type
        printf("ERROR return has wrong data type\n");
    }
}

struct mCc_parser_result *
mCc_ast_semantic_check(struct mCc_parser_result *result) {
    assert(result);

    g_result = result;
    struct mCc_ast_function_def_array *f = result->func_def;

    struct mCc_ast_visitor visitor = symbol_table_visitor(result);
    mCc_ast_visit_function_def_array(f, &visitor);

    return g_result;
}

