struct coolimagedata
{
	struct	BitMap *bm;
	APTR		chunkydata;
	APTR		mask;
	LONG		*coltable;
	WORD		width,height,depth,numcols;
	BOOL		pensalloced;
	BOOL		imageok;

	WORD		type;
};


