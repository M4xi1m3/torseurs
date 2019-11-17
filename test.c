#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct expressionNode ExpressionNode;
struct expressionNode {
    ExpressionNode* next;
    double num;
    char* var;
};

typedef struct expression Expression;
struct expression {
    ExpressionNode* head;
};

/**
 * Get display of individual expression node.
 */
int EN_get_individual_display(ExpressionNode* self, char* buffer, size_t buffer_len) {
    if (strcmp(self->var, "") == 0) {
        snprintf(buffer, buffer_len, "%g", self->num);
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
    E__simplify_add(self, 0);
    E__simplify_remove_zero(self, 1);
    return 0;
}

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

int main() {
    ExpressionNode* a = malloc(sizeof(ExpressionNode));
    a->next = NULL;
    a->num = 42;
    a->var = "x";
    ExpressionNode* b = malloc(sizeof(ExpressionNode));
    b->next = a;
    b->num = 36;
    b->var = "y";
    ExpressionNode* c = malloc(sizeof(ExpressionNode));
    c->next = b;
    c->num = 12;
    c->var = "";
    
    Expression e = {c};
    
    ExpressionNode* a2 = malloc(sizeof(ExpressionNode));
    a2->next = NULL;
    a2->num = 6;
    a2->var = "x";
    ExpressionNode* b2 = malloc(sizeof(ExpressionNode));
    b2->next = a2;
    b2->num = 9;
    b2->var = "y";
    ExpressionNode* c2 = malloc(sizeof(ExpressionNode));
    c2->next = b2;
    c2->num = 3.5;
    c2->var = "";
    
    Expression e2 = {c2};
    
    char buffer[60];
    E_multiply_scalar(&e, 20);
    E_add(&e, &e2);
    
    E_simplify(&e);
    
    char** var_names = malloc(20 * sizeof(char*));
    int vars = E_get_variables(&e, var_names, 20);
    
    for(int i = 0; i < vars; i++) {
        printf("%s\n", var_names[i]);
    }
    
    E_get_display(&e, buffer, 60);
    printf("%s\n", buffer);
    printf("%g\n", E_get_value_for(&e, "x"));
    
    
    return 0;
}
