//
// Created by ivan on 23.05.18.
//

#include <mCc/tac.h>
#include <memory.h>
#include <stdlib.h>
#include <mCc/code_generation.h>
#include <mCc/utils.h>
#include <assert.h>

#define MALLOC(ptr, size)                                                      \
	ptr = malloc(size);                                                    \
	if ((ptr) == NULL) {                                                   \
		printf("Malloc failed for %p of"                               \
		       "size %li",                                             \
		       ptr, (size));                                           \
		return NULL;                                                   \
	}

#define REALLOC(ptr, size)                                                     \
	void *temp = realloc(ptr, size);                                       \
	if (temp == NULL) {                                                    \
		printf("Realloc failed for %p of"                              \
		       "size %li",                                             \
		       ptr, (size));                                           \
		return NULL;                                                   \
	}                                                                      \
	(ptr) = temp;

#define MALLOC_(ptr, size)                                                     \
	ptr = malloc(size);                                                    \
	if ((ptr) == NULL) {                                                   \
		printf("Malloc failed for %p of"                               \
		       "size %li",                                             \
		       ptr, (size));                                           \
		return;                                                        \
	}

#define REALLOC_(ptr, size)                                                    \
	void *temp = realloc(ptr, size);                                       \
	if (temp == NULL) {                                                    \
		printf("Realloc failed for %p of"                              \
		       "size %li",                                             \
		       ptr, (size));                                           \
		return;                                                        \
	}                                                                      \
	(ptr) = temp;

#define CHECK_BIT(var, pos) ((var) & (1 << (pos)))

#define NEW_SINGLE_LINE                                                        \
	struct mCc_assembly_line *retval;                                      \
	MALLOC(retval, sizeof(struct mCc_assembly_line))                       \
	retval->next = NULL;

#define NEW_DOUBLE_LINE                                                        \
	NEW_SINGLE_LINE                                                        \
	struct mCc_assembly_line *temp;                                        \
	MALLOC(temp, sizeof(struct mCc_assembly_line))                         \
	retval->next = temp;                                                   \
	temp->next = NULL;                                                     \
	temp->prev = retval;

#define NEW_TRIPLE_LINE                                                        \
	NEW_DOUBLE_LINE                                                        \
	struct mCc_assembly_line *temp1;                                       \
	MALLOC(temp1, sizeof(struct mCc_assembly_line))                        \
	temp->next = temp1;                                                    \
	temp1->next = NULL;                                                    \
	temp1->prev = temp;

#define NEW_QUADRUPLE_LINE                                                     \
	NEW_TRIPLE_LINE                                                        \
	struct mCc_assembly_line *temp2;                                       \
	MALLOC(temp2, sizeof(struct mCc_assembly_line))                        \
	temp1->next = temp2;                                                   \
	temp2->prev = temp1;                                                   \
	temp2->next = NULL;

static enum mCc_tac_literal_type
ast_to_tac_literal_type(enum mCc_ast_literal_type type);
static struct mCc_assembly_line *
operation_plus(struct mCc_assembly_line *retval, struct mCc_tac_list *tac,
	       struct mCc_assembly_line *current);
static struct mCc_assembly_line *
operation_minus(struct mCc_assembly_line *retval, struct mCc_tac_list *tac,
		struct mCc_assembly_line *current);
static struct mCc_assembly_line *
operation_mul(struct mCc_assembly_line *retval, struct mCc_tac_list *tac,
	      struct mCc_assembly_line *current);
static struct mCc_assembly_line *
operation_div(struct mCc_assembly_line *retval, struct mCc_tac_list *tac,
	      struct mCc_assembly_line *current);


void init_globals()
{

	string_label_idx = 0;
	float_label_idx = 0;
	label_idx = 0;
	is_float_condition = false;
	jump_cond = -1;
	skip = 0;
	stack = NULL;
	param_stack = NULL;
	MALLOC_(lost_registers, sizeof(struct lost_regs))
	lost_registers->counter = 0;
	lost_registers->reg = NULL;
	MALLOC_(label, sizeof(struct labels))
	label->counter = 0;
	label->l = NULL;
	MALLOC_(registers, sizeof(struct regs))
	free_all_registers();
	no_more_registers = false;
}


struct mCc_assembly *mCc_assembly_generate(struct mCc_tac_list *tac,
					   char *filename)
{
	// init vars
	init_globals();

    struct mCc_assembly *assembly;
	MALLOC(assembly, sizeof(struct mCc_assembly));
	assembly->counter = 0;
    struct mCc_assembly_line *current = assembly->head;

    // generate labels, builtins, first line etc.
    assembly->head = mcc_assembly_generate_labels(tac, filename);
	// generate code

	do {
		assembly->counter++;
		while (skip > 0) {
			tac = tac->next;
			skip--;
		}
		while (current->next != NULL)
			current = current->next;

		while (lost_registers->counter > 0) {
			move_line_to_end(
				current, tac,
				lost_registers->reg[lost_registers->counter - 1]
					.identifier);
			lost_registers->counter--;
			while (current->next != NULL)
				current = current->next;
		}
		if (no_more_registers) {
			tac = tac->prev;
			current = consume_registers(tac, current);
			no_more_registers = false;
		}
		struct mCc_assembly_line *temp = NULL;
		switch (tac->type) {
		case MCC_TAC_ELEMENT_TYPE_COPY_LITERAL:
			temp = mCc_assembly_copy_literal(tac, current);
			break;
		case MCC_TAC_ELEMENT_TYPE_COPY_IDENTIFIER:
			temp = mCc_assembly_copy_identifier(tac, current);
			break;
		case MCC_TAC_ELEMENT_TYPE_UNARY:
		case MCC_TAC_ELEMENT_TYPE_BINARY:
			temp = mCc_assembly_operation(tac, current);
			break;
		case MCC_TAC_ELEMENT_TYPE_UNCONDITIONAL_JUMP:
			temp = mCc_assembly_jump(tac, current);
			break;
		case MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP:
			temp = mCc_assembly_conditional_jump(tac, current);
			break;
		case MCC_TAC_ELEMENT_TYPE_LABEL:
			temp = mCc_assembly_create_label(tac, current);
			break;
		case MCC_TAC_ELEMENT_TYPE_PARAMETER_SETUP_CALL:
			temp = mCc_assembly_call_param(tac, current);
			break;
		case MCC_TAC_ELEMENT_TYPE_PROCEDURE_CALL:
			temp = mCc_assembly_procedure_call(tac, current);
			break;
		case MCC_TAC_ELEMENT_TYPE_LOAD:
			temp = mCc_assembly_load(tac, current);
			break;
		case MCC_TAC_ELEMENT_TYPE_STORE:
			temp = mCc_assembly_store(tac, current);
			break;
		case MCC_TAC_ELEMENT_TYPE_FUNCTION_START:
			temp = mCc_assembly_function_start(tac, current);
			break;
		case MCC_TAC_ELEMENT_TYPE_FUNCTION_END:
			temp = mCc_assembly_function_end(tac, current);
			break;
		case MCC_TAC_ELEMENT_TYPE_RETURN:
			temp = mCc_assembly_function_return(tac, current);
			break;
		case MCC_TAC_ELEMENT_TYPE_UNKNOWN:
		case MCC_TAC_ELEMENT_TYPE_ADDRESS_ASSIGNMENT:
		case MCC_TAC_ELEMENT_TYPE_POINTER_ASSIGNMENT:
		case MCC_TAC_ELEMENT_TYPE_PARAMETER_SETUP:
			break;
		}
		if (temp == NULL) {
			tac = tac->next;
			continue;
		}
		while (current->next != NULL)
			current = current->next;
		temp->prev = current;
		current->next = temp;
		tac = tac->next;
	} while (tac != NULL);
	delete_stack();
	return assembly;
}

struct mCc_assembly_line *consume_registers(struct mCc_tac_list *tc,
					    struct mCc_assembly_line *current)
{
	assert(current);

