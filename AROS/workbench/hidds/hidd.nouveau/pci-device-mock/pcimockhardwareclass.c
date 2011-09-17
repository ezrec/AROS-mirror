/*
    Copyright 2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <aros/debug.h>
#include <proto/oop.h>

#include "pcimockhardware.h"
#include "pcimockhardware_intern.h"
#include "pcimock_intern.h"

OOP_Object * METHOD(PCIMockHardware, Root, New)
{
    o = (OOP_Object *)OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);

    return o;
}

VOID PCIMockHardware__Root__Dispose(OOP_Class *cl, OOP_Object *o, OOP_Msg msg)
{
}

VOID METHOD(PCIMockHardware, Root, Get)
{
}

