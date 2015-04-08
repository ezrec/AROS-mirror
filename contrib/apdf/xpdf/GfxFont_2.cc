//========================================================================
//
// GfxFont.cc
//
// Copyright 1996-2001 Derek B. Noonburg
//
//========================================================================

#ifdef __GNUC__
#pragma implementation
#endif

#include <aconf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "gmem.h"
#include "Error.h"
#include "Object.h"
#include "Dict.h"
#include "GlobalParams.h"
#include "CMap.h"
#include "CharCodeToUnicode.h"
#include "FontEncoding.h"
#include "FontEncodingTables.h"
#include "BuiltinFontTables.h"
#include "FontFile.h"
#include "GfxFont.h"
#include "FontMap.h"
#include "FontCache.h"
#include "goo/gFile.h"

//------------------------------------------------------------------------

struct StdFontMapEntry {
  char *altName;
  char *properName;
};

static StdFontMapEntry stdFontMap[] = {
  { "Arial",                        "Helvetica" },
  { "Arial,Bold",                   "Helvetica-Bold" },
  { "Arial,BoldItalic",             "Helvetica-BoldOblique" },
  { "Arial,Italic",                 "Helvetica-Oblique" },
  { "Arial-Bold",                   "Helvetica-Bold" },
  { "Arial-BoldItalic",             "Helvetica-BoldOblique" },
  { "Arial-BoldItalicMT",           "Helvetica-BoldOblique" },
  { "Arial-BoldMT",                 "Helvetica-Bold" },
  { "Arial-Italic",                 "Helvetica-Oblique" },
  { "Arial-ItalicMT",               "Helvetica-Oblique" },
  { "ArialMT",                      "Helvetica" },
  { "Courier,Bold",                 "Courier-Bold" },
  { "Courier,Italic",               "Courier-Oblique" },
  { "Courier,BoldItalic",           "Courier-BoldOblique" },
  { "CourierNew",                   "Courier" },
  { "CourierNew,Bold",              "Courier-Bold" },
  { "CourierNew,BoldItalic",        "Courier-BoldOblique" },
  { "CourierNew,Italic",            "Courier-Oblique" },
  { "CourierNew-Bold",              "Courier-Bold" },
  { "CourierNew-BoldItalic",        "Courier-BoldOblique" },
  { "CourierNew-Italic",            "Courier-Oblique" },
  { "CourierNewPS-BoldItalicMT",    "Courier-BoldOblique" },
  { "CourierNewPS-BoldMT",          "Courier-Bold" },
  { "CourierNewPS-ItalicMT",        "Courier-Oblique" },
  { "CourierNewPSMT",               "Courier" },
  { "Helvetica,Bold",               "Helvetica-Bold" },
  { "Helvetica,BoldItalic",         "Helvetica-BoldOblique" },
  { "Helvetica,Italic",             "Helvetica-Oblique" },
  { "Helvetica-BoldItalic",         "Helvetica-BoldOblique" },
  { "Helvetica-Italic",             "Helvetica-Oblique" },
  { "TimesNewRoman",                "Times-Roman" },
  { "TimesNewRoman,Bold",           "Times-Bold" },
  { "TimesNewRoman,BoldItalic",     "Times-BoldItalic" },
  { "TimesNewRoman,Italic",         "Times-Italic" },
  { "TimesNewRoman-Bold",           "Times-Bold" },
  { "TimesNewRoman-BoldItalic",     "Times-BoldItalic" },
  { "TimesNewRoman-Italic",         "Times-Italic" },
  { "TimesNewRomanPS",              "Times-Roman" },
  { "TimesNewRomanPS-Bold",         "Times-Bold" },
  { "TimesNewRomanPS-BoldItalic",   "Times-BoldItalic" },
  { "TimesNewRomanPS-BoldItalicMT", "Times-BoldItalic" },
  { "TimesNewRomanPS-BoldMT",       "Times-Bold" },
  { "TimesNewRomanPS-Italic",       "Times-Italic" },
  { "TimesNewRomanPS-ItalicMT",     "Times-Italic" },
  { "TimesNewRomanPSMT",            "Times-Roman" }
};

//------------------------------------------------------------------------
// GfxFont
//------------------------------------------------------------------------

GfxFont *GfxFont::makeFont(XRef *xref, Ref idA, Dict *fontDict) {
  GString *nameA;
  GfxFont *font;
  Object obj1;

  // get base font name
  nameA = NULL;
  fontDict->lookup("BaseFont", &obj1);
  if (obj1.isName()) {
    nameA = new GString(obj1.getName());
  }
  obj1.free();

  // get font type
  font = NULL;
  fontDict->lookup("Subtype", &obj1);
  if (obj1.isName("Type1") || obj1.isName("MMType1")) {
    font = new Gfx8BitFont(xref, idA, fontType1, fontDict);
  } else if (obj1.isName("Type1C")) {
    font = new Gfx8BitFont(xref, idA, fontType1C, fontDict);
  } else if (obj1.isName("Type3")) {
    font = new Gfx8BitFont(xref, idA, fontType3, fontDict);
  } else if (obj1.isName("TrueType")) {
    font = new Gfx8BitFont(xref, idA, fontTrueType, fontDict);
  } else if (obj1.isName("Type0")) {
    font = new GfxCIDFont(xref, idA, fontDict);
  } else {
    error(-1, "Unknown font type: '%s'",
	  obj1.isName() ? obj1.getName() : "???");
    font = new Gfx8BitFont(xref, idA, fontUnknownType, fontDict);
  }
  obj1.free();

  if (font && !font->isOk()) {
    delete font;
    font = NULL;
  }

  return font;
}

GfxFont::GfxFont(Ref idA) {
  ok = gFalse;
  //tag = new GString(tagA);
  id = idA;
  name = NULL;
  origName = NULL;
  base = NULL;
  embFontName = NULL;
  extFontFile = NULL;
}

