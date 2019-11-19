#ifndef EXPRESSION_SOLVER_H
#define EXPRESSION_SOLVER_H

#include "expression.h"

typedef struct solutions Solutions;
struct solutions {
    char** names;
    double* values;
    int number;
};

// Affichage du système
void ES_show_system(double **A, double *b, int n);
// Free solutions.
void ES_free(Solutions* s);
// Solve a system of linear equations.
Solutions* ES_solve(Expression** expressions, int num_expressions);

#endif
