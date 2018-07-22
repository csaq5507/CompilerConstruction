#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include "mCc/tac.h"
#include "mCc/utils.h"

static void tac_single_expression(struct mCc_ast_single_expression *expression,
				  void *data);
static void tac_expression(struct mCc_ast_expression *expression, void *data);
static void tac_call_expression(struct mCc_ast_call_expr *expression,
				void *data);
static void tac_function_def(struct mCc_ast_function_def *f, void *data);
static void tac_stmt(struct mCc_ast_stmt *stmt, void *data);
static void tac_compound_stmt(struct mCc_ast_compound_stmt *c_stmt, void *data);
static void tac_ret_stmt(struct mCc_ast_ret_stmt *stmt, void *data);
static void tac_ass_stmt(struct mCc_ast_assignment *stmt, void *data);
static void tac_if_stmt(struct mCc_ast_if_stmt *stmt, void *data);
static void tac_while_stmt(struct mCc_ast_while_stmt *stmt, void *data);
static void tac_declaration(struct mCc_ast_declaration *declaration,
			    void *data);
static void tac_parameter(struct mCc_ast_parameter *parameter, void *data);
static void tac_argument(struct mCc_ast_argument *argument, void *data);


static void set_tac_literal (tac_list *elem, struct mCc_ast_single_expression *expression);
static void add_jump(struct mCc_tac_list *start, struct mCc_tac_list *end,
                     struct mCc_tac_list *jump);
static enum mCc_tac_operation_type ast_to_tac_op_type(enum mCc_ast_binary_op op);
enum mCc_tac_operation_type get_inverse_operator (enum mCc_tac_operation_type type);
static void remove_jump(struct mCc_tac_list *start, struct mCc_tac_list *end,
                        const char *label);
static void negate_or(struct mCc_ast_expression *expression);
static void negate_and(struct mCc_ast_expression *expression);
static void negate_tac (struct mCc_ast_expression *expression);
static void negate_tac_single (struct mCc_ast_single_expression *expression);
static void tac_single_expression_unary(struct mCc_ast_single_expression *expression,
                                        struct mCc_tac_list *elem);
static void generate_tac_operation_or(struct mCc_ast_expression *expression);
static void generate_tac_operation_and(struct mCc_ast_expression *expression);
static struct mCc_ast_compound_stmt *delete_stmt_from_list(struct mCc_ast_compound_stmt *c_stmt,
                                                           int position);
static void bool_operation_assignment(struct mCc_ast_assignment *stmt);
static void bool_and_or_assignment(struct mCc_ast_assignment *stmt);
static void bool_assignment(struct mCc_ast_assignment *stmt);
static bool handle_empty_if_stmt(struct mCc_ast_if_stmt *stmt);



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

		.function_def_type = tac_function_def,
		.function_def_void = tac_function_def,

		.statement = tac_stmt,
		.c_stmt = tac_compound_stmt,
		.ret_stmt = tac_ret_stmt,
		.ass_stmt = tac_ass_stmt,
		.if_stmt = tac_if_stmt,
		.while_stmt = tac_while_stmt,

		.decl_stmt = tac_declaration,
		.parameter = tac_parameter,
		.argument = tac_argument,

	};
}

/*------------------ LIST HANDLER ------------------*/

tac_list *tac_new_list()
{
	tac_list *elem = malloc(sizeof(*elem));
	elem->type = MCC_TAC_ELEMENT_TYPE_UNKNOWN;
	elem->next = NULL;
	elem->prev = NULL;
	elem->identifier1 = NULL;
	return elem;
}

/*------------------ VISITOR FUNCTIONS ------------------*/

static void set_tac_literal (tac_list *elem, struct mCc_ast_single_expression *expression) {
    switch (expression->literal->type) {
        case (MCC_AST_LITERAL_TYPE_INT):
            elem->literal_type = MCC_TAC_LITERAL_TYPE_INT;
            elem->i_literal = expression->literal->i_value;
            break;
        case (MCC_AST_LITERAL_TYPE_FLOAT):
            elem->literal_type = MCC_TAC_LITERAL_TYPE_FLOAT;
            elem->f_literal = expression->literal->f_value;
            break;
        case (MCC_AST_LITERAL_TYPE_BOOL):
            elem->literal_type = MCC_TAC_LITERAL_TYPE_BOOL;
            elem->b_literal = expression->literal->b_value;
            break;
        case (MCC_AST_LITERAL_TYPE_STRING):
            elem->literal_type = MCC_TAC_LITERAL_TYPE_STRING;
            elem->s_literal =
                    copy_string(expression->literal->s_value);
            break;
    }
}

static void add_jump(struct mCc_tac_list *start, struct mCc_tac_list *end,
                     struct mCc_tac_list *jump) {
    assert(start);
    assert(end);
    assert(jump);

    while (start != end) {
        if (start->type == MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP &&
            start->jump == NULL) {
            start->jump = jump;
        }
        start = start->next;
    }
    if (start->type == MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP &&
        start->jump == NULL) {
        start->jump = jump;
    }
}

static enum mCc_tac_operation_type ast_to_tac_op_type(enum mCc_ast_binary_op op) {
    switch (op) {
        case (MCC_AST_BINARY_OP_ADD):
            return MCC_TAC_OPERATION_TYPE_PLUS;
        case (MCC_AST_BINARY_OP_DIV):
            return MCC_TAC_OPERATION_TYPE_DIVISION;
        case (MCC_AST_BINARY_OP_MUL):
            return MCC_TAC_OPERATION_TYPE_MULTIPLY;
        case (MCC_AST_BINARY_OP_SUB):
            return MCC_TAC_OPERATION_TYPE_MINUS;
        case (MCC_AST_BINARY_OP_AND):
            return MCC_TAC_OPERATION_TYPE_AND;
        case (MCC_AST_BINARY_OP_EQ):
            return MCC_TAC_OPERATION_TYPE_EQ;
        case (MCC_AST_BINARY_OP_GE):
            return MCC_TAC_OPERATION_TYPE_GE;
        case (MCC_AST_BINARY_OP_GT):
            return MCC_TAC_OPERATION_TYPE_GT;
        case (MCC_AST_BINARY_OP_LE):
            return MCC_TAC_OPERATION_TYPE_LE;
        case (MCC_AST_BINARY_OP_LT):
            return MCC_TAC_OPERATION_TYPE_LT;
        case (MCC_AST_BINARY_OP_NEQ):
            return MCC_TAC_OPERATION_TYPE_NE;
        case (MCC_AST_BINARY_OP_OR):
            return MCC_TAC_OPERATION_TYPE_OR;
    }
    return MCC_TAC_OPERATION_TYPE_PLUS;
}

enum mCc_tac_operation_type get_inverse_operator (enum mCc_tac_operation_type type) {
    switch (type) {
        case (MCC_TAC_OPERATION_TYPE_PLUS):
            return MCC_TAC_OPERATION_TYPE_PLUS;
        case (MCC_TAC_OPERATION_TYPE_DIVISION):
            return MCC_TAC_OPERATION_TYPE_DIVISION;
        case (MCC_TAC_OPERATION_TYPE_MULTIPLY):
            return MCC_TAC_OPERATION_TYPE_MULTIPLY;
        case (MCC_TAC_OPERATION_TYPE_MINUS):
            return MCC_TAC_OPERATION_TYPE_MINUS;
        case (MCC_TAC_OPERATION_TYPE_FAC):
            return MCC_TAC_OPERATION_TYPE_FAC;
        case (MCC_TAC_OPERATION_TYPE_AND):
            return MCC_TAC_OPERATION_TYPE_AND;
        case (MCC_TAC_OPERATION_TYPE_OR):
            return MCC_TAC_OPERATION_TYPE_OR;
        case (MCC_TAC_OPERATION_TYPE_EQ):
            return MCC_TAC_OPERATION_TYPE_NE;
        case (MCC_TAC_OPERATION_TYPE_GE):
            return MCC_TAC_OPERATION_TYPE_LT;
        case (MCC_TAC_OPERATION_TYPE_GT):
            return MCC_TAC_OPERATION_TYPE_LE;
        case (MCC_TAC_OPERATION_TYPE_LE):
            return MCC_TAC_OPERATION_TYPE_GT;
        case (MCC_TAC_OPERATION_TYPE_LT):
            return MCC_TAC_OPERATION_TYPE_GE;
        case (MCC_TAC_OPERATION_TYPE_NE):
            return MCC_TAC_OPERATION_TYPE_EQ;
    }
    return type;
}

static void remove_jump(struct mCc_tac_list *start, struct mCc_tac_list *end,
                        const char *label) {
    assert(start);
    assert(end);
    assert(label);

    while (start != end) {
        if (start->type == MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP &&
            start->jump != NULL &&
            strcmp(start->jump->identifier1, label) == 0) {
            //free(start->jump);
            start->jump = NULL;
        }
        start = start->next;
    }
}

