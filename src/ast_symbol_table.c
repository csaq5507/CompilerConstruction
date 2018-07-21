
#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include <mCc/utils.h>
#include "mCc/ast_symbol_table.h"
#include "mCc/error.h"

#define ARRAY_SIZE 2048

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


static ast_symbol_table *create_new_symbol_table_node();
static void mCc_ast_symbol_table_add_default_function_names();
static ast_symbol_table *add_element_symbols(ast_symbol_table *head, char *old,
					     char *new,
					     enum mCc_ast_type d_type,
					     int num_params,
					     enum mCc_ast_type *l_types,
					 	 int symbol_size);
static char *find_element_symbols(ast_symbol_table *head, char *elem);
static enum mCc_ast_type find_identifier_type(ast_symbol_table *head,
					      char *elem);
static int find_element_param_num(ast_symbol_table *head, char *elem);
static int find_element_symbol_size(ast_symbol_table *head, char *elem);
static void delete_symbol_table_node(ast_symbol_table *head);
static enum mCc_ast_type *find_element_param_types(ast_symbol_table *head,
						   char *elem);

static ast_current_fun *create_new_current_fun_node();
static void delete_current_fun(ast_current_fun *head);

static void ast_symbol_table_identifier(struct mCc_ast_identifier *identifier,
					void *data);

static void ast_symbol_table_func_type(struct mCc_ast_function_def *f,
				       void *data);
static void ast_symbol_table_func_void(struct mCc_ast_function_def *f,
				       void *data);
static void ast_symbol_table_close_func(struct mCc_ast_function_def *f,
					void *data);

static void ast_symbol_table_compound_stmt(struct mCc_ast_compound_stmt *c_stmt,
					   void *data);
static void
ast_symbol_table_close_compound_stmt(struct mCc_ast_compound_stmt *c_stmt,
				     void *data);

static void ast_symbol_close_if_stmt(struct mCc_ast_if_stmt *stmt, void *data);
static void ast_symbol_table_ass_stmt(struct mCc_ast_assignment *stmt,
				      void *data);
static void ast_symbol_table_decl_stmt(struct mCc_ast_declaration *stmt,
				       void *data);
static void ast_symbol_table_ret_stmt(struct mCc_ast_ret_stmt *stmt,
				      void *data);
static void ast_symbol_table_if_stmt(struct mCc_ast_if_stmt *stmt, void *data);
static void ast_symbol_table_close_stmt(struct mCc_ast_stmt *stmt, void *data);

static void
ast_symbol_table_expression_single(struct mCc_ast_single_expression *expression,
				   void *data);

static void
ast_symbol_table_call_expression(struct mCc_ast_call_expr *expression,
				 void *data);

const char *print_type(enum mCc_ast_type type)
{
	switch (type) {
	case MCC_AST_TYPE_STRING:
		return "string";
	case MCC_AST_TYPE_INT:
		return "int";
	case MCC_AST_TYPE_FLOAT:
		return "float";
	case MCC_AST_TYPE_BOOL:
		return "bool";
	case MCC_AST_TYPE_STRING_ARRAY:
		return "string_array";
	case MCC_AST_TYPE_INT_ARRAY:
		return "int_array";
	case MCC_AST_TYPE_FLOAT_ARRAY:
		return "float_array";
	case MCC_AST_TYPE_BOOL_ARRAY:
		return "bool_array";
	case MCC_AST_TYPE_VOID:
		return "void";
	default:
		return "unknown";
	}
}

static struct mCc_ast_visitor symbol_table_visitor(ast_symbol_table *data)
{
	assert(data);

	return (struct mCc_ast_visitor){

		.traversal = MCC_AST_VISIT_DEPTH_FIRST,
		.order = MCC_AST_VISIT_PRE_ORDER,

		.userdata = data,

		.identifier = ast_symbol_table_identifier,

		.function_def_type = ast_symbol_table_func_type,
		.function_def_void = ast_symbol_table_func_void,
		.close_function_def = ast_symbol_table_close_func,

		.c_stmt = ast_symbol_table_compound_stmt,
		.close_c_stmt = ast_symbol_table_close_compound_stmt,

		.close_if_stmt = ast_symbol_close_if_stmt,
		.ass_stmt = ast_symbol_table_ass_stmt,
		.decl_stmt = ast_symbol_table_decl_stmt,
		.ret_stmt = ast_symbol_table_ret_stmt,
		.if_stmt = ast_symbol_table_if_stmt,
		.close_statement = ast_symbol_table_close_stmt,

		.single_expression = ast_symbol_table_expression_single,
		.call_expression = ast_symbol_table_call_expression,
	};
};


