/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#if !defined(AROS_DRIVER_FUNCTIONS_H)
#define AROS_DRIVER_FUNCTIONS_H

#include <GL/gl.h>
#include "context.h"

void _aros_init_driver_functions(struct dd_function_table * functions);
GLboolean _aros_recalculate_buffer_width_height(GLcontext *ctx);

#endif /* AROS_DRIVER_FUNCTIONS_H */
