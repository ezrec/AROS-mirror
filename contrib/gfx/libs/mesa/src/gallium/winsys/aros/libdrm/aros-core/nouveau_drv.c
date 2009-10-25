/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "drmP.h"

static struct drm_driver driver;

void nouveau_exit(void)
{
    drm_exit(&driver);
}

int nouveau_init(void)
{
    return drm_init(&driver);
}