static ast_symbol_table *create_new_symbol_table_node()
{
	ast_symbol_table *table;
	MALLOC(table, sizeof(ast_symbol_table));

	table->next = NULL;
	table->prev = NULL;
	table->symbols_counter = 0;
	table->symbols = NULL;
	return table;
}

static void mCc_ast_symbol_table_add_default_function_names()
{
	char *print = "print";
	char *print_nl = "print_nl";
	char *print_int = "print_int";
	char *print_float = "print_float";
	char *read_int = "read_int";
	char *read_float = "read_float";


	table = add_element_symbols(table, print, print, MCC_AST_TYPE_VOID, 1,
				    NULL, 0);
	table = add_element_symbols(table, print_nl, print_nl,
				    MCC_AST_TYPE_VOID, 0, NULL, 0);
	table = add_element_symbols(table, print_int, print_int,
				    MCC_AST_TYPE_VOID, 1, NULL, 0);
	table = add_element_symbols(table, print_float, print_float,
				    MCC_AST_TYPE_VOID, 1, NULL, 0);
	table = add_element_symbols(table, read_int, read_int, MCC_AST_TYPE_INT,
				    0, NULL, 0);
	table = add_element_symbols(table, read_float, read_float,
				    MCC_AST_TYPE_FLOAT, 0, NULL, 0);
}


static ast_symbol_table *add_element_symbols(ast_symbol_table *head, char *old,
					     char *new,
					     enum mCc_ast_type d_type,
					     int num_params,
					     enum mCc_ast_type *l_types,
						 int symbol_size)
{
	assert(head);
	assert(old);

	ast_symbol *symbol;
	MALLOC(symbol, sizeof(ast_symbol));

	symbol->mCc_symbol_old = old;
	symbol->mCc_symbol_new = new;
	symbol->d_type = d_type;
	symbol->func_param_counter = num_params;
	symbol->l_types = l_types;
	symbol->symbol_size = symbol_size;
	REALLOC(head->symbols,
		sizeof(ast_symbol) * (head->symbols_counter + 1));

	memcpy(&(head->symbols[head->symbols_counter]), symbol,
	       sizeof(*symbol));

	head->symbols_counter++;
	free(symbol);
	return head;
}

static char *find_element_symbols(ast_symbol_table *head, char *elem)
{
	assert(head);
	assert(elem);
	for (int i = 0; i < head->symbols_counter; i++) {
		char *curr = head->symbols[i].mCc_symbol_old;
		if (strcmp(curr, elem) == 0) {
			return head->symbols[i].mCc_symbol_new;
		}
	}
	return NULL;
}

static enum mCc_ast_type find_identifier_type(ast_symbol_table *head,
					      char *elem)
{
	assert(head);
	assert(elem);
	while (head != NULL) {
		for (int i = 0; i < head->symbols_counter; i++) {
			if (strcmp(head->symbols[i].mCc_symbol_old, elem)
			    == 0) {
				return head->symbols[i].d_type;
			}
		}
		head = head->prev;
	}
	return MCC_AST_TYPE_VOID;
}

static int find_element_param_num(ast_symbol_table *head, char *elem)
{
	assert(head);
	assert(elem);
	while (head != NULL) {
		for (int i = 0; i < head->symbols_counter; i++) {
			if (strcmp(head->symbols[i].mCc_symbol_old, elem)
			    == 0) {
				return head->symbols[i].func_param_counter;
			}
		}
		head = head->prev;
	}
	return 0;
}

static enum mCc_ast_type *find_element_param_types(ast_symbol_table *head,
						   char *elem)
{
	assert(head);
	assert(elem);
	while (head != NULL) {
		for (int i = 0; i < head->symbols_counter; i++) {
			if (strcmp(head->symbols[i].mCc_symbol_old, elem)
			    == 0) {
				return head->symbols[i].l_types;
			}
		}
		head = head->prev;
	}
	return NULL;
}

