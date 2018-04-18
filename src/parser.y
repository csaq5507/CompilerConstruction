%define api.prefix {mCc_parser_}

%define api.pure full
%lex-param   {void *scanner}
%parse-param {void *scanner} {struct mCc_parser_result* result}

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

extern int line_counter;
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
%token COMMA ","


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

%token <char*> UNKNWON "unkown"

%token RETURN "return"
%token IF "if"
%token ELSE "else"
%token ASSIGNMENT "="
%token WHILE "while"

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


binary_op       : PLUS              { $$ = MCC_AST_BINARY_OP_ADD; }
                | MINUS             { $$ = MCC_AST_BINARY_OP_SUB; }
                | ASTER             { $$ = MCC_AST_BINARY_OP_MUL; }
                | SLASH             { $$ = MCC_AST_BINARY_OP_DIV; }
                | LT                { $$ = MCC_AST_BINARY_OP_LT; }
                | GT                { $$ = MCC_AST_BINARY_OP_GT; }
                | LE                { $$ = MCC_AST_BINARY_OP_LE; }
                | GE                { $$ = MCC_AST_BINARY_OP_GE; }
                | EQ                { $$ = MCC_AST_BINARY_OP_EQ; }
                | NEQ               { $$ = MCC_AST_BINARY_OP_NEQ; }
                | AND               { $$ = MCC_AST_BINARY_OP_AND; }
                | OR                { $$ = MCC_AST_BINARY_OP_OR; }
                ;

unary_op        : MINUS             { $$ = MCC_AST_UNARY_OP_NEGATION; }
                | FAC               { $$ = MCC_AST_UNARY_OP_FAC; }
                ;

type            : INT               { $$ = MCC_AST_LITERAL_TYPE_INT; }
                | BOOL              { $$ = MCC_AST_LITERAL_TYPE_BOOL; }
                | FLOAT             { $$ = MCC_AST_LITERAL_TYPE_FLOAT; }
                | STRING            { $$ = MCC_AST_LITERAL_TYPE_STRING; }
                ;

literal         : INT_LITERAL       { $$ = mCc_ast_new_literal_int($1);    }
                | FLOAT_LITERAL     { $$ = mCc_ast_new_literal_float($1);  }
                | BOOL_LITERAL      { $$ = mCc_ast_new_literal_bool($1);   }
                | STRING_LITERAL    { $$ = mCc_ast_new_literal_string($1); }
                ;


toplevel        : function_def_arr                  { result->func_def = $1; }
                ;

function_def_arr: function_def_arr function_def     { $$ = mCc_ast_add_function_def_to_array($1,$2); }
                | function_def                      { $$ = mCc_ast_new_function_def_array($1); }
                ;


function_def    : VOID IDENTIFIER LPARENTH parameter
                    RPARENTH LBRACE compound_stmt RBRACE          { $$ = mCc_ast_new_void_function_def($2,$4,$7); }

                | type IDENTIFIER LPARENTH parameter
                    RPARENTH LBRACE compound_stmt RBRACE         { $$ = mCc_ast_new_type_function_def($1,$2,$4,$7); }

                ;



parameter       :  declaration                       { $$ = mCc_ast_new_single_parameter($1); }
                | %empty                             { $$ = mCc_ast_new_empty_parameter_array();  }
                | parameter COMMA declaration          { $$ = mCc_ast_new_parameter_array($1,$3); }
                ;


compound_stmt   : statement                         { $$ = mCc_ast_new_single_compound($1); }
                | compound_stmt statement           { $$ = mCc_ast_new_compound_array($1,$2); }
                | %empty                            { $$ = mCc_ast_new_empty_compound(); }
                ;


statement       : if_stmt                           { $$ = mCc_ast_new_if_stmt($1); }
                | ret_stmt SEMICOLON                { $$ = mCc_ast_new_ret_stmt($1); }
                | while_stmt                        { $$ = mCc_ast_new_while_stmt($1); }
                | assignment SEMICOLON              { $$ = mCc_ast_new_assignment($1); }
                | declaration SEMICOLON             { $$ = mCc_ast_new_declaration($1); }
                | expression SEMICOLON              { $$ = mCc_ast_new_expression($1); }
                | LBRACE compound_stmt RBRACE       { $$ = mCc_ast_new_compound_stmt($2); }
                ;


if_stmt         : IF LPARENTH expression RPARENTH statement
                                                    { $$ = mCc_ast_new_if($3,$5); }
                | IF LPARENTH expression RPARENTH statement ELSE statement
                                                    { $$ = mCc_ast_new_if_else($3,$5,$7); }
                ;


