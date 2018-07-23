
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

#include "mCc/ast.h"
#include <mCc/utils.h>

#define MALLOC(ptr,size) 				\
ptr = malloc(size);                		\
if((ptr) == NULL)        				\
{                                   	\
	printf("Malloc failed for %p of"	\
		   "size %li",ptr,(size));  	\
	return NULL;                    	\
}

#define REALLOC(ptr,size) 				\
void * temp = realloc(ptr,size);   		\
if(temp == NULL)     					\
{                                       \
	printf("Realloc failed for %p of"   \
		   "size %li",ptr,(size));      \
	return NULL;                        \
}                                       \
(ptr) = temp;

#define ADD_POINTER(ptr, size, p_type, counter, elem)   	\
    if ((ptr) == NULL) {                                	\
        MALLOC(ptr,size)                                	\
    }                                                   	\
    else {                                             		\
        REALLOC(ptr,size)                               	\
}                                                       	\
    switch (p_type) {                                   	\
        case POINTER_IDENTIFIER:                        	\
            (ptr)[counter].i_pointer = (ast_identifier *) (elem);            	\
            break;                                      	\
        case POINTER_LITERAL:                           	\
            (ptr)[counter].l_pointer = (ast_literal *) (elem);            	\
            break;                                      	\
        case POINTER_SLITERAL:                          	\
            (ptr)[counter].sl_pointer = (ast_literal *) (elem);           	\
            break;                                      	\
        case POINTER_EXPRESSION:                        	\
            (ptr)[counter].expr_pointer = (ast_expr *) (elem);         	\
            break;                                      	\
        case POINTER_SINGLE_EXPRESSION:                   	\
            (ptr)[counter].single_expr_pointer = (ast_single_expr *) (elem);    	\
            break;                                      	\
        case POINTER_CALL_EXPRESSION:                    	\
            (ptr)[counter].call_expr_pointer = (ast_call_expr *) (elem);      	\
            break;                                      	\
        case POINTER_FUNC_DEF:                        		\
            (ptr)[counter].func_def_pointer = (ast_function_def *) (elem);         \
            break;                                      	\
        case POINTER_FUNC_DEF_ARRAY:                        \
            (ptr)[counter].func_def_array_pointer = (ast_function_def_array *) (elem);	\
            break;                                      	\
        case POINTER_DECLARATION:                        	\
            (ptr)[counter].declaration_pointer = (ast_declaration *) (elem);   	\
            break;                                      	\
        case POINTER_STMT:                       	 		\
            (ptr)[counter].stmt_pointer = (ast_stmt *) (elem);         	\
            break;                                      	\
        case POINTER_IF_STMT:                        		\
            (ptr)[counter].if_stmt_pointer = (ast_if_stmt *) (elem);         	\
            break;                                      	\
        case POINTER_WHILE_STMT:                        	\
            (ptr)[counter].while_stmt_pointer = (ast_while_stmt *) (elem);      	\
            break;                                      	\
        case POINTER_RET_STMT:                        		\
            (ptr)[counter].ret_stmt_pointer = (ast_ret_stmt *) (elem);         \
            break;                                      	\
        case POINTER_COMPOUND_STMT:                        	\
            (ptr)[counter].compound_stmt_pointer = (ast_compound_stmt *) (elem); 	\
            break;                                      	\
        case POINTER_ASSIGNMENT:                        	\
            (ptr)[counter].assignment_pointer = (ast_assignment *) (elem);    	\
            break;                                      	\
        case POINTER_PARAMETER:                        		\
            (ptr)[counter].parameter_pointer = (ast_parameter *) (elem);     	\
            break;                                      	\
        case POINTER_ARGUMENT:                        		\
            (ptr)[counter].argument_pointer = (ast_argument *) (elem);         \
            break;                                      	\
        default:                                        	\
            break;                                      	\
    }                                                   	\
    (ptr)[(counter)++].type = p_type;


static struct pointer_stack *p_stack = NULL;
static int stack_counter = 0;


/* ---------------------------------------------------------------- Identifier */

void mCc_ast_delete_identifier(ast_identifier *identifier)
{
	assert(identifier);

    free(identifier->name);
    free(identifier->renamed);


	if (identifier->param_types != NULL)
		free(identifier->param_types);

	free(identifier);
}

