#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include "mCc/tac.h"


#define ARRAY_SIZE 2048
#define DEBUG 0

static void tac_single_expression(struct mCc_ast_single_expression *expression, void *data);
static void tac_expression(struct mCc_ast_expression *expression, void *data);
static void tac_call_expression(struct mCc_ast_call_expr *expression, void *data);

//static void tac_stmt (struct mCc_ast_stmt, void *data);
//static void tac_compound_stmt(struct mCc_ast_compound_stmt, void *data);
//static void tac_ret_stmt(struct mCc_ast_ret_stmt *stmt, void *data);
static void tac_ass_stmt(struct mCc_ast_assignment *stmt, void *data);
//static void tac_if_stmt(struct mCc_ast_if_stmt *stmt, void *data);
//static void tac_while_stmt(struct mCc_ast_while_stmt *stmt, void *data);
/*
static void tac_declaration(struct mCc_ast_declaration, void *data);
static void tac_parameter(struct mCc_ast_parameter, void *data);
static void tac_argument(struct mCc_ast_argument, void *data);
*/

static struct mCc_ast_visitor tac_visitor(void *out)
{
    assert(out);

    return (struct mCc_ast_visitor){
            .traversal = MCC_AST_VISIT_DEPTH_FIRST,
            .order = MCC_AST_VISIT_POST_ORDER,

            .userdata = out,

            .single_expression = tac_single_expression,
            .expression = tac_expression,
            .call_expression = tac_call_expression,

            //.c_stmt = tac_compound_stmt,
           // .ret_stmt = tac_ret_stmt,
            .ass_stmt = tac_ass_stmt,
            //.if_stmt = tac_if_stmt,
            //.while_stmt = tac_while_stmt,
/*
            .decl_stmt = tac_declaration,
            .parameter = tac_parameter,
            .argument = tac_argument,*/

    };
}

/*------------------ LIST HANDLER ------------------*/

tac_list *tac_new_list() {
    tac_list *elem = malloc(sizeof(*elem));
    elem->next = NULL;
    elem->prev = NULL;
    elem->jump = NULL;
    elem->operation_type = MCC_TAC_OPERATION_TYPE_UNKNOWN;
    elem->type = MCC_TAC_ELEMENT_TYPE_UNKNOWN;
    elem->l_type = MCC_TAC_LITERAL_TYPE_UNKNWON;
    elem->is_label = false;
    elem->num_function_param = -1;
    return elem;
}

/*------------------ VISITOR FUNCTIONS ------------------*/