GfxFont::~GfxFont() {
  //delete tag;
  if (base) {
    if (getType() == fontType3)
      delete base;
    else
      baseFontCache->release(base);
  }
  if (name) {
    delete name;
  }
  if (origName) {
    delete origName;
  }
  if (embFontName) {
    delete embFontName;
  }
  if (extFontFile) {
    delete extFontFile;
  }
}

void GfxFont::readFontDescriptor(XRef *xref, Dict *fontDict) {
  Object obj1, obj2, obj3, obj4;
  double t;
  int i;

  // assume Times-Roman by default (for substitution purposes)
  flags = fontSerif;

  embFontID.num = -1;
  embFontID.gen = -1;
  missingWidth = 0;

  if (fontDict->lookup("FontDescriptor", &obj1)->isDict()) {

    // get flags
    if (obj1.dictLookup("Flags", &obj2)->isInt()) {
      flags = obj2.getInt();
    }
    obj2.free();

    // get name
    obj1.dictLookup("FontName", &obj2);
    if (obj2.isName()) {
      embFontName = new GString(obj2.getName());
    }
    obj2.free();

    // look for embedded font file
    if (obj1.dictLookupNF("FontFile", &obj2)->isRef()) {
      if (type == fontType1) {
	embFontID = obj2.getRef();
      } else {
	error(-1, "Mismatch between font type and embedded font file");
      }
    }
    obj2.free();
    if (embFontID.num == -1 &&
	obj1.dictLookupNF("FontFile2", &obj2)->isRef()) {
      if (type == fontTrueType || type == fontCIDType2) {
	embFontID = obj2.getRef();
      } else {
	error(-1, "Mismatch between font type and embedded font file");
      }
    }
    obj2.free();
    if (embFontID.num == -1 &&
	obj1.dictLookupNF("FontFile3", &obj2)->isRef()) {
      if (obj2.fetch(xref, &obj3)->isStream()) {
	obj3.streamGetDict()->lookup("Subtype", &obj4);
	if (obj4.isName("Type1")) {
	  if (type == fontType1) {
	    embFontID = obj2.getRef();
	  } else {
	    error(-1, "Mismatch between font type and embedded font file");
	  }
	} else if (obj4.isName("Type1C")) {
	  if (type == fontType1) {
	    type = fontType1C;
	    embFontID = obj2.getRef();
	  } else if (type == fontType1C) {
	    embFontID = obj2.getRef();
	  } else {
	    error(-1, "Mismatch between font type and embedded font file");
	  }
	} else if (obj4.isName("TrueType")) {
	  if (type == fontTrueType) {
	    embFontID = obj2.getRef();
	  } else {
	    error(-1, "Mismatch between font type and embedded font file");
	  }
	} else if (obj4.isName("CIDFontType0C")) {
	  if (type == fontCIDType0) {
	    type = fontCIDType0C;
	    embFontID = obj2.getRef();
	  } else {
	    error(-1, "Mismatch between font type and embedded font file");
	  }
	} else {
	  error(-1, "Unknown embedded font type '%s'",
		obj4.isName() ? obj4.getName() : "???");
	}
	obj4.free();
      }
      obj3.free();
    }
    obj2.free();

    // look for MissingWidth
    obj1.dictLookup("MissingWidth", &obj2);
    if (obj2.isNum()) {
      missingWidth = obj2.getNum();
    }
    obj2.free();

    // get Ascent and Descent
    obj1.dictLookup("Ascent", &obj2);
    if (obj2.isNum()) {
      t = 0.001 * obj2.getNum();
      // some broken font descriptors set ascent and descent to 0
      if (t != 0) {
	ascent = t;
      }
    }
    obj2.free();
    obj1.dictLookup("Descent", &obj2);
    if (obj2.isNum()) {
      t = 0.001 * obj2.getNum();
      // some broken font descriptors set ascent and descent to 0
      if (t != 0) {
	descent = t;
      }
    }
    obj2.free();

    // font FontBBox
    if (obj1.dictLookup("FontBBox", &obj2)->isArray()) {
      for (i = 0; i < 4 && i < obj2.arrayGetLength(); ++i) {
	if (obj2.arrayGet(i, &obj3)->isNum()) {
	  fontBBox[i] = 0.001 * obj3.getNum();
	}
	obj3.free();
      }
    }
    obj2.free();

  }
  obj1.free();
}

CharCodeToUnicode *GfxFont::readToUnicodeCMap(Dict *fontDict, int nBits) {
  CharCodeToUnicode *ctu;
  GString *buf;
  Object obj1;
  int c;

  if (!fontDict->lookup("ToUnicode", &obj1)->isStream()) {
    obj1.free();
    return NULL;
  }
  buf = new GString();
  obj1.streamReset();
  while ((c = obj1.streamGetChar()) != EOF) {
    buf->append(c);
  }
  obj1.streamClose();
  obj1.free();
  ctu = CharCodeToUnicode::parseCMap(buf, nBits);
  delete buf;
  return ctu;
}

void GfxFont::findExtFontFile() {
#if 0
  if (name) {
    if (type == fontType1) {
      extFontFile = globalParams->findFontFile(name, ".pfa", ".pfb");
    } else if (type == fontTrueType) {
      extFontFile = globalParams->findFontFile(name, ".ttf", NULL);
    }
  }
#else
  if (name && (type == fontType1 || type == fontTrueType) &&
      embFontID.num == -1) {
    char **path;
    myFILE *f;
    for (path = globalParams->getFontPath(); *path; ++path) {
      GString *fileName;
      fileName = appendToPath(new GString(*path), name->getCString());
      f = myfopen(fileName->getCString(), "rb");
      if (!f) {
	if (type == fontType1) {
	  fileName->append(".pfb");
	  f = myfopen(fileName->getCString(), "rb");
	  if (!f) {
	    fileName->del(fileName->getLength() - 4, 4);
	    fileName->append(".pfa");
	    f = myfopen(fileName->getCString(), "rb");
	  }
	} else {
	  fileName->append(".ttf");
	  f = myfopen(fileName->getCString(), "rb");
	}
      }
      if (f) {
	myfclose(f);
	extFontFile = fileName;
	break;
      }
      delete fileName;
    }
  }
#endif
}

