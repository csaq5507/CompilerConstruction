
#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include "mCc/ast_symbol_table.h"
#include "mCc/parser.h"

#define DEBUG 0

static ast_symbol_table *create_new_symbol_table_node();
static void mCc_ast_symbol_table_add_default_function_names();
static ast_symbol_table *add_element_symbols(ast_symbol_table *head, char *old,
					     char *new);
static char *find_element_symbols(ast_symbol_table *head, char *elem);
static void delete_symbol_table_node(ast_symbol_table *head);

static void ast_symbol_table_func_type(struct mCc_ast_function_def *f,
				       void *data);
static void ast_symbol_table_func_void(struct mCc_ast_function_def *f,
				       void *data);
static void ast_symbol_table_parameter(struct mCc_ast_parameter *param,
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

int counter= 0;

static ast_symbol_table *add_element_symbols(ast_symbol_table *head, char *old,
					     char *new)
{
	assert(head);
	assert(old);
	assert(new);

    counter++;
	ast_symbol *symbol = malloc(sizeof(*symbol));
	symbol->mCc_symbol_old = malloc(sizeof(char *) * strlen(old));
	symbol->mCc_symbol_new = malloc(sizeof(char *) * strlen(new));
	strcpy(symbol->mCc_symbol_old, old);
	strcpy(symbol->mCc_symbol_new, new);


	ast_symbol *temp = realloc(
		head->symbols, sizeof(*symbol) * (head->symbols_counter + 1));
	if (temp == NULL) {
		return NULL;
	}
	head->symbols = temp;

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
		if (strcmp(head->symbols[i].mCc_symbol_old, elem) == 0) {
			return head->symbols[i].mCc_symbol_new;
		}
	}
	return NULL;
}

static void delete_symbol_table_node(ast_symbol_table *head)
{
	while(head->prev != NULL)
		head=head->prev;


    ast_symbol_table *current = head;
    while( current != NULL ) {
        ast_symbol_table *next = current->next;
        if (current->symbols != NULL) {
            free(current->symbols->mCc_symbol_new);
            free(current->symbols->mCc_symbol_old);
            free(current->symbols);
        }
        free( current );
        current = next;
    }
}

static void ast_symbol_table_func_type(struct mCc_ast_function_def *f,
				       void *data)
{
	assert(f);
	assert(data);

    char help[2048];
    sprintf(help, "%s%d", f->identifier->name, g_counter++);
    if (find_element_symbols(table, f->identifier->name) != NULL) {
        char error_msg[1024] = {0};
        snprintf(error_msg, sizeof(error_msg), "Allready defined: %s", f->identifier->name);
        struct mCc_parser_error *error =  malloc(sizeof(struct mCc_parser_error));
        strcpy(error->error_msg, error_msg);
        error->error_line = 0;
        h_result->errors = add_parse_error(h_result->errors, error);
        h_result->status = MCC_PARSER_STATUS_ERROR;
    } else {
        table = add_element_symbols(table, f->identifier->name,
                                    help);

        char *temp = realloc(f->identifier->renamed,
                             (sizeof(char *) * strlen(help)));
        if (temp == NULL)
            assert(NULL);
        f->identifier->renamed = temp;
        strcpy(f->identifier->renamed, help);


    }
}

static void ast_symbol_table_func_void(struct mCc_ast_function_def *f,
				       void *data)
{
	assert(f);
	assert(data);

    char help[2048];
    sprintf(help, "%s%d", f->identifier->name, g_counter++);
    if (find_element_symbols(table, f->identifier->name) != NULL) {
        char error_msg[1024] = {0};
        snprintf(error_msg, sizeof(error_msg), "Allready defined: %s", f->identifier->name);
        struct mCc_parser_error *error =  malloc(sizeof(struct mCc_parser_error));
        strcpy(error->error_msg, error_msg);
        error->error_line = 0;
        h_result->errors = add_parse_error(h_result->errors, error);
        h_result->status = MCC_PARSER_STATUS_ERROR;
    } else {
        table = add_element_symbols(table, f->identifier->name,
                                    help);
        char *temp = realloc(f->identifier->renamed,
                             (sizeof(char *) * strlen(help)));
        if (temp == NULL) {
            assert(NULL);
        }
        f->identifier->renamed = temp;
        strcpy(f->identifier->renamed, help);
    }

    if (strcmp(f->identifier->name, "main") == 1) {
        has_main = true;
    }
}

