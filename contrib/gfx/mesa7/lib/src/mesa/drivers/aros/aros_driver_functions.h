#if !defined(AROS_DRIVER_FUNCTIONS_H)
#define AROS_DRIVER_FUNCTIONS_H

#include <GL/gl.h>
#include "context.h"

const GLubyte * aros_get_string(GLcontext *ctx, GLenum name);
void aros_clear_color(GLcontext *ctx, const GLfloat color[4]);
void aros_clear(GLcontext* ctx, GLbitfield mask);
void aros_update_state(GLcontext *ctx, GLbitfield new_state);
void _aros_init_driver_functions(struct dd_function_table * functions);

#endif /* AROS_DRIVER_FUNCTIONS_H */
