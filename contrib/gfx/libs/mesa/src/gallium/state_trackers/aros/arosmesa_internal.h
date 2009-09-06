/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#ifndef AROSMESA_INTERNAL_H
#define AROSMESA_INTERNAL_H

#include "main/mtypes.h"
#include "state_tracker/st_context.h"

#include <GL/arosmesa.h>

/* AROS visual */
struct arosmesa_visual
{
    GLvisual Base;                                  /* Base class - must be first */
};

typedef struct arosmesa_visual * AROSMesaVisual;

#define GET_GL_VIS_PTR(arosmesa_vis) (&arosmesa_vis->Base)
#define GET_AROS_VIS_PTR(gl_vis) ((AROSMesaVisual)gl_vis)

/* AROS context */
struct arosmesa_context
{
    struct st_context *     st;                     /* Base class - must be first */
    AROSMesaVisual          visual;                 /* the visual context */
};

#define GET_GL_CTX_PTR(arosmesa_ctx) (&arosmesa_ctx->st->Ctx)
#define GET_AROS_CTX_PTR(gl_ctx) ((AROSMesaContext)gl_ctx->DriverCtx)

#endif /* AROSMESA_INTERNAL_H */
