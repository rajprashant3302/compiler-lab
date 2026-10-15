
/* Name: Prashant Raj */
/* Roll Number: 2401CS04 */
/* CS3104 - Lab Assignment 6: Source Code Analyzer Using Lex */

%option yylineno noyywrap

%x COMMENT

%{
#include <stdio.h>

int single_comments = 0;
int multi_comments = 0;
int string_count = 0;
int char_count = 0;
int preprocessor_count = 0;
int identifier_count = 0;
int lexical_error = 0;

int saw_input = 0;
int last_was_newline = 0;
int total_lines = 0;

/* Track whether input exists and whether its last character is '\n'. */
#define YY_USER_ACTION \
    do { \
        if (yyleng > 0) { \
            saw_input = 1; \
            last_was_newline = (yytext[yyleng - 1] == '\n'); \
        } \
    } while (0);

void print_summary(void)
{
    total_lines = saw_input
                  ? yylineno - (last_was_newline ? 1 : 0)
                  : 0;

    printf("\n------------------------------\n");
    printf("Source Code Statistics\n");
    printf("------------------------------\n");
    printf("Lines              : %d\n", total_lines);
    printf("Single-line comments: %d\n", single_comments);
    printf("Multi-line comments : %d\n", multi_comments);
    printf("Comments            : %d\n",
           single_comments + multi_comments);
    printf("String literals     : %d\n", string_count);
    printf("Character literals  : %d\n", char_count);
    printf("Preprocessor        : %d\n", preprocessor_count);
    printf("Identifiers         : %d\n", identifier_count);
    printf("------------------------------\n");

    if (lexical_error)
        printf("REJECT\n");
    else
        printf("ACCEPT\n");
}
%}

/* Reusable regular expressions */
ID          [a-zA-Z_][a-zA-Z0-9_]*
ESCAPE      \\.
STRCHAR     ([^\\\"\n]|{ESCAPE})
CHARCHAR    ([^\\'\n]|{ESCAPE})

%%

/* Ignore whitespace outside comments and literals */
[ \t\r]+                  ;
\n                        ;

/* Preprocessor directive: a complete line beginning with # */
^[ \t]*#[^\n]*            {
                              preprocessor_count++;
                              printf("PREPROCESSOR      %s\n", yytext);
                          }

/* Single-line comment */
"//"[^\n]*                {
                              single_comments++;
                          }

/* Start of multi-line comment */
"/*"                      {
                              multi_comments++;
                              BEGIN(COMMENT);
                          }

/* Valid string literal, including escaped characters */
\"{STRCHAR}*\"            {
                              string_count++;
                              printf("STRING_LITERAL    %s\n", yytext);
                          }

/* Unterminated string at newline */
\"{STRCHAR}*              {
                              lexical_error = 1;
                              printf("Lexical Error: unterminated string '%s'\n",
                                     yytext);
                          }

/* Valid character literal: one character or one escape sequence */
\'{CHARCHAR}\'            {
                              char_count++;
                              printf("CHAR_LITERAL      %s\n", yytext);
                          }

/* Unterminated or malformed character literal */
\'{CHARCHAR}*             {
                              lexical_error = 1;
                              printf("Lexical Error: unterminated or malformed character literal '%s'\n",
                                     yytext);
                          }

/* Identifiers */
{ID}                      {
                              identifier_count++;
                              printf("IDENTIFIER        %s\n", yytext);
                          }

/* Ignore all other ordinary source characters */
.                         ;

/* Rules active only inside a multi-line comment */
<COMMENT>"*/"             {
                              BEGIN(INITIAL);
                          }

<COMMENT>\n               ;

<COMMENT>[^*\n]+          ;

<COMMENT>"*"              ;

/* Unterminated multi-line comment at EOF */
<COMMENT><<EOF>>          {
                              lexical_error = 1;
                              printf("Lexical Error: unterminated multi-line comment\n");
                              print_summary();
                              return 0;
                          }

/* Normal end of input */
<<EOF>>                   {
                              print_summary();
                              return 0;
                          }

%%

int main(void)
{
    yylex();
    return 0;
}
