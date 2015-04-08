//========================================================================
//
// FreeType.cc - FreeType handling
//
// Copyright 2002-2004 Emmanuel Lesueur
//
//========================================================================


#ifdef __GNUC__
#pragma implementation
#endif

#define USE_FT_INTERNALS 1

#define DB(x) //x

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#include <setjmp.h>
#include <unistd.h>
#include "gmem.h"
#include "gfile.h"
#include "GString.h"
#include "Object.h"
#include "Stream.h"
#include "GfxState.h"
#include "GfxFont.h"
#include "GlobalParams.h"
#include "Error.h"
#include "TextOutputDev.h"
#include "FontOutputDev.h"
#include "AOutputDev.h"
#include "FreeType.h"
#include "fofi/FoFiTrueType.h"
#include FT_SIZES_H
#if USE_FT_INTERNALS //~ cff cid->gid map
#include <freetype/internal/cfftypes.h>
#include <freetype/internal/tttypes.h>
#endif

#define DEBUG_SPANS(x)			;
#define DEBUG_FREETYPE(x)		;
#define DEBUG_APATH_MAKEREGION(x)	;
#define DEBUG_ENCODING(x)		;

extern "C" void dprintf(const char *, ...);
//#define printf dprintf

class Engine {
public:
  Engine() : engineOk(false) {
    if(FT_Init_FreeType(&engine)/* || FT_Init_Post_Extension(engine)*/)
      printf("Can't initialize TrueType engine\n");
    else
      engineOk = true;
  }
  ~Engine() {
    if (engineOk)
      FT_Done_FreeType(engine);
  }
  bool isOk() const { return engineOk; }
  operator FT_Library () const { return engine; }
private:
  Engine(const Engine&);
  Engine& operator = (const Engine&);
  FT_Library engine;
  bool engineOk;
};

Engine engine;


//------------------------------------------------------------------------
// RegionBuilder
//------------------------------------------------------------------------

class RegionBuilder
{
public:
  RegionBuilder(int x, int y, int w, int h);
  ~RegionBuilder();

  ARegion* get() { ARegion* r = region; region = NULL; return r; }

  static void gray_spans(int y, int count, FT_Span* span, void*);

  void* user() { return region; }
private:
  ARegion* region;
};

RegionBuilder::RegionBuilder(int x, int y, int w, int h)
{
  class RegionPtr {
  public:
    RegionPtr(ARegion *r) : ptr(r) {}
    ~RegionPtr() { delete ptr; }
    ARegion *release() { ARegion *r = ptr; ptr = NULL; return r; }
    ARegion *operator -> () { return ptr; }
  private:
    ARegion *ptr;
  };

  RegionPtr r(new ARegion);
  DEBUG_FREETYPE(dprintf("RegionBuilder::ctor: x %d y %d w %d h %d\n",
	x, y, w, h));
  r->x0 = x;
  r->y0 = y;
  r->width = w;
  r->height = h;
  DEBUG_FREETYPE(dprintf("RegionBuilder::ctor: x0 %d y0 %d w %d h %d\n",
	r->x0, r->y0, r->width, r->height));
  r->alloc_rows(h);
  region = r.release();
}

RegionBuilder::~RegionBuilder()
{
  delete region;
}

void RegionBuilder::gray_spans(int y, int count, FT_Span* span, void* user)
{
  DEBUG_SPANS(dprintf("gray_spans: y %d count %d spans %p\n", y, count, span));
  int n = count;
  for (int k = 0; k < count; ++k)
  {
    if (span->len > 255)
    {
      n += (span->len - 1) / 255;
    }
    ++span;
  }
  span -= count;

  ARegion* region = static_cast<ARegion*>(user);
  ARegion::Span* p = region->add_spans(y, n);
  for (int k = 0; k < count; ++k)
  {
    DEBUG_SPANS(dprintf("gray_spans: x %d len %d coverage %d\n",
	  span->x, span->len, span->coverage));
    int len = span->len;
    int x = span->x;
    int coverage = span->coverage;
    while (len > 255)
    {
      p->x = x;
      p->length = 255;
      p->alpha = coverage;
      ++p;
      x += 255;
      len -= 255;
    }
    p->x = x;
    p->length = len;
    p->alpha = coverage;
    ++p;
    ++span;
  }
}


//------------------------------------------------------------------------
// FTBaseFont
//------------------------------------------------------------------------

