
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

#include "mCc/ast.h"
#include <mCc/ast_visit.h>

#define DEBUG 0

/* --------------------------------------------------------------VISITOR */
static struct mCc_ast_visitor ast_delete_visitor(void *data)
{
	return (struct mCc_ast_visitor){
		.traversal = MCC_AST_VISIT_DEPTH_FIRST,
		.order = MCC_AST_VISIT_POST_ORDER,

		.userdata = data,

		.identifier = mCc_ast_delete_identifier,
		.expression = mCc_ast_delete_expression,
		.argument = mCc_ast_delete_argument,
		.statement = mCc_ast_delete_stmt,
		.literal = mCc_ast_delete_literal,
		.s_literal = mCc_ast_delete_literal_value,

            .parameter = mCc_ast_delete_parameter,
		.ass_stmt = mCc_ast_delete_assignment,
		.single_expression = mCc_ast_delete_single_expression,
		.decl_stmt = mCc_ast_delete_declaration,
		.if_stmt = mCc_ast_delete_if_stmt,
		.ret_stmt = mCc_ast_delete_ret_stmt,
		.c_stmt = mCc_ast_delete_compound_stmt,
		.while_stmt = mCc_ast_delete_while_stmt,
		.call_expression = mCc_ast_delete_call_expr,
		.function_def_type = mCc_ast_delete_function_def,
		.function_def_void = mCc_ast_delete_function_def,

	};
}

void mCc_ast_delete_identifier(ast_identifier *identifier, void *data)
{
	assert(identifier);
	assert(data);
    if (identifier->name != identifier->renamed) {
        free(identifier->name);
        free(identifier->renamed);
    } else {
        free(identifier->name);
    }
	if (DEBUG)
		printf("identifier: \n");
	free(identifier);
}

/* ---------------------------------------------------------------- Literals */
ast_identifier *mCc_ast_new_identifier(char *name, int line)
{
	assert(name);

	ast_identifier *identifier =
		(ast_identifier *)malloc(sizeof(ast_identifier));
	if (identifier == NULL)
		return NULL;

	identifier->name = name;
    identifier->renamed = malloc(sizeof(char *) * strlen(name));
	memcpy(identifier->renamed, name, sizeof(char *) * strlen(name));
	identifier->node.sloc.start_line = line;
	return identifier;
}

/* ---------------------------------------------------------------- Literals */
ast_literal *mCc_ast_new_literal_int(long value)
{
	ast_literal *lit = malloc(sizeof(*lit));
	if (!lit) {
		return NULL;
	}

	lit->type = MCC_AST_LITERAL_TYPE_INT;
	lit->i_value = value;
	return lit;
}

ast_literal *mCc_ast_new_literal_float(double value)
{
	ast_literal *lit = malloc(sizeof(*lit));
	if (!lit) {
		return NULL;
	}

	lit->type = MCC_AST_LITERAL_TYPE_FLOAT;
	lit->f_value = value;
	return lit;
}

ast_literal *mCc_ast_new_literal_bool(bool value)
{
	ast_literal *lit = malloc(sizeof(*lit));
	if (!lit) {
		return NULL;
	}

	lit->type = MCC_AST_LITERAL_TYPE_BOOL;
	lit->b_value = value;
	return lit;
}


char *replace(char const *const original, char const *const pattern,
	      char const *const replacement)
{
	size_t const replen = strlen(replacement);
	size_t const patlen = strlen(pattern);
	size_t const orilen = strlen(original);

	size_t patcnt = 0;
	const char *oriptr;
	const char *patloc;

	// find how many times the pattern occurs in the original string
	for (oriptr = original; (patloc = strstr(oriptr, pattern)) != NULL;
	     oriptr = patloc + patlen) {
		patcnt++;
	}

	{
		// allocate memory for the new string
		size_t const retlen = orilen + patcnt * (replen - patlen);
		char *const returned =
			(char *)malloc(sizeof(char) * (retlen + 1));

		if (returned != NULL) {
			// copy the original string,
			// replacing all the instances of the pattern
			char *retptr = returned;
			for (oriptr = original;
			     (patloc = strstr(oriptr, pattern)) != NULL;
			     oriptr = patloc + patlen) {
				size_t const skplen = patloc - oriptr;
				// copy the section until the occurence of the
				// pattern
				strncpy(retptr, oriptr, skplen);
				retptr += skplen;
				// copy the replacement
				strncpy(retptr, replacement, replen);
				retptr += replen;
			}
			// copy the rest of the string.
			strcpy(retptr, oriptr);
		}
		return returned;
	}
}

