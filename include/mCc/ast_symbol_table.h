#ifndef MINICOMPILER_AST_SYMBOL_TABLE_H
#define MINICOMPILER_AST_SYMBOL_TABLE_H

#include <stdio.h>

#include "mCc/ast.h"
#include "ast_visit.h"
#include "parser.h"

#ifdef __cplusplus
extern "C" {
#endif


/* Forward Declarations */
typedef struct mCc_ast_symbol_table ast_symbol_table;
typedef struct mCc_ast_symbol ast_symbol;
typedef struct mCc_ast_current_func ast_current_fun;

struct mCc_parser_result *
mCc_ast_symbol_table(struct mCc_parser_result *result);


ast_symbol_table *table;
ast_current_fun *current_fun;
int g_counter;
struct mCc_parser_result *h_result;
bool has_main;

/* ###################### STRUCTS ###################### */

typedef struct mCc_ast_symbol_table {

	ast_symbol_table *prev;
	ast_symbol_table *next;

	int symbols_counter;
	ast_symbol *symbols;

} ast_symbol_table;

typedef struct mCc_ast_symbol {
	char *mCc_symbol_old;
	char *mCc_symbol_new;
	enum mCc_ast_type d_type;

	int func_param_counter;
	enum mCc_ast_type *l_types;
} ast_symbol;

typedef struct mCc_ast_current_func {
	struct mCc_ast_current_func *prev;
	struct mCc_ast_current_func *next;

	struct mCc_ast_identifier *identifier;

	enum mCc_ast_type type;
	bool has_ret;

	bool in_if;
	bool in_else;
} ast_current_fun;


#ifdef __cplusplus
}
#endif
#endif // MINICOMPILER_AST_SYMBOL_TABLE_H
