
#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include "mCc/ast_symbol_table.h"
#include "mCc/parser.h"

#define DEBUG 0

static ast_symbol_table *create_new_symbol_table_node();
static void mCc_ast_symbol_table_add_default_function_names();
static ast_symbol_table *add_element_symbol_table(ast_symbol_table *head,
						  ast_symbol_table *next);
static ast_symbol_table *add_element_symbols(ast_symbol_table *head, char *old,
					     char *new);
static char *find_element_symbols(ast_symbol_table *head, char *elem);
static void delete_symbol_table_node(ast_symbol_table *head);

static void ast_symbol_table_func_type(struct mCc_ast_function_def *f,
				       void *data);
static void ast_symbol_table_func_void(struct mCc_ast_function_def *f,
				       void *data);
static void ast_symbol_table_parameter(struct mCc_ast_declaration *declaration,
				       void *data);
static void ast_symbol_table_close_func(struct mCc_ast_function_def *f,
					void *data);

static void ast_symbol_table_compound_stmt(struct mCc_ast_compound_stmt *c_stmt,
					   void *data);
static void
ast_symbol_table_close_compound_stmt(struct mCc_ast_compound_stmt *c_stmt,
				     void *data);

static void ast_symbol_table_ass_stmt(struct mCc_ast_assignment *stmt,
				      void *data);
static void ast_symbol_table_decl_stmt(struct mCc_ast_declaration *stmt,
				       void *data);

static void
ast_symbol_table_expression_single(struct mCc_ast_single_expression *expression,
                                   void *data);

static void
ast_symbol_table_call_expression(struct mCc_ast_call_expr *expression,
                                   void *data);


static struct mCc_ast_visitor symbol_table_visitor(ast_symbol_table *data)
{
	assert(data);

	return (struct mCc_ast_visitor){
		.traversal = MCC_AST_VISIT_DEPTH_FIRST,
		.order = MCC_AST_VISIT_PRE_ORDER,

		.userdata = data,

		.function_def_type = ast_symbol_table_func_type,
		.function_def_void = ast_symbol_table_func_void,
		.close_function_def = ast_symbol_table_close_func,
		.parameter = ast_symbol_table_parameter,

		.c_stmt = ast_symbol_table_compound_stmt,
		.close_c_stmt = ast_symbol_table_close_compound_stmt,

		.ass_stmt = ast_symbol_table_ass_stmt,
		.decl_stmt = ast_symbol_table_decl_stmt,

		.single_expression = ast_symbol_table_expression_single,
        .call_expression = ast_symbol_table_call_expression,

	};
};

static ast_symbol_table *create_new_symbol_table_node()
{
	ast_symbol_table *table = malloc(sizeof(*table));
	table->next = NULL;
	table->next_counter = 0;
	table->prev = NULL;
	table->symbols_counter = 0;
	table->symbols = NULL;
	return table;
}

static void mCc_ast_symbol_table_add_default_function_names() {
    char *print = "print";
    char *print_nl = "print_nl";
    char *print_int = "print_int";
    char *print_float = "print_float";
    char *read_int = "read_int";
    char *read_float = "read_float";

    table = add_element_symbols(table, print, print);
    table = add_element_symbols(table, print_nl, print_nl);
    table = add_element_symbols(table, print_int, print_int);
    table = add_element_symbols(table, print_float, print_float);
    table = add_element_symbols(table, read_int, read_int);
    table = add_element_symbols(table, read_float, read_float);

}

static ast_symbol_table *add_element_symbol_table(ast_symbol_table *head,
						  ast_symbol_table *next)
{
	assert(head);
	assert(next);

	ast_symbol_table *temp =
		realloc(head->next, sizeof(*next) * (head->next_counter + 1));
	if (temp == NULL) {
		// TODO throw error
		return NULL;
	}
	head->next = temp;
	memcpy(&(head->next[head->next_counter]), next, sizeof(*next));

	head->next_counter++;

	return head;
}

static ast_symbol_table *add_element_symbols(ast_symbol_table *head, char *old,
					     char *new)
{
	assert(head);
	assert(old);
	assert(new);

	ast_symbol *symbol = malloc(sizeof(*symbol));
	symbol->mCc_symbol_old = malloc(sizeof(char *) * strlen(old));
	symbol->mCc_symbol_new = malloc(sizeof(char *) * strlen(new));
	strcpy(symbol->mCc_symbol_old, old);
	strcpy(symbol->mCc_symbol_new, new);

	ast_symbol *temp = realloc(
		head->symbols, sizeof(*symbol) * (head->symbols_counter + 1));
	if (temp == NULL) {
		// TODO throw error
		return NULL;
	}
	head->symbols = temp;
	memcpy(&(head->symbols[head->symbols_counter]), symbol,
	       sizeof(*symbol));

	head->symbols_counter++;

	return head;
}

