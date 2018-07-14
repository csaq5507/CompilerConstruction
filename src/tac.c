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

static void tac_single_expression(struct mCc_ast_single_expression *expression,
				  void *data)
{
	assert(expression);
	assert(data);

	tac_list *elem = tac_new_list();

	elem->identifier1 = new_string("reg_%d", v_counter++);

	if (expression->type == MCC_AST_SINGLE_EXPRESSION_TYPE_LITERAL) {
		elem->type = MCC_TAC_ELEMENT_TYPE_COPY_LITERAL;
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

		elem->identifier2 =
			copy_string(expression->identifier->renamed);

		tac_list *temp = expression->identifier_expression->tac_end;

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
		elem->type = MCC_TAC_ELEMENT_TYPE_UNARY;
		switch (expression->unary_operator) {
		case (MCC_AST_UNARY_OP_FAC):
			elem->unary_op_type = MCC_TAC_OPERATION_TYPE_FAC;
			break;
		case (MCC_AST_UNARY_OP_NEGATION):
			elem->unary_op_type = MCC_TAC_OPERATION_TYPE_MINUS;
			break;
		}
		tac_list *temp = expression->unary_expression->tac_end;

		elem->unary_identifier = copy_string(temp->identifier1);

		expression->tac_start = expression->unary_expression->tac_start;
		elem->prev = temp;
		temp->next = elem;
		expression->tac_end = elem;
	} else if (expression->type == MCC_AST_SINGLE_EXPRESSION_TYPE_PARENTH) {
		expression->tac_start = expression->expression->tac_start;
		expression->tac_end = expression->expression->tac_end;
		free(elem->identifier1);
		free(elem);
	}
}

static void tac_expression(struct mCc_ast_expression *expression, void *data)
{
	assert(expression);
	assert(data);


	if (expression->type == MCC_AST_EXPRESSION_TYPE_SINGLE) {
		expression->tac_start = expression->single_expr->tac_start;
		expression->tac_end = expression->single_expr->tac_end;
	} else if (expression->type == MCC_AST_EXPRESSION_TYPE_BINARY) {
		tac_list *elem = tac_new_list();

		elem->identifier1 = new_string("reg_%d", v_counter++);

		elem->type = MCC_TAC_ELEMENT_TYPE_BINARY;

		tac_list *temp_lhs_end = expression->lhs->tac_end;
		tac_list *temp_lhs_star = expression->lhs->tac_start;

		elem->lhs = copy_string(temp_lhs_end->identifier1);

		tac_list *temp_rhs_end = expression->rhs->tac_end;
		tac_list *temp_rhs_start = expression->rhs->tac_start;

		elem->rhs = copy_string(temp_rhs_end->identifier1);

		switch (expression->op) {
		case (MCC_AST_BINARY_OP_ADD):
			elem->binary_op_type = MCC_TAC_OPERATION_TYPE_PLUS;
			break;
		case (MCC_AST_BINARY_OP_DIV):
			elem->binary_op_type = MCC_TAC_OPERATION_TYPE_DIVISION;
			break;
		case (MCC_AST_BINARY_OP_MUL):
			elem->binary_op_type = MCC_TAC_OPERATION_TYPE_MULTIPLY;
			break;
		case (MCC_AST_BINARY_OP_SUB):
			elem->binary_op_type = MCC_TAC_OPERATION_TYPE_MINUS;
			break;
		case (MCC_AST_BINARY_OP_AND):
			elem->binary_op_type = MCC_TAC_OPERATION_TYPE_AND;
			break;
		case (MCC_AST_BINARY_OP_EQ):
			elem->binary_op_type = MCC_TAC_OPERATION_TYPE_EQ;
			break;
		case (MCC_AST_BINARY_OP_GE):
			elem->binary_op_type = MCC_TAC_OPERATION_TYPE_GE;
			break;
		case (MCC_AST_BINARY_OP_GT):
			elem->binary_op_type = MCC_TAC_OPERATION_TYPE_GT;
			break;
		case (MCC_AST_BINARY_OP_LE):
			elem->binary_op_type = MCC_TAC_OPERATION_TYPE_LE;
			break;
		case (MCC_AST_BINARY_OP_LT):
			elem->binary_op_type = MCC_TAC_OPERATION_TYPE_LT;
			break;
		case (MCC_AST_BINARY_OP_NEQ):
			elem->binary_op_type = MCC_TAC_OPERATION_TYPE_NE;
			break;
		case (MCC_AST_BINARY_OP_OR):
			elem->binary_op_type = MCC_TAC_OPERATION_TYPE_OR;
			break;
		}
		temp_lhs_end->next = temp_rhs_start;
		temp_rhs_start->prev = temp_lhs_end;
		temp_rhs_end->next = elem;
		elem->prev = temp_rhs_end;
		expression->tac_start = temp_lhs_star;
		expression->tac_end = elem;
	}
}

