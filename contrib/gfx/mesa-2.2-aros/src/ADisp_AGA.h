extern BOOL Amiga_Standard_init(struct amigamesa_context *c,struct TagItem *tagList);
extern BOOL Amiga_Standard_init_db(struct amigamesa_context *c,struct TagItem *tagList);

/* private functions (CyberGfx usses them) */

struct RastPort *make_rastport( int width, int height, int depth, struct BitMap *friendbm );
void destroy_rastport( struct RastPort *rp );
BOOL make_temp_raster( struct RastPort *rp );
void destroy_temp_raster( struct RastPort *rp );
void AllocOneLine(struct amigamesa_context *AmigaMesaCreateContext);
void FreeOneLine(struct amigamesa_context *AmigaMesaCreateContext);

BOOL alloc_temp_rastport(struct amigamesa_context * c);
void free_temp_rastport(struct amigamesa_context * c);

void Amiga_Standard_SwapBuffer(struct amigamesa_context *amesa);
void Amiga_Standard_Dispose(struct amigamesa_context *c);
void Amiga_Standard_Dispose_db(struct amigamesa_context *c);