static void negate_or(struct mCc_ast_expression *expression) {
    assert(expression);

    struct mCc_tac_list *tac_end = expression->tac_end;
    if (expression->rhs->type == MCC_AST_EXPRESSION_TYPE_SINGLE) {
        negate_tac_single(expression->rhs->single_expr);
        expression->tac_end = expression->rhs->single_expr->tac_end;
    }
    else {
        negate_tac(expression->rhs);
        expression->tac_end = expression->rhs->tac_end;
    }
    if (tac_end->type == MCC_TAC_ELEMENT_TYPE_LABEL) {
        remove_jump(expression->tac_start, expression->tac_end,
                    tac_end->identifier1);
        expression->tac_end = tac_end->prev;
        expression->rhs->tac_end = tac_end->prev;
        tac_end->prev->next = tac_end->next;
        if (expression->rhs->type == MCC_AST_EXPRESSION_TYPE_SINGLE) {
            expression->rhs->single_expr->tac_end = expression->tac_end;
        }
        tac_end->prev = NULL;
        tac_end->next = NULL;
        mCc_tac_delete(tac_end);
    }
}

static void negate_and(struct mCc_ast_expression *expression) {
    assert(expression);

    expression->rhs->tac_end = expression->tac_end;
    negate_tac(expression->rhs);
    expression->tac_end = expression->rhs->tac_end;
    struct mCc_tac_list *tac_end = expression->tac_end;
    tac_list *label;
    if (tac_end->type == MCC_TAC_ELEMENT_TYPE_LABEL) {
        label = tac_end;
    } else {
        label = tac_new_list();
        label->type = MCC_TAC_ELEMENT_TYPE_LABEL;
        label->identifier1 = new_string("L%d", l_counter++);
        tac_end->next = label;
        if (tac_end->type == MCC_TAC_ELEMENT_TYPE_BINARY ||
                tac_end->type == MCC_TAC_ELEMENT_TYPE_UNARY) {
            tac_list *jump = tac_new_list();
            jump->type = MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP;
            jump->identifier1 = copy_string(tac_end->identifier1);
            jump->jump = NULL;
            jump->next = label;
            label->prev = jump;
            jump->prev = tac_end;
            tac_end->next = jump;
            tac_end = jump;
        }
        label->prev = tac_end;
        label->next = NULL;
        expression->tac_end = tac_end->next;
        if (expression->rhs->type == MCC_AST_EXPRESSION_TYPE_SINGLE) {
            expression->rhs->single_expr->tac_end = expression->tac_end;
        }
        expression->rhs->tac_end = expression->tac_end;
    }
    add_jump(expression->lhs->tac_start, expression->lhs->tac_end, label);
}

static void negate_tac (struct mCc_ast_expression *expression) {
    assert(expression);

    if (expression->op == MCC_AST_BINARY_OP_OR) {
        negate_or(expression);
    } else if (expression->op == MCC_AST_BINARY_OP_AND) {
        negate_and(expression);
    } else {
        tac_list *expression_end = expression->tac_end;
        tac_list *operation = expression->tac_end;
        while (operation->type == MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP ||
                operation->type == MCC_TAC_ELEMENT_TYPE_LABEL )
            operation =operation->prev;

        if (operation->type == MCC_TAC_ELEMENT_TYPE_BINARY)
            operation->binary_op_type = get_inverse_operator(operation->binary_op_type);
        else if (operation->type == MCC_TAC_ELEMENT_TYPE_COPY_LITERAL)
            operation->b_literal = !operation->b_literal;
        else if (operation->type == MCC_TAC_ELEMENT_TYPE_COPY_IDENTIFIER) {
            if (expression_end->type == MCC_TAC_ELEMENT_TYPE_LABEL ||
                    expression_end->type == MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP) {
                tac_list *unary_not = tac_new_list();
                unary_not->type = MCC_TAC_ELEMENT_TYPE_UNARY;
                unary_not->unary_op_type = MCC_TAC_OPERATION_TYPE_FAC;
                unary_not->identifier1 = new_string("reg_%d", v_counter++);
                unary_not->unary_identifier = copy_string(operation->identifier1);
                unary_not->next = operation->next;
                unary_not->prev = operation;
                operation->next = unary_not;
                unary_not->next->prev = unary_not;
                if (expression_end->type == MCC_TAC_ELEMENT_TYPE_LABEL) {
                    free(expression_end->prev->identifier1);
                    expression_end->prev->identifier1 = copy_string(unary_not->identifier1);
                } else {
                    free(expression_end->identifier1);
                    expression_end->identifier1 = copy_string(unary_not->identifier1);
                }
            } else {
                tac_list *unary_not = tac_new_list();
                unary_not->type = MCC_TAC_ELEMENT_TYPE_UNARY;
                unary_not->unary_op_type = MCC_TAC_OPERATION_TYPE_FAC;
                unary_not->identifier1 = new_string("reg_%d", v_counter++);
                unary_not->unary_identifier = copy_string(operation->identifier1);
                operation->next = unary_not;
                unary_not->prev = operation;
                expression->tac_end = unary_not;
                if (expression->lhs != NULL)
                    expression->lhs->tac_end = unary_not;
                if (expression->single_expr != NULL)
                    expression->single_expr->tac_end = unary_not;
            }
        }
    }
}

static void negate_tac_single (struct mCc_ast_single_expression *expression) {
    assert(expression);

    if (expression->type == MCC_AST_SINGLE_EXPRESSION_TYPE_PARENTH) {
        expression->tac_end = expression->expression->tac_end;
        negate_tac(expression->expression);
        expression->tac_end = expression->expression->tac_end;
    } else if (expression->type == MCC_AST_SINGLE_EXPRESSION_TYPE_IDENTIFIER) {
        tac_list *tmp = expression->tac_end;
        tac_list *unary_not = tac_new_list();
        unary_not->type = MCC_TAC_ELEMENT_TYPE_UNARY;
        unary_not->unary_op_type = MCC_TAC_OPERATION_TYPE_FAC;
        unary_not->identifier1 = new_string("reg_%d", v_counter++);
        unary_not->unary_identifier = copy_string(
                tmp->identifier1);

        tmp->next = unary_not;
        unary_not->prev = tmp;
        expression->tac_end = unary_not;
    }
}

static void tac_single_expression_unary(struct mCc_ast_single_expression *expression,
                                        struct mCc_tac_list *elem) {
    assert(expression);

    if (expression->unary_operator == MCC_AST_UNARY_OP_NEGATION) {
        elem->type = MCC_TAC_ELEMENT_TYPE_UNARY;
        elem->unary_op_type = MCC_TAC_OPERATION_TYPE_MINUS;

        tac_list *temp = expression->unary_expression->tac_end;

        elem->unary_identifier = copy_string(temp->identifier1);

        expression->tac_start = expression->unary_expression->tac_start;
        elem->prev = temp;
        temp->next = elem;
        expression->tac_end = elem;
    } else if (expression->unary_operator == MCC_AST_UNARY_OP_FAC &&
               expression->unary_expression->type == MCC_AST_EXPRESSION_TYPE_SINGLE &&
               expression->unary_expression->single_expr->type != MCC_AST_SINGLE_EXPRESSION_TYPE_PARENTH){
        elem->type = MCC_TAC_ELEMENT_TYPE_UNARY;
        elem->unary_op_type = MCC_TAC_OPERATION_TYPE_FAC;

        tac_list *temp = expression->unary_expression->tac_end;

        elem->unary_identifier = copy_string(temp->identifier1);

        expression->tac_start = expression->unary_expression->tac_start;
        elem->prev = temp;
        temp->next = elem;
        expression->tac_end = elem;
    } else {
        if (expression->unary_expression->type == MCC_AST_EXPRESSION_TYPE_SINGLE) {
            negate_tac_single(expression->unary_expression->single_expr);

            expression->tac_start = expression->unary_expression->single_expr->expression->tac_start;
            expression->tac_end = expression->unary_expression->single_expr->expression->tac_end;
        } else {
            negate_tac(expression->unary_expression);

            expression->tac_start = expression->unary_expression->tac_start;
            expression->tac_end = expression->unary_expression->tac_end;
        }
        free(elem->identifier1);
        free(elem);
    }


}

static void tac_single_expression(struct mCc_ast_single_expression *expression,
				  void *data)
{
	assert(expression);
	assert(data);

    tac_list *elem = tac_new_list();

	elem->identifier1 = new_string("reg_%d", v_counter++);

	if (expression->type == MCC_AST_SINGLE_EXPRESSION_TYPE_LITERAL) {
		elem->type = MCC_TAC_ELEMENT_TYPE_COPY_LITERAL;
        set_tac_literal(elem, expression);
		expression->tac_start = elem;
		expression->tac_end = elem;
	} else if (expression->type
		   == MCC_AST_SINGLE_EXPRESSION_TYPE_IDENTIFIER) {
		elem->type = MCC_TAC_ELEMENT_TYPE_COPY_IDENTIFIER;

		elem->copy_identifier =
			copy_string(expression->identifier->renamed);
		expression->tac_start = elem;
		expression->tac_end = elem;
	} else if (expression->type
		   == MCC_AST_SINGLE_EXPRESSION_TYPE_IDENTIFIER_EX) {
		elem->type = MCC_TAC_ELEMENT_TYPE_LOAD;

        tac_list *temp = expression->identifier_expression->tac_end;
		elem->identifier2 =
			copy_string(expression->identifier->renamed);
		elem->identifier3 = copy_string(temp->identifier1);
		temp->next = elem;
		elem->prev = temp;
		expression->tac_start =
			expression->identifier_expression->tac_start;
		expression->tac_end = elem;
	} else if (expression->type
		   == MCC_AST_SINGLE_EXPRESSION_TYPE_CALL_EXPR) {
		expression->tac_start = expression->call_expr->tac_start;
		expression->tac_end = expression->call_expr->tac_end;
		free(elem->identifier1);
		free(elem);
	} else if (expression->type
		   == MCC_AST_SINGLE_EXPRESSION_TYPE_UNARY_OP) {
        tac_single_expression_unary(expression, elem);
	} else if (expression->type == MCC_AST_SINGLE_EXPRESSION_TYPE_PARENTH) {
		expression->tac_start = expression->expression->tac_start;
		expression->tac_end = expression->expression->tac_end;
		free(elem->identifier1);
		free(elem);
	}
}