#if 0
char *GfxFont::readExtFontFile(int *len) {
  myFILE *f;
  char *buf;

  if (!(f = myfopen(extFontFile->getCString(), "rb"))) {
    error(-1, "External font file '%s' vanished", extFontFile->getCString());
    return NULL;
  }
  myfseek(f, 0, SEEK_END);
  *len = (int)myftell(f);
  myfseek(f, 0, SEEK_SET);
  buf = (char *)gmalloc(*len);
  if ((int)myfread(buf, 1, *len, f) != *len) {
    error(-1, "Error reading external font file '%s'", extFontFile);
  }
  myfclose(f);
  return buf;
}
#endif

char *GfxFont::readEmbFontFile(XRef *xref, int *len) {
  char *buf;
  Object obj1, obj2;
  Stream *str;
  int c;
  int size, i;

  obj1.initRef(embFontID.num, embFontID.gen);
  obj1.fetch(xref, &obj2);
  if (!obj2.isStream()) {
    error(-1, "Embedded font file is not a stream");
    obj2.free();
    obj1.free();
    embFontID.num = -1;
    return NULL;
  }
  str = obj2.getStream();

  buf = NULL;
  i = size = 0;
  str->reset();
  while ((c = str->getChar()) != EOF) {
    if (i == size) {
      size += 4096;
      buf = (char *)grealloc(buf, size);
    }
    buf[i++] = c;
  }
  *len = i;
  str->close();

  obj2.free();
  obj1.free();

  return buf;
}

//------------------------------------------------------------------------
// Gfx8BitFont
//------------------------------------------------------------------------