static void tac_single_expression(struct mCc_ast_single_expression *expression, void *data) {
    assert(expression);
    assert(data);

    tac_list *elem = tac_new_list();
    char help[ARRAY_SIZE] = {0};
    sprintf(help, "t%d", v_counter++);

    elem->f_identifier  = malloc(sizeof(char *) * ARRAY_SIZE);
    strcpy(elem->f_identifier, help);

    if (expression->type == MCC_AST_SINGLE_EXPRESSION_TYPE_LITERAL) {
        if (DEBUG) {
            printf("COPY: %s: ", elem->f_identifier);
        }
        elem->type = MCC_TAC_ELEMENT_TYPE_COPY;
        switch (expression->literal->type) {
            case (MCC_AST_LITERAL_TYPE_INT):
                elem->l_type = MCC_TAC_LITERAL_TYPE_INT;
                elem->i_literal = expression->literal->i_value;
                if (DEBUG)
                    printf("%ld\n", elem->i_literal);
                break;
            case (MCC_AST_LITERAL_TYPE_FLOAT):
                elem->l_type = MCC_TAC_LITERAL_TYPE_FLOAT;
                elem->f_literal = expression->literal->f_value;
                if (DEBUG)
                    printf("\n");
                break;
            case (MCC_AST_LITERAL_TYPE_BOOL):
                elem->l_type = MCC_TAC_LITERAL_TYPE_BOOL;
                elem->b_literal = expression->literal->b_value;
                if (DEBUG)
                    printf("\n");
                break;
            case (MCC_AST_LITERAL_TYPE_STRING):
                elem->l_type = MCC_TAC_LITERAL_TYPE_STRING;
                elem->s_literal = expression->literal->s_value;
                if (DEBUG)
                    printf("\n");
                break;
        }
        expression->tac_start=elem;
        expression->tac_end=elem;
    } else if (expression->type == MCC_AST_SINGLE_EXPRESSION_TYPE_IDENTIFIER) {
        if (DEBUG)
            printf("IDENTIFIER\n");
        elem->type = MCC_TAC_ELEMENT_TYPE_COPY;
        elem->s_identifier = malloc(sizeof(char *) * strlen(expression->identifier->renamed));
        strcpy(elem->s_identifier, expression->identifier->renamed);
        expression->tac_start=elem;
        expression->tac_end=elem;
    } else if (expression->type == MCC_AST_SINGLE_EXPRESSION_TYPE_IDENTIFIER_EX) {
        if (DEBUG)
            printf("IDENTIFIER []\n");
        elem->type = MCC_TAC_ELEMENT_TYPE_LOAD;

        elem->s_identifier = malloc(sizeof(char *) * strlen(expression->identifier->renamed));
        strcpy(elem->s_identifier, expression->identifier->renamed);

        tac_list *temp = expression->identifier_expression->tac_end;
        elem->t_identifier = malloc(sizeof(char *) * strlen(temp->f_identifier));
        strcpy(elem->t_identifier, temp->f_identifier);
        expression->tac_start=elem;
        expression->tac_end=elem;
    } else if (expression->type == MCC_AST_SINGLE_EXPRESSION_TYPE_CALL_EXPR) {
        if (DEBUG)
            printf("CALL EXPRESSION\n");
        expression->tac_start=expression->call_expr->tac_start;
        expression->tac_end=expression->call_expr->tac_end;
        // TODO there should be nothing to do .
    } else if (expression->type == MCC_AST_SINGLE_EXPRESSION_TYPE_UNARY_OP) {
        if (DEBUG)
            printf("UNARY OPERATOR\n");
        elem->type = MCC_TAC_ELEMENT_TYPE_UNARY;
        switch (expression->unary_operator) {
            case (MCC_AST_UNARY_OP_FAC):
                elem->operation_type = MCC_TAC_OPERATION_TYPE_FAC;
                break;
            case (MCC_AST_UNARY_OP_NEGATION):
                elem->operation_type = MCC_TAC_OPERATION_TYPE_MINUS;
                break;
        }
        tac_list *temp = expression->unary_expression->tac_end;
        elem->s_identifier = malloc(sizeof(char *) * strlen(temp->f_identifier));
        strcpy(elem->s_identifier, temp->f_identifier);
        expression->tac_start=elem;
        expression->tac_end=elem;
    } else if (expression->type == MCC_AST_SINGLE_EXPRESSION_TYPE_PARENTH) {
        if (DEBUG)
            printf("{ EXPRESSION }\n");
        expression->tac_start=expression->expression->tac_start;
        expression->tac_end=expression->expression->tac_end;
        // TODO there should be nothing to do ...
    }
}

