#ifndef BISON_COMPLEX_TAB_H
# define BISON_COMPLEX_TAB_H

#ifndef YYSTYPE
typedef union {
	double	rval;
	Symbol	*sym;
	Node	*node;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
# define	NUMBER	257
# define	VAR	258
# define	CONST	259
# define	C_BLTIN	260
# define	R_BLTIN	261
# define	FUNCDEF	262
# define	UFUNC	263
# define	COMMAND	264
# define	UNDEF	265
# define	PARAMETER	266
# define	PRECISION	267
# define	REPEAT	268
# define	UMINUS	269


extern YYSTYPE yylval;

#endif /* not BISON_COMPLEX_TAB_H */
