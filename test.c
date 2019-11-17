#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct expressionNode ExpressionNode;
struct expressionNode {
    ExpressionNode* next;
    double num;
    char* var;
};

typedef struct expression Expression;
struct expression {
    ExpressionNode* head;
    int should_free_strings;
    int should_free_zero;
};

/**
 * Get display of individual expression node.
 */
int EN_get_individual_display(ExpressionNode* self, char* buffer, size_t buffer_len) {
    if (strcmp(self->var, "") == 0) {
        snprintf(buffer, buffer_len, "%G", self->num);
    } else {
        if (self->num == 1) {
            snprintf(buffer, buffer_len, "%s", self->var);
        } else if (self->num == -1) {
            snprintf(buffer, buffer_len, "-%s", self->var);
        } else {
            snprintf(buffer, buffer_len, "%g%s", self->num, self->var);
        }
    }
    return 0;
}

/**
 * Get string representing whole expression.
 */
int E_get_display(Expression* self, char* buffer, size_t buffer_len) {
    ExpressionNode* temp = self->head;
    char tmp_buffer[20] = {0};
    buffer[0] = 0;
    
    while(temp != NULL) {
        EN_get_individual_display(temp, tmp_buffer, 20);
        // Check for buffer overflow.
        if (strlen(buffer) + strlen(tmp_buffer) > buffer_len) {
            return 1;
        }
        
        strcat(buffer, tmp_buffer);
        
        if (temp->next != NULL) {
            // Check for buffer overflow.
            if (strlen(buffer) + 3 > buffer_len) {
                return 2;
            }
            
            strcat(buffer, " + ");
        }
        
        temp = temp->next;
    }
    
    return 0;
}

/**
 * Multiply the whole expression by a scalar.
 */
int E_multiply_scalar(Expression* self, double scalar) {
    ExpressionNode* temp = self->head;
    
    while(temp != NULL) {
        temp->num *= scalar;
        temp = temp->next;
    }
    
    return 0;
}

/**
 * Multiply two expression together.
 * (Warning: Error when trying to do var*var)
 */
int E_multiply(Expression* self, Expression* other) {
    ExpressionNode* temp = self->head;
    while(temp != NULL) {
        ExpressionNode* current = other->head;
        while(current != NULL) {
            if(strcmp(temp->var, "") == 0) {
                temp->num *= current->num;
                temp->var = current->var;
            } else if(strcmp(current->var, "") == 0) {
                temp->num = current->num;
            } else {
                return 1;
            }
            current = current->next;
        }
        temp = temp->next;
    }
    
    return 0;
}

/**
 * Add an expression to another.
 */
int E_add(Expression* self, Expression* other) {
    ExpressionNode* temp = self->head;
    if(temp == NULL) {
        self->head = other->head;
        return 0;
    }
    
    while(temp->next != NULL) {
        temp = temp->next;
    }
    
    temp->next = other->head;
    return 0;
}

/**
 * Symplify by adding same var ExpressionNodes.
 */
int E__simplify_add(Expression* self, int should_free) {
    ExpressionNode* temp = self->head;
    
    while(temp != NULL) {
        ExpressionNode* current = temp->next;
        
        while(current != NULL) {
            
            if(strcmp(temp->var, current->var) == 0) {
                temp->num += current->num;
                current->num = 0;
                if(should_free)
                    free(current->var);
                current->var = "";
            }
            current = current->next;
        }
        temp = temp->next;
    }
    return 0;
}

/**
 * Symplify by removing ExpressionNodes with num = 0.
 */
int E__simplify_remove_zero(Expression* self, int should_free) {
    ExpressionNode* temp = self->head;
    ExpressionNode* prev = NULL;
    
    while(temp != NULL) {
        if(temp->num == 0) {
            if(prev == NULL) {
                if(temp->next == NULL) {
                    temp->num = 0;
                    temp->var = "";
                } else {
                    self->head = temp->next;
                    
                    if(should_free)
                        free(temp);
                }
            } else {
                if(temp->next == NULL) {
                    if(should_free)
                        free(prev->next);
                    prev->next = NULL;
                } else {
                    prev->next = temp->next;
                    
                    if(should_free)
                        free(temp);
                }
                temp = prev;
                prev = NULL;
                continue;
            }
        }
        prev = temp;
        temp = temp->next;
    }
    
    return 0;
}

/**
 * Simplify and expression.
 */
int E_simplify(Expression* self) {
    E__simplify_add(self, self->should_free_strings);
    E__simplify_remove_zero(self, self->should_free_zero);
    return 0;
}

/**
 * Get the value of a specific variable.
 */