	current = current->prev;
	free(current->next);
	current->next = NULL;
	struct mCc_tac_list *temp_tac = tc->next;
	struct mCc_assembly_line *temp;
	while (true) {
		temp = NULL;
		switch (temp_tac->type) {
		case MCC_TAC_ELEMENT_TYPE_COPY_IDENTIFIER:
			if (!is_register(temp_tac->identifier1)
			    && !is_float(temp_tac->copy_identifier))
				temp = mCc_assembly_copy_identifier(temp_tac,
								    current);
			break;
		case MCC_TAC_ELEMENT_TYPE_UNARY:
		case MCC_TAC_ELEMENT_TYPE_BINARY:
			temp = mCc_assembly_operation(temp_tac, current);
			break;
		case MCC_TAC_ELEMENT_TYPE_PARAMETER_SETUP_CALL:
			if (strcmp(temp_tac->identifier1, "result") != 0
			    && !is_float(temp_tac->identifier1)
			    && (strcmp(get_register(temp_tac->identifier1),
				       "temperoria")
				!= 0))
				temp = mCc_assembly_call_param(temp_tac,
							       current);
			break;
		case MCC_TAC_ELEMENT_TYPE_STORE:
			temp = mCc_assembly_store(temp_tac, current);
			break;
		case MCC_TAC_ELEMENT_TYPE_COPY_LITERAL:
		case MCC_TAC_ELEMENT_TYPE_PROCEDURE_CALL:
		case MCC_TAC_ELEMENT_TYPE_LOAD:
		case MCC_TAC_ELEMENT_TYPE_UNCONDITIONAL_JUMP:
		case MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP:
		case MCC_TAC_ELEMENT_TYPE_LABEL:
		case MCC_TAC_ELEMENT_TYPE_FUNCTION_START:
		case MCC_TAC_ELEMENT_TYPE_FUNCTION_END:
		case MCC_TAC_ELEMENT_TYPE_RETURN:
		case MCC_TAC_ELEMENT_TYPE_UNKNOWN:
		case MCC_TAC_ELEMENT_TYPE_ADDRESS_ASSIGNMENT:
		case MCC_TAC_ELEMENT_TYPE_POINTER_ASSIGNMENT:
		case MCC_TAC_ELEMENT_TYPE_PARAMETER_SETUP:
			break;
		}
		if (temp != NULL)
			break;
		temp_tac = temp_tac->next;
	}
	tc = temp_tac->prev;
	tc->next = temp_tac->next;
	switch (temp_tac->type) {
	case MCC_TAC_ELEMENT_TYPE_COPY_IDENTIFIER:
		free(temp_tac->copy_identifier);
		break;
	case MCC_TAC_ELEMENT_TYPE_COPY_LITERAL:
		if (temp_tac->literal_type == MCC_TAC_LITERAL_TYPE_STRING)
			free(temp_tac->s_literal);
		break;
	case MCC_TAC_ELEMENT_TYPE_BINARY:
		free(temp_tac->lhs);
		free(temp_tac->rhs);
		break;
	case MCC_TAC_ELEMENT_TYPE_UNARY:
		free(temp_tac->unary_identifier);
		break;
	case MCC_TAC_ELEMENT_TYPE_LOAD:
	case MCC_TAC_ELEMENT_TYPE_STORE:
		free(temp_tac->identifier2);
		free(temp_tac->identifier3);
		break;
	default:
		break;
	}
	if (temp_tac->identifier1 != NULL && strlen(temp_tac->identifier1) > 0)
		free(temp_tac->identifier1);
	free(temp_tac);
	current->next = temp;
	temp->prev = current;
	while (current->next != NULL)
		current = current->next;
	return current;
}

void mCc_assembly_delete(struct mCc_assembly *assembly)
{
	struct mCc_assembly_line *current = assembly->head;
	while (current != NULL) {
		if (current->type != MCC_ASSEMBLY_BUILTIN)
			free(current->instruction);

		if (current->next)
			current = current->next;
		else {
			free(current);
			break;
		}
		free(current->prev);
	}
	free(assembly);
	for (int i = 0; i < label->counter; i++)
		free(label->l[i].value);
	free(label->l);
}

void mCc_assembly_print(FILE *out, struct mCc_assembly *ass)
{
	struct mCc_assembly_line *current = ass->head;
	while (current != NULL) {
		fprintf(out, "%s\n", current->instruction);
		current = current->next;
	}
}


/************************************************/
/************************************************/

struct mCc_assembly_line *
mCc_assembly_create_label(struct mCc_tac_list *tac,
			  struct mCc_assembly_line *current)
{
	assert(current);

	NEW_SINGLE_LINE
	retval->type = MCC_ASSEMBLY_LABEL;
	retval->next = NULL;
	retval->instruction = new_string("%s:", get_label(tac->identifier1));
	return retval;
}

struct mCc_assembly_line *mCc_assembly_store(struct mCc_tac_list *tac,
					     struct mCc_assembly_line *current)
{
	assert(current);

	if (get_pos(tac->identifier1) > 0) {
		NEW_QUADRUPLE_LINE
		retval->type = MCC_ASSEMBLY_MOV;

		retval->instruction = new_string("\tmovl\t%d(%s), %s",
						 get_pos(tac->identifier1),
						 "%ebp", get_register("temp"));
		temp->type = MCC_ASSEMBLY_MUL;
		temp->instruction = new_string("\timull\t$%d, %s",
					       get_var(tac->identifier1)->size,
					       get_register(tac->identifier2));
		temp1->type = MCC_ASSEMBLY_ADD;
		temp1->instruction = new_string("\taddl\t%s, %s",
						get_register(tac->identifier2),
						get_register("temp"));
		temp2->type = MCC_ASSEMBLY_MOV;
		if (is_float(tac->identifier3)) {
			top_float_register(tac->identifier3, current);
                temp2->instruction = new_string(
                        "\tfstp\t(%s)", get_register("temp"));
                free_register("temp");
                free_register(tac->identifier2);
                pop_float_register();

		} else {
			temp2->instruction =
				new_string("\tmovl\t%s, (%s)",
					   get_register(tac->identifier3),
					   get_register("temp"));
			free_register("temp");
			free_register(tac->identifier2);
			free_register(tac->identifier3);
		}
		return retval;
	} else {
		NEW_SINGLE_LINE
		if (is_float(tac->identifier3)) {
			top_float_register(tac->identifier3, current);
            retval->instruction = new_string(
                "\tfstps\t%d(%s,%s,%d)",
                get_pos(tac->identifier1), "%ebp",
                get_register(tac->identifier2),
                get_var(tac->identifier1)->size);
            pop_float_register();
		} else
			retval->instruction =
				new_string("\tmovl\t%s, %d(%s,%s,%d)",
					   get_register(tac->identifier3),
					   get_pos(tac->identifier1), "%ebp",
					   get_register(tac->identifier2),
					   get_var(tac->identifier1)->size);

		retval->type = MCC_ASSEMBLY_MOV;
		free_register(tac->identifier2);
		free_register(tac->identifier3);
		return retval;
	}
}

struct mCc_assembly_line *mCc_assembly_load(struct mCc_tac_list *tac,
					    struct mCc_assembly_line *current)
{
	assert(current);

	if (get_pos(tac->identifier2) > 0) {
		NEW_QUADRUPLE_LINE
		retval->type = MCC_ASSEMBLY_MOV;

		retval->instruction = new_string(
			"\tmovl\t%d(%s), %s", get_pos(tac->identifier2), "%ebp",
			get_register(tac->identifier1));
		temp->type = MCC_ASSEMBLY_MUL;

		temp->instruction = new_string("\timull\t$%d, %s",
					       get_var(tac->identifier2)->size,
					       get_register(tac->identifier3));
		temp1->type = MCC_ASSEMBLY_ADD;
		temp1->instruction = new_string("\taddl\t%s, %s",
						get_register(tac->identifier3),
						get_register(tac->identifier1));
		temp2->type = MCC_ASSEMBLY_MOV;
		if (get_var(tac->identifier2)->type
		    == MCC_TAC_LITERAL_TYPE_FLOAT) {
			temp2->instruction = new_string(
				"\tflds\t(%s)", get_register(tac->identifier1));
			free_register(tac->identifier1);
			push_float_register(tac->identifier1);
		} else {
			temp2->instruction =
				new_string("\tmovl\t(%s), %s",
					   get_register(tac->identifier1),
					   get_register(tac->identifier1));
		}
		free_register(tac->identifier3);
		return retval;
	} else {
		NEW_SINGLE_LINE
		if (get_var(tac->identifier2) != NULL
		    && get_var(tac->identifier2)->type
			       == MCC_TAC_LITERAL_TYPE_FLOAT) {
			push_float_register(tac->identifier1);
			retval->instruction =
				new_string("\tflds\t%d(%s,%s,%d)",
					   get_pos(tac->identifier2), "%ebp",
					   get_register(tac->identifier3),
					   get_var(tac->identifier2)->size);

		} else {
			retval->instruction =
				new_string("\tmovl\t%d(%s,%s,%d), %s",
					   get_pos(tac->identifier2), "%ebp",
					   get_register(tac->identifier3),
					   get_var(tac->identifier2)->size,
					   get_register(tac->identifier1));
		}
		free_register(tac->identifier3);
		return retval;
	}
}

