#include <exec/exec.h>
#include <dos/dos.h>
#include <graphics/text.h>
#include <intuition/intuition.h>
#include <diskfont/diskfont.h>
#include <diskfont/diskfonttag.h>
#include <diskfont/glyph.h>
#include <diskfont/oterrors.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/bullet.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>

#include <aros/debug.h>
#include <aros/macros.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/****************************************************************************************/

#define ARG_TEMPLATE "FONTNAME/A,FONTSIZE/N/A"
#define ARG_FONTNAME 0
#define ARG_FONTSIZE 1
#define NUM_ARGS 2

/****************************************************************************************/

#define FIXED_TO_FIXED26_DIV (65536/64)
#define FIXED26_TO_FIXED(x) ((x) << (16 - 6))
#define FIXED_TO_FIXED26(x) ((x) >> (16 - 6))

#define FIXED_TO_APPROX(x) (((x) + 0x8000) >> 16)

#define FLOOR(x) ((x) & -64)

#define CEILING(x) (((x) + 63) & -64)

/****************************************************************************************/

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct UtilityBase *UtilityBase;
struct Library *BulletBase;

struct TagItem *otaglist;
struct GlyphEngine *ge;
struct GlyphMap *gm[257];
struct TextFont font;
ULONG charloc[257];
WORD charspace[257];
WORD charkern[257];

APTR   gfxdata;

STRPTR fontname;
ULONG fontsize;

LONG xdpi, ydpi, lochar, hichar, baseline, gfxwidth;

struct RDArgs *myargs;
IPTR args[NUM_ARGS];
char s[256], otagpath[256];

/****************************************************************************************/

static void freeglyphmaps(void);

/****************************************************************************************/

static void cleanup(char *msg)
{
    if (msg) printf("makebitmapfont: %s\n", msg);
    
    if (gfxdata) FreeVec(gfxdata);
    
    freeglyphmaps();
    
    if (ge) CloseEngine(ge);
    if (BulletBase) CloseLibrary(BulletBase);    
    if (otaglist) FreeVec(otaglist);
    
    if (UtilityBase) CloseLibrary((struct Library *)UtilityBase);
    if (GfxBase) CloseLibrary((struct Library *)GfxBase);
    if (IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);
    
    if (myargs) FreeArgs(myargs);
    
    exit(0);
}

/****************************************************************************************/

static void getarguments(void)
{
    if (!(myargs = ReadArgs(ARG_TEMPLATE, args, NULL)))
    {
    	Fault(IoErr(), 0, s, 255);
	cleanup(s);
    }

    fontname = (STRPTR)args[ARG_FONTNAME];
    fontsize = *(IPTR *)args[ARG_FONTSIZE];
    
}

/****************************************************************************************/

static void openlibs(void)
{
    IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 38);
    if (!IntuitionBase) cleanup("Can't open intuition.library!\n");
    
    GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 38);
    if (!GfxBase) cleanup("Can't open graphics.library!\n");

    UtilityBase = (struct UtilityBase *)OpenLibrary("utility.library", 38);
    if (!UtilityBase) cleanup("Can't open utility.library!\n");

}

/****************************************************************************************/

static void openfontfile(void)
{
    BPTR  fontfile;
    UWORD fchid;
    
    fontfile = Open(fontname, MODE_OLDFILE);
    if (!fontfile)
    {
    	Fault(IoErr(), 0, s, 255);
	cleanup(s);
    }
    
    if (Read(fontfile, &fchid, sizeof(UWORD)) != sizeof(UWORD))
    {
    	Fault(IoErr(), 0, s, 255);
	Close(fontfile);
	cleanup(s);
    }
    
    Close(fontfile);
    
    if (AROS_WORD2BE(fchid) != OFCH_ID)
    {
    	cleanup("Font Contents Header ID is not OFCH_ID, ie. specified font is not an installed outline font!");
    }
}

/****************************************************************************************/

