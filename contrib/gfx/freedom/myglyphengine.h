
#ifndef MYGLYPHENGINE_H
#define MYGLYPHENGINE_H

#ifndef  DISKFONT_GLYPH_H
#include <diskfont/glyph.h>
#endif

#include "freetype.h"
#include "myglyphrequest.h"

struct MyGlyphEngine
{
	struct Library			*gle_Library;
	STRPTR					gle_Name;
	TT_Engine				gle_Engine;
	TT_Face					gle_Face;
	TT_Instance				gle_Instance;
	TT_Glyph					gle_Glyph;
	TT_CharMap				gle_CharMap;

	TT_Face_Properties	gle_Face_Properties;
	TT_Instance_Metrics	gle_Instance_Metrics;
	TT_Glyph_Metrics		gle_Glyph_Metrics;
	TT_Raster_Map			gle_Raster_Map;
	TT_Matrix				gle_Matrix;
	TT_Outline				gle_Outline;
	BOOL						gle_FontOK;

	struct MyGlyphReq		gle_GlyphRequest;
	char						gle_OTagName[300];
	char						gle_FontName[300];
};

#define MYGE(x) ((struct MyGlyphEngine *)(x))

#endif /* MYGLYPHENGINE_H */


