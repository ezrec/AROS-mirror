/*
    Copyright © 2012-2013, The AROS Development Team. All rights reserved.
    $Id$

    This file defines the private part of StdCBase.
    This should only be used internally in stdc.library code so
    changes can be made to this structure without breaking backwards
    compatibility.
*/
#include <libraries/stdc.h>

struct StdCIntBase
{
    struct StdCBase StdCBase;

    /* stdlib.h */
    APTR mempool;
    unsigned int srand_seed;
};

