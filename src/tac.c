#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include "mCc/tac.h"

static void tac_single_expression(struct mCc_ast_single_expression *expression, void *data);
/*static void tac_expression(struct mCc_ast_expression *expression, void *data);
static void tac_call_expression(struct mCc_ast_call_expr *expression, void *data);

//static void tac_stmt (struct mCc_ast_stmt, void *data);
static void tac_compound_stmt(struct mCc_ast_compound_stmt, void *data);
static void tac_ret_stmt(struct mCc_ast_ret_stmt *stmt, void *data);
static void tac_ass_stmt(struct mCc_ast_assignment *stmt, void *data);
static void tac_if_stmt(struct mCc_ast_if_stmt *stmt, void *data);
static void tac_while_stmt(struct mCc_ast_while_stmt *stmt, void *data);

static void tac_declaration(struct mCc_ast_declaration, void *data);
static void tac_parameter(struct mCc_ast_parameter, void *data);
static void tac_argument(struct mCc_ast_argument, void *data);
*/

static struct mCc_ast_visitor tac_visitor(void *out)
{
    assert(out);

    return (struct mCc_ast_visitor){
            .traversal = MCC_AST_VISIT_DEPTH_FIRST,
            .order = MCC_AST_VISIT_PRE_ORDER,

            .userdata = out,

            .single_expression = tac_single_expression,
            /*.expression = tac_expression,
            .call_expression = tac_call_expression,

            .c_stmt = tac_compound_stmt,
            .ret_stmt = tac_ret_stmt,
            .ass_stmt = tac_ass_stmt,
            .if_stmt = tac_if_stmt,
            .while_stmt = tac_while_stmt,

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
    elem->is_label = false;
    return elem;
}

/*------------------ VISITOR FUNCTIONS ------------------*/

static void tac_single_expression(struct mCc_ast_single_expression *expression, void *data) {
    assert(expression);
    assert(data);
}
/*
static void tac_expression(struct mCc_ast_expression *expression, void *data){

}

static void tac_call_expression(struct mCc_ast_call_expr *expression, void *data){

}

static void tac_stmt (struct mCc_ast_stmt, void *data){

}

static void tac_compound_stmt(struct mCc_ast_compound_stmt, void *data){

}

static void tac_ret_stmt(struct mCc_ast_ret_stmt *stmt, void *data){

}

static void tac_ass_stmt(struct mCc_ast_assignment *stmt, void *data){

}

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
        free(current);
        current = next;
    }
}

void mCc_tac_print(FILE *out, struct mCc_tac_list *head){
    assert(out);
    assert(head);
}