static void tac_expression(struct mCc_ast_expression *expression, void *data){
    assert(expression);
    assert(data);

    tac_list *elem = tac_new_list();

    char help[ARRAY_SIZE] = {0};
    sprintf(help, "t%d", v_counter++);

    elem->f_identifier  = malloc(sizeof(char *) * ARRAY_SIZE);
    strcpy(elem->f_identifier, help);

    if (expression->type == MCC_AST_EXPRESSION_TYPE_SINGLE) {
        if (DEBUG)
            printf("SINGLE EXPRESSION\n");
        expression->tac_start=expression->single_expr->tac_start;
        expression->tac_end=expression->single_expr->tac_end;
        // TODO there should be nothing to do ...
    } else if  (expression->type == MCC_AST_EXPRESSION_TYPE_BINARY) {
        if (DEBUG)
            printf("BINARY OPERATION\n");
        elem->type = MCC_TAC_ELEMENT_TYPE_BINARY;

        tac_list *temp_lhs = expression->lhs->tac_end;
        elem->s_identifier = malloc(sizeof(char *) * strlen(temp_lhs->f_identifier));
        strcpy(elem->s_identifier, temp_lhs->f_identifier);

        tac_list *temp_rhs = expression->rhs->tac_end;
        elem->t_identifier = malloc(sizeof(char *) * strlen(temp_rhs->f_identifier));
        strcpy(elem->t_identifier, temp_rhs->f_identifier);

        switch (expression->op) {
            case (MCC_AST_BINARY_OP_ADD):
                elem->operation_type = MCC_TAC_OPERATION_TYPE_PLUS;
                break;
            case (MCC_AST_BINARY_OP_DIV):
                elem->operation_type = MCC_TAC_OPERATION_TYPE_DIVISION;
                break;
            case (MCC_AST_BINARY_OP_MUL):
                elem->operation_type = MCC_TAC_OPERATION_TYPE_MULTIPLY;
                break;
            case (MCC_AST_BINARY_OP_SUB):
                elem->operation_type = MCC_TAC_OPERATION_TYPE_MINUS;
                break;
            case (MCC_AST_BINARY_OP_AND):
                elem->operation_type = MCC_TAC_OPERATION_TYPE_AND;
                break;
            case (MCC_AST_BINARY_OP_EQ):
                elem->operation_type = MCC_TAC_OPERATION_TYPE_EQ;
                break;
            case (MCC_AST_BINARY_OP_GE):
                elem->operation_type = MCC_TAC_OPERATION_TYPE_GE;
                break;
            case (MCC_AST_BINARY_OP_GT):
                elem->operation_type = MCC_TAC_OPERATION_TYPE_GT;
                break;
            case (MCC_AST_BINARY_OP_LE):
                elem->operation_type = MCC_TAC_OPERATION_TYPE_LE;
                break;
            case (MCC_AST_BINARY_OP_LT):
                elem->operation_type = MCC_TAC_OPERATION_TYPE_LT;
                break;
            case (MCC_AST_BINARY_OP_NEQ):
                elem->operation_type = MCC_TAC_OPERATION_TYPE_NE;
                break;
            case (MCC_AST_BINARY_OP_OR):
                elem->operation_type = MCC_TAC_OPERATION_TYPE_OR;
                break;
        }
        expression->tac_start=elem;
        expression->tac_end=elem;
    }
}

static void tac_call_expression(struct mCc_ast_call_expr *expression, void *data){
    assert(expression);
    assert(data);

    if (DEBUG)
        printf("CALL EXPRESSION\n");
    tac_list *elem = tac_new_list();

    char help[ARRAY_SIZE] = {0};
    sprintf(help, "t%d", v_counter++);

    elem->type = MCC_TAC_ELEMENT_TYPE_PROCEDURE_CALL;


    elem->f_identifier = malloc(sizeof(char *) * strlen(expression->identifier->renamed));
    strcpy(elem->f_identifier, expression->identifier->renamed);
    elem->num_function_param = expression->arguments->counter;

    expression->tac_start=elem;
    expression->tac_end=elem;
}
/*
static void tac_stmt (struct mCc_ast_stmt *stmt, void *data){
    assert(stmt);
    assert(data);
}*/
/*
static void tac_compound_stmt(struct mCc_ast_compound_stmt, void *data){

}

static void tac_ret_stmt(struct mCc_ast_ret_stmt *stmt, void *data){

}
*/
static void tac_ass_stmt(struct mCc_ast_assignment *stmt, void *data){
    assert(stmt);
    assert(data);

    if (DEBUG)
        printf("ASSIGNMENT STMT\n");
    tac_list *elem = tac_new_list();

    char help[ARRAY_SIZE] = {0};
    sprintf(help, "t%d", v_counter++);

    elem->type = MCC_TAC_ELEMENT_TYPE_COPY;

    elem->f_identifier  = malloc(sizeof(char *) * ARRAY_SIZE);
    strcpy(elem->f_identifier, help);

    tac_list *temp = stmt->expression->tac_end;
    elem->s_identifier = malloc(sizeof(char *) * strlen(temp->f_identifier));
    strcpy(elem->s_identifier, temp->f_identifier);

    stmt->tac_start=elem;
    stmt->tac_end=elem;
}
/*
static void tac_if_stmt(struct mCc_ast_if_stmt *stmt, void *data){

}

static void tac_while_stmt(struct mCc_ast_while_stmt *stmt, void *data){

}


static void tac_declaration(struct mCc_ast_declaration, void *data){

}

static void tac_parameter(struct mCc_ast_parameter, void *data){

}

static void tac_argument(struct mCc_ast_argument, void *data){

}
*/

