/*
 * Based on the code from the ttf.library by Richard Griffith.
 */
/* common routines for ttfinstall and ttfmanager */

#include <exec/types.h>
#include <exec/lists.h>
#include <exec/nodes.h>
#include <exec/memory.h>
#include <exec/execbase.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/alib.h>
#include <diskfont/diskfonttag.h>
#include <graphics/text.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <aros/macros.h>

#include <freetype/freetype.h>
#include <freetype/ftxerr18.h> /* error strings */
#include <freetype/ftxkern.h>  /* kerning       */

/* extra spec tags
Spec1 is full filename of ttf file
Spec2 is custom code page (Indirect) or Zero (direct)
Spec3 is custom transform (Indirect) or Zero (direct)
Spec4 is metric source kludge
Spec5 is custom y metrics
Spec6 is bold filename
Spec7 is italic filename
Spec8 is bold italic filename
*/
#define  OT_Spec2	(OT_Level1 | 0x102)
#define  OT_Spec3	(OT_Level1 | 0x103)
#define  OT_Spec4	(OT_Level1 | 0x104)
#define  OT_Spec5	(OT_Level1 | 0x105)
#define  OT_Spec6	(OT_Level1 | 0x106)
#define  OT_Spec7	(OT_Level1 | 0x107)
#define  OT_Spec8	(OT_Level1 | 0x108)

/* freetype related items */

static  TT_Error     error;

TT_Engine    engine;
TT_Face      face;
TT_Instance  instance;
TT_Glyph     glyph;

TT_Instance_Metrics  imetrics;
TT_Outline           outline;
TT_Glyph_Metrics     metrics;

TT_Face_Properties   properties;

TT_CharMap  cmap;

TT_Kerning kerning_dir;

static char  name_buffer[257];
static int   name_len = 0;

/* much of the following was blatantly lifted from Amish S. Dave's
   type1.library t1manager program.
*/

/* ttf.library and installation specific stuff */
char enginename[]="freetype1";

char base_fontname[25];
char font_filename[32];
char otag_filename[32];
char family[256];

char family_bold_filename[256]="";
char family_italic_filename[256]="";
char family_bold_italic_filename[256]="";

char install_dir[256];
char install_font[256];
char install_otag[256];

long SpaceWidth=0;	/* fudge value */
long SymbolSet= -1;
long YSizeFactor=0;
long IsFixed=FALSE;
long SlantStyle=OTS_Upright;
long StemWeight=OTS_Medium;
long HorizStyle=OTH_Normal;
long SerifFlag=0;
long InhibitAlgoStyle = 0;

/* font info we grab when font is open to reference later */
char font_nametable[9][256];
ULONG advance_Width_Max;
ULONG Units_Per_EM;
long num_Faces;
long num_Glyphs;
long yMin, yMax;
long xMin, xMax;
long Lowest_Rec_PPEM;
long Ascender;
long Descender;
long Line_Gap;
long fsType;
long sTypoAscender;
long sTypoDescender;
long sTypoLineGap;
long usWinAscent;
long usWinDescent;
long num_glyphs;
long KernPairs;
long usFirstCharIndex;
long calc_yMin, calc_yMax;

long MONO_SpaceWidth=0;
long PROP_SpaceWidth=0;

unsigned short int codepage[256];
int save_codepage=0;	/* save codepage in .otag file */
int metric_source=0;	/* where to get height metrics */
long metric_custom=0;	/* custom min and max */

/* The next array is actually to be a big endian UWORD array */
UWORD availsizelist[21] = {AROS_WORD2BE(8), AROS_WORD2BE(12), AROS_WORD2BE(14),
    AROS_WORD2BE(16), AROS_WORD2BE(24), AROS_WORD2BE(32), AROS_WORD2BE(48),
    AROS_WORD2BE(60), AROS_WORD2BE(72)};

int spaces_in_name_ok=0;

static unsigned char fontfiledata[] = {0x0f, 0x03, 0x00, 0x00};