FTBaseFont::FTBaseFont(const char *fileName, char **fontEnc, GBool pdfFontHasEncoding) :
  BaseFont(fileName), ok(gFalse), buf(NULL), codeMap(NULL)
{
  char *name;
  int unicodeCmap, macRomanCmap, msSymbolCmap;
  int i,j;

  DB(printf("new base font (%s)\n", *fileName == 1 ? "embedded" : fileName));

  if (!openFont(fileName))
    return;

#if USE_FT_INTERNALS
  DB(printf("driver class %s\n", face->driver->root.clazz->module_name));

  if (!strcmp(face->driver->root.clazz->module_name, "type1") ||
      !strcmp(face->driver->root.clazz->module_name, "cff")) {

    mode = ftFontModeCodeMapDirect;
    codeMap = (Guint *)gmalloc(256 * sizeof(Guint));
    for (i = 0; i < 256; ++i) {
      codeMap[i] = 0;
      if ((name = fontEnc[i])) {
	codeMap[i] = FT_Get_Name_Index(face, name);
	DEBUG_ENCODING(printf("fontEnc[%d]=%s -> %d\n", i, name, codeMap[i]));
      }
    }

  } else
#endif
	  if (FT_Select_Charmap(face, ft_encoding_unicode) == 0) {
    mode = ftFontModeUnicode;

  } else {

    // To match up with the Adobe-defined behaviour, we choose a cmap
    // like this:
    // 1. If the PDF font has an encoding:
    //    1a. If the TrueType font has a Microsoft Unicode cmap, use it,
    //        and use the Unicode indexes, not the char codes.
    //    1b. If the TrueType font has a Macintosh Roman cmap, use it,
    //        and reverse map the char names through MacRomanEncoding to
    //        get char codes.
    // 2. If the PDF font does not have an encoding:
    //    2a. If the TrueType font has a Macintosh Roman cmap, use it,
    //        and use char codes directly.
    //    2b. If the TrueType font has a Microsoft Symbol cmap, use it,
    //        and use (0xf000 + char code).
    // 3. If none of these rules apply, use the first cmap and hope for
    //    the best (this shouldn't happen).
#if 1
    unicodeCmap = macRomanCmap = msSymbolCmap = 0xffff;
    for (i = 0; i < face->num_charmaps; ++i) {
      DB(printf("cmap[%d]: platform %d encoding %d\n", i,
	    face->charmaps[i]->platform_id, face->charmaps[i]->encoding_id));
      if (face->charmaps[i]->platform_id == 3 &&
	  face->charmaps[i]->encoding_id == 1) {
	unicodeCmap = i;
      } else if (face->charmaps[i]->platform_id == 1 &&
		 face->charmaps[i]->encoding_id == 0) {
	macRomanCmap = i;
      } else if (face->charmaps[i]->platform_id == 3 &&
		 face->charmaps[i]->encoding_id == 0) {
	msSymbolCmap = i;
      }
    }
    i = 0;
    mode = ftFontModeCharCode;
    charMapOffset = 0;
    DB(printf("unicode %d macroman %d mssymbols %d has_encoding %d\n",
	  unicodeCmap, macRomanCmap, msSymbolCmap, pdfFontHasEncoding));
    if (pdfFontHasEncoding) {
      if (unicodeCmap != 0xffff) {
	i = unicodeCmap;
	mode = ftFontModeUnicode;
      } else if (macRomanCmap != 0xffff) {
	i = macRomanCmap;
	mode = ftFontModeCodeMap;
	codeMap = (Guint *)gmalloc(256 * sizeof(Guint));
	for (j = 0; j < 256; ++j) {
	  if (fontEnc[j]) {
	    codeMap[j] = globalParams->getMacRomanCharCode(fontEnc[j]);
	    DB(printf("%d: macroman[%s]=%d\n", j, fontEnc[j], codeMap[j]));
	  } else {
	    codeMap[j] = 0;
	  }
	}
      }
    } else {
      if (macRomanCmap != 0xffff) {
	i = macRomanCmap;
	mode = ftFontModeCharCode;
      } else if (msSymbolCmap != 0xffff) {
	i = msSymbolCmap;
	mode = ftFontModeCharCodeOffset;
	charMapOffset = 0xf000;
      }
    }

    DB(printf("charmap %d mode %d offset %d\n", i, mode, charMapOffset));
    if (FT_Set_Charmap(face, face->charmaps[i])) {
      DB(printf("Can't set charmap\n"));
      return;
    }
#else
    DB(printf("Can't set charmap\n"));
    return;
#endif
  }

  DB(printf("created base font %p\n", this));
  ok = gTrue;
}