static void openotagfile(void)
{
    struct FileInfoBlock *fib;
    struct TagItem *ti;
    BPTR otagfile;
    LONG i;
    char *sp;
    
    for(i = 0; fontname[i] && i < 255; i++)
    {
    	otagpath[i] = tolower(fontname[i]);
    }
    otagpath[i] = 0;
    
    sp = strstr(otagpath, ".font");
    if (!sp) cleanup("Panic. Font name does not contain \".font\"!");
    
    strcpy(sp, ".otag");
    
    fib = AllocDosObject(DOS_FIB, NULL);
    if (!fib) cleanup("Out of memory!");
    
    otagfile = Open(otagpath, MODE_OLDFILE);
    if (!otagfile)
    {
    	Fault(IoErr(), 0, s, 255);
	FreeDosObject(DOS_FIB, fib);
	cleanup(s);
    }
    
    if (!ExamineFH(otagfile, fib))
    {
    	Fault(IoErr(), 0, s, 255);
	FreeDosObject(DOS_FIB, fib);
	Close(otagfile);
	cleanup(s);
    }
    
    i = fib->fib_Size;
    FreeDosObject(DOS_FIB, fib);
    
    otaglist = AllocVec(i, MEMF_PUBLIC | MEMF_CLEAR);
    if (!otaglist)
    {
    	Close(otagfile);
	cleanup("Out of memory!");
    }
    
    if (Read(otagfile, otaglist, i) != i)
    {
    	Fault(IoErr(), 0, s, 255);
	Close(otagfile);
	cleanup(s);
    }
    
    Close(otagfile);
    
    if ((AROS_LONG2BE(otaglist->ti_Tag) != OT_FileIdent) ||
    	(AROS_LONG2BE(otaglist->ti_Data) != i))
    {
    	cleanup("Bad .otag file!");
    }
    
    ti = otaglist;
    do
    {
    	ti->ti_Tag = AROS_LONG2BE(ti->ti_Tag);
	ti->ti_Data = AROS_LONG2BE(ti->ti_Data);
	
	if (ti->ti_Tag & OT_Indirect)
	{
	    ti->ti_Data = (IPTR)otaglist + ti->ti_Data;
	}
	
	kprintf("otaglist tag %x  data = %x\n",
	    ti->ti_Tag, ti->ti_Data);
	
    } while ((ti++)->ti_Tag != TAG_DONE);
}

/****************************************************************************************/

static void openfontengine(void)
{
    struct TagItem *ti;
    UBYTE buf[256];
    
    ti = FindTagItem(OT_Engine, otaglist);
    if (!ti) cleanup(".otag file misses OT_Engine tag which defines the name of the font engine to be used with this font!");
    
    strncpy(buf, (STRPTR)ti->ti_Data, 255 - 8);
    strcat(buf, ".library");
    
    BulletBase = OpenLibrary(buf, 0);
    if (!BulletBase)
    {
    	sprintf(s, "Could not open font engine \"%s\"!", buf);
	cleanup(s);
    }
    
    ge = OpenEngine();
    if (!ge) cleanup("OpenEngine() failed!");
    
}

/****************************************************************************************/

static void setupfontengine(void)
{
    struct TagItem maintags[] =
    {
    	{OT_OTagList, (IPTR)otaglist},
	{OT_OTagPath, (IPTR)otagpath},
	{TAG_DONE   	    	    }
    };
    struct TagItem sizetags[] =
    {
    	{OT_PointHeight, 0  },
	{OT_DeviceDPI  , 0  },
	{OT_DotSize    , 0  },
	{TAG_DONE   	    }
    };
    LONG pointheight, xdot, ydot;
    LONG ysizefactor, ysizefactor_low, ysizefactor_high;
    
    if (SetInfoA(ge, maintags) != OTERR_Success)
    {
    	cleanup("Setting main tags failed!");
    }
    
    ysizefactor = GetTagData(OT_YSizeFactor, 0x10001, otaglist);
    ysizefactor_low = ysizefactor & 0xFFFF;
    ysizefactor_high = (ysizefactor >> 16) & 0xFFFF;
    
    xdpi = 72 * ysizefactor_high / ysizefactor_low;
    ydpi = 72 * ysizefactor_high / ysizefactor_low;

    kprintf("\n\nxdpi = %d   ydpi = %d\n", xdpi, ydpi);
        
    pointheight = fontsize << 16;
    
    xdot = ydot = 100;
    
    sizetags[0].ti_Data = pointheight;
    sizetags[1].ti_Data = (xdpi << 16) | ydpi;
    sizetags[2].ti_Data = (xdot << 16) | ydot;

    if (SetInfoA(ge, sizetags) != OTERR_Success)
    {
    	cleanup("Setting size tags failed!");
    }
 
}