static char *find_element_symbols(ast_symbol_table *head, char *elem)
{
	assert(head);
	assert(elem);

	for (int i = 0; i < head->symbols_counter; i++) {
		if (strcmp(head->symbols[i].mCc_symbol_old, elem) == 0) {
			return head->symbols[i].mCc_symbol_new;
		}
	}
	return NULL;
}

static void delete_symbol_table_node(ast_symbol_table *head)
{
	if (head->next_counter == 0) {
		free(head->symbols);
        free(head->next);
        //free(head);
	} else {
		for (int i = 0; i < head->next_counter; i++) {
			delete_symbol_table_node(&head->next[i]);
		}
        free(head->symbols);
        free(head->next);
		free(head);
	}
}

static void ast_symbol_table_func_type(struct mCc_ast_function_def *f,
				       void *data)
{
	assert(f);
	assert(data);

    char help[2048];
    sprintf(help, "%s%d", f->identifier, g_counter++);
    if (find_element_symbols(table, f->identifier) != NULL) {
        printf("Allready defined: %s\n", f->identifier);
        // TODO throw error because already declared
    } else {
        table = add_element_symbols(table, f->identifier,
                                    help);
        char *temp = realloc(f->identifier,
                             (sizeof(char *) * strlen(help)));
        if (temp == NULL)
            assert(NULL);
        f->identifier = temp;
        strcpy(f->identifier, help);
    }
}

static void ast_symbol_table_func_void(struct mCc_ast_function_def *f,
				       void *data)
{
	assert(f);
	assert(data);

    char help[2048];
    sprintf(help, "%s%d", f->identifier, g_counter++);
    if (find_element_symbols(table, f->identifier) != NULL) {
        printf("Allready defined: %s\n", f->identifier);
        // TODO throw error because already declared
    } else {
        table = add_element_symbols(table, f->identifier,
                                    help);
        char *temp = realloc(f->identifier,
                             (sizeof(char *) * strlen(help)));
        if (temp == NULL)
            assert(NULL);
        f->identifier = temp;
        strcpy(f->identifier, help);
    }
}

static void ast_symbol_table_close_func(struct mCc_ast_function_def *f,
					void *data)
{
	assert(f);
	assert(data);

	if(DEBUG)
		printf("func close\n");
	if (table->prev != NULL)
		table = table->prev;
}

static void ast_symbol_table_compound_stmt(struct mCc_ast_compound_stmt *c_stmt,
					   void *data)
{
	assert(c_stmt);
	assert(data);

	if(DEBUG)
		printf("cmp stmt\n");
	ast_symbol_table *new = create_new_symbol_table_node();
	table = add_element_symbol_table(table, new);
	new->prev = table;
	table = new;
}

static void
ast_symbol_table_close_compound_stmt(struct mCc_ast_compound_stmt *c_stmt,
				     void *data)
{
	assert(c_stmt);
	assert(data);

	if(DEBUG)
		printf("close cmp stmt\n");
	if (table->prev != NULL)
		table = table->prev;
}

static void ast_symbol_table_ass_stmt(struct mCc_ast_assignment *stmt,
				      void *data)
{
	assert(stmt);
	assert(data);

	char *new_name = find_element_symbols(table, stmt->identifier);
	ast_symbol_table *temp = table;

	while (new_name == NULL && temp->prev != NULL) {
		temp = temp->prev;
		new_name = find_element_symbols(temp, stmt->identifier);
	}
	if (new_name == NULL) {
		printf("Missing definition of: %s\n", stmt->identifier);
		// TODO throw error because already declared
	} else {
		char *temp = realloc(stmt->identifier,
				     sizeof(char *) * strlen(new_name));
		if (temp == NULL)
			assert(NULL);
		stmt->identifier = temp;
		strcpy(stmt->identifier, new_name);
	}
}

static void ast_symbol_table_decl_stmt(struct mCc_ast_declaration *stmt,
				       void *data)
{
	assert(stmt);
	assert(data);

	char help[2048];
	switch (stmt->type) {
	case (MCC_AST_DECLARATION_TYPE_SINGLE):
		sprintf(help, "%s%d", stmt->identifier, g_counter++);
		if (find_element_symbols(table, stmt->identifier) != NULL) {
			printf("Allready defined: %s\n", stmt->identifier);
			// TODO throw error because already declared
		} else {
			table = add_element_symbols(table, stmt->identifier,
						    help);
			char *temp = realloc(stmt->identifier,
					     (sizeof(char *) * strlen(help)));
			if (temp == NULL)
				assert(NULL);
			stmt->identifier = temp;
			strcpy(stmt->identifier, help);
		}
		break;
	case (MCC_AST_DECLARATION_TYPE_ARRAY):
		sprintf(help, "%s%d", stmt->array_identifier,
			g_counter++);
		if (find_element_symbols(table, stmt->identifier) != NULL) {
			printf("Allready defined: %s\n", stmt->identifier);
			// TODO throw error because already declared
		} else {
			table = add_element_symbols(
				table, stmt->array_identifier, help);
			char *temp = realloc(stmt->array_identifier,
					     (sizeof(char *) * strlen(help)));
			if (temp == NULL)
				assert(NULL);
			stmt->array_identifier = temp;
			strcpy(stmt->array_identifier, help);
		}
		break;
	}
}

