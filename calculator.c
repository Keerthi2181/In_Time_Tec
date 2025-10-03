#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_LEN 100

void trimWhitespace(char* inputStr) {
    int i, k = 0;
    for (i = 0; inputStr[i]; i++) {
        if (!isspace(inputStr[i])) {
            inputStr[k++] = inputStr[i];
        }
    }
    inputStr[k] = '\0';
}

int validateExpression(char *expression) {
    for (int i = 0; expression[i]; i++) {
        if (!isdigit(expression[i]) && !strchr("+-*/", expression[i])) {
            return 0;
        }
    }
    return 1;
}

int calculate(char *expression, int *err) {
    int nums[MAX_LEN], num_idx = 0;
    char operators[MAX_LEN], op_idx = 0;
    int i = 0;

    while (expression[i]) {
        if (isdigit(expression[i])) {
            int current_val = 0;
            while (isdigit(expression[i])) {
                current_val = current_val * 10 + (expression[i] - '0');
                i++;
            }
            nums[num_idx++] = current_val;
        } else {
            operators[op_idx++] = expression[i];
            i++;
        }
    }

    for (i = 0; i < op_idx; i++) {
        if (operators[i] == '*' || operators[i] == '/') {
            if (operators[i] == '/' && nums[i+1] == 0) {
                *err = 1;
                return 0;
            }
            nums[i] = (operators[i] == '*') ? (nums[i] * nums[i+1]) : (nums[i] / nums[i+1]);

            for (int j = i + 1; j < num_idx - 1; j++) nums[j] = nums[j+1];
            for (int j = i; j < op_idx - 1; j++) operators[j] = operators[j+1];

            num_idx--;
            op_idx--;
            i--;
        }
    }

    int final_result = nums[0];
    for (i = 0; i < op_idx; i++) {
        if (operators[i] == '+') final_result += nums[i+1];
        else final_result -= nums[i+1];
    }

    return final_result;
}

int main() {
    char user_input[MAX_LEN];
    printf("Enter expression: ");
    fgets(user_input, MAX_LEN, stdin);

    trimWhitespace(user_input);

    if (!validateExpression(user_input)) {
        printf("Error: Invalid expression.\n");
        return 1;
    }

    int error_code = 0;
    int solution = calculate(user_input, &error_code);

    if (error_code) {
        printf("Error: Division by zero.\n");
    } else {
        printf("%d\n", solution);
    }

    return 0;
}
