#include "expression.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
 * Copy an expression
 */
Expression* E_copy(Expression* e) {
    Expression* out = malloc(sizeof(Expression));
    
    // Check for null
    if (e->head == NULL) {
        out->head = NULL;
        return out;
    }
    
    // Copy head
    ExpressionNode* new_head = malloc(sizeof(ExpressionNode));
    new_head->num = e->head->num;
    new_head->var = malloc(strlen(e->head->var) + 1);
    strcpy(new_head->var, e->head->var);
    
    // Copy the rest
    ExpressionNode* p = new_head;
    ExpressionNode* temp = e->head->next;
    
    while(temp != NULL) {
        p->next = malloc(sizeof(ExpressionNode));
        p = p->next;
        p->num = temp->num;
        p->var = malloc(strlen(temp->var) + 1);
        strcpy(p->var, temp->var);
        
        temp = temp->next;
    }
    p->next = NULL;
    out->head = new_head;
    return out;
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
                
                free(temp->var);
                temp->var = malloc(strlen(current->var) + 1);
                strcpy(temp->var, current->var);
                
            } else if(strcmp(current->var, "") == 0) {
                temp->num *= current->num;
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
 * Symplify by adding same var ExpressionNodes.
 */
int E__simplify_add(Expression* self) {
    ExpressionNode* temp = self->head;
    
    while(temp != NULL) {
        ExpressionNode* current = temp->next;
        
        while(current != NULL) {
            
            if(strcmp(temp->var, current->var) == 0) {
                temp->num += current->num;
                current->num = 0;
                /*
                free(current->var);
                current->var = malloc(1);*/
                *current->var = '\0';
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
int E__simplify_remove_zero(Expression* self) {
    ExpressionNode* temp = self->head;
    ExpressionNode* prev = NULL;
    
    while(temp != NULL) {
        if(temp->num == 0) {
            if(prev == NULL) {
                if(temp->next == NULL) {
                    temp->num = 0;
                    free(temp->var);
                    temp->var = malloc(1);
                    *temp->var = '\0';
                } else {
                    self->head = temp->next;
                    
                    free(temp->var);
                    free(temp);
                }
            } else {
                if(temp->next == NULL) {
                    free(prev->next->var);
                    free(prev->next);
                    prev->next = NULL;
                } else {
                    prev->next = temp->next;
                    
                    free(temp->var);
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
    E__simplify_add(self);
    E__simplify_remove_zero(self);
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
    if (self->head == NULL) {
        free(self);
        return 1;
    }
    ExpressionNode* curr = self->head;
    while((curr = self->head) != NULL) {
        self->head = self->head->next;
        
        free(curr->var);
        free(curr);
    }
    free(self);
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
    
    other->head = NULL;
    return 0;
}

/**
 * Printf the shit out of it!
 */
void E_debug(Expression* self) {
    ExpressionNode* temp = self->head;
    printf("FS %d FZ %d\n");
    while(temp != NULL) {
        printf("%p  N %p=>%s M %p=>%g\n", temp, temp->var, temp->var, &temp->num, temp->num);
        temp = temp->next;
    }
}