char message_buffer[256];

struct otagnode
{
	struct MinNode MyNode;
	ULONG ti_Tag;
	ULONG ti_Data;
	ULONG size;
};

/* some forward protos */
struct MinList *NewOTagList(void);
int AddOTag(struct MinList *list, ULONG tag, ULONG data, ULONG size);
ULONG SizeOTagList(struct MinList *list);
void DumpOTagList(struct MinList *list, UBYTE *buf);
void FreeOTagList(struct MinList *list);

/*
 * Create the otags file in memory.
 * filename is the actual outline file
 * all tags must be set beforehand, as well as install_dir
 * and font_filename and otag_filename (set_base_fontnames)
 *
 * This is currently a multi-step procedure:
 *	Create a linked list containing the tag information and a size field
 *	that contains the size of OT_Indirect data
 *
 *	Allocate memory for the total size of the otag file.
 *
 *	Insert the linked list and OT_Indirect information into the memory.
 *	(while correcting the OT_Indirect ti_Data pointers)
 *
 *	Put the total size in the OT_FileIdent data field.
 *
 */
BOOL TTF_InstallFont(char *filename)
{
    struct TagItem *OTags;
    ULONG otagsize;
    struct MinList *otaglist;
    BPTR otagfile = NULL;
    BPTR fontfile = NULL;
    void *otagbuf;
    long mtemp;

    /* quick fix attempt, set the InhibitAlgoStyle flags here */
    /* engine does not do undeline, yet. */
    InhibitAlgoStyle = FSF_UNDERLINED;
    /* engine does not do algorithmic bold, but will do with families */
    if(strlen(family_bold_filename)==0) InhibitAlgoStyle |= FSF_BOLD;

    /* can always do FSF_ITALIC by transform or file,
     * and FSF_EXTENDED by transform
     */
    strcpy(install_font,install_dir);
    AddPart(install_font,font_filename,sizeof(install_font));
	
    strcpy(install_otag,install_dir);
    AddPart(install_otag,otag_filename,sizeof(install_otag));

    if(metric_source==5)
    {
	mtemp=calc_yMin;
	if(mtemp<0) mtemp=0-mtemp;
	metric_custom = (calc_yMax << 16) | mtemp;
    }
    else
	metric_custom = 0;

    if ( (otaglist = NewOTagList()) &&
	 (AddOTag(otaglist, OT_FileIdent, 0, 0)) &&
	 (AddOTag(otaglist, OT_Engine, (ULONG) enginename, strlen(enginename) + 1)) &&
	 (AddOTag(otaglist, OT_Family, (ULONG) family, strlen(family) + 1)) &&
#if 0
	 (AddOTag(otaglist, OT_BName, (ULONG) family_bold, strlen(family_bold) + 1)) &&
	 (AddOTag(otaglist, OT_IName, (ULONG) family_italic, strlen(family_italic) + 1)) &&
	 (AddOTag(otaglist, OT_BIName, (ULONG) family_bold_italic, strlen(family_bold_italic) + 1)) &&
#endif
	 (AddOTag(otaglist, OT_InhibitAlgoStyle, (ULONG) InhibitAlgoStyle, 0)) &&
	 (AddOTag(otaglist, OT_AvailSizes, (ULONG) availsizelist, sizeof(availsizelist))) &&
	 (AddOTag(otaglist, OT_IsFixed, (ULONG) IsFixed, 0)) &&
	 (AddOTag(otaglist, OT_YSizeFactor, (LONG) YSizeFactor, 0)) &&
	 (AddOTag(otaglist, OT_SpaceWidth, (ULONG) SpaceWidth, 0)) &&
	 (AddOTag(otaglist, OT_SymbolSet, (ULONG) SymbolSet, 0)) &&
	 (AddOTag(otaglist, OT_SerifFlag, (ULONG) SerifFlag, 0)) &&
	 (AddOTag(otaglist, OT_StemWeight, (ULONG) StemWeight, 0)) &&
	 (AddOTag(otaglist, OT_SlantStyle, (ULONG) SlantStyle, 0)) &&
	 (AddOTag(otaglist, OT_HorizStyle, (ULONG) HorizStyle, 0)) &&
	 (AddOTag(otaglist, OT_SpecCount, 3L, 0)) &&
	 (AddOTag(otaglist, OT_Spec1 | OT_Indirect, (ULONG) filename, strlen(filename) + 1)) &&
	 (save_codepage ?
	     (AddOTag(otaglist, OT_Spec2 | OT_Indirect, (ULONG)&codepage[0], 512)) :
	     (AddOTag(otaglist, OT_Spec2, 0L, 0)) ) &&
	 (AddOTag(otaglist, OT_Spec3, 0L, 0)) &&
	 (AddOTag(otaglist, OT_Spec4, metric_source, 0)) &&
	 (AddOTag(otaglist, OT_Spec5, metric_custom, 0)) &&
	 (strlen(family_bold_filename) ?
	     (AddOTag(otaglist, OT_Spec6 | OT_Indirect, (ULONG)&family_bold_filename,
		      strlen(family_bold_filename)+1)) :
	     (AddOTag(otaglist, OT_Spec6, 0L, 0)) ) &&
	 (strlen(family_italic_filename) ?
	     (AddOTag(otaglist, OT_Spec7 | OT_Indirect, (ULONG)&family_italic_filename,
		      strlen(family_italic_filename)+1)) :
	     (AddOTag(otaglist, OT_Spec7, 0L, 0)) ) &&
	 (strlen(family_bold_italic_filename) ?
	     (AddOTag(otaglist, OT_Spec8 | OT_Indirect, (ULONG)&family_bold_italic_filename,
		      strlen(family_bold_italic_filename)+1)) :
	     (AddOTag(otaglist, OT_Spec8, 0L, 0)) ) &&
	 (AddOTag(otaglist, TAG_DONE, 0, 0)) )
    {
	otagsize = SizeOTagList(otaglist);
	if (otagbuf = (void *)AllocMem(otagsize, MEMF_CLEAR))
	{
	    DumpOTagList(otaglist, otagbuf);
	    FreeOTagList(otaglist);
	    OTags = (struct TagItem *) otagbuf;
	    OTags[0].ti_Data = (ULONG)AROS_LONG2BE(otagsize);
	}
	else
	{
	    FreeOTagList(otaglist);
	    return FALSE;		/* failure allocating otagbuf */
	}
    }
    else
    {
	/* Note: FreeOTagList is safe to call with a NULL argument */
	FreeOTagList(otaglist);
	return FALSE;			/* failure allocating otaglist */
    }
    
/**
 * Write the file to the previously determined otagfilename
 **/
// What about already existing files?

    if (otagfile = Open(install_otag, MODE_NEWFILE))
    {
	Write(otagfile, otagbuf, otagsize);
	Close(otagfile);
	FreeMem(otagbuf, otagsize);
    }
    else
    {
	sprintf(message_buffer,"Error creating %s\n", install_otag);
	FreeMem(otagbuf, otagsize);
	return FALSE;			/* failure creating otag file */
    }

    /* Create the .font file: */
    if (fontfile = Open(install_font, MODE_NEWFILE))
    {
	Write(fontfile, fontfiledata, sizeof(fontfiledata));
	Close(fontfile);
    }
    else
    {
	sprintf(message_buffer,"Error creating %s\n", install_font);
	return FALSE;			/* failure creating font file */
    }
    
#if 0
don''t need or want dir yet
// Create the font directory: NEED ERROR CHECKING
// Note: passing NULL to UnLock is harmless...
	BPTR newdirlock = NULL;
    
    newdirlock = CreateDir(dirfilename);
    if (newdirlock)
	UnLock(newdirlock);
    else
    {
	MyErrorMsg1("Error trying to create directory:\n'%s'\n", dirfilename);
	return FALSE;			/* failure creating font dir */
    }
#endif
    return TRUE;
}