while_stmt      : WHILE LPARENTH expression RPARENTH statement
                                                    { $$ = mCc_ast_new_while($3,$5); }
                ;


ret_stmt        : RETURN                          { $$ = mCc_ast_new_empty_ret(); }
                | RETURN expression              { $$ = mCc_ast_new_ret($2); }
                ;


declaration     : type LBRACKET INT_LITERAL RBRACKET
                    IDENTIFIER                      { $$ = mCc_ast_new_array_declaration($1,$3,$5);}
                | type IDENTIFIER                { $$ = mCc_ast_new_single_declaration($1,$2);}
                ;


assignment      : IDENTIFIER LBRACKET expression RBRACKET "=" expression
                                                    { $$ = mCc_ast_new_array_assignment($1,$3,$6); }
                | IDENTIFIER ASSIGNMENT expression  { $$ = mCc_ast_new_single_assignment($1,$3); }
                ;


expression      : single_expr binary_op expression  { $$ = mCc_ast_new_expression_binary_op($2, $1, $3); }
                | single_expr                       { $$ = mCc_ast_new_expression_single($1); }
                ;


single_expr     : literal                           { $$ = mCc_ast_new_single_expression_literal($1); }
                | IDENTIFIER LBRACKET expression RBRACKET
                                                    { $$ = mCc_ast_new_single_expression_identifier_ex($1,$3); }
                | IDENTIFIER                        { $$ = mCc_ast_new_single_expression_identifier($1); }
                | call_expr                         { $$ = mCc_ast_new_single_expression_call_expr($1); }
                | unary_op expression               { $$ = mCc_ast_new_single_expression_unary_op($1,$2); }
                | LPARENTH expression RPARENTH      { $$ = mCc_ast_new_single_expression_parenth($2); }
                ;


call_expr       : IDENTIFIER LPARENTH RPARENTH      { $$ = mCc_ast_new_empty_call_expr($1); }
                | IDENTIFIER LPARENTH arguments RPARENTH
                                                    { $$ = mCc_ast_new_call_expr($1,$3); }
                ;


arguments       : expression                        { $$ = mCc_ast_new_single_argument($1); }
                | arguments COMMA expression          { $$ = mCc_ast_new_argument_array($1,$3); }
                ;



%%

#include <assert.h>
#include <string.h>

#include "scanner.h"

void yyerror(yyscan_t *scanner, struct mCc_parser_result * result, const char *msg) {
    struct mCc_parser_error *error =  malloc(sizeof(struct mCc_parser_error));
    strcpy(error->error_msg,msg);
    error->error_line = line_counter;
	result->errors = add_parse_error(result->errors, error);
}


struct mCc_parser_error_array* new_parse_error_array() {
    struct mCc_parser_error_array *parser_error_array =
                malloc(sizeof(*parser_error_array));

    parser_error_array->errors = NULL;
    parser_error_array->counter = 0;

    return parser_error_array;
}

struct mCc_parser_error_array* add_parse_error(struct mCc_parser_error_array* array, struct mCc_parser_error *error) {
    assert(array);
    assert(error);
    if(array->counter == 0)
    {
        array->errors = malloc(sizeof(*error));
        array->errors[array->counter] = *error;

    } else
    {
        struct mCc_parser_error * temp = realloc(array->errors, sizeof(*error) * (array->counter + 1));
        if(temp == NULL)
        {
            //TODO throw error
            return NULL;
        }
        array->errors = temp;
        memcpy(&(array->errors[array->counter]),error, sizeof(*error));
    }
    array->counter++;
    return array;
}

struct mCc_parser_result mCc_parser_parse_string(const char *input)
{
	assert(input);


	FILE *in = fmemopen((void *)input, strlen(input), "r");
	if (!in) {
		struct mCc_parser_result result;
		result.status = MCC_PARSER_STATUS_UNABLE_TO_OPEN_STREAM;
		return result;
	}

	struct mCc_parser_result result = mCc_parser_parse_file(in);

	fclose(in);

	return result;
}

struct mCc_parser_result mCc_parser_parse_file(FILE *input)
{
	assert(input);

    yyscan_t scanner;
	mCc_parser_lex_init(&scanner);
	mCc_parser_set_in(input, scanner);

    struct mCc_parser_result result;

	result.status = MCC_PARSER_STATUS_OK;
	result.errors = new_parse_error_array();

	if (yyparse(scanner, &result) != 0) {
		//result.status = MCC_PARSER_STATUS_UNKNOWN_ERROR;
	}

	mCc_parser_lex_destroy(scanner);
	return result;
}
