
#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include "mCc/ast_symbol_table.h"

#define DEBUG 0
#define ARRAY_SIZE 2048

static ast_symbol_table *create_new_symbol_table_node();
static void mCc_ast_symbol_table_add_default_function_names();
static ast_symbol_table *add_element_symbols(ast_symbol_table *head, char *old,
					     char *new, enum mCc_ast_type d_type, int num_params);
static char *find_element_symbols(ast_symbol_table *head, char *elem);
static enum mCc_ast_type find_identifier_type(ast_symbol_table *head, char *elem);
static int find_element_param_num(ast_symbol_table *head, char *elem);
static void delete_symbol_table_node(ast_symbol_table *head);

static ast_current_fun *create_new_current_fun_node();
static void delete_current_fun(ast_current_fun *head);

static void ast_symbol_table_identifier(struct mCc_ast_identifier* identifier, void *data);

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

static void ast_symbol_close_if_stmt(struct mCc_ast_if_stmt *stmt,
                            void *data);
static void ast_symbol_table_ass_stmt(struct mCc_ast_assignment *stmt,
				      void *data);
static void ast_symbol_table_decl_stmt(struct mCc_ast_declaration *stmt,
				       void *data);
static void ast_symbol_table_ret_stmt(struct mCc_ast_ret_stmt *stmt,
                        void *data);
static void ast_symbol_table_if_stmt(struct mCc_ast_if_stmt *stmt,
                        void *data);
static void ast_symbol_table_close_stmt(struct mCc_ast_stmt *stmt,
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

        .identifier = ast_symbol_table_identifier,

		.function_def_type = ast_symbol_table_func_type,
		.function_def_void = ast_symbol_table_func_void,
	    .close_function_def = ast_symbol_table_close_func,
		.parameter = ast_symbol_table_parameter,

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

    table = add_element_symbols(table, print, print, MCC_AST_TYPE_VOID, 1);
    table = add_element_symbols(table, print_nl, print_nl, MCC_AST_TYPE_VOID, 0);
    table = add_element_symbols(table, print_int, print_int, MCC_AST_TYPE_VOID, 1);
    table = add_element_symbols(table, print_float, print_float, MCC_AST_TYPE_VOID, 1);
    table = add_element_symbols(table, read_int, read_int, MCC_AST_TYPE_INT, 0);
    table = add_element_symbols(table, read_float, read_float, MCC_AST_TYPE_FLOAT, 0);

}


