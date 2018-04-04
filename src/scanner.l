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

int line_counter = 0;
%}


%x comment

int_literal   	-?[0-9]+
float_literal 	-?[0-9]+\.[0-9]+
string_literal	-?\"[^\"]*\"

identifier		-?[a-zA-Z_][a-zA-Z0-9_]*
comment_between	-?[^*\n]*
comment_star	-?"*"+[^*/\n]* 
comment_end		-?"*"+"/"

new_line		-?\n
%%


"/*"         			BEGIN(comment);


<comment>{comment_between}       /* eat anything that's not a '*' */
<comment>{comment_star}   		/* eat up '*'s not followed by '/'s */
<comment>{new_line}            	++line_counter;
<comment>{comment_end}  			BEGIN(INITIAL);



{new_line}        {line_counter++; }

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

","               { return TK_KOMMA; }

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

{identifier}    { yylval->TK_IDENTIFIER = yytext; return TK_IDENTIFIER; }


[ \t\r\n]+        { /* ignore */ }

<<EOF>>           { return TK_END; }

.                 { fprintf(stderr, "invalid character '%c' at line %d\n", yytext[0], line_counter); return TK_ERROR; }