/****************************************************************************************/

static void freeglyphmaps(void)
{
    WORD i;
    
    for(i = 0; i < 257; i++)
    {
    	if (gm[i])
	{
	    struct TagItem releasetags[] =
	    {
	    	{OT_GlyphMap, (IPTR)gm[i]},
		{TAG_DONE   	    	 }
	    };
	    
	    ReleaseInfoA(ge, releasetags);
	    
	    gm[i] = NULL;
	}
    }
}

/****************************************************************************************/

static void getglyphmaps(void)
{
    WORD i;
    
    lochar = -1;
    baseline = 0;
    gfxwidth = 0;
    
    for(i = 0; i < 257; i++)
    {
    	struct TagItem settags[] =
	{
	    {OT_GlyphCode, (i < 256) ? i : 0x25A1},
	    {TAG_DONE	    	    	    	 }
	};
	struct TagItem obtaintags[] =
	{
	    {OT_GlyphMap, (IPTR)&gm[i]	    	},
	    {TAG_DONE	    	    	    	}
	};
	
	SetInfoA(ge, settags);
	ObtainInfoA(ge, obtaintags);
	
	if (gm[i])
	{
	    if (i < 256)
	    {
	    	if (lochar == -1) lochar = i;
	    	hichar = i;
	    }
	    
	    if (gm[i]->glm_Y0 - (WORD)gm[i]->glm_BlackTop > baseline)
	    {
	    	baseline = gm[i]->glm_Y0 - (WORD)gm[i]->glm_BlackTop;
	    }
	    
	    gfxwidth += gm[i]->glm_BlackWidth;
	}
    }

    printf("lochar   = %ld\n", lochar);
    printf("hichar   = %ld\n", hichar);
    printf("baseline = %ld\n", baseline);
    printf("gfxwidth = %ld\n", baseline);

}

/****************************************************************************************/

