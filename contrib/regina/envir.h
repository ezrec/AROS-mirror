struct envir
{
   environment e ;
   int type ;
   struct envir *next, *prev ;
};

#if defined(_AMIGA) || defined(__AROS__)
struct envir *amiga_find_envir( const tsd_t *TSD, const streng * );
streng *AmigaSubCom( const tsd_t *TSD, const streng *command, struct envir *envir, int *rc );
#endif
