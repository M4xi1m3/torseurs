#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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

typedef struct solutions Solutions;
struct solutions {
    char** names;
    double* values;
    int number;
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
    free(self);
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

/* Affichage du système */
void ES_show_system(double **A, double *b, int n) {
	int i , j ;
	for(i = 0 ; i < n ; i++)
	{
		printf("  (");
		for(j = 0 ; j < n ; j++)
		{
			printf("  %.3lg  ",A[i][j]);
		}
		printf(" )    (X%d)   =",i+1);
		printf("\t%.3f",b[i]);
		printf("\n");
	}
}

double** ES_malloc_matrix(int rows, int cols) {
    double ** ptr = (double**) malloc(sizeof(double*) * rows + sizeof (double) * cols * rows) ;
    double *  dat = (double* ) (ptr+rows) ;
    int i ;
    if ( ptr == NULL ) return NULL;
    for ( i = 0 ; i < rows ; ++ i , dat += cols ) ptr [ i ] = dat;
    return ptr;
}

/**
 * Gauss elimination.
 *
 * http://www.student.montefiore.ulg.ac.be/~ggilles/C/gauss.c
 */
int ES__gauss(double **A, double *b, double *x, int n) {
     int i, j, k;
     int imin;
     double p;
     double sum, valmin, tump1, tump2;
     
     for(k = 0 ; k < n-1 ; k++) {
        /* Dans un premier temps, on cherche l'élément minimum (non */
        /* nul) en valeur absolue dans la colonne k et d'indice i   */
        /* supérieur ou égal à k.                                   */
        
        valmin = A[k][k] ; imin = k ;
        for(i = k+1 ; i < n ; i++)
        {
           if (valmin != 0)
           {
              if (abs(A[i][k]) < abs(valmin) && A[i][k] != 0)
              {
                 valmin = A[i][k] ;
                 imin = i ;
              }
           }
           else 
           {
                 valmin = A[i][k] ;
                 imin = i ;
           }     
        }
        
        /* Si l'élément minimum est nul, on peut en déduire */
        /* que la matrice est singulière. Le pogramme est   */
        /* alors interrompu.                                */
        
        if (valmin == 0.)
        {
           return 1;
        }
        
        /* Si la matrice n'est pas singulière, on inverse    */
        /* les éléments de la ligne imax avec les éléments   */
        /* de la ligne k. On fait de même avec le vecteur b. */
        
        for(j = 0 ; j < n ; j++)
        {
           tump1 = A[imin][j] ;
           A[imin][j] = A[k][j] ;
           A[k][j] = tump1 ;
        }
        
        tump2 = b[imin] ;
        b[imin] = b[k] ;
        b[k] = tump2 ;
        
        
        /* On procède à la réduction de la matrice par la */
        /* méthode d'élimination de Gauss. */
        
        for(i = k+1 ; i < n ; i++)
        {
           p = A[i][k]/A[k][k] ;
           
           for(j = 0 ; j < n ; j++)
           {
              A[i][j] = A[i][j] - p*A[k][j] ;
           }
           
           b[i] = b[i] - p*b[k] ; 
        }
     }   
     
     /* On vérifie que la matrice n'est toujours pas singulière. */
     /* Si c'est le cas, on interrompt le programme. */
     
     if (A[n-1][n-1] == 0)
     {
        return 1;
     }
     
     /* Une fois le système réduit, on obtient une matrice triangulaire */
     /* supérieure et la résolution du système se fait très simplement. */
     
     x[n-1] = b[n-1]/A[n-1][n-1] ;
     
     for(i = n-2 ; i > -1 ; i--)
     {
           sum = 0 ;
           
           for(j = n-1 ; j > i ; j--)
           {
              sum = sum + A[i][j]*x[j] ;
           }
           x[i] = (b[i] - sum)/A[i][i] ;
     }
     
     return 0;
}

int ES__remove_duplicates(char** buffer, int num_vars, char** out_buffer) {
    int num_elements = 0;
    for(int i = 0; i < num_vars; i++) {
        int in_array = 0;
        for(int j = 0; j < num_elements; j++) {
            if (out_buffer[j] != NULL) {
                if (strcmp(out_buffer[j], buffer[i]) == 0 || strcmp(buffer[i], "") == 0) {
                    in_array = 1;
                    break;
                }
            }
        }
        if (!in_array) {
            out_buffer[num_elements++] = buffer[i];
        }
    }
    return num_elements;
}

void ES_free(Solutions* s) {
    free(s->values);
    free(s->names);
    free(s);
}

Solutions* ES_solve(Expression** expressions, int num_expressions) {
    char** buffer = malloc(40 * sizeof(char*));
    char** out_buffer = malloc(40 * sizeof(char*));
    int num_vars = 0;
    
    for(int i = 0; i < num_expressions; i++) {
        num_vars += E_get_variables(expressions[i], buffer + num_vars, 40 - num_vars);
    }
    
    int num_elements = ES__remove_duplicates(buffer, num_vars, out_buffer);
    free(buffer);
    
    double** matrix = ES_malloc_matrix(num_elements, num_elements);
    double* value = malloc(num_elements * sizeof(double));
    double* solutions = malloc(num_elements * sizeof(double));
    
    for(int i = 0; i < num_elements; i++) {
        for(int j = 0; j < num_elements; j++) {
            if (strcmp(out_buffer[j], "") == 0) {
                continue;
            }
            matrix[i][j] = E_get_value_for(expressions[i], out_buffer[j]);
        }
        value[i] = -E_get_value_for(expressions[i], "");
    }
    
    // ES_show_system(matrix, value, num_elements);
    
    ES__gauss(matrix, value, solutions, num_elements);
    
    free(value);
    free(matrix);
    
    Solutions* s = malloc(sizeof(Solutions));
    s->names = out_buffer;
    s->values = solutions;
    s->number = num_elements;
    
    return s;
}

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
    e->should_free_strings = 1;
    e->should_free_zero = 1;
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
        EP_append(e, temp_expression);
        
        
        // printf("%s => '%s' : '%s'\n", token, num, var);
        token = strtok(NULL, "+");
    }
    
    free(expression_copy);
    free(var);
    free(num);
    return e;
}



int main() {
    Expression** e_list = malloc(sizeof(Expression*) * 5);
    e_list[0] = EP_parse("xB + xA");
    e_list[1] = EP_parse("yB + yA");
    e_list[2] = EP_parse("-30000 + zB");
    e_list[3] = EP_parse("-27000000 + -1800yA");
    e_list[4] = EP_parse("1900000 + 1800xA");

    Solutions* s = ES_solve(e_list, 5);
    
    for(int i = 0; i < s->number; i++) {
        printf("%s = %lg\n", s->names[i], s->values[i]);
    }
    
    ES_free(s);
    
    for(int i = 0; i < 5; i++) {
        E_free(e_list[i]);
    }
    
    free(e_list);

    return 0;
}