static void generate_tac_operation_or(struct mCc_ast_expression *expression) {
	assert(expression);

	struct mCc_tac_list *jump_lhs;
	struct mCc_tac_list *jump_rhs;
	struct mCc_tac_list *label;

    tac_list *rhs_last = expression->rhs->tac_end;
    tac_list *lhs_last = expression->lhs->tac_end;

	if (rhs_last->type == MCC_TAC_ELEMENT_TYPE_LABEL) {
		label = rhs_last;
        jump_rhs = rhs_last->prev;
	} else if (rhs_last->type == MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP) {
		jump_rhs = rhs_last;
		label = tac_new_list();

		label->type = MCC_TAC_ELEMENT_TYPE_LABEL;
		label->identifier1 = new_string("L%d", l_counter++);
	} else {
		jump_rhs = tac_new_list();
		label = tac_new_list();

		jump_rhs->type = MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP;
		jump_rhs->identifier1 = copy_string(((struct mCc_tac_list *) expression->rhs->tac_end)->identifier1);
		jump_rhs->jump = NULL;
		label->type = MCC_TAC_ELEMENT_TYPE_LABEL;
		label->identifier1 = new_string("L%d", l_counter++);
	}

	if (lhs_last->type == MCC_TAC_ELEMENT_TYPE_LABEL) {
		lhs_last->prev->next = lhs_last->next;
		expression->lhs->tac_end = lhs_last->prev;
		lhs_last->next = NULL;
		lhs_last->prev = NULL;
		remove_jump(expression->lhs->tac_start, expression->lhs->tac_end,
				lhs_last->identifier1);
        if (expression->lhs->type == MCC_AST_SINGLE_EXPRESSION_TYPE_PARENTH) {
            expression->lhs->expression->tac_end = expression->lhs->tac_end;
        }

		mCc_tac_delete(lhs_last);

		jump_lhs = expression->lhs->tac_end;
        lhs_last = expression->lhs->tac_end;
	} else if (lhs_last->type == MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP) {
        struct mCc_tac_list *label_and = tac_new_list();
        label_and->type = MCC_TAC_ELEMENT_TYPE_LABEL;
        label_and->identifier1 = new_string("L%d", l_counter++);
        lhs_last->next = label_and;
        label_and->prev = lhs_last;
        expression->lhs->tac_end = label_and;

        add_jump(expression->lhs->tac_start, lhs_last->prev,
                 label_and);

        lhs_last = label_and;
		jump_lhs = lhs_last;
	} else {
		jump_lhs = tac_new_list();
		jump_lhs->type = MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP;
		jump_lhs->identifier1 = copy_string(((struct mCc_tac_list *) expression->lhs->tac_end)->identifier1);
		jump_lhs->jump = label;
	}

    expression->lhs->tac_end = lhs_last;
    if (expression->lhs->type == MCC_AST_SINGLE_EXPRESSION_TYPE_PARENTH) {
        expression->lhs->expression->tac_end = expression->lhs->tac_end;
        if (expression->lhs->expression->type == MCC_AST_EXPRESSION_TYPE_BINARY)
            expression->lhs->expression->rhs->tac_end = expression->lhs->tac_end;
    }
	negate_tac_single(expression->lhs);
    lhs_last = expression->lhs->tac_end;
    if (lhs_last->type == MCC_TAC_ELEMENT_TYPE_UNARY) {
        free(jump_lhs->identifier1);
        jump_lhs->identifier1 = copy_string(lhs_last->identifier1);
    }

	expression->tac_start = expression->lhs->tac_start;
    if (lhs_last->type != MCC_TAC_ELEMENT_TYPE_LABEL) {
        ((struct mCc_tac_list *)expression->lhs->tac_end)->next = jump_lhs;
        jump_lhs->prev = expression->lhs->tac_end;
        jump_lhs->next = expression->rhs->tac_start;
        ((struct mCc_tac_list *)expression->rhs->tac_start)->prev = jump_lhs;
    } else {
        ((struct mCc_tac_list *)expression->lhs->tac_end)->next = expression->rhs->tac_start;
        ((struct mCc_tac_list *)expression->rhs->tac_start)->prev = expression->lhs->tac_end;
    }

    expression->lhs->tac_end = jump_lhs;

    add_jump(expression->lhs->tac_start, expression->lhs->tac_end,
             label);

    if (rhs_last->type != MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP) {
        ((struct mCc_tac_list *)expression->rhs->tac_end)->next = jump_rhs;
        if (jump_rhs->prev == NULL)
            jump_rhs->prev = expression->rhs->tac_end;
        jump_rhs->next = label;
        label->prev = jump_rhs;
    } else {
        ((struct mCc_tac_list *)expression->rhs->tac_end)->next = label;
        label->prev = expression->rhs->tac_end;
    }

	expression->tac_end = label;

    expression->rhs->tac_end = expression->tac_end;
}

static void generate_tac_operation_and(struct mCc_ast_expression *expression) {
	assert(expression);

	struct mCc_tac_list *jump_lhs = tac_new_list();
    struct mCc_tac_list *jump_rhs = tac_new_list();

    tac_list *rhs_last = expression->rhs->tac_end;
    tac_list *lhs_last = expression->lhs->tac_end;


    if (rhs_last->type == MCC_TAC_ELEMENT_TYPE_LABEL ||
            rhs_last->type == MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP) {
        free(jump_rhs);
        jump_rhs = rhs_last;
    } else {
        jump_rhs->type = MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP;
        jump_rhs->identifier1 = copy_string(((struct mCc_tac_list *) expression->rhs->tac_end)->identifier1);
        jump_rhs->jump = NULL;
    }

    if (lhs_last->type == MCC_TAC_ELEMENT_TYPE_LABEL ||
            lhs_last->type == MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP) {
        free(jump_lhs);
        jump_lhs = lhs_last;
    } else {
        jump_lhs->type = MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP;
        jump_lhs->identifier1 = copy_string(((struct mCc_tac_list *) expression->lhs->tac_end)->identifier1);
        jump_lhs->jump = NULL;
    }

    if (!(lhs_last->type == MCC_TAC_ELEMENT_TYPE_LABEL ||
        lhs_last->type == MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP)) {
        ((struct mCc_tac_list *)expression->lhs->tac_end)->next = jump_lhs;
        jump_lhs->prev = expression->lhs->tac_end;
        jump_lhs->next = expression->rhs->tac_start;
        ((struct mCc_tac_list *)expression->rhs->tac_start)->prev = jump_lhs;
    } else {
        ((struct mCc_tac_list *)expression->lhs->tac_end)->next = expression->rhs->tac_start;
        ((struct mCc_tac_list *)expression->rhs->tac_start)->prev = expression->lhs->tac_end;
    }

    expression->tac_start = expression->lhs->tac_start;
    if (!(rhs_last->type == MCC_TAC_ELEMENT_TYPE_LABEL ||
        rhs_last->type == MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP)) {
        ((struct mCc_tac_list *)expression->rhs->tac_end)->next = jump_rhs;
        jump_rhs->prev = expression->rhs->tac_end;
        expression->tac_end = jump_rhs;
    } else {
        expression->tac_end = expression->rhs->tac_end;
    }

    expression->rhs->tac_end = expression->tac_end;
    expression->lhs->tac_end = jump_lhs;

}

