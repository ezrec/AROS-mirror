#if !defined(AROS_RB_FUNCTIONS_H)
#define AROS_RB_FUNCTIONS_H

#include <GL/gl.h>
#include "context.h"
#include "AROSMesa_intern.h"

GLboolean aros_renderbuffer_allocstorage(GLcontext *ctx, struct gl_renderbuffer *rb,
                            GLenum internalFormat, GLuint width, GLuint height);

void aros_renderbuffer_delete(struct gl_renderbuffer *rb);

AROSMesaRenderBuffer aros_new_renderbuffer(void);
void aros_delete_renderbuffer(AROSMesaRenderBuffer aros_rb);

#endif /* AROS_RB_FUNCTIONS_H */
