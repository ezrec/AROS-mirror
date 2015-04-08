//========================================================================
//
// GfxFont.h
//
// Copyright 1996-2001 Derek B. Noonburg
//
//========================================================================

#ifndef GFXFONT_H
#define GFXFONT_H

#ifdef __GNUC__
#pragma interface
#endif

#include "gtypes.h"
#include "GString.h"
#include "Object.h"
#include "CharTypes.h"

class Dict;
class CMap;
class CharCodeToUnicode;
struct GfxFontCIDWidths;

//------------------------------------------------------------------------
// GfxFontType
//------------------------------------------------------------------------

enum GfxFontType {
  //----- Gfx8BitFont
  fontUnknownType,
  fontType1,
  fontType1C,
  fontType3,
  fontTrueType,
  //----- GfxCIDFont
  fontCIDType0,
  fontCIDType0C,
  fontCIDType2
};

//------------------------------------------------------------------------
// GfxFontCIDWidths
//------------------------------------------------------------------------

struct GfxFontCIDWidthExcep {
  CID first;			// this record applies to
  CID last;			//   CIDs <first>..<last>
  double width;			// char width
};

struct GfxFontCIDWidthExcepV {
  CID first;			// this record applies to
  CID last;			//   CIDs <first>..<last>
  double height;		// char height
  double vx, vy;		// origin position
};

struct GfxFontCIDWidths {
  double defWidth;		// default char width
  double defHeight;		// default char height
  double defVY;			// default origin position
  GfxFontCIDWidthExcep *exceps;	// exceptions
  int nExceps;			// number of valid entries in exceps
  GfxFontCIDWidthExcepV *	// exceptions for vertical font
    excepsV;
  int nExcepsV;			// number of valid entries in excepsV
};

//------------------------------------------------------------------------
// GfxFont
//------------------------------------------------------------------------

#define fontFixedWidth (1 << 0)
#define fontSerif      (1 << 1)
#define fontSymbolic   (1 << 2)
#define fontScript     (1 << 3)
#define fontStdCharSet (1 << 5)
#define fontItalic     (1 << 6)
#define fontAllCap     (1 << 16)
#define fontSmallCap   (1 << 17)
#define fontBold       (1 << 18)

class BaseFont;

class GfxFont {
public:

  // Build a GfxFont object.
  static GfxFont *makeFont(XRef *xref, Ref idA, Dict *fontDict);

  GfxFont(Ref idA);

  virtual ~GfxFont();

  GBool isOk() { return ok; }

  // Get font tag.
  //GString *getTag() { return tag; }

  // Get font dictionary ID.
  Ref *getID() { return &id; }

  // Does this font match the tag?
  //GBool matches(char *tagA) { return !tag->cmp(tagA); }

  // Get base font name.
  GString *getName() { return name; }

  // Get original base font name.
  GString *getOrigName() { return origName; }

  // Get font type.
  GfxFontType getType() { return type; }
  virtual GBool isCIDFont() { return gFalse; }

  // Get original font type.
  GfxFontType getOrigType() { return origType; }

  // Get embedded font ID, i.e., a ref for the font file stream.
  // Returns false if there is no embedded font.
  GBool getEmbeddedFontID(Ref *embID)
    { *embID = embFontID; return embFontID.num >= 0; }

  // Get the PostScript font name for the embedded font.  Returns
  // NULL if there is no embedded font.
  char *getEmbeddedFontName()
    { return embFontName ? embFontName->getCString() : (char *)NULL; }

  // Get the name of the external font file.  Returns NULL if there
  // is no external font file.
  GString *getExtFontFile() { return extFontFile; }

  // Get font descriptor flags.
  GBool isFixedWidth() { return flags & fontFixedWidth; }
  GBool isSerif() { return flags & fontSerif; }
  GBool isSymbolic() { return flags & fontSymbolic; }
  GBool isItalic() { return flags & fontItalic; }
  GBool isBold() { return flags & fontBold; }
  int getFlags() { return flags; }

  // Return the font matrix.
  double *getFontMatrix() { return fontMat; }

  // Return the font bounding box.
  double *getFontBBox() { return fontBBox; }

  // Return the ascent and descent values.
  double getAscent() { return ascent; }
  double getDescent() { return descent; }

  // Read an external or embedded font file into a buffer.
  char *readExtFontFile(int *len);
  char *readEmbFontFile(XRef *xref, int *len);