static void ast_symbol_table_close_func(struct mCc_ast_function_def *f,
					void *data)
{
	assert(f);
	assert(data);

	if(DEBUG)
		printf("func close\n");
	if (table->prev != NULL) {
		ast_symbol_table *temp = table;
		table = table->prev;
		temp->prev=NULL;
		delete_symbol_table_node(temp);
		table->next = NULL;
	}
}

static void ast_symbol_table_compound_stmt(struct mCc_ast_compound_stmt *c_stmt,
					   void *data)
{
	assert(c_stmt);
	assert(data);

	if(DEBUG)
		printf("cmp stmt\n");
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

	if(DEBUG)
		printf("close cmp stmt\n");
	if (table->prev != NULL) {
		ast_symbol_table *temp = table;
		table = table->prev;
		temp->prev=NULL;
		delete_symbol_table_node(temp);
		table->next = NULL;
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
        char error_msg[1024] = {0};
        snprintf(error_msg, sizeof(error_msg), "Missing definition of: %s", stmt->identifier->name);
        struct mCc_parser_error *error =  malloc(sizeof(struct mCc_parser_error));
        strcpy(error->error_msg, error_msg);
        error->error_line = 0;
        h_result->errors = add_parse_error(h_result->errors, error);
        h_result->status = MCC_PARSER_STATUS_ERROR;
	} else {
		char *temp = realloc(stmt->identifier->renamed,
				     sizeof(char *) * strlen(new_name) );
        if (temp == NULL) {
            assert(NULL);
        }
		stmt->identifier->renamed = temp;
		strcpy(stmt->identifier->renamed, new_name);
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
		sprintf(help, "%s%d", stmt->identifier->name, g_counter++);
		if (find_element_symbols(table, stmt->identifier->name) != NULL) {
            char error_msg[1024] = {0};
            snprintf(error_msg, sizeof(error_msg), "Allready defined: %s", stmt->identifier->name);
            struct mCc_parser_error *error =  malloc(sizeof(struct mCc_parser_error));
            strcpy(error->error_msg, error_msg);
            error->error_line = 0;
            h_result->errors = add_parse_error(h_result->errors, error);
            h_result->status = MCC_PARSER_STATUS_ERROR;
		} else {
			table = add_element_symbols(table, stmt->identifier->name,
						    help);
			char *temp = realloc(stmt->identifier->renamed,
					     (sizeof(char *) * strlen(help)));
            if (temp == NULL) {
                assert(NULL);
            }
			stmt->identifier->renamed = temp;
			strcpy(stmt->identifier->renamed, help);
		}
		break;
	case (MCC_AST_DECLARATION_TYPE_ARRAY):
		sprintf(help, "%s%d", stmt->array_identifier->name,
			g_counter++);
		if (find_element_symbols(table, stmt->array_identifier->name) != NULL) {
            char error_msg[1024] = {0};
            snprintf(error_msg, sizeof(error_msg), "Allready defined: %s", stmt->array_identifier->name);
            struct mCc_parser_error *error =  malloc(sizeof(struct mCc_parser_error));
            strcpy(error->error_msg, error_msg);
            error->error_line = 0;
            h_result->errors = add_parse_error(h_result->errors, error);
            h_result->status = MCC_PARSER_STATUS_ERROR;
		} else {
			table = add_element_symbols(
				table, stmt->array_identifier->renamed, help);
			char *temp = realloc(stmt->array_identifier->renamed,
					     (sizeof(char *) * strlen(help)));
            if (temp == NULL) {
                assert(NULL);
            }
			stmt->array_identifier->renamed = temp;
			strcpy(stmt->array_identifier->renamed, help);
		}
		break;
	}

}

