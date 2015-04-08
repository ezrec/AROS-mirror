//========================================================================
//
// TrueType.cc - TrueType fonts handling
//
// Copyright 1999-2001 Emmanuel Lesueur
//
//========================================================================


#ifdef __GNUC__
#pragma implementation
#endif

#define DB(x) //x

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#include <setjmp.h>
#include "gmem.h"
#include "GString.h"
#include "Object.h"
#include "Stream.h"
#include "GfxState.h"
#include "GfxFont.h"
#include "FontFile.h"
#include "GlobalParams.h"
#include "Error.h"
#include "TextOutputDev.h"
#include "FontOutputDev.h"
#include "AOutputDev.h"
#include "type1/pictures.h"
#include "freetype/ftnameid.h"

class Engine {
public:
  Engine() : engineOk(false) {
    if(TT_Init_FreeType(&engine)/* || TT_Init_Post_Extension(engine)*/)
      printf("Can't initialize TrueType engine\n");
    else
      engineOk = true;
  }
  ~Engine() {
    if (engineOk)
      TT_Done_FreeType(engine);
  }
  bool isOk() const { return engineOk; }
  operator TT_Engine () const { return engine; }
private:
  Engine(const Engine&);
  Engine& operator = (const Engine&);
  TT_Engine engine;
  bool engineOk;
};

Engine engine;


extern "C" {
  extern jmp_buf stck_state;
  extern XYspace *IDENTITY;
  char *t1_get_abort_message(int);
}
extern FontEncoding macRomanEncoding;
extern FontEncoding winAnsiEncoding;

//------------------------------------------------------------------------
// TTBaseFont
//------------------------------------------------------------------------

TTBaseFont::TTBaseFont(const char *fileName, char **fontEnc, GBool pdfFontHasEncoding) :
  BaseFont(fileName), ok(gFalse), buf(NULL),
  numGlyphs(0)
{
  char tmpName[32];
  TT_UShort unicodeCmap, macRomanCmap, msSymbolCmap;
  TT_UShort platform, encoding, i;
  int j;

  if (!engine.isOk())
    return;

  fileName = cloneBuf(fileName, tmpName);
  if (TT_Error err = TT_Open_Face(engine, fileName, &face)) {
    error(-1, "TT: error 0x%lx opening font \"%s\"", err, *fileName == '\001' ? getName() : fileName);
    return;
  }

  /*TT_String *str;
  TT_UShort len;
  if (!TT_Get_Name_String(face, TT_NAME_ID_FULL_NAME, &str, &len)) {
    name.append(str,len);
  }*/

  if (TT_Error err = TT_Get_Face_Properties(face, &props))
    error(-1, "TT: Error 0x%lx looking for face properties in font \"%s\".", err, getName());

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
  unicodeCmap = macRomanCmap = msSymbolCmap = 0xffff;
  for (i = 0; i < props.num_CharMaps; ++i) {
    if (!TT_Get_CharMap_ID(face, i, &platform, &encoding)) {
      if (platform == 3 && encoding == 1) {
	unicodeCmap = i;
      } else if (platform == 1 && encoding == 0) {
	macRomanCmap = i;
      } else if (platform == 3 && encoding == 0) {
	msSymbolCmap = i;
      }
    }
  }
  i = 0;
  mode = ttFontModeCharCode;
  charMapOffset = 0;
  if (pdfFontHasEncoding) {
    if (unicodeCmap != 0xffff) {
      i = unicodeCmap;
      mode = ttFontModeUnicode;
    } else if (macRomanCmap != 0xffff) {
      i = macRomanCmap;
      mode = ttFontModeCodeMap;
      codeMap = (Guchar *)gmalloc(256 * sizeof(Guchar));
      for (j = 0; j < 256; ++j) {
	if (fontEnc[j]) {
	  codeMap[j] = (Guchar)globalParams->getMacRomanCharCode(fontEnc[j]);
	} else {
	  codeMap[j] = 0;
	}
      }
    }
  } else {
    if (macRomanCmap != 0xffff) {
      i = macRomanCmap;
      mode = ttFontModeCharCode;
    } else if (msSymbolCmap != 0xffff) {
      i = msSymbolCmap;
      mode = ttFontModeCharCodeOffset;
      charMapOffset = 0xf000;
    }
  }
  TT_Get_CharMap(face, i, &charMap);


#if 0
  glyphIndex = new Guchar[props.num_Glyphs];
  memset(glyphIndex, 0, props.num_Glyphs * sizeof(*glyphIndex));

  int i;
  int charMapNum = -1;
  for (i = 0; i < properties.num_CharMaps; ++i) {
    TT_UShort platformID, encodingID;
    if (TT_Error err = TT_Get_CharMap_ID(face, i, &platformID, &encodingID))
      error(-1, "TT: Error 0x%lx looking for charmap", err);
    else if (platformID == TT_PLATFORM_MACINTOSH &&
	     encodingID == TT_MAC_ID_ROMAN) {
      if (!stdEnc || encID == encMacRoman) {
	stdEnc = &macRomanEncoding;
	charMapNum = i;
      }
    } else if (platformID == TT_PLATFORM_MICROSOFT &&
	       encodingID == TT_MS_ID_UNICODE_CS) {
      if (!stdEnc || encID == encWinAnsi) {
	stdEnc = &winAnsiEncoding;
	charMapNum = i;
      }
    }
  }

  if (stdEnc == NULL) {
    if (TT_Error err = TT_Load_PS_Names(face, &post)) {
      error(-1, "TT: error 0x%lx looking for char names in font \"%s\".", err, getName());
      return;
    }
  } else {
    if (TT_Error err = TT_Get_CharMap(face, charMapNum, &charMap)) {
      error(-1, "TT: error 0x%lx looking for CharMap in font \"%s\".", err, getName());
      return;
    }
  }
#endif

  ok = gTrue;
}

