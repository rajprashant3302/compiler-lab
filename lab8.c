
/*
 * Name: Your Name
 * Roll Number: Your Roll Number
 * Course: CS3104 - Compiler Design Laboratory
 * Assignment 8: Shift-Reduce Parsing
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 1000

char input[MAX][20];
char stack[MAX][20];

int top = -1;
int input_pos = 0;
int input_count = 0;

/* Return operator precedence. */
int precedence(const char *op)
{
    if (strcmp(op, "*") == 0)
        return 2;

    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0)
        return 1;

    return 0;
}

/* Check whether a symbol is an operator. */
int is_operator(const char *s)
{
    return strcmp(s, "+") == 0 ||
           strcmp(s, "-") == 0 ||
           strcmp(s, "*") == 0;
}

/* Push a symbol onto the stack. */
void push(const char *symbol)
{
    if (top >= MAX - 1)
    {
        printf("REJECT: Stack overflow\n");
        exit(EXIT_FAILURE);
    }

    strcpy(stack[++top], symbol);
}

/* Display the current stack. */
void display_stack(void)
{
    int i;

    for (i = 0; i <= top; i++)
        printf("%s ", stack[i]);
}

/* Reduce id to E. */
int reduce_id(void)
{
    if (top >= 0 && strcmp(stack[top], "id") == 0)
    {
        printf("REDUCE: id -> E\n");
        strcpy(stack[top], "E");
        return 1;
    }

    return 0;
}

/* Reduce E operator E to E. */
int reduce_expression(void)
{
    if (top >= 2 &&
        strcmp(stack[top], "E") == 0 &&
        is_operator(stack[top - 1]) &&
        strcmp(stack[top - 2], "E") == 0)
    {
        printf("REDUCE: E %s E -> E\n", stack[top - 1]);

        strcpy(stack[top - 2], "E");
        top -= 2;

        return 1;
    }

    return 0;
}

/* Reduce ( E ) to E. */
int reduce_parentheses(void)
{
    if (top >= 2 &&
        strcmp(stack[top], ")") == 0 &&
        strcmp(stack[top - 1], "E") == 0 &&
        strcmp(stack[top - 2], "(") == 0)
    {
        printf("REDUCE: ( E ) -> E\n");

        strcpy(stack[top - 2], "E");
        top -= 2;

        return 1;
    }

    return 0;
}

/* Read and classify input tokens. */
int tokenize(void)
{
    char line[MAX];
    int i = 0;
    int j;

    if (fgets(line, sizeof(line), stdin) == NULL)
        return 0;

    while (line[i] != '\0')
    {
        if (isspace((unsigned char)line[i]))
        {
            i++;
            continue;
        }

        if (strncmp(&line[i], "id", 2) == 0 &&
            (line[i + 2] == '\0' ||
             isspace((unsigned char)line[i + 2]) ||
             strchr("+-*()", line[i + 2]) != NULL))
        {
            strcpy(input[input_count++], "id");
            i += 2;
        }
        else if (strchr("+-*()", line[i]) != NULL)
        {
            input[input_count][0] = line[i];
            input[input_count][1] = '\0';
            input_count++;
            i++;
        }
        else
        {
            printf("REJECT: Invalid input symbol '%c'\n", line[i]);
            return 0;
        }

        if (input_count >= MAX - 1)
        {
            printf("REJECT: Input too long\n");
            return 0;
        }
    }

    /* Append the end-of-input marker. */
    strcpy(input[input_count++], "$");

    /* Validate that all tokens are nonempty. */
    for (j = 0; j < input_count; j++)
    {
        if (input[j][0] == '\0')
            return 0;
    }

    return 1;
}

int main(void)
{
    int accepted = 0;

    if (!tokenize())
    {
        printf("REJECT\n");
        return 0;
    }

    /* Initially, the stack contains only $. */
    push("$");

    printf("\n--- SHIFT-REDUCE PARSING ---\n");

    while (1)
    {
        char *lookahead = input[input_pos];

        /*
         * ACCEPT when the stack is $ E
         * and the lookahead is $.
         */
        if (strcmp(lookahead, "$") == 0 &&
            top == 1 &&
            strcmp(stack[0], "$") == 0 &&
            strcmp(stack[1], "E") == 0)
        {
            printf("ACCEPT\n");
            accepted = 1;
            break;
        }

        /*
         * First, reduce an operand if possible.
         */
        if (reduce_id())
            continue;

        /*
         * Reduce parentheses before processing
         * surrounding operators.
         */
        if (reduce_parentheses())
            continue;

        /*
         * Resolve an operator conflict when the
         * stack ends with E operator E.
         */
        if (top >= 2 &&
            strcmp(stack[top], "E") == 0 &&
            is_operator(stack[top - 1]) &&
            strcmp(stack[top - 2], "E") == 0 &&
            (is_operator(lookahead) ||
             strcmp(lookahead, ")") == 0 ||
             strcmp(lookahead, "$") == 0))
        {
            char stack_op[20];

            strcpy(stack_op, stack[top - 1]);

            if (is_operator(lookahead))
            {
                printf("SHIFT/REDUCE CONFLICT: stack operator '%s', "
                       "incoming operator '%s'\n",
                       stack_op, lookahead);

                if (precedence(stack_op) >= precedence(lookahead))
                {
                    printf("DECISION: REDUCE (higher or equal "
                           "precedence; left associativity)\n");

                    reduce_expression();
                    continue;
                }
                else
                {
                    printf("DECISION: SHIFT (incoming operator "
                           "has higher precedence)\n");

                    printf("SHIFT: %s\n", lookahead);
                    push(lookahead);
                    input_pos++;
                    continue;
                }
            }
            else
            {
                printf("DECISION: REDUCE before '%s'\n", lookahead);
                reduce_expression();
                continue;
            }
        }

        /*
         * Shift operands, operators, and opening
         * parentheses.
         */
        if (strcmp(lookahead, "id") == 0 ||
            is_operator(lookahead) ||
            strcmp(lookahead, "(") == 0)
        {
            printf("SHIFT: %s\n", lookahead);
            push(lookahead);
            input_pos++;
            continue;
        }

        /*
         * Shift a closing parenthesis only when
         * a matching opening parenthesis exists.
         */
        if (strcmp(lookahead, ")") == 0)
        {
            int found_open = 0;
            int i;

            for (i = top; i >= 0; i--)
            {
                if (strcmp(stack[i], "(") == 0)
                {
                    found_open = 1;
                    break;
                }
            }

            if (found_open)
            {
                printf("SHIFT: )\n");
                push(")");
                input_pos++;
                continue;
            }
        }

        printf("REJECT: No valid SHIFT or REDUCE operation\n");
        break;
    }

    if (!accepted)
        printf("Final result: REJECT\n");

    return 0;
}