struct MinList *NewOTagList(void)
{
    struct MinList *otaglist;

    if (otaglist = (struct MinList *)AllocMem(sizeof(struct MinList), MEMF_CLEAR))
	NewList((struct List *)otaglist);

    return otaglist;
}



int AddOTag(struct MinList *list, ULONG tag, ULONG data, ULONG size)
{
    struct otagnode *node;

    if (!(node = AllocMem(sizeof(struct otagnode), MEMF_CLEAR)))
	return 0;

    node->ti_Tag = tag;
    node->ti_Data = data;
    node->size = size;
    AddTail( (struct List *)list, (struct Node *)node );

    return 1;
}


ULONG SizeOTagList(struct MinList *list)
{
    struct otagnode *node;
    ULONG size = 0;

    for (node = (struct otagnode *)(list->mlh_Head); node->MyNode.mln_Succ; node = (struct otagnode *)node->MyNode.mln_Succ)
	size += 2 * sizeof(ULONG) + node->size;

    return size;
}


/**
 * Create an otag file in memory
 * first pass through the list, and copy the ti_Tag and ti_Data values
 * Then pass again, and copy the OT_Indirect information over, modifying
 * the corresponding memory ti_Data pointers accordingly.
 **/
void DumpOTagList(struct MinList *list, UBYTE *buf)
{
    struct otagnode *node;
    ULONG *cur;
    UBYTE *ptr;

    cur = (ULONG *)buf;
    for (node = (struct otagnode *)(list->mlh_Head); node->MyNode.mln_Succ; node = (struct otagnode *)node->MyNode.mln_Succ)
    {
	*(cur++) = AROS_LONG2BE(node->ti_Tag);
	cur++;	/* don't bother setting ti_Data here */
    }

    ptr = (UBYTE *)cur;
    cur = (ULONG *)buf;
    for (node = (struct otagnode *)(list->mlh_Head); node->MyNode.mln_Succ; node = (struct otagnode *)node->MyNode.mln_Succ)
    {
	cur++;		/* Advance to ti_Data field */
	
	if (node->size > 0)		/* Store indirect data */
	{
	    *(cur++) = (ULONG) AROS_LONG2BE(ptr - buf);
	    CopyMem((void *)node->ti_Data, ptr, node->size);
	    ptr += node->size;
	}
	else				/* Copy the ti_Data */
	    *(cur++) = (ULONG)AROS_LONG2BE(node->ti_Data);
    }
}


