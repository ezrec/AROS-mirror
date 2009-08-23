/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#if !defined(AROS_DRIVER_FUNCTIONS_H)
#define AROS_DRIVER_FUNCTIONS_H

#include <GL/gl.h>
#include "context.h"

void _aros_init_driver_functions(struct dd_function_table * functions);
void _aros_recalculate_width_height(GLcontext *ctx, GLint requestedwidth, GLint requestedheight);

#endif /* AROS_DRIVER_FUNCTIONS_H */
