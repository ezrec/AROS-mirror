#if !defined(AROSMESA_RAST_TC_H)
#define AROSMESA_RAST_TC_H

#include "../src/context.h"
#include "../src/types.h"

/* Private Includes and protos for TrueColor Rasterizer */

extern void arosTC_update_state( GLcontext *ctx );

extern BOOL arosTC_Standard_init(struct arosmesa_context *c,struct TagItem *tagList);
extern BOOL arosTC_Standard_init_db(struct arosmesa_context *c,struct TagItem *tagList);

#endif /* AROSMESA_RAST_TC_H */