static void tac_call_expression(struct mCc_ast_call_expr *expression,
				void *data)
{
	assert(expression);
	assert(data);

	/* TODO for array and string
	 MCC_TAC_ELEMENT_TYPE_ADDRESS_ASSIGNMENT,    x = &y
	 since they are called by reference not by value
	*/

	tac_list *elem = tac_new_list();

	elem->type = MCC_TAC_ELEMENT_TYPE_PROCEDURE_CALL;

	elem->identifier1 = copy_string(expression->identifier->renamed);

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
			for (int i = 0; i < expression->arguments->counter; i++) {
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
			for (int i = 0; i < expression->arguments->counter; i++) {
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

static void tac_compound_stmt(struct mCc_ast_compound_stmt *c_stmt, void *data)
{
	assert(c_stmt);
	assert(data);

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

	tac_list *elem = tac_new_list();
	elem->type = MCC_TAC_ELEMENT_TYPE_RETURN;

	if (stmt->expression != NULL) {
		tac_list *temp = stmt->expression->tac_end;

		elem->identifier1 = copy_string(temp->identifier1);

		elem->prev = temp;
		temp->next = elem;

		stmt->tac_start = stmt->expression->tac_start;
		stmt->tac_end = elem;
	} else {
		stmt->tac_start = elem;
		stmt->tac_end = elem;
	}
}

static void tac_ass_stmt(struct mCc_ast_assignment *stmt, void *data)
{
	assert(stmt);
	assert(data);

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

static void tac_if_stmt(struct mCc_ast_if_stmt *stmt, void *data)
{
	assert(stmt);
	assert(data);

	tac_list *jump_false = tac_new_list();
	tac_list *jump = tac_new_list();
	tac_list *label = tac_new_list();
	tac_list *labelHelp = tac_new_list();


	labelHelp->type = MCC_TAC_ELEMENT_TYPE_LABEL;
	labelHelp->identifier1 = new_string("L%d", l_counter++);

	tac_list *temp_expression_end = stmt->expression->tac_end;
	jump_false->type = MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP;
	jump->type = MCC_TAC_ELEMENT_TYPE_UNCONDITIONAL_JUMP;

	jump_false->identifier1 = copy_string(temp_expression_end->identifier1);

	temp_expression_end->next = jump_false;
	jump_false->prev = temp_expression_end;

	label->type = MCC_TAC_ELEMENT_TYPE_LABEL;

	label->identifier1 = new_string("L%d", l_counter++);

	if (stmt->else_statement != NULL
	    && stmt->else_statement->type == MCC_AST_COMPOUND_STMT) {
		tac_list *label_end = tac_new_list();
		label_end->type = MCC_TAC_ELEMENT_TYPE_LABEL;
		label_end->identifier1 = new_string("L%d", l_counter++);
		tac_list *temp_else_stmt_start =
			stmt->else_statement->tac_start;
		tac_list *temp_else_stmt_end = stmt->else_statement->tac_end;
		tac_list *temp_stmt_start = stmt->statement->tac_start;
		tac_list *temp_stmt_end = stmt->statement->tac_end;
		jump_false->jump = label;
		jump->jump = label_end;

		jump_false->next = labelHelp;
		labelHelp->prev = jump_false;
		labelHelp->next = temp_stmt_start;
		temp_stmt_start->prev = labelHelp;

		temp_stmt_end->next = jump;
		jump->prev = temp_stmt_end;

		jump->next = label;
		label->prev = jump;

		label->next = temp_else_stmt_start;
		temp_else_stmt_start->prev = label;

		temp_else_stmt_end->next = label_end;
		label_end->prev = temp_else_stmt_end;

		stmt->tac_end = label_end;

	} else if (stmt->else_statement != NULL
		   && stmt->else_statement->type != MCC_AST_COMPOUND_STMT) {
		tac_list *temp_else_stmt_start =
			stmt->else_statement->tac_start;
		tac_list *temp_else_stmt_end = stmt->else_statement->tac_end;
		tac_list *temp_stmt_start = stmt->statement->tac_start;
		tac_list *temp_stmt_end = stmt->statement->tac_end;
		jump_false->jump = label;
		jump->jump = temp_else_stmt_end;

		jump_false->next = labelHelp;
		labelHelp->prev = jump_false;
		labelHelp->next = temp_stmt_start;
		temp_stmt_start->prev = labelHelp;

		temp_stmt_end->next = jump;
		jump->prev = temp_stmt_end;

		jump->next = label;
		label->prev = jump;

		label->next = temp_else_stmt_start;
		temp_else_stmt_start->prev = label;

		stmt->tac_end = temp_else_stmt_end;
	} else {
		free(jump);
		tac_list *temp_stmt_start = stmt->statement->tac_start;
		tac_list *temp_stmt_end = stmt->statement->tac_end;
		jump_false->jump = label;

		jump_false->next = labelHelp;
		labelHelp->prev = jump_false;
		labelHelp->next = temp_stmt_start;
		temp_stmt_start->prev = labelHelp;

		temp_stmt_end->next = label;
		label->prev = temp_stmt_end;

		stmt->tac_end = label;
	}
	stmt->tac_start = stmt->expression->tac_start;
}

static void tac_while_stmt(struct mCc_ast_while_stmt *stmt, void *data)
{
	assert(stmt);
	assert(data);

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
			fprintf(out, "COPY: %s = %d", current->identifier1,
				current->b_literal);
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
		fprintf(out, "CONDITIONAL JUMP: %s %s", current->identifier1,
			current->jump->identifier1);
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