struct mCc_assembly_line *
mcc_assembly_generate_labels(struct mCc_tac_list *temp_tac, char *filename)
{
	builtin = 0;
	NEW_SINGLE_LINE

	struct mCc_assembly_line *current = retval;

	current->type = MCC_ASSEMBLY_FILE;
	current->prev = NULL;
	current->next = NULL;
	current->instruction = new_string("\t.file\t\"%s.mC\"", filename);

	do {
		if (temp_tac->type == MCC_TAC_ELEMENT_TYPE_COPY_LITERAL) {
			if (temp_tac->literal_type
			    == MCC_TAC_LITERAL_TYPE_STRING) {
				current->next =
					mCc_assembly_create_string_label(
						temp_tac, current);
				current->next->prev = current;
			} else if (temp_tac->literal_type
				   == MCC_TAC_LITERAL_TYPE_FLOAT) {
				current->next = mCc_assembly_create_float_label(
					temp_tac, current);
				current->next->prev = current;
			}
		} else if (temp_tac->type
			   == MCC_TAC_ELEMENT_TYPE_PROCEDURE_CALL) {
			if (is_builtin(temp_tac->identifier1)) {
				current->next =
					mCc_assembly_create_builtin_func(
						temp_tac, current);
				if (current->next == NULL)
					current = current->prev;
				else
					current->next->prev = current;
			}
		} else if (temp_tac->type == MCC_TAC_ELEMENT_TYPE_LABEL)
			set_label(temp_tac->identifier1,
				  new_string(".L%d", label_idx++));
		else if (temp_tac->type == MCC_TAC_ELEMENT_TYPE_FUNCTION_START)
			set_label(temp_tac->identifier1,
				  new_string("%s", temp_tac->identifier1));
		temp_tac = temp_tac->next;
		while (current->next != NULL)
			current = current->next;
	} while (temp_tac != NULL);
	return retval;
}

struct mCc_assembly_line *
mCc_assembly_call_param(struct mCc_tac_list *tac,
			struct mCc_assembly_line *current)
{
	assert(current);

	if (strcmp(tac->identifier1, "result") != 0) {
		if (is_float(tac->identifier1)) {
			NEW_DOUBLE_LINE
			if (top_float_register(tac->identifier1, current)) {
				retval->type = MCC_ASSEMBLY_SUB;
				retval->instruction =
					new_string("\tsubl\t$8, %s", "%esp");

				temp->type = MCC_ASSEMBLY_PUSH;
				char temp_push_var[10] = "123456";
				temp_push_var[6] = (char)push_vars++;
				set_var(8, 1, temp_push_var);
				get_var(temp_push_var)->type =
					MCC_TAC_LITERAL_TYPE_FLOAT;
				temp->instruction =
					new_string("\tfstps\t(%s)", "%esp");
				pop_float_register();
				return retval;
			} else {
				return NULL;
			}
		} else {
			NEW_SINGLE_LINE
			retval->type = MCC_ASSEMBLY_PUSH;
			retval->instruction = new_string(
				"\tpushl\t%s", get_register(tac->identifier1));

			char temp_push_var[10] = "123456";
			temp_push_var[6] = (char)push_vars++;
			set_var(4, 1, temp_push_var);
			free_register(tac->identifier1);
			return retval;
		}
	} else
		return NULL;
}

struct mCc_assembly_line *
mCc_assembly_function_return(struct mCc_tac_list *tac,
			     struct mCc_assembly_line *current)
{
	assert(current);

	NEW_DOUBLE_LINE

	struct mCc_tac_list *func_end = tac;
	while (func_end->type != MCC_TAC_ELEMENT_TYPE_FUNCTION_END)
		func_end = func_end->next;
	retval->type = MCC_ASSEMBLY_MOV;

	if (tac->identifier1 == NULL) {
		retval->instruction = new_string("\tnop");
		if (strcmp(func_end->identifier1, "main") == 0) {
			retval->instruction =
				new_string("\tmovl\t$0, %s", "%eax");
		}
	} else {
		if (is_float(tac->identifier1)) {
			top_float_register(tac->identifier1, current);
            retval->instruction = new_string("\tnop");
		} else {
			if (registers->eax != NULL
			    && strcmp(registers->eax, tac->identifier1) == 0)
				retval->instruction = new_string("\tnop");
			else
				retval->instruction = new_string(
					"\tmovl\t%s, %s",
					get_register(tac->identifier1), "%eax");
			free_register(tac->identifier1);
		}
	}
	temp->type = MCC_ASSEMBLY_JMP;
	temp->instruction = new_string("\tjmp\t.RET%s", func_end->identifier1);
	return retval;
}

struct mCc_assembly_line *
mCc_assembly_procedure_call(struct mCc_tac_list *tac,
			    struct mCc_assembly_line *current)
{
	assert(current);

	lose_all_registers();

	if (tac->ret_type != MCC_AST_TYPE_VOID) {
		if (tac->ret_type == MCC_AST_TYPE_FLOAT)
			push_float_register(tac->identifier1);
		else
			get_register(tac->identifier1);
	}

	NEW_SINGLE_LINE
	retval->type = MCC_ASSEMBLY_CALL;
	retval->instruction = new_string("\tcall\t%s", tac->identifier1);
	return retval;
}

struct mCc_assembly_line *
mCc_assembly_create_builtin_func(struct mCc_tac_list *tac,
				 struct mCc_assembly_line *current)
{
	assert(current);

	NEW_SINGLE_LINE

