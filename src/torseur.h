#ifndef TORSEUR_H
#define TORSEUR_H

#include "expression_solver.h"

typedef struct vecteur Vecteur;
struct vecteur {
    Expression* x;
    Expression* y;
    Expression* z;
};

typedef struct torseur Torseur;
struct torseur {
    int free_strings;
    char* name;
    char* point;
    Vecteur p;
    Vecteur r;
    Vecteur m;
    Expression** _expressions;
};

// Move a torseur to 0;0;0
void T_move_to_origin(Torseur* t);
// Print a torseur
void T_print(Torseur* t);
// Solve a system of torseurs
Solutions* T_solve(Torseur** list, int number);
// Free a torseur
void T_free(Torseur* t);

#endif