static ast_symbol_table *add_element_symbols(ast_symbol_table *head, char *old,
					     char *new, enum mCc_ast_type d_type, int num_params)
{
	assert(head);
	assert(old);
	assert(new);

	ast_symbol *symbol = malloc(sizeof(*symbol));
	symbol->mCc_symbol_old = old;
	symbol->mCc_symbol_new = new;
    symbol->d_type = d_type;
    symbol->func_param_counter = num_params;


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

static enum mCc_ast_type find_identifier_type(ast_symbol_table *head, char *elem)
{
    assert(head);
    assert(elem);
    while (head != NULL) {
        for (int i = 0; i < head->symbols_counter; i++) {
            if (strcmp(head->symbols[i].mCc_symbol_old, elem) == 0) {
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
            if (strcmp(head->symbols[i].mCc_symbol_old, elem) == 0) {
                return head->symbols[i].func_param_counter;
            }
        }
        head = head->prev;
    }
    return 0;
}

static void delete_symbol_table_node(ast_symbol_table *head)
{
	while(head->prev != NULL)
		head=head->prev;


    ast_symbol_table *current = head;
    while( current != NULL ) {
        ast_symbol_table *next = current->next;
        if (current->symbols != NULL) {
            free(current->symbols);
        }
        free( current );
        current = next;
    }
}

static ast_current_fun *create_new_current_fun_node() {
    ast_current_fun *elem = malloc(sizeof(*elem));
    elem->prev = NULL;
    elem->next = NULL;
    elem->has_ret = false;
    elem->type = MCC_AST_TYPE_VOID;
    elem->identifier = NULL;
    elem->in_if = false;
    elem->in_else = false;
    return elem;
}

static void delete_current_fun(ast_current_fun *head) {
    while(head->prev != NULL)
        head=head->prev;

    ast_current_fun *current = head;
    while( current != NULL ) {
        ast_current_fun *next = current->next;
        free( current );
        current = next;
    }
}

static void ast_symbol_table_identifier(struct mCc_ast_identifier* identifier, void *data) {
    assert(identifier);
    assert(data);

    enum mCc_ast_type d_type = find_identifier_type(table, identifier->name);
    identifier->d_type = d_type;
}


static void ast_symbol_table_func_type(struct mCc_ast_function_def *f,
				       void *data)
{
	assert(f);
	assert(data);

    if (strcmp(f->identifier->name, "main") == 0) {
        char error_msg[1024] = {0};
        snprintf(error_msg, sizeof(error_msg), "main function has return type");
        struct mCc_parser_error *error =  malloc(sizeof(struct mCc_parser_error));
        strcpy(error->error_msg, error_msg);
        error->error_line = f->identifier->node.sloc.start_line;
        h_result->errors = add_parse_error(h_result->errors, error);
        h_result->status = MCC_PARSER_STATUS_ERROR;

        if (has_main == true) {
            char error_msg[1024] = {0};
            snprintf(error_msg, sizeof(error_msg), "main function allready defined");
            struct mCc_parser_error *error =  malloc(sizeof(struct mCc_parser_error));
            strcpy(error->error_msg, error_msg);
            error->error_line = f->identifier->node.sloc.start_line;
            h_result->errors = add_parse_error(h_result->errors, error);
            h_result->status = MCC_PARSER_STATUS_ERROR;
        }
    }

    char help[ARRAY_SIZE] = {0};
    sprintf(help, "%s%d", f->identifier->name, g_counter++);
    if (find_element_symbols(table, f->identifier->name) != NULL) {
        char error_msg[1024] = {0};
        snprintf(error_msg, sizeof(error_msg), "Allready defined: %s", f->identifier->name);
        struct mCc_parser_error *error =  malloc(sizeof(struct mCc_parser_error));
        strcpy(error->error_msg, error_msg);
        error->error_line = f->identifier->node.sloc.start_line;
        h_result->errors = add_parse_error(h_result->errors, error);
        h_result->status = MCC_PARSER_STATUS_ERROR;
    } else {
        char *temp =
                realloc(f->identifier->renamed, sizeof(char *) * ARRAY_SIZE);
        if (temp == NULL) {
            // TODO throw error
            assert(NULL);
        }
        f->identifier->renamed = temp;
        strcpy(f->identifier->renamed, help);
        switch (f->l_type) {
            case (MCC_AST_LITERAL_TYPE_INT):
                table = add_element_symbols(table, f->identifier->name,
                                            f->identifier->renamed, MCC_AST_TYPE_INT, f->params->counter);
                break;
            case (MCC_AST_LITERAL_TYPE_FLOAT):
                table = add_element_symbols(table, f->identifier->name,
                                            f->identifier->renamed, MCC_AST_TYPE_FLOAT, f->params->counter);
                break;
            case (MCC_AST_LITERAL_TYPE_BOOL):
                table = add_element_symbols(table, f->identifier->name,
                                            f->identifier->renamed, MCC_AST_TYPE_BOOL, f->params->counter);
                break;
            case (MCC_AST_LITERAL_TYPE_STRING):
                table = add_element_symbols(table, f->identifier->name,
                                            f->identifier->renamed, MCC_AST_TYPE_STRING, f->params->counter);
                break;
        }

    }

    ast_symbol_table *new = create_new_symbol_table_node();
    table->next = new;
    new->prev = table;
    table = new;

    ast_current_fun *n_fun = create_new_current_fun_node();
    switch (f->l_type) {
        case(MCC_AST_LITERAL_TYPE_INT):
            n_fun->type = MCC_AST_TYPE_INT;
            break;
        case(MCC_AST_LITERAL_TYPE_FLOAT):
            n_fun->type = MCC_AST_TYPE_FLOAT;
            break;
        case(MCC_AST_LITERAL_TYPE_BOOL):
            n_fun->type = MCC_AST_TYPE_BOOL;
            break;
        case(MCC_AST_LITERAL_TYPE_STRING):
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

    if (strcmp(f->identifier->name, "main") == 0) {
        if (has_main == true) {
            char error_msg[1024] = {0};
            snprintf(error_msg, sizeof(error_msg), "main function allready defined");
            struct mCc_parser_error *error =  malloc(sizeof(struct mCc_parser_error));
            strcpy(error->error_msg, error_msg);
            error->error_line = f->identifier->node.sloc.start_line;
            h_result->errors = add_parse_error(h_result->errors, error);
            h_result->status = MCC_PARSER_STATUS_ERROR;
        }
        has_main = true;
    } else {
        char help[ARRAY_SIZE] = {0};
        sprintf(help, "%s%d", f->identifier->name, g_counter++);
        if (find_element_symbols(table, f->identifier->name) != NULL) {
            char error_msg[1024] = {0};
            snprintf(error_msg, sizeof(error_msg), "Allready defined: %s", f->identifier->name);
            struct mCc_parser_error *error =  malloc(sizeof(struct mCc_parser_error));
            strcpy(error->error_msg, error_msg);
            error->error_line = f->identifier->node.sloc.start_line;
            h_result->errors = add_parse_error(h_result->errors, error);
            h_result->status = MCC_PARSER_STATUS_ERROR;
        } else {
            char *temp =
                    realloc(f->identifier->renamed, sizeof(char *) * ARRAY_SIZE);
            if (temp == NULL) {
                // TODO throw error
                assert(NULL);
            }
            f->identifier->renamed = temp;
            strcpy(f->identifier->renamed, help);
            table = add_element_symbols(table, f->identifier->name,
                                        f->identifier->renamed, MCC_AST_TYPE_VOID, f->params->counter);
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

	if(DEBUG)
		printf("func close\n");
	if (table->prev != NULL) {
		ast_symbol_table *temp = table;
		table = table->prev;
		temp->prev=NULL;
		delete_symbol_table_node(temp);
		table->next = NULL;
	}

    if (f->type != MCC_AST_FUNCTION_DEF_TYPE_VOID && !current_fun->has_ret) {
        char error_msg[1024] = {0};
        snprintf(error_msg, sizeof(error_msg), "Missing return for function: %s", f->identifier->name);
        struct mCc_parser_error *error =  malloc(sizeof(struct mCc_parser_error));
        strcpy(error->error_msg, error_msg);
        error->error_line = f->identifier->node.sloc.start_line;
        h_result->errors = add_parse_error(h_result->errors, error);
        h_result->status = MCC_PARSER_STATUS_ERROR;
    }

    if(current_fun->prev != NULL) {
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

static void ast_symbol_close_if_stmt(struct mCc_ast_if_stmt *stmt,
                            void *data) {
    assert(stmt);
    assert(data);

    if (current_fun->in_if) {
        if(current_fun->has_ret) {
            if(current_fun->prev != NULL) {
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
            if(current_fun->prev != NULL) {
                ast_current_fun *temp = current_fun;
                current_fun = current_fun->prev;
                temp->prev = NULL;
                delete_current_fun(temp);
                current_fun->next = NULL;
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
        char error_msg[1024] = {0};
        snprintf(error_msg, sizeof(error_msg), "Missing definition of: %s", stmt->identifier->name);
        struct mCc_parser_error *error =  malloc(sizeof(struct mCc_parser_error));
        strcpy(error->error_msg, error_msg);
        error->error_line = stmt->identifier->node.sloc.start_line;
        h_result->errors = add_parse_error(h_result->errors, error);
        h_result->status = MCC_PARSER_STATUS_ERROR;
	} else {
        char *temp =
                realloc(stmt->identifier->renamed, sizeof(char *) * strlen(new_name));
        if (temp == NULL) {
            // TODO throw error
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
	char help[ARRAY_SIZE] = {0};
	switch (stmt->type) {
	case (MCC_AST_DECLARATION_TYPE_SINGLE):
		sprintf(help, "%s%d", stmt->identifier->name, g_counter++);
		if (find_element_symbols(table, stmt->identifier->name) != NULL) {
            char error_msg[1024] = {0};
            snprintf(error_msg, sizeof(error_msg), "Allready defined: %s", stmt->identifier->name);
            struct mCc_parser_error *error =  malloc(sizeof(struct mCc_parser_error));
            strcpy(error->error_msg, error_msg);
            error->error_line =stmt->identifier->node.sloc.start_line;
            h_result->errors = add_parse_error(h_result->errors, error);
            h_result->status = MCC_PARSER_STATUS_ERROR;
		} else {
            char *temp =
                    realloc(stmt->identifier->renamed, sizeof(char *) * ARRAY_SIZE);
            if (temp == NULL) {
                // TODO throw error
                assert(NULL);
            }
            stmt->identifier->renamed = temp;
            strcpy(stmt->identifier->renamed, help);
            switch(stmt->literal){
                case (MCC_AST_LITERAL_TYPE_INT):
                    table = add_element_symbols(table, stmt->identifier->name,
                                                stmt->identifier->renamed, MCC_AST_TYPE_INT, 0);
                    break;
                case (MCC_AST_LITERAL_TYPE_FLOAT):
                    table = add_element_symbols(table, stmt->identifier->name,
                                                stmt->identifier->renamed, MCC_AST_TYPE_FLOAT, 0);
                    break;
                case (MCC_AST_LITERAL_TYPE_STRING):
                    table = add_element_symbols(table, stmt->identifier->name,
                                                stmt->identifier->renamed, MCC_AST_TYPE_STRING, 0);
                    break;
                case (MCC_AST_LITERAL_TYPE_BOOL):
                    table = add_element_symbols(table, stmt->identifier->name,
                                                stmt->identifier->renamed, MCC_AST_TYPE_BOOL, 0);
                    break;
            }
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
            error->error_line = stmt->array_identifier->node.sloc.start_line;
            h_result->errors = add_parse_error(h_result->errors, error);
            h_result->status = MCC_PARSER_STATUS_ERROR;
		} else {
            char *temp =
                    realloc(stmt->array_identifier->renamed, sizeof(char *) * strlen(help));
            if (temp == NULL) {
                // TODO throw error
                assert(NULL);
            }
            stmt->array_identifier->renamed = temp;
            strcpy(stmt->array_identifier->renamed, help);
            switch(stmt->literal){
                case (MCC_AST_LITERAL_TYPE_INT):
                    table = add_element_symbols(table, stmt->array_identifier->name,
                                                stmt->array_identifier->renamed, MCC_AST_TYPE_INT_ARRAY, 0);
                    break;
                case (MCC_AST_LITERAL_TYPE_FLOAT):
                    table = add_element_symbols(table, stmt->array_identifier->name,
                                                stmt->array_identifier->renamed, MCC_AST_TYPE_FLOAT_ARRAY, 0);
                    break;
                case (MCC_AST_LITERAL_TYPE_STRING):
                    table = add_element_symbols(table, stmt->array_identifier->name,
                                                stmt->array_identifier->renamed, MCC_AST_TYPE_STRING_ARRAY, 0);
                    break;
                case (MCC_AST_LITERAL_TYPE_BOOL):
                    table = add_element_symbols(table, stmt->array_identifier->name,
                                                stmt->array_identifier->renamed, MCC_AST_TYPE_BOOL_ARRAY, 0);
                    break;
            }
		}
		break;
	}

}

static void ast_symbol_table_ret_stmt(struct mCc_ast_ret_stmt *stmt,
                                      void *data){
    assert(stmt);
    assert(data);

    current_fun->has_ret = true;
    stmt->d_type = current_fun->type;
    /*if (stmt->expression->d_type != current_fun->type) {
        char error_msg[1024] = {0};
        snprintf(error_msg, sizeof(error_msg), "Return %s has wrong type", current_fun->identifier->name);
        struct mCc_parser_error *error =  malloc(sizeof(struct mCc_parser_error));
        strcpy(error->error_msg, error_msg);
        error->error_line = current_fun->identifier->node.sloc.start_line;
        h_result->errors = add_parse_error(h_result->errors, error);
        h_result->status = MCC_PARSER_STATUS_ERROR;
    }*/
}

static void ast_symbol_table_if_stmt(struct mCc_ast_if_stmt *stmt,
                                        void *data) {
    assert(stmt);
    assert(data);

    if (stmt->else_statement != NULL) {
        ast_current_fun *n_fun = create_new_current_fun_node();
        n_fun->type = current_fun->type;
        n_fun->prev = current_fun;
        n_fun->identifier = current_fun->identifier;
        n_fun->in_if = true;
        current_fun->next = n_fun;
        current_fun = n_fun;
    }
}

static void ast_symbol_table_close_stmt(struct mCc_ast_stmt *stmt,
                                        void *data) {
    assert(stmt);
    assert(data);
    if (current_fun->in_else) {
        if(current_fun->has_ret) {
            current_fun->in_if = false;
            current_fun->in_else = false;
        }
        if(current_fun->prev != NULL) {
            ast_current_fun *temp = current_fun;
            current_fun = current_fun->prev;
            current_fun->has_ret = temp->has_ret;
            temp->prev = NULL;
            delete_current_fun(temp);
            current_fun->next = NULL;
        }
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
            error->error_line = expression->identifier->node.sloc.start_line;
            h_result->errors = add_parse_error(h_result->errors, error);
            h_result->status = MCC_PARSER_STATUS_ERROR;
		} else {
            char *temp =
                    realloc(expression->identifier->renamed, sizeof(char *) * strlen(new_name));
            if (temp == NULL) {
                // TODO throw error
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

    /* Num Parameter check*/
    if (expression->arguments != NULL) {
        if (expression->arguments->counter != find_element_param_num(table, expression->identifier->name)) {
            char error_msg[1024] = {0};
            snprintf(error_msg, sizeof(error_msg), "Wrong number of arguments for functin call: %s", expression->identifier->name);
            struct mCc_parser_error *error =  malloc(sizeof(struct mCc_parser_error));
            strcpy(error->error_msg, error_msg);
            error->error_line = expression->identifier->node.sloc.start_line;
            h_result->errors = add_parse_error(h_result->errors, error);
            h_result->status = MCC_PARSER_STATUS_ERROR;
        }
    } else if (find_element_param_num(table, expression->identifier->name) != 0){
        char error_msg[1024] = {0};
        snprintf(error_msg, sizeof(error_msg), "Wrong number of arguments for functin call: %s", expression->identifier->name);
        struct mCc_parser_error *error =  malloc(sizeof(struct mCc_parser_error));
        strcpy(error->error_msg, error_msg);
        error->error_line = expression->identifier->node.sloc.start_line;
        h_result->errors = add_parse_error(h_result->errors, error);
        h_result->status = MCC_PARSER_STATUS_ERROR;
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
        char error_msg[1024] = {0};
        snprintf(error_msg, sizeof(error_msg), "Missing definition of: %s", expression->identifier->name);
        struct mCc_parser_error *error =  malloc(sizeof(struct mCc_parser_error));
        strcpy(error->error_msg, error_msg);
        error->error_line = expression->identifier->node.sloc.start_line;
        h_result->errors = add_parse_error(h_result->errors, error);
        h_result->status = MCC_PARSER_STATUS_ERROR;
    } else {
        char *temp =
                realloc(expression->identifier->renamed, sizeof(char *) * strlen(new_name));
        if (temp == NULL) {
            // TODO throw error
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

    current_fun = create_new_current_fun_node();

	struct mCc_ast_visitor visitor = symbol_table_visitor(table);
    mCc_ast_symbol_table_add_default_function_names();

	mCc_ast_visit_function_def_array(f, &visitor);
	delete_symbol_table_node(table);

    if(!has_main) {
        char error_msg[1024] = {0};
        snprintf(error_msg, sizeof(error_msg), "Missing main function");
        struct mCc_parser_error *error =  malloc(sizeof(struct mCc_parser_error));
        strcpy(error->error_msg, error_msg);
        error->error_line = 0;
        h_result->errors = add_parse_error(h_result->errors, error);
        h_result->status = MCC_PARSER_STATUS_ERROR;
    }

    delete_current_fun(current_fun);

	return h_result;
}