Gfx8BitFont::Gfx8BitFont(XRef *xref, Ref idA, GfxFontType typeA, Dict *fontDict):
  GfxFont(idA)
{
  BuiltinFont *builtinFont;
  char **baseEnc;
  EncodingID encID;
  FontEncoding *baseEncFromFontFile;
  char *buf;
  int len;
  FontFile *fontFile;
  int code, code2;
  char *charName;
  GBool missing, hex;
  Unicode toUnicode[256];
  double mul;
  int firstChar, lastChar;
  Gushort w;
  Object obj1, obj2, obj3;
  int n, i, a, b, m;

  type = typeA;
  ctu = NULL;

  // Acrobat 4.0 and earlier substituted Base14-compatible fonts
  // without providing Widths and a FontDescriptor, so we munge the
  // names into the proper Base14 names.  (This table is from
  // implementation note 44 in the PDF 1.4 spec.)
  if (name) {
    a = 0;
    b = sizeof(stdFontMap) / sizeof(StdFontMapEntry);
    // invariant: stdFontMap[a].altName <= name < stdFontMap[b].altName
    while (b - a > 1) {
      m = (a + b) / 2;
      if (name->cmp(stdFontMap[m].altName) >= 0) {
	a = m;
      } else {
	b = m;
      }
    }
    if (!name->cmp(stdFontMap[a].altName)) {
      delete name;
      name = new GString(stdFontMap[a].properName);
    }
  }

  // is it a built-in font?
  builtinFont = NULL;
  if (name) {
    for (i = 0; i < nBuiltinFonts; ++i) {
      if (!name->cmp(builtinFonts[i].name)) {
	builtinFont = &builtinFonts[i];
	break;
      }
    }
  } else {
    char buf[32];
    sprintf(buf,"Unnamed-%d-%d",id.gen,id.num);
    name = new GString(buf);
  }
  origName = name->copy();

  // default ascent/descent values
  if (builtinFont) {
    ascent = 0.001 * builtinFont->ascent;
    descent = 0.001 * builtinFont->descent;
    fontBBox[0] = 0.001 * builtinFont->bbox[0];
    fontBBox[1] = 0.001 * builtinFont->bbox[1];
    fontBBox[2] = 0.001 * builtinFont->bbox[2];
    fontBBox[3] = 0.001 * builtinFont->bbox[3];
  } else {
    ascent = 0.95;
    descent = -0.35;
    fontBBox[0] = fontBBox[1] = fontBBox[2] = fontBBox[3] = 0;
  }

  // get info from font descriptor
  readFontDescriptor(xref, fontDict);

  // look for font substitution
  if (!docFontMap || !docFontMap->substitute(this))
    globalFontMap.substitute(this);

  // look for an external font file
  findExtFontFile();

  // if we have an embedded font that we can't handle, or
  // no external font file, do a property-based substitution.
  if (!extFontFile && type != fontType3 &&
      ((type != fontType1 && type != fontType1C &&
	type != fontTrueType) || embFontID.num == -1)) {
    int f = flags, oldf = -1;
    int pass = 4;
    GfxFontType t;
    const char *substName = NULL;
    do {
      switch (pass) {
      case 1:
	f &= ~fontScript;
      case 2:
	f &= ~fontAllCap;
      case 3:
	f &= ~fontSmallCap;
      case 4:
	if (f != oldf) {
	  substName = ::getFont(f, t);
	  if (substName) {
	    substitute(new GString((char*)substName), t);
	    findExtFontFile();
	  }
	  oldf = f;
	}
	break;
      }
    } while (!extFontFile && --pass);

    //if (!extFontFile)
    //  error(-1, "Can't find font \"%s\".", origName->getCString());
  }

  // get font matrix
  fontMat[0] = fontMat[3] = 1;
  fontMat[1] = fontMat[2] = fontMat[4] = fontMat[5] = 0;
  if (fontDict->lookup("FontMatrix", &obj1)->isArray()) {
    for (i = 0; i < 6 && i < obj1.arrayGetLength(); ++i) {
      if (obj1.arrayGet(i, &obj2)->isNum()) {
	fontMat[i] = obj2.getNum();
      }
      obj2.free();
    }
  }
  obj1.free();

  // get Type3 font definition
  if (type == fontType3) {
    fontDict->lookup("CharProcs", &charProcs);
    if (!charProcs.isDict()) {
      error(-1, "Missing or invalid CharProcs dictionary in Type 3 font");
      charProcs.free();
    }
  }

  //----- build the font encoding -----

  // Encodings start with a base encoding, which can come from
  // (in order of priority):
  //   1. FontDict.Encoding or FontDict.Encoding.BaseEncoding
  //        - MacRoman / MacExpert / WinAnsi / Standard
  //   2. embedded or external font file
  //   3. default:
  //        - builtin --> builtin encoding
  //        - TrueType --> MacRomanEncoding
  //        - others --> StandardEncoding
  // and then add a list of differences (if any) from
  // FontDict.Encoding.Differences.

  // check FontDict for base encoding
  hasEncoding = gFalse;
  baseEnc = NULL;
  encID = encUnknown;
  baseEncFromFontFile = NULL;
  fontDict->lookup("Encoding", &obj1);
  if (obj1.isDict()) {
    obj1.dictLookup("BaseEncoding", &obj2);
    if (obj2.isName("MacRomanEncoding")) {
      hasEncoding = gTrue;
      baseEnc = macRomanEncoding;
      encID = encMacRoman;
    } else if (obj2.isName("MacExpertEncoding")) {
      hasEncoding = gTrue;
      baseEnc = macExpertEncoding;
      encID = encMacExpert;
    } else if (obj2.isName("WinAnsiEncoding")) {
      hasEncoding = gTrue;
      baseEnc = winAnsiEncoding;
      encID = encWinAnsi;
    } else if (obj2.isName("StandardEncoding")) {
      hasEncoding = gTrue;
      baseEnc = standardEncoding;
      encID = encStandard;
    }
    obj2.free();
  } else if (obj1.isName("MacRomanEncoding")) {
    hasEncoding = gTrue;
    baseEnc = macRomanEncoding;
    encID = encMacRoman;
  } else if (obj1.isName("MacExpertEncoding")) {
    hasEncoding = gTrue;
    baseEnc = macExpertEncoding;
    encID = encMacExpert;
  } else if (obj1.isName("WinAnsiEncoding")) {
    hasEncoding = gTrue;
    baseEnc = winAnsiEncoding;
    encID = encWinAnsi;
  } else if (obj1.isName("StandardEncoding")) {
    hasEncoding = gTrue;
    baseEnc = standardEncoding;
    encID = encStandard;
  }

  // check embedded or external font file for base encoding
  fontFile = NULL;
  buf = NULL;
#if 1
  if ((type == fontType1 || type == fontType1C || type == fontTrueType) &&
      (extFontFile || embFontID.num >= 0)) {
    base = baseFontCache->getFont(xref, extFontFile ? extFontFile->getCString() : NULL,
				  this, encID);
    if (base && !baseEnc) {
      baseEncFromFontFile = base->getEncoding(gFalse);
    }
  } else if (type == fontType3) {
    base = new T3BaseFont(xref, fontDict);
    if (!base->isOk()) {
      delete base;
      base = NULL;
    }
  }
#else
  if ((type == fontType1 || type == fontType1C || type == fontTrueType) &&
      (extFontFile || embFontID.num >= 0)) {
    if (extFontFile) {
      buf = readExtFontFile(&len);
    } else {
      buf = readEmbFontFile(xref, &len);
    }
    if (buf) {
      if (type == fontType1) {
	fontFile = new Type1FontFile(buf, len);
      } else if (type == fontType1C) {
	fontFile = new Type1CFontFile(buf, len);
      } else {
	fontFile = new TrueTypeFontFile(buf, len);
      }
      if (fontFile->getName()) {
	if (embFontName) {
	  delete embFontName;
	}
	embFontName = new GString(fontFile->getName());
      }
      if (!baseEnc) {
	baseEnc = fontFile->getEncoding();
	baseEncFromFontFile = gTrue;
      }
      gfree(buf);
    }
  }
#endif

  // get default base encoding
  if (!baseEnc) {
    if (builtinFont) {
      baseEnc = builtinFont->defaultBaseEnc;
    } else if (type == fontTrueType) {
      baseEnc = macRomanEncoding;
    } else {
      baseEnc = standardEncoding;
    }
  }

  // copy the base encoding
  if (baseEncFromFontFile) {
    for (i = 0; i < 256; ++i) {
      enc[i] = copyString(baseEncFromFontFile->getCharName(i));
      encFree[i] = gTrue;
    }
  } else {
    for (i = 0; i < 256; ++i) {
      enc[i] = baseEnc[i];
      encFree[i] = gFalse;
    }
  }

  // merge differences into encoding
  if (obj1.isDict()) {
    obj1.dictLookup("Differences", &obj2);
    if (obj2.isArray()) {
      code = 0;
      for (i = 0; i < obj2.arrayGetLength(); ++i) {
	obj2.arrayGet(i, &obj3);
	if (obj3.isInt()) {
	  code = obj3.getInt();
	} else if (obj3.isName()) {
	  if (code < 256) {
	    if (encFree[code]) {
	      gfree(enc[code]);
	    }
	    enc[code] = copyString(obj3.getName());
	    encFree[code] = gTrue;
	  }
	  ++code;
	} else {
	  error(-1, "Wrong type in font encoding resource differences (%s)",
		obj3.getTypeName());
	}
	obj3.free();
      }
    }
    obj2.free();
  }
  obj1.free();
  if (fontFile) {
    delete fontFile;
  }

  //----- build the mapping to Unicode -----

  // look for a ToUnicode CMap
  if (!(ctu = readToUnicodeCMap(fontDict, 8))) {

    // no ToUnicode CMap, so use the char names

    // pass 1: use the name-to-Unicode mapping table
    missing = hex = gFalse;
    for (code = 0; code < 256; ++code) {
      if ((charName = enc[code])) {
	if (!(toUnicode[code] = globalParams->mapNameToUnicode(charName)) &&
	    strcmp(charName, ".notdef")) {
	  // if it wasn't in the name-to-Unicode table, check for a
	  // name that looks like 'Axx' or 'xx', where 'A' is any letter
	  // and 'xx' is two hex digits
	  if ((strlen(charName) == 3 &&
	       isalpha(charName[0]) &&
	       isxdigit(charName[1]) && isxdigit(charName[2]) &&
	       ((charName[1] >= 'a' && charName[1] <= 'f') ||
		(charName[1] >= 'A' && charName[1] <= 'F') ||
		(charName[2] >= 'a' && charName[2] <= 'f') ||
		(charName[2] >= 'A' && charName[2] <= 'F'))) ||
	      (strlen(charName) == 2 &&
	       isxdigit(charName[0]) && isxdigit(charName[1]) &&
	       ((charName[0] >= 'a' && charName[0] <= 'f') ||
		(charName[0] >= 'A' && charName[0] <= 'F') ||
		(charName[1] >= 'a' && charName[1] <= 'f') ||
		(charName[1] >= 'A' && charName[1] <= 'F')))) {
	    hex = gTrue;
	  }
	  missing = gTrue;
	}
      } else {
	toUnicode[code] = 0;
      }
    }

    // pass 2: try to fill in the missing chars, looking for names of
    // the form 'Axx', 'xx', 'Ann', or 'nn', where 'A' is any letter,
    // 'xx' is two hex digits, and 'nn' is 2-4 decimal digits
    if (missing && globalParams->getMapNumericCharNames()) {
      for (code = 0; code < 256; ++code) {
	if ((charName = enc[code]) && !toUnicode[code] &&
	    strcmp(charName, ".notdef")) {
	  n = strlen(charName);
	  code2 = -1;
	  if (hex && n == 3 && isalpha(charName[0]) &&
	      isxdigit(charName[1]) && isxdigit(charName[2])) {
	    sscanf(charName+1, "%x", &code2);
	  } else if (hex && n == 2 &&
		     isxdigit(charName[0]) && isxdigit(charName[1])) {
	    sscanf(charName, "%x", &code2);
	  } else if (!hex && n >= 2 && n <= 4 &&
		     isdigit(charName[0]) && isdigit(charName[1])) {
	    code2 = atoi(charName);
	  } else if (n >= 3 && n <= 5 &&
		     isdigit(charName[1]) && isdigit(charName[2])) {
	    code2 = atoi(charName+1);
	  }
	  if (code2 >= 0 && code2 <= 0xff) {
	    toUnicode[code] = (Unicode)code2;
	  }
	}
      }
    }

    ctu = CharCodeToUnicode::make8BitToUnicode(toUnicode);
  }

  //----- get the character widths -----

  // initialize all widths
  for (code = 0; code < 256; ++code) {
    widths[code] = missingWidth * 0.001;
  }

  // use widths from font dict, if present
  fontDict->lookup("FirstChar", &obj1);
  firstChar = obj1.isInt() ? obj1.getInt() : 0;
  obj1.free();
  fontDict->lookup("LastChar", &obj1);
  lastChar = obj1.isInt() ? obj1.getInt() : 255;
  obj1.free();
  mul = (type == fontType3) ? fontMat[0] : 0.001;
  fontDict->lookup("Widths", &obj1);
  if (obj1.isArray()) {
    for (code = firstChar; code <= lastChar; ++code) {
      obj1.arrayGet(code - firstChar, &obj2);
      if (obj2.isNum()) {
	widths[code] = obj2.getNum() * mul;
      }
      obj2.free();
    }

  // use widths from built-in font
  } else if (builtinFont) {
    // this is a kludge for broken PDF files that encode char 32
    // as .notdef
    if (builtinFont->widths->getWidth("space", &w)) {
      widths[32] = 0.001 * w;
    }
    for (code = 0; code < 256; ++code) {
      if (enc[code] && builtinFont->widths->getWidth(enc[code], &w)) {
	widths[code] = 0.001 * w;
      }
    }

  // couldn't find widths -- use defaults
  } else {
    // this is technically an error -- the Widths entry is required
    // for all but the Base-14 fonts -- but certain PDF generators
    // apparently don't include widths for Arial and TimesNewRoman
    if (isFixedWidth()) {
      i = 0;
    } else if (isSerif()) {
      i = 8;
    } else {
      i = 4;
    }
    if (isBold()) {
      i += 2;
    }
    if (isItalic()) {
      i += 1;
    }
    builtinFont = builtinFontSubst[i];
    // this is a kludge for broken PDF files that encode char 32
    // as .notdef
    if (builtinFont->widths->getWidth("space", &w)) {
      widths[32] = 0.001 * w;
    }
    for (code = 0; code < 256; ++code) {
      if (enc[code] && builtinFont->widths->getWidth(enc[code], &w)) {
	widths[code] = 0.001 * w;
      }
    }
  }
  obj1.free();

  ok = gTrue;
}

