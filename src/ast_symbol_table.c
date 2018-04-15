
#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include "mCc/ast_symbol_table.h"

static ast_symbol_table* create_new_symbol_table_node();
static ast_symbol_table* add_element_symbol_table(ast_symbol_table *head,
                                                  ast_symbol_table *next);
static ast_symbol_table* add_element_symbols(ast_symbol_table *head,
                                             char *old, char *new);
static char* find_element_symbols(ast_symbol_table *head, char *elem);
static void delete_symbol_table_node(ast_symbol_table *head);

static void ast_symbol_table_func_type(struct mCc_ast_function_def *f,
                                       void *table);
static void ast_symbol_table_func_void(struct mCc_ast_function_def *f,
                                       void *table);
static void ast_symbol_table_identifier(char *identifier, void *data);

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

    };
};

static ast_symbol_table* create_new_symbol_table_node(){
    ast_symbol_table *table = malloc(sizeof(*table));
    table->next = NULL;
    table->next_counter = 0;
    table->prev = NULL;
    table->symbols_counter = 0;
    table->symbols = NULL;
    return table;
}

static ast_symbol_table* add_element_symbol_table(ast_symbol_table *head,
                                                  ast_symbol_table *next){
    assert(head);
    assert(next);

    ast_symbol_table *temp = realloc(head->next,
                             sizeof(*next) * (head->next_counter + 1));
    if (temp == NULL) {
        // TODO throw error
        return NULL;
    }
    head->next = temp;
    memcpy(&(head->next[head->next_counter]), next, sizeof(*next));

    head->next_counter++;

    return head;
}

static ast_symbol_table* add_element_symbols(ast_symbol_table *head,
                                             char *old, char *new){
    assert(head);
    assert(old);
    assert(new);

    ast_symbol *symbol = malloc(sizeof(*symbol));
    symbol->old = old;
    symbol->new = new;

    ast_symbol *temp = realloc(head->symbols,
                                    sizeof(*symbol) * (head->symbols_counter + 1));
    if (temp == NULL) {
        // TODO throw error
        return NULL;
    }
    head->symbols = temp;
    memcpy(&(head->symbols[head->symbols_counter]), symbol, sizeof(*symbol));

    head->symbols_counter++;

    return head;
}

static char* find_element_symbols(ast_symbol_table *head,
                                  char *elem){
    assert(head);
    assert(elem);

    for(int i = 0; i < head->symbols_counter; i++) {
        if (strcmp(head->symbols[i].old, elem) == 0)
            return head->symbols[i].new;
    }
    return NULL;
}

static void delete_symbol_table_node(ast_symbol_table *head){
    while(true) {
        if (head->prev != NULL) {
            printf("X1\n");
            head = head->prev;
        } else {
            printf("X2\n");
            break;
        }
    }
    printf("%d\n", head->next_counter);
    if (head->next_counter == 0) {
        printf("ENTERED\n");
        for (int i = 0; i < head->symbols_counter; i++)
            free(&head->symbols[i]);
        // TODO free does not work
        //free(head);
    } else {
        for (int i = 0; i < head->next_counter; i++) {
            delete_symbol_table_node(&head->next[i]);
        }
        free(head);
    }
}

static void ast_symbol_table_func_type(struct mCc_ast_function_def *f,
                                       void *data){
    assert(f);
    assert(data);

    printf("HERE TYPE\n");
    if (table->prev != NULL)
        printf("HALLO\n");
    ast_symbol_table *new = create_new_symbol_table_node();
    table = add_element_symbol_table(table, new);
    new->prev = table;
    table = new;
    if (table->prev != NULL)
        printf("BYE\n");
}

static void ast_symbol_table_func_void(struct mCc_ast_function_def *f,
                                       void *data){
    assert(f);
    assert(data);

    printf("HERE VOID\n");
    if (table->prev != NULL)
        printf("HALLO\n");
    ast_symbol_table *new = create_new_symbol_table_node();
    table = add_element_symbol_table(table, new);
    new->prev = table;
    table = new;
    if (table->prev != NULL)
        printf("BYE\n");
}

static void ast_symbol_table_identifier(char *identifier, void *data) {
    assert(identifier);
    assert(data);

    char help[64];
    sprintf(help, "%s%d", identifier, table->symbols_counter);
    if (find_element_symbols(table, help) == NULL) {
        printf("ALLREADY\n");
        //TODO throw error because already declared
    }
    else
        table = add_element_symbols(table, identifier, help);
}

struct mCc_ast_function_def_array* mCc_ast_symbol_table
        (struct mCc_ast_function_def_array *f){
    assert(f);


    table = create_new_symbol_table_node();
    struct mCc_ast_visitor visitor = symbol_table_visitor(table);

    mCc_ast_visit_function_def_array(f, &visitor);

    delete_symbol_table_node(table);

    return f;
}