ast_identifier *mCc_ast_new_identifier(char *name, int line)
{
	assert(name);

	ast_identifier *identifier;
    MALLOC(identifier, sizeof(ast_identifier))

	identifier->name = name;

	identifier->renamed = copy_string(identifier->name);

	identifier->node.sloc.start_line = line;
	identifier->d_type = MCC_AST_TYPE_VOID;
	identifier->param_types = NULL;
	identifier->size = 0;

    ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_IDENTIFIER, stack_counter, identifier);

    return identifier;
}

/* ---------------------------------------------------------------- Literals */
ast_literal *mCc_ast_new_literal_int(long value)
{
	ast_literal *lit;
    MALLOC(lit,sizeof(ast_literal))

	lit->type = MCC_AST_LITERAL_TYPE_INT;
	lit->i_value = value;

    ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_LITERAL, stack_counter, lit);

	return lit;
}

ast_literal *mCc_ast_new_literal_float(double value)
{
	ast_literal *lit;
    MALLOC(lit,sizeof(ast_literal))

	lit->type = MCC_AST_LITERAL_TYPE_FLOAT;
	lit->f_value = value;

    ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_LITERAL, stack_counter, lit);

	return lit;
}

ast_literal *mCc_ast_new_literal_bool(char * value)
{
	ast_literal *lit;
    MALLOC(lit,sizeof(ast_literal))

    lit->type = MCC_AST_LITERAL_TYPE_BOOL;
	lit->b_value = strcmp(value,"true")==0?true:false;

    ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_LITERAL, stack_counter, lit);

	return lit;
}

ast_literal *mCc_ast_new_literal_string(char *value)
{
	char *t = replace(value, "\\n", "#enter#");
	char *temp = replace(t, "\"", "");
	free(t);
	ast_literal *lit;
    MALLOC(lit,sizeof(ast_literal))

	lit->type = MCC_AST_LITERAL_TYPE_STRING;
	lit->s_value = copy_string(temp);
    free(temp);

    ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_SLITERAL, stack_counter, lit);

	return lit;
}

void mCc_ast_delete_literal_value(ast_literal *literal)
{
	assert(literal);

	free(literal->s_value);
}

void mCc_ast_delete_literal(ast_literal *literal)
{
	assert(literal);

	free(literal);
}


/* ------------------------------------------------------------- Expressions */
ast_expr *mCc_ast_new_expression_single(ast_single_expr *single_expr)
{
	assert(single_expr);

	ast_expr *expr;
    MALLOC(expr,sizeof(ast_expr))

	expr->type = MCC_AST_EXPRESSION_TYPE_SINGLE;
	expr->single_expr = single_expr;
	expr->d_type = MCC_AST_TYPE_VOID;

    ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_EXPRESSION, stack_counter, expr);

	return expr;
}

ast_expr *mCc_ast_new_expression_binary_op(enum mCc_ast_binary_op op,
					   ast_single_expr *lhs, ast_expr *rhs)
{
	assert(lhs);
	assert(rhs);

    ast_expr *expr;
    MALLOC(expr,sizeof(ast_expr))

	expr->type = MCC_AST_EXPRESSION_TYPE_BINARY;
	expr->op = op;
	expr->lhs = lhs;
	expr->rhs = rhs;
	expr->d_type = MCC_AST_TYPE_VOID;

    ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_EXPRESSION, stack_counter, expr);

	return expr;
}

/* Single Expression */
ast_single_expr *mCc_ast_new_single_expression_literal(ast_literal *literal)
{
	assert(literal);

    ast_single_expr *expr;
    MALLOC(expr,sizeof(ast_single_expr))

	expr->type = MCC_AST_SINGLE_EXPRESSION_TYPE_LITERAL;
	expr->literal = literal;
	expr->d_type = MCC_AST_TYPE_VOID;

    ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_SINGLE_EXPRESSION, stack_counter, expr);

	return expr;
}

ast_single_expr *
mCc_ast_new_single_expression_identifier(ast_identifier *identifier)
{
	assert(identifier);

    ast_single_expr *expr;
    MALLOC(expr,sizeof(ast_single_expr))

	expr->type = MCC_AST_SINGLE_EXPRESSION_TYPE_IDENTIFIER;
	expr->only_identifier = identifier;

    ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_SINGLE_EXPRESSION, stack_counter, expr);

    return expr;
}