static void makefontbitmap(void)
{
    struct BitMap bm, glyphbm;
    struct RastPort temprp;
    LONG i, xpos;
    
    /* Make gfxwidth a multiple of 32 */
    gfxwidth = (gfxwidth + 31) & ~31;
    
    gfxdata = AllocVec(gfxwidth / 8 * fontsize * 3, MEMF_CHIP);
    if (!gfxdata) cleanup("Out of memory!");
    
    InitBitMap(&bm, 1, gfxwidth, fontsize);
    bm.Planes[0] = gfxdata;
   
    glyphbm.Planes[0] = (APTR)((IPTR)gfxdata) + (gfxwidth / 8 * (fontsize * 2));
  
    xpos = 0;
    
    InitRastPort(&temprp);
    temprp.BitMap = &bm;
    
    SetRast(&temprp, 0);
    SetAPen(&temprp, 1);
    SetDrMd(&temprp, JAM1);
    
    for(i = lochar; i <= hichar + 1; i++)
    {
    	struct GlyphMap *g;
    	WORD index;
	
	index = (i <= hichar) ? i : 256;
	
	g = gm[index];
    	if (g)
	{
	    LONG y, blackl, blackr, blackw, ypos;
	    UBYTE *src, *dest;
	    
	    blackl = g->glm_BlackLeft  & ~15;
	    blackr = ((g->glm_BlackLeft + g->glm_BlackWidth - 1) & ~15) + 16;
	    blackw = blackr - blackl;
	    
	    src = g->glm_BitMap +
	          g->glm_BMModulo * g->glm_BlackTop + 
		  (g->glm_BlackLeft / 16) * 2;
		  
	    dest = (UBYTE *)glyphbm.Planes[0];
	    
	    for(y = 0; y < g->glm_BlackHeight; y++)
	    {
	    	CopyMem(src, dest, blackw / 8);
		src  += g->glm_BMModulo;
		dest += blackw / 8;
	    }
	    
	    InitBitMap(&glyphbm, 1, blackw, g->glm_BlackHeight);
	    
	    ypos = baseline - (g->glm_Y0 - (WORD)g->glm_BlackTop);

    	#if 1 
	//if (i != 32)   
	    BltTemplate(glyphbm.Planes[0],
	    	        g->glm_BlackLeft & 15,
		        blackw / 8,
		        &temprp,
		        xpos,
		        ypos,
			g->glm_BlackWidth,
			g->glm_BlackHeight);
    	#endif	
	    //kprintf("blackwidth [%d] = %d\n", i, g->glm_BlackWidth);
	
	    charloc[i - lochar] = (xpos << 16) + g->glm_BlackWidth;
	    
	    xpos += g->glm_BlackWidth;
		    
	}
	
    }
    
    DeinitRastPort(&temprp);
    
    for(i = lochar; i <= hichar + 1; i++)
    {
    	struct GlyphMap *g;
    	WORD index;
	
	index = (i <= hichar) ? i : 256;
	g = gm[index];
    	if (!g) g = gm[256];
	
	if (g)
	{
	    charkern [i - lochar] = ((WORD)g->glm_BlackLeft) - g->glm_X0;	
	    charspace[i - lochar] = g->glm_X1 - (WORD)g->glm_BlackLeft;

    	#if 0
    	    kprintf("#%d  kern = %d  black = %d space = %d\n",
	    i,
	    charkern[i - lochar],
	    charloc[i - lochar] & 0xffff,
	    charspace[i - lochar]);
	#endif
	    
    	    /* alternative way to calculate charspace based on g->glm_Width:
	    
	                         xdpi       glm_Width   
	    	 ( fontsize x --------- x ------------- ) - charkern
		                 72          65536     
	    */
	}
    }
}

/****************************************************************************************/

static void showfontbitmap(void)
{
    struct Screen *scr;
    struct RastPort rp;
    
    scr = LockPubScreen(NULL);
    
    InitRastPort(&rp);
    rp.BitMap = scr->RastPort.BitMap;
    SetAPen(&rp, 1);
    SetBPen(&rp, 2);
    SetDrMd(&rp, JAM2);
    
    BltTemplate(gfxdata,
    	    	0,
		gfxwidth / 8,
		&rp,
		0,
		0,
		gfxwidth,
		fontsize);
		
    DeinitRastPort(&rp);
}

/****************************************************************************************/

static void makefont(void)
{
    font.tf_Message.mn_Node.ln_Type = NT_FONT;
    font.tf_Message.mn_Node.ln_Name = "convertedfont.font";
    font.tf_YSize = fontsize;
    font.tf_Style = 0;
    font.tf_Flags = FPF_ROMFONT | FPF_PROPORTIONAL | FPF_DESIGNED;
    font.tf_XSize = 10;
    font.tf_Baseline = baseline - 1; /* CHECKME: */
    font.tf_BoldSmear = 1;
    font.tf_Accessors = 0;
    font.tf_LoChar = lochar;
    font.tf_HiChar = hichar;
    font.tf_CharData = gfxdata;
    font.tf_Modulo = gfxwidth / 8;
    font.tf_CharLoc = charloc;
    font.tf_CharSpace = charspace;
    font.tf_CharKern = charkern;
    
    {
    	struct TagItem fonttags[] =
	{
	    {TA_DeviceDPI, (xdpi << 16) | ydpi  },
	    {TAG_DONE	    	    	    	}
	};
    	ExtendFont(&font, fonttags);
    }

    AddFont(&font);
    
    printf("Font ready!\n");
    Wait(SIGBREAKF_CTRL_C);
    
    RemFont(&font);
    StripFont(&font);
    
}

/****************************************************************************************/

int main(void)
{
    getarguments();
    openlibs();
    openfontfile();
    openotagfile();
    openfontengine();
    setupfontengine();
    getglyphmaps();
    makefontbitmap();
    makefont();
    
    //showfontbitmap();
    
    cleanup(0);
    
    return 0;
}

/****************************************************************************************/