static void ast_symbol_table_parameter(struct mCc_ast_parameter *params,
				       void *data)
{
	assert(params);
	assert(data);


}

static void
ast_symbol_table_expression_single(struct mCc_ast_single_expression *expression,
				   void *data)
{
	assert(expression);
	assert(data);

	if (expression->type == MCC_AST_SINGLE_EXPRESSION_TYPE_IDENTIFIER) {
		char *new_name =
			find_element_symbols(table, expression->identifier->name);
		ast_symbol_table *temp = table;

		while (new_name == NULL && temp->prev != NULL) {
			temp = temp->prev;
			new_name = find_element_symbols(temp,
							expression->identifier->name);
		}
		if (new_name == NULL) {
            char error_msg[1024] = {0};
            snprintf(error_msg, sizeof(error_msg), "Missing definition of: %s", expression->identifier->name);
            struct mCc_parser_error *error =  malloc(sizeof(struct mCc_parser_error));
            strcpy(error->error_msg, error_msg);
            error->error_line = 0;
            h_result->errors = add_parse_error(h_result->errors, error);
            h_result->status = MCC_PARSER_STATUS_ERROR;
		} else {
			char *temp = realloc(expression->identifier->renamed,
					     sizeof(char *) * strlen(new_name));
			if (temp == NULL) {
                assert(NULL);
            }
			expression->identifier->renamed = temp;
			strcpy(expression->identifier->renamed, new_name);
		}
	}
}

static void
ast_symbol_table_call_expression(struct mCc_ast_call_expr *expression,
                                 void *data){
    assert(expression);
    assert(data);

    char *new_name =
            find_element_symbols(table, expression->identifier->name);
    ast_symbol_table *temp = table;

    while (new_name == NULL && temp->prev != NULL) {
        temp = temp->prev;
        new_name = find_element_symbols(temp,
                                        expression->identifier->name);
    }
    if (new_name == NULL) {
        char error_msg[1024] = {0};
        snprintf(error_msg, sizeof(error_msg), "Missing definition of: %s", expression->identifier->name);
        struct mCc_parser_error *error =  malloc(sizeof(struct mCc_parser_error));
        strcpy(error->error_msg, error_msg);
        error->error_line = 0;
        h_result->errors = add_parse_error(h_result->errors, error);
        h_result->status = MCC_PARSER_STATUS_ERROR;
    } else {
        char *temp = realloc(expression->identifier->renamed,
                             sizeof(char *) * strlen(new_name));
        if (temp == NULL) {
            assert(NULL);
        }
        expression->identifier->renamed = temp;
        strcpy(expression->identifier->renamed, new_name);
    }
}

struct mCc_parser_result *
mCc_ast_symbol_table(struct mCc_parser_result *result)
{
	assert(result);

    h_result = result;
    struct mCc_ast_function_def_array *f = result->func_def;

    g_counter = 0;
	table = create_new_symbol_table_node();
    has_main = false;

	struct mCc_ast_visitor visitor = symbol_table_visitor(table);
    mCc_ast_symbol_table_add_default_function_names();

	mCc_ast_visit_function_def_array(f, &visitor);
    printf("%d",counter);
	delete_symbol_table_node(table);
/*
    if(!has_main) {
        char error_msg[1024] = {0};
        snprintf(error_msg, sizeof(error_msg), "Missing main function");
        struct mCc_parser_error *error =  malloc(sizeof(struct mCc_parser_error));
        strcpy(error->error_msg, error_msg);
        error->error_line = 0;
        h_result->errors = add_parse_error(h_result->errors, error);
        h_result->status = MCC_PARSER_STATUS_ERROR;
    }
*/
	return h_result;
}