	if (strcmp(tac->identifier1, "print") == 0 && !CHECK_BIT(builtin, 0)) {
		builtin += 1;
		retval->instruction =
			"\t.section\t.rodata\n"
			".LCPRINT:\n"
			"\t.string\t\"%s\"\n"
			"\t.text\n"
			"\t.globl\tprint\n"
			"\t.type\tprint, @function\n"
			"print:\n"
			"\tpushl\t%ebp\n"
			"\tmovl\t%esp, %ebp\n"
			"\tsubl\t$8, %esp\n"
			"\tsubl\t$8, %esp\n"
			"\tpushl\t8(%ebp)\n"
			"\tpushl\t$.LCPRINT\n"
			"\tcall\tprintf\n"
			"\taddl\t$16, %esp\n"
			"\tnop\n"
			"\tleave\n"
			"\tret\n"
			"\t.size\tprint, .-print";
	} else if (strcmp(tac->identifier1, "print_nl") == 0
		   && !CHECK_BIT(builtin, 1)) {
		builtin += 2;
		retval->instruction =
			"\t.globl\tprint_nl\n"
			"\t.type\tprint_nl, @function\n"
			"print_nl:\n"
			"\tpushl\t%ebp\n"
			"\tmovl\t%esp, %ebp\n"
			"\tsubl\t$8, %esp\n"
			"\tsubl\t$12, %esp\n"
			"\tpushl\t$10\n"
			"\tcall\tputchar\n"
			"\taddl\t$16, %esp\n"
			"\tnop\n"
			"\tleave\n"
			"\tret\n"
			"\t.size\tprint_nl, .-print_nl";
	} else if (strcmp(tac->identifier1, "print_int") == 0
		   && !CHECK_BIT(builtin, 2)) {
		builtin += 4;
		retval->instruction =
			"\t.section\t.rodata\n"
			".LCPRINTINT:\n"
			"\t.string\t\"%ld\"\n"
			"\t.text\n"
			"\t.globl\tprint_int\n"
			"\t.type\tprint_int, @function\n"
			"print_int:\n"
			"\tpushl\t%ebp\n"
			"\tmovl\t%esp, %ebp\n"
			"\tsubl\t$8, %esp\n"
			"\tsubl\t$8, %esp\n"
			"\tpushl\t8(%ebp)\n"
			"\tpushl\t$.LCPRINTINT\n"
			"\tcall\tprintf\n"
			"\taddl\t$16, %esp\n"
			"\tnop\n"
			"\tleave\n"
			"\tret\n"
			"\t.size\tprint_int, .-print_int";
	} else if (strcmp(tac->identifier1, "print_float") == 0
		   && !CHECK_BIT(builtin, 3)) {
		builtin += 8;
		retval->instruction =
			"\t.section\t.rodata\n"
			".LCPRINTFLOAT:\n"
			"\t.string\t\"%f\"\n"
			"\t.text\n"
			"\t.globl\tprint_float\n"
			"\t.type\tprint_float, @function\n"
			"print_float:\n"
			"\tpushl\t%ebp\n"
			"\tmovl\t%esp, %ebp\n"
			"\tsubl\t$8, %esp\n"
			"\tflds\t8(%ebp)\n"
			"\tsubl\t$4, %esp\n"
			"\tleal\t-8(%esp), %esp\n"
			"\tfstpl\t(%esp)\n"
			"\tpushl\t$.LCPRINTFLOAT\n"
			"\tcall\tprintf\n"
			"\taddl\t$12, %esp\n"
			"\tnop\n"
			"\tleave\n"
			"\tret\n"
			"\t.size\tprint_float, .-print_float";
	} else if (strcmp(tac->identifier1, "read_int") == 0
		   && !CHECK_BIT(builtin, 4)) {
		builtin += 16;
		retval->instruction =
			"\t.section\t.rodata\n"
			".LCREADINT:\n"
			"\t.string\t\"%ld\"\n"
			"\t.text\n"
			"\t.type\tread_int, @function\n"
			"read_int:\n"
			"\tpushl\t%ebp\n"
			"\tmovl\t%esp, %ebp\n"
			"\tsubl\t$24, %esp\n"
			"\tmovl\t$0, -12(%ebp)\n"
			"\tsubl\t$8, %esp\n"
			"\tleal\t-12(%ebp), %eax\n"
			"\tpushl\t%eax\n"
			"\tpushl\t$.LCREADINT\n"
			"\tcall\t__isoc99_scanf\n"
			"\taddl\t$16, %esp\n"
			"\tmovl\t-12(%ebp), %eax\n"
			"\tleave\n"
			"\tret\n"
			"\t.size\tread_int, .-read_int";
	} else if (strcmp(tac->identifier1, "read_float") == 0
		   && !CHECK_BIT(builtin, 5)) {
		builtin += 32;
		retval->instruction =
			"\t.section\t.rodata\n"
			".LCREADFLOAT:\n"
			"\t.string\t\"%f\"\n"
			"\t.text\n"
			"\t.globl\tread_float\n"
			"\t.type\tread_float, @function\n"
			"read_float:\n"
			"\tpushl\t%ebp\n"
			"\tmovl\t%esp, %ebp\n"
			"\tsubl\t$24, %esp\n"
			"\tfldz\n"
			"\tfstps\t-12(%ebp)\n"
			"\tsubl\t$8, %esp\n"
			"\tleal\t-12(%ebp), %eax\n"
			"\tpushl\t%eax\n"
			"\tpushl\t$.LCREADFLOAT\n"
			"\tcall\t__isoc99_scanf\n"
			"\taddl\t$16, %esp\n"
			"\tflds\t-12(%ebp)\n"
			"\tleave\n"
			"\tret\n"
			"\t.size\tread_float, .-read_float";
	} else {
		free(retval);
		return NULL;
	}
	retval->type = MCC_ASSEMBLY_BUILTIN;
	return retval;
}

struct mCc_assembly_line *
mCc_assembly_create_string_label(struct mCc_tac_list *tac,
				 struct mCc_assembly_line *current)
{
	assert(current);

	NEW_TRIPLE_LINE

	retval->type = MCC_ASSEMBLY_LABEL;
	retval->instruction = new_string(".LC%d:", string_label_idx);
	set_label(tac->identifier1, new_string(".LC%d", string_label_idx++));
	temp->type = MCC_ASSEMBLY_CONSTANT;
	char *tmp = replace(tac->s_literal, "\n", "\\n");
	char *tmp1 = replace(tmp, "#enter#", "\\n");
	temp->instruction = new_string("\t.string \"%s\"", tmp1);
	free(tmp1);
	free(tmp);
	temp1->type = MCC_ASSEMBLY_DIRECTIVE;
	temp1->instruction = new_string("\t.text");
	return retval;
}

struct mCc_assembly_line *
mCc_assembly_create_float_label(struct mCc_tac_list *tac,
				struct mCc_assembly_line *current)
{
	assert(current);

	NEW_DOUBLE_LINE

	retval->type = MCC_ASSEMBLY_LABEL;
	retval->instruction = new_string(".LF%d:", float_label_idx);
	set_label(tac->identifier1, new_string(".LF%d", float_label_idx++));
	temp->type = MCC_ASSEMBLY_CONSTANT;
	temp->instruction = new_string("\t.float %f", tac->f_literal);
	return retval;
}

struct mCc_assembly_line *
mCc_assembly_function_end(struct mCc_tac_list *tac,
			  struct mCc_assembly_line *current)
{
	assert(current);

	NEW_QUADRUPLE_LINE
	retval->type = MCC_ASSEMBLY_LABEL;
	retval->instruction = new_string(".RET%s:", tac->identifier1);
	temp->type = MCC_ASSEMBLY_LEAVE;
	temp->instruction = new_string("\tleave");
	temp1->type = MCC_ASSEMBLY_RET;
	temp1->instruction = new_string("\tret");
	temp2->type = MCC_ASSEMBLY_DIRECTIVE;
	temp2->instruction = new_string("\t.size\t%s, .-%s", tac->identifier1,
					tac->identifier1);

	if (strcmp(tac->identifier1, "main") == 0) {
		struct mCc_assembly_line *temp4;
		MALLOC(temp4, sizeof(struct mCc_assembly_line))
		temp4->next = retval;
		retval->prev = temp4;
		temp4->type = MCC_ASSEMBLY_MOV;
		temp4->instruction = new_string("\tmovl\t$0, %s", "%eax");
		free_all_registers();
		return temp4;
	}
	free_all_registers();
	return retval;
}

struct mCc_assembly_line *
mCc_assembly_function_start(struct mCc_tac_list *tac,
			    struct mCc_assembly_line *current)
{
	assert(current);

	NEW_QUADRUPLE_LINE
	int num_params = tac->num_function_param;
	struct mCc_assembly_line *temp3;
	struct mCc_assembly_line *temp4;
	MALLOC(temp4, sizeof(struct mCc_assembly_line))
	MALLOC(temp3, sizeof(struct mCc_assembly_line))
	temp2->next = temp3;
	temp3->prev = temp2;
	temp3->next = temp4;
	temp4->prev = temp3;
	temp4->next = NULL;
	retval->type = MCC_ASSEMBLY_DIRECTIVE;
	retval->instruction = new_string("\t.globl\t%s", tac->identifier1);
	temp->type = MCC_ASSEMBLY_DIRECTIVE;
	temp->instruction =
		new_string("\t.type\t%s, @function", tac->identifier1);
	temp1->type = MCC_ASSEMBLY_LABEL;
	temp1->instruction = new_string("%s:", get_label(tac->identifier1));
	temp2->type = MCC_ASSEMBLY_PUSH;
	if (strcmp(tac->identifier1, "main") == 0) {
		temp2->instruction = new_string(
			"\tleal\t4(%s), %s\n"
			"\tandl\t$-16, %s\n"
			"\tpushl\t-4(%s)\n"
			"\tpushl\t%s",
			"%esp", "%ecx", "%esp", "%ecx", "%ebp");
		temp3->type = MCC_ASSEMBLY_MOV;
		temp3->instruction = new_string(
			"\tmovl\t%s, %s\n"
			"\tpushl\t%s",
			"%esp", "%ebp", "%ecx");
	} else {
		temp2->instruction = new_string("\tpushl\t%s", "%ebp");
		temp3->type = MCC_ASSEMBLY_MOV;
		temp3->instruction =
			new_string("\tmovl\t%s, %s", "%esp", "%ebp");
	}
	struct mCc_tac_list *temp_tac = tac;
	new_stack();
	set_param_var(8, 1, "#temp_ebp");