ast_single_expr *
mCc_ast_new_single_expression_identifier_ex(ast_identifier *identifier,
					    ast_expr *identifier_expression)
{
	assert(identifier);
	assert(identifier_expression);

    ast_single_expr *expr;
    MALLOC(expr,sizeof(ast_single_expr))

	expr->type = MCC_AST_SINGLE_EXPRESSION_TYPE_IDENTIFIER_EX;
	expr->identifier = identifier;
	expr->identifier_expression = identifier_expression;

    ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_SINGLE_EXPRESSION, stack_counter, expr);

    return expr;
}

ast_single_expr *
mCc_ast_new_single_expression_call_expr(ast_call_expr *call_expr)
{
	assert(call_expr);

    ast_single_expr *expr;
    MALLOC(expr,sizeof(ast_single_expr))

	expr->type = MCC_AST_SINGLE_EXPRESSION_TYPE_CALL_EXPR;
	expr->call_expr = call_expr;

    ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_SINGLE_EXPRESSION, stack_counter, expr);

    return expr;
}

ast_single_expr *
mCc_ast_new_single_expression_unary_op(enum mCc_ast_unary_op unary_op,
				       ast_expr *unary_expression)
{
	assert(unary_expression);

    ast_single_expr *expr;
    MALLOC(expr,sizeof(ast_single_expr))

	expr->type = MCC_AST_SINGLE_EXPRESSION_TYPE_UNARY_OP;
	expr->unary_operator = unary_op;
	expr->unary_expression = unary_expression;

    ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_SINGLE_EXPRESSION, stack_counter, expr);

    return expr;
}

ast_single_expr *mCc_ast_new_single_expression_parenth(ast_expr *expression)
{
	assert(expression);

    ast_single_expr *expr;
    MALLOC(expr,sizeof(ast_single_expr))

	expr->type = MCC_AST_SINGLE_EXPRESSION_TYPE_PARENTH;
	expr->expression = expression;

    ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_SINGLE_EXPRESSION, stack_counter, expr);

    return expr;
}

/* Call Expression */
ast_call_expr *mCc_ast_new_empty_call_expr(ast_identifier *identifier)
{
    assert(identifier);
    ast_call_expr *call_expr;
    MALLOC(call_expr,sizeof(ast_call_expr))

    call_expr->identifier = identifier;
	call_expr->arguments = NULL;

    ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_CALL_EXPRESSION, stack_counter, call_expr);

    return call_expr;
}

ast_call_expr *mCc_ast_new_call_expr(ast_identifier *identifier,
				     ast_argument *arguments)
{
	assert(arguments);
    ast_call_expr *call_expr;
    MALLOC(call_expr,sizeof(ast_call_expr))

    call_expr->identifier = identifier;
	call_expr->arguments = arguments;
	call_expr->d_type = MCC_AST_TYPE_VOID;

    ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_CALL_EXPRESSION, stack_counter, call_expr);

    return call_expr;
}


/* ----------------------------------------------------------- Function Def */
ast_function_def *mCc_ast_new_void_function_def(ast_identifier *identifier,
						ast_parameter *params,
						ast_compound_stmt *c_stmt)
{
    assert(identifier);
    assert(params);
    assert(c_stmt);
	ast_function_def *f;
    MALLOC(f, sizeof(ast_function_def))

	f->type = MCC_AST_FUNCTION_DEF_TYPE_VOID;

	f->void_value = "void";
	f->identifier = identifier;
	f->identifier->d_type = MCC_AST_TYPE_VOID;
	f->params = params;
	f->c_stmt = c_stmt;

    ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_FUNC_DEF, stack_counter, f);

    return f;
}

ast_function_def *mCc_ast_new_type_function_def(enum mCc_ast_literal_type type,
						ast_identifier *identifier,
						ast_parameter *params,
						ast_compound_stmt *c_stmt)
{

    assert(identifier);
    assert(params);
    assert(c_stmt);
    ast_function_def *f;
    MALLOC(f, sizeof(ast_function_def))

	f->type = MCC_AST_FUNCTION_DEF_TYPE_TYPE;

	f->l_type = type;
	f->identifier = identifier;
	switch (f->l_type) {
	case (MCC_AST_LITERAL_TYPE_INT):
		f->identifier->d_type = MCC_AST_TYPE_INT;
		break;
	case (MCC_AST_LITERAL_TYPE_FLOAT):
		f->identifier->d_type = MCC_AST_TYPE_FLOAT;
		break;
	case (MCC_AST_LITERAL_TYPE_STRING):
		f->identifier->d_type = MCC_AST_TYPE_STRING;
		break;
	case (MCC_AST_LITERAL_TYPE_BOOL):
		f->identifier->d_type = MCC_AST_TYPE_BOOL;
		break;
	}
	f->params = params;
	f->c_stmt = c_stmt;

    ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_FUNC_DEF, stack_counter, f);

    return f;
}