static void tac_expression(struct mCc_ast_expression *expression, void *data)
{
	assert(expression);
	assert(data);


	if (expression->type == MCC_AST_EXPRESSION_TYPE_SINGLE) {
		expression->tac_start = expression->single_expr->tac_start;
		expression->tac_end = expression->single_expr->tac_end;
	} else if (expression->type == MCC_AST_EXPRESSION_TYPE_BINARY) {

		enum mCc_tac_operation_type op_type = ast_to_tac_op_type(expression->op);

		if (op_type == MCC_TAC_OPERATION_TYPE_OR) {
			generate_tac_operation_or(expression);
		} else if (op_type == MCC_TAC_OPERATION_TYPE_AND) {
			generate_tac_operation_and(expression);
		} else {
            tac_list *elem = tac_new_list();
			elem->identifier1 = new_string("reg_%d", v_counter++);

			elem->type = MCC_TAC_ELEMENT_TYPE_BINARY;

			tac_list *temp_lhs_end = expression->lhs->tac_end;
			tac_list *temp_lhs_star = expression->lhs->tac_start;
            tac_list *temp_rhs_end = expression->rhs->tac_end;
            tac_list *temp_rhs_start = expression->rhs->tac_start;

            elem->lhs = copy_string(temp_lhs_end->identifier1);
            elem->rhs = copy_string(temp_rhs_end->identifier1);
            elem->binary_op_type = op_type;

            if (temp_rhs_end->type == MCC_TAC_ELEMENT_TYPE_BINARY) {
                if (elem->binary_op_type == MCC_TAC_OPERATION_TYPE_MINUS &&
                        temp_rhs_end->binary_op_type==  MCC_TAC_OPERATION_TYPE_MINUS) {
                    temp_rhs_end->binary_op_type= MCC_TAC_OPERATION_TYPE_PLUS;
                }
                temp_rhs_end->next = temp_lhs_star;
                temp_lhs_star->prev = temp_rhs_end;
                temp_lhs_end->next = elem;
                elem->prev = temp_lhs_end;

                expression->tac_start = temp_rhs_start;
                expression->tac_end = elem;
            } else {

                temp_lhs_end->next = temp_rhs_start;
                temp_rhs_start->prev = temp_lhs_end;
                temp_rhs_end->next = elem;
                elem->prev = temp_rhs_end;
                expression->tac_start = temp_lhs_star;
                expression->tac_end = elem;
            }


		}

	}
}

static void tac_call_expression(struct mCc_ast_call_expr *expression,
				void *data)
{
	assert(expression);
	assert(data);

	tac_list *elem = tac_new_list();

	elem->type = MCC_TAC_ELEMENT_TYPE_PROCEDURE_CALL;

	elem->identifier1 = copy_string(expression->identifier->renamed);
    elem->ret_type = expression->d_type;
	if (expression->d_type != MCC_AST_TYPE_VOID) {
		tac_list *ret_elem = tac_new_list();
		ret_elem->type = MCC_TAC_ELEMENT_TYPE_PARAMETER_SETUP_CALL;

		ret_elem->identifier1 = copy_string("result");
		ret_elem->param_size =
			1; // default only arrays have another size
		ret_elem->next = elem;
		elem->prev = ret_elem;
		if (expression->arguments == NULL)
			elem->num_function_param = 0;
		else
			elem->num_function_param =
				expression->arguments->counter + 1;
		expression->tac_start = ret_elem;
		expression->tac_end = elem;
	} else {
		expression->tac_start = elem;
		expression->tac_end = elem;
		if (expression->arguments == NULL)
			elem->num_function_param = 0;
		else
			elem->num_function_param =
				expression->arguments->counter;
	}

	if (expression->arguments != NULL) {
		for (int i = expression->arguments->counter - 1; i >= 0; i--) {
			tac_list *temp_end =
				expression->arguments->expression[i].tac_end;
			tac_list *temp_start = expression->tac_start;
			temp_end->next = expression->tac_start;
			temp_start->prev = temp_end;
			expression->tac_start =
				expression->arguments->expression[i].tac_start;
		}
		if (expression->d_type == MCC_AST_TYPE_VOID) {
			for (int i = expression->arguments->counter - 1; i >= 0; i--) {
				tac_list *actual =
						expression->arguments->expression[i].tac_end;
				tac_list *ret_elem = tac_new_list();
				ret_elem->type = MCC_TAC_ELEMENT_TYPE_PARAMETER_SETUP_CALL;

				ret_elem->identifier1 = copy_string(actual->identifier1);
				ret_elem->param_size =
						1; // default only arrays have another size
				tac_list *temp_end = expression->tac_end;
				tac_list *temp_end1 = temp_end->prev;
				temp_end1->next = ret_elem;
				ret_elem->prev = temp_end1;
				ret_elem->next = temp_end;
				temp_end->prev = ret_elem;
			}
		} else {
			for (int i = expression->arguments->counter - 1; i >= 0; i--) {
				tac_list *actual =
						expression->arguments->expression[i].tac_end;
				tac_list *ret_elem = tac_new_list();
				ret_elem->type = MCC_TAC_ELEMENT_TYPE_PARAMETER_SETUP_CALL;

				ret_elem->identifier1 = copy_string(actual->identifier1);
				ret_elem->param_size =
						1; // default only arrays have another size
				tac_list *temp_end = expression->tac_end;
				temp_end = temp_end->prev;
				tac_list *temp_end1 = temp_end->prev;
				temp_end1->next = ret_elem;
				ret_elem->prev = temp_end1;
				ret_elem->next = temp_end;
				temp_end->prev = ret_elem;
			}
		}
	}
}

static void tac_function_def(struct mCc_ast_function_def *f, void *data)
{
	assert(f);
	assert(data);


	tac_list *start = tac_new_list();
	tac_list *end = tac_new_list();

	start->type = MCC_TAC_ELEMENT_TYPE_FUNCTION_START;
    start->num_function_param = f->params->counter;
	start->identifier1 = copy_string(f->identifier->renamed);

	end->type = MCC_TAC_ELEMENT_TYPE_FUNCTION_END;

	end->identifier1 = copy_string(f->identifier->renamed);

	tac_list *temp_c_stmt_start = f->c_stmt->tac_start;
	tac_list *temp_c_stmt_end = f->c_stmt->tac_end;
	tac_list *temp_param_end = f->params->tac_end;
	tac_list *temp_param_start = f->params->tac_start;

	start->next = temp_param_start;
	temp_param_start->prev = start;

	temp_param_end->next = temp_c_stmt_start;
	temp_c_stmt_start->prev = temp_param_end;

	temp_c_stmt_end->next = end;
	end->prev = temp_c_stmt_end;

	end->next = head;
	head->prev = end;

	head = start;
}

static void tac_stmt(struct mCc_ast_stmt *stmt, void *data)
{
	assert(stmt);
	assert(data);


	switch (stmt->type) {
        case (MCC_AST_IF_STMT):
            stmt->tac_start = stmt->if_stmt->tac_start;
            stmt->tac_end = stmt->if_stmt->tac_end;
            break;
        case (MCC_AST_WHILE_STMT):
            stmt->tac_start = stmt->while_stmt->tac_start;
            stmt->tac_end = stmt->while_stmt->tac_end;
            break;
        case (MCC_AST_RET_STMT):
            stmt->tac_start = stmt->ret_stmt->tac_start;
            stmt->tac_end = stmt->ret_stmt->tac_end;
            break;
        case (MCC_AST_DECL_STMT):
            stmt->tac_start = stmt->declaration->tac_start;
            stmt->tac_end = stmt->declaration->tac_end;
            break;
        case (MCC_AST_ASS_STMT):
            stmt->tac_start = stmt->assignment->tac_start;
            stmt->tac_end = stmt->assignment->tac_end;
            break;
        case (MCC_AST_EXPR_STMT):
            stmt->tac_start = stmt->expression->tac_start;
            stmt->tac_end = stmt->expression->tac_end;
            break;
        case (MCC_AST_COMPOUND_STMT):
            stmt->tac_start = stmt->compound_stmt->tac_start;
            stmt->tac_end = stmt->compound_stmt->tac_end;
            break;
        default:
            break;
	}
}

static struct mCc_ast_compound_stmt *delete_stmt_from_list(struct mCc_ast_compound_stmt *c_stmt,
                                                                  int position) {
    assert(c_stmt);
    int i;
    for (i = position; i < c_stmt->counter - 1; i++) {
        c_stmt->statements[i] = c_stmt->statements[i + 1];
    }
    c_stmt->counter = c_stmt->counter - 1;
    return c_stmt;
}

static void tac_compound_stmt(struct mCc_ast_compound_stmt *c_stmt, void *data)
{
	assert(c_stmt);
	assert(data);

    for (int i = 0; i < c_stmt->counter; i++) {
        tac_list *temp_start = c_stmt->statements[i].tac_start;
        tac_list *temp_end = c_stmt->statements[i].tac_end;
        if (temp_start == temp_end &&
                temp_start->type == MCC_TAC_ELEMENT_TYPE_LABEL) {
            mCc_tac_delete(temp_start);
            c_stmt = delete_stmt_from_list(c_stmt, i);
            i--;
        }
    }

	if (c_stmt->counter == 0) {
		tac_list *elem = tac_new_list();
		c_stmt->tac_start = elem;
		c_stmt->tac_end = elem;
	} else {
		ast_stmt *prev = &c_stmt->statements[0];
		for (int i = 1; i < c_stmt->counter; i++) {
			tac_list *temp_start = c_stmt->statements[i].tac_start;
			tac_list *temp_end = prev->tac_end;
			temp_start->prev = temp_end;
			temp_end->next = temp_start;
			prev = &c_stmt->statements[i];
		}
		c_stmt->tac_start = c_stmt->statements[0].tac_start;
		c_stmt->tac_end =
			c_stmt->statements[c_stmt->counter - 1].tac_end;
	}
}