ast_literal *mCc_ast_new_literal_string(char *value)
{
	char *t = replace(value, "\\n", "enter");
	char *temp = replace(t, "\"", " ");
	free(t);
	ast_literal *lit = malloc(sizeof(*lit));
	if (!lit) {
		return NULL;
	}
	lit->s_value = malloc(sizeof(char *) * strlen(temp));
	lit->type = MCC_AST_LITERAL_TYPE_STRING;
	strcpy(lit->s_value, temp);
	free(temp);
	return lit;
}

void mCc_ast_delete_literal_value(ast_literal *literal, void *data)
{
	assert(literal);
	assert(data);

	free(literal->s_value);
	if (DEBUG)
		printf("literal_svalue\n");
}

void mCc_ast_delete_literal(ast_literal *literal, void *data)
{
	assert(literal);
	assert(data);


	free(literal);
	if (DEBUG)
		printf("literal\n");
}


/* ------------------------------------------------------------- Expressions */
ast_expr *mCc_ast_new_expression_single(ast_single_expr *single_expr)
{
	assert(single_expr);

	ast_expr *expr = malloc(sizeof(*expr));
	if (!expr) {
		return NULL;
	}

	expr->type = MCC_AST_EXPRESSION_TYPE_SINGLE;
	expr->single_expr = single_expr;
	return expr;
}

ast_expr *mCc_ast_new_expression_binary_op(enum mCc_ast_binary_op op,
					   ast_single_expr *lhs, ast_expr *rhs)
{
	assert(lhs);
	assert(rhs);

	ast_expr *expr = malloc(sizeof(*expr));
	if (!expr) {
		return NULL;
	}

	expr->type = MCC_AST_EXPRESSION_TYPE_BINARY;
	expr->op = op;
	expr->lhs = lhs;
	expr->rhs = rhs;
	return expr;
}

void mCc_ast_delete_expression(ast_expr *expression, void *data)
{
	assert(expression);
	assert(data);

	if (expression->type == MCC_AST_EXPRESSION_TYPE_SINGLE) {
		free(expression->single_expr);
		if (DEBUG)
			printf("single expr\n");

	} else {
		free(expression->lhs);
		if (DEBUG)
			printf("expr_lhs\n");

		free(expression->rhs);
		if (DEBUG)
			printf("expr_rhs\n");
	}
}

/* Single Expression */
ast_single_expr *mCc_ast_new_single_expression_literal(ast_literal *literal)
{
	assert(literal);

	ast_single_expr *expr = malloc(sizeof(*expr));
	if (!expr) {
		return NULL;
	}

	expr->type = MCC_AST_SINGLE_EXPRESSION_TYPE_LITERAL;
	expr->literal = literal;
	return expr;
}

ast_single_expr *
mCc_ast_new_single_expression_identifier(ast_identifier *identifier)
{
	assert(identifier);

	ast_single_expr *expr = malloc(sizeof(*expr));
	if (!expr) {
		return NULL;
	}
	expr->type = MCC_AST_SINGLE_EXPRESSION_TYPE_IDENTIFIER;
	expr->only_identifier = identifier;
	return expr;
}

ast_single_expr *
mCc_ast_new_single_expression_identifier_ex(ast_identifier *identifier,
					    ast_expr *identifier_expression)
{
	assert(identifier);
	assert(identifier_expression);

	ast_single_expr *expr = malloc(sizeof(*expr));
	if (!expr) {
		return NULL;
	}
	expr->type = MCC_AST_SINGLE_EXPRESSION_TYPE_IDENTIFIER_EX;
	expr->identifier = identifier;
	expr->identifier_expression = identifier_expression;
	return expr;
}

ast_single_expr *
mCc_ast_new_single_expression_call_expr(ast_call_expr *call_expr)
{
	assert(call_expr);

	ast_single_expr *expr = malloc(sizeof(*expr));
	if (!expr) {
		return NULL;
	}
	expr->type = MCC_AST_SINGLE_EXPRESSION_TYPE_CALL_EXPR;
	expr->call_expr = call_expr;
	return expr;
}

ast_single_expr *
mCc_ast_new_single_expression_unary_op(enum mCc_ast_unary_op unary_op,
				       ast_expr *unary_expression)
{
	assert(unary_expression);

	ast_single_expr *expr = malloc(sizeof(*expr));
	if (!expr) {
		return NULL;
	}
	expr->type = MCC_AST_SINGLE_EXPRESSION_TYPE_UNARY_OP;
	expr->unary_operator = unary_op;
	expr->unary_expression = unary_expression;
	return expr;
}