void FreeOTagList(struct MinList *list)
{
    struct otagnode *work_node, *next_node;

    if (list == NULL)
	return;

    work_node = (struct otagnode *)(list->mlh_Head);
    while (next_node = (struct otagnode *)(work_node->MyNode.mln_Succ))
    {
	FreeMem(work_node, sizeof(struct otagnode));
	work_node = next_node;
    }
    FreeMem(list, sizeof(struct MinList));
}


/* end t1manager code */

static char*  LookUp_Name( int  index )
{
    unsigned short  i, n;

    unsigned short  platform, encoding, language, id;
    char*           string;
    unsigned short  string_len;

    int             j, found;


    n = properties.num_Names;

    for ( i = 0; i < n; i++ )
    {
	TT_Get_Name_ID( face, i, &platform, &encoding, &language, &id );
	TT_Get_Name_String( face, i, &string, &string_len );

	if ( id == index )
	{
	    /* The following code was inspired from Mark Leisher's */
	    /* ttf2bdf package                                     */

	    found = 0;

	    /* Try to find a Microsoft English name */

	    if ( platform == 3 )
		for ( j = 1; j >= 0; j-- )
		    if ( encoding == j )  /* Microsoft ? */
			if ( (language & 0x3FF) == 0x009 )    /* English language */
			{
			    found = 1;
			    break;
			}

	    if ( !found && platform == 0 && language == 0 )
		found = 1;

	    /* Found a Unicode Name. */

	    if ( found )
	    {
		if ( string_len > 510 )
		    string_len = 510;

		name_len = 0;

		for ( i = 1; i < string_len; i += 2 )
		    name_buffer[name_len++] = string[i];

		name_buffer[name_len] = '\0';

		return name_buffer;
	    }
	}
    }

    /* Not found */
    return NULL;
}


