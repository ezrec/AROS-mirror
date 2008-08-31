#if !defined(AROS_RB_FUNCTIONS_H)
#define AROS_RB_FUNCTIONS_H

#include <GL/gl.h>
#include "context.h"
#include "AROSMesa_intern.h"

GLboolean aros_renderbuffer_allocstorage(GLcontext *ctx, struct gl_renderbuffer *rb,
                            GLenum internalFormat, GLuint width, GLuint height);

void aros_renderbuffer_delete(struct gl_renderbuffer *rb);

AROSMesaRenderBuffer aros_new_renderbuffer(struct RastPort * rp, GLboolean ownsrastport);
void aros_delete_renderbuffer(AROSMesaRenderBuffer aros_rb);

/* GET */
void aros_renderbuffer_getrow(GLcontext* ctx, struct gl_renderbuffer *rb, GLuint count, GLint x, GLint y, void *values);

void aros_renderbuffer_getvalues(GLcontext *ctx, struct gl_renderbuffer *rb, GLuint count, const GLint x[], 
                const GLint y[], void * values);

/* PUT */
void aros_renderbuffer_putrow(GLcontext *ctx, struct gl_renderbuffer *rb, GLuint count, GLint x, GLint y,
                const void * values, const GLubyte mask[]);

void aros_renderbuffer_putvalues(GLcontext* ctx, struct gl_renderbuffer *rb, GLuint count, const GLint x[], 
                const GLint y[], const void * values, const GLubyte * mask);

void aros_renderbuffer_putrowrgb(GLcontext *ctx, struct gl_renderbuffer *rb, GLuint count, GLint x, GLint y,
                const void * values,  const GLubyte * mask);

void aros_renderbuffer_putmonorow(GLcontext *ctx, struct gl_renderbuffer *rb, GLuint count, GLint x, GLint y,
                const void *value, const GLubyte *mask);

void aros_renderbuffer_putmonovalues(GLcontext* ctx, struct gl_renderbuffer *rb, GLuint count, const GLint x[], 
                const GLint y[], const void *value, const GLubyte *mask);


#endif /* AROS_RB_FUNCTIONS_H */
