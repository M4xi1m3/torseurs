#include "expression.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
 * Append an ExpressionNode at the end of an Expression.
 */
void E_append(Expression* e, ExpressionNode* n) {
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

void E_print(Expression* self) {
    char buffer[50] = {0};
    E_get_display(self, buffer, 50);
    printf("%s\n", buffer);
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
        if (temp->num != 0.0)
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
int E__simplify_remove_zero(Expression* e) {
    // E_debug(e);
    // Store head node 
    ExpressionNode* temp = e->head, *prev; 

    if (e->head == NULL) {
        e->head = malloc(sizeof(ExpressionNode));
        e->head->next = NULL;
        e->head->num = 0;
        e->head->var = malloc(1);
        *e->head->var = '\0';
        return 1;
    }
    
    if (e->head->next == NULL) {
        if ((e->head->num + 0.0) == 0.0) {
            free(temp->var);
            temp->var = malloc(1);
            *temp->var = '\0';
        }
        return 1;
    }

    // If head node itself holds the key to be deleted 
    if (temp != NULL && (temp->num + 0.0) == 0.0) { 
        e->head = temp->next;     // Changed head
        free(temp->var);
        free(temp);               // free old head 
        return 0; 
    } 
  
    // Search for the key to be deleted, keep track of the 
    // previous node as we need to change 'prev->next' 
    while (temp != NULL && (temp->num + 0.0) != 0.0) { 
        prev = temp; 
        temp = temp->next; 
    } 
  
    // If key was not present in linked list 
    if (temp == NULL) return 1; 
  
    // Unlink the node from linked list 
    prev->next = temp->next; 
    
    free(temp->var);
    free(temp);  // Free memory 
}


/**
 * Simplify and expression.
 */
int E_simplify(Expression* self) {
    E__simplify_add(self);
    while(!E__simplify_remove_zero(self));
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
 * Check if an expression is equal to zero
 */
int E_is_zero(Expression* self) {
    ExpressionNode* temp = self->head;
    
    while(temp != NULL) {
        if (temp->num != 0.0) {
            return 0;
        }
        temp = temp->next;
    }
    return 1;
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
            
            var_names[num_vars] = malloc(strlen(temp->var) + 1);
            strcpy(var_names[num_vars], temp->var);
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
    Expression* copy = E_copy(other);
    
    ExpressionNode* temp = self->head;
    if(temp == NULL) {
        self->head = copy->head;
        return 0;
    }
    
    while(temp->next != NULL) {
        temp = temp->next;
    }
    
    temp->next = copy->head;
    
    copy->head = NULL;
    E_free(copy);
    return 0;
}

/**
 * Printf the shit out of it!
 */
void E_debug(Expression* self) {
    ExpressionNode* temp = self->head;
    printf(" -- Expression %p\n", self);
    while(temp != NULL) {
        printf("%p  N %p=>%s M %p=>%lg\n", temp, temp->var, temp->var, &temp->num, temp->num);
        temp = temp->next;
    }
}