Gfx8BitFont::~Gfx8BitFont() {
  int i;

  for (i = 0; i < 256; ++i) {
    if (encFree[i] && enc[i]) {
      gfree(enc[i]);
    }
  }
  ctu->decRefCnt();
  if (charProcs.isDict()) {
    charProcs.free();
  }
}

int Gfx8BitFont::getNextChar(char *s, int len, CharCode *code,
			     Unicode *u, int uSize, int *uLen,
			     double *dx, double *dy, double *ox, double *oy) {
  CharCode c;

  *code = c = (CharCode)(*s & 0xff);
  *uLen = ctu->mapToUnicode(c, u, uSize);
  *dx = widths[c];
  *dy = *ox = *oy = 0;
  return 1;
}

CharCodeToUnicode *Gfx8BitFont::getToUnicode() {
  ctu->incRefCnt();
  return ctu;
}

Object *Gfx8BitFont::getCharProc(int code, Object *proc) {
  if (charProcs.isDict()) {
    charProcs.dictLookup(enc[code], proc);
  } else {
    proc->initNull();
  }
  return proc;
}

void Gfx8BitFont::substitute(GString *name1, GfxFontType type1) {
  delete name;
  name = name1;
  type = type1;
  delete embFontName;
  embFontName = NULL;
  embFontID.num = -1;
  embFontID.gen = -1;
}

