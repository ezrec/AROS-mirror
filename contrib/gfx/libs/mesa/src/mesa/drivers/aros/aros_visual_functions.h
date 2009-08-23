/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#if !defined(AROS_VISUAL_FUNCTIONS_H)
#define AROS_VISUAL_FUNCTIONS_H

#include <GL/gl.h>
#include "arosmesa_internal.h"

AROSMesaVisual aros_new_visual(GLboolean db_flag);
void _aros_destroy_visual(AROSMesaVisual aros_vis);

#endif /* AROS_VISUAL_FUNCTIONS_H */