double E_get_value_for(Expression* self, char* var) {
    ExpressionNode* temp = self->head;
    
    while(temp != NULL) {
        if(strcmp(temp->var, var) == 0) {
            return temp->num;
        }
        temp = temp->next;
    }
    return 0;
}

/**
 * Get list of all variables.
 */
int E_get_variables(Expression* self, char** var_names, int max_vars) {
    ExpressionNode* temp = self->head;
    
    int num_vars = 0;
    
    while(temp != NULL) {
        int in_array = 0;
        for(int i = 0; i < num_vars; i++) {
            if(strcmp(var_names[i], temp->var) == 0) {
                in_array = 1;
                break;
            }
        }
        
        if(!in_array) {
            if (num_vars >= max_vars)
                return -1;
            
            var_names[num_vars] = temp->var;
            num_vars++;
        }
        
        temp = temp->next;
    }
    return num_vars;
}

/**
 * Free everything.
 */
int E_free(Expression* self) {
    if (self->head == NULL)
        return 1;
    ExpressionNode* curr = self->head;
    while((curr = self->head) != NULL) {
        self->head = self->head->next;
        
        if (self->should_free_strings)
            free(curr->var);
        free(curr);
    }
    return 0;
}

/**
 * Printf the shit out of it!
 */
void E_debug(Expression* self) {
    ExpressionNode* temp = self->head;
    printf("FS %d FZ %d\n", self->should_free_strings, self->should_free_zero);
    while(temp != NULL) {
        printf("  N %s M %g\n", temp->var, temp->num);
        temp = temp->next;
    }
}

#define EP_STATE_READ_NUM   0
#define EP_STATE_READ_VAR   1

#define EP_BUFFER_LEN       40

void EP_append(Expression* e, ExpressionNode* n) {
    if (e->head == NULL) {
        e->head = n;
        return;
    }

    ExpressionNode* temp = e->head;
    while(temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = n;
}

/**
 * Parses an expression from a string.
 */
int EP_parse(Expression* e, char* expression) {
    E_free(e);
    e->should_free_strings = 1;
    e->should_free_zero = 1;

    char buffer[EP_BUFFER_LEN] = {0};
    int tmp_len = 0;
    
    ExpressionNode* temp_expression = malloc(sizeof(ExpressionNode));
    temp_expression->next = NULL;
    temp_expression->num = 0;
    temp_expression->var = "";

    int state = EP_STATE_READ_NUM;
    int expression_len = strlen(expression);
    for(int i = 0; i <= expression_len; i++) {
        switch(state) {
            case EP_STATE_READ_NUM:
                if (!isspace(expression[i]))
                    if (strchr("-+0123456789.", expression[i]) == NULL) {
                        buffer[tmp_len++] = '\0';
                        sscanf(buffer, "%lg", &(temp_expression->num));
                        
                        memset(buffer, 0, EP_BUFFER_LEN);
                        state = EP_STATE_READ_VAR;
                        tmp_len = 0;
                        i--;
                    } else {
                        buffer[tmp_len++] = expression[i];
                    }
                if(i == expression_len) {
                    buffer[tmp_len++] = '\0';
                    sscanf(buffer, "%lg", &(temp_expression->num));
                    temp_expression->var = malloc(1);
                    *(temp_expression->var) = '\0';
                    EP_append(e, temp_expression);
                    temp_expression = NULL;
                }
                break;
            case EP_STATE_READ_VAR:
                if (!isspace(expression[i]))
                    if (expression[i] == '+' || expression[i] == '-') {
                        
                        temp_expression->var = malloc(strlen(buffer) + 1);
                        strcpy(temp_expression->var, buffer);
                        EP_append(e, temp_expression);
                        
                        temp_expression = malloc(sizeof(ExpressionNode));
                        temp_expression->next = NULL;
                        temp_expression->num = 0;
                        temp_expression->var = "";
                        
                        memset(buffer, 0, EP_BUFFER_LEN);
                        state = EP_STATE_READ_NUM;
                        tmp_len = 0;
                        
                        if (expression[i] == '-') {
                            buffer[tmp_len++] = '-';
                        }
                        
                    } else {
                        buffer[tmp_len++] = expression[i];
                    }
                if(i == expression_len) {
                    temp_expression->var = malloc(strlen(buffer) + 1);
                    strcpy(temp_expression->var, buffer);
                    EP_append(e, temp_expression);
                    temp_expression = NULL;
                }
                break;
        }
    }
}

int main() {
    Expression e = {NULL, 0, 0};
    
    EP_parse(&e, "3x + 12y -7z -2");
    E_debug(&e);
    char buffer[60];
    E_get_display(&e, buffer, 60);
    printf("%s\n", buffer);
    
    E_free(&e);
    return 0;
}