ast_function_def_array *
mCc_ast_add_function_def_to_array(ast_function_def_array *f,
				  ast_function_def *f2)
{
	assert(f);
	assert(f2);


    for (int i = 0; i < stack_counter; i++) {
        if (p_stack[i].type == POINTER_FUNC_DEF &&
                (p_stack[i].func_def_pointer == f2 ||
                        p_stack[i].func_def_pointer == f->function_def)) {
            p_stack[i].type = POINTER_NULL;
            p_stack[i].func_def_pointer = NULL;
        }
    }

    REALLOC(f->function_def, sizeof(ast_function_def) * (f->counter + 1))

    memcpy(&(f->function_def[f->counter]), f2, sizeof(ast_function_def));
	free(f2);
	f->counter++;

    ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_FUNC_DEF, stack_counter, f->function_def);

    return f;
}

ast_function_def_array *mCc_ast_new_function_def_array(ast_function_def *f)
{
	assert(f);

	ast_function_def_array *function_array;
    MALLOC(function_array, sizeof(ast_function_def_array))

	function_array->counter = 1;
	function_array->function_def = f;

    ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_FUNC_DEF_ARRAY, stack_counter, function_array);

	return function_array;
}

ast_function_def_array *mCc_ast_gen_func_def(ast_expr * expr)
{
    assert(expr);

    ast_function_def *f;
    MALLOC(f, sizeof(ast_function_def))

    f->type = MCC_AST_FUNCTION_DEF_TYPE_VOID;

    f->void_value = "void";
    f->identifier = mCc_ast_new_identifier(new_string("%s","temp"),0);
    f->identifier->d_type = MCC_AST_TYPE_VOID;
    f->params = mCc_ast_new_empty_parameter_array();
    f->c_stmt = mCc_ast_new_single_compound(mCc_ast_new_expression(expr));

    ast_function_def_array *function_array;
    MALLOC(function_array, sizeof(ast_function_def_array))

    function_array->counter = 1;
    function_array->function_def = f;

    ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_FUNC_DEF, stack_counter, f);
    ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_FUNC_DEF_ARRAY, stack_counter, function_array);

    return function_array;
}

void mCc_ast_delete_pointer()
{
    for (int i = 0; i < stack_counter; i++) {
        if (p_stack[i].type == POINTER_IDENTIFIER) {
            mCc_ast_delete_identifier(p_stack[i].i_pointer);
            p_stack[i].i_pointer = NULL;
        } else if (p_stack[i].type == POINTER_LITERAL) {
            mCc_ast_delete_literal(p_stack[i].l_pointer);
            p_stack[i].l_pointer = NULL;
        } else if (p_stack[i].type == POINTER_SLITERAL) {
            mCc_ast_delete_literal_value(p_stack[i].sl_pointer);
            mCc_ast_delete_literal(p_stack[i].sl_pointer);
            p_stack[i].sl_pointer = NULL;
        } else if (p_stack[i].type == POINTER_EXPRESSION) {
                free(p_stack[i].expr_pointer);
            p_stack[i].expr_pointer = NULL;
        } else if (p_stack[i].type == POINTER_SINGLE_EXPRESSION) {
            free(p_stack[i].single_expr_pointer);
            p_stack[i].single_expr_pointer = NULL;
        } else if (p_stack[i].type == POINTER_CALL_EXPRESSION) {
            free(p_stack[i].call_expr_pointer);
            p_stack[i].call_expr_pointer = NULL;
        } else if (p_stack[i].type == POINTER_FUNC_DEF) {
                free(p_stack[i].func_def_pointer);
            p_stack[i].func_def_pointer = NULL;
        } else if (p_stack[i].type == POINTER_FUNC_DEF_ARRAY) {
            free(p_stack[i].func_def_array_pointer);
            p_stack[i].func_def_array_pointer = NULL;
        } else if (p_stack[i].type == POINTER_DECLARATION) {
                free(p_stack[i].declaration_pointer);
            p_stack[i].declaration_pointer = NULL;
        } else if (p_stack[i].type == POINTER_STMT) {
                free(p_stack[i].stmt_pointer);
            p_stack[i].stmt_pointer = NULL;
        } else if (p_stack[i].type == POINTER_IF_STMT) {
            free(p_stack[i].if_stmt_pointer);
            p_stack[i].if_stmt_pointer = NULL;
        } else if (p_stack[i].type == POINTER_WHILE_STMT) {
            free(p_stack[i].while_stmt_pointer);
            p_stack[i].while_stmt_pointer = NULL;
        } else if (p_stack[i].type == POINTER_RET_STMT) {
            free(p_stack[i].ret_stmt_pointer);
            p_stack[i].ret_stmt_pointer = NULL;
        } else if (p_stack[i].type == POINTER_COMPOUND_STMT) {
            free(p_stack[i].compound_stmt_pointer);
            p_stack[i].compound_stmt_pointer = NULL;
        } else if (p_stack[i].type == POINTER_ASSIGNMENT) {
            free(p_stack[i].assignment_pointer);
            p_stack[i].assignment_pointer = NULL;
        } else if (p_stack[i].type == POINTER_PARAMETER) {
            free(p_stack[i].parameter_pointer);
            p_stack[i].parameter_pointer = NULL;
        } else if (p_stack[i].type == POINTER_ARGUMENT) {
            free(p_stack[i].argument_pointer);
            p_stack[i].argument_pointer = NULL;
        }
        p_stack[i].type = POINTER_NULL;
    }

    stack_counter = 0;
    if (p_stack != NULL)
        free(p_stack);
    p_stack = NULL;
}


