
extern APTR AllocCopyStringPPC( char *string );
extern void FreeStringPPC( APTR string );

extern ULONG getPPC(Object *obj, ULONG id);

extern APTR AllocNodePPC(struct MinList *list, ULONG size);
extern void FreeNodePPC(APTR node);
extern void FreeAllNodesPPC(struct MinList *list);