/*------------------ FUNCTION AVAILABLE FROM OUTSIDE ------------------*/

struct mCc_tac_list *mCc_tac_generate(struct mCc_ast_function_def_array *f){
    assert(f);

    head = tac_new_list();
    tail = head;

    v_counter = 0;
    l_counter = 0;

    struct mCc_ast_visitor visitor = tac_visitor(f);

    mCc_ast_visit_function_def_array(f, &visitor);

    return head;
}

void mCc_tac_delete(struct mCc_tac_list *head){
    while (head->prev != NULL)
        head = head->prev;


    struct mCc_tac_list *current = head;
    while (current != NULL) {
        struct mCc_tac_list *next = current->next;
        if (current->f_identifier != NULL)
            free(current->f_identifier);
        if (current->s_identifier != NULL)
            free(current->s_identifier);
        if (current->t_identifier != NULL)
            free(current->t_identifier);
        free(current);
        current = next;
    }
}

void mCc_tac_print(FILE *out, struct mCc_tac_list *head){
    assert(out);
    assert(head);

    struct mCc_tac_list *current = head;
    while (current != NULL) {
        struct mCc_tac_list *next = current->next;
        switch (current->type) {
            case (MCC_TAC_ELEMENT_TYPE_UNKNOWN):
                printf("UNKNOWN\n");
                break;
            case (MCC_TAC_ELEMENT_TYPE_COPY):
                switch (current->l_type) {
                    case (MCC_TAC_LITERAL_TYPE_UNKNWON):
                        fprintf(stdout, "COPY: %s = %s\n", current->f_identifier, current->s_identifier);
                        break;
                    case (MCC_TAC_LITERAL_TYPE_INT):
                        fprintf(stdout, "COPY: %s = %ld\n", current->f_identifier, current->i_literal);
                        break;
                    case (MCC_TAC_LITERAL_TYPE_FLOAT):
                        fprintf(stdout, "COPY: %s = %f\n", current->f_identifier, current->f_literal);
                        break;
                    case (MCC_TAC_LITERAL_TYPE_BOOL):
                        fprintf(stdout, "COPY: %s = %d\n", current->f_identifier, current->b_literal);
                        break;
                    case (MCC_TAC_LITERAL_TYPE_STRING):
                        fprintf(stdout, "COPY: %s = %s\n", current->f_identifier, current->s_literal);
                        break;
                }
                break;
            case (MCC_TAC_ELEMENT_TYPE_UNARY):
                printf("UNARY\n");
                break;
            case (MCC_TAC_ELEMENT_TYPE_BINARY):
                printf("BINARY\n");
                break;
            case (MCC_TAC_ELEMENT_TYPE_UNCONDITIONAL_JUMP):
                printf("UNCONDITIONAL JUMP\n");
                break;
            case (MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP):
                printf("CONDITIONAL JUMP\n");
                break;
            case (MCC_TAC_ELEMENT_TYPE_LABEL):
                printf("LABEL\n");
                break;
            case (MCC_TAC_ELEMENT_TYPE_PARAMETER_SETUP):
                printf("SETUP\n");
                break;
            case (MCC_TAC_ELEMENT_TYPE_PROCEDURE_CALL):
                printf("CALL: %s, %d\n", current->f_identifier, current->num_function_param);
                break;
            case (MCC_TAC_ELEMENT_TYPE_LOAD):
                printf("LOAD\n");
                break;
            case (MCC_TAC_ELEMENT_TYPE_STORE):
                printf("STORE\n");
                break;
            case (MCC_TAC_ELEMENT_TYPE_ADDRESS_ASSIGNMENT):
                printf("ADDRESS ASSIGNMENT\n");
                break;
            case (MCC_TAC_ELEMENT_TYPE_POINTER_ASSIGNMENT):
                printf("POINTER ASSIGNMENT\n");
                break;
        }
        current = next;
    }
}