static int find_element_symbol_size(ast_symbol_table *head,
												   char *elem)
{
	assert(head);
	assert(elem);
	while (head != NULL) {
		for (int i = 0; i < head->symbols_counter; i++) {
			if (strcmp(head->symbols[i].mCc_symbol_old, elem)
				== 0) {
				return head->symbols[i].symbol_size;
			}
		}
		head = head->prev;
	}
	return 0;
}

static void delete_symbol_table_node(ast_symbol_table *head)
{
	while (head->prev != NULL)
		head = head->prev;

	ast_symbol_table *current = head;
	while (current != NULL) {
		ast_symbol_table *next = current->next;
		if (current->symbols != NULL) {
			free(current->symbols);
		}
		free(current);
		current = next;
	}
}

static ast_current_fun *create_new_current_fun_node()
{
	ast_current_fun *elem;
	MALLOC(elem, sizeof(ast_current_fun));
	elem->prev = NULL;
	elem->next = NULL;
	elem->has_ret = false;
	elem->type = MCC_AST_TYPE_VOID;
	elem->identifier = NULL;
	elem->in_if = false;
	elem->in_else = false;
	elem->missing_if_ret = false;
	elem->has_else = false;
	return elem;
}

static void delete_current_fun(ast_current_fun *head)
{
	while (head->prev != NULL)
		head = head->prev;

	ast_current_fun *current = head;
	while (current != NULL) {
		ast_current_fun *next = current->next;
		free(current);
		current = next;
	}
}

static void ast_symbol_table_identifier(struct mCc_ast_identifier *identifier,
					void *data)
{
	assert(identifier);
	assert(data);

	enum mCc_ast_type d_type =
		find_identifier_type(table, identifier->name);
	identifier->d_type = d_type;
}

static enum mCc_ast_type literal_to_ast_type_single(enum mCc_ast_literal_type type) {
	switch (type) {
		case (MCC_AST_LITERAL_TYPE_INT):
			return MCC_AST_TYPE_INT;
		case (MCC_AST_LITERAL_TYPE_FLOAT):
			return MCC_AST_TYPE_FLOAT;
		case (MCC_AST_LITERAL_TYPE_BOOL):
			return MCC_AST_TYPE_BOOL;
		case (MCC_AST_LITERAL_TYPE_STRING):
			return MCC_AST_TYPE_STRING;
	}
	return MCC_AST_TYPE_INT;
}

static enum mCc_ast_type literal_to_ast_type_array(enum mCc_ast_literal_type type) {
	switch (type) {
		case (MCC_AST_LITERAL_TYPE_INT):
			return MCC_AST_TYPE_INT_ARRAY;
		case (MCC_AST_LITERAL_TYPE_FLOAT):
			return MCC_AST_TYPE_FLOAT_ARRAY;
		case (MCC_AST_LITERAL_TYPE_BOOL):
			return MCC_AST_TYPE_BOOL_ARRAY;
		case (MCC_AST_LITERAL_TYPE_STRING):
			return MCC_AST_TYPE_STRING_ARRAY;
	}
	return MCC_AST_TYPE_INT_ARRAY;
}


static void add_function_name_to_table(struct mCc_ast_function_def *f) {
	switch (f->l_type) {
		case (MCC_AST_LITERAL_TYPE_INT):
			table = add_element_symbols(
					table, f->identifier->name,
					f->identifier->renamed, MCC_AST_TYPE_INT,
					f->params->counter, f->identifier->param_types, 0);
			break;
		case (MCC_AST_LITERAL_TYPE_FLOAT):
			table = add_element_symbols(
					table, f->identifier->name,
					f->identifier->renamed, MCC_AST_TYPE_FLOAT,
					f->params->counter, f->identifier->param_types, 0);
			break;
		case (MCC_AST_LITERAL_TYPE_BOOL):
			table = add_element_symbols(
					table, f->identifier->name,
					f->identifier->renamed, MCC_AST_TYPE_BOOL,
					f->params->counter, f->identifier->param_types, 0);
			break;
		case (MCC_AST_LITERAL_TYPE_STRING):
			table = add_element_symbols(
					table, f->identifier->name,
					f->identifier->renamed, MCC_AST_TYPE_STRING,
					f->params->counter, f->identifier->param_types, 0);
			break;
	}
}

