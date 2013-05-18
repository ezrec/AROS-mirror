/*
    Copyright © 2012-2013, The AROS Development Team. All rights reserved.
    $Id$

    This file defines the private part of PosixCBase.
    This should only be used internally in posixc.library code so
    changes can be made to this structure without breaking backwards
    compatibility.
*/
#include <libraries/posixc.h>

struct random_state;
struct __env_item;

struct PosixCIntBase
{
    struct PosixCBase PosixCBase;

    /* random.c */
    struct random_state *rs;

    /* __posixc_environ.c; don't use this field outside that file */
    char ***environptr;

    /* __env.c */
    struct __env_item *env_list;
};

