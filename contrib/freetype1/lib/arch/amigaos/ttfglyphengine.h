#ifndef _ARCH_AMIGAOS_TTFGLYPHENGINE_H
#define _ARCH_AMIGAOS_TTFGLYPHENGINE_H

#include "freetype.h"
#include "extend/ftxkern.h"  /* kerning       */

struct TTF_GlyphEngine_ {

	/* diskfont standard */
	struct Library *gle_Library;	/* should be our lib base */
	char	*gle_Name;		/* library name "ttf" */

	/* ttf.library specific - private data */
	
	/* freetype specific entites */
	TT_Engine		engine;

	TT_Face			face;
	TT_CharMap		char_map;
	TT_Face_Properties	properties;
	
	TT_Instance		instance;
	TT_Instance_Metrics	imetrics;

	TT_Glyph		glyph;
	TT_Glyph_Metrics	metrics;
	TT_Outline		outline;

	TT_Kerning		kerning_dir;
	TT_Kern_Subtable	*kern_subtable;
	long			KernPairs;

	/* ttf.library current request/state */
	int  last_error;
	int  face_established;
	char ttf_filename[256];
	char base_filename[256];
	char bold_filename[256];
	char italic_filename[256];
	char bold_italic_filename[256];

	unsigned long cmap_index;
	unsigned long requested_cmap;
	unsigned short  platform, encoding;
	int  request_char;
	int  request_char2;
	int  glyph_code;

	int  instance_changed;
	int  point_size;
	int  corrected_upem;
	int  metric_source;
	int  metric_custom;
	int  xres, yres;
	int  hinted;
	
	TT_Fixed hold_sin, hold_cos;

	int  do_rotate;
	TT_Matrix rotate_matrix;
	
	int  do_shear;
	TT_Matrix shear_matrix;

	int  do_embold;
	int  embold_x, embold_y;

	int  bold_sig, italic_sig;

	unsigned short int codepage[256];

/*	struct GlyphMap		GMap0; */
	struct GlyphMap		*GMap;
/*	void			*GlyphPool; */

	
};

typedef struct TTF_GlyphEngine_ TTF_GlyphEngine ;

/* flag that we need to pick an encoding table */
#define NO_CMAP_SET 0xFFFFFFFF

TTF_GlyphEngine *AllocGE(void);
void             FreeGE(TTF_GlyphEngine *);
int              set_last_error(TTF_GlyphEngine *, int);
int              ChooseEncoding(TTF_GlyphEngine *);
int              SetInstance(TTF_GlyphEngine *);
void             set_default_codepage(TTF_GlyphEngine *);

#endif /* _ARCH_AMIGAOS_TTFGLYPHENGINE_H */
