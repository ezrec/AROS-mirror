/*
 * Based on the code from the ttf.library by Richard Griffith.
 */
#include "kerning.h"
#include "glyph.h"
#include "ttfdebug.h"

#include <proto/exec.h>
#include <proto/alib.h>
#include <exec/memory.h>
#include <diskfont/oterrors.h>

void FreeWidthList(struct MinList *list)
{
    struct GlyphWidthEntry *work_node, *next_node;

//    D(bug("FreeWidthList at %lx\n",list));
    if (list == NULL)
	return;

    work_node = (struct GlyphWidthEntry *)(list->mlh_Head);
    while (next_node = 
	   (struct GlyphWidthEntry *)(work_node->gwe_Node.mln_Succ))
    {
	FreeMem(work_node, sizeof(struct GlyphWidthEntry));
	work_node = next_node;
    }
    FreeMem(list, sizeof(struct MinList));
}


struct MinList *GetWidthList(TTF_GlyphEngine *ge)
{
    struct MinList *gw;
    struct GlyphWidthEntry *node;
    ULONG howwide;
    int hold_start, c_start, c_stop, i, j;
    ULONG units_per_em;

    if (ge->instance_changed)
    {
	/* reset everything first */
	if(SetInstance(ge)!=OTERR_Success)
	    return NULL;
    }
    if (ge->cmap_index==NO_CMAP_SET && ChooseEncoding(ge)!=0)
	return NULL;
	
    gw = (struct MinList *)AllocMem(sizeof(struct MinList), MEMF_CLEAR);

    if(gw==NULL)
    {
	D(bug("GetWidthList - AllocMem for list failed\n"));
	set_last_error(ge,OTERR_NoMemory);
	return NULL;
    }
    NewList((struct List *)gw);

#if 0
    units_per_em=ge->properties.header->Units_Per_EM;
    int yMin,yMax;

    yMin=ge->properties.header->yMin; if(yMin>0) yMin = 0-yMin;
    yMax=ge->properties.header->yMax;
    units_per_em=yMax-yMin;
#endif

    units_per_em = ge->corrected_upem;

    hold_start=ge->request_char;
    c_start=ge->request_char;
    c_stop=ge->request_char2;
    if(c_start>c_stop)
    {
	/* just in case */
	c_start=ge->request_char2;
	c_stop=ge->request_char;
    }
    D(bug("GetWidthList - from %ld to %ld\n",c_start,c_stop));

    for(i=c_start;i<=c_stop;i++)
    {
	ge->request_char=i;
	j=UnicodeToGlyphIndex(ge);

	if(j==0)
	{
	    D(bug("GetWidthList - no glyph %ld i=%ld p=%ld e=%ld\n"
		  ,i, ge->cmap_index,ge->platform, ge->encoding));
	    continue;
	}

	if(TT_Load_Glyph(ge->instance,ge->glyph,j,0))
	{
	    D(bug("Error loading glyph %ld.\n",(LONG)j));
	    howwide=0;
	}
	else
	{
	    TT_Get_Glyph_Metrics( ge->glyph, &ge->metrics );
	    howwide=((ge->metrics.advance)<<16)/units_per_em;
	}

	node = AllocMem(sizeof(struct GlyphWidthEntry), MEMF_CLEAR);
	if(node==NULL)
	{
	    D(bug("GetWidthList - AllocMem for %ld failed\n",i));
	    FreeWidthList(gw);
	    set_last_error(ge,OTERR_NoMemory);
	    ge->request_char=hold_start;
	    return NULL;
	}

	node->gwe_Code = i;	/* amiga char */
	/* character advance, as fraction of em width */
	node->gwe_Width = howwide;

//		D(bug("GetWidthList - glyph %ld width %lx\n",
//			node->gwe_Code,	node->gwe_Width	));
		
	AddTail( (struct List *)gw, (struct Node *)node );
    }

    ge->request_char=hold_start;
    return gw;
}

int get_kerning_dir(TTF_GlyphEngine *ge)
{
    TT_Error err;
    TT_Kern_Subtable *k;
    TT_Kern_0_Pair   *p;	/* a table of nPairs 'pairs' */

    int i,kern_value;
    TT_UShort l,r;		/* left and right indexes */

    /* instance change may be irrelevant, but just in case */
    if (ge->instance_changed)
	if(SetInstance(ge)!=OTERR_Success)
	    return NULL;
    
    if(ge->KernPairs<0)
    {
	ge->KernPairs=0;
	ge->kern_subtable=NULL;
		
	err = TT_Get_Kerning_Directory(ge->face, &ge->kerning_dir);
	if(err) return(0);
	
	k=ge->kerning_dir.tables;
	
	for(i=0; i<ge->kerning_dir.nTables; ++i)
	{
	    /* try and ignore 'override' coverage */
	    if(k->format==0 && (k->coverage & 0x07)==1)
	    {
		err=TT_Load_Kerning_Table(ge->face,i);
		if(err==0)
		{
		    ge->KernPairs=k->t.kern0.nPairs;
		    ge->kern_subtable=k;
		    break;
		}
	    }
	    ++k;
	}
    }

    if(ge->kern_subtable==NULL) return(0);

    /* get left and right glyph indexes */
    l=char_to_glyph(ge,ge->request_char);
    r=char_to_glyph(ge,ge->request_char2);
    if(l==0 || r==0) return(0);	/* no chance */
	
    /* scan table for match  xxx - should binary search */
    p=ge->kern_subtable->t.kern0.pairs;
    kern_value=0;
	
    D(bug( "KernLookup l=%ld r=%ld\n",l,r));
    for(i=0; i<ge->kern_subtable->t.kern0.nPairs; i++)
    {
	if(p->left > l) break;   /* passed its place */
	if(p->left == l)
	{
	    if(p->right == r)
	    {
		kern_value = ((0-(p->value)) << 16 ) / ge->corrected_upem;
					
/* 	/ge->properties.header->Units_Per_EM; */

		D(bug( "KernValue %ld (0x%lx at %ld)\n",p->value, kern_value,ge->corrected_upem ));

		break;
	    }
	}
	++p;
    }
	
    return (kern_value);
}
