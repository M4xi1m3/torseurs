#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "expression.h"
#include "expression_solver.h"
#include "expression_parser.h"
#include "torseur.h"

char* T_strcpy_malloc(char* buffer) {
    char* out = malloc(strlen(buffer) + 1);
    strcpy(out, buffer);
    return out;
}

Torseur** T_input(int* num) {
    *num = 0;
    char buffer[50];
    
    // printf("Number: ");
    scanf("%d", num);
    getchar();
    
    Torseur** tl = malloc(sizeof(Torseur*) * (*num));
    
    for(int i = 0; i < (*num); i++) {
        tl[i] = malloc(sizeof(Torseur));
        
        // printf("Nom: ");
        scanf("%[^\n]", buffer);
        getchar();
        tl[i]->name = T_strcpy_malloc(buffer);
        
        // printf("Position: \n");
        // printf("Point: ");
        scanf("%[^\n]", buffer);
        getchar();
        tl[i]->point = T_strcpy_malloc(buffer);
        
        // printf("x: ");
        scanf("%[^\n]", buffer);
        getchar();
        tl[i]->p.x = EP_parse(buffer);
        // printf("y: ");
        scanf("%[^\n]", buffer);
        getchar();
        tl[i]->p.y = EP_parse(buffer);
        // printf("z: ");
        scanf("%[^\n]", buffer);
        getchar();
        tl[i]->p.z = EP_parse(buffer);
        
        // printf("Composantes: \n");
        
        // printf("x: ");
        scanf("%[^\n]", buffer);
        getchar();
        tl[i]->r.x = EP_parse(buffer);
        // printf("y: ");
        scanf("%[^\n]", buffer);
        getchar();
        tl[i]->r.y = EP_parse(buffer);
        // printf("z: ");
        scanf("%[^\n]", buffer);
        getchar();
        tl[i]->r.z = EP_parse(buffer);
        
        // printf("l: ");
        scanf("%[^\n]", buffer);
        getchar();
        tl[i]->m.x = EP_parse(buffer);
        // printf("m: ");
        scanf("%[^\n]", buffer);
        getchar();
        tl[i]->m.y = EP_parse(buffer);
        // printf("n: ");
        scanf("%[^\n]", buffer);
        getchar();
        tl[i]->m.z = EP_parse(buffer);
    }
    
    return tl;
}

int main() {
    int num = 0;
    Torseur** tl = T_input(&num);
    
    // for(int i = 0; i < num; i++) {
    //     T_print(tl[i]);
    // }
    
    Solutions* s = T_solve(tl, num);
    
    ES_print(s);
    
    for(int i = 0; i < num; i++) {
        // T_print(tl[i]);
        T_free(tl[i]);
    }
    free(tl);
    ES_free(s);
    
    return 0;
}