/* ----------------------------------------------------------- Declaration */
ast_declaration *
mCc_ast_new_array_declaration(enum mCc_ast_literal_type literal, int numerator,
			      ast_identifier *identifier)
{
    assert(identifier);
	ast_declaration *decl;
    MALLOC(decl, sizeof(ast_declaration))

	decl->type = MCC_AST_DECLARATION_TYPE_ARRAY;
	decl->literal = literal;
	decl->numerator = numerator;
	decl->array_identifier = identifier;

    ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_DECLARATION, stack_counter, decl);

    return decl;
}

ast_declaration *
mCc_ast_new_single_declaration(enum mCc_ast_literal_type literal,
			       ast_identifier *identifier)
{
    assert(identifier);
    ast_declaration *decl;
    MALLOC(decl, sizeof(ast_declaration))

	decl->type = MCC_AST_DECLARATION_TYPE_SINGLE;
	decl->literal = literal;
	decl->identifier = identifier;

    ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_DECLARATION, stack_counter, decl);

    return decl;
}


/* ----------------------------------------------------------- Statement */
// IF
ast_stmt *mCc_ast_new_if_stmt(ast_if_stmt *if_stmt)
{
	assert(if_stmt);

	ast_stmt *stmt;
    MALLOC(stmt, sizeof(ast_stmt))

	stmt->type = MCC_AST_IF_STMT;
	stmt->if_stmt = if_stmt;

    ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_STMT, stack_counter, stmt);

    return stmt;
}

ast_if_stmt *mCc_ast_new_if(ast_expr *ex, ast_stmt *stmt)
{
	assert(ex);
	assert(stmt);

	ast_if_stmt *if_stmt;
    MALLOC(if_stmt,sizeof(ast_if_stmt))

	if_stmt->expression = ex;
	if_stmt->statement = stmt;
	if_stmt->else_statement = NULL;

    ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_IF_STMT, stack_counter, if_stmt);

	return if_stmt;
}

ast_if_stmt *mCc_ast_new_if_else(ast_expr *ex, ast_stmt *stmt,
				 ast_stmt *elsestmt)
{
	assert(ex);
	assert(stmt);

    ast_if_stmt *if_stmt;
    MALLOC(if_stmt,sizeof(ast_if_stmt))

	if_stmt->expression = ex;
	if_stmt->statement = stmt;
	if_stmt->else_statement = elsestmt;

    ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_IF_STMT, stack_counter, if_stmt);

	return if_stmt;
}

// WHILE
ast_stmt *mCc_ast_new_while_stmt(ast_while_stmt *while_stmt)
{
	assert(while_stmt);

    ast_stmt *stmt;
    MALLOC(stmt, sizeof(ast_stmt))

	stmt->type = MCC_AST_WHILE_STMT;
	stmt->while_stmt = while_stmt;

    ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_STMT, stack_counter, stmt);

    return stmt;
}