FTBaseFont::FTBaseFont(const char *fileName,
		       Gushort *cidToGIDA, int cidToGIDLenA) :
  BaseFont(fileName), ok(gFalse), buf(NULL), codeMap(NULL)
{
  DB(printf("new cid font (%s, %p, %d)\n", *fileName == 1 ? "embedded" : fileName,
			  cidToGIDA, cidToGIDLenA));
  if (!openFont(fileName))
    return;
  cidToGID = cidToGIDA;
  cidToGIDLen = cidToGIDLenA;
  mode = ftFontModeCIDToGIDMap;
  DB(printf("created base font %p\n", this));
  ok = gTrue;
}

FTBaseFont::FTBaseFont(const char *fileName) :
  BaseFont(fileName), ok(gFalse), buf(NULL), codeMap(NULL)
{
  DB(printf("new cff font (%s)\n", *fileName == 1 ? "embedded" : fileName));
  if (!openFont(fileName))
    return;
  cidToGID = NULL;
  cidToGIDLen = 0;
  mode = ftFontModeCFFCharset;
  DB(printf("created base font %p\n", this));
  ok = gTrue;
}

FTBaseFont::~FTBaseFont() {
  clearCache();
  if (ok)
    FT_Done_Face(face);
  gfree(buf);
  if (codeMap) {
    gfree(codeMap);
  }
  if (tmpFileName) {
    unlink(tmpFileName->getCString());
    delete tmpFileName;
  }
}

static void fileWrite(void *stream, char *data, int len) {
  fwrite(data, 1, len, (FILE *)stream);
}

int FTBaseFont::openFont(const char *fileName) {
  char tmpName[32];

  tmpFileName = NULL;

  if (!engine.isOk()) {
    DB(printf("no engine\n"));
    return 0;
  }

  fileName = cloneBuf(fileName, tmpName);

  if (FT_Error err = FT_New_Face(engine, fileName, 0, &face)) {
    DB(printf("Can't open face (%d)\n", err));
    FT_Done_Face(face);
    // Try to handle missing CMap
    if (err == 0x92) {
      DB(printf("Converting...\n"));
      FILE *f;
      if (openTempFile(&tmpFileName, &f, "wb", NULL)) {
	if (!buf) {
	  myFILE *f2 = myfopen(fileName, "rb");
	  if (f2) {
	    bufLen = f2->size();
	    if (buf = (char*)gmalloc(bufLen)) {
	      myfread(buf, bufLen, 1, f2);
	    }
	    myfclose(f2);
	  }
	}
	if (buf) {
	  FoFiTrueType *ttf = FoFiTrueType::make(buf, bufLen);
	  ttf->writeTTF(fileWrite, f);
	  fclose(f);
	  delete ttf;
	  gfree(buf);
	  buf = NULL;
	  err = FT_New_Face(engine, tmpFileName->getCString(), 0, &face);
	  if (err) {
	    DB(printf("Can't open face (%d)\n", err));
	    FT_Done_Face(face);
	  }
	} else {
	  fclose(f);
	}
      }
    }

    if (err) {
      error(-1, "FT: error 0x%lx opening font \"%s\"", err, *fileName == '\001' ? getName() : fileName);
      return 0;
    }
  }

  DB(printf("face %p\n", face));

  //FT_Select_Charmap(face, ft_encoding_unicode);
  return 1;
}

