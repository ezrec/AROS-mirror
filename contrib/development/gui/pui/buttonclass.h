struct buttondata
{
	Object *image;
	char	*text;
	char	*t2;
	ULONG	flags;
	WORD	textstyle;
	WORD	textpixellen;			/* alles */
	WORD	textpixellen2;			/* bis '_' */
	WORD	textpixellen3;			/* Zeichen nach '_' */
	BYTE	textalign;
	UBYTE	textlen;
	UBYTE textlen1;
	UBYTE textlen2;
};

