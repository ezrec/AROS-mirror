/* ttfinstall - install truetype font to a specified directory/file */

#include <freetype/freetype.h>
extern  TT_Engine    engine;    /* must already be open! */

extern char base_fontname[25];
extern char font_filename[32];
extern char otag_filename[32];
extern char family[256];
extern char install_dir[256];
extern char install_font[256];
extern char install_otag[256];
extern char family_bold_filename[256];
extern char family_italic_filename[256];
extern char family_bold_italic_filename[256];

extern long SpaceWidth;
extern long SymbolSet;
extern long YSizeFactor;
extern long IsFixed;
extern long SlantStyle;
extern long StemWeight;
extern long HorizStyle;
extern long SerifFlag;
extern long InhibitAlgoStyle;

/* font info we grab when font is open to reference later */
extern char font_nametable[9][256];
extern ULONG advance_Width_Max;
extern ULONG Units_Per_EM;
extern long num_Faces;
extern long num_Glyphs;
extern long yMin, yMax;
extern long xMin, xMax;
extern long Lowest_Rec_PPEM;
extern long Ascender;
extern long Descender;
extern long Line_Gap;
extern long fsType;
extern long sTypoAscender;
extern long sTypoDescender;
extern long sTypoLineGap;
extern long usWinAscent;
extern long usWinDescent;
extern long MONO_SpaceWidth;
extern long PROP_SpaceWidth;
extern long num_glyphs;
extern long KernPairs;
extern long usFirstCharIndex;
extern long calc_yMin, calc_yMax;

extern unsigned short int codepage[256];
extern int save_codepage;
extern int metric_source;

extern UWORD availsizelist[];

extern char message_buffer[256];

int initialize_freetype(void);
int terminate_freetype(void);

extern BOOL TTF_InstallFont(char *filename);
extern void set_base_fontnames(char *filename, char *fontname );
extern void make_clean_basename(char *fontname);
extern int set_properties_tags(void);
extern int  ttfinst(char *filename, char *fontname);
extern int sanctify_to_dir(char *to);
extern int calc_ymin_ymax(char *filename);

/* freetype related items */

extern  TT_Engine    engine;    /* must already be open! */

static  TT_Error     error;

extern  TT_Face      face;
extern  TT_Instance  instance;
extern  TT_Glyph     glyph;

extern  TT_Instance_Metrics  imetrics;
extern  TT_Outline           outline;
extern  TT_Glyph_Metrics     metrics;

extern TT_Face_Properties   properties;

extern  TT_CharMap  cmap;


/* extra spec tags
Spec1 is full filename of ttf file
Spec2 is custom code page (Indirect) or Zero (direct)
Spec3 is custom transform (Indirect) or Zero (direct)
Spec4 is metric source
Spec5 is custom y metrics
*/
#define  OT_Spec2	(OT_Level1 | 0x102)
#define  OT_Spec3	(OT_Level1 | 0x103)
#define  OT_Spec4	(OT_Level1 | 0x104)
#define  OT_Spec5	(OT_Level1 | 0x105)