//------------------------------------------------------------------------
// GfxCIDFont
//------------------------------------------------------------------------

static int cmpWidthExcep(const void *w1, const void *w2) {
  return ((GfxFontCIDWidthExcep *)w1)->first -
         ((GfxFontCIDWidthExcep *)w2)->first;
}

static int cmpWidthExcepV(const void *w1, const void *w2) {
  return ((GfxFontCIDWidthExcepV *)w1)->first -
         ((GfxFontCIDWidthExcepV *)w2)->first;
}

GfxCIDFont::GfxCIDFont(XRef *xref, Ref idA, Dict *fontDict):
  GfxFont(idA)
{
  Dict *desFontDict;
  GString *collection, *cMapName;
  Object desFontDictObj;
  Object obj1, obj2, obj3, obj4, obj5, obj6;
  int c1, c2;
  int excepsSize, i, j, k;

  ascent = 0.95;
  descent = -0.35;
  fontBBox[0] = fontBBox[1] = fontBBox[2] = fontBBox[3] = 0;
  cMap = NULL;
  ctu = NULL;
  widths.defWidth = 1.0;
  widths.defHeight = -1.0;
  widths.defVY = 0.880;
  widths.exceps = NULL;
  widths.nExceps = 0;
  widths.excepsV = NULL;
  widths.nExcepsV = 0;
  cidToGID = NULL;
  cidToGIDLen = 0;

  // get the descendant font
  if (!fontDict->lookup("DescendantFonts", &obj1)->isArray()) {
    error(-1, "Missing DescendantFonts entry in Type 0 font");
    obj1.free();
    goto err1;
  }
  if (!obj1.arrayGet(0, &desFontDictObj)->isDict()) {
    error(-1, "Bad descendant font in Type 0 font");
    goto err3;
  }
  obj1.free();
  desFontDict = desFontDictObj.getDict();

  // font type
  if (!desFontDict->lookup("Subtype", &obj1)) {
    error(-1, "Missing Subtype entry in Type 0 descendant font");
    goto err3;
  }
  if (obj1.isName("CIDFontType0")) {
    type = fontCIDType0;
  } else if (obj1.isName("CIDFontType2")) {
    type = fontCIDType2;
  } else {
    error(-1, "Unknown Type 0 descendant font type '%s'",
	  obj1.isName() ? obj1.getName() : "???");
    goto err3;
  }
  obj1.free();

  // get info from font descriptor
  readFontDescriptor(xref, desFontDict);

  // look for an external font file
  findExtFontFile();

  //----- encoding info -----

  // char collection
  if (!desFontDict->lookup("CIDSystemInfo", &obj1)->isDict()) {
    error(-1, "Missing CIDSystemInfo dictionary in Type 0 descendant font");
    goto err3;
  }
  obj1.dictLookup("Registry", &obj2);
  obj1.dictLookup("Ordering", &obj3);
  if (!obj2.isString() || !obj3.isString()) {
    error(-1, "Invalid CIDSystemInfo dictionary in Type 0 descendant font");
    goto err4;
  }
  collection = obj2.getString()->copy()->append('-')->append(obj3.getString());
  obj3.free();
  obj2.free();
  obj1.free();

  // look for a ToUnicode CMap
  if (!(ctu = readToUnicodeCMap(fontDict, 16))) {

    // the "Adobe-Identity" and "Adobe-UCS" collections don't have
    // cidToUnicode files
    if (collection->cmp("Adobe-Identity") &&
	collection->cmp("Adobe-UCS")) {

      // look for a user-supplied .cidToUnicode file
      if (!(ctu = globalParams->getCIDToUnicode(collection))) {
	error(-1, "Unknown character collection '%s'",
	      collection->getCString());
	delete collection;
	goto err2;
      }
    }
  }

  // encoding (i.e., CMap)
  //~ need to handle a CMap stream here
  //~ also need to deal with the UseCMap entry in the stream dict
  if (!fontDict->lookup("Encoding", &obj1)->isName()) {
    error(-1, "Missing or invalid Encoding entry in Type 0 font");
    delete collection;
    goto err3;
  }
  cMapName = new GString(obj1.getName());
  obj1.free();
  if (!(cMap = globalParams->getCMap(collection, cMapName))) {
    error(-1, "Unknown CMap '%s' for character collection '%s'",
	  cMapName->getCString(), collection->getCString());
    delete collection;
    delete cMapName;
    goto err2;
  }
  delete collection;
  delete cMapName;

  // CIDToGIDMap (for embedded TrueType fonts)
  if (type == fontCIDType2) {
    fontDict->lookup("CIDToGIDMap", &obj1);
    if (obj1.isStream()) {
      cidToGIDLen = 0;
      i = 64;
      cidToGID = (Gushort *)gmalloc(i * sizeof(Gushort));
      obj1.streamReset();
      while ((c1 = obj1.streamGetChar()) != EOF &&
	     (c2 = obj1.streamGetChar()) != EOF) {
	if (cidToGIDLen == i) {
	  i *= 2;
	  cidToGID = (Gushort *)grealloc(cidToGID, i * sizeof(Gushort));
	}
	cidToGID[cidToGIDLen++] = (Gushort)((c1 << 8) + c2);
      }
    } else if (!obj1.isName("Identity") && !obj1.isNull()) {
      error(-1, "Invalid CIDToGIDMap entry in CID font");
    }
    obj1.free();
  }

  //----- character metrics -----

  // default char width
  if (desFontDict->lookup("DW", &obj1)->isInt()) {
    widths.defWidth = obj1.getInt() * 0.001;
  }
  obj1.free();

  // char width exceptions
  if (desFontDict->lookup("W", &obj1)->isArray()) {
    excepsSize = 0;
    i = 0;
    while (i + 1 < obj1.arrayGetLength()) {
      obj1.arrayGet(i, &obj2);
      obj1.arrayGet(i + 1, &obj3);
      if (obj2.isInt() && obj3.isInt() && i + 2 < obj1.arrayGetLength()) {
	if (obj1.arrayGet(i + 2, &obj4)->isNum()) {
	  if (widths.nExceps == excepsSize) {
	    excepsSize += 16;
	    widths.exceps = (GfxFontCIDWidthExcep *)
	      grealloc(widths.exceps,
		       excepsSize * sizeof(GfxFontCIDWidthExcep));
	  }
	  widths.exceps[widths.nExceps].first = obj2.getInt();
	  widths.exceps[widths.nExceps].last = obj3.getInt();
	  widths.exceps[widths.nExceps].width = obj4.getNum() * 0.001;
	  ++widths.nExceps;
	} else {
	  error(-1, "Bad widths array in Type 0 font");
	}
	obj4.free();
	i += 3;
      } else if (obj2.isInt() && obj3.isArray()) {
	if (widths.nExceps + obj3.arrayGetLength() > excepsSize) {
	  excepsSize = (widths.nExceps + obj3.arrayGetLength() + 15) & ~15;
	  widths.exceps = (GfxFontCIDWidthExcep *)
	    grealloc(widths.exceps,
		     excepsSize * sizeof(GfxFontCIDWidthExcep));
	}
	j = obj2.getInt();
	for (k = 0; k < obj3.arrayGetLength(); ++k) {
	  if (obj3.arrayGet(k, &obj4)->isNum()) {
	    widths.exceps[widths.nExceps].first = j;
	    widths.exceps[widths.nExceps].last = j;
	    widths.exceps[widths.nExceps].width = obj4.getNum() * 0.001;
	    ++j;
	    ++widths.nExceps;
	  } else {
	    error(-1, "Bad widths array in Type 0 font");
	  }
	  obj4.free();
	}
	i += 2;
      } else {
	error(-1, "Bad widths array in Type 0 font");
	++i;
      }
      obj3.free();
      obj2.free();
    }
    qsort(widths.exceps, widths.nExceps, sizeof(GfxFontCIDWidthExcep),
	  &cmpWidthExcep);
  }
  obj1.free();

  // default metrics for vertical font
  if (desFontDict->lookup("DW2", &obj1)->isArray() &&
      obj1.arrayGetLength() == 2) {
    if (obj1.arrayGet(0, &obj2)->isNum()) {
      widths.defVY = obj1.getNum() * 0.001;
    }
    obj2.free();
    if (obj1.arrayGet(1, &obj2)->isNum()) {
      widths.defHeight = obj1.getNum() * 0.001;
    }
    obj2.free();
  }
  obj1.free();

  // char metric exceptions for vertical font
  if (desFontDict->lookup("W2", &obj1)->isArray()) {
    excepsSize = 0;
    i = 0;
    while (i + 1 < obj1.arrayGetLength()) {
      obj1.arrayGet(0, &obj2);
      obj2.arrayGet(0, &obj3);
      if (obj2.isInt() && obj3.isInt() && i + 4 < obj1.arrayGetLength()) {
	if (obj1.arrayGet(i + 2, &obj4)->isNum() &&
	    obj1.arrayGet(i + 3, &obj5)->isNum() &&
	    obj1.arrayGet(i + 4, &obj6)->isNum()) {
	  if (widths.nExcepsV == excepsSize) {
	    excepsSize += 16;
	    widths.excepsV = (GfxFontCIDWidthExcepV *)
	      grealloc(widths.excepsV,
		       excepsSize * sizeof(GfxFontCIDWidthExcepV));
	  }
	  widths.excepsV[widths.nExcepsV].first = obj2.getInt();
	  widths.excepsV[widths.nExcepsV].last = obj3.getInt();
	  widths.excepsV[widths.nExcepsV].height = obj4.getNum() * 0.001;
	  widths.excepsV[widths.nExcepsV].vx = obj5.getNum() * 0.001;
	  widths.excepsV[widths.nExcepsV].vy = obj6.getNum() * 0.001;
	  ++widths.nExcepsV;
	} else {
	  error(-1, "Bad widths (W2) array in Type 0 font");
	}
	obj6.free();
	obj5.free();
	obj4.free();
	i += 5;
      } else if (obj2.isInt() && obj3.isArray()) {
	if (widths.nExcepsV + obj3.arrayGetLength() / 3 > excepsSize) {
	  excepsSize =
	    (widths.nExcepsV + obj3.arrayGetLength() / 3 + 15) & ~15;
	  widths.excepsV = (GfxFontCIDWidthExcepV *)
	    grealloc(widths.excepsV,
		     excepsSize * sizeof(GfxFontCIDWidthExcepV));
	}
	j = obj2.getInt();
	for (k = 0; k < obj3.arrayGetLength(); ++k) {
	  if (obj3.arrayGet(k, &obj4)->isNum() &&
	      obj3.arrayGet(k, &obj5)->isNum() &&
	      obj3.arrayGet(k, &obj6)->isNum()) {
	    widths.excepsV[widths.nExceps].first = j;
	    widths.excepsV[widths.nExceps].last = j;
	    widths.excepsV[widths.nExceps].height = obj4.getNum() * 0.001;
	    widths.excepsV[widths.nExceps].vx = obj5.getNum() * 0.001;
	    widths.excepsV[widths.nExceps].vy = obj6.getNum() * 0.001;
	    ++j;
	    ++widths.nExcepsV;
	  } else {
	    error(-1, "Bad widths (W2) array in Type 0 font");
	  }
	  obj6.free();
	  obj5.free();
	  obj4.free();
	}
	i += 2;
      } else {
	error(-1, "Bad widths (W2) array in Type 0 font");
	++i;
      }
      obj3.free();
      obj2.free();
    }
    qsort(widths.excepsV, widths.nExcepsV, sizeof(GfxFontCIDWidthExcepV),
	  &cmpWidthExcepV);
  }
  obj1.free();

  desFontDictObj.free();
  ok = gTrue;
  return;

 err4:
  obj3.free();
  obj2.free();
 err3:
  obj1.free();
 err2:
  desFontDictObj.free();
 err1:;
}

