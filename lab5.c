
/*
 * Name: Prashant Raj
 * Roll Number: YOUR_ROLL_NUMBER
 * Lab Assignment 5: Regular Expression-Based Lexical Analyzer
 * Course: CS3104 - Compiler Design Laboratory
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>

#define MAX_INPUT 10000

typedef struct {
    const char *pattern;
    const char *token;
} TokenPattern;

/* Regular expressions for token categories */
TokenPattern patterns[] = {
    {"^[a-zA-Z_][a-zA-Z0-9_]*", "IDENTIFIER"},
    {"^[0-9]+\\.[0-9]+",         "REAL"},
    {"^[0-9]+",                  "INTEGER"},

    {"^==|^!=|^<=|^>=",           "RELATIONAL"},
    {"^<|^>",                    "RELATIONAL"},
    {"^=",                       "ASSIGN"},

    {"^\\+",                     "PLUS"},
    {"^-",                       "MINUS"},
    {"^\\*",                     "MULTIPLY"},
    {"^/",                       "DIVIDE"},
    {"^%",                       "MODULO"},

    {"^\\(",                     "LPAREN"},
    {"^\\)",                     "RPAREN"},
    {"^\\{",                     "LBRACE"},
    {"^\\}",                     "RBRACE"},
    {"^;",                       "SEMICOLON"},
    {"^,",                       "COMMA"},
    {"^\\.",                     "DOT"}
};

#define PATTERN_COUNT (sizeof(patterns) / sizeof(patterns[0]))

/* Check whether an identifier is a keyword */
int isKeyword(const char *word) {
    const char *keywords[] = {
        "if", "else", "while",
        "int", "string", "return"
    };

    int count = sizeof(keywords) / sizeof(keywords[0]);

    for (int i = 0; i < count; i++) {
        if (strcmp(word, keywords[i]) == 0)
            return 1;
    }

    return 0;
}

/* Return the length of the match, or zero if it does not match */
int matchLength(const char *input, const char *pattern) {
    regex_t regex;
    regmatch_t match;

    if (regcomp(&regex, pattern, REG_EXTENDED) != 0)
        return 0;

    int result = regexec(&regex, input, 1, &match, 0);
    int length = 0;

    if (result == 0 && match.rm_so == 0)
        length = (int)match.rm_eo;

    regfree(&regex);
    return length;
}

/* Scan the complete input and print tokens */
int analyze(const char *input) {
    int position = 0;
    int length = (int)strlen(input);
    int rejected = 0;

    while (position < length) {
        /* Ignore whitespace */
        if (isspace((unsigned char)input[position])) {
            position++;
            continue;
        }

        int longest = 0;
        int selected = -1;

        /* Find the longest matching token */
        for (int i = 0; i < PATTERN_COUNT; i++) {
            int current = matchLength(input + position,
                                      patterns[i].pattern);

            if (current > longest) {
                longest = current;
                selected = i;
            }
        }

        if (selected == -1) {
            unsigned char ch = (unsigned char)input[position];

            if (isprint(ch))
                printf("LEXICAL ERROR: Invalid character '%c'\n", ch);
            else
                printf("LEXICAL ERROR: Invalid character (ASCII %u)\n",
                       (unsigned int)ch);

            rejected = 1;
            break;
        }

        char lexeme[MAX_INPUT];
        if (longest >= MAX_INPUT) {
            printf("LEXICAL ERROR: Token too long\n");
            rejected = 1;
            break;
        }

        strncpy(lexeme, input + position, longest);
        lexeme[longest] = '\0';

        const char *token = patterns[selected].token;

        /* Distinguish keywords from identifiers */
        if (strcmp(token, "IDENTIFIER") == 0 &&
            isKeyword(lexeme)) {
            token = "KEYWORD";
        }

        /* Print the token class and its lexeme */
        printf("%-12s %s\n", token, lexeme);

        position += longest;
    }

    if (rejected) {
        printf("REJECT\n");
        return 0;
    }

    printf("ACCEPT\n");
    return 1;
}

int main(void) {
    char input[MAX_INPUT];
    int ch;
    int length = 0;

    /* Read the complete source program, including newlines */
    while ((ch = getchar()) != EOF) {
        if (length >= MAX_INPUT - 1) {
            printf("LEXICAL ERROR: Input too long\n");
            printf("REJECT\n");
            return 1;
        }

        input[length++] = (char)ch;
    }

    input[length] = '\0';

    analyze(input);

    return 0;
}
