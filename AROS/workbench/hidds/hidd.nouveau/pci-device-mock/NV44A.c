/*
    Copyright 2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <aros/debug.h>
#include <proto/oop.h>

#include "pcimockhardware.h"
#include "pcimockhardware_intern.h"
#include "pcimock_intern.h"

OOP_Object * METHOD(NV44AMockHardware, Root, New)
{
    o = (OOP_Object *)OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);

    return o;
}

