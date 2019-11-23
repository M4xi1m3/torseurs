#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "expression_node.h"

typedef struct expression Expression;
struct expression {
    ExpressionNode* head;
};

// Append an ExpressionNode to an Expression.
void E_append(Expression* e, ExpressionNode* n);
// Copy an expression
Expression* E_copy(Expression* e);
// Get string representing whole expression.
int E_get_display(Expression* self, char* buffer, size_t buffer_len);
// Multiply the whole expression by a scalar.
int E_multiply_scalar(Expression* self, double scalar);
// Multiply two expression together (Warning: Error when trying to do var*var).
int E_multiply(Expression* self, Expression* other);
// Simplify and expression.
int E_simplify(Expression* self);
// Get the value of a specific variable.
double E_get_value_for(Expression* self, char* var);
// Get list of all variables.
int E_get_variables(Expression* self, char** var_names, int max_vars);
// Free everything.
int E_free(Expression* self);
// Add an expression to another.
int E_add(Expression* self, Expression* other);
// Printf the shit out of it!
void E_debug(Expression* self);
// Print the expression.
void E_print(Expression* self);
// Check if an expression is equal to zero
int E_is_zero(Expression* self);

#endif
