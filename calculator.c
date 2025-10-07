#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LEN 100

void trimWhitespace(char *inputStr) {
    int i, k = 0;
    for (i = 0; inputStr[i]; i++) {
        if (!isspace(inputStr[i])) {
            inputStr[k++] = inputStr[i];
        }
    }
    inputStr[k] = '\0';
}

int validateExpression(char *expression) {
    int length = strlen(expression);
    if (length == 0) return 0;

    if (strchr("+-*/", expression[0]) || strchr("+-*/", expression[length - 1])) {
        return 0;
    }

    bool checkdigit= true;

    for (int i = 0; i < length; i++) {
        char ch = expression[i];

        if (isdigit(ch)) {
            while (isdigit(expression[i + 1])) {
                i++;
            }
            checkdigit= false;
        } 
        else if (strchr("+-*/", ch)) {
            if (checkdigit) return 0;
            checkdigit= true;
        } 
        else {
            return 0;
        }
    }

    if (checkdigit) return 0;

    return 1;
}

int calculate(char *expression, int *err) {
    int numstack[MAX_LEN];
    char opstack[MAX_LEN];
    int numtop = -1;
    int optop = -1;

    int i = 0;
    while (expression[i]) {
        if (isdigit(expression[i])) {
            int cur = 0;
            while (isdigit(expression[i])) {
                cur= cur * 10 + (expression[i] - '0');
                i++;
            }
            numstack[++numtop] = cur;
        } 
        else if (strchr("+-*/", expression[i])) {
            char op = expression[i];

            if (op == '*' || op == '/') {
                if (numtop < 0) {
                    *err = 1;
                    return 0;
                }
                int left = numstack[numtop--];

                i++;
                if (!isdigit(expression[i])) {
                    *err = 1;
                    return 0;
                }
                int right = 0;
                while (isdigit(expression[i])) {
                    right = right * 10 + (expression[i] - '0');
                    i++;
                }

                if (op == '/' && right == 0) {
                    *err = 1;
                    return 0;
                }

                int res = (op == '*') ? (left * right) : (left / right);
                numstack[++numtop] = res;
                continue;
            } 
            else {
                opstack[++optop] = op;
            }
            i++;
        } 
        else {
            *err = 1;
            return 0;
        }
    }

    int result = numstack[0];
    int numindex = 1;
    for (int j = 0; j <= optop; j++) {
        if (opstack[j] == '+') result += numstack[numindex++];
        else if (opstack[j] == '-') result -= numstack[numindex++];
    }

    return result;
}

int main() {
    char inputexp[MAX_LEN];
    printf("Enter expression: ");
    fgets(inputexp, MAX_LEN, stdin);

    trimWhitespace(inputexp);

    if (!validateExpression(inputexp)) {
        printf("Error: Invalid expression.\n");
        return 1;
    }

    int error = 0;
    int solution = calculate(inputexp, &error);

    if (error) {
        printf("Error: Division by zero or invalid sequence.\n");
    } else {
        printf("Result: %d\n", solution);
    }

    return 0;
}
