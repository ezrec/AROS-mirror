typedef union {
	double	rval;
	Symbol	*sym;
	Node	*node;
} YYSTYPE;
#define	NUMBER	258
#define	VAR	259
#define	CONST	260
#define	C_BLTIN	261
#define	R_BLTIN	262
#define	FUNCDEF	263
#define	UFUNC	264
#define	COMMAND	265
#define	UNDEF	266
#define	PARAMETER	267
#define	PRECISION	268
#define	REPEAT	269
#define	UMINUS	270


extern YYSTYPE yylval;