void make_clean_basename(char *fontname)
{
    char *i;
    int cnt,c;

    i=fontname;	/* already specified */
    cnt=0;
    while(*i)
    {
	c=*i;
	/* allow ascii letters numbers, harmless punctuation. 
	 * spaces only if requested
	 */
	if (	   (c>='A' && c<='Z') 
		   || (c>='a' && c<='z')
		   || (c>='0' && c<='9')
		   || c=='.' || c=='_' || c=='-'
		   || (c==' ' && spaces_in_name_ok!=0)
		   )
	    base_fontname[cnt++]=c;
	if(cnt>23) break;
	++i;
    }
    base_fontname[cnt]=0;
}


/* convert the truetype font name to amiga filenames.
   Since there is a <=30 character limit, make sure we leave room for
   the .font and .otag parts
*/
void set_base_fontnames( char *filename, char *fontname )
{
    char *i;
    int cnt,c;

    if(fontname==NULL)
    {
	/* make a name up */
	
	/* get full font name, in case of error, use filename */
	if ( LookUp_Name( 4 ) )
	    i=name_buffer;
	else /* not likely, but just in case */
	    i=FilePart(filename);
	make_clean_basename(i);
    }
    else
    {
	i=fontname;	/* already specified */
	cnt=0;
	/* use what user specifed, clip size if needed */
	while(*i)
	{
	    c=*i;
	    base_fontname[cnt++]=c;
	    if(cnt>23) break;
	    ++i;
	}
	base_fontname[cnt]=0;
    }

    strcpy(font_filename,base_fontname);
    strcat(font_filename,".font");

    strcpy(otag_filename,base_fontname);
    strcat(otag_filename,".otag");
}

void pick_a_symbolset(void)
{
    int i, n;
    unsigned short  platform, encoding;

    /* if cmap index not specified, choose one, hopefully unicode */
    /* To prefer Windows unicode, as apple tables are often trashed in
     * Windows fonts, we go backwards. Also, if Windows Symbol exists,
     * use it, as any other table is likely garbage */

    if(SymbolSet<0)
    {
	n = properties.num_CharMaps; /* already checked for zero */

	for ( i = n-1; i >= 0; i-- )
	{
	    TT_Get_CharMap_ID(face, i, &platform, &encoding);
	    if ((platform == 3 && encoding == 0)  ||
		(platform == 3 && encoding == 1)  ||
		(platform == 0 && encoding == 0))
	    {
		SymbolSet= i;
		break;
	    }
	}

	if ( SymbolSet < 0 )
	{
	    i=0;
	    SymbolSet= i;
	    /* just use the first one, whatever it is */
	    TT_Get_CharMap_ID(face, i, &platform, &encoding);
	    TT_Get_CharMap( face, i, &cmap );
	}

	/* not exactly like bullet, but same intent */
    }

    /* this is for SpaceWidth to find ' ' */
    if(SymbolSet<0x10000)
	TT_Get_CharMap( face, SymbolSet, &cmap );
    else
	TT_Get_CharMap( face, 0, &cmap );
}

void get_kerning_dir(void)
{
    TT_Error err;
    TT_Kern_Subtable *k;

    int i;

    KernPairs=0;
		
    err = TT_Get_Kerning_Directory(face, &kerning_dir);
    if(err) return;
	
    k=kerning_dir.tables;
	
    for(i=0; i<kerning_dir.nTables; ++i)
    {
	/* try and ignore 'override' coverage */
	if(k->format==0 && (k->coverage & 0x07)==1)
	{
	    err=TT_Load_Kerning_Table(face,i);
	    if(err==0)
	    {
		KernPairs=k->t.kern0.nPairs;
		return;
	    }
	}
	++k;
    }

    return;
}

