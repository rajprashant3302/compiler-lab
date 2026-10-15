
/*
 * Name: Prashant Raj
 * Roll Number: YOUR_ROLL_NUMBER
 * Course: CS3104 - Compiler Design Laboratory
 * Lab Assignment 2: Two-Pass Expression Translator
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 1000

char postfix[MAX][MAX];
int postfixCount = 0;

/* Check operator precedence */
int precedence(char op)
{
    if (op == '*' || op == '/')
        return 2;

    if (op == '+' || op == '-')
        return 1;

    return 0;
}

/* Phase 1: Convert infix expression to postfix */
int toPostfix(char infix[])
{
    char stack[MAX];
    int top = -1;
    int i = 0;
    int expectOperand = 1;

    postfixCount = 0;

    while (infix[i] != '\0')
    {
        if (isspace((unsigned char)infix[i]))
        {
            i++;
            continue;
        }

        /* Read a multi-digit non-negative integer */
        if (isdigit((unsigned char)infix[i]))
        {
            if (!expectOperand)
            {
                printf("Error: Missing operator between operands.\n");
                return 0;
            }

            int j = 0;

            while (isdigit((unsigned char)infix[i]))
            {
                if (j >= MAX - 1)
                {
                    printf("Error: Number is too long.\n");
                    return 0;
                }

                postfix[postfixCount][j++] = infix[i++];
            }

            postfix[postfixCount][j] = '\0';
            postfixCount++;
            expectOperand = 0;
            continue;
        }

        char ch = infix[i];

        /* Opening parenthesis */
        if (ch == '(')
        {
            if (!expectOperand)
            {
                printf("Error: Missing operator before '('.\n");
                return 0;
            }

            stack[++top] = ch;
            expectOperand = 1;
        }

        /* Closing parenthesis */
        else if (ch == ')')
        {
            if (expectOperand)
            {
                printf("Error: Invalid or empty parentheses.\n");
                return 0;
            }

            while (top >= 0 && stack[top] != '(')
            {
                char op[2] = {stack[top--], '\0'};
                strcpy(postfix[postfixCount++], op);
            }

            if (top < 0)
            {
                printf("Error: Mismatched parentheses.\n");
                return 0;
            }

            top--;
            expectOperand = 0;
        }

        /* Arithmetic operators */
        else if (ch == '+' || ch == '-' || ch == '*' || ch == '/')
        {
            if (expectOperand)
            {
                printf("Error: Operator in an invalid position.\n");
                return 0;
            }

            while (top >= 0 && stack[top] != '(' &&
                   precedence(stack[top]) >= precedence(ch))
            {
                char op[2] = {stack[top--], '\0'};
                strcpy(postfix[postfixCount++], op);
            }

            stack[++top] = ch;
            expectOperand = 1;
        }

        /* Invalid character */
        else
        {
            printf("Error: Invalid character '%c'.\n", ch);
            return 0;
        }

        i++;
    }

    if (postfixCount == 0 || expectOperand)
    {
        printf("Error: Incomplete or empty expression.\n");
        return 0;
    }

    /* Pop remaining operators */
    while (top >= 0)
    {
        if (stack[top] == '(')
        {
            printf("Error: Mismatched parentheses.\n");
            return 0;
        }

        char op[2] = {stack[top--], '\0'};
        strcpy(postfix[postfixCount++], op);
    }

    return 1;
}

/* Phase 2: Evaluate postfix expression */
int evalPostfix(int *result)
{
    int stack[MAX];
    int top = -1;

    for (int i = 0; i < postfixCount; i++)
    {
        char *token = postfix[i];

        /* If token is a number, push it */
        if (isdigit((unsigned char)token[0]))
        {
            char *end;
            long long value = strtoll(token, &end, 10);

            if (*end != '\0' || value > 2147483647LL)
            {
                printf("Error: Integer out of range.\n");
                return 0;
            }

            stack[++top] = (int)value;
        }
        else
        {
            /* Every binary operator needs two operands */
            if (top < 1)
            {
                printf("Error: Invalid postfix expression.\n");
                return 0;
            }

            int b = stack[top--];
            int a = stack[top--];
            long long value;

            switch (token[0])
            {
                case '+':
                    value = (long long)a + b;
                    break;

                case '-':
                    value = (long long)a - b;
                    break;

                case '*':
                    value = (long long)a * b;
                    break;

                case '/':
                    if (b == 0)
                    {
                        printf("Error: Division by zero.\n");
                        return 0;
                    }

                    value = a / b;
                    break;

                default:
                    printf("Error: Unknown operator.\n");
                    return 0;
            }

            if (value < -2147483647LL - 1 ||
                value > 2147483647LL)
            {
                printf("Error: Integer overflow.\n");
                return 0;
            }

            stack[++top] = (int)value;
        }
    }

    if (top != 0)
    {
        printf("Error: Invalid postfix expression.\n");
        return 0;
    }

    *result = stack[top];
    return 1;
}

int main(void)
{
    char infix[MAX];
    int result;

    printf("Enter expression: ");

    if (fgets(infix, sizeof(infix), stdin) == NULL)
    {
        printf("Error: Unable to read expression.\n");
        return 1;
    }

    if (!toPostfix(infix))
        return 1;

    /* Print Phase 1 output */
    printf("Postfix: ");

    for (int i = 0; i < postfixCount; i++)
        printf("%s ", postfix[i]);

    printf("\n");

    /* Phase 2 consumes only the postfix token list */
    if (!evalPostfix(&result))
        return 1;

    printf("Result: %d\n", result);

    return 0;
}
