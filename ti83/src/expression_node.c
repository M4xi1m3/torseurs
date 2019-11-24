#include "expression_node.h"

#include <stdio.h>
#include <string.h>

/**
 * Get display of individual expression node.
 */
int EN_get_individual_display(ExpressionNode* self, char* buffer, size_t buffer_len) {
    if (strcmp(self->var, "") == 0) {
        // snprintf(buffer, buffer_len, "%G", self->num);
    } else {
        if (self->num == 1) {
            // snprintf(buffer, buffer_len, "%s", self->var);
        } else if (self->num == -1) {
            // snprintf(buffer, buffer_len, "-%s", self->var);
        } else {
            // snprintf(buffer, buffer_len, "%g%s", self->num, self->var);
        }
    }
    return 0;
}