static void tac_ret_stmt(struct mCc_ast_ret_stmt *stmt, void *data)
{
	assert(stmt);
	assert(data);



    tac_list *elem_return = tac_new_list();
    elem_return->type = MCC_TAC_ELEMENT_TYPE_RETURN;

	if (stmt->expression != NULL) {
        tac_list *expression_end = stmt->expression->tac_end;

        if ((stmt->d_type == MCC_AST_TYPE_BOOL &&
             (expression_end->type == MCC_TAC_ELEMENT_TYPE_BINARY ||
              expression_end->type == MCC_TAC_ELEMENT_TYPE_UNARY)) ||
                (expression_end->type == MCC_TAC_ELEMENT_TYPE_LABEL ||
                   expression_end->type == MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP)) {

            tac_list *label_false = tac_new_list();
            label_false->type = MCC_TAC_ELEMENT_TYPE_LABEL;
            label_false->identifier1 = new_string("L%d", l_counter++);
            tac_list *label = tac_new_list();
            label->type = MCC_TAC_ELEMENT_TYPE_LABEL;
            label->identifier1 = new_string("L%d", l_counter++);

            tac_list *elem_false = tac_new_list();
            tac_list *elem_true = tac_new_list();

            elem_false->type = MCC_TAC_ELEMENT_TYPE_COPY_LITERAL;
            elem_false->literal_type = MCC_TAC_LITERAL_TYPE_BOOL;
            elem_false->b_literal = false;
            elem_false->identifier1 = new_string("reg%d", v_counter++);

            elem_true->type = MCC_TAC_ELEMENT_TYPE_COPY_LITERAL;
            elem_true->literal_type = MCC_TAC_LITERAL_TYPE_BOOL;
            elem_true->b_literal = true;
            elem_true->identifier1 = copy_string(elem_false->identifier1);

            if (stmt->d_type == MCC_AST_TYPE_BOOL &&
                (expression_end->type == MCC_TAC_ELEMENT_TYPE_BINARY ||
                 expression_end->type == MCC_TAC_ELEMENT_TYPE_UNARY)) {
                tac_list *jump_false = tac_new_list();
                tac_list *jump = tac_new_list();
                jump_false->type = MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP;
                jump_false->identifier1 = copy_string(expression_end->prev->identifier1);
                jump_false->jump = label_false;
                jump->type = MCC_TAC_ELEMENT_TYPE_UNCONDITIONAL_JUMP;
                jump->jump = label;

                expression_end->next = jump_false;
                jump_false->prev = expression_end;
                jump_false->next = elem_true;
                elem_true->prev = jump_false;
                elem_true->next = jump;
                jump->prev = elem_true;
                jump->next = label_false;
                label_false->prev = jump;
                label_false->next = elem_false;
                elem_false->prev = label_false;
                elem_false->next = label;
                label->prev = elem_false;
            } else {
                tac_list *jump = tac_new_list();
                jump->type = MCC_TAC_ELEMENT_TYPE_UNCONDITIONAL_JUMP;
                jump->jump = label;

                expression_end->next = elem_true;
                elem_true->prev = expression_end;
                elem_true->next = jump;
                jump->prev = elem_true;
                jump->next = label_false;
                label_false->prev = jump;
                label_false->next = elem_false;
                elem_false->prev = label_false;
                elem_false->next = label;
                label->prev = elem_false;
            }

            elem_return->type = MCC_TAC_ELEMENT_TYPE_RETURN;
            elem_return->identifier1 = copy_string(elem_false->identifier1);

            label->next = elem_return;
            elem_return->prev = label;

            stmt->tac_start = stmt->expression->tac_start;
            stmt->tac_end = elem_return;

        } else {

            tac_list *temp = stmt->expression->tac_end;

            elem_return->identifier1 = copy_string(temp->identifier1);

            elem_return->prev = temp;
            temp->next = elem_return;

            stmt->tac_start = stmt->expression->tac_start;
            stmt->tac_end = elem_return;

        }

	} else {
		stmt->tac_start = elem_return;
		stmt->tac_end = elem_return;
	}
}

static void bool_operation_assignment(struct mCc_ast_assignment *stmt) {
    assert(stmt);

    tac_list *expression_end = stmt->expression->tac_end;

    tac_list *label_false = tac_new_list();
    label_false->type = MCC_TAC_ELEMENT_TYPE_LABEL;
    label_false->identifier1 = new_string("L%d", l_counter++);
    tac_list *label = tac_new_list();
    label->type = MCC_TAC_ELEMENT_TYPE_LABEL;
    label->identifier1 = new_string("L%d", l_counter++);

    tac_list *jump_false = tac_new_list();
    tac_list *jump = tac_new_list();
    jump_false->type = MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP;
    jump_false->identifier1 = copy_string(expression_end->prev->identifier1);
    jump_false->jump = label_false;
    jump->type = MCC_TAC_ELEMENT_TYPE_UNCONDITIONAL_JUMP;
    jump->jump = label;

    if (stmt->numerator == NULL) {
        tac_list *elem_false = tac_new_list();
        tac_list *elem_true = tac_new_list();

        elem_false->type = MCC_TAC_ELEMENT_TYPE_COPY_LITERAL;
        elem_false->literal_type = MCC_TAC_LITERAL_TYPE_BOOL;
        elem_false->b_literal = false;
        elem_false->identifier1 = copy_string(stmt->identifier->renamed);

        elem_true->type = MCC_TAC_ELEMENT_TYPE_COPY_LITERAL;
        elem_true->literal_type = MCC_TAC_LITERAL_TYPE_BOOL;
        elem_true->b_literal = true;
        elem_true->identifier1 = copy_string(stmt->identifier->renamed);

        expression_end->next = jump_false;
        jump_false->prev = expression_end;
        jump_false->next = elem_true;
        elem_true->prev = jump_false;
        elem_true->next = jump;
        jump->prev = elem_true;
        jump->next = label_false;
        label_false->prev = jump;
        label_false->next = elem_false;
        elem_false->prev = label_false;
        elem_false->next = label;
        label->prev = elem_false;

        stmt->tac_start = stmt->expression->tac_start;
        stmt->tac_end = label;
    } else {
        tac_list *elem_false = tac_new_list();
        tac_list *elem_true = tac_new_list();
        tac_list *elem_store = tac_new_list();
        tac_list *numerator_end = stmt->numerator->tac_end;
        tac_list *expression_start = stmt->expression->tac_start;

        elem_false->type = MCC_TAC_ELEMENT_TYPE_COPY_LITERAL;
        elem_false->literal_type = MCC_TAC_LITERAL_TYPE_BOOL;
        elem_false->b_literal = false;
        elem_false->identifier1 = new_string("reg%d", v_counter++);

        elem_true->type = MCC_TAC_ELEMENT_TYPE_COPY_LITERAL;
        elem_true->literal_type = MCC_TAC_LITERAL_TYPE_BOOL;
        elem_true->b_literal = true;
        elem_true->identifier1 = copy_string(elem_false->identifier1);

        elem_store->type = MCC_TAC_ELEMENT_TYPE_STORE;
        elem_store->identifier1 = copy_string(stmt->identifier->renamed);
        elem_store->identifier2 = copy_string(numerator_end->identifier1);
        elem_store->identifier3 = copy_string(elem_false->identifier1);

        numerator_end->next = expression_start;
        expression_start->prev = numerator_end;
        expression_end->next = jump_false;
        jump_false->prev = expression_end;
        jump_false->next = elem_true;
        elem_true->prev = jump_false;
        elem_true->next = jump;
        jump->prev = elem_true;
        jump->next = label_false;
        label_false->prev = jump;
        label_false->next = elem_false;
        elem_false->prev = label_false;
        elem_false->next = label;
        label->prev = elem_false;

        label->next = elem_store;
        elem_store->prev = label;

        stmt->tac_start = stmt->numerator->tac_start;
        stmt->tac_end = elem_store;
    }
}