GfxCIDFont::~GfxCIDFont() {
  if (cMap) {
    cMap->decRefCnt();
  }
  if (ctu) {
    ctu->decRefCnt();
  }
  gfree(widths.exceps);
  gfree(widths.excepsV);
  if (cidToGID) {
    gfree(cidToGID);
  }
}

int GfxCIDFont::getNextChar(char *s, int len, CharCode *code,
			    Unicode *u, int uSize, int *uLen,
			    double *dx, double *dy, double *ox, double *oy) {
  CID cid;
  double w, h, vx, vy;
  int n, a, b, m;

  if (!cMap) {
    *code = 0;
    *uLen = 0;
    *dx = *dy = 0;
    return 1;
  }

  *code = (CharCode)(cid = cMap->getCID(s, len, &n));
  if (ctu) {
    *uLen = ctu->mapToUnicode(cid, u, uSize);
  } else {
    *uLen = 0;
  }

  // horizontal
  if (cMap->getWMode() == 0) {
    w = widths.defWidth;
    h = vx = vy = 0;
    if (widths.nExceps > 0 && cid >= widths.exceps[0].first) {
      a = 0;
      b = widths.nExceps;
      // invariant: widths.exceps[a].first <= cid < widths.exceps[b].first
      while (b - a > 1) {
	m = (a + b) / 2;
	if (widths.exceps[m].first <= cid) {
	  a = m;
	} else {
	  b = m;
	}
      }
      if (cid <= widths.exceps[a].last) {
	w = widths.exceps[a].width;
      }
    }

  // vertical
  } else {
    w = 0;
    h = widths.defHeight;
    vx = widths.defWidth / 2;
    vy = widths.defVY;
    if (widths.nExcepsV > 0 && cid >= widths.excepsV[0].first) {
      a = 0;
      b = widths.nExcepsV;
      // invariant: widths.excepsV[a].first <= cid < widths.excepsV[b].first
      while (b - a > 1) {
	m = (a + b) / 2;
	if (widths.excepsV[m].last <= cid) {
	  a = m;
	} else {
	  b = m;
	}
      }
      if (cid <= widths.excepsV[a].last) {
	h = widths.excepsV[a].height;
	vx = widths.excepsV[a].vx;
	vy = widths.excepsV[a].vy;
      }
    }
  }

  *dx = w;
  *dy = h;
  *ox = vx;
  *oy = vy;

  return n;
}