static void ast_symbol_table_func_type(struct mCc_ast_function_def *f,
				       void *data)
{
	assert(f);
	assert(data);

	if (strcmp(f->identifier->name, "main") == 0) {
		if(add_errors) {
			char error_msg[1024] = {0};
			snprintf(error_msg, sizeof(error_msg), ERROR_MAIN_NOT_VOID,
					 print_type(f->identifier->d_type));
			mCc_add_error(error_msg, f->identifier->node.sloc.start_line,
						  h_result);
		}
		if (has_main == true) {
			if(!add_errors) {
				char error_msg[1024] = {0};
				snprintf(error_msg, sizeof(error_msg),
						 ERROR_DUBLICATE_FUNCTION, f->identifier->name);
				char *tmp = new_string(ERROR_DUBLICATE_FUNCTION,
									   f->identifier->name);
				mCc_add_error(tmp,
							  f->identifier->node.sloc.start_line,
							  h_result);
				free(tmp);
			}
		}

	}

	if (f->params != NULL) {
		if (f->identifier->param_types!= NULL)
			free(f->identifier->param_types);
		f->identifier->param_types =
				malloc(sizeof(enum mCc_ast_type) * f->params->counter);
		enum mCc_ast_type h_param[f->params->counter];
		for (int i = 0; i < f->params->counter; i++) {
			if (f->params->declaration[i].type
				== MCC_AST_DECLARATION_TYPE_SINGLE) {
				h_param[i] = literal_to_ast_type_single(f->params->declaration[i].literal);
			} else if (f->params->declaration[i].type
					   == MCC_AST_DECLARATION_TYPE_ARRAY) {
				h_param[i] = literal_to_ast_type_array(f->params->declaration[i].literal);
			}
		}
		memcpy(f->identifier->param_types, h_param,
			   sizeof(*h_param) * f->params->counter);
	}

	if (find_element_symbols(table, f->identifier->name) != NULL) {
		if(!add_errors) {
			char error_msg[1024] = {0};
			snprintf(error_msg, sizeof(error_msg), ERROR_DUBLICATE_FUNCTION,
					 f->identifier->name);
			mCc_add_error(error_msg, f->identifier->node.sloc.start_line,
						  h_result);
		}
	} else {
		free(f->identifier->renamed);
		f->identifier->renamed =
			new_string("%s%d", f->identifier->name, g_counter++);
		add_function_name_to_table(f);
	}

	ast_symbol_table *new = create_new_symbol_table_node();
	table->next = new;
	new->prev = table;
	table = new;

	ast_current_fun *n_fun = create_new_current_fun_node();
	switch (f->l_type) {
		case (MCC_AST_LITERAL_TYPE_INT):
			n_fun->type = MCC_AST_TYPE_INT;
			break;
		case (MCC_AST_LITERAL_TYPE_FLOAT):
			n_fun->type = MCC_AST_TYPE_FLOAT;
			break;
		case (MCC_AST_LITERAL_TYPE_BOOL):
			n_fun->type = MCC_AST_TYPE_BOOL;
			break;
		case (MCC_AST_LITERAL_TYPE_STRING):
			n_fun->type = MCC_AST_TYPE_STRING;
			break;
	}
	n_fun->prev = current_fun;
	n_fun->identifier = f->identifier;
	current_fun->next = n_fun;
	current_fun = n_fun;
}