ast_while_stmt *mCc_ast_new_while(ast_expr *ex, ast_stmt *stmt)
{
	assert(ex);
	assert(stmt);

    ast_while_stmt *while_stmt;
    MALLOC(while_stmt,sizeof(ast_while_stmt))

    while_stmt->expression = ex;
	while_stmt->statement = stmt;

    ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_WHILE_STMT, stack_counter, while_stmt);

	return while_stmt;
}


// RETURN
ast_stmt *mCc_ast_new_ret_stmt(ast_ret_stmt *ret_stmt)
{
	assert(ret_stmt);

    ast_stmt *stmt;
    MALLOC(stmt, sizeof(ast_stmt))

	stmt->type = MCC_AST_RET_STMT;
	ret_stmt->d_type = MCC_AST_TYPE_VOID;
	stmt->ret_stmt = ret_stmt;

    ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_STMT, stack_counter, stmt);

    return stmt;
}

ast_ret_stmt *mCc_ast_new_empty_ret()
{
	ast_ret_stmt *ret_stmt;
    MALLOC(ret_stmt,sizeof(ast_ret_stmt))

    ret_stmt->expression = NULL;

    ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_RET_STMT, stack_counter, ret_stmt);

	return ret_stmt;
}

ast_ret_stmt *mCc_ast_new_ret(ast_expr *ex)
{
	assert(ex);

    ast_ret_stmt *ret_stmt;
    MALLOC(ret_stmt,sizeof(ast_ret_stmt))

    ret_stmt->expression = ex;


	ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_RET_STMT, stack_counter, ret_stmt);

	return ret_stmt;
}

// DECLARATION
ast_stmt *mCc_ast_new_declaration(ast_declaration *decl_stmt)
{
	assert(decl_stmt);

    ast_stmt *stmt;
    MALLOC(stmt, sizeof(ast_stmt))

	stmt->type = MCC_AST_DECL_STMT;
	stmt->declaration = decl_stmt;

	ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_STMT, stack_counter, stmt);

	return stmt;
}

// ASSIGNMENT
ast_stmt *mCc_ast_new_assignment(ast_assignment *ass_stmt)
{
	assert(ass_stmt);

    ast_stmt *stmt;
    MALLOC(stmt, sizeof(ast_stmt))

	stmt->type = MCC_AST_ASS_STMT;
	stmt->assignment = ass_stmt;

	ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_STMT, stack_counter, stmt);

	return stmt;
}

// EXPRESSION
ast_stmt *mCc_ast_new_expression(ast_expr *expr_stmt)
{
	assert(expr_stmt);

    ast_stmt *stmt;
    MALLOC(stmt, sizeof(ast_stmt))

	stmt->type = MCC_AST_EXPR_STMT;
	stmt->expression = expr_stmt;

	ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_STMT, stack_counter, stmt);

	return stmt;
}

// COMPOUND
ast_stmt *mCc_ast_new_compound_stmt(ast_compound_stmt *compound_stmt)
{
	assert(compound_stmt);

    ast_stmt *stmt;
    MALLOC(stmt, sizeof(ast_stmt))

	stmt->type = MCC_AST_COMPOUND_STMT;
	stmt->compound_stmt = compound_stmt;

	ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_STMT, stack_counter, stmt);

	return stmt;
}

ast_compound_stmt *mCc_ast_new_empty_compound()
{
	ast_compound_stmt *new_stmts;
    MALLOC(new_stmts,sizeof(ast_compound_stmt))

	new_stmts->statements = NULL;
	new_stmts->counter = 0;

	ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_COMPOUND_STMT, stack_counter, new_stmts);

	return new_stmts;
}

ast_compound_stmt *mCc_ast_new_single_compound(ast_stmt *stmt)
{
	assert(stmt);

    ast_compound_stmt *new_stmts;
    MALLOC(new_stmts,sizeof(ast_compound_stmt))

    new_stmts->statements = stmt;
	new_stmts->counter = 1;

	ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_COMPOUND_STMT, stack_counter, new_stmts);

	return new_stmts;
}