	// wrong direction push
	while (temp_tac->type != MCC_TAC_ELEMENT_TYPE_FUNCTION_END) {
		if (temp_tac->type == MCC_TAC_ELEMENT_TYPE_PARAMETER_SETUP) {
			if (num_params > 0) {
				set_param_var(get_literal_size(
						      temp_tac->decl_lit_type),
					      temp_tac->param_size,
					      temp_tac->identifier1);
				num_params--;
			} else {
				set_var(get_literal_size(
						temp_tac->decl_lit_type),
					temp_tac->param_size,
					temp_tac->identifier1);
			}
			get_var(temp_tac->identifier1)->type =
				ast_to_tac_literal_type(
					temp_tac->decl_lit_type);
		}
		temp_tac = temp_tac->next;
	}

	temp4->type = MCC_ASSEMBLY_SUB;
	temp4->instruction =
		new_string("\tsubl\t$%d, %s", get_stack_size(), "%esp");
	return retval;
}

static struct mCc_assembly_line *
operation_plus(struct mCc_assembly_line *retval, struct mCc_tac_list *tac,
	       struct mCc_assembly_line *current)
{
	retval->type = MCC_ASSEMBLY_ADD;
	if (is_float(tac->rhs)) {
		retval->instruction = float_binary_op(tac, "faddp", current);
	} else {
		retval->instruction =
			new_string("\taddl\t%s, %s", get_register(tac->rhs),
				   get_register(tac->lhs));
	}
	return retval;
}

static struct mCc_assembly_line *
operation_minus(struct mCc_assembly_line *retval, struct mCc_tac_list *tac,
		struct mCc_assembly_line *current)
{
	if (tac->type == MCC_TAC_ELEMENT_TYPE_UNARY) {
		if (is_float(tac->unary_identifier)) {
			MALLOC(current->next, sizeof(struct mCc_assembly_line))
			current->next->instruction = new_string("\tfldz");
			current->next->type = MCC_ASSEMBLY_FLD;
			current->next->prev = current;
			current->next->next = NULL;
			push_float_register("temp");
			retval->instruction = new_string(
				"\tfsubp\t%s, %s", get_register("temp"),
				get_register(tac->unary_identifier));
			pop_float_register();
			update_register(tac->unary_identifier,
					tac->identifier1);

			return retval;
		} else {
			struct mCc_assembly_line *temp;
			MALLOC(temp, sizeof(struct mCc_assembly_line))
			retval->next = temp;
			temp->next = NULL;
			temp->prev = retval;
			retval->type = MCC_ASSEMBLY_MOV;
			retval->instruction = new_string("\tmovl\t$0, %s",
							 get_register("temp"));
			temp->type = MCC_ASSEMBLY_SUB;
			temp->instruction =
				new_string("\tsubl\t%s, %s",
					   get_register(tac->identifier3),
					   get_register("temp"));

			free_register(tac->identifier3);
			update_register("temp", tac->identifier1);
			return retval;
		}
	} else {
		retval->type = MCC_ASSEMBLY_SUB;
		if (is_float(tac->rhs)) {
			retval->instruction =
				float_binary_op(tac, "fsubp", current);
		} else {
			retval->instruction = new_string(
				"\tsubl\t%s, %s", get_register(tac->rhs),
				get_register(tac->lhs));
		}
		return NULL;
	}
	return NULL;
}

static struct mCc_assembly_line *
operation_mul(struct mCc_assembly_line *retval, struct mCc_tac_list *tac,
	      struct mCc_assembly_line *current)
{
	retval->type = MCC_ASSEMBLY_MUL;
	if (is_float(tac->rhs)) {
		retval->instruction = float_binary_op(tac, "fmulp", current);
	} else
		retval->instruction =
			new_string("\timull\t%s, %s", get_register(tac->rhs),
				   get_register(tac->lhs));
	return retval;
}

static struct mCc_assembly_line *
operation_div(struct mCc_assembly_line *retval, struct mCc_tac_list *tac,
	      struct mCc_assembly_line *current)
{
	retval->type = MCC_ASSEMBLY_DIV;
	if (is_float(tac->rhs)) {
		retval->instruction = float_binary_op(tac, "fdivp", current);
	} else {
		struct mCc_assembly_line *temp;
		MALLOC(temp, sizeof(struct mCc_assembly_line))
		retval->next = temp;
		temp->prev = retval;
		struct mCc_assembly_line *temp1;
		MALLOC(temp1, sizeof(struct mCc_assembly_line))
		temp->next = temp1;
		temp1->next = NULL;
		temp1->prev = temp;
		if (strcmp(registers->eax, tac->lhs) == 0) {
			retval->instruction = new_string("\tnop");
		} else {
			add_lost_register(registers->eax);
			free_register(registers->eax);
			char *old_lhs = get_register(tac->lhs);
			free_register(tac->lhs);
			get_register(tac->lhs);
			retval->instruction =
				new_string("\tmovl\t%s, %s", old_lhs, "%eax");
		}
		temp->instruction = new_string("\tcltd\n\tidivl\t%s",
					       get_register(tac->rhs));
		temp1->instruction = new_string("\tmovl\t%s, %s", "%eax",
						get_register(tac->identifier1));

		free_register(tac->lhs);
		free_register(tac->rhs);
	}
	return retval;
}

struct mCc_assembly_line *
mCc_assembly_operation(struct mCc_tac_list *tac,
		       struct mCc_assembly_line *current)
{
	assert(current);

	NEW_SINGLE_LINE
	struct mCc_assembly_line *tmp;

	int op = (tac->type == MCC_TAC_ELEMENT_TYPE_UNARY)
			 ? tac->unary_op_type
			 : tac->binary_op_type;

	switch (op) {
	case MCC_TAC_OPERATION_TYPE_PLUS:
		retval = operation_plus(retval, tac, current);
		retval->type = MCC_ASSEMBLY_ADD;
		break;
	case MCC_TAC_OPERATION_TYPE_MINUS:
		tmp = operation_minus(retval, tac, current);
		if (tmp != NULL)
			retval = tmp;
		break;
	case MCC_TAC_OPERATION_TYPE_MULTIPLY:
		retval = operation_mul(retval, tac, current);
		break;
	case MCC_TAC_OPERATION_TYPE_DIVISION:
		retval = operation_div(retval, tac, current);
		break;
	case MCC_TAC_OPERATION_TYPE_EQ:
	case MCC_TAC_OPERATION_TYPE_NE:
	case MCC_TAC_OPERATION_TYPE_LT:
	case MCC_TAC_OPERATION_TYPE_GT:
	case MCC_TAC_OPERATION_TYPE_LE:
	case MCC_TAC_OPERATION_TYPE_GE:
		free(retval);
		retval = mCc_assembly_condition(tac, current);
		break;
	case MCC_TAC_OPERATION_TYPE_AND:
	case MCC_TAC_OPERATION_TYPE_OR:
		free(retval);
		retval = NULL;
		break;
	case MCC_TAC_OPERATION_TYPE_FAC:
		if (jump_cond == -1) {
			jump_cond = -2;
			update_register(tac->unary_identifier,
					tac->identifier1);
		}
		free(retval);
		retval = NULL;
		break;
	default:
		retval=NULL;
		break;
	}
	if (tac->type != MCC_TAC_ELEMENT_TYPE_UNARY) {
		if (!is_float(tac->rhs)) {
			if (strncmp(get_register(tac->rhs), "%s", 2) == 0) {
				free_register(tac->lhs);
				update_register(tac->rhs, tac->identifier1);
			} else {
				free_register(tac->rhs);
				update_register(tac->lhs, tac->identifier1);
			}
		}
	}

	return retval;
}

char *float_binary_op(struct mCc_tac_list *tac, char *operation,
		      struct mCc_assembly_line *current)
{
	assert(current);

	char *retval;
	if (top_float_register(tac->lhs, current)) {
		retval = new_string("\t%s\t%s, %s", operation,
				    get_register(tac->lhs),
				    get_register(tac->rhs));
		pop_float_register();
		update_register(tac->rhs, tac->identifier1);
	} else {
		retval = new_string("\tnop");
	}
	return retval;
}

struct mCc_assembly_line *
mCc_assembly_condition(struct mCc_tac_list *tac,
		       struct mCc_assembly_line *current)
{
	assert(current);