const char *FTBaseFont::cloneBuf(const char *fileName, char *tmpname) {
  if(fileName[0]=='\001' && fileName[1]==':' && fileName[2]==':') {
    unsigned x = 0;
    fileName += 3;
    int n = 0;
    while (*fileName != ':') {
      x |= *fileName++ - '0' << n;
      n += 3;
    }
    unsigned y = 0;
    ++fileName;
    n = 0;
    while (*fileName != '\0') {
      y |= *fileName++ - '0' << n;
      n += 3;
    }
    char* oldbuf = (char*)x;
    bufLen = y;
    buf = (char*)gmalloc(bufLen);
    memcpy(buf, oldbuf, bufLen);

    char *t = tmpname;
    *t++ = '\001';
    *t++ = ':';
    *t++ = ':';
    x = unsigned(buf);
    while(x) {
	*t++ = char('0' + (x & 7));
	x >>= 3;
    }
    *t++ = ':';
    x = bufLen;
    while(x) {
	*t++ = char('0' + (x & 7));
	x >>= 3;
    }
    *t = '\0';
    fileName = tmpname;
  }

  /*if(myFILE* f=myfopen(fileName,"rb")) {
      char name[256];
      static int num;
      sprintf(name,"sd0:font%d.ttf",++num);
      if(FILE* f2=fopen(name,"wb")) {
	  printf("writing font\n");
	  do {
	    static char buf[1024];
	    int sz=myfread(buf,1,sizeof(buf),f);
	    if(sz<=0)
		break;
	    fwrite(buf,1,sz,f2);
	  } while(true);
	  fclose(f2);
      }
      myfclose(f);
  } */

  return fileName;
}

GBool FTBaseFont::isOk() {
  return ok;
}

OutputFont *FTBaseFont::create(double m11, double m12, double m21, double m22) {
  return new FTOutputFont(this, m11, m12, m21, m22);
}

//------------------------------------------------------------------------
// TTOutputFont
//------------------------------------------------------------------------

FTOutputFont::FTOutputFont(FTBaseFont *base1, double m11, double m12, double m21, double m22):
  AOutputFont(base1, m11, m12, m21, m22), ok(gFalse), glyphs(NULL)
{
  FT_Error err;
  FT_Face face = base1->face;
  double size, div;
  //FT_Instance_Metrics metrics;
  //int x, xMin, xMax;
  //int y, yMin, yMax;
  int i;

  DB(printf("new font %f %f %f %f\n", m11, m12, m21, m22));
  if (FT_New_Size(face, &sizeObj)) {
    DB(printf("Can't create size\n"));
    return;
  }
  face->size = sizeObj;
  size = sqrt(m21*m21 + m22*m22);
  if (FT_Set_Pixel_Sizes(face, 0, (int)size)) {
    DB(printf("Can't set size\n"));
    return;
  }

  matrix.xx = FT_Fixed(m11 * 65536 / size);
  matrix.yx = FT_Fixed(m21 * 65536 / size);
  matrix.xy = FT_Fixed(m12 * 65536 / size);
  matrix.yy = FT_Fixed(m22 * 65536 / size);

  // allocate the glyph array
  glyphs = new Glyph [face->num_glyphs];

  DB(printf("created output font %p from base %p\n", this, base1));
}

FTOutputFont::~FTOutputFont() {
  if (glyphs) {
    delete [] glyphs;
  }
}

GBool FTOutputFont::isOk() {
  return glyphs != NULL;
}

int FTOutputFont::getCharIndex(CharCode c, Unicode u) {
  int idx = 0;
  FTBaseFont *fontFile = (FTBaseFont *)base;
  //DB(printf("getCharIndex(%d, %d) mode %d\n", c, u, fontFile->mode));
  switch (fontFile->mode) {
  case ftFontModeUnicode:
    idx = FT_Get_Char_Index(fontFile->face, (FT_ULong)u);
    break;
  case ftFontModeCharCode:
    idx = FT_Get_Char_Index(fontFile->face, (FT_ULong)c);
    break;
  case ftFontModeCharCodeOffset:
    idx = FT_Get_Char_Index(fontFile->face,
			    (FT_ULong)(c + fontFile->charMapOffset));
    break;
  case ftFontModeCodeMap:
    if (c <= 0xff) {
      idx = FT_Get_Char_Index(fontFile->face, (FT_ULong)fontFile->codeMap[c]);
      //idx = FT_Get_Char_Index(fontFile->face, (FT_ULong)c);
    } else {
      idx = 0;
    }
    break;
  case ftFontModeCodeMapDirect:
    if (c <= 0xff) {
      idx = (FT_UInt)fontFile->codeMap[c];
    } else {
      idx = 0;
    }
    break;
  case ftFontModeCIDToGIDMap:
    if (fontFile->cidToGIDLen) {
      if ((int)c < fontFile->cidToGIDLen) {
	idx = (FT_UInt)fontFile->cidToGID[c];
      } else {
	idx = (FT_UInt)0;
      }
    } else {
      idx = (FT_UInt)c;
    }
    break;
  case ftFontModeCFFCharset:
#if USE_FT_INTERNALS //~ cff cid->gid map
    int j;
    CFF_Font cff = (CFF_Font)((TT_Face)fontFile->face)->extra.data;
    idx = 0;
    for (j = 0; j < (int)cff->num_glyphs; ++j) {
      if (cff->charset.sids[j] == c) {
	idx = j;
	break;
      }
    }
#endif
    break;
  }
  //DB(printf("index %d\n", idx));
  return idx;
}


