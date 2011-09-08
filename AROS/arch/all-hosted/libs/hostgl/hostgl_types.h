/*
    Copyright 2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#ifndef HOSTGL_TYPES_H
#define HOSTGL_TYPES_H

#include <GL/arosmesa.h>

#include "x11_hostlib.h"

/* AROSMesa context */
struct arosmesa_context
{
    Display *display;
    Window window;
};

#endif /* HOSTGL_INTERNAL_H */