	jump_cond = tac->binary_op_type;
	if (is_float(tac->rhs) || is_float(tac->lhs)) {
		is_float_condition = true;
		NEW_DOUBLE_LINE
		top_float_register(tac->lhs, current);
			retval->instruction = new_string(
				"\tfcomip\t%s, %s", get_register(tac->rhs),
				get_register(tac->lhs));
			pop_float_register();
			while (current->next != NULL)
				current = current->next;
			top_float_register(tac->rhs, current);
			temp->instruction = new_string("\tfstp\t%s",
						       get_register(tac->rhs));
			pop_float_register();
		retval->type = MCC_ASSEMBLY_CMP;
		temp->type = MCC_ASSEMBLY_FSTP;
		return retval;
	} else {
		NEW_SINGLE_LINE
		retval->type = MCC_ASSEMBLY_CMP;
		retval->instruction =
			new_string("\tcmpl\t%s, %s", get_register(tac->rhs),
				   get_register(tac->lhs));
		free_register(tac->lhs);
		free_register(tac->rhs);
		return retval;
	}
}

struct mCc_assembly_line *
mCc_assembly_conditional_jump(struct mCc_tac_list *tac,
			      struct mCc_assembly_line *current)
{
	assert(current);

	char *identifier;
	if (tac->type == MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP)
		identifier = tac->jump->identifier1;
	else
		identifier = tac->identifier1;
	if (jump_cond < 0) {
		NEW_DOUBLE_LINE
		retval->instruction = new_string(
			"\tcmpl\t$0, %s", get_register(tac->identifier1));
		free_register(tac->identifier1);
		if (jump_cond < -1)
			temp->instruction =
				new_string("\tjne\t%s", get_label(identifier));
		else
			temp->instruction =
				new_string("\tje\t%s", get_label(identifier));
		temp->type = MCC_ASSEMBLY_JMP;
		retval->type = MCC_ASSEMBLY_CMP;
		jump_cond = -1;
		return retval;
	}
	NEW_SINGLE_LINE
	retval->type = MCC_ASSEMBLY_JMP;
	switch (jump_cond) {
	case MCC_TAC_OPERATION_TYPE_EQ:
		retval->instruction =
			new_string("\tjne\t%s", get_label(identifier));
		break;
	case MCC_TAC_OPERATION_TYPE_NE:
		retval->instruction =
			new_string("\tje\t%s", get_label(identifier));
		break;
	case MCC_TAC_OPERATION_TYPE_LT:
		if (is_float_condition) {
			retval->instruction =
				new_string("\tjae\t%s", get_label(identifier));
		} else {
			retval->instruction =
				new_string("\tjge\t%s", get_label(identifier));
		}
		break;
	case MCC_TAC_OPERATION_TYPE_GT:
		if (is_float_condition) {
			retval->instruction =
				new_string("\tjbe\t%s", get_label(identifier));
		} else {
			retval->instruction =
				new_string("\tjle\t%s", get_label(identifier));
		}
		break;
	case MCC_TAC_OPERATION_TYPE_LE:
		if (is_float_condition) {
			retval->instruction =
				new_string("\tja\t%s", get_label(identifier));
		} else {
			retval->instruction =
				new_string("\tjg\t%s", get_label(identifier));
		}
		break;
	case MCC_TAC_OPERATION_TYPE_GE:
		if (is_float_condition) {
			retval->instruction =
				new_string("\tjb\t%s", get_label(identifier));
		} else {
			retval->instruction =
				new_string("\tjl\t%s", get_label(identifier));
		}
		break;
	default:
		break;
	}
	is_float_condition = false;
	jump_cond = -1;
	return retval;
}

struct mCc_assembly_line *mCc_assembly_jump(struct mCc_tac_list *tac,
					    struct mCc_assembly_line *current)
{
	assert(current);

	NEW_SINGLE_LINE
	retval->type = MCC_ASSEMBLY_JMP;
	retval->instruction =
		new_string("\tjmp\t%s", get_label(tac->jump->identifier1));
	pop_float_register();
	pop_float_register();
	pop_float_register();
	pop_float_register();
	pop_float_register();
	return retval;
}

struct mCc_assembly_line *
mCc_assembly_copy_identifier(struct mCc_tac_list *tac,
			     struct mCc_assembly_line *current)
{
	assert(current);

	NEW_SINGLE_LINE
	retval->type = MCC_ASSEMBLY_MOV;

	if (is_register(tac->identifier1)) {
		if (get_var(tac->copy_identifier) != NULL
		    && get_var(tac->copy_identifier)->array_size > 1) {
			retval->instruction = new_string(
				"\tleal\t%d(%s), %s",
				get_pos(tac->copy_identifier), "%ebp",
				get_register(tac->identifier1));
		} else {
			if (get_var(tac->copy_identifier) != NULL
			    && get_var(tac->copy_identifier)->type
				       == MCC_TAC_LITERAL_TYPE_FLOAT) {
				retval->instruction = new_string(
					"\tflds\t%d(%s)",
					get_pos(tac->copy_identifier), "%ebp");
				push_float_register(tac->identifier1);
			} else {
				retval->instruction = new_string(
					"\tmovl\t%d(%s), %s",
					get_pos(tac->copy_identifier), "%ebp",
					get_register(tac->identifier1));
			}
		}
	} else {
		if (is_float(tac->copy_identifier)) {
			top_float_register(tac->copy_identifier, current);
				retval->instruction = new_string(
					"\tfstps\t%d(%s)",
					get_pos(tac->identifier1), "%ebp");
				get_var(tac->identifier1)->type =
					MCC_TAC_LITERAL_TYPE_FLOAT;
				pop_float_register();



		} else {
			retval->instruction =
				new_string("\tmovl\t%s, %d(%s)",
					   get_register(tac->copy_identifier),
					   get_pos(tac->identifier1), "%ebp");
		}
		free_register(tac->copy_identifier);
	}
	return retval;
}

struct mCc_assembly_line *
mCc_assembly_copy_literal(struct mCc_tac_list *tac,
			  struct mCc_assembly_line *current)
{
	assert(current);

	NEW_SINGLE_LINE
	retval->type = MCC_ASSEMBLY_MOV;
	switch (tac->literal_type) {
	case (MCC_TAC_LITERAL_TYPE_INT):
		retval->instruction =
			new_string("\tmovl\t$%d, %s", tac->i_literal,
				   get_register(tac->identifier1));
		break;
	case (MCC_TAC_LITERAL_TYPE_FLOAT):
		retval->instruction =
			new_string("\tflds\t%s", get_label(tac->identifier1));
		push_float_register(tac->identifier1);
		retval->type = MCC_ASSEMBLY_FLD;
		break;
	case (MCC_TAC_LITERAL_TYPE_BOOL):
		if (is_register(tac->identifier1)) {
			retval->instruction = new_string(
				"\tmovl\t$%d, %s", tac->b_literal ? 1 : 0,
				get_register(tac->identifier1));
		} else {
			retval->instruction = new_string(
				"\tmovl\t$%d, %d(%s)", tac->b_literal ? 1 : 0,
				get_pos(tac->identifier1), "%ebp");
		}
		break;
	case (MCC_TAC_LITERAL_TYPE_STRING):
		retval->instruction = new_string(
			"\tmovl\t$%s, %s", get_label(tac->identifier1),
			get_register(tac->identifier1));
		break;
	}
	return retval;
}


/****************************************************************/
/****************************************************************/

bool is_float(char *identifier)
{
	return strncmp(get_register(identifier), "%s", 2) == 0;
}

bool is_register(char *identifier)
{
	if (strlen(identifier) >= 4 && identifier[0] == 'r'
	    && identifier[1] == 'e' && identifier[2] == 'g'
	    && identifier[3] == '_')
		return true;
	return false;
}

static enum mCc_tac_literal_type
ast_to_tac_literal_type(enum mCc_ast_literal_type type)
{

	switch (type) {
	case (MCC_AST_LITERAL_TYPE_INT):
		return MCC_TAC_LITERAL_TYPE_INT;
	case (MCC_AST_LITERAL_TYPE_STRING):
		return MCC_TAC_LITERAL_TYPE_STRING;
	case (MCC_AST_LITERAL_TYPE_FLOAT):
		return MCC_TAC_LITERAL_TYPE_FLOAT;
	case (MCC_AST_LITERAL_TYPE_BOOL):
		return MCC_TAC_LITERAL_TYPE_BOOL;
	}
	return MCC_TAC_LITERAL_TYPE_INT;
}