static void ast_symbol_table_func_void(struct mCc_ast_function_def *f,
				       void *data)
{
	assert(f);
	assert(data);

	if (f->params != NULL) {
        if (f->identifier->param_types!= NULL)
            free(f->identifier->param_types);
		f->identifier->param_types =
			malloc(sizeof(enum mCc_ast_type) * f->params->counter);
		enum mCc_ast_type h_param[f->params->counter];
		for (int i = 0; i < f->params->counter; i++) {
			if (f->params->declaration[i].type
			    == MCC_AST_DECLARATION_TYPE_SINGLE) {
				h_param[i] = literal_to_ast_type_single(f->params->declaration[i].literal);
			} else if (f->params->declaration[i].type
				   == MCC_AST_DECLARATION_TYPE_ARRAY) {
				h_param[i] = literal_to_ast_type_array(f->params->declaration[i].literal);
			}
		}
		memcpy(f->identifier->param_types, h_param,
		       sizeof(*h_param) * f->params->counter);
	}

	if (strcmp(f->identifier->name, "main") == 0) {
		if (has_main == true) {
			if(!add_errors) {
				char *tmp = new_string(ERROR_DUBLICATE_FUNCTION,
									   f->identifier->name);
				mCc_add_error(tmp,
							  f->identifier->node.sloc.start_line,
							  h_result);
				free(tmp);
			}
		}
		free(f->identifier->renamed);
		f->identifier->renamed = copy_string(f->identifier->name);

		has_main = true;
	} else {
		if (find_element_symbols(table, f->identifier->name) != NULL) {
			if(!add_errors) {
				char *tmp = new_string(ERROR_DUBLICATE_FUNCTION,
									   f->identifier->name);
				mCc_add_error(tmp,
							  f->identifier->node.sloc.start_line,
							  h_result);
				free(tmp);
			}
		}
		else {
			free(f->identifier->renamed);
			f->identifier->renamed = new_string(
				"%s%d", f->identifier->name, g_counter++);
			table = add_element_symbols(
				table, f->identifier->name,
				f->identifier->renamed, MCC_AST_TYPE_VOID,
				f->params->counter, f->identifier->param_types, 0);
		}
	}

	ast_symbol_table *new = create_new_symbol_table_node();
	table->next = new;
	new->prev = table;
	table = new;

	ast_current_fun *n_fun = create_new_current_fun_node();
	n_fun->type = MCC_AST_TYPE_VOID;
	n_fun->prev = current_fun;
	n_fun->identifier = f->identifier;
	current_fun->next = n_fun;
	current_fun = n_fun;
}

static void ast_symbol_table_close_func(struct mCc_ast_function_def *f,
					void *data)
{
	assert(f);
	assert(data);

	if (table->prev != NULL) {
		ast_symbol_table *temp = table;
		table = table->prev;
		temp->prev = NULL;
		delete_symbol_table_node(temp);
		table->next = NULL;
	}

	if (f->type != MCC_AST_FUNCTION_DEF_TYPE_VOID
	    && !current_fun->has_ret) {
		if(add_errors) {
			char error_msg[1024] = {0};

			snprintf(error_msg, sizeof(error_msg), ERROR_NO_RETURN,
					 f->identifier->name);
			mCc_add_error(error_msg, f->identifier->node.sloc.start_line,
						  h_result);
		}
	}

	if (current_fun->prev != NULL) {
		ast_current_fun *temp = current_fun;
		current_fun = current_fun->prev;
		temp->prev = NULL;
		delete_current_fun(temp);
		current_fun->next = NULL;
	}
}

static void ast_symbol_table_compound_stmt(struct mCc_ast_compound_stmt *c_stmt,
					   void *data)
{
	assert(c_stmt);
	assert(data);

	ast_symbol_table *new = create_new_symbol_table_node();
	table->next = new;
	new->prev = table;
	table = new;
}

static void
ast_symbol_table_close_compound_stmt(struct mCc_ast_compound_stmt *c_stmt,
				     void *data)
{
	assert(c_stmt);
	assert(data);

	if (table->prev != NULL) {
		ast_symbol_table *temp = table;
		table = table->prev;
		temp->prev = NULL;
		delete_symbol_table_node(temp);
		table->next = NULL;
	}
}