int set_properties_tags(void)
{
    int howwide;
    unsigned int range;
    int i;
    int rc=0;

    for(i=0;i<9;++i)
    {
	if (LookUp_Name(i))
	    strcpy(font_nametable[i],name_buffer);
	else
	    strcpy(font_nametable[i],"");
    }

    num_Glyphs        = properties.num_Glyphs;
    num_Faces         = properties.num_Faces;
    xMax              = properties.header->xMax;
    xMin              = properties.header->xMin;
    Lowest_Rec_PPEM   = properties.header->Lowest_Rec_PPEM;
    advance_Width_Max = properties.horizontal->advance_Width_Max;
    Units_Per_EM      = properties.header->Units_Per_EM;
    Ascender          = properties.horizontal->Ascender;
    Descender         = properties.horizontal->Descender;
    Line_Gap          = properties.horizontal->Line_Gap;
    fsType            = properties.os2->fsType; /* embedding */
    sTypoAscender     = properties.os2->sTypoAscender;
    sTypoDescender    = properties.os2->sTypoDescender;
    sTypoLineGap      = properties.os2->sTypoLineGap;
    usWinAscent       = properties.os2->usWinAscent;
    usWinDescent      = properties.os2->usWinDescent;
    usFirstCharIndex  = properties.os2->usFirstCharIndex;

    get_kerning_dir();

    /* stash the family name for otag file */
    if ( strlen(font_nametable[1]) )
	strcpy(family,font_nametable[1]);
    else
	strcpy(family,"UnnamedFamily");

    IsFixed=FALSE;
    if(properties.postscript->isFixedPitch)
	IsFixed=TRUE;


    SlantStyle=OTS_Upright;
    if(properties.postscript->italicAngle < 0)
	SlantStyle=OTS_Italic;
    if(properties.postscript->italicAngle > 0)
	SlantStyle=OTS_LeftItalic;

    /* often not set properly, but flag is mostly meaningless */
    SerifFlag=FALSE;
    range=properties.os2->sFamilyClass;
    range=(range>>8) & 0xFF;
    if(range>0 && range<6) SerifFlag=TRUE;

    range=properties.os2->usWeightClass;
    /* there are lots of buggy fonts out there, apparently
	   using 1-9 instead of 100-900 */
    if(range<10) range=range * 100;
    StemWeight=OTS_UltraThin;
    if(range>=200) StemWeight=OTS_Thin;
    if(range>=300) StemWeight=OTS_Light;
    if(range>=400) StemWeight=OTS_Book;
    if(range>=500) StemWeight=OTS_Medium;
    if(range>=600) StemWeight=OTS_DemiBold;
    if(range>=700) StemWeight=OTS_Bold;
    if(range>=800) StemWeight=OTS_Black;
    if(range>=900) StemWeight=OTS_UltraBlack;

    range=properties.os2->usWidthClass;
    HorizStyle=OTH_UltraCompressed;
    if(range>1) HorizStyle=OTH_ExtraCompressed;
    if(range>2) HorizStyle=OTH_Compressed;
    if(range>3) HorizStyle=OTH_Condensed;
    if(range>4) HorizStyle=OTH_Normal;
    if(range>5) HorizStyle=OTH_SemiExpanded;
    if(range>6) HorizStyle=OTH_Expanded;
    if(range>7) HorizStyle=OTH_ExtraExpanded;


    /* Calculate OT_SpaceWidth - a quote from AmigaMail V2:

     This tag's value is the width of the space character at 250 points
     (where there are 72.307 points in an inch).  The width is in Design
     Window Units (DWUs).  One DWU is equal to 1/2540 inches.

     Very generic, eh?
     */

    /* we must have a cmap for the following */
    pick_a_symbolset();

    /* monospace is an exception */
    howwide=properties.horizontal->advance_Width_Max;
    MONO_SpaceWidth = ((howwide * 250) / 72);
	
    if(SymbolSet>0x10000)
	/* raw glyphs, just use max? */
	PROP_SpaceWidth=MONO_SpaceWidth;
    else
    {
	i=TT_Char_Index( cmap, ' '); /* which glyph is space? */

	error = 0;
	if(error==0) error = TT_New_Glyph( face, &glyph );
	if(error==0) error = TT_New_Instance( face, &instance );
	if(error==0) error = TT_Set_Instance_Resolutions(instance,2540,2540);
	if(error==0) error = TT_Set_Instance_CharSize(instance, 250*64);
	if(error==0) error = TT_Load_Glyph(instance,glyph,i,TTLOAD_DEFAULT);

        if(error==0) TT_Get_Glyph_Metrics( glyph, &metrics );

	if(error==0)
	    howwide = metrics.advance;
	else
	{
	    sprintf(message_buffer,"Broken Font: %s",
		    TT_ErrToString18( error ));
	    rc=error;
	    howwide=Units_Per_EM;	/* good default? */
	}

	PROP_SpaceWidth = howwide >> 6 ;
	/* pain in the ass caclulation */
	/*	SpaceWidth = (((howwide * 250)/units_per_em) * 72) / 2540; */
	/*	SpaceWidth = ((howwide * 250) / 72); */
    }
    if(IsFixed) SpaceWidth=MONO_SpaceWidth;
    else  SpaceWidth=PROP_SpaceWidth;

    /* set OT_YSizeFactor
     * This tag's value is a ratio of the point height of a typeface to
     * its ``black'' height (the point height minus the space on the
     * typeface's top and bottom).  The high word is the point height
     * factor and the low word is the black height factor.  Note that
     * these values form a ratio.  Individually, they do not necessarily
     * reflect any useful value.

     * ttf.library no longer requires pre-scaling by diskfont.library
     * so this just sets a 1:1 ratio. We store some of the values for
     * use in ttfmanager though.
     */
#if 0
    decender_hack = properties.os2->sTypoDescender;
    if (decender_hack>0) decender_hack = 0 - decender_hack;
    YSizeFactor = ((properties.header->Units_Per_EM)<<16) |
	(properties.os2->sTypoAscender
	 - decender_hack
	 + properties.os2->sTypoLineGap) ;
#endif

    /* xxx - try using header - should be calculated and correct? */

    yMin=properties.header->yMin;
    yMax=properties.header->yMax;
    if (yMin>0) yMin = 0 - yMin;

    /*	YSizeFactor = (Units_Per_EM<<16) | (yMax - yMin); */
    YSizeFactor = (Units_Per_EM<<16) | Units_Per_EM;

    /* xxx - this is in the wrong place */	
    /* engine does not do undeline, yet. */
    InhibitAlgoStyle = FSF_UNDERLINED;
    /* engine does not do algorithmic bold, but will do with families */
    if(strlen(family_bold_filename)==0) InhibitAlgoStyle |= FSF_BOLD;

    /*
     other bits FSF_ITALIC 	| FSF_EXTENDED ;
     */
    return rc;
}