CharCodeToUnicode *GfxCIDFont::getToUnicode() {
  ctu->incRefCnt();
  return ctu;
}

GString *GfxCIDFont::getCollection() {
  return cMap ? cMap->getCollection() : (GString *)NULL;
}

//------------------------------------------------------------------------
// GfxFontDict
//------------------------------------------------------------------------

GfxFontDict::GfxFontDict(XRef *xref, Dict *fontDict) {
  int i;
  Object obj1;

  numFonts = fontDict->getLength();
  fonts = (Entry *)gmalloc(numFonts * sizeof(GfxFont *));
  for (i = 0; i < numFonts; ++i) {
    fontDict->getValNF(i, &obj1);
    fonts[i].tag = new GString(fontDict->getKey(i));
    if (obj1.isRef()) {
      fonts[i].gfxFont = gfxFontCache->getGfxFont(xref, obj1.getRef());
    } else {
      error(-1, "font resource is not a dictionary");
      fonts[i].gfxFont = NULL;
    }
    obj1.free();
  }
}

GfxFontDict::~GfxFontDict() {
  int i;

  for (i = 0; i < numFonts; ++i) {
    gfxFontCache->release(fonts[i].gfxFont);
    delete fonts[i].tag;
  }
  gfree(fonts);
}

GfxFont *GfxFontDict::lookup(char *tag) {
  int i;

  for (i = 0; i < numFonts; ++i) {
    if (fonts[i].tag->cmp(tag) == 0) {
      return fonts[i].gfxFont;
    }
  }
  return NULL;
}
