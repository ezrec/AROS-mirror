
/* utils.c */
#ifdef __MORPHOS__
APTR DoSuperNew(struct IClass *cl,Object *obj,...);
#elif defined(__AROS__)
Object *DoSuperNew(struct IClass *cl, Object *obj, IPTR tag1, ...);
#else
Object * VARARGS68K DoSuperNew(struct IClass *cl, Object *obj, ...);
#endif
APTR allocVecPooled(APTR pool, ULONG size);
void freeVecPooled (APTR pool, APTR mem);
APTR reallocVecPooledNC(APTR pool,APTR mem,ULONG size);

/* brc1.c */
int BRCUnpack(APTR pSource, APTR pDest, LONG srcBytes0, LONG dstBytes0);

/* spacer.c */
BOOL initSpacerClass(void);

/* dragbar.c */
BOOL initDragBarClass(void);


