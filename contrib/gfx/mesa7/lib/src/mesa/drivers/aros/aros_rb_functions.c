#include "aros_rb_functions.h"


#include <aros/debug.h>
#include <proto/cybergraphics.h>
#include <proto/graphics.h>
#include <cybergraphx/cybergraphics.h>

#include <GL/AROSMesa.h>
#include "swrast/swrast.h"
#include "renderbuffer.h"

#if (AROS_BIG_ENDIAN == 1)
#define AROS_PIXFMT RECFMT_ARGB    /* Big Endian Archs. */
#else
#define AROS_PIXFMT RECTFMT_BGRA32   /* Little Endian Archs. */
#endif

/* Private functions */
static void aros_renderbuffer_free_imageline(AROSMesaRenderBuffer aros_rb)
{
    if (aros_rb && aros_rb->imageline)
    {
        FreeVec(aros_rb->imageline);
        aros_rb->imageline = NULL;
    }
}

static void aros_renderbuffer_alloc_imageline(AROSMesaRenderBuffer aros_rb)
{
    if (aros_rb)
    {
        if (aros_rb->imageline)
            aros_renderbuffer_free_imageline(aros_rb);
        aros_rb->imageline = AllocVec((GET_GL_RB_PTR(aros_rb))->Width * 4, MEMF_ANY);
    }
}

/*
 * Deallocate a rastport.
 */

static void aros_renderbuffer_destroy_rastport(AROSMesaRenderBuffer aros_rb)
{
    if (!aros_rb)
        return;

    if (aros_rb->ownsrastport != GL_TRUE)
        return;
    
    WaitBlit();
    FreeBitMap(aros_rb->rp->BitMap);
    FreeRastPort(aros_rb->rp);
}


static void MyReadPixelArray(UBYTE *a, ULONG b, ULONG c, ULONG d, struct RastPort *e,
                ULONG f, ULONG g,ULONG h, ULONG i,ULONG j)
{
    ReadPixelArray(a,b,c,d,e,f,g,h,i,j);
}

/* Read a horizontal span of color pixels. */
/* GetRow */
void aros_renderbuffer_getrow(GLcontext* ctx, struct gl_renderbuffer *rb,
                          GLuint count, GLint x, GLint y, void * values)
{
    AROSMesaContext amesa = GET_AROS_CTX_PTR(ctx);
    AROSMesaRenderBuffer aros_rb = GET_AROS_RB_PTR(rb);
    int i;
    ULONG col;
    struct RastPort *rp = aros_rb->rp;
    GLubyte * rgba = (GLubyte*)values;
    GLubyte * p_set;

    D(bug("[AROSMESA] aros_renderbuffer_getrow\n"));

    y = FIXy(y);
    x = FIXx(x);
    
    if(aros_rb->imageline)
    {
        MyReadPixelArray(aros_rb->imageline, 0, 0, count * 4, rp, x, y, count, 1, AROS_PIXFMT);
        for(i = 0; i<count; i++)
        {
            p_set = rgba + i * 4;
            col = ((ULONG*)aros_rb->imageline)[i];
            *(p_set + RCOMP) = (col & 0xff0000) >> 16;
            *(p_set + GCOMP) = (col & 0xff00) >> 8;
            *(p_set + BCOMP) = col & 0xff;
            *(p_set + ACOMP) = 255;
        }
    } 
    else
    {
        for (i=0; i<count; i++, x++)
        {
            col = ReadRGBPixel(rp, x, y);
            p_set = rgba + i * 4;
            *(p_set + RCOMP) = (col & 0xff0000) >> 16;
            *(p_set + GCOMP) = (col & 0xff00) >> 8;
            *(p_set + BCOMP) = col & 0xff;
            *(p_set + ACOMP) = 255;
        }
    }
}

/* Read an array of color pixels. */
/* GetValues */
void aros_renderbuffer_getvalues( GLcontext *ctx, struct gl_renderbuffer *rb, GLuint count, const GLint x[], 
                    const GLint y[], void * values)
{
    AROSMesaContext amesa = GET_AROS_CTX_PTR(ctx);
    AROSMesaRenderBuffer aros_rb = GET_AROS_RB_PTR(rb);
    int i, col;
    struct RastPort *rp = aros_rb->rp;
    GLubyte * rgba = (GLubyte*)values;
    GLubyte * p_set;

    D(bug("[AROSMESA] aros_renderbuffer_getvalues\n"));

    for (i = 0; i < count; i++)
    {
            col = ReadRGBPixel(rp, FIXx(x[i]), FIXy(y[i]));
            p_set = rgba + i * 4;

            *(p_set + RCOMP) = (col & 0xff0000) >> 16;
            *(p_set + GCOMP) = (col & 0xff00) >> 8;
            *(p_set + BCOMP) = col & 0xff;
            *(p_set + ACOMP) = 255;
    }
}

