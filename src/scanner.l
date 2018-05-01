%option prefix="mCc_parser_"

%option batch
%option bison-bridge
%option noinput
%option nounput
%option noyywrap
%option reentrant

%{
#include "parser.tab.h"

#define YYSTYPE MCC_PARSER_STYPE

int line_counter = 2;
%}

%x comment

%x LEXING_ERROR


int_literal   	[0-9]+
float_literal 	[0-9]+\.[0-9]+
string_literal	L?\"(\\.|[^\\"])*\"

identifier		[a-zA-Z_][a-zA-Z0-9_]*
comment_between	-?[^*\n\r]*
comment_star	-?"*"+[^*/]* 
comment_end		-?"*"+"/"
comment_linebreak_n	[ \t]*[\n][ \t]*
comment_linebreak_r	[ \t]*[\r][ \t]*
%%


"/*"         			BEGIN(comment);


<comment>{comment_linebreak_n}    {line_counter++; }
<comment>{comment_linebreak_r}    {line_counter++; }
<comment>{comment_between}       /* eat anything that's not a '*' */
<comment>{comment_star}   		/* eat up '*'s not followed by '/'s */
<comment>{comment_end}  		BEGIN(INITIAL);


{int_literal}     { yylval->TK_INT_LITERAL = atol(yytext); return TK_INT_LITERAL; }

{float_literal}   { yylval->TK_FLOAT_LITERAL = atof(yytext); return TK_FLOAT_LITERAL; }

"true"   			{ yylval->TK_BOOL_LITERAL = atof(yytext); return TK_BOOL_LITERAL; }
"false"    			{ yylval->TK_BOOL_LITERAL = atof(yytext); return TK_BOOL_LITERAL; }

{string_literal}    { yylval->TK_STRING_LITERAL = yytext; return TK_STRING_LITERAL; }


"+"               { return TK_PLUS; }
"-"               { return TK_MINUS; }
"*"               { return TK_ASTER; }
"/"               { return TK_SLASH; }

"("               { return TK_LPARENTH; }
")"               { return TK_RPARENTH; }
"["               { return TK_LBRACKET; }
"]"               { return TK_RBRACKET; }
"{"               { return TK_LBRACE; }
"}"               { return TK_RBRACE; }

";"               { return TK_SEMICOLON; }

","               { return TK_COMMA; }

"<"               { return TK_LT; }
">"               { return TK_GT; }
"<="               { return TK_LE; }
">="               { return TK_GE; }
"&&"               { return TK_AND; }
"||"               { return TK_OR; }
"=="               { return TK_EQ; }
"!="               { return TK_NEQ; }
"!"               { return TK_FAC; }

"="                 {return TK_ASSIGNMENT; }

"void"				{ return TK_VOID; }
"bool"				{ return TK_BOOL; }
"int"				{return TK_INT; }
"float"				{ return TK_FLOAT; }
"string"				{ return TK_STRING; }
"return"            { return TK_RETURN; }
"if"                { return TK_IF; }
"else"              { return TK_ELSE; }
"while"             {return TK_WHILE; }

{identifier}    {yylval->TK_IDENTIFIER = malloc(sizeof(char*) * strlen(yytext)); strcpy(yylval->TK_IDENTIFIER,yytext); return TK_IDENTIFIER; }


[ \t]*[\n][ \t]*     	{line_counter++; }
[ \t]*[\r][ \t]*     	{line_counter++; }

[ \t]+  				{/* eat up whitespace */}

<<EOF>>           { return TK_END; }

.                 { BEGIN(LEXING_ERROR); yyless(0); }
<LEXING_ERROR>.+  { fprintf(stderr,"Invalid character '%c' found at line %d, just before '%s'\n", *yytext, line_counter, yytext+1); exit(1); }