ARegion *FTOutputFont::getCharInterior(CharCode c, Unicode u) {
  int idx = getCharIndex(c, u);
  Glyph& g = glyphs[idx];
  ARegion *area = g.region;
  if (!area) {
    FT_Face face = ((FTBaseFont *)base)->face;
    DB(printf("font %p face %p char %d u %d idx %d\n", this, face, c, u, idx));
    face->size = sizeObj;
    FT_Set_Transform(face, &matrix, NULL);
    FT_Error err = FT_Load_Glyph(face, idx, FT_LOAD_NO_HINTING | FT_LOAD_NO_BITMAP);
    if (err == 0) {
      FT_BBox cbox;
      //FT_Glyph glyph;

      /*if (FT_Get_Glyph(face->glyph, &glyph) == 0)*/ {
	 if (face->glyph->format == ft_glyph_format_outline) {
	   //FT_Outline& outline = ((FT_OutlineGlyph)glyph)->outline;
	   FT_Outline& outline = face->glyph->outline;

	   FT_Outline_Get_CBox(&outline, &cbox);

	   cbox.xMin >>= 6;
	   cbox.yMin >>= 6;
	   cbox.xMax += 63;
	   cbox.yMax += 63;
	   cbox.xMax >>= 6;
	   cbox.yMax >>= 6;

	   RegionBuilder region_builder(
	       -cbox.xMin,
	       -cbox.yMin,
	       cbox.xMax - cbox.xMin,
	       cbox.yMax - cbox.yMin);

	   FT_Bitmap bitmap;
	   bitmap.width = 1;
	   bitmap.rows = 1;
	   bitmap.pitch = 1;
	   bitmap.num_grays = 256;
	   bitmap.pixel_mode = ft_pixel_mode_grays;

	   FT_Raster_Params params;
	   params.target = &bitmap;
	   params.flags = ft_raster_flag_aa | ft_raster_flag_direct;
	   params.gray_spans = RegionBuilder::gray_spans;
	   params.user = region_builder.user();

#if 0
	   printf("points %d contours %d flags 0x%x\n", outline.n_points, outline.n_contours, outline.flags);
	   for (int k = 0; k < outline.n_points; ++k)
	     printf("\t%f, %f (%d)\n",
		 outline.points[k].x / 64.0,
		 outline.points[k].y / 64.0,
		 outline.tags[k]);
	   for (int k = 0; k < outline.n_contours; ++k)
	     printf("\t%d\n", outline.contours[k]);
#endif
	   FT_Outline_Render(engine, &outline, &params);

	   area = region_builder.get();
	 } else {
	   DB(printf("not an outline glyph\n"));
	 }
	 //FT_Done_Glyph(glyph);
      }
    } else {
      DB(printf("can't load glyph: %d\n", err));
    }

    DB(printf("area %p\n", area));
    g.region = area;
  }
  return area;
}

