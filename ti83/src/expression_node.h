#ifndef EXPRESSION_NODE_H
#define EXPRESSION_NODE_H

#include <stddef.h>

typedef struct expressionNode ExpressionNode;
struct expressionNode {
    ExpressionNode* next;
    double num;
    char* var;
};

// Get display of individual expression node.
int EN_get_individual_display(ExpressionNode* self, char* buffer, size_t buffer_len);

#endif
