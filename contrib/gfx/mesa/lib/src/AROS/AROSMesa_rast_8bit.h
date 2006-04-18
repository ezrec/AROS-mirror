#if !defined(AROSMESA_RAST_8BIT_H)
#define AROSMESA_RAST_8BIT_H

#include "../src/context.h"
#include "../src/types.h"

/* Private Includes and protos for 8bit rasterizer */

extern void aros8bit_update_state( GLcontext *ctx );
extern BOOL aros8bit_Standard_init(struct arosmesa_context *c,struct TagItem *tagList);
extern BOOL aros8bit_Standard_init_db(struct arosmesa_context *c,struct TagItem *tagList);

extern void aros8bit_Standard_SwapBuffer(struct arosmesa_context *amesa);
extern void aros8bit_Standard_Dispose(struct arosmesa_context *c);
extern void aros8bit_Standard_Dispose_db(struct arosmesa_context *c);

#endif /* AROSMESA_RAST_8BIT_H */