ast_single_expr *mCc_ast_new_single_expression_parenth(ast_expr *expression)
{
	assert(expression);

	ast_single_expr *expr = malloc(sizeof(*expr));
	if (!expr) {
		return NULL;
	}
	expr->type = MCC_AST_SINGLE_EXPRESSION_TYPE_PARENTH;
	expr->expression = expression;
	return expr;
}

void mCc_ast_delete_single_expression(ast_single_expr *expression, void *data)
{
	assert(expression);
	assert(data);

	switch (expression->type) {
	case MCC_AST_SINGLE_EXPRESSION_TYPE_IDENTIFIER_EX:
		free(expression->identifier_expression);
		if (DEBUG)
			printf("single_expression->ident_expr\n");
		break;
	case MCC_AST_SINGLE_EXPRESSION_TYPE_UNARY_OP:
		free(expression->unary_expression);
		if (DEBUG)
			printf("single_expression->unaryexpr\n");
		break;
	case MCC_AST_SINGLE_EXPRESSION_TYPE_PARENTH:
		free(expression->expression);
		if (DEBUG)
			printf("single_expression->parenthex\n");
		break;
	case MCC_AST_SINGLE_EXPRESSION_TYPE_CALL_EXPR:
		free(expression->call_expr);
		if (DEBUG)
			printf("single_expression->call_expr\n");
		break;
	case MCC_AST_SINGLE_EXPRESSION_TYPE_LITERAL:

	case MCC_AST_SINGLE_EXPRESSION_TYPE_IDENTIFIER:

		break;
	}
}

/* Call Expression */
ast_call_expr *mCc_ast_new_empty_call_expr(ast_identifier *identifier)
{
	ast_call_expr *call_expr = malloc(sizeof(*call_expr));
	call_expr->identifier = identifier;
	call_expr->arguments = NULL;
	return call_expr;
}

ast_call_expr *mCc_ast_new_call_expr(ast_identifier *identifier,
				     ast_argument *arguments)
{
	assert(arguments);

	ast_call_expr *call_expr = malloc(sizeof(*call_expr));
	call_expr->identifier = identifier;
	call_expr->arguments = arguments;
	return call_expr;
}

void mCc_ast_delete_call_expr(ast_call_expr *call_expr, void *data)
{
	assert(call_expr);
	assert(data);

	free(call_expr->arguments);
	if (DEBUG)
		printf("call_expr_arguments\n");
}


/* ----------------------------------------------------------- Function Def */
ast_function_def *mCc_ast_new_void_function_def(ast_identifier *identifier,
						ast_parameter *params,
						ast_compound_stmt *c_stmt)
{
	ast_function_def *f = malloc(sizeof(*f));
	if (!f) {
		return NULL;
	}
	f->type = MCC_AST_FUNCTION_DEF_TYPE_VOID;

	f->void_value = "void";
	f->identifier = identifier;
	f->params = params;
	f->c_stmt = c_stmt;
	return f;
}

ast_function_def *mCc_ast_new_type_function_def(enum mCc_ast_literal_type type,
						ast_identifier *identifier,
						ast_parameter *params,
						ast_compound_stmt *c_stmt)
{
	ast_function_def *f = malloc(sizeof(*f));
	if (!f) {
		return NULL;
	}

	f->type = MCC_AST_FUNCTION_DEF_TYPE_TYPE;

	f->l_type = type;
	f->identifier = identifier;
	f->params = params;
	f->c_stmt = c_stmt;
	return f;
}

ast_function_def_array *
mCc_ast_add_function_def_to_array(ast_function_def_array *f,
				  ast_function_def *f2)
{
	assert(f);
	assert(f2);

	ast_function_def *temp =
		realloc(f->function_def, sizeof(*f2) * (f->counter + 1));
	if (temp == NULL) {
		// TODO throw error
		return NULL;
	}
	f->function_def = temp;

	memcpy(&(f->function_def[f->counter]), f2, sizeof(*f2));
    free(f2);
	f->counter++;
	return f;
}

ast_function_def_array *mCc_ast_new_function_def_array(ast_function_def *f)
{
	assert(f);

	ast_function_def_array *function_array =
		(ast_function_def_array *)malloc(
			sizeof(ast_function_def_array));
	function_array->counter = 1;
	function_array->function_def = f;

	return function_array;
}

