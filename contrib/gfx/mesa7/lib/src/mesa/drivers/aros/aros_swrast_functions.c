#include "aros_swrast_functions.h"

#define DEBUG 0
#include <aros/debug.h>
#include "swrast/swrast.h"
#include "swrast/s_context.h"

#include "main/context.h"

/* Z-disabled */

#define NAME aros_simple_textured_triangle
#define INTERP_INT_TEX 1
#define S_SCALE twidth
#define T_SCALE theight

#define SETUP_CODE							\
   struct gl_renderbuffer *rb = GET_GL_RB_PTR(GET_AROS_CTX_PTR(ctx)->renderbuffer);\
   struct gl_texture_object *obj = ctx->Texture.Unit[0].Current2D;	\
   const GLint b = obj->BaseLevel;					\
   const GLfloat twidth = (GLfloat) obj->Image[0][b]->Width;		\
   const GLfloat theight = (GLfloat) obj->Image[0][b]->Height;		\
   const GLint twidth_log2 = obj->Image[0][b]->WidthLog2;		\
   const GLchan *texture = (const GLchan *) obj->Image[0][b]->Data;	\
   const GLint smask = obj->Image[0][b]->Width - 1;			\
   const GLint tmask = obj->Image[0][b]->Height - 1;			\
   if (!texture) {							\
      /* this shouldn't happen */					\
      return;								\
   }

#define RENDER_SPAN( span )						\
   GLuint i;								\
   ULONG * dp = (ULONG*)(GET_AROS_CTX_PTR(ctx)->renderbuffer->buffer); \
   span.intTex[0] -= FIXED_HALF; /* off-by-one error? */		\
   span.intTex[1] -= FIXED_HALF;					\
   dp += (CorrectY(span.y) * rb->Width) + span.x; \
   for (i = 0; i < span.end; i++, dp++) {					\
      GLint s = FixedToInt(span.intTex[0]) & smask;			\
      GLint t = FixedToInt(span.intTex[1]) & tmask;			\
      GLint pos = (t << twidth_log2) + s;				\
      pos = pos << 2;  /* multiply by 4 (RGBA) */			\
      *dp = TC_ARGB(texture[pos], texture[pos+1], texture[pos+2], 255); \
      span.intTex[0] += span.intTexStep[0];				\
      span.intTex[1] += span.intTexStep[1];				\
   }									\

#include "s_tritemp.h"

/* Z-enabled */

#define NAME aros_simple_z_textured_triangle
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define INTERP_INT_TEX 1
#define S_SCALE twidth
#define T_SCALE theight

#define SETUP_CODE							\
   struct gl_renderbuffer *rb = GET_GL_RB_PTR(GET_AROS_CTX_PTR(ctx)->renderbuffer);\
   struct gl_texture_object *obj = ctx->Texture.Unit[0].Current2D;	\
   const GLint b = obj->BaseLevel;					\
   const GLfloat twidth = (GLfloat) obj->Image[0][b]->Width;		\
   const GLfloat theight = (GLfloat) obj->Image[0][b]->Height;		\
   const GLint twidth_log2 = obj->Image[0][b]->WidthLog2;		\
   const GLchan *texture = (const GLchan *) obj->Image[0][b]->Data;	\
   const GLint smask = obj->Image[0][b]->Width - 1;			\
   const GLint tmask = obj->Image[0][b]->Height - 1;			\
   if (!texture) {							\
      /* this shouldn't happen */					\
      return;								\
   }

#define RENDER_SPAN( span )						\
   GLuint i;				    				\
   ULONG * dp = (ULONG*)(GET_AROS_CTX_PTR(ctx)->renderbuffer->buffer); \
   span.intTex[0] -= FIXED_HALF; /* off-by-one error? */		\
   span.intTex[1] -= FIXED_HALF;					\
   dp += (CorrectY(span.y) * rb->Width) + span.x; \
   for (i = 0; i < span.end; i++, dp++) {					\
      const GLuint z = FixedToDepth(span.z);				\
      if (z < zRow[i]) {						\
         GLint s = FixedToInt(span.intTex[0]) & smask;			\
         GLint t = FixedToInt(span.intTex[1]) & tmask;			\
         GLint pos = (t << twidth_log2) + s;				\
         pos = pos << 2;  /* multiply by 4 (RGBA)*/			\
         *dp = TC_ARGB(texture[pos], texture[pos+1], texture[pos+2], 255); \
         zRow[i] = z;							\
      }									\
      span.intTex[0] += span.intTexStep[0];				\
      span.intTex[1] += span.intTexStep[1];				\
      span.z += span.zStep;						\
   }									\

#include "s_tritemp.h"




static void aros_choose_triangle(GLcontext * ctx)
{
    SWcontext * swrast = SWRAST_CONTEXT(ctx);
//    if (swrast->_RasterMask == (DEPTH_BIT | TEXTURE_BIT))
   if (swrast->_RasterMask & DEPTH_BIT)
        swrast->Triangle = aros_simple_z_textured_triangle;
    else
        swrast->Triangle = aros_simple_textured_triangle;
}

void aros_swrast_initialize(GLcontext * ctx)
{
    SWRAST_CONTEXT(ctx)->choose_triangle = aros_choose_triangle;
}