static void ast_symbol_close_if_stmt(struct mCc_ast_if_stmt *stmt, void *data)
{
	assert(stmt);
	assert(data);


	if (current_fun->in_if) {
		if (!current_fun->has_else) {
			current_fun->has_ret = false;
		}
		if (current_fun->has_ret) {
			if (current_fun->prev != NULL) {
				ast_current_fun *temp = current_fun;
				current_fun = current_fun->prev;
				temp->prev = NULL;
				delete_current_fun(temp);
				current_fun->next = NULL;
			}
			ast_current_fun *n_fun = create_new_current_fun_node();
			n_fun->type = current_fun->type;
			n_fun->prev = current_fun;
			n_fun->identifier = current_fun->identifier;
			n_fun->in_if = false;
			n_fun->in_else = true;
			current_fun->next = n_fun;
			current_fun = n_fun;
		} else {
			if (current_fun->has_else) {
				ast_current_fun *n_fun =
					create_new_current_fun_node();
				n_fun->type = current_fun->type;
				n_fun->prev = current_fun;
				n_fun->identifier = current_fun->identifier;
				n_fun->in_if = false;
				n_fun->in_else = true;
				n_fun->missing_if_ret = true;
				current_fun->next = n_fun;
				current_fun = n_fun;
			} else {
				if (current_fun->prev != NULL) {
					ast_current_fun *temp = current_fun;
					current_fun = current_fun->prev;
					temp->prev = NULL;
					delete_current_fun(temp);
					current_fun->next = NULL;
				}
			}
		}
	}
}

static void ast_symbol_table_ass_stmt(struct mCc_ast_assignment *stmt,
				      void *data)
{
	assert(stmt);
	assert(data);
	char *new_name = find_element_symbols(table, stmt->identifier->name);
	ast_symbol_table *temp = table;

	while (new_name == NULL && temp->prev != NULL) {
		temp = temp->prev;
		new_name = find_element_symbols(temp, stmt->identifier->name);
	}
	if (new_name == NULL) {
		if(add_errors) {
			char error_msg[1024] = {0};
			snprintf(error_msg, sizeof(error_msg),
					 ERROR_MISSING_VARIABLE_DEF, stmt->identifier->name);
			mCc_add_error(error_msg, stmt->identifier->node.sloc.start_line,
						  h_result);
		}
	} else {
		free(stmt->identifier->renamed);
		stmt->identifier->renamed = copy_string(new_name);
		stmt->identifier->size = find_element_symbol_size(temp, stmt->identifier->name);
	}
}

static void add_decl_name_to_table_single(struct mCc_ast_declaration *stmt) {
	switch (stmt->literal) {
		case (MCC_AST_LITERAL_TYPE_INT):
			table = add_element_symbols(
					table, stmt->identifier->name,
					stmt->identifier->renamed,
					MCC_AST_TYPE_INT, 0, NULL, 1);
			break;
		case (MCC_AST_LITERAL_TYPE_FLOAT):
			table = add_element_symbols(
					table, stmt->identifier->name,
					stmt->identifier->renamed,
					MCC_AST_TYPE_FLOAT, 0, NULL, 1);
			break;
		case (MCC_AST_LITERAL_TYPE_STRING):
			table = add_element_symbols(
					table, stmt->identifier->name,
					stmt->identifier->renamed,
					MCC_AST_TYPE_STRING, 0, NULL, 1);
			break;
		case (MCC_AST_LITERAL_TYPE_BOOL):
			table = add_element_symbols(
					table, stmt->identifier->name,
					stmt->identifier->renamed,
					MCC_AST_TYPE_BOOL, 0, NULL, 1);
			break;
	}
}

static void single_declaration(struct mCc_ast_declaration *stmt) {
	char help[ARRAY_SIZE] = {0};
	sprintf(help, "%s%d", stmt->identifier->name, g_counter++);
	if (find_element_symbols(table, stmt->identifier->name)
		!= NULL) {
		if(add_errors) {
			char error_msg[1024] = {0};
			snprintf(error_msg, sizeof(error_msg),
					 ERROR_DUBLICATE_VARIABLE,
					 stmt->identifier->name);
			mCc_add_error(error_msg,
						  stmt->identifier->node.sloc.start_line,
						  h_result);
		}
	} else {
		free(stmt->identifier->renamed);
		stmt->identifier->renamed = copy_string(help);
		add_decl_name_to_table_single(stmt);
	}
}

