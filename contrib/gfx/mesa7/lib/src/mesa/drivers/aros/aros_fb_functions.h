#if !defined(AROS_FB_FUNCTIONS_H)
#define AROS_FB_FUNCTIONS_H

#include <GL/gl.h>
#include "context.h"
#include "AROSMesa_intern.h"

AROSMesaFrameBuffer aros_new_framebuffer(GLvisual * visual);
void aros_delete_framebuffer(AROSMesaFrameBuffer aros_fb);

#endif /* AROS_FB_FUNCTIONS_H */