APath::Part *FTOutputFont::getCharOutline(CharCode c, Unicode u) {
  int idx = getCharIndex(c, u);
  //Glyph& g = glyphs[idx];
  //APath *path = g.outline;
  APath::Part *p = NULL;
  //if (!path) {
    FT_Face face = ((FTBaseFont *)base)->face;
    DB(printf("font %p face %p char %d u %d idx %d\n", this, face, c, u, idx));
    face->size = sizeObj;
    FT_Set_Transform(face, &matrix, NULL);
    FT_Error err = FT_Load_Glyph(face, idx, FT_LOAD_NO_HINTING | FT_LOAD_NO_BITMAP);
    if (err == 0) {
      FT_BBox cbox;
      if (face->glyph->format == ft_glyph_format_outline) {
	FT_Outline& outline = face->glyph->outline;

#if 0
	printf("points %d contours %d flags 0x%x\n", outline.n_points, outline.n_contours, outline.flags);
	int n = outline.contours[0];
	int j = 0;
	for (int k = 0; k < outline.n_points; ++k) {
	  printf("\t%f, %f (%d)\n",
	      outline.points[k].x / 64.0,
	      outline.points[k].y / 64.0,
	      outline.tags[k]);
	  if (k == n) {
	    printf("\t-----------\n");
	    n = outline.contours[++j];
	  }
	}
	//for (int k = 0; k < outline.n_contours; ++k)
	//  printf("\t%d\n", outline.contours[k]);
#endif
	int i = 0;
	double x = 0;
	double y = 0;

	for (int k = 0; k < outline.n_contours; ++k) {
	  int i0 = i;
	  double x0 = outline.points[i].x / 64.0;
	  double y0 = outline.points[i].y / 64.0;
	  APath::Part *q = APath::move(x0 - x, y0 - y);
	  x = x0;
	  y = y0;
	  int j = outline.contours[k];
	  while (++i <= j) {
	    double x1 = outline.points[i].x / 64.0;
	    double y1 = outline.points[i].y / 64.0;
	    APath::Part *t;
	    if (outline.tags[i] == FT_Curve_Tag_On) {
	      t = APath::line(x1 - x, y1 - y);
	    } else {
	      double x2 = outline.points[i + 1].x / 64.0;
	      double y2 = outline.points[i + 1].y / 64.0;
	      if (outline.tags[i] == FT_Curve_Tag_Cubic) {
		int l = i + 2 > j ? i0 : i + 2;
		double x3 = outline.points[l].x / 64.0;
		double y3 = outline.points[l].y / 64.0;
		i += 2;
		t = APath::bezier(x1 - x, y1 - y, x2 - x, y2 - y, x3 - x, y3 - y);
		if (l == i0) {
		  x1 = x2;
		  y1 = y2;
		} else {
		  x1 = x3;
		  y1 = y3;
		}
	      } else {
		if (outline.tags[i + 1] == FT_Curve_Tag_On) {
		  ++i;
		} else {
		  x2 = (x1 + x2) / 2;
		  y2 = (y1 + y2) / 2;
		}
		t = APath::bezier(x1 - x, y1 - y, x2 - x, y2 - y);
		x1 = x2;
		y1 = y2;
	      }
	    }
	    q = APath::join(q, t);
	    x = x1;
	    y = y1;
	  }
	  q = APath::close(q);
	  p = APath::join(p, q);
	  x = x0;
	  y = y0;
	  i = j + 1;
	}

	//path = new APath(p);
	//APath::print(p);
      }
    }
    //g.outline = path;
  //}
  //return path;
  return p;
}


  class Counter : public APath::Scanner
  {
    public:
      Counter() : num_points(0), num_contours(0), ok(false) {}

      void open(APath::Part*) { ok = true; }
      void move(double, double) { if (ok) ++num_points; }
      void line(double, double) { if (ok) ++num_points; }
      void bezier(double, double, double, double) { if (ok) num_points += 2; }
      void bezier(double, double, double, double, double, double) { if (ok) num_points += 3; }
      void close() { if (ok) ++num_contours; ok = false; }
      int num_points;
      int num_contours;
      bool ok;
  };

  class Builder : public APath::Scanner
  {
    public:
      Builder(FT_Outline& o) : outline(o), ok(false), cur_x(0), cur_y(0), point(0), contour(0) {}

      void open(APath::Part*) { ok = true; }
      void move(double x, double y)
      {
	if(ok)
	{
	  x += cur_x;
	  y += cur_y;
	  outline.points[point].x = FT_Pos(x * 64);
	  outline.points[point].y = FT_Pos(y * 64);
	  outline.tags[point] = 1;
	  ++point;
	  cur_x = x;
	  cur_y = y;
	}
      }
      void line(double x, double y)
      {
	if (ok)
	{
	  x += cur_x;
	  y += cur_y;
	  outline.points[point].x = FT_Pos(x * 64);
	  outline.points[point].y = FT_Pos(y * 64);
	  outline.tags[point] = 1;
	  ++point;
	  cur_x = x;
	  cur_y = y;
	}
      }
      void bezier(double x1, double y1, double x2, double y2)
      {
	if (ok)
	{
	  outline.points[point].x = FT_Pos((x1 + cur_x) * 64);
	  outline.points[point].y = FT_Pos((y1 + cur_y) * 64);
	  outline.points[point + 1].x = FT_Pos((x2 + cur_x) * 64);
	  outline.points[point + 1].y = FT_Pos((y2 + cur_y) * 64);
	  outline.tags[point] = 0;
	  outline.tags[point + 1] = 1;
	  point += 2;
	  cur_x += x2;
	  cur_y += y2;
	}
      }
      void bezier(double x1, double y1, double x2, double y2, double x3, double y3)
      {
	if (ok)
	{
	  outline.points[point].x = FT_Pos((x1 + cur_x) * 64);
	  outline.points[point].y = FT_Pos((y1 + cur_y) * 64);
	  outline.points[point + 1].x = FT_Pos((x2 + cur_x) * 64);
	  outline.points[point + 1].y = FT_Pos((y2 + cur_y) * 64);
	  outline.points[point + 2].x = FT_Pos((x3 + cur_x) * 64);
	  outline.points[point + 2].y = FT_Pos((y3 + cur_y) * 64);
	  outline.tags[point] = 2;
	  outline.tags[point + 1] = 2;
	  outline.tags[point + 2] = 1;
	  point += 3;
	  cur_x += x3;
	  cur_y += y3;
	}
      }
      void close()
      {
	if (ok)
	{
	  outline.contours[contour] = point - 1;
	  ++contour;
	  ok = false;
	}
      }

    private:
      double cur_x, cur_y;
      FT_Outline& outline;
      int contour;
      int point;
      bool ok;
  };