static void add_decl_name_to_table_array(struct mCc_ast_declaration *stmt) {
	switch (stmt->literal) {
		case (MCC_AST_LITERAL_TYPE_INT):
			table = add_element_symbols(
					table, stmt->array_identifier->name,
					stmt->array_identifier->renamed,
					MCC_AST_TYPE_INT_ARRAY, 0, NULL, stmt->numerator);
			break;
		case (MCC_AST_LITERAL_TYPE_FLOAT):
			table = add_element_symbols(
					table, stmt->array_identifier->name,
					stmt->array_identifier->renamed,
					MCC_AST_TYPE_FLOAT_ARRAY, 0, NULL, stmt->numerator);
			break;
		case (MCC_AST_LITERAL_TYPE_STRING):
			table = add_element_symbols(
					table, stmt->array_identifier->name,
					stmt->array_identifier->renamed,
					MCC_AST_TYPE_STRING_ARRAY, 0, NULL, stmt->numerator);
			break;
		case (MCC_AST_LITERAL_TYPE_BOOL):
			table = add_element_symbols(
					table, stmt->array_identifier->name,
					stmt->array_identifier->renamed,
					MCC_AST_TYPE_BOOL_ARRAY, 0, NULL, stmt->numerator);
			break;
	}
}

static void array_declaration(struct mCc_ast_declaration *stmt) {
	char help[ARRAY_SIZE] = {0};
	sprintf(help, "%s%d", stmt->array_identifier->name,
			g_counter++);
	if (find_element_symbols(table, stmt->array_identifier->name)
		!= NULL) {
		if(add_errors) {
			char error_msg[1024] = {0};
			snprintf(error_msg, sizeof(error_msg),
					 ERROR_DUBLICATE_VARIABLE,
					 stmt->identifier->name);
			mCc_add_error(error_msg,
						  stmt->identifier->node.sloc.start_line,
						  h_result);
		}
	} else {
		free(stmt->array_identifier->renamed);
		stmt->array_identifier->renamed = copy_string(help);
		add_decl_name_to_table_array(stmt);
	}
}

static void ast_symbol_table_decl_stmt(struct mCc_ast_declaration *stmt,
				       void *data)
{
	assert(stmt);
	assert(data);
	switch (stmt->type) {
		case (MCC_AST_DECLARATION_TYPE_SINGLE):
			single_declaration(stmt);
			break;
		case (MCC_AST_DECLARATION_TYPE_ARRAY):
			array_declaration(stmt);
			break;
	}
}

static void ast_symbol_table_ret_stmt(struct mCc_ast_ret_stmt *stmt, void *data)
{
	assert(stmt);
	assert(data);

	current_fun->has_ret = true;
	stmt->d_type = current_fun->type;
}

static void ast_symbol_table_if_stmt(struct mCc_ast_if_stmt *stmt, void *data)
{
	assert(stmt);
	assert(data);

	ast_current_fun *n_fun = create_new_current_fun_node();
	if (stmt->else_statement != NULL) {
		n_fun->has_else = true;
	} else {
		n_fun->has_else = false;
	}
	n_fun->type = current_fun->type;
	n_fun->prev = current_fun;
	n_fun->identifier = current_fun->identifier;
	n_fun->in_if = true;
	current_fun->next = n_fun;
	current_fun = n_fun;
}

static void ast_symbol_table_close_stmt(struct mCc_ast_stmt *stmt, void *data)
{
	assert(stmt);
	assert(data);
	if (current_fun->in_else) {
		if (current_fun->has_ret) {
			current_fun->in_if = false;
			current_fun->in_else = false;
		}
		if (current_fun->prev != NULL) {
			ast_current_fun *temp = current_fun;
			if (current_fun->missing_if_ret)
				current_fun->has_ret = false;
			current_fun = current_fun->prev;
			current_fun->has_ret = temp->has_ret;
			temp->prev = NULL;
			delete_current_fun(temp);
			current_fun->next = NULL;
		}
	}
}

