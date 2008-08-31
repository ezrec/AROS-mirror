#include "aros_fb_functions.h"
#include "aros_rb_functions.h"

#include <aros/debug.h>
#include "framebuffer.h"


AROSMesaFrameBuffer aros_new_framebuffer(GLvisual * visual)
{
    AROSMesaFrameBuffer aros_fb = NULL;
    struct gl_framebuffer * fb = NULL;

    D(bug("[AROSMESA] aros_new_framebuffer\n"));

    /* Allocated memory for aros structure */
    aros_fb = AllocVec(sizeof(struct arosmesa_framebuffer), MEMF_PUBLIC | MEMF_CLEAR);

    if (!aros_fb)
        return NULL;

    fb = GET_GL_FB_PTR(aros_fb);

    /* Initialize mesa structure */
    _mesa_initialize_framebuffer(fb, visual);

    return aros_fb;
}

void aros_delete_framebuffer(AROSMesaFrameBuffer aros_fb)
{
    if (!aros_fb)
        return;

    FreeVec(aros_fb);
}
