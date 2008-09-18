#include "aros_visual_functions.h"

#include <aros/debug.h>
#include "context.h"

AROSMesaVisual aros_new_visual(GLboolean db_flag)
{
    AROSMesaVisual aros_vis = NULL;
    GLvisual * vis = NULL;
    int  indexBits, redBits, greenBits, blueBits, alphaBits, depthBits, stencilBits, accumBits;

    D(bug("[AROSMESA] aros_new_visual\n"));

    /* Allocated memory for aros structure */
    aros_vis = AllocVec(sizeof(struct arosmesa_visual), MEMF_PUBLIC | MEMF_CLEAR);

    if (!aros_vis)
        return NULL;

    vis = GET_GL_FB_PTR(aros_vis);

    // Create core visual 
    depthBits = DEFAULT_SOFTWARE_DEPTH_BITS;
    stencilBits = 8;
    accumBits = 16;
    indexBits = 0;
    redBits = CHAN_BITS;
    greenBits = CHAN_BITS;
    blueBits = CHAN_BITS;
    alphaBits = CHAN_BITS;

    /* Initialize mesa structure */
    if(!_mesa_initialize_visual(vis,
                                GL_TRUE,        /* RGB */
                                GL_FALSE,       /* Double buffer - AROSMesa uses front buffer as back buffer */
                                GL_FALSE,       /* stereo */
                                redBits,
                                greenBits,
                                blueBits,
                                alphaBits,
                                indexBits,
                                depthBits,
                                stencilBits,
                                accumBits,
                                accumBits,
                                accumBits,
                                accumBits,
                                1))
    {
        aros_delete_visual(aros_vis);
        return NULL;
    }

    /* Initialize aros structure field */
    aros_vis->db_flag = db_flag;    

    return aros_vis;
}

void aros_delete_visual(AROSMesaVisual aros_vis)
{
    if (!aros_vis)
        return;

    FreeVec(aros_vis);
}