static void
ast_symbol_table_expression_single(struct mCc_ast_single_expression *expression,
				   void *data)
{
	assert(expression);
	assert(data);

	if (expression->type == MCC_AST_SINGLE_EXPRESSION_TYPE_IDENTIFIER
	    || expression->type
		       == MCC_AST_SINGLE_EXPRESSION_TYPE_IDENTIFIER_EX) {
		char *new_name = find_element_symbols(
			table, expression->identifier->name);
		ast_symbol_table *temp = table;

		while (new_name == NULL && temp->prev != NULL) {
			temp = temp->prev;
			new_name = find_element_symbols(
				temp, expression->identifier->name);
		}
		if (new_name == NULL) {
			if(add_errors) {
				char error_msg[1024] = {0};
				snprintf(error_msg, sizeof(error_msg),
						 ERROR_MISSING_VARIABLE_DEF,
						 expression->identifier->name);
				mCc_add_error(
						error_msg,
						expression->identifier->node.sloc.start_line,
						h_result);
			}
		} else {
			free(expression->identifier->renamed);
			expression->identifier->renamed = copy_string(new_name);
			expression->identifier->size = find_element_symbol_size(
					temp, expression->identifier->name);
		}
	}
}

static void
ast_symbol_table_call_expression(struct mCc_ast_call_expr *expression,
				 void *data)
{
	assert(expression);
	assert(data);

	/* Num Parameter check*/

	int num_args =
		find_element_param_num(table, expression->identifier->name);
	if (expression->arguments != NULL) {
		if (expression->arguments->counter != num_args) {
			if(add_errors) {
				char error_msg[1024] = {0};
				snprintf(error_msg, sizeof(error_msg),
						 ERROR_NUM_ARGUMENTS,
						 expression->identifier->name, num_args,
						 expression->arguments->counter);
				mCc_add_error(
						error_msg,
						expression->identifier->node.sloc.start_line,
						h_result);
			}
		}
		enum mCc_ast_type *t_type = find_element_param_types(
			table, expression->identifier->name);
		int t_type_length = find_element_param_num(
			table, expression->identifier->name);
		if (t_type != NULL) {
			//if (expression->identifier->param_types!= NULL)
			//	free(expression->identifier->param_types);

            if (expression->identifier->param_types== NULL) {
                expression->identifier->param_types =
                        malloc(sizeof(enum mCc_ast_type) * t_type_length);
                memcpy(expression->identifier->param_types, t_type,
                       sizeof(enum mCc_ast_type) * t_type_length);
            }

		}

	} else if (num_args != 0) {
		if(add_errors) {
			char error_msg[1024] = {0};
			snprintf(error_msg, sizeof(error_msg), ERROR_NUM_ARGUMENTS,
					 expression->identifier->name, num_args, 0);
			mCc_add_error(error_msg,
						  expression->identifier->node.sloc.start_line,
						  h_result);
		}
	}

	char *new_name =
		find_element_symbols(table, expression->identifier->name);
	ast_symbol_table *temp = table;

	while (new_name == NULL && temp->prev != NULL) {
		temp = temp->prev;
		new_name = find_element_symbols(temp,
						expression->identifier->name);
	}
	if (new_name == NULL) {
		if(add_errors) {
			char error_msg[1024] = {0};
			snprintf(error_msg, sizeof(error_msg),
					 ERROR_MISSING_FUNCTION_DEF,
					 expression->identifier->name);
			mCc_add_error(error_msg,
						  expression->identifier->node.sloc.start_line,
						  h_result);
		}
	} else {
		free(expression->identifier->renamed);
		expression->identifier->renamed = copy_string(new_name);
	}
}

struct mCc_parser_result *mCc_ast_symbol_table(struct mCc_parser_result *result)
{
	assert(result);
	add_errors = false;

	h_result = result;
	struct mCc_ast_function_def_array *f = result->func_def;

	g_counter = 0;
	table = create_new_symbol_table_node();
	has_main = false;

	current_fun = create_new_current_fun_node();

	struct mCc_ast_visitor visitor = symbol_table_visitor(table);
	mCc_ast_symbol_table_add_default_function_names();
	mCc_ast_visit_function_def_array(f, &visitor);

	add_errors = true;
	mCc_ast_visit_function_def_array(f, &visitor);

	delete_symbol_table_node(table);

	if (!has_main) {
		mCc_add_error(ERROR_NO_MAIN, 0, h_result);
	}

	delete_current_fun(current_fun);

	return h_result;
}