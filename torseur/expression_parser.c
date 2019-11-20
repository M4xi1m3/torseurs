#include "expression_parser.h"
#include "expression.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

/**
 * Split a token into the number and the var part.
 */
int EP__split_number(char* token, char* number, char* var) {
    int var_name = 0;
    char c;
    
    int number_buffer_len = 0;
    int var_len = 0;
    int i = 0;
    
    do {
        c = token[i];
        i++;
        if (isspace(c)) {
            continue;
        }
        
        if (strchr("-+0123456789.", c) != NULL && var_name == 0) {
            number[number_buffer_len++] = c;
        } else {
            var_name = 1;
        }
        if (var_name) {
            var[var_len++] = c;
        }
    } while(c != '\0');
    var[var_len++] = '\0';
    number[number_buffer_len++] = '\0';
    
    return 0;
}

/**
 * Parses an expression from a string.
 */
Expression* EP_parse(char* expression) {
    Expression* e = malloc(sizeof(Expression));
    e->head = NULL;
    
    char* expression_copy = malloc(strlen(expression) + 1);
    strcpy(expression_copy, expression);
    
    char* token = strtok(expression_copy, "+");
    
    char* var = malloc(40);
    double number = 0;
    char* num = malloc(40);
    
    while (token != NULL) {
        EP__split_number(token, num, var);

        ExpressionNode* temp_expression = malloc(sizeof(ExpressionNode));
        temp_expression->next = NULL;

        if (strcmp(num, "-") == 0) {
            temp_expression->num = -1;
        } else if (strcmp(num, "") == 0 || strcmp(num, "+") == 0) {
            temp_expression->num = 1;
        } else {
            sscanf(num, "%lg", &(temp_expression->num));
        }
        
        char* new_var = malloc(strlen(var) + 1);
        strcpy(new_var, var);
        temp_expression->var = new_var;
        E_append(e, temp_expression);
        
        // printf("%s => '%s' : '%s'\n", token, num, new_var);
        token = strtok(NULL, "+");
    }
    
    free(expression_copy);
    free(var);
    free(num);
    return e;
}