void mCc_ast_delete_function_def(ast_function_def *f, void *data)
{
	assert(f);
	assert(data);

	if (f->type == MCC_AST_FUNCTION_DEF_TYPE_VOID) {
		if (DEBUG)
			printf("function_void_value\n");
	}

	free(f->params);
	if (DEBUG)
		printf("function_def_params\n");
	free(f->c_stmt);
	if (DEBUG)
		printf("function_def_c_stmt\n");
}

void mCc_ast_delete_function_def_array(ast_function_def_array *f)
{
	assert(f);

	struct mCc_ast_visitor visitor = ast_delete_visitor(f);

	mCc_ast_visit_function_def_array(f, &visitor);

	free(f->function_def);
	if (DEBUG)
		printf("func_def_array\n");
	free(f);
}


/* ----------------------------------------------------------- Declaration */
ast_declaration *
mCc_ast_new_array_declaration(enum mCc_ast_literal_type literal, int numerator,
			      ast_identifier *identifier)
{

	ast_declaration *decl = malloc(sizeof(*decl));
	if (!decl) {
		return NULL;
	}
	decl->type = MCC_AST_DECLARATION_TYPE_ARRAY;
	decl->literal = literal;
	decl->numerator = numerator;
	decl->array_identifier = identifier;
	return decl;
}

ast_declaration *
mCc_ast_new_single_declaration(enum mCc_ast_literal_type literal,
			       ast_identifier *identifier)
{
	ast_declaration *decl = malloc(sizeof(*decl));
	if (!decl) {
		return NULL;
	}
	decl->type = MCC_AST_DECLARATION_TYPE_SINGLE;
	decl->literal = literal;
	decl->identifier = identifier;
	return decl;
}

void mCc_ast_delete_declaration(ast_declaration *decl, void *data)
{
	assert(decl);
	assert(data);
}


/* ----------------------------------------------------------- Statement */
// IF
ast_stmt *mCc_ast_new_if_stmt(ast_if_stmt *if_stmt)
{
	assert(if_stmt);

	ast_stmt *stmt = malloc(sizeof(*stmt));
	if (!stmt) {
		return NULL;
	}
	stmt->type = MCC_AST_IF_STMT;
	stmt->if_stmt = if_stmt;
	return stmt;
}

ast_if_stmt *mCc_ast_new_if(ast_expr *ex, ast_stmt *stmt)
{
	assert(ex);
	assert(stmt);

	ast_if_stmt *if_stmt = malloc(sizeof(*if_stmt));
	if_stmt->expression = ex;
	if_stmt->statement = stmt;
	if_stmt->else_statement = NULL;

	return if_stmt;
}

ast_if_stmt *mCc_ast_new_if_else(ast_expr *ex, ast_stmt *stmt,
				 ast_stmt *elsestmt)
{
	assert(ex);
	assert(stmt);

	ast_if_stmt *if_stmt = malloc(sizeof(*if_stmt));
	if_stmt->expression = ex;
	if_stmt->statement = stmt;
	if_stmt->else_statement = elsestmt;

	return if_stmt;
}

void mCc_ast_delete_if_stmt(ast_if_stmt *if_stmt, void *data)
{
	assert(if_stmt);
	assert(data);

	free(if_stmt->expression);
	if (DEBUG)
		printf("if_stmt_expr\n");
	free(if_stmt->statement);
	if (DEBUG)
		printf("if_stmt_stmt\n");
	if (if_stmt->else_statement != NULL) {
		free(if_stmt->else_statement);
		if (DEBUG)
			printf("if_stmt_else_stmt\n");
	}
}

// WHILE
ast_stmt *mCc_ast_new_while_stmt(ast_while_stmt *while_stmt)
{
	assert(while_stmt);

	ast_stmt *stmt = malloc(sizeof(*stmt));
	if (!stmt) {
		return NULL;
	}
	stmt->type = MCC_AST_WHILE_STMT;
	stmt->while_stmt = while_stmt;
	return stmt;
}

ast_while_stmt *mCc_ast_new_while(ast_expr *ex, ast_stmt *stmt)
{
	assert(ex);
	assert(stmt);

	ast_while_stmt *while_stmt = malloc(sizeof(*while_stmt));
	while_stmt->expression = ex;
	while_stmt->statement = stmt;

	return while_stmt;
}

