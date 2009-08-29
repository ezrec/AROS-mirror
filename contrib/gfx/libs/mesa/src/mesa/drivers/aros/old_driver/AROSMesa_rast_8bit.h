#if !defined(AROSMESA_RAST_8BIT_H)
#define AROSMESA_RAST_8BIT_H

#include "../glheader.h"
#include "../context.h"
#include "../colormac.h"
#include "../depth.h"
#include "../extensions.h"
#include "../macros.h"
#include "../matrix.h"
#include "../mem.h"
#include "../mmath.h"
#include "../mtypes.h"
#include "../texformat.h"
#include "../texstore.h"
#include "../array_cache/acache.h"
#include "../swrast/swrast.h"
#include "../swrast_setup/swrast_setup.h"
#include "../swrast/s_context.h"
#include "../swrast/s_depth.h"
#include "../swrast/s_lines.h"
#include "../swrast/s_triangle.h"
#include "../swrast/s_trispan.h"
#include "../tnl/tnl.h"
#include "../tnl/t_context.h"
#include "../tnl/t_pipeline.h"

/* Private Includes and protos for 8bit rasterizer */

extern void aros8bit_update_state( GLcontext *ctx );
extern BOOL aros8bit_Standard_init(struct arosmesa_context *c,struct TagItem *tagList);
extern BOOL aros8bit_Standard_init_db(struct arosmesa_context *c,struct TagItem *tagList);

extern void aros8bit_Standard_SwapBuffer(struct arosmesa_context *amesa);
extern void aros8bit_Standard_Dispose(struct arosmesa_context *c);
extern void aros8bit_Standard_Dispose_db(struct arosmesa_context *c);

#endif /* AROSMESA_RAST_8BIT_H */
