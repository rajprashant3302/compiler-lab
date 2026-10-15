
/* Name: Prashant Raj */
/* Roll Number: YOUR_ROLL_NUMBER */

%{
#include <stdio.h>
#include <string.h>
%}

%x COMMENT

%%

"//".*                  { /* Ignore single-line comment */ }

"/*"                    { BEGIN(COMMENT); }

<COMMENT>"*/"           { BEGIN(INITIAL); }
<COMMENT>\n             { /* Ignore newline */ }
<COMMENT>.              { /* Ignore comment character */ }
<COMMENT><<EOF>>        {
                            printf("Error: unterminated comment\n");
                            return 0;
                        }

"int"|"float"|"char"|"double"|"if"|"else"|"while"|"for"|"return"|"void"
                        { printf("%-12s -> KEYWORD\n", yytext); }

[0-9]+(\.[0-9]+)?       { printf("%-12s -> NUMBER\n", yytext); }

[a-zA-Z_][a-zA-Z0-9_]*  { printf("%-12s -> IDENTIFIER\n", yytext); }

"=="|"<="|">="|"!="     { printf("%-12s -> OPERATOR\n", yytext); }

"+"|"-"|"*"|"/"|"="|"<"|">"
                        { printf("%-12s -> OPERATOR\n", yytext); }

";"|","|"("|")"|"{"|"}"
                        { printf("%-12s -> PUNCTUATION\n", yytext); }

[ \t\r\n]+              { /* Ignore whitespace */ }

.                       { /* Ignore unrecognized characters */ }

%%

int main(void)
{
    yylex();
    return 0;
}

int yywrap(void)
{
    return 1;
}