void mCc_ast_delete_while_stmt(ast_while_stmt *while_stmt, void *data)
{
	assert(while_stmt);
	assert(data);

	free(while_stmt->expression);
	if (DEBUG)
		printf("while_stmt_expr\n");
	free(while_stmt->statement);
	if (DEBUG)
		printf("while_stmt_stmt\n");
}


// RETURN
ast_stmt *mCc_ast_new_ret_stmt(ast_ret_stmt *ret_stmt)
{
	assert(ret_stmt);

	ast_stmt *stmt = malloc(sizeof(*stmt));
	if (!stmt) {
		return NULL;
	}
	stmt->type = MCC_AST_RET_STMT;
	stmt->ret_stmt = ret_stmt;
	return stmt;
}

ast_ret_stmt *mCc_ast_new_empty_ret()
{
	ast_ret_stmt *ret_stmt = malloc(sizeof(*ret_stmt));
	ret_stmt->expression = NULL;

	return ret_stmt;
}

ast_ret_stmt *mCc_ast_new_ret(ast_expr *ex)
{
	assert(ex);

	ast_ret_stmt *ret_stmt = malloc(sizeof(*ret_stmt));
	ret_stmt->expression = ex;

	return ret_stmt;
}

void mCc_ast_delete_ret_stmt(ast_ret_stmt *ret_stmt, void *data)
{
	assert(ret_stmt);
	assert(data);
	if (ret_stmt->expression != NULL) {
		free(ret_stmt->expression);
		if (DEBUG)
			printf("ret_ex\n");
	}
}

// DECLARATION
ast_stmt *mCc_ast_new_declaration(ast_declaration *decl_stmt)
{
	assert(decl_stmt);

	ast_stmt *stmt = malloc(sizeof(*stmt));
	if (!stmt) {
		return NULL;
	}
	stmt->type = MCC_AST_DECL_STMT;
	stmt->declaration = decl_stmt;
	return stmt;
}

// ASSIGNMENT
ast_stmt *mCc_ast_new_assignment(ast_assignment *ass_stmt)
{
	assert(ass_stmt);

	ast_stmt *stmt = malloc(sizeof(*stmt));
	if (!stmt) {
		return NULL;
	}
	stmt->type = MCC_AST_ASS_STMT;
	stmt->assignment = ass_stmt;
	return stmt;
}

// EXPRESSION
ast_stmt *mCc_ast_new_expression(ast_expr *expr_stmt)
{
	assert(expr_stmt);

	ast_stmt *stmt = malloc(sizeof(*stmt));
	if (!stmt) {
		return NULL;
	}
	stmt->type = MCC_AST_EXPR_STMT;
	stmt->expression = expr_stmt;
	return stmt;
}

// COMPOUND
ast_stmt *mCc_ast_new_compound_stmt(ast_compound_stmt *compound_stmt)
{
	assert(compound_stmt);

	ast_stmt *stmt = malloc(sizeof(*stmt));

	stmt->type = MCC_AST_COMPOUND_STMT;
	stmt->compound_stmt = compound_stmt;
	return stmt;
}

ast_compound_stmt *mCc_ast_new_empty_compound()
{

	ast_compound_stmt *new_stmts = malloc(sizeof(*new_stmts));
	new_stmts->statements = NULL;
	new_stmts->counter = 0;
	return new_stmts;
}

ast_compound_stmt *mCc_ast_new_single_compound(ast_stmt *stmt)
{
	assert(stmt);

	ast_compound_stmt *new_stmts = malloc(sizeof(*new_stmts));
	new_stmts->statements = stmt;
	new_stmts->counter = 1;
	return new_stmts;
}

ast_compound_stmt *mCc_ast_new_compound_array(ast_compound_stmt *stmts,
					      ast_stmt *stmt)
{
	assert(stmts);
	assert(stmt);

	ast_stmt *temp = realloc(stmts->statements,
				 sizeof(*stmt) * (stmts->counter + 1));
	if (temp == NULL) {
		// TODO throw error
		return NULL;
	}
	stmts->statements = temp;
	memcpy(&(stmts->statements[stmts->counter]), stmt, sizeof(*stmt));
    free(stmt);
	stmts->counter++;

	return stmts;
}

void mCc_ast_delete_compound_stmt(ast_compound_stmt *compound_stmt, void *data)
{
	assert(compound_stmt);
	assert(data);


    if (DEBUG)
        printf("compound\n");
    free(compound_stmt->statements);
}