static void bool_and_or_assignment(struct mCc_ast_assignment *stmt) {
    assert(stmt);

    tac_list *expression_end = stmt->expression->tac_end;

    tac_list *label_false = tac_new_list();
    label_false->type = MCC_TAC_ELEMENT_TYPE_LABEL;
    label_false->identifier1 = new_string("L%d", l_counter++);
    tac_list *label = tac_new_list();
    label->type = MCC_TAC_ELEMENT_TYPE_LABEL;
    label->identifier1 = new_string("L%d", l_counter++);

    tac_list *jump = tac_new_list();
    jump->type = MCC_TAC_ELEMENT_TYPE_UNCONDITIONAL_JUMP;
    jump->jump = label;

    if (stmt->numerator == NULL) {
        tac_list *elem_false = tac_new_list();
        tac_list *elem_true = tac_new_list();

        elem_false->type = MCC_TAC_ELEMENT_TYPE_COPY_LITERAL;
        elem_false->literal_type = MCC_TAC_LITERAL_TYPE_BOOL;
        elem_false->b_literal = false;
        elem_false->identifier1 = copy_string(stmt->identifier->renamed);

        elem_true->type = MCC_TAC_ELEMENT_TYPE_COPY_LITERAL;
        elem_true->literal_type = MCC_TAC_LITERAL_TYPE_BOOL;
        elem_true->b_literal = true;
        elem_true->identifier1 = copy_string(stmt->identifier->renamed);

        expression_end->next = elem_true;
        elem_true->prev = expression_end;
        elem_true->next = jump;
        jump->prev = elem_true;
        jump->next = label_false;
        label_false->prev = jump;
        label_false->next = elem_false;
        elem_false->prev = label_false;
        elem_false->next = label;
        label->prev = elem_false;

        stmt->tac_start = stmt->expression->tac_start;
        stmt->tac_end = label;
        add_jump(stmt->tac_start, stmt->tac_end, label_false);
    } else {
        tac_list *elem_false = tac_new_list();
        tac_list *elem_true = tac_new_list();
        tac_list *elem_store = tac_new_list();
        tac_list *numerator_end = stmt->numerator->tac_end;
        tac_list *expression_start = stmt->expression->tac_start;

        elem_false->type = MCC_TAC_ELEMENT_TYPE_COPY_LITERAL;
        elem_false->literal_type = MCC_TAC_LITERAL_TYPE_BOOL;
        elem_false->b_literal = false;
        elem_false->identifier1 = new_string("reg%d", v_counter++);

        elem_true->type = MCC_TAC_ELEMENT_TYPE_COPY_LITERAL;
        elem_true->literal_type = MCC_TAC_LITERAL_TYPE_BOOL;
        elem_true->b_literal = true;
        elem_true->identifier1 = copy_string(elem_false->identifier1);

        elem_store->type = MCC_TAC_ELEMENT_TYPE_STORE;
        elem_store->identifier1 = copy_string(stmt->identifier->renamed);
        elem_store->identifier2 = copy_string(numerator_end->identifier1);
        elem_store->identifier3 = copy_string(elem_false->identifier1);

        numerator_end->next = expression_start;
        expression_start->prev = numerator_end;
        expression_end->next = elem_true;
        elem_true->prev = expression_end;
        elem_true->next = jump;
        jump->prev = elem_true;
        jump->next = label_false;
        label_false->prev = jump;
        label_false->next = elem_false;
        elem_false->prev = label_false;
        elem_false->next = label;
        label->prev = elem_false;

        label->next = elem_store;
        elem_store->prev = label;

        stmt->tac_start = stmt->numerator->tac_start;
        stmt->tac_end = elem_store;
        add_jump(stmt->tac_start, stmt->tac_end, label_false);
    }
}

static void bool_assignment(struct mCc_ast_assignment *stmt) {
    assert(stmt);

    tac_list *expression_end = stmt->expression->tac_end;

    if (expression_end->type == MCC_TAC_ELEMENT_TYPE_BINARY ||
        expression_end->type == MCC_TAC_ELEMENT_TYPE_UNARY) {
        bool_operation_assignment(stmt);

    } else if (expression_end->type == MCC_TAC_ELEMENT_TYPE_LABEL ||
               expression_end->type == MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP) {

        bool_and_or_assignment(stmt);

    } else {
        if (stmt->numerator == NULL) {
            tac_list *elem = tac_new_list();

            elem->type = MCC_TAC_ELEMENT_TYPE_COPY_IDENTIFIER;

            elem->identifier1 = copy_string(stmt->identifier->renamed);

            tac_list *temp = stmt->expression->tac_end;

            elem->copy_identifier = copy_string(temp->identifier1);

            temp->next = elem;
            elem->prev = temp;

            stmt->tac_start = stmt->expression->tac_start;
            stmt->tac_end = elem;
        } else {
            tac_list *elem = tac_new_list();

            elem->type = MCC_TAC_ELEMENT_TYPE_STORE;

            elem->identifier1 = copy_string(stmt->identifier->renamed);

            tac_list *numerator_end = stmt->numerator->tac_end;

            elem->identifier2 = copy_string(numerator_end->identifier1);

            tac_list *expression_start = stmt->expression->tac_start;

            elem->identifier3 = copy_string(expression_end->identifier1);

            numerator_end->next = expression_start;
            expression_start->prev = numerator_end;

            expression_end->next = elem;
            elem->prev = expression_end;

            stmt->tac_start = stmt->numerator->tac_start;
            stmt->tac_end = elem;
        }
    }

}

static void tac_ass_stmt(struct mCc_ast_assignment *stmt, void *data)
{
	assert(stmt);
	assert(data);

    if (stmt->identifier->d_type == MCC_AST_TYPE_BOOL) {
        bool_assignment(stmt);
    } else {
        if (stmt->numerator == NULL) {
            tac_list *elem = tac_new_list();

            elem->type = MCC_TAC_ELEMENT_TYPE_COPY_IDENTIFIER;

            elem->identifier1 = copy_string(stmt->identifier->renamed);

            tac_list *temp = stmt->expression->tac_end;

            elem->copy_identifier = copy_string(temp->identifier1);

            temp->next = elem;
            elem->prev = temp;

            stmt->tac_start = stmt->expression->tac_start;
            stmt->tac_end = elem;
        } else {
            tac_list *elem = tac_new_list();

            elem->type = MCC_TAC_ELEMENT_TYPE_STORE;

            elem->identifier1 = copy_string(stmt->identifier->renamed);

            tac_list *numerator_end = stmt->numerator->tac_end;

            elem->identifier2 = copy_string(numerator_end->identifier1);

            tac_list *expression_end = stmt->expression->tac_end;
            tac_list *expression_start = stmt->expression->tac_start;

            elem->identifier3 = copy_string(expression_end->identifier1);

            numerator_end->next = expression_start;
            expression_start->prev = numerator_end;

            expression_end->next = elem;
            elem->prev = expression_end;

            stmt->tac_start = stmt->numerator->tac_start;
            stmt->tac_end = elem;
        }
    }

}

static bool handle_empty_if_stmt(struct mCc_ast_if_stmt *stmt) {
    assert(stmt);

    if (stmt->else_statement == NULL) {
        if (stmt->statement->tac_start == stmt->statement->tac_end) {
            tac_list *labelHelp = tac_new_list();
            labelHelp->type = MCC_TAC_ELEMENT_TYPE_LABEL;
            labelHelp->identifier1 = new_string("L%d", l_counter++);
            stmt->tac_start = labelHelp;
            stmt->tac_end = labelHelp;
            mCc_tac_delete(stmt->expression->tac_start);
            mCc_tac_delete(stmt->statement->tac_start);
            return true;
        }
    } else {
        if (stmt->statement->tac_start == stmt->statement->tac_end &&
            stmt->else_statement->tac_start == stmt->else_statement->tac_end) {
            tac_list *labelHelp = tac_new_list();
            labelHelp->type = MCC_TAC_ELEMENT_TYPE_LABEL;
            labelHelp->identifier1 = new_string("L%d", l_counter++);
            stmt->tac_start = labelHelp;
            stmt->tac_end = labelHelp;
            mCc_tac_delete(stmt->expression->tac_start);
            mCc_tac_delete(stmt->statement->tac_start);
            mCc_tac_delete(stmt->else_statement->tac_start);
            return true;
        } else if (stmt->statement->tac_start == stmt->statement->tac_end) {
            mCc_tac_delete(stmt->statement->tac_start);
            if (stmt->else_statement->type != MCC_AST_IF_STMT) {
                negate_tac(stmt->expression);
                stmt->statement = stmt->else_statement;
                stmt->else_statement = NULL;
            } else {
                mCc_tac_delete(stmt->expression->tac_start);
                stmt->tac_start = stmt->else_statement->if_stmt->tac_start;
                stmt->tac_end = stmt->else_statement->if_stmt->tac_end;
                stmt->expression = stmt->else_statement->if_stmt->expression;
                stmt->statement = stmt->else_statement->if_stmt->statement;
                stmt->else_statement = stmt->else_statement->if_stmt->else_statement;
                return true;
            }
        } else {
            if (stmt->else_statement->tac_start == stmt->else_statement->tac_end) {
                mCc_tac_delete(stmt->else_statement->tac_start);
                stmt->else_statement = NULL;
            }
        }

    }

    return false;
}

