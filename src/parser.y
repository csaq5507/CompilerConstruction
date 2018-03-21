%define api.prefix {mCc_parser_}

%define api.pure full
%lex-param   {void *scanner}
%parse-param {void *scanner} {struct mCc_ast_expression** result}

%define parse.trace
%define parse.error verbose

%code requires {
#include "mCc/parser.h"
}

%{
#include <string.h>

int mCc_parser_lex();
void mCc_parser_error();
%}

%define api.value.type union
%define api.token.prefix {TK_}

%token END 0 "EOF"

%token <long>   INT_LITERAL   "integer literal"
%token <double> FLOAT_LITERAL "float literal"
%token <bool> BOOLEAN_LITERAL "boolean literal"
%token <char*> STRING_LITERAL "string literal"

%token LPARENTH "("
%token RPARENTH ")"
%token LBRACKET "["
%token RBRACKET "]"
%token LBRACE "{"
%token RBRACE "}"

%token SEMICOLON ";"


%token PLUS  "+"
%token MINUS "-"
%token ASTER "*"
%token SLASH "/"
%token LT "<"
%token GT ">"
%token LE "<="
%token GE ">="
%token AND "&&"
%token OR "||"
%token EQ "=="
%token NEQ "!="

%token FAC "!"

%type <enum mCc_ast_binary_op> binary_op

%type <enum mCc_ast_unary_op> unary_op

%type <struct mCc_ast_expression *> expression
%type <struct mCc_ast_literal *> literal
%type <struct mCc_ast_single_expression *> single_expr
%type <struct mCc_ast_function_def *> function_def_void function_def_type
%type <struct mCc_ast_stmt *> if_stmt while_stmt ret_stmt decl_stmt ass_stmt expr_stmt compound_stmt

%type <struct mCc_ast_parameter *> parameter
%type <struct mCc_ast_argument *> argument
%type <struct mCc_ast_call_expr *> call_expr
%type <struct mCc_ast_assignment *> assignment
%type <struct mCc_ast_declaration *> declaration

%start toplevel

%%

toplevel : expression { *result = $1; }
         ;

binary_op : PLUS  { $$ = MCC_AST_BINARY_OP_ADD; }
          | MINUS { $$ = MCC_AST_BINARY_OP_SUB; }
          | ASTER { $$ = MCC_AST_BINARY_OP_MUL; }
          | SLASH { $$ = MCC_AST_BINARY_OP_DIV; }
          | LT { $$ = MCC_AST_BINARY_OP_LT; }
          | GT { $$ = MCC_AST_BINARY_OP_GT; }
          | LE { $$ = MCC_AST_BINARY_OP_LE; }
          | GE { $$ = MCC_AST_BINARY_OP_GE; }
          | EQ { $$ = MCC_AST_BINARY_OP_EQ; }
          | NEQ { $$ = MCC_AST_BINARY_OP_NEQ; }
          | AND { $$ = MCC_AST_BINARY_OP_AND; }
          | OR { $$ = MCC_AST_BINARY_OP_OR; }
          ;

unary_op : MINUS { $$ = MCC_AST_UNARY_OP_NEGATION; }
		  | FAC { $$ = MCC_AST_UNARY_OP_FAC; }
          ;

single_expr : literal                         { $$ = mCc_ast_new_single_expression_literal($1); }
            | identifier expression   		  { $$ = mCc_ast_new_single_expression_identifier($1,$2); }
            | call_expr                       { $$ = mCc_ast_new_single_expression_call_expr($1); }
            | unary_op expression             { $$ = mCc_ast_new_single_expression_unary_op($1,$2); }
            | LPARENTH expression RPARENTH    { $$ = mCc_ast_new_single_expression_parenth($2); }
            ;

expression : single_expr                      { $$ = $1;                                           }
           | single_expr binary_op expression { $$ = mCc_ast_new_expression_binary($2, $1, $3); }
           ;

literal : INT_LITERAL   { $$ = mCc_ast_new_literal_int($1);   }
        | FLOAT_LITERAL { $$ = mCc_ast_new_literal_float($1); }
        ;

%%

#include <assert.h>

#include "scanner.h"

void yyerror(yyscan_t *scanner, const char *msg) {}

struct mCc_parser_result mCc_parser_parse_string(const char *input)
{
	assert(input);

	struct mCc_parser_result result = { 0 };

	FILE *in = fmemopen((void *)input, strlen(input), "r");
	if (!in) {
		result.status = MCC_PARSER_STATUS_UNABLE_TO_OPEN_STREAM;
		return result;
	}

	result = mCc_parser_parse_file(in);

	fclose(in);

	return result;
}

struct mCc_parser_result mCc_parser_parse_file(FILE *input)
{
	assert(input);

	yyscan_t scanner;
	mCc_parser_lex_init(&scanner);
	mCc_parser_set_in(input, scanner);

	struct mCc_parser_result result = {
		.status = MCC_PARSER_STATUS_OK,
	};

	if (yyparse(scanner, &result.expression) != 0) {
		result.status = MCC_PARSER_STATUS_UNKNOWN_ERROR;
	}

	mCc_parser_lex_destroy(scanner);

	return result;
}
