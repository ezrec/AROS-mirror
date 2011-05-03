/*
    Copyright © 2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <proto/exec.h>

#undef CopyMem
#define CopyMem CopyMemQuick

#include "copymem.c"