/* simple no frills default install */
int  ttfinst(char *filename,char *fontname)
{
    int rc=0;

    /* Open and Load face */

    error = TT_Open_Face( engine, filename, &face );
    if ( error )
    {
	sprintf(message_buffer,"Error: %-28s %s.\n",
		TT_ErrToString18( error ), filename );
	return 16;
    }

    /* get face properties and allocate preload arrays */

    TT_Get_Face_Properties( face, &properties );
    num_glyphs = properties.num_Glyphs;
    if(properties.num_CharMaps == 0)
    {
	sprintf(message_buffer,"Error: %-28s %s.\n",
		"No character maps", filename );
	return 16;
    }

    SymbolSet=-1;
    set_base_fontnames(filename,fontname);
    if(set_properties_tags())
	rc=16;
    else
	if (TTF_InstallFont(filename)==FALSE) rc=16;
	
    TT_Close_Face( face );

    return rc;
}


int sanctify_to_dir(char *to)
{
    BPTR lock;
    struct FileInfoBlock *fib;
    int rc=0;

    lock=Lock(to,ACCESS_READ);
    if(lock==0)
    {
	sprintf(message_buffer,"Cannot lock TO directory %s\n",to);
	return 1;
    }

    fib=AllocDosObjectTags(DOS_FIB,0,0);
    if(fib==NULL)
    {
	sprintf(message_buffer,"TO directory AllocDosObject failed");
	UnLock(lock);
	return 1;
    }

    if(Examine(lock,fib)==FALSE)
    {
	sprintf(message_buffer,"Cannot examine TO directory %s\n",to);
	rc=1;
    }
    else if(fib->fib_DirEntryType<=0)
    {
	/* wrong type, we want dir */
	sprintf(message_buffer,
		"'TO %s' is invalid.\n"
		"A directory is required\n"
		,to);
	rc=1;
    }

    FreeDosObject(DOS_FIB,fib);
	
    UnLock(lock);
	
    if(rc==0)
	strcpy(install_dir,to);

    return rc;
}

