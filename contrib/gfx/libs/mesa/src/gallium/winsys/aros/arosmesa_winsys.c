/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/


#include "arosmesa_winsys.h"

struct arosmesa_driver driver;

void arosmesa_set_driver (const struct arosmesa_driver * drv)
{
   driver = *drv;
}