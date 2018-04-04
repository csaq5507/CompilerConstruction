%define api.prefix {mCc_parser_}

%define api.pure full
%lex-param   {void *scanner}
%parse-param {void *scanner} {struct mCc_ast_function_def_array** result}

%define parse.trace
%define parse.error verbose

%code requires {
#include "mCc/parser.h"
}

%{
#include <string.h>
#include <stdio.h>

int mCc_parser_lex();
void mCc_parser_error();

extern int line_num;
%}


%define api.value.type union
%define api.token.prefix {TK_}

%token END 0 "EOF"

%token <long>       INT_LITERAL    "integer literal"
%token <double>     FLOAT_LITERAL   "float literal"
%token <bool>       BOOL_LITERAL    "boolean literal"
%token <char*>      STRING_LITERAL  "string literal"

%token <char*>      IDENTIFIER      "identifier"


%token LPARENTH "("
%token RPARENTH ")"
%token LBRACKET "["
%token RBRACKET "]"
%token LBRACE "{"
%token RBRACE "}"

%token SEMICOLON ";"
%token KOMMA ","


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

%token VOID "void"
%token BOOL "bool"
%token INT "int"
%token FLOAT "float"
%token STRING "string"

%token ERROR "error"

%token RETURN "return"
%token IF "if"
%token ELSE "else"
%token ASSIGNMENT "="

%type <enum mCc_ast_binary_op> binary_op

%type <enum mCc_ast_unary_op> unary_op

%type <enum mCc_ast_literal_type> type

%type <struct mCc_ast_expression *> expression
%type <struct mCc_ast_literal *> literal
%type <struct mCc_ast_single_expression *> single_expr
%type <struct mCc_ast_function_def *> function_def
%type <struct mCc_ast_function_def_array *> function_def_arr
%type <struct mCc_ast_stmt *> statement

%type <struct mCc_ast_parameter *> parameter
%type <struct mCc_ast_argument *> arguments
%type <struct mCc_ast_call_expr *> call_expr
%type <struct mCc_ast_assignment *> assignment
%type <struct mCc_ast_declaration *> declaration

%type <struct mCc_ast_compound_stmt *> compound_stmt
%type <struct mCc_ast_if_stmt *> if_stmt
%type <struct mCc_ast_while_stmt *> while_stmt
%type <struct mCc_ast_ret_stmt *> ret_stmt


%start toplevel


%%


binary_op       : PLUS              { printf("PLUS\n"); $$ = MCC_AST_BINARY_OP_ADD; }
                | MINUS             { printf("MINUS\n"); $$ = MCC_AST_BINARY_OP_SUB; }
                | ASTER             { printf("ASTER\n"); $$ = MCC_AST_BINARY_OP_MUL; }
                | SLASH             { printf("SLASH\n"); $$ = MCC_AST_BINARY_OP_DIV; }
                | LT                { printf("LT\n"); $$ = MCC_AST_BINARY_OP_LT; }
                | GT                { printf("GT\n"); $$ = MCC_AST_BINARY_OP_GT; }
                | LE                { printf("LE\n"); $$ = MCC_AST_BINARY_OP_LE; }
                | GE                { printf("GE\n"); $$ = MCC_AST_BINARY_OP_GE; }
                | EQ                { printf("EQ\n"); $$ = MCC_AST_BINARY_OP_EQ; }
                | NEQ               { printf("NEQ\n"); $$ = MCC_AST_BINARY_OP_NEQ; }
                | AND               { printf("AND\n"); $$ = MCC_AST_BINARY_OP_AND; }
                | OR                { printf("OR\n"); $$ = MCC_AST_BINARY_OP_OR; }
                ;

unary_op        : MINUS             { printf("MINUS\n"); $$ = MCC_AST_UNARY_OP_NEGATION; }
                | FAC               { printf("FAC\n"); $$ = MCC_AST_UNARY_OP_FAC; }
                ;

type            : INT               { printf("INT\n"); $$ = MCC_AST_LITERAL_TYPE_INT; }
                | BOOL              { printf("BOOL\n"); $$ = MCC_AST_LITERAL_TYPE_BOOL; }
                | FLOAT             { printf("FLOAT\n"); $$ = MCC_AST_LITERAL_TYPE_FLOAT; }
                | STRING            { printf("STRING\n"); $$ = MCC_AST_LITERAL_TYPE_STRING; }
                ;

literal         : INT_LITERAL       { printf("INT_LIT\n"); $$ = mCc_ast_new_literal_int($1);    }
                | FLOAT_LITERAL     { printf("FLOAT_LIT\n"); $$ = mCc_ast_new_literal_float($1);  }
                | BOOL_LITERAL      { printf("BOOL_LIT\n"); $$ = mCc_ast_new_literal_bool($1);   }
                | STRING_LITERAL    { printf("STRING_LIT\n"); $$ = mCc_ast_new_literal_string($1); }
                ;


toplevel        : function_def_arr                  { printf("toplevel"); *result = $1; }
                ;

function_def_arr: function_def_arr function_def     { printf("func_def_arr_1\n"); $$ = mCc_ast_add_function_def_to_array($1,$2); }
                | function_def                      { printf("func_def_arr_2\n"); $$ = mCc_ast_new_function_def_array($1); }
                ;


function_def    : VOID IDENTIFIER LPARENTH parameter
                    RPARENTH LBRACE compound_stmt RBRACE          { printf("func_def_void\n"); $$ = mCc_ast_new_void_function_def($2,$4,$7); }

                | type IDENTIFIER LPARENTH parameter
                    RPARENTH LBRACE compound_stmt RBRACE         { printf("\n\n D \n\n");$$ = mCc_ast_new_type_function_def($1,$2,$4,$7); }
                ;