/* find exact bbox for current font and codepage */
int calc_ymin_ymax(char *filename)
{
    int glyph_index, i, j;
    int face_opened=0;

    if(save_codepage==0)
    {
	/* if not set yet, set as default now */
	for(i=0;i<256;++i) codepage[i]=i;
	
	GetVar( "ttfcodepage", (STRPTR)codepage, 512, 
		LV_VAR | GVF_BINARY_VAR | GVF_DONT_NULL_TERM);
    }

    error = 0;
    error = TT_Open_Face( engine, filename, &face );

    if(error==0) face_opened=1;
    if(error==0) error = TT_New_Glyph( face, &glyph );
    if(error==0) error = TT_New_Instance( face, &instance );

    if ( error )
    {
	sprintf(message_buffer,"Error: %-28s %s.\n",
		TT_ErrToString18( error ), filename );
	if(face_opened) TT_Close_Face( face );
	return 16;
    }

    if(SymbolSet<0x10000)
	TT_Get_CharMap( face, SymbolSet, &cmap );
		
    calc_yMin=0; calc_yMax=0;

    for(i=33;i<256;++i)
    {
	if(SymbolSet<0x10000)
	{
	    j=codepage[i];
	    if(usFirstCharIndex>=0xF000) j=i-32+usFirstCharIndex;
	    glyph_index=TT_Char_Index( cmap, j);
	}
	else
	{
	    if(i<128) glyph_index=(SymbolSet&0xFFFF) + i - 32;
	    else glyph_index=0;
	}

	if(glyph_index>0)
	{
	    error = TT_Load_Glyph(instance,glyph,glyph_index,0);
	    if(error==0)
	    {
		TT_Get_Glyph_Metrics( glyph, &metrics );
		if(metrics.bbox.yMin<calc_yMin)
		    calc_yMin=metrics.bbox.yMin;
		if(metrics.bbox.yMax>calc_yMax)
		    calc_yMax=metrics.bbox.yMax;
	    }
	}
    }

    TT_Close_Face( face );
    return 0;
}

int initialize_freetype(void)
{
    /* Initialize freetype engine */

    if ( (error = TT_Init_FreeType( &engine )) )
    {
	sprintf(message_buffer,
		"Error while initializing freetype engine: %s",
		TT_ErrToString18( error ) );
	return error;
    }

    if (error=TT_Init_Kerning_Extension(engine))
    {
	sprintf(message_buffer,
		"Error while initializing freetype (kerning): %s",
		TT_ErrToString18( error ) );
	return error;
    }

    return 0;
}

int terminate_freetype(void)
{
    TT_Done_FreeType( engine );
    return 0;
}