static void ast_symbol_table_parameter(struct mCc_ast_declaration *declaration,
				       void *data)
{
	assert(declaration);
	assert(data);

	char help[2048];
	switch (declaration->type) {
	case (MCC_AST_DECLARATION_TYPE_SINGLE):
		sprintf(help, "%s%d", declaration->identifier,
			g_counter++);

		if (find_element_symbols(table, declaration->identifier)
		    != NULL) {
			printf("Allready defined: %s\n",
			       declaration->identifier);
			// TODO throw error because already declared
		} else {
			table = add_element_symbols(
				table, declaration->identifier, help);
			char *temp = realloc(declaration->identifier,
					     (sizeof(char *) * strlen(help)));
			if (temp == NULL)
				assert(NULL);
			declaration->identifier = temp;
			strcpy(declaration->identifier, help);
		}
		break;
	case (MCC_AST_DECLARATION_TYPE_ARRAY):
		sprintf(help, "%s%d", declaration->array_identifier,
                g_counter++);
		if (find_element_symbols(table, declaration->array_identifier)
		    != NULL) {
			printf("Allready defined: %s\n",
			       declaration->array_identifier);
			// TODO throw error because already declared
		} else {
			table = add_element_symbols(
				table, declaration->array_identifier, help);
			char *temp = realloc(declaration->array_identifier,
					     (sizeof(char *) * strlen(help)));
			if (temp == NULL)
				assert(NULL);
			declaration->array_identifier = temp;
			strcpy(declaration->array_identifier, help);
		}
		break;
	}
}

static void
ast_symbol_table_expression_single(struct mCc_ast_single_expression *expression,
				   void *data)
{
	assert(expression);
	assert(data);

	if (expression->type == MCC_AST_SINGLE_EXPRESSION_TYPE_IDENTIFIER) {
		char *new_name =
			find_element_symbols(table, expression->identifier);
		ast_symbol_table *temp = table;

		while (new_name == NULL && temp->prev != NULL) {
			temp = temp->prev;
			new_name = find_element_symbols(temp,
							expression->identifier);
		}
		if (new_name == NULL) {
			printf("Missing definition of: %s\n",
			       expression->identifier);
			// TODO throw error because already declared
		} else {
			char *temp = realloc(expression->identifier,
					     sizeof(char *) * strlen(new_name));
			if (temp == NULL)
				assert(NULL);
			expression->identifier = temp;
			strcpy(expression->identifier, new_name);
		}
	}
}

static void
ast_symbol_table_call_expression(struct mCc_ast_call_expr *expression,
                                 void *data){
    assert(expression);
    assert(data);

    char *new_name =
            find_element_symbols(table, expression->identifier);
    ast_symbol_table *temp = table;

    while (new_name == NULL && temp->prev != NULL) {
        temp = temp->prev;
        new_name = find_element_symbols(temp,
                                        expression->identifier);
    }
    if (new_name == NULL) {
        //struct mCc_parser_error *error =  malloc(sizeof(struct mCc_parser_error));
        //strcpy(error->error_msg,expression->identifier);
        //error->error_line = 0;
        //result->errors = add_parse_error(result->errors, error);

        printf("Missing definition of: %s\n",
               expression->identifier);
        // TODO throw error because already declared
    } else {
        char *temp = realloc(expression->identifier,
                             sizeof(char *) * strlen(new_name));
        if (temp == NULL)
            assert(NULL);
        expression->identifier = temp;
        strcpy(expression->identifier, new_name);
    }
}

struct mCc_parser_result *
mCc_ast_symbol_table(struct mCc_parser_result *result)
{
	assert(result);

    result = result;
    struct mCc_ast_function_def_array *f = result->func_def;

    g_counter = 0;
	table = create_new_symbol_table_node();
	struct mCc_ast_visitor visitor = symbol_table_visitor(table);
    mCc_ast_symbol_table_add_default_function_names();

	mCc_ast_visit_function_def_array(f, &visitor);

    while (true) {
        if (table->prev != NULL) {
            table = table->prev;
        } else {
            break;
        }
    }

	delete_symbol_table_node(table);

	return result;
}