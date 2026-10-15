/* Name: Prashant Raj*/
/* Roll Number: 2401CS04 */

%option yylineno
%option noyywrap

%{
#include <stdio.h>
#include <string.h>

#define MAX_SYMBOLS 1000

typedef struct {
    char name[100];
    int first_line;
    int count;
} Symbol;

Symbol table[MAX_SYMBOLS];
int symbol_count = 0;

int is_keyword(char *s)
{
    char *keywords[] = {
        "int", "float", "char", "double",
        "if", "else", "while", "for",
        "return", "void"
    };

    for (int i = 0; i < 10; i++) {
        if (strcmp(s, keywords[i]) == 0)
            return 1;
    }

    return 0;
}

void add_symbol(char *name, int line)
{
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(table[i].name, name) == 0) {
            table[i].count++;
            return;
        }
    }

    strcpy(table[symbol_count].name, name);
    table[symbol_count].first_line = line;
    table[symbol_count].count = 1;
    symbol_count++;
}

void print_symbol_table()
{
    printf("\nSymbol Table:\n");

    for (int i = 0; i < symbol_count; i++) {
        printf(" %s first line %d occurrences %d\n",
               table[i].name,
               table[i].first_line,
               table[i].count);
    }
}
%}

%%

[   \t\r]+ ;

\n ;

"int"|"float"|"char"|"double"|"if"|"else"|"while"|"for"|"return"|"void" {
    printf("%d: %s -> KEYWORD\n", yylineno, yytext);
}

[0-9]+(\.[0-9]+){2,} {
    printf("Lexical Error at line %d: malformed number '%s'\n", yylineno, yytext);
}

[0-9]+\.[0-9]+[a-zA-Z_][a-zA-Z0-9_]* {
    printf("Lexical Error at line %d: malformed number '%s'\n", yylineno, yytext);
}

[0-9]+[a-zA-Z_][a-zA-Z0-9_]* {
    printf("Lexical Error at line %d: malformed number '%s'\n", yylineno, yytext);
}

[0-9]+\.[0-9]+ {
    printf("%d: %s -> NUMBER\n", yylineno, yytext);
}

[0-9]+ {
    printf("%d: %s -> NUMBER\n", yylineno, yytext);
}

[a-zA-Z_][a-zA-Z0-9_]* {
    if (is_keyword(yytext)) {
        printf("%d: %s -> KEYWORD\n", yylineno, yytext);
    } else {
        printf("%d: %s -> IDENTIFIER\n", yylineno, yytext);
        add_symbol(yytext, yylineno);
    }
}

\"[^\"\n]*\" {
    printf("%d: %s -> STRING\n", yylineno, yytext);
}

\"[^\"\n]* {
    printf("Lexical Error at line %d: unterminated string '%s'\n", yylineno, yytext);
}

"=="|"<="|">="|"!=" {
    printf("%d: %s -> OPERATOR\n", yylineno, yytext);
}

"+"|"-"|"*"|"/"|"="|"<"|">" {
    printf("%d: %s -> OPERATOR\n", yylineno, yytext);
}

";"|","|"("|")"|"{"|"}" {
    printf("%d: %s -> PUNCTUATION\n", yylineno, yytext);
}

. {
    printf("Lexical Error at line %d: unexpected character '%s'\n",
           yylineno, yytext);
}

<<EOF>> {
    print_symbol_table();
    return 0;
}

%%