  // Get the next char from a string <s> of <len> bytes, returning the
  // char <code>, its Unicode mapping <u>, its displacement vector
  // (<dx>, <dy>), and its origin offset vector (<ox>, <oy>).  <uSize>
  // is the number of entries available in <u>, and <uLen> is set to
  // the number actually used.  Returns the number of bytes used by
  // the char code.
  virtual int getNextChar(char *s, int len, CharCode *code,
			  Unicode *u, int uSize, int *uLen,
			  double *dx, double *dy, double *ox, double *oy) = 0;

  BaseFont *getBaseFont() { return base; }

protected:

  void readFontDescriptor(XRef *xref, Dict *fontDict);
  CharCodeToUnicode *readToUnicodeCMap(Dict *fontDict, int nBits);
  void GfxFont::findExtFontFile();

  //GString *tag;		// PDF font tag
  Ref id;			// reference (used as unique ID)
  GString *name;		// font name
  GString *origName;		// original font name
  GfxFontType type;		// type of font
  GfxFontType origType;		// original font type
  int flags;			// font descriptor flags
  GString *embFontName;		// name of embedded font
  Ref embFontID;		// ref to embedded font file stream
  GString *extFontFile;		// external font file name
  double fontMat[6];		// font matrix (Type 3 only)
  double fontBBox[4];		// font bounding box
  double missingWidth;		// "default" width
  double ascent;		// max height above baseline
  double descent;		// max depth below baseline
  GBool ok;
  BaseFont *base;
};

//------------------------------------------------------------------------
// Gfx8BitFont
//------------------------------------------------------------------------

class Gfx8BitFont: public GfxFont {
public:

  Gfx8BitFont(XRef *xref, Ref idA, GfxFontType typeA, Dict *fontDict);

  virtual ~Gfx8BitFont();

  virtual int getNextChar(char *s, int len, CharCode *code,
			  Unicode *u, int uSize, int *uLen,
			  double *dx, double *dy, double *ox, double *oy);

  // Return the encoding.
  char **getEncoding() { return enc; }

  // Return the Unicode map.
  CharCodeToUnicode *getToUnicode();

  // Return the character name associated with <code>.
  char *getCharName(int code) { return enc[code]; }

  // Returns true if the PDF font specified an encoding.
  GBool getHasEncoding() { return hasEncoding; }

  // Get width of a character or string.
  double getWidth(Guchar c) { return widths[c]; }

  // Return the Type 3 CharProc for the character associated with <code>.
  Object *getCharProc(int code, Object *proc);

  // Substitute a font
  void substitute(GString *name1, GfxFontType type1);

private:

  char *enc[256];		// char code --> char name
  char encFree[256];		// boolean for each char name: if set,
				//   the string is malloc'ed
  CharCodeToUnicode *ctu;	// char code --> Unicode
  GBool hasEncoding;
  double widths[256];		// character widths
  Object charProcs;		// Type3 CharProcs dictionary
};

//------------------------------------------------------------------------
// GfxCIDFont
//------------------------------------------------------------------------

class GfxCIDFont: public GfxFont {
public:

  GfxCIDFont(XRef *xref, Ref idA, Dict *fontDict);

  virtual ~GfxCIDFont();

  virtual GBool isCIDFont() { return gTrue; }

  virtual int getNextChar(char *s, int len, CharCode *code,
			  Unicode *u, int uSize, int *uLen,
			  double *dx, double *dy, double *ox, double *oy);

  // Return the Unicode map.
  CharCodeToUnicode *getToUnicode();

  // Get the collection name (<registry>-<ordering>).
  GString *getCollection();

  // Return the CID-to-GID mapping table.  These should only be called
  // if type is fontCIDType2.
  Gushort *getCIDToGID() { return cidToGID; }
  int getCIDToGIDLen() { return cidToGIDLen; }

private:

  CMap *cMap;			// char code --> CID
  CharCodeToUnicode *ctu;	// CID --> Unicode
  GfxFontCIDWidths widths;	// character widths
  Gushort *cidToGID;		// CID --> GID mapping (for embedded
				//   TrueType fonts)
  int cidToGIDLen;
};

//------------------------------------------------------------------------
// GfxFontDict
//------------------------------------------------------------------------

class GfxFontDict {
public:

  // Build the font dictionary, given the PDF font dictionary.
  GfxFontDict(XRef *xref, Dict *fontDict);

  // Destructor.
  ~GfxFontDict();

  // Get the specified font.
  GfxFont *lookup(char *tag);

  // Iterative access.
  int getNumFonts() { return numFonts; }
  GfxFont *getFont(int i) { return fonts[i].gfxFont; }

private:

  struct Entry {
    GString *tag;
    GfxFont *gfxFont;
  };
  Entry *fonts;			// list of fonts
  int numFonts;			// number of fonts
};

#endif
