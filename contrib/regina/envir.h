struct envir
{
   streng *name ;
   int type ;
   int subtype ;
   struct envir *next, *prev ;
};

#if defined(_AMIGA) || defined(__AROS__)
struct envir *amiga_find_envir( const tsd_t *TSD, const streng * );
streng *AmigaSubCom( const tsd_t *TSD, const streng *command, struct envir *envir, int *rc );
#endif