// DELETE
void mCc_ast_delete_stmt(ast_stmt *stmt, void *data)
{
	assert(stmt);
	assert(data);

    if (DEBUG)
	    printf("\ndelete_stmt\n");

	switch (stmt->type) {
	case MCC_AST_COMPOUND_STMT:
		free(stmt->compound_stmt);
		if (DEBUG)
			printf("stmt_>compound\n");
		break;
	case MCC_AST_IF_STMT:
		free(stmt->if_stmt);
		if (DEBUG)
			printf("stmt_>if\n");
		break;
	case MCC_AST_WHILE_STMT:
		free(stmt->while_stmt);
		if (DEBUG)
			printf("stmt_>while\n");
		break;
	case MCC_AST_RET_STMT:
		free(stmt->ret_stmt);
		if (DEBUG)
			printf("stmt_>ret\n");
		break;
	case MCC_AST_DECL_STMT:
		free(stmt->declaration);
		if (DEBUG)
			printf("stmt_>decl\n");
		break;
	case MCC_AST_ASS_STMT:
		free(stmt->assignment);
		if (DEBUG)
			printf("stmt_>assi\n");
		break;
	case MCC_AST_EXPR_STMT:
		free(stmt->expression);
		if (DEBUG)
			printf("stmt_>expr\n");
		break;
	}
}

/* ----------------------------------------------------------- Assignment */
ast_assignment *mCc_ast_new_single_assignment(ast_identifier *identifier,
					      ast_expr *ex)
{
	assert(ex);

	ast_assignment *ass = malloc(sizeof(*ass));
	ass->identifier = identifier;
	ass->expression = ex;
	ass->numerator = NULL;
	return ass;
}

ast_assignment *mCc_ast_new_array_assignment(ast_identifier *identifier,
					     ast_expr *ex, ast_expr *ex2)
{
	assert(ex);
	assert(ex2);

	ast_assignment *ass = malloc(sizeof(*ass));
	ass->identifier = identifier;
	ass->expression = ex2;
	ass->numerator = ex;
	return ass;
}

void mCc_ast_delete_assignment(ast_assignment *assignment, void *data)
{
	assert(assignment);
	assert(data);

	free(assignment->expression);
	if (DEBUG)
		printf("assi->expr\n");
	if (assignment->numerator != NULL) {
		free(assignment->numerator);
		if (DEBUG)
			printf("assi->numerator\n");
	}
}

/* ------------------------------------------------------------- Parameter */
ast_parameter *mCc_ast_new_parameter_array(ast_parameter *params,
					   ast_declaration *decl)
{
	assert(params);
	assert(decl);

	ast_declaration *temp = realloc(params->declaration,
					sizeof(*decl) * (params->counter + 1));
	if (temp == NULL) {
		// TODO throw error
		return NULL;
	}
	params->declaration = temp;
	memcpy(&(params->declaration[params->counter]), decl, sizeof(*decl));
    free(decl);
	params->counter++;

	return params;
}

ast_parameter *mCc_ast_new_empty_parameter_array()
{

	ast_parameter *new_params = malloc(sizeof(*new_params));
	new_params->declaration = NULL;
	new_params->counter = 0;
	return new_params;
}

ast_parameter *mCc_ast_new_single_parameter(ast_declaration *decl)
{
	assert(decl);

	ast_parameter *new_params = malloc(sizeof(*new_params));
	new_params->declaration = decl;
	new_params->counter = 1;
	return new_params;
}

void mCc_ast_delete_parameter(ast_parameter *params, void *data)
{
	assert(params);
	assert(data);

	free(params->declaration);
}

/* ------------------------------------------------------------- Argument */
ast_argument *mCc_ast_new_single_argument(ast_expr *ex)
{
	assert(ex);

	ast_argument *argument = malloc(sizeof(*argument));
	argument->expression = ex;
	argument->counter = 1;
	return argument;
}

ast_argument *mCc_ast_new_argument_array(ast_argument *arguments, ast_expr *ex)
{
	assert(arguments);
	assert(ex);

	ast_expr *temp = realloc(arguments->expression,
				 sizeof(*ex) * (arguments->counter + 1));
	if (temp == NULL) {
		// TODO throw error
		return NULL;
	}
	arguments->expression = temp;

	memcpy(&(arguments->expression[arguments->counter]), ex, sizeof(*ex));

	arguments->counter++;
    free(ex);
	return arguments;
}

void mCc_ast_delete_argument(ast_argument *argument, void *data)
{
	assert(argument);
	assert(data);
	if (argument->expression != NULL) {
		free(argument->expression);
		if (DEBUG)
			printf("argu_exif (DEBUG)\n"
			       "    \t");
	}
}