int get_literal_size(enum mCc_ast_literal_type type)
{
	switch (type) {
	case MCC_AST_LITERAL_TYPE_STRING:
		return 4;
	case MCC_AST_LITERAL_TYPE_INT:
		return 4;
	case MCC_AST_LITERAL_TYPE_FLOAT:
		return 8;
	case MCC_AST_LITERAL_TYPE_BOOL:
		return 4;
	default:
		return 4;
	}
}

bool is_builtin(char *function)
{
	if (strcmp(function, "print") == 0 || strcmp(function, "print_nl") == 0
	    || strcmp(function, "print_int") == 0
	    || strcmp(function, "print_float") == 0
	    || strcmp(function, "read_int") == 0
	    || strcmp(function, "read_float") == 0)
		return true;
	return false;
}

void set_label(char *key, char *value)
{
	for (int i = 0; i < label->counter; ++i) {
		if (strcmp(label->l[i].key, key) == 0) {
			label->l[i].value = value;
			return;
		}
	}
	if (label->counter == 0) {
		MALLOC_((label->l), (sizeof(struct label_identification)))
	} else {
		REALLOC_((label->l), ((label->counter + 1)
				      * sizeof(struct label_identification)))
	}
	label->l[label->counter].key = key;
	label->l[label->counter].value = value;
	label->counter++;
}

char *get_label(char *key)
{
	for (int i = 0; i < label->counter; ++i) {
		if (strcmp(label->l[i].key, key) == 0)
			return label->l[i].value;
	}
	return NULL;
}

void set_var(int size, int array_size, char *identifier)
{
	for (int i = 0; i < stack->counter; ++i) {
		if (stack->variables[i].identifier == NULL)
			continue;
		if (strcmp(stack->variables[i].identifier, identifier) == 0)
			return;
	}
	if (stack->counter == 0) {
		MALLOC_((stack->variables), (sizeof(struct variable)))
		stack->variables[stack->counter].stack_diff =
			-(size * array_size);
		stack->variables[stack->counter].size = size;
		stack->variables[stack->counter].array_size = array_size;
	} else {
		REALLOC_((stack->variables),
			 ((stack->counter + 1) * sizeof(struct variable)))
		stack->variables[stack->counter].stack_diff =
			(stack->variables[stack->counter - 1].stack_diff
			 - (size * array_size));
		stack->variables[stack->counter].size = size;
		stack->variables[stack->counter].array_size = array_size;
	}
	stack->variables[stack->counter].identifier = identifier;
	stack->counter++;
}

void set_param_var(int size, int array_size, char *identifier)
{
	for (int i = 0; i < param_stack->counter; ++i) {
		if (param_stack->variables[i].identifier == NULL)
			continue;
		if (strcmp(param_stack->variables[i].identifier, identifier)
		    == 0)
			return;
	}
	if (param_stack->counter == 0) {
		MALLOC_((param_stack->variables), (sizeof(struct variable)))
		param_stack->variables[param_stack->counter].stack_diff = 0;
		param_stack->variables[param_stack->counter].size = size;
		param_stack->variables[param_stack->counter].array_size =
			array_size;
	} else {
		REALLOC_((param_stack->variables),
			 ((param_stack->counter + 1) * sizeof(struct variable)))
		if (param_stack->variables[param_stack->counter - 1].array_size
		    > 1)
			param_stack->variables[param_stack->counter]
				.stack_diff =
				param_stack->variables[param_stack->counter - 1]
					.stack_diff
				+ 4;

		else
			param_stack->variables[param_stack->counter]
				.stack_diff =
				param_stack->variables[param_stack->counter - 1]
					.stack_diff
				+ param_stack
					  ->variables[param_stack->counter - 1]
					  .size;
		param_stack->variables[param_stack->counter].size = size;
		param_stack->variables[param_stack->counter].array_size =
			array_size;
	}
	param_stack->variables[param_stack->counter].identifier = identifier;
	param_stack->counter++;
}

int get_pos(char *identifier)
{
	return get_var(identifier)->stack_diff;
}

struct variable *get_var(char *identifier)
{
	for (int i = 0; i < stack->counter; ++i) {
		if (strcmp(stack->variables[i].identifier, identifier) == 0)
			return &stack->variables[i];
	}
	for (int i = 0; i < param_stack->counter; ++i) {
		if (strcmp(param_stack->variables[i].identifier, identifier)
		    == 0)
			return &param_stack->variables[i];
	}
	return NULL;
}

void new_stack()
{
	delete_stack();
	MALLOC_(stack, sizeof(struct stack_helper))
	stack->counter = 0;
	stack->variables = NULL;

	MALLOC_(param_stack, sizeof(struct stack_helper))
	param_stack->counter = 0;
	param_stack->variables = NULL;
}

void delete_stack()
{

	if (stack != NULL) {
		free(stack->variables);
		free(stack);
	}
	if (param_stack != NULL) {
		free(param_stack->variables);
		free(param_stack);
	}
}

int get_stack_size()
{
	if (stack->counter == 0) {
		if (param_stack->counter == 0)
			return 0;
		else
			return (param_stack->variables[param_stack->counter - 1]
					.stack_diff);
	}

	if (param_stack->counter == 0)
		return -(stack->variables[stack->counter - 1].stack_diff);
	return ((-(stack->variables[stack->counter - 1].stack_diff))
		+ param_stack->variables[param_stack->counter - 1].stack_diff);
}

void free_register(char *identifier)
{
	if (identifier == NULL)
		return;
	if (registers->eax != NULL && strcmp(registers->eax, identifier) == 0) {
		registers->eax = NULL;
	}
	if (registers->ebx != NULL && strcmp(registers->ebx, identifier) == 0) {
		registers->ebx = NULL;
	}
	if (registers->ecx != NULL && strcmp(registers->ecx, identifier) == 0) {
		registers->ecx = NULL;
	}
	if (registers->edx != NULL && strcmp(registers->edx, identifier) == 0) {
		registers->edx = NULL;
	}
}

void free_all_registers()
{
	registers->eax = NULL;
	registers->ebx = NULL;
	registers->ecx = NULL;
	registers->edx = NULL;
	registers->st0 = NULL;
	registers->st1 = NULL;
	registers->st2 = NULL;
	registers->st3 = NULL;
	registers->st4 = NULL;
	registers->st5 = NULL;
	registers->st6 = NULL;
	registers->st7 = NULL;
}

char *get_register(char *identifier)
{
	if (registers->eax != NULL && strcmp(registers->eax, identifier) == 0)
		return "%eax";
	if (registers->ebx != NULL && strcmp(registers->ebx, identifier) == 0)
		return "%ebx";
	if (registers->ecx != NULL && strcmp(registers->ecx, identifier) == 0)
		return "%ecx";
	if (registers->edx != NULL && strcmp(registers->edx, identifier) == 0)
		return "%edx";
	if (registers->st0 != NULL && strcmp(registers->st0, identifier) == 0)
		return "%st(0)";
	if (registers->st1 != NULL && strcmp(registers->st1, identifier) == 0)
		return "%st(1)";
	if (registers->st2 != NULL && strcmp(registers->st2, identifier) == 0)
		return "%st(2)";
	if (registers->st3 != NULL && strcmp(registers->st3, identifier) == 0)
		return "%st(3)";
	if (registers->st4 != NULL && strcmp(registers->st4, identifier) == 0)
		return "%st(4)";
	if (registers->st5 != NULL && strcmp(registers->st5, identifier) == 0)
		return "%st(5)";
	if (registers->st6 != NULL && strcmp(registers->st6, identifier) == 0)
		return "%st(6)";
	if (registers->st7 != NULL && strcmp(registers->st7, identifier) == 0)
		return "%st(7)";
	if (registers->eax == NULL) {
		registers->eax = identifier;
		return "%eax";
	}
	if (registers->ebx == NULL) {
		registers->ebx = identifier;
		return "%ebx";
	}
	if (registers->ecx == NULL) {
		registers->ecx = identifier;
		return "%ecx";
	}
	if (registers->edx == NULL) {
		registers->edx = identifier;
		return "%edx";
	}
	no_more_registers = true;
	return "temperoria";
}