static void tac_if_stmt(struct mCc_ast_if_stmt *stmt, void *data)
{
	assert(stmt);
	assert(data);


    if (handle_empty_if_stmt(stmt)) {
        return;
    }

    tac_list *temp_expression_end = stmt->expression->tac_end;
    if (temp_expression_end->type == MCC_TAC_ELEMENT_TYPE_BINARY ||
            temp_expression_end->type == MCC_TAC_ELEMENT_TYPE_UNARY ||
            temp_expression_end->type == MCC_TAC_ELEMENT_TYPE_COPY_LITERAL||
            temp_expression_end->type == MCC_TAC_ELEMENT_TYPE_COPY_IDENTIFIER ||
            temp_expression_end->type == MCC_TAC_ELEMENT_TYPE_PROCEDURE_CALL) {
        tac_list *jump_false = tac_new_list();

        jump_false->type = MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP;

        jump_false->identifier1 = copy_string(temp_expression_end->identifier1);

        jump_false->jump = NULL;

        temp_expression_end->next = jump_false;
        jump_false->prev = temp_expression_end;
        stmt->expression->tac_end = jump_false;
    }

    tac_list *prev_end;
    tac_list *label_end;
    if (stmt->else_statement != NULL) {
        prev_end = stmt->else_statement->tac_end;

        if (prev_end->type == MCC_TAC_ELEMENT_TYPE_LABEL) {
            label_end = prev_end;
        } else {
            label_end = tac_new_list();
            label_end->type = MCC_TAC_ELEMENT_TYPE_LABEL;
            label_end->identifier1 = new_string("L%d", l_counter++);
        }
    } else {
        label_end = tac_new_list();
        label_end->type = MCC_TAC_ELEMENT_TYPE_LABEL;
        label_end->identifier1 = new_string("L%d", l_counter++);
    }


    tac_list *jump = tac_new_list();
    temp_expression_end = stmt->expression->tac_end;
    jump->type = MCC_TAC_ELEMENT_TYPE_UNCONDITIONAL_JUMP;

    stmt->tac_start = stmt->expression->tac_start;
    if (stmt->else_statement != NULL) {

        tac_list *label = tac_new_list();
        label->type = MCC_TAC_ELEMENT_TYPE_LABEL;
        label->identifier1 = new_string("L%d", l_counter++);

        tac_list *temp_else_stmt_start =
                stmt->else_statement->tac_start;
        tac_list *temp_else_stmt_end = stmt->else_statement->tac_end;
        tac_list *temp_stmt_start = stmt->statement->tac_start;
        tac_list *temp_stmt_end = stmt->statement->tac_end;
        jump->jump = label_end;

        temp_expression_end->next = temp_stmt_start;
        temp_stmt_start->prev = temp_expression_end;

        temp_stmt_end->next = jump;
        jump->prev = temp_stmt_end;

        jump->next = label;
        label->prev = jump;
        label->next = temp_else_stmt_start;
        temp_else_stmt_start->prev = label;

        if (temp_else_stmt_end != label_end) {
            temp_else_stmt_end->next = label_end;
            label_end->prev = temp_else_stmt_end;
        }

        stmt->tac_end = label_end;
        add_jump(stmt->tac_start, stmt->tac_end,
                 label);
    } else {
        free(jump);
        tac_list *temp_stmt_start = stmt->statement->tac_start;
        tac_list *temp_stmt_end = stmt->statement->tac_end;

        temp_expression_end->next = temp_stmt_start;
        temp_stmt_start->prev = temp_expression_end;

        temp_stmt_end->next = label_end;
        label_end->prev = temp_stmt_end;

        stmt->tac_end = label_end;
        add_jump(stmt->tac_start, stmt->tac_end,
                                            label_end);
    }

}

static void tac_while_stmt(struct mCc_ast_while_stmt *stmt, void *data)
{
	assert(stmt);
	assert(data);

	if (stmt->statement->tac_start == stmt->statement->tac_end) {
		tac_list *labelHelp = tac_new_list();
		labelHelp->type = MCC_TAC_ELEMENT_TYPE_LABEL;
		labelHelp->identifier1 = new_string("L%d", l_counter++);
		stmt->tac_start = labelHelp;
		stmt->tac_end = labelHelp;
		mCc_tac_delete(stmt->expression->tac_start);
		mCc_tac_delete(stmt->statement->tac_start);
    } else if (((tac_list *)stmt->expression->tac_end)->type == MCC_TAC_ELEMENT_TYPE_BINARY ||
               ((tac_list *)stmt->expression->tac_end)->type == MCC_TAC_ELEMENT_TYPE_UNARY ||
               ((tac_list *)stmt->expression->tac_end)->type == MCC_TAC_ELEMENT_TYPE_COPY_LITERAL) {

        tac_list *jump = tac_new_list();
        tac_list *jump_false = tac_new_list();
        tac_list *label_jump_false = tac_new_list();
        tac_list *label_jump = tac_new_list();
        tac_list *labelHelp = tac_new_list();

        labelHelp->type = MCC_TAC_ELEMENT_TYPE_LABEL;
        labelHelp->identifier1 = new_string("L%d", l_counter++);


        label_jump->type = MCC_TAC_ELEMENT_TYPE_LABEL;
        label_jump->identifier1 = new_string("L%d", l_counter++);

        label_jump_false->type = MCC_TAC_ELEMENT_TYPE_LABEL;
        label_jump_false->identifier1 = new_string("L%d", l_counter++);


        tac_list *temp_stmt_start = stmt->statement->tac_start;
        tac_list *temp_stmt_end = stmt->statement->tac_end;

        tac_list *temp_expression_start = stmt->expression->tac_start;
        tac_list *temp_expression_end = stmt->expression->tac_end;

        label_jump->next = temp_expression_start;
        temp_expression_start->prev = label_jump;

        jump_false->type = MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP;

        jump_false->identifier1 = copy_string(temp_expression_end->identifier1);

        jump->type = MCC_TAC_ELEMENT_TYPE_UNCONDITIONAL_JUMP;
        jump->jump = label_jump;

        temp_expression_end->next = jump_false;
        jump_false->prev = temp_expression_end;

        jump_false->jump = label_jump_false;

        jump_false->next = labelHelp;
        labelHelp->prev = jump_false;
        labelHelp->next = temp_stmt_start;
        temp_stmt_start->prev = labelHelp;


        temp_stmt_end->next = jump;
        jump->prev = temp_stmt_end;

        jump->next = label_jump_false;
        label_jump_false->prev = jump;

        stmt->tac_end = label_jump_false;

        stmt->tac_start = label_jump;

    } else {

        tac_list *jump = tac_new_list();
        tac_list *label_jump = tac_new_list();

        jump->type = MCC_TAC_ELEMENT_TYPE_UNCONDITIONAL_JUMP;
        jump->jump = label_jump;
        label_jump->type = MCC_TAC_ELEMENT_TYPE_LABEL;
        label_jump->identifier1 = new_string("L%d", l_counter++);


        tac_list *prev_end = stmt->statement->tac_end;

        tac_list *label_end;
        if (prev_end->type == MCC_TAC_ELEMENT_TYPE_LABEL) {
            label_end = prev_end;
        } else {
            label_end = tac_new_list();
            label_end->type = MCC_TAC_ELEMENT_TYPE_LABEL;
            label_end->identifier1 = new_string("L%d", l_counter++);
        }

        tac_list *temp_expression_end = stmt->expression->tac_end;
        tac_list *temp_expression_start = stmt->expression->tac_start;

        tac_list *temp_stmt_start = stmt->statement->tac_start;
        tac_list *temp_stmt_end = stmt->statement->tac_end;

        label_jump->next = temp_expression_start;
        temp_expression_start->prev = label_jump;

        temp_expression_end->next = temp_stmt_start;
        temp_stmt_start->prev = temp_expression_end;

        temp_stmt_end->next = jump;
        jump->prev = temp_stmt_end;

        jump->next = label_end;
        label_end->prev = jump;

        stmt->tac_start = label_jump;
        stmt->tac_end = label_end;
        add_jump(stmt->tac_start, stmt->tac_end,
                 label_end);
	}
}

static void tac_declaration(struct mCc_ast_declaration *declaration, void *data)
{
	assert(declaration);
	assert(data);

	if (declaration->type == MCC_AST_DECLARATION_TYPE_SINGLE) {
		tac_list *elem = tac_new_list();
		elem->type = MCC_TAC_ELEMENT_TYPE_PARAMETER_SETUP;

		elem->identifier1 =
			copy_string(declaration->identifier->renamed);
		elem->decl_lit_type = declaration->literal;
		elem->param_size = 1;
		declaration->tac_start = elem;
		declaration->tac_end = elem;
	} else if (declaration->type == MCC_AST_DECLARATION_TYPE_ARRAY) {
		tac_list *elem = tac_new_list();
		elem->type = MCC_TAC_ELEMENT_TYPE_PARAMETER_SETUP;

		elem->identifier1 =
			copy_string(declaration->array_identifier->renamed);
		elem->decl_lit_type = declaration->literal;
		elem->param_size = declaration->numerator;
		declaration->tac_start = elem;
		declaration->tac_end = elem;
	}
}

static void tac_parameter(struct mCc_ast_parameter *parameter, void *data)
{
	assert(parameter);
	assert(data);

	if (parameter->counter == 0) {
		tac_list *elem = tac_new_list();
		parameter->tac_start = elem;
		parameter->tac_end = elem;
	} else {
		ast_declaration *prev = &parameter->declaration[0];
		for (int i = 1; i < parameter->counter; i++) {
			tac_list *temp_start =
				parameter->declaration[i].tac_start;
			tac_list *temp_end = prev->tac_end;
			temp_start->prev = temp_end;
			temp_end->next = temp_start;
			prev = &parameter->declaration[i];
		}
		parameter->tac_start = parameter->declaration[0].tac_start;
		parameter->tac_end =
			parameter->declaration[parameter->counter - 1].tac_end;
	}
}

static void tac_argument(struct mCc_ast_argument *argument, void *data)
{
	assert(argument);
	assert(data);
}


/*------------------ FUNCTION AVAILABLE FROM OUTSIDE ------------------*/

struct mCc_tac_list *mCc_tac_generate(struct mCc_ast_function_def_array *f)
{
	assert(f);

	head = tac_new_list();
	tail = head;

	v_counter = 0;
	l_counter = 0;

	struct mCc_ast_visitor visitor = tac_visitor(f);

	mCc_ast_visit_function_def_array(f, &visitor);

