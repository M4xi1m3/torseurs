#include <stdlib.h>
#include <stdio.h>


#include "torseur.h"

/**
 * Move a torseur to 0;0;0
 *
 *      Mo = Ma + OA ^ R
 *
 *      yOA yR
 *      zOA zR
 *      xOA xR
 *      yOA yR
 *
 *      x = yOA * zR - zOA * yR
 *      y = zOA * xR - xOA * zR
 *      z = xOA * yR - yOA * xR
 *
 *      xMb = xMa + yOA * zR - zOA * yR
 *      yMb = yMa + zOA * xR - xOA * zR
 *      zMb = zMa + xOA * yR - yOA * xR
 */
void T_move_to_origin(Torseur* t) {
    
    Expression *point_x, *point_y, *point_z;
    Expression *tor_x, *tor_y, *tor_z;
    
    // Calculation for:
    // xMb = xMa + yOA * zR - zOA * yR
    point_z = E_copy(t->p.z);   // zOA
    point_y = E_copy(t->p.y);   // yOA
    tor_z = E_copy(t->r.z);     // zR
    tor_y = E_copy(t->r.y);     // yR
    
    E_multiply(point_z, tor_y);             // zOA * yR
    E_multiply_scalar(point_z, -1.0);       // -1(zOA * yR)
    
    E_multiply(point_y, tor_z);             // yOA * zR
    E_add(point_y, point_z);                // (yOA * zR) + -1(zOA * yR)
    
    E_add(t->m.x, point_y);                 // xMb = xMa + (yOA * zR) + -1(zOA * yR)
    E_simplify(t->m.x);
    
    E_free(point_z);
    E_free(point_y);
    E_free(tor_z);
    E_free(tor_y);
    
    // Calculation for:
    // yMb = yMa + zOA * xR - xOA * zR
    point_x = E_copy(t->p.x);   // xOA
    point_z = E_copy(t->p.z);   // zOA
    tor_x = E_copy(t->r.x);     // xR
    tor_z = E_copy(t->r.z);     // zR
    
    E_multiply(point_x, tor_z);             // xOA * zR
    E_multiply_scalar(point_x, -1.0);       // -1(xOA * zR)
    
    E_multiply(point_z, tor_x);             // zOA * xR
    E_add(point_z, point_x);                // (zOA * xR) + -1(xOA * zR)
    
    E_add(t->m.y, point_z);                 // yMb = yMa + (zOA * xR) + -1(xOA * zR)
    E_simplify(t->m.y);
    
    E_free(point_x);
    E_free(point_z);
    E_free(tor_x);
    E_free(tor_z);
    
    // Calculation for:
    // zMb = zMa + xOA * yR - yOA * xR
    point_y = E_copy(t->p.y);   // yOA
    point_x = E_copy(t->p.x);   // xOA
    tor_y = E_copy(t->r.y);     // yR
    tor_x = E_copy(t->r.x);     // xR
    
    E_multiply(point_y, tor_x);             // yOA * xR
    E_multiply_scalar(point_y, -1.0);       // -1(yOA * xR)
    
    E_multiply(point_x, tor_y);             // xOA * yR
    E_add(point_x, point_y);                // (xOA * yR) + -1(yOA * xR)
    
    E_add(t->m.z, point_x);                 // yMb = yMa + (zOA * xR) + -1(xOA * zR)
    E_simplify(t->m.z);
    
    E_free(point_y);
    E_free(point_x);
    E_free(tor_y);
    E_free(tor_x);
    
    E_multiply_scalar(t->p.x, 0);
    E_simplify(t->p.x);
    E_multiply_scalar(t->p.y, 0);
    E_simplify(t->p.y);
    E_multiply_scalar(t->p.z, 0);
    E_simplify(t->p.z);
}

/**
 * Print a torseur
 */
void T_print(Torseur* t) {
    printf(" == Torseur: %s ==\n", t->name);
    printf("  Point %s:\n", t->point);
    char buffer[100];
    E_get_display(t->p.x, buffer, 50);
    printf("   x: %s\n", buffer);
    E_get_display(t->p.y, buffer, 50);
    printf("   y: %s\n", buffer);
    E_get_display(t->p.z, buffer, 50);
    printf("   z: %s\n", buffer);
    printf("  Résultante:\n");
    E_get_display(t->r.x, buffer, 50);
    printf("   x: %s\n", buffer);
    E_get_display(t->r.y, buffer, 50);
    printf("   y: %s\n", buffer);
    E_get_display(t->r.z, buffer, 50);
    printf("   z: %s\n", buffer);
    printf("  Moment:\n");
    E_get_display(t->m.x, buffer, 50);
    printf("   x: %s\n", buffer);
    E_get_display(t->m.y, buffer, 50);
    printf("   y: %s\n", buffer);
    E_get_display(t->m.z, buffer, 50);
    printf("   z: %s\n", buffer);
}

void T__fill_expressions(Torseur* t) {
    t->_expressions = malloc(6 * sizeof(Expression*));
    
    t->_expressions[0] = t->r.x;
    t->_expressions[1] = t->r.y;
    t->_expressions[2] = t->r.z;
    t->_expressions[3] = t->m.x;
    t->_expressions[4] = t->m.y;
    t->_expressions[5] = t->m.z;
}

/**
 * Solve a system of torseurs
 */
Solutions* T_solve(Torseur** list, int number) {
    Expression** e_list = malloc(sizeof(Expression*) * 6);
    
    // Malloc list of expressions.
    for(int i = 0; i < 6; i++) {
        e_list[i] = malloc(sizeof(Expression));
        e_list[i]->head = NULL;
        E_simplify(e_list[i]);
    }
    
    // Move everything to 0;0;0
    for(int i = 0; i < number; i++) {
        Torseur* t = list[i];
        T_move_to_origin(t);
        T__fill_expressions(t);
    }
    
    // Sum the expressions.
    for(int i = 0; i < 6; i++) {
        for(int j = 0; j < number; j++) {
            E_add(e_list[i], list[j]->_expressions[i]);
        }
    }
    
    // Symplify everything.
    for(int i = 0; i < 6; i++) {
        E_simplify(e_list[i]);
        // E_print(e_list[i]);
    }
    
    // Remove zero (important, else we get weird results.)
    int new_buff_size = 0;
    for(int i = 0; i < 6; i++) {
        if (!E_is_zero(e_list[i]))
            new_buff_size++;
    }
    Expression** e_final_list = malloc(sizeof(Expression*) * new_buff_size);
    int count = 0;
    for(int i = 0; i < 6; i++) {
        if (!E_is_zero(e_list[i]))
            e_final_list[count++] = e_list[i];
        // E_print(e_list[i]);
    }
    
    Solutions* s = ES_solve(e_final_list, new_buff_size);
    
    
    // Free the expression lists in the screws.
    for(int i = 0; i < number; i++) {
        free(list[i]->_expressions);
    }
    
    
    // Free everythins.
    for(int i = 0; i < 6; i++) {
        E_free(e_list[i]);
    }
    free(e_list);
    free(e_final_list);
    return s;
}

/**
 * Free a torseur
 */
void T_free(Torseur* t) {
    if (t->free_strings) {
        free(t->name);
        free(t->point);
    }
    
    // E_debug(t->p.x);
    E_free(t->p.x);
    E_free(t->p.y);
    E_free(t->p.z);
    E_free(t->r.x);
    E_free(t->r.y);
    E_free(t->r.z);
    E_free(t->m.x);
    E_free(t->m.y);
    E_free(t->m.z);
    
    free(t);
}