void push_float_register(char *identifier)
{
	registers->st7 = registers->st6;
	registers->st6 = registers->st5;
	registers->st5 = registers->st4;
	registers->st4 = registers->st3;
	registers->st3 = registers->st2;
	registers->st2 = registers->st1;
	registers->st1 = registers->st0;
	registers->st0 = identifier;
}

char *pop_float_register()
{
	if (registers->st0 != NULL) {
		char *retval = registers->st0;
		registers->st0 = registers->st1;
		registers->st1 = registers->st2;
		registers->st2 = registers->st3;
		registers->st3 = registers->st4;
		registers->st4 = registers->st5;
		registers->st5 = registers->st6;
		registers->st6 = registers->st7;
		registers->st7 = NULL;

		return retval;
	}
	return "pop_error";
}

void update_register(char *old_identifier, char *new_identifier)
{
	if (registers->eax != NULL
	    && strcmp(registers->eax, old_identifier) == 0)
		registers->eax = new_identifier;
	if (registers->ebx != NULL
	    && strcmp(registers->ebx, old_identifier) == 0)
		registers->ebx = new_identifier;
	if (registers->ecx != NULL
	    && strcmp(registers->ecx, old_identifier) == 0)
		registers->ecx = new_identifier;
	if (registers->edx != NULL
	    && strcmp(registers->edx, old_identifier) == 0)
		registers->edx = new_identifier;
	if (registers->st0 != NULL
	    && strcmp(registers->st0, old_identifier) == 0)
		registers->st0 = new_identifier;
	if (registers->st1 != NULL
	    && strcmp(registers->st1, old_identifier) == 0)
		registers->st1 = new_identifier;
	if (registers->st2 != NULL
	    && strcmp(registers->st2, old_identifier) == 0)
		registers->st2 = new_identifier;
	if (registers->st3 != NULL
	    && strcmp(registers->st3, old_identifier) == 0)
		registers->st3 = new_identifier;
	if (registers->st4 != NULL
	    && strcmp(registers->st4, old_identifier) == 0)
		registers->st4 = new_identifier;
	if (registers->st5 != NULL
	    && strcmp(registers->st5, old_identifier) == 0)
		registers->st5 = new_identifier;
	if (registers->st6 != NULL
	    && strcmp(registers->st6, old_identifier) == 0)
		registers->st6 = new_identifier;
	if (registers->st7 != NULL
	    && strcmp(registers->st7, old_identifier) == 0)
		registers->st7 = new_identifier;
}

bool top_float_register(char *identifier, struct mCc_assembly_line *current)
{
	if (registers->st0 == NULL)
		return false;
	if (strcmp(registers->st0, identifier) == 0) {
		return true;
	} else {
		NEW_SINGLE_LINE
		current->next = retval;
		retval->prev = current;
		if (strcmp(registers->st1, identifier) == 0)
			retval->instruction =
				new_string("\tfxch\t%s", "%st(1)");
		else if (strcmp(registers->st2, identifier) == 0)
			retval->instruction =
				new_string("\tfxch\t%s", "%st(2)");
		else if (strcmp(registers->st3, identifier) == 0)
			retval->instruction =
				new_string("\tfxch\t%s", "%st(3)");
		else if (strcmp(registers->st4, identifier) == 0)
			retval->instruction =
				new_string("\tfxch\t%s", "%st(4)");
		else if (strcmp(registers->st5, identifier) == 0)
			retval->instruction =
				new_string("\tfxch\t%s", "%st(5)");
		else if (strcmp(registers->st6, identifier) == 0)
			retval->instruction =
				new_string("\tfxch\t%s", "%st(6)");
		else if (strcmp(registers->st7, identifier) == 0)
			retval->instruction =
				new_string("\tfxch\t%s", "%st(7)");
		fxch(identifier);
		retval->type = MCC_ASSEMBLY_FXCH;
		return true;
	}
	return false;
}

void fxch(char *identifier1)
{
	if (strcmp(registers->st0, identifier1) == 0) {
		return;
	} else if (strcmp(registers->st1, identifier1) == 0) {
		char *st0 = registers->st0;
		registers->st0 = registers->st1;
		registers->st1 = st0;
	} else if (strcmp(registers->st2, identifier1) == 0) {
		char *st0 = registers->st0;
		registers->st0 = registers->st2;
		registers->st2 = st0;
	} else if (strcmp(registers->st3, identifier1) == 0) {
		char *st0 = registers->st0;
		registers->st0 = registers->st3;
		registers->st3 = st0;
	} else if (strcmp(registers->st4, identifier1) == 0) {
		char *st0 = registers->st0;
		registers->st0 = registers->st4;
		registers->st4 = st0;
	} else if (strcmp(registers->st5, identifier1) == 0) {
		char *st0 = registers->st0;
		registers->st0 = registers->st5;
		registers->st5 = st0;
	} else if (strcmp(registers->st6, identifier1) == 0) {
		char *st0 = registers->st0;
		registers->st0 = registers->st6;
		registers->st6 = st0;
	} else if (strcmp(registers->st7, identifier1) == 0) {
		char *st0 = registers->st0;
		registers->st0 = registers->st7;
		registers->st7 = st0;
	}
}

void move_line_to_end(struct mCc_assembly_line *current,
		      struct mCc_tac_list *tac, char *identifier)
{
	if (identifier == NULL)
		return;
	while (tac != NULL) {
		switch (tac->type) {
		case MCC_TAC_ELEMENT_TYPE_COPY_LITERAL:
			if (strcmp(tac->identifier1, identifier) == 0) {
				current->next =
					mCc_assembly_copy_literal(tac, current);
				current->next->prev = current;
				return;
			}
			break;
		case MCC_TAC_ELEMENT_TYPE_COPY_IDENTIFIER:
			if (strcmp(tac->identifier1, identifier) == 0) {
				current->next = mCc_assembly_copy_identifier(
					tac, current);
				current->next->prev = current;
				return;
			}
			break;
		case MCC_TAC_ELEMENT_TYPE_BINARY:
		case MCC_TAC_ELEMENT_TYPE_UNARY:
			if (strcmp(tac->identifier1, identifier) == 0) {
				move_line_to_end(current, tac->prev, tac->lhs);
				while (current->next != NULL)
					current = current->next;
				move_line_to_end(current, tac->prev, tac->rhs);
				while (current->next != NULL)
					current = current->next;
				current->next =
					mCc_assembly_operation(tac, current);
				if (current->next != NULL)
					current->next->prev = current;
				return;
			}
			break;
		case MCC_TAC_ELEMENT_TYPE_LOAD:
			if (strcmp(tac->identifier1, identifier) == 0) {
				move_line_to_end(current, tac->prev,
						 tac->identifier2);
				while (current->next != NULL)
					current = current->next;
				current->next = mCc_assembly_load(tac, current);
				current->next->prev = current;
			}
		default:
			break;
		}
		tac = tac->prev;
	}
}

void add_lost_register(char *identifier)
{
	if (lost_registers->counter == 0) {
		MALLOC_(lost_registers->reg, sizeof(lost_registers->reg))
	} else {
		REALLOC_(lost_registers->reg,
			 sizeof(lost_registers->reg)
				 * (lost_registers->counter + 1))
	}
	lost_registers->reg[lost_registers->counter].identifier = identifier;
	lost_registers->counter++;
}

void lose_all_registers()
{
	if (registers->eax != NULL) {
		add_lost_register(registers->eax);
	}
	if (registers->ebx != NULL) {
		add_lost_register(registers->ebx);
	}
	if (registers->ecx != NULL) {
		add_lost_register(registers->ecx);
	}
	if (registers->edx != NULL) {
		add_lost_register(registers->edx);
	}
	if (registers->st0 != NULL) {
		add_lost_register(registers->st0);
	}
	if (registers->st1 != NULL) {
		add_lost_register(registers->st1);
	}
	if (registers->st2 != NULL) {
		add_lost_register(registers->st2);
	}
	if (registers->st3 != NULL) {
		add_lost_register(registers->st3);
	}
	if (registers->st4 != NULL) {
		add_lost_register(registers->st4);
	}
	if (registers->st5 != NULL) {
		add_lost_register(registers->st5);
	}
	if (registers->st6 != NULL) {
		add_lost_register(registers->st6);
	}
	if (registers->st7 != NULL) {
		add_lost_register(registers->st7);
	}
	free_all_registers();
}