ARegion* APath::make_region(bool eo) const
{
  ARegion* region = NULL;

  DEBUG_APATH_MAKEREGION(dprintf("APath::make_region: path %p\n", this));

  Counter counter;
  counter.scan(this);

  DEBUG_APATH_MAKEREGION(dprintf("APath::make_region: points %d contours %d\n",
	counter.num_contours, counter.num_points));

  if (counter.num_contours && counter.num_points)
  {
    FT_Outline outline;

    if (FT_Outline_New(engine, counter.num_points, counter.num_contours, &outline) == 0)
    {
      DEBUG_APATH_MAKEREGION(dprintf("APath::make_region: outline ok\n"));

      Builder builder(outline);
      builder.scan(this);
      //outline.flags |= ft_outline_high_precision;

      if (eo)
	outline.flags |= ft_outline_even_odd_fill;

      FT_BBox cbox;
      FT_Outline_Get_CBox(&outline, &cbox);

      cbox.xMin >>= 6;
      cbox.yMin >>= 6;
      cbox.xMax += 63;
      cbox.yMax += 63;
      cbox.xMax >>= 6;
      cbox.yMax >>= 6;

      RegionBuilder region_builder(-cbox.xMin,
	  -cbox.yMin,
	  cbox.xMax - cbox.xMin,
	  cbox.yMax - cbox.yMin);

      FT_Bitmap bitmap;
      bitmap.width = 100;
      bitmap.rows = 100;
      bitmap.pitch = 100;
      bitmap.num_grays = 256;
      bitmap.pixel_mode = ft_pixel_mode_grays;

      FT_Raster_Params params;
      params.target = &bitmap;
      params.flags = ft_raster_flag_aa | ft_raster_flag_direct;
      params.gray_spans = RegionBuilder::gray_spans;
      params.user = region_builder.user();

#if 0
      printf("points %d contours %d flags 0x%x\n", outline.n_points, outline.n_contours, outline.flags);
      for (int k = 0; k < outline.n_points; ++k)
	printf("\t%f, %f (%d)\n",
	    outline.points[k].x / 64.0,
	    outline.points[k].y / 64.0,
	    outline.tags[k]);
      for (int k = 0; k < outline.n_contours; ++k)
	printf("\t%d\n", outline.contours[k]);

#endif
      //int t=outline.n_contours;
      //outline.n_contours = 1;
      FT_Outline_Render(engine, &outline, &params);
      //outline.n_contours = t;

      region = region_builder.get();
      /*if (region)
	{
	region->y0 = region->height - 1;
	}*/
#if  0
      printf("region: x %d y %d w %d h %d\n", region->x0, region->y0, region->width, region->height);
      for (int k = 0; k < region->height; ++k)
      {
	ARegion::Row* r = region->rows[k];
	if (r)
	{
	  printf("%03d: %d", k, r->count);
	  for (int i = 0; i < r->count; ++i)
	    printf(" [%d %d %d]",
		r->spans[i].x,
		r->spans[i].length,
		r->spans[i].alpha);
	  printf("\n");
	}
      }
#endif

      FT_Outline_Done(engine, &outline);
    }
  }

  return region;
}