	while (head->prev != NULL)
		head = head->prev;
	struct mCc_tac_list *temp = head;
	while (true) {
		if (temp->type == MCC_TAC_ELEMENT_TYPE_UNKNOWN) {
			struct mCc_tac_list *temp2 = temp->next;
			if (temp->prev != NULL)
				temp->prev->next = temp->next;

			if (temp->next != NULL)
				temp->next->prev = temp->prev;
			if (temp == head)
				head = temp2;
			free(temp);
			temp = temp2;
		} else
			temp = temp->next;
		if (temp == NULL)
			break;
	}
	return head;
}

void mCc_tac_delete(struct mCc_tac_list *head)
{
	while (head->prev != NULL)
		head = head->prev;

	struct mCc_tac_list *current = head;
	while (current != NULL) {
		struct mCc_tac_list *next = current->next;
		switch (current->type) {
            case MCC_TAC_ELEMENT_TYPE_COPY_IDENTIFIER:
                free(current->copy_identifier);
                break;
            case MCC_TAC_ELEMENT_TYPE_COPY_LITERAL:
                if (current->literal_type
                    == MCC_TAC_LITERAL_TYPE_STRING)
                    free(current->s_literal);
                break;
            case MCC_TAC_ELEMENT_TYPE_BINARY:
                free(current->lhs);
                free(current->rhs);
                break;
            case MCC_TAC_ELEMENT_TYPE_UNARY:
                free(current->unary_identifier);
                break;
            case MCC_TAC_ELEMENT_TYPE_LOAD:
            case MCC_TAC_ELEMENT_TYPE_STORE:
                free(current->identifier2);
                free(current->identifier3);
                break;
            default:
                break;
		}
		if (current->identifier1 != NULL
		    && strlen(current->identifier1) > 0)
			free(current->identifier1);
		free(current);
		current = next;
	}
}

void mCc_tac_print_reverse(FILE *out, struct mCc_tac_list *head)
{
    assert(out);
    assert(head);

    struct mCc_tac_list *current = head;
    while (current->next != NULL) {
        current = current->next;
    }

    while (current != NULL) {
        print_tac_elem(out, current);
        fprintf(out, "\n");
        current = current->prev;
    }
}

void mCc_tac_print(FILE *out, struct mCc_tac_list *head)
{
	assert(out);
	assert(head);

	struct mCc_tac_list *current = head;
	while (current != NULL) {
		struct mCc_tac_list *next = current->next;
		print_tac_elem(out, current);
		fprintf(out, "\n");
		current = next;
	}
}

void print_tac_elem(FILE *out, tac_list *current)
{
	assert(current);
	assert(out);

	switch (current->type) {
	case (MCC_TAC_ELEMENT_TYPE_UNKNOWN):
		fprintf(out, "UNKNOWN:");
		break;
	case (MCC_TAC_ELEMENT_TYPE_COPY_LITERAL):
		switch (current->literal_type) {
            case (MCC_TAC_LITERAL_TYPE_INT):
                fprintf(out, "COPY: %s = %ld", current->identifier1,
                    current->i_literal);
                break;
            case (MCC_TAC_LITERAL_TYPE_FLOAT):
                fprintf(out, "COPY: %s = %f", current->identifier1,
                    current->f_literal);
                break;
            case (MCC_TAC_LITERAL_TYPE_BOOL):
                if (current->b_literal)
                    fprintf(out, "COPY: %s = true", current->identifier1);
                else
                    fprintf(out, "COPY: %s = false", current->identifier1);
                break;
            case (MCC_TAC_LITERAL_TYPE_STRING):
                fprintf(out, "COPY: %s = %s", current->identifier1,
                    current->s_literal);
                break;
		}
		break;
	case (MCC_TAC_ELEMENT_TYPE_COPY_IDENTIFIER):
		fprintf(out, "COPY: %s = %s", current->identifier1,
			current->copy_identifier);
		break;
	case (MCC_TAC_ELEMENT_TYPE_UNARY):
		fprintf(out, "UNARY: ");
		switch (current->unary_op_type) {
            case (MCC_TAC_OPERATION_TYPE_MINUS):
                fprintf(out, " - ");
                break;
            case (MCC_TAC_OPERATION_TYPE_FAC):
                fprintf(out, " ! ");
                break;
            case (MCC_TAC_OPERATION_TYPE_PLUS):
            case (MCC_TAC_OPERATION_TYPE_DIVISION):
            case (MCC_TAC_OPERATION_TYPE_MULTIPLY):
            case (MCC_TAC_OPERATION_TYPE_AND):
            case (MCC_TAC_OPERATION_TYPE_EQ):
            case (MCC_TAC_OPERATION_TYPE_GE):
            case (MCC_TAC_OPERATION_TYPE_GT):
            case (MCC_TAC_OPERATION_TYPE_LE):
            case (MCC_TAC_OPERATION_TYPE_LT):
            case (MCC_TAC_OPERATION_TYPE_NE):
            case (MCC_TAC_OPERATION_TYPE_OR):
                break;
		}
		fprintf(out, "%s %s", current->identifier1,
			current->unary_identifier);
		break;
	case (MCC_TAC_ELEMENT_TYPE_BINARY):
		fprintf(out, "BINARY: ");
		switch (current->binary_op_type) {
            case (MCC_TAC_OPERATION_TYPE_PLUS):
                fprintf(out, " + ");
                break;
            case (MCC_TAC_OPERATION_TYPE_DIVISION):
                fprintf(out, " / ");
                break;
            case (MCC_TAC_OPERATION_TYPE_MULTIPLY):
                fprintf(out, " * ");
                break;
            case (MCC_TAC_OPERATION_TYPE_MINUS):
                fprintf(out, " - ");
                break;
            case (MCC_TAC_OPERATION_TYPE_AND):
                fprintf(out, " && ");
                break;
            case (MCC_TAC_OPERATION_TYPE_EQ):
                fprintf(out, " == ");
                break;
            case (MCC_TAC_OPERATION_TYPE_GE):
                fprintf(out, " >= ");
                break;
            case (MCC_TAC_OPERATION_TYPE_GT):
                fprintf(out, " > ");
                break;
            case (MCC_TAC_OPERATION_TYPE_LE):
                fprintf(out, " <= ");
                break;
            case (MCC_TAC_OPERATION_TYPE_LT):
                fprintf(out, " < ");
                break;
            case (MCC_TAC_OPERATION_TYPE_NE):
                fprintf(out, " != ");
                break;
            case (MCC_TAC_OPERATION_TYPE_OR):
                fprintf(out, " || ");
                break;
            case (MCC_TAC_OPERATION_TYPE_FAC):
                break;
		}
		fprintf(out, "%s %s %s", current->identifier1, current->lhs,
			current->rhs);
		break;
	case (MCC_TAC_ELEMENT_TYPE_UNCONDITIONAL_JUMP):
		fprintf(out, "UNCONDITIONAL JUMP: %s",
			current->jump->identifier1);
		break;
	case (MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP):
        if (current->jump != NULL)
		    fprintf(out, "CONDITIONAL JUMP: %s %s", current->identifier1,
			current->jump->identifier1);
        else
            fprintf(out, "CONDITIONAL JUMP: %s %s", current->identifier1,
                    "(null)");
		break;
	case (MCC_TAC_ELEMENT_TYPE_LABEL):
		fprintf(out, "LABEL: %s", current->identifier1);
		break;
	case (MCC_TAC_ELEMENT_TYPE_PARAMETER_SETUP_CALL):
        fprintf(out, "CALL_PARAM: %s %d", current->identifier1,
                current->param_size);
        break;
	case (MCC_TAC_ELEMENT_TYPE_PARAMETER_SETUP):
		fprintf(out, "PARAM: %s %d", current->identifier1,
			current->param_size);
		break;
	case (MCC_TAC_ELEMENT_TYPE_PROCEDURE_CALL):
		fprintf(out, "CALL: %s, %d", current->identifier1,
			current->num_function_param);
		break;
	case (MCC_TAC_ELEMENT_TYPE_LOAD):
		fprintf(out, "LOAD: %s %s %s", current->identifier1,
			current->identifier2, current->identifier3);
		break;
	case (MCC_TAC_ELEMENT_TYPE_STORE):
		fprintf(out, "STORE: %s %s %s", current->identifier1,
			current->identifier2, current->identifier3);
		break;
	case (MCC_TAC_ELEMENT_TYPE_ADDRESS_ASSIGNMENT):
		fprintf(out, "ADDRESS ASSIGNMENT");
		break;
	case (MCC_TAC_ELEMENT_TYPE_POINTER_ASSIGNMENT):
		fprintf(out, "POINTER ASSIGNMENT");
		break;
	case (MCC_TAC_ELEMENT_TYPE_FUNCTION_START):
		fprintf(out, "<FUNCTION START> %s", current->identifier1);
		break;
	case (MCC_TAC_ELEMENT_TYPE_FUNCTION_END):
		fprintf(out, "<FUNCTION END> %s", current->identifier1);
		break;
	case (MCC_TAC_ELEMENT_TYPE_RETURN):
		fprintf(out, "RETURN %s", current->identifier1);
		break;
	}
}

struct mCc_tac_list *get_at(struct mCc_tac_list *head, int index)
{
	assert(head);
	while (index-- > 0 && head != NULL)
		head = head->next;

	return head;
}