/* Write a horizontal span of RGBA color pixels with a boolean mask. */
/* PutRow */
void aros_renderbuffer_putrow(GLcontext *ctx, struct gl_renderbuffer *rb, GLuint count, GLint x, GLint y,
                const void * values, const GLubyte mask[])
{
    AROSMesaContext amesa = GET_AROS_CTX_PTR(ctx);
    AROSMesaRenderBuffer aros_rb = GET_AROS_RB_PTR(rb);
    int i, drawbits_counter;
    ULONG *dp = NULL;
    struct RastPort *rp = aros_rb->rp;
    GLubyte * rgba = (GLubyte*)values;
    GLubyte * p_get;

    D(bug("[AROSMESA] aros_renderbuffer_putrow(count=%d,x=%d,y=%d)", n, x, y));

    y = FIXy(y);
    x = FIXx(x);


    if((dp = (ULONG*)aros_rb->imageline))
    {       
        /* if imageline allocated then use fastversion */
        D(bug("FAST "));
        if (mask)
        {
            D(bug("mask\n"));
            drawbits_counter = 0;
            
            for (i = 0; i < count; i++)       /* draw pixel (x[i],y[i]) */
            {
                p_get = rgba + i * 4;
        
                if (mask[i])
                {
                    /* Mask Bit Set, So Draw */
                    drawbits_counter++;
                    *dp = TC_ARGB(*(p_get + RCOMP), *(p_get + GCOMP), *(p_get + BCOMP), *(p_get + ACOMP));
                    dp++;
                }
                else
                {
                    if(drawbits_counter)
                    {
                        WritePixelArray(aros_rb->imageline, 0, 0, 4 * drawbits_counter, 
                            rp, x, y, drawbits_counter, 1, AROS_PIXFMT);
                        dp = (ULONG*)aros_rb->imageline;
                        x = x + drawbits_counter;
                        drawbits_counter = 0;
                    }
                    else
                    {
                        x++;
                    }
               }
            }

            if(drawbits_counter)
            {
                WritePixelArray(aros_rb->imageline, 0, 0, 4 * drawbits_counter, rp, x, y, drawbits_counter, 1, AROS_PIXFMT);
            }
        }
        else
        {
            D(bug("nomask\n"));
            for (i = 0; i < count; i++)           /* draw pixel (x[i],y[i])*/
            {
                p_get = rgba + i * 4;
                *dp = TC_ARGB(*(p_get + RCOMP), *(p_get + GCOMP), *(p_get + BCOMP), *(p_get + ACOMP));
                dp++;
            }
            WritePixelArray(aros_rb->imageline, 0, 0, 4 * count, rp, x, y, count, 1, AROS_PIXFMT);
        }
    }
    else
    {  /* Slower version */
        D(bug("SLOW "));
        if (mask)
        {
            D(bug("mask\n"));
            /* draw some pixels */
            for (i = 0; i < count; i++, x++)
            {
                p_get = rgba + i * 4;
                if (mask[i])
                {
                    /* draw pixel x,y using color red[i]/green[i]/blue[i]/alpha[i] */
                    WriteRGBPixel(rp, x, y, TC_ARGB(*(p_get + RCOMP), *(p_get + GCOMP), *(p_get + BCOMP), 
                                                                                                *(p_get + ACOMP)));
                }
            }
        }
        else
        {
            D(bug("nomask\n"));
            /* draw all pixels */
            for (i = 0; i < count; i++, x++)
            {
                p_get = rgba + i * 4;
                /* draw pixel x,y using color red[i]/green[i]/blue[i]/alpha[i] */
                WriteRGBPixel(rp, x, y,TC_ARGB(*(p_get + RCOMP), *(p_get + GCOMP), *(p_get + BCOMP), 
                                                                                                *(p_get + ACOMP)));
            }
        }
    }
}


/* Write an array of RGBA pixels with a boolean mask. */
/* PutValues */
void aros_renderbuffer_putvalues(GLcontext* ctx, struct gl_renderbuffer *rb, GLuint count, const GLint x[], 
                const GLint y[], const void * values, const GLubyte * mask)