TTBaseFont::TTBaseFont(const char *fileName,
		       Gushort *cidToGIDA, int cidToGIDLenA) :
  BaseFont(fileName), ok(gFalse), buf(NULL),
  numGlyphs(0)
{
  char tmpName[32];

  if (!engine.isOk())
    return;

  fileName = cloneBuf(fileName, tmpName);

  cidToGID = cidToGIDA;
  cidToGIDLen = cidToGIDLenA;
  if (TT_Error err = TT_Open_Face(engine, fileName, &face)) {
    error(-1, "TT: error 0x%lx opening font \"%s\"", err, *fileName == '\001' ? getName() : fileName);
    return;
  }

  if (TT_Error err = TT_Get_Face_Properties(face, &props))
    error(-1, "TT: Error 0x%lx looking for face properties in font \"%s\".", err, getName());

  mode = ttFontModeCIDToGIDMap;
  ok = gTrue;
}

TTBaseFont::~TTBaseFont() {
  clearCache();
  if (ok)
    TT_Close_Face(face);
  gfree(buf);
  if (codeMap) {
    gfree(codeMap);
  }
}

const char *TTBaseFont::cloneBuf(const char *fileName, char *tmpname) {
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
    size_t len = y;
    buf = (char*)gmalloc(len);
    memcpy(buf, oldbuf, len);

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
    x = len;
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

GBool TTBaseFont::isOk() {
  return ok;
}

#if 0
void TTBaseFont::loadEncoding() {
  if (stdEnc) {
    for (int i = 0; i < 256; ++i)
      if (char *name = stdEnc->getCharName(i))
	encoding->addChar(i, copyString((char*)name));
  } else {
    if (TT_Error err = TT_Get_CharMap(face, 0, &charMap)) {
      error(-1, "TT: error 0x%lx looking for CharMap in font \"%s\".", err, getName());
      return;
    }
    for (int i = 0; i < 256; ++i) {
      if (TT_UShort index = TT_Char_Index(charMap, i)) {
	TT_String *name;
	if (TT_Error err = TT_Get_PS_Name(face, index, &name))
	  error(-1, "TT: error 0x%lx looking for glyph name", err);
	else if(strcmp(name, ".notdef")) {
	  encoding->addChar(i, copyString((char*)name));
	}
      }
    }
  }
}

void TTBaseFont::buildCharMap(FontEncoding *enc, short *map) {
  int n = 0;

  if (stdEnc) {
    for (int i = 0; i < 256; ++i) {
      if (TT_UShort index = TT_Char_Index(charMap, i)) {
	if (char *name = stdEnc->getCharName(i)) {
	  int code = enc->getCharCode(name);
	  if (code >= 0 && code < 256) {
	    map[code] = index;
	    glyphIndex[index] = n;
	    ++n;
	  }
	}
      }
    }
  } else {
    for (int i = 0; i < properties.num_Glyphs; ++i) {
      TT_String *name;
      if (TT_Error err = TT_Get_PS_Name(face, i, &name))
	error(-1, "TT: error 0x%lx looking for glyph name", err);
      else if (strcmp(name, ".notdef")) {
	int code = enc->getCharCode(name);
	if (code >= 0 && code < 256) {
	  map[code] = i;
	  glyphIndex[i] = n;
	  ++n;
	}
      }
    }
  }
  if (n == 0)
    error(-1, "TT: No supported encoding in \"%s\"", getName());
  numGlyphs = n;
}
#endif

OutputFont *TTBaseFont::create(double m11, double m12, double m21, double m22) {
  return new TTOutputFont(this, m11, m12, m21, m22);
}

//------------------------------------------------------------------------
// TTOutputFont
//------------------------------------------------------------------------

TTOutputFont::TTOutputFont(TTBaseFont *base1, double m11, double m12, double m21, double m22):
  AOutputFont(base1, m11, m12, m21, m22), ok(gFalse),
  charSpaceLocal(NULL), glyphs(NULL)
{
  TT_Error err;
  //TT_Instance_Metrics metrics;
  //int x, xMin, xMax;
  //int y, yMin, yMax;
  int i;
  double sz = sqrt(m21 * m21 + m22 * m22);

  sz *= 64;

  if ((err = TT_New_Instance(base1->face, &instance)) ||
      (err = TT_Set_Instance_Resolutions(instance, 72, 72)) ||
      (err = TT_Set_Instance_CharSize(instance, /*1000 * 64*/TT_F26Dot6(sz))) /*||
      (err = TT_Get_Instance_Metrics(instance, &metrics))*/) {
    error(-1, "TT: error 0x%lx creating instance of font \"%s\"", err, base1->getName());
    return;
  }

  if (int err = setjmp(stck_state)) {
    error(-1, "T1 Error: %s\n", t1_get_abort_message(err));
    return;
  }

  // transform the four corners of the font bounding box -- the min
  // and max values form the bounding box of the transformed font
  /*x = (int)((m[0] * props.header->xMin + m[2] * props.header->yMin) *
	    0.001 * metrics.x_ppem / props.header->Units_Per_EM);
  xMin = xMax = x;
  y = (int)((m[1] * props.header->xMin + m[3] * props.header->yMin) *
	    0.001 * metrics.x_ppem / props.header->Units_Per_EM);
  yMin = yMax = y;
  x = (int)((m[0] * props.header->xMin + m[2] * props.header->yMax) *
	    0.001 * metrics.x_ppem / props.header->Units_Per_EM);
  if (x < xMin) {
    xMin = x;
  } else if (x > xMax) {
    xMax = x;
  }
  y = (int)((m[1] * props.header->xMin + m[3] * props.header->yMax) *
	    0.001 * metrics.x_ppem / props.header->Units_Per_EM);
  if (y < yMin) {
    yMin = y;
  } else if (y > yMax) {
    yMax = y;
  }
  x = (int)((m[0] * props.header->xMax + m[2] * props.header->yMin) *
	    0.001 * metrics.x_ppem / props.header->Units_Per_EM);
  if (x < xMin) {
    xMin = x;
  } else if (x > xMax) {
    xMax = x;
  }
  y = (int)((m[1] * props.header->xMax + m[3] * props.header->yMin) *
	    0.001 * metrics.x_ppem / props.header->Units_Per_EM);
  if (y < yMin) {
    yMin = y;
  } else if (y > yMax) {
    yMax = y;
  }
  x = (int)((m[0] * props.header->xMax + m[2] * props.header->yMax) *
	    0.001 * metrics.x_ppem / props.header->Units_Per_EM);
  if (x < xMin) {
    xMin = x;
  } else if (x > xMax) {
    xMax = x;
  }
  y = (int)((m[1] * props.header->xMax + m[3] * props.header->yMax) *
	    0.001 * metrics.x_ppem / props.header->Units_Per_EM);
  if (y < yMin) {
    yMin = y;
  } else if (y > yMax) {
    yMax = y;
  }
  xOffset = -xMin;
  yOffset = -yMin;

  // compute the transform matrix
  matrix.xx = (TT_Fixed)(m[0] * 65.536);
  matrix.yx = (TT_Fixed)(m[1] * 65.536);
  matrix.xy = (TT_Fixed)(m[2] * 65.536);
  matrix.yy = (TT_Fixed)(m[3] * 65.536);*/

  // setup charSpaceLocal for this font
  charSpaceLocal = (XYspace *) Permanent(Transform(IDENTITY,
    m11 / sz, m12 / sz, m21 / sz, m22 / sz));

  // allocate the glyph array
  glyphs = new Glyph [base1->numGlyphs];
}

TTOutputFont::~TTOutputFont() {
  if (glyphs) {
    delete [] glyphs;
    TT_Done_Instance(instance);
  }
  if (charSpaceLocal)
    KillSpace(charSpaceLocal);
}

GBool TTOutputFont::isOk() {
  return glyphs != NULL;
}


// taken from ttf2pfb (from Chun-Yu Lee) in the FreeType package
static segment *curveto(segment *path, XYspace *space,
			TT_Outline& outline,
			TT_F26Dot6 x, TT_F26Dot6 y,
			int s, int e,
			TT_F26Dot6& lastx, TT_F26Dot6& lasty) {
  int  N, i;
  TT_F26Dot6 sx[3], sy[3], cx[4], cy[4];

  N = e - s + 1;
  cx[0] = lastx; cy[0] = lasty;
  if (s == e) {
    cx[1] = (2 * outline.points[s].x + outline.points[s - 1].x) / 3;
    cy[1] = (2 * outline.points[s].y + outline.points[s - 1].y) / 3;
    cx[2] = (2 * outline.points[s].x + x) / 3;
    cy[2] = (2 * outline.points[s].y + y) / 3;
    cx[3] = x;
    cy[3] = y;
    path = Join(path, Bezier(Loc(space, cx[1] - cx[0], cy[1] - cy[0]),
			     Loc(space, cx[2] - cx[0], cy[2] - cy[0]),
			     Loc(space, cx[3] - cx[0], cy[3] - cy[0])));
  } else {
    for (i = 0; i < N; i++) {
      sx[0] = i == 0 ? outline.points[s - 1].x :
	(outline.points[i + s].x + outline.points[i + s - 1].x) / 2;
      sy[0] = i == 0 ? outline.points[s - 1].y :
	(outline.points[i + s].y + outline.points[i + s - 1].y) / 2;
      sx[1] = outline.points[s + i].x;
      sy[1] = outline.points[s + i].y;
      sx[2] = i == N - 1 ? x : (outline.points[s + i].x + outline.points[s + i + 1].x) / 2;
      sy[2] = i == N - 1 ? y : (outline.points[s + i].y + outline.points[s + i + 1].y) / 2;
      cx[1] = (2 * sx[1] + sx[0]) / 3;
      cy[1] = (2 * sy[1] + sy[0]) / 3;
      cx[2] = (2 * sx[1] + sx[2]) / 3;
      cy[2] = (2 * sy[1] + sy[2]) / 3;
      cx[3] = sx[2];
      cy[3] = sy[2];
      path = Join(path, Bezier(Loc(space, cx[1] - cx[0], cy[1] - cy[0]),
			       Loc(space, cx[2] - cx[0], cy[2] - cy[0]),
			       Loc(space, cx[3] - cx[0], cy[3] - cy[0])));
      cx[0] = cx[3]; cy[0] = cy[3];
    }
  }
  lastx = x;
  lasty = y;
}

int TTOutputFont::getCharIndex(CharCode c, Unicode u) {
  int idx = 0;
  TTBaseFont *fontFile = (TTBaseFont *)base;

  switch (fontFile->mode) {
  case ttFontModeUnicode:
    idx = TT_Char_Index(fontFile->charMap, (TT_UShort)u);
    break;
  case ttFontModeCharCode:
    idx = TT_Char_Index(fontFile->charMap, (TT_UShort)c);
    break;
  case ttFontModeCharCodeOffset:
    idx = TT_Char_Index(fontFile->charMap,
			(TT_UShort)(c + fontFile->charMapOffset));
    break;
  case ttFontModeCodeMap:
    if (c <= 0xff) {
      idx = TT_Char_Index(fontFile->charMap,
			  (TT_UShort)(fontFile->codeMap[c] & 0xff));
    } else {
      idx = 0;
    }
    break;
  case ttFontModeCIDToGIDMap:
    if (fontFile->cidToGIDLen) {
      if ((int)c < fontFile->cidToGIDLen) {
	idx = fontFile->cidToGID[c];
      } else {
	idx = 0;
      }
    } else {
      idx = c;
    }
    break;
  }

  return idx;
}

segment *TTOutputFont::getT1Outline(int idx) {
  Glyph& g = glyphs[idx];
  if (g.outline)
    return g.outline;

  TT_Glyph glyph;
  if (TT_New_Glyph(((TTBaseFont*)base)->face, &glyph))
    return NULL;

  TT_Outline outline;
  TT_Error err;
  if ((err = TT_Load_Glyph(instance, glyph, idx, TTLOAD_DEFAULT)) ||
      (err = TT_Get_Glyph_Outline(glyph, &outline))) {
    error(-1, "TT: error 0x%lx loading glyph %d in font \"%s\"", err, idx, base->getName());
    TT_Done_Glyph(glyph);
    return NULL;
  }

  // The following converts the ttf outline into a type1 outline.
  // This was adapted from Chun-Yu Lee's ttf2pfb.
  XYspace *space = charSpaceLocal;
  segment *path = NULL;
  int i, j;
  int start_offpt, end_offpt = 0, fst;
  TT_Pos lastx = 0;
  TT_Pos lasty = 0;

  for (i = 0, j = 0; i < outline.n_contours; i++) {
    fst = j;
    segment *p = Loc(space, outline.points[j].x - lastx, outline.points[j].y - lasty);
    lastx = outline.points[j].x;
    lasty = outline.points[j].y;
    if (path)
      path = Join(path, p);
    else
      path = p;
    j++;

    start_offpt = 0; /*start at least 1*/
    /* data pts for all contours stored in one array.
       each round j init at last j + 1 */

    /* start_offpt means start of off points.
       0 means no off points in record.
       N means the position of the off point.
       end_offpt means the ending off point.
       lastx, lasty is the last ON point from which Curve and Line
       shall start.
     */

    /* start with j=0. into loop, j=1.
       if pt[1] off, if start_offpt == 0, toggle start_offpt
	   next j=2. if on, now start_off != 0, run Curveto.
       if pt[1] on, start_off == 0, will run Lineto.
     */
    for (; j <= outline.contours[i]; j++) {
      if (!(outline.flags[j] & 1)) {
	if (!start_offpt) {
	  start_offpt = end_offpt = j;
	} else
	  end_offpt++;
      } else {                    /*On Curve*/
	if (start_offpt) {
	  /* start_offpt stuck at j, end_offpt++.
	     end_offpt - start_offpt gives no of off pts.
	     start_offpt gives start of sequence.
	     why need outline.xCoord[j] outline.yCoord[j]?
	   */
	  path = curveto(path, space, outline,
			 outline.points[j].x, outline.points[j].y,
			 start_offpt, end_offpt, lastx, lasty);
	  start_offpt = 0;

	  /* also use start_offpt as indicator to save one variable!!
	     after curveto, reset condition. */
	} else {
	  path = Join(path, Line(Loc(space, outline.points[j].x - lastx, outline.points[j].y - lasty)));
	  lastx = outline.points[j].x;
	  lasty = outline.points[j].y;
	}
      }
    }
    /* looks like closepath fst = first, i.e. go back to first */
    if (start_offpt) {
      path = curveto(path, space, outline,
		     outline.points[fst].x, outline.points[fst].y,
		     start_offpt, end_offpt, lastx, lasty);
    }/* else*/ {
      segment *cur = Phantom(path);
      path = ClosePath(path);
      path = Join(Snap(path), cur);
    }
  }
  //if (path)
    //path = ClosePath(path);
  TT_Done_Glyph(glyph);
  g.outline = path;
  return path;
}

region *TTOutputFont::getCharInterior(CharCode c, Unicode u) {
  int idx = getCharIndex(c, u);
  Glyph& g = glyphs[idx];
  region *area = g.region;
  if (!area) {
    if (int err = setjmp(stck_state)) {
      error(-1, "T1 Error: %s\n", t1_get_abort_message(err));
      return NULL;
    }
    if(segment *path = getT1Outline(idx))
      area = Interior(Dup(path), CONTINUITY + WINDINGRULE);
    g.region = area;
  }
  return area;
}

segment *TTOutputFont::getCharOutline(CharCode c, Unicode u) {
  int idx = getCharIndex(c, u);
  Glyph& g = glyphs[idx];
  segment *path = g.outline;
  if (!path) {
    if (int err = setjmp(stck_state)) {
      error(-1, "T1 Error: %s\n", t1_get_abort_message(err));
      return NULL;
    }
    g.outline = path = getT1Outline(idx);
  }
  return path;
}