ast_compound_stmt *mCc_ast_new_compound_array(ast_compound_stmt *stmts,
					      ast_stmt *stmt)
{
	assert(stmts);
	assert(stmt);


    for (int i = 0; i < stack_counter; i++) {
        if (p_stack[i].type == POINTER_STMT &&
                (p_stack[i].stmt_pointer == stmt ||
                        p_stack[i].stmt_pointer == stmts->statements))
        {
                p_stack[i].type = POINTER_NULL;
                p_stack[i].stmt_pointer = NULL;
        }
    }

    REALLOC(stmts->statements, sizeof(ast_stmt) * (stmts->counter + 1))

    memcpy(&(stmts->statements[stmts->counter]), stmt, sizeof(ast_stmt));
	free(stmt);
	stmts->counter++;
    ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_STMT, stack_counter, stmts->statements);
	return stmts;
}

/* ----------------------------------------------------------- Assignment */
ast_assignment *mCc_ast_new_single_assignment(ast_identifier *identifier,
					      ast_expr *ex)
{
	assert(ex);
    assert(identifier);

	ast_assignment *ass;
    MALLOC(ass,sizeof(ast_assignment))

	ass->identifier = identifier;
	ass->expression = ex;
	ass->numerator = NULL;

	ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_ASSIGNMENT, stack_counter, ass);

	return ass;
}

ast_assignment *mCc_ast_new_array_assignment(ast_identifier *identifier,
					     ast_expr *ex, ast_expr *ex2)
{
	assert(ex);
	assert(ex2);

    ast_assignment *ass;
    MALLOC(ass,sizeof(ast_assignment))

    ass->identifier = identifier;
	ass->expression = ex2;
	ass->numerator = ex;

	ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_ASSIGNMENT, stack_counter, ass);

	return ass;
}

/* ------------------------------------------------------------- Parameter */
ast_parameter *mCc_ast_new_parameter_array(ast_parameter *params,
					   ast_declaration *decl)
{
	assert(params);
	assert(decl);



    for (int i = 0; i < stack_counter; i++) {
        if (p_stack[i].type == POINTER_DECLARATION &&
                (p_stack[i].declaration_pointer == decl ||
                        p_stack[i].declaration_pointer == params->declaration)) {
            p_stack[i].type = POINTER_NULL;
            p_stack[i].declaration_pointer = NULL;
        }
    }

    REALLOC(params->declaration, sizeof(ast_declaration) * (params->counter +1));

    memcpy(&(params->declaration[params->counter]), decl, sizeof(ast_declaration));
	free(decl);
	params->counter++;

    ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_DECLARATION, stack_counter, params->declaration);

	return params;
}

ast_parameter *mCc_ast_new_empty_parameter_array()
{
    ast_parameter *new_params;
    MALLOC(new_params,sizeof(ast_parameter))

	new_params->declaration = NULL;
	new_params->counter = 0;

	ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_PARAMETER, stack_counter, new_params);

	return new_params;
}

ast_parameter *mCc_ast_new_single_parameter(ast_declaration *decl)
{
	assert(decl);
    ast_parameter *new_params;
    MALLOC(new_params,sizeof(ast_parameter))

	new_params->declaration = decl;
	new_params->counter = 1;

	ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_PARAMETER, stack_counter, new_params);

	return new_params;
}

/* ------------------------------------------------------------- Argument */
ast_argument *mCc_ast_new_single_argument(ast_expr *ex)
{
	assert(ex);

	ast_argument *argument;
    MALLOC(argument, sizeof(ast_argument))

	argument->expression = ex;
	argument->counter = 1;

	ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_ARGUMENT, stack_counter, argument);

	return argument;
}

ast_argument *mCc_ast_new_argument_array(ast_argument *arguments, ast_expr *ex)
{
	assert(arguments);
	assert(ex);

    for (int i = 0; i < stack_counter; i++) {
        if (p_stack[i].type == POINTER_EXPRESSION &&
                (p_stack[i].expr_pointer == ex ||
                        p_stack[i].expr_pointer == arguments->expression)) {
                p_stack[i].type = POINTER_NULL;
                p_stack[i].expr_pointer = NULL;
            }
    }

    REALLOC(arguments->expression,sizeof(ast_expr) * (arguments->counter + 1))

    memcpy(&(arguments->expression[arguments->counter]), ex, sizeof(ast_expr));
	arguments->counter++;
	free(ex);

    ADD_POINTER(p_stack, sizeof(pointer_stack) * (stack_counter + 1), POINTER_EXPRESSION, stack_counter, arguments->expression);
	return arguments;
}