{
    AROSMesaContext amesa = GET_AROS_CTX_PTR(ctx);
    AROSMesaRenderBuffer aros_rb = GET_AROS_RB_PTR(rb);    
    int i;
    struct RastPort *rp = aros_rb->rp;
    GLubyte * rgba = (GLubyte*)values;
    GLubyte * p_get;

    D(bug("[AROSMESA] aros_renderbuffer_putvalues\n"));

    for (i = 0; i < count; i++)
    {
        p_get = rgba + i * 4;

        if (mask[i])
        {
            /* write pixel x[i], y[i] using red[i],green[i],blue[i],alpha[i] */
            WriteRGBPixel(rp, FIXx(x[i]), FIXy(y[i]), 
                TC_ARGB(*(p_get + RCOMP), *(p_get + GCOMP), *(p_get + BCOMP), *(p_get + ACOMP)));
        }
    }
}

void aros_renderbuffer_putrowrgb( GLcontext *ctx, struct gl_renderbuffer *rb, GLuint count, GLint x, GLint y,
                                 const void * values,  const GLubyte * mask)
{
    D(bug("[AROSMESA] aros_renderbuffer_putrowrgb\n"));
}

/*
* Write a horizontal span of pixels with a boolean mask.  The current color
* is used for all pixels.
*/
void aros_renderbuffer_putmonorow( GLcontext *ctx, struct gl_renderbuffer *rb, GLuint count, GLint x, GLint y,
                                 const void *value, const GLubyte *mask)
{
    D(bug("[AROSMESA] aros_renderbuffer_putmonorow\n"));
}

void aros_renderbuffer_putmonovalues(GLcontext* ctx, struct gl_renderbuffer *rb, GLuint count, const GLint x[], 
                const GLint y[], const void *value, const GLubyte *mask)
{
    D(bug("[AROSMESA] aros_renderbuffer_putmonovalues\n"));
}

GLboolean aros_renderbuffer_allocstorage(GLcontext *ctx, struct gl_renderbuffer *rb,
                            GLenum internalFormat, GLuint width, GLuint height)
{
    D(bug("[AROSMESA] aros_renderbuffer_storage\n"));
    
    rb->PutRow = aros_renderbuffer_putrow;
    rb->PutRowRGB = aros_renderbuffer_putrowrgb;
    rb->PutMonoRow = aros_renderbuffer_putmonorow;
    rb->PutValues = aros_renderbuffer_putvalues;
    rb->PutMonoValues = aros_renderbuffer_putmonovalues;
    rb->GetRow = aros_renderbuffer_getrow;
    rb->GetValues = aros_renderbuffer_getvalues;
    rb->RedBits = rb->DataType;
    rb->GreenBits = rb->DataType;
    rb->BlueBits = rb->DataType;
    rb->AlphaBits = rb->DataType;
    rb->Width = width;
    rb->Height = height;

    aros_renderbuffer_alloc_imageline(GET_AROS_RB_PTR(rb));

    return GL_TRUE;
}

void aros_renderbuffer_delete(struct gl_renderbuffer *rb)
{
    D(bug("[AROSMESA] aros_renderbuffer_delete\n"));
    /* No op */
}

AROSMesaRenderBuffer aros_new_renderbuffer(struct RastPort * rp, GLboolean ownsrastport)
{
    AROSMesaRenderBuffer aros_rb = NULL;
    struct gl_renderbuffer * rb = NULL;

    D(bug("[AROSMESA] aros_new_renderbuffer\n"));

    /* Allocated memory for aros structure */
    aros_rb = AllocVec(sizeof(struct arosmesa_renderbuffer), MEMF_PUBLIC | MEMF_CLEAR);
    rb = GET_GL_RB_PTR(aros_rb);

    /* Initialize mesa structure */
    _mesa_init_renderbuffer(rb, 0);

    if (rb)
    {
        rb->InternalFormat = GL_RGBA;
        rb->_BaseFormat = GL_RGBA;
        rb->_ActualFormat = GL_RGBA;
        rb->DataType = GL_UNSIGNED_BYTE;
        rb->AllocStorage = aros_renderbuffer_allocstorage;
        rb->Delete = aros_renderbuffer_delete;
        /* This will trigger AllocStorage */
        rb->Width = 0;
        rb->Height = 0;
    }
    
    /* Initialize aros structure */
    aros_rb->rp = rp;
    aros_rb->ownsrastport = ownsrastport;
    aros_rb->imageline = NULL;

    return aros_rb;
}

void aros_delete_renderbuffer(AROSMesaRenderBuffer aros_rb)
{
    if (aros_rb)
    {
        aros_renderbuffer_free_imageline(aros_rb);
 
        if (aros_rb->ownsrastport == GL_TRUE)
            aros_renderbuffer_destroy_rastport(aros_rb);

        FreeVec(aros_rb);
    }
}