parameter       :  declaration                       { printf("param_dec\n"); $$ = mCc_ast_new_single_parameter($1); }
                | %empty                             { printf("param_empty\n"); $$ = mCc_ast_new_empty_parameter_array();  }
                | parameter "," declaration          { printf("multi params\n"); $$ = mCc_ast_new_parameter_array($1,$3); }
                ;


compound_stmt   : statement                         { printf("compund_stmt_stmt\n"); $$ = mCc_ast_new_single_compound($1); }
                | compound_stmt statement           { printf("compound_stmt-cmp\n"); $$ = mCc_ast_new_compound_array($1,$2); }
                ;


statement       : if_stmt                           { printf("if_stmt\n"); $$ = mCc_ast_new_if_stmt($1); }
                | ret_stmt SEMICOLON                { printf("ret_stmt\n"); $$ = mCc_ast_new_ret_stmt($1); }
                | while_stmt                        { printf("while_stmt\n"); $$ = mCc_ast_new_while_stmt($1); }
                | declaration SEMICOLON             { printf("decl_SEMI\n"); $$ = mCc_ast_new_declaration($1); }
                | assignment SEMICOLON              { printf("assign_SEMI\n"); $$ = mCc_ast_new_assignment($1); }
                | expression SEMICOLON              { printf("expression_SEMI\n"); $$ = mCc_ast_new_expression($1); }
                | LBRACE compound_stmt RBRACE       { printf("stmt_cmp\n"); $$ = mCc_ast_new_compound_stmt($2); }
                ;


if_stmt         : IF LPARENTH expression RPARENTH statement
                                                    { printf("if_stmt_impl_1\n"); $$ = mCc_ast_new_if($3,$5); }
                | IF LPARENTH expression RPARENTH statement ELSE statement
                                                    { printf("if_stmt_impl_2\n"); $$ = mCc_ast_new_if_else($3,$5,$7); }
                ;


while_stmt      : "while" LPARENTH expression RPARENTH statement
                                                    { printf("while_stmt_impl_1\n"); $$ = mCc_ast_new_while($3,$5); }
                ;


ret_stmt        : RETURN                          { printf("ret_stmt_impl_1\n"); $$ = mCc_ast_new_empty_ret(); }
                | RETURN expression              { printf("ret_stmt_impl_2\n"); $$ = mCc_ast_new_ret($2); }
                ;


declaration     : type LBRACKET INT_LITERAL RBRACKET
                    IDENTIFIER                      { printf("decl_1\n"); $$ = mCc_ast_new_array_declaration($1,$3,$5); }
                | type IDENTIFIER                { printf("decl_2\n"); $$ = mCc_ast_new_single_declaration($1,$2); }
                ;


assignment      : IDENTIFIER ASSIGNMENT expression         { printf("assign_1\n"); $$ = mCc_ast_new_single_assignment($1,$3); }
                | IDENTIFIER LBRACKET expression RBRACKET "=" expression
                                                    { printf("assign_2\n"); $$ = mCc_ast_new_array_assignment($1,$3,$6); }
                ;


expression      : single_expr                       { printf("express_sing\n"); $$ = mCc_ast_new_expression_single($1); }
                | single_expr binary_op expression  { printf("express_arr\n"); $$ = mCc_ast_new_expression_binary_op($2, $1, $3); }
                ;


single_expr     : literal                           { printf("literal\n"); $$ = mCc_ast_new_single_expression_literal($1); }
                | IDENTIFIER                        { printf("ident\n"); $$ = mCc_ast_new_single_expression_identifier($1); }
                | IDENTIFIER expression   		    { printf("ident_expr\n"); $$ = mCc_ast_new_single_expression_identifier_ex($1,$2); }
                | call_expr                         { printf("call_expr\n"); $$ = mCc_ast_new_single_expression_call_expr($1); }
                | unary_op expression               { printf("unary_op\n"); $$ = mCc_ast_new_single_expression_unary_op($1,$2); }
                | LPARENTH expression RPARENTH      { printf("singl_expr\n"); $$ = mCc_ast_new_single_expression_parenth($2); }
                ;


call_expr       : IDENTIFIER LPARENTH RPARENTH      { printf("call_expr_1\n"); $$ = mCc_ast_new_empty_call_expr($1); }
                | IDENTIFIER LPARENTH arguments RPARENTH
                                                    { printf("call_expr_2\n"); $$ = mCc_ast_new_call_expr($1,$3); }
                ;


arguments       : expression                        { printf("arg_1\n"); $$ = mCc_ast_new_single_argument($1); }
                | arguments "," expression          { printf("arg_2\n"); $$ = mCc_ast_new_argument_array($1,$3); }
                ;



%%

#include <assert.h>

#include "scanner.h"


void yyerror(yyscan_t *scanner, const char *msg) {
	printf("Error: %s\n", msg);
}


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

/*    func_def_arrray = 1;
    parameter_arrray = 1;
    compound_arrray = 1;
    argument_arrray = 1;
*/

    printf("B\n\n");
    yyscan_t scanner;
	mCc_parser_lex_init(&scanner);
	mCc_parser_set_in(input, scanner);

	struct mCc_parser_result result = {
		.status = MCC_PARSER_STATUS_OK,
	};
    printf("C\n\n");

	if (yyparse(scanner, &result.func_def) != 0) {
		result.status = MCC_PARSER_STATUS_UNKNOWN_ERROR;
	}
	printf("D\n\n");


	mCc_parser_lex_destroy(scanner);
    printf("E\n\n");

	return result;
}
