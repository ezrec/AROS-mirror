/*
 * Based on the code from the ttf.library by Richard Griffith.
 */
#include "ttfglyphengine.h"
#include "ttfdebug.h"

#include <proto/exec.h>
#include <proto/dos.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <diskfont/oterrors.h>

void set_default_codepage(TTF_GlyphEngine *ge)
{
    int i;
    
    for(i=0;i<256;++i) ge->codepage[i]=i;

    GetVar("ttfcodepage", (STRPTR)&ge->codepage, 512, 
	   LV_VAR | GVF_BINARY_VAR | GVF_DONT_NULL_TERM);
}

/* close down and dispose of GlyphEngine */
void FreeGE(TTF_GlyphEngine *ge)
{
    if(ge==NULL) return;
	
/*	ClearAllGlyphs(ge); */
	
    if(ge->face_established)
	TT_Close_Face( ge->face );
	
    TT_Done_FreeType( ge->engine );
    
    FreeVec(ge);
}

/* initialize and setup the GlyphEngine structure */
TTF_GlyphEngine *AllocGE(void)
{
    TTF_GlyphEngine *ge;
    TT_Error  error;
    
    ge=AllocVec(sizeof(TTF_GlyphEngine),MEMF_PUBLIC | MEMF_CLEAR);
    if(ge!=NULL) {
	set_last_error(ge,OTERR_Success);
	/*		ge->GlyphPool = NULL; */

	if (error = TT_Init_FreeType( &ge->engine ) )
	{
	    D(bug("Error initializing engine, code = %ld.\n",
		  (LONG) error ));
	    FreeVec(ge);
	    ge=NULL;
	}
	else {
	    /* pre-inits */
	    if (error=TT_Init_Kerning_Extension(ge->engine))
	    {
		D(bug("Error initializing kerning, "
		      "code = %ld.\n", (LONG) error ));
	    }
	    ge->KernPairs = -1;
	    ge->cmap_index=NO_CMAP_SET;
	    set_default_codepage(ge);
	}

    }

    return(ge);
}	

int set_last_error(TTF_GlyphEngine *ge,int error)
{
    ge->last_error=error;
    return error;
}

/* choose a unicode mapping */
int ChooseEncoding(TTF_GlyphEngine *ge)
{
    int i, n;

    n = (&ge->properties)->num_CharMaps;
//    D(bug( "ChooseEncoding - there are %ld maps\n",n ));

    /* if specified in otag file, use it */
//    D(bug( "ChooseEncoding - otag specified map %lx\n",ge->requested_cmap));

    ge->cmap_index=ge->requested_cmap;

    /* raw encoding does not need CharMap loaded */
    if((ge->cmap_index > 0x10000L)&&(ge->cmap_index < 0x20000L))
	return set_last_error(ge,OTERR_Success);

    if(n==0)
	return set_last_error(ge,OTERR_BadFace); /* no maps */

    /* valid map specified, use it */
    if((ge->cmap_index)<=n)
    {
	i=ge->cmap_index;	
	TT_Get_CharMap_ID(ge->face, i, &ge->platform, &ge->encoding);
	TT_Get_CharMap( ge->face, i, &ge->char_map );
//		D(bug( "ChooseEncoding - otag picked i=%ld p=%ld e=%ld\n",
//			ge->cmap_index, ge->platform, ge->encoding ));
	return set_last_error(ge,OTERR_Success);
    }

    /* invalid, or not specified, look for a Unicode charmap */
    for ( i = 0; i < n; i++ )
    {
	TT_Get_CharMap_ID(ge->face, i, &ge->platform, &ge->encoding);
//		D(bug( "ChooseEncoding - checking i=%ld p=%ld e=%ld\n",
//			i, ge->platform, ge->encoding ));
	if ((ge->platform == 3 && ge->encoding == 1)  ||
	    (ge->platform == 0 && ge->encoding == 0))
	{
	    TT_Get_CharMap( ge->face, i, &ge->char_map );
	    ge->cmap_index=i;
	    i = n + 1;
	}
    }

    if ( i == n )
    {
	D(bug( "No Unicode mapping table, using first\n" ));
	/* just use the first one, whatever it is */
	TT_Get_CharMap_ID(ge->face, 0, &ge->platform, &ge->encoding);
	TT_Get_CharMap( ge->face, 0, &ge->char_map );
	ge->cmap_index=0;
    }
		
//    D(bug("Platform=%ld Encoding=%ld\n",
//          (LONG)ge->platform,(LONG)ge->encoding));
    return(set_last_error(ge,OTERR_Success));
}


int SetInstance(TTF_GlyphEngine *ge)
{
    TT_Error error;
    long yMin, yMax, alt_point,a,d;

    /* set instance characteristics */

    error = TT_Set_Instance_Resolutions( ge->instance,
					 ge->xres, ge->yres );

    if (error)
    {
	D(bug("Could not set resolution.\n"));
	return set_last_error(ge,OTERR_Failure);
    }


/* adjust point size according to chosen metric source
   each has pros and cons, so user gets to decide
 */

    if (ge->metric_source==1)
    {
	/* (nearly) raw em */
	alt_point = ge->point_size - 1;
	ge->corrected_upem = ge->properties.header->Units_Per_EM;
    }
    else
    {
	switch(ge->metric_source)
	{
	case 2: /* Horizontal Header */
	    yMax = ge->properties.horizontal->Ascender;
	    yMin = ge->properties.horizontal->Descender;
	    break;
	
	case 3: /* Typo */
	    yMax = ge->properties.os2->sTypoAscender;
	    yMin = ge->properties.os2->sTypoDescender;
	    break;

	case 4: /* usWin */
	    yMax = ge->properties.os2->usWinAscent;
	    yMin = ge->properties.os2->usWinDescent;
	    break;

	case 5:	/* custom bbox */
	    yMin=ge->metric_custom & 0xFFFF;
	    yMax=ge->metric_custom >> 16;
	    break;
	
	default:
	case 0:	/* global bbox */
	    yMin=ge->properties.header->yMin;
	    yMax=ge->properties.header->yMax;
	    break;
	}

	/* normalize, provide defaults for bad fonts */
	if(yMax==0)
	{
	    /* not set, default */
	    yMin=ge->properties.header->yMin;
	    yMax=ge->properties.header->yMax;
	}
	if(yMin>0)
	    yMin = 0-yMin;
		
	/* split min/max to whole units on each side of baseline */

	ge->corrected_upem = yMax-yMin;
		
	alt_point = (ge->properties.header->Units_Per_EM * ge->point_size) / (yMax-yMin);

	a = alt_point * yMax / (yMax-yMin);
	d = alt_point * (0-yMin) / (yMax-yMin);
	alt_point = alt_point - (alt_point - a - d);
    }
		
    error = TT_Set_Instance_CharSize(ge->instance, alt_point*64);
    if (error)
    {
	D(bug("Could not set point size.\n"));
	return set_last_error(ge,OTERR_Failure);
    }

    TT_Get_Instance_Metrics(ge->instance, &ge->imetrics );

    /* get rid of any old glyphs */
    /*	ClearAllGlyphs(ge);	*/

    ge->instance_changed=FALSE;

    ChooseEncoding(ge);

    return(set_last_error(ge,OTERR_Success));
}
