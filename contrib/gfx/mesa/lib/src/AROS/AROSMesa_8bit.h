/* Private Includes and protos for 8bit driver */

extern BOOL aros8bit_Standard_init(struct arosmesa_context *c,struct TagItem *tagList);
extern BOOL aros8bit_Standard_init_db(struct arosmesa_context *c,struct TagItem *tagList);

/* private functions (CyberGfx usses them) */

struct RastPort *make_rastport( int width, int height, int depth, struct BitMap *friendbm );
void destroy_rastport( struct RastPort *rp );
BOOL make_temp_raster( struct RastPort *rp );
void destroy_temp_raster( struct RastPort *rp );
void AllocOneLine(struct arosmesa_context *AROSMesaCreateContext);
void FreeOneLine(struct arosmesa_context *AROSMesaCreateContext);

BOOL alloc_temp_rastport(struct arosmesa_context * c);
void free_temp_rastport(struct arosmesa_context * c);

void aros8bit_Standard_SwapBuffer(struct arosmesa_context *amesa);
void aros8bit_Standard_Dispose(struct arosmesa_context *c);
void aros8bit_Standard_Dispose_db(struct arosmesa_context *c);
