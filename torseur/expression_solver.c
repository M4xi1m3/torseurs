#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "expression_solver.h"

/**
 * Affichage du système
 */
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

/**
 * Malloc a matrix for ES__gauss
 */
double** ES__malloc_matrix(int rows, int cols) {
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

/**
 * Remove duplicates in a char**
 */
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
        } else {
            free(buffer[i]);
        }
    }
    return num_elements;
}

/**
 * Free solutions.
 */
void ES_free(Solutions* s) {
    free(s->values);
    
    for(int i = 0; i < s->number; i++) {
        free(s->names[i]);
    }
    
    free(s->names);
    free(s);
}

/**
 * Print solutions
 */
void ES_print(Solutions* s) {
    for(int i = 0; i < s->number; i++) {
        printf("%s = %lg\n", s->names[i], s->values[i]);
    }
}

/**
 * Solve a system of linear equations.
 */
Solutions* ES_solve(Expression** expressions, int num_expressions) {
    char** buffer = malloc(40 * sizeof(char*));
    char** out_buffer = malloc(40 * sizeof(char*));
    int num_vars = 0;
    
    for(int i = 0; i < num_expressions; i++) {
        E_simplify(expressions[i]);
    }
    
    for(int i = 0; i < num_expressions; i++) {
        num_vars += E_get_variables(expressions[i], buffer + num_vars, 40 - num_vars);
    }
    
    int num_elements = ES__remove_duplicates(buffer, num_vars, out_buffer);
    free(buffer);
    
    double** matrix = ES__malloc_matrix(num_elements, num_elements);
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

