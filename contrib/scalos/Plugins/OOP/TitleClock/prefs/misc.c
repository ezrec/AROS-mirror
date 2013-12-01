/*
 *  misc.c - routines for misc stuff
 *
 * $Date$
 * $Revision$
 *
 *  0.1
 *  20010718 DM Created. Added routines for calculating length of a string in a given font (ignoring underscores) and the longest in an array of values
 *
 */

#include <exec/types.h>
#include <proto/graphics.h>
#include <proto/diskfont.h>

#include <exec/libraries.h>
#include <graphics/rastport.h>
#include <graphics/text.h>

#include <string.h>
#include <stdio.h>
#include <stdarg.h>


WORD USTextLength(STRPTR string, struct TextFont *tf)
{
	char            temp[256];      /* Some temporary gubbins */
	STRPTR          t2;             /* Working pointer to the above temporary string */
	struct RastPort rp;             /* We need this for calculating the length */
	ULONG           textlen = 0;    /* Length of the text */

	for(t2 = temp; *string; string++)
	{
		if(*string != '_')
		{
			*t2++ = *string;
			textlen++;
		}
	}
	*t2 = '\0';

	//printf("Non-US: %s\n", temp);

	InitRastPort(&rp);
	SetFont(&rp, tf);
	return(TextLength(&rp, temp, textlen));
}


WORD MaxLen(int count, ...)
{
	va_list vl;
	WORD    len, max = 0;
	int     i;

	va_start(vl, count);

	for(i = 0; i < count; i++)
	{
		len = va_arg(vl, int);
		//printf("next arg: %d\n", len);
		if(len > max) max = len;
	}

	va_end(vl);
	return(max);
}


#if 0
struct GfxBase *GfxBase;
struct Library *DiskfontBase;

int main(void)
{
	struct TextAttr ta = {"xen.font", 13, 0, 0};
	struct TextFont *tf;
	GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 36);
	DiskfontBase = (struct Library *)OpenLibrary("diskfont.library", 0);

	if(GfxBase && DiskfontBase)
	{
		tf = OpenDiskFont(&ta);
		if(tf)
		{
			printf("Pixel length: %d\n", USTextLength("_Save", tf));
			CloseFont(tf);
		}
	}

	if(DiskfontBase) CloseLibrary(DiskfontBase);
	if(GfxBase) CloseLibrary((struct Library *)GfxBase);

	printf("max is: %d\n", MaxLen(4, 9, 6, 3, 8));

	return(0);
}
#endif

