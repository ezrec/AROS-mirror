/* Written by Michal 'kiero' Wozniak (wozniak_m@interia.pl)
*/

#ifndef PACKAGE_NAME
#include <config.h>
#endif

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>

#include "goo/gmem.h"
#include "goo/GooString.h"
#include "goo/GooList.h"
#include "goo/GooHash.h"
#include "goo/gfile.h"
#include "Error.h"
#include "NameToCharCode.h"
#include "CharCodeToUnicode.h"
#include "UnicodeMap.h"
#include "CMap.h"
#include "BuiltinFontTables.h"
#include "FontEncodingTables.h"
#include "GlobalParams.h"
#include "GfxFont.h"

#include <dos/dosextens.h>
#include <clib/debug_protos.h>
#include "../../fontcache.h"

#if MULTITHREADED
#  define lockGlobalParams            gLockMutex(&mutex)
#  define lockUnicodeMapCache         gLockMutex(&unicodeMapCacheMutex)
#  define lockCMapCache               gLockMutex(&cMapCacheMutex)
#  define unlockGlobalParams          gUnlockMutex(&mutex)
#  define unlockUnicodeMapCache       gUnlockMutex(&unicodeMapCacheMutex)
#  define unlockCMapCache             gUnlockMutex(&cMapCacheMutex)
#else
#  define lockGlobalParams
#  define lockUnicodeMapCache
#  define lockCMapCache
#  define unlockGlobalParams
#  define unlockUnicodeMapCache
#  define unlockCMapCache
#endif

#define DEFAULT_SUBSTITUTE_FONT "Helvetica"

static struct {
    char *name;
    char *t1FileName;
    char *ttFileName;
} displayFontTab[] = {
    {"Courier",               "n022003l.pfb", "cour.ttf"},
    {"Courier-Bold",          "n022004l.pfb", "courbd.ttf"},
    {"Courier-BoldOblique",   "n022024l.pfb", "courbi.ttf"},
    {"Courier-Oblique",       "n022023l.pfb", "couri.ttf"},
    {"Helvetica",             "n019003l.pfb", "arial.ttf"},
    {"Helvetica-Bold",        "n019004l.pfb", "arialbd.ttf"},
    {"Helvetica-BoldOblique", "n019024l.pfb", "arialbi.ttf"},
    {"Helvetica-Oblique",     "n019023l.pfb", "ariali.ttf"},
    // TODO: not sure if "symbol.ttf" is right
    {"Symbol",                "s050000l.pfb", "symbol.ttf"},
    {"Times-Bold",            "n021004l.pfb", "timesbd.ttf"},
    {"Times-BoldItalic",      "n021024l.pfb", "timesbi.ttf"},
    {"Times-Italic",          "n021023l.pfb", "timesi.ttf"},
    {"Times-Roman",           "n021003l.pfb", "times.ttf"},
    // TODO: not sure if "wingding.ttf" is right
    {"ZapfDingbats",          "d050000l.pfb", "wingding.ttf"},

    // those seem to be frequently accessed by PDF files and I kind of guess
    // which font file do the refer to
    {"Palatino", NULL, "pala.ttf"},
    {"Palatino-Roman", NULL, "pala.ttf"},
    {"Palatino-Bold", NULL, "palab.ttf"},
    {"Palatino-Italic", NULL, "palai.ttf"},
    {"Palatino,Italic", NULL, "palai.ttf"},
    {"Palatino-BoldItalic", NULL, "palabi.ttf"},

    {"ArialBlack",        NULL, "arialbd.ttf"},

    {"ArialNarrow", NULL, "arialn.ttf"},
    {"ArialNarrow,Bold", NULL, "arialnb.ttf"},
    {"ArialNarrow,Italic", NULL, "arialni.ttf"},
    {"ArialNarrow,BoldItalic", NULL, "arialnbi.ttf"},
    {"ArialNarrow-Bold", NULL, "arialnb.ttf"},
    {"ArialNarrow-Italic", NULL, "arialni.ttf"},
    {"ArialNarrow-BoldItalic", NULL, "arialnbi.ttf"},

    {"HelveticaNarrow", NULL, "arialn.ttf"},
    {"HelveticaNarrow,Bold", NULL, "arialnb.ttf"},
    {"HelveticaNarrow,Italic", NULL, "arialni.ttf"},
    {"HelveticaNarrow,BoldItalic", NULL, "arialnbi.ttf"},
    {"HelveticaNarrow-Bold", NULL, "arialnb.ttf"},
    {"HelveticaNarrow-Italic", NULL, "arialni.ttf"},
    {"HelveticaNarrow-BoldItalic", NULL, "arialnbi.ttf"},

    {"BookAntiqua", NULL, "bkant.ttf"},
    {"BookAntiqua,Bold", NULL, "bkant.ttf"},
    {"BookAntiqua,Italic", NULL, "bkant.ttf"},
    {"BookAntiqua,BoldItalic", NULL, "bkant.ttf"},
    {"BookAntiqua-Bold", NULL, "bkant.ttf"},
    {"BookAntiqua-Italic", NULL, "bkant.ttf"},
    {"BookAntiqua-BoldItalic", NULL, "bkant.ttf"},

    {"Verdana", NULL, "verdana.ttf"},
    {"Verdana,Bold", NULL, "verdanab.ttf"},
    {"Verdana,Italic", NULL, "verdanai.ttf"},
    {"Verdana,BoldItalic", NULL, "verdanaz.ttf"},
    {"Verdana-Bold", NULL, "verdanab.ttf"},
    {"Verdana-Italic", NULL, "verdanai.ttf"},
    {"Verdana-BoldItalic", NULL, "verdanaz.ttf"},

    {"Tahoma", NULL, "tahoma.ttf"},
    {"Tahoma,Bold", NULL, "tahomabd.ttf"},
    {"Tahoma,Italic", NULL, "tahoma.ttf"},
    {"Tahoma,BoldItalic", NULL, "tahomabd.ttf"},
    {"Tahoma-Bold", NULL, "tahomabd.ttf"},
    {"Tahoma-Italic", NULL, "tahoma.ttf"},
    {"Tahoma-BoldItalic", NULL, "tahomabd.ttf"},

    {"CCRIKH+Verdana", NULL, "verdana.ttf"},
    {"CCRIKH+Verdana,Bold", NULL, "verdanab.ttf"},
    {"CCRIKH+Verdana,Italic", NULL, "verdanai.ttf"},
    {"CCRIKH+Verdana,BoldItalic", NULL, "verdanaz.ttf"},
    {"CCRIKH+Verdana-Bold", NULL, "verdanab.ttf"},
    {"CCRIKH+Verdana-Italic", NULL, "verdanai.ttf"},
    {"CCRIKH+Verdana-BoldItalic", NULL, "verdanaz.ttf"},

    {"Georgia", NULL, "georgia.ttf"},
    {"Georgia,Bold", NULL, "georgiab.ttf"},
    {"Georgia,Italic", NULL, "georgiai.ttf"},
    {"Georgia,BoldItalic", NULL, "georgiaz.ttf"},
    {"Georgia-Bold", NULL, "georgiab.ttf"},
    {"Georgia-Italic", NULL, "georgiai.ttf"},
    {"Georgia-BoldItalic", NULL, "georgiaz.ttf"},

    {NULL}
};

#define FONTS_SUBDIR "FONTS:"	// TODO: make sure to scan all directories belonging to an assign

static bool FileExists(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (f)
		fclose(f);
    return f != NULL;
    
}

/* no need to do this on MorphOS */

void GlobalParams::setupBaseFonts(char * dir)
{
}

static struct fontcache *setupFontCache()
{
	char *fonts[] = {"sys:fonts", "mossys:fonts", "PROGDIR:fonts", NULL};
	struct fontcache *fontcache;
	int isnew = FALSE;

	struct Process *process = (struct Process*)FindTask(NULL);
	struct Window *oldWindowPtr = (struct Window*)process->pr_WindowPtr;
	process->pr_WindowPtr = (struct Window*)-1;
		
	fontcache = fcLoad("PROGDIR:vpdf.fontcache");
	//kprintf("checking progdir. %p\n", fontcache);
	if (fontcache == NULL)
	{
		/* retry from T: in case we had PROGDIR write protected */
		fontcache = fcLoad("T:vpdf.fontcache");
		//kprintf("checking t. %p\n", fontcache);
		if (fontcache == NULL)
		{
			isnew = TRUE;
			fontcache = fcCreate(fonts);
		}
    }
    
    /* add builtin substitutions to the cache */
    
    if (fontcache != NULL)	
    {
    	int i = 0;
		while(displayFontTab[i].name != NULL)
		{
			struct fontpattern *pat = fcPatternAlloc();
			if (pat != NULL)
			{
				char *name = displayFontTab[i].name;
				char family[128];
				char path[512];
				int j;
				int exists = FALSE;
				
				strcpy(family, name);
				if (strstr(family, ","))
					*strstr(family, ",") = '\0';
				if (strstr(family, "-"))
					*strstr(family, "-") = '\0';
			
				fcPatternAddString(pat, FC_FAMILY, family);
				fcPatternAddString(pat, FC_FULLNAME, name);
				
				j=0; 
				while(fonts[j] != NULL)
				{
					if (displayFontTab[i].t1FileName != NULL)
					{
						snprintf(path, sizeof(path), "%s/%s", fonts[j], displayFontTab[i].t1FileName);
						if (FileExists(path))
						{
							fcPatternAddString(pat, FC_FILE, path);
							exists = TRUE;
						}
					}
					if (displayFontTab[i].ttFileName != NULL)
					{
						snprintf(path, sizeof(path), "%s/%s", fonts[j], displayFontTab[i].ttFileName);
						if (FileExists(path))
						{
							fcPatternAddString(pat, FC_FILE, path);
							exists = TRUE;
						}
					}
					j++;
				}
					
				if (strstr(name, "Italic"))
					fcPatternAddInteger(pat, FC_SLANT, FC_SLANT_ITALIC);
				if (strstr(name, "Roman"))
					fcPatternAddInteger(pat, FC_SLANT, FC_SLANT_ROMAN);
				if (strstr(name, "Bold"))
					fcPatternAddInteger(pat, FC_WEIGHT, FC_WEIGHT_BOLD);
				else
					fcPatternAddInteger(pat, FC_WEIGHT, FC_WEIGHT_REGULAR);
					
				if (exists)
					fcAddPattern(fontcache, pat);
				else
					fcPatternDestroy(pat);
			}
			
			i++;
		}
		
		if (isnew)
		{
			/* save cache. main dir is PROGDIR: but when it fails (write protected?) try T: */
			if (fcSave(fontcache, "PROGDIR:vpdf.fontcache") == FALSE)
			{
				fcSave(fontcache, "T:vpdf.fontcache");
			}
		}
    }
    
    //kprintf("fontcache loaded to %p\n", fontcache);
    
    process->pr_WindowPtr = oldWindowPtr;
    
    /* TODO: handle some substitution rules here? */
    
    return fontcache;
}

static GBool findModifier(const char *name, const char *modifier, const char **start)
{
	const char *match;

	if (name == NULL)
		return gFalse;

	match = strstr(name, modifier);
	if (match)
	{
		if (*start == NULL || match < *start)
			*start = match;
		return gTrue;
	}
	else
	{
		return gFalse;
	}
}

static struct fontpattern *buildFcPattern(struct fontpattern *p, GfxFont *font, GooString *base14Name)
{
	int weight = -1,
		slant = -1,
		width = -1;
		
	bool deleteFamily = false;
	char *family, *name, *lang, *modifiers;
	const char *start;
	
	// this is all heuristics will be overwritten if font had proper info
	name = base14Name != NULL ? base14Name->getCString() : font->getName()->getCString();
	//printf("  building font substitution for font:%s, family:%s, serif:%d, symbolic:%d\n", name, font->getFamily()->getCString(), font->isSerif(), font->isSymbolic());

	modifiers = strchr (name, ',');
	if (modifiers == NULL)
		modifiers = strchr (name, '-');

	// remove the - from the names, for some reason, Fontconfig does not understand "MS-Mincho" but does with "MS Mincho"
	int len = strlen(name);
	for (int i = 0; i < len; i++)
		name[i] = (name[i] == '-' ? ' ' : name[i]);

	start = NULL;
	findModifier(modifiers, "Regular", &start);
	findModifier(modifiers, "Roman", &start);

	if (findModifier(modifiers, "Oblique", &start))
		slant = FC_SLANT_OBLIQUE;
	if (findModifier(modifiers, "Italic", &start))
		slant = FC_SLANT_ITALIC;
	if (findModifier(modifiers, "Bold", &start))
		weight = FC_WEIGHT_BOLD;
	if (findModifier(modifiers, "Light", &start))
		weight = FC_WEIGHT_LIGHT;
	if (findModifier(modifiers, "Condensed", &start))
		width = FC_WIDTH_CONDENSED;

	if (start)
	{
		// There have been "modifiers" in the name, crop them to obtain the family name
		family = new char[len+1];
		strcpy(family, name);
		int pos = (modifiers - name);
		family[pos] = '\0';
		deleteFamily = true;
	}
	else
	{
		family = name;
	}

	//printf("   family: %s\n", family != NULL ? family : "no family");

	// use font flags
	if (font->isBold())
		weight = FC_WEIGHT_BOLD;
	if (font->isItalic())
		slant = FC_SLANT_ITALIC;

	// if the FontDescriptor specified a family name use it
	if (font->getFamily()) {
		if (deleteFamily)
		{
			delete[] family;
			deleteFamily = false;
		}
		family = font->getFamily()->getCString();
	}

	// if the FontDescriptor specified a weight use it
	switch (font -> getWeight())
	{
		case GfxFont::W100: weight = FC_WEIGHT_EXTRALIGHT; break; 
		case GfxFont::W200: weight = FC_WEIGHT_LIGHT; break; 
		case GfxFont::W300: weight = FC_WEIGHT_BOOK; break; 
		case GfxFont::W400: weight = FC_WEIGHT_NORMAL; break; 
		case GfxFont::W500: weight = FC_WEIGHT_MEDIUM; break; 
		case GfxFont::W600: weight = FC_WEIGHT_DEMIBOLD; break; 
		case GfxFont::W700: weight = FC_WEIGHT_BOLD; break; 
		case GfxFont::W800: weight = FC_WEIGHT_EXTRABOLD; break; 
		case GfxFont::W900: weight = FC_WEIGHT_BLACK; break; 
		default: break; 
	}

	// if the FontDescriptor specified a width use it
	switch (font -> getStretch())
	{
		case GfxFont::UltraCondensed: width = FC_WIDTH_ULTRACONDENSED; break; 
		case GfxFont::ExtraCondensed: width = FC_WIDTH_EXTRACONDENSED; break; 
		case GfxFont::Condensed: width = FC_WIDTH_CONDENSED; break; 
		case GfxFont::SemiCondensed: width = FC_WIDTH_SEMICONDENSED; break; 
		case GfxFont::Normal: width = FC_WIDTH_NORMAL; break; 
		case GfxFont::SemiExpanded: width = FC_WIDTH_SEMIEXPANDED; break; 
		case GfxFont::Expanded: width = FC_WIDTH_EXPANDED; break; 
		case GfxFont::ExtraExpanded: width = FC_WIDTH_EXTRAEXPANDED; break; 
		case GfxFont::UltraExpanded: width = FC_WIDTH_ULTRAEXPANDED; break; 
		default: break; 
	}

	// find the language we want the font to support
	if (font->isCIDFont())
	{
		GooString *collection = ((GfxCIDFont *)font)->getCollection();
		if (collection)
		{
			if (strcmp(collection->getCString(), "Adobe-GB1") == 0)
				lang = "zh-cn"; // Simplified Chinese
			else if (strcmp(collection->getCString(), "Adobe-CNS1") == 0)
				lang = "zh-tw"; // Traditional Chinese
			else if (strcmp(collection->getCString(), "Adobe-Japan1") == 0)
				lang = "ja"; // Japanese
			else if (strcmp(collection->getCString(), "Adobe-Japan2") == 0)
				lang = "ja"; // Japanese
			else if (strcmp(collection->getCString(), "Adobe-Korea1") == 0)
				lang = "ko"; // Korean
			else if (strcmp(collection->getCString(), "Adobe-UCS") == 0)
				lang = "xx";
			else if (strcmp(collection->getCString(), "Adobe-Identity") == 0)
				lang = "xx";
			else
			{
				error(errInternal, -1, "Unknown CID font collection, please report to poppler bugzilla.");
				lang = "xx";
			}
		}
		else
			lang = "xx";
	}
	else
		lang = "xx";

	fcPatternAddString(p, FC_FAMILY, family);
	fcPatternAddString(p, FC_LANG, lang);

	if (weight != -1)
		fcPatternAddInteger(p, FC_WEIGHT, weight);
	else if (!font->isBold())
		fcPatternAddInteger(p, FC_WEIGHT, FC_WEIGHT_NORMAL);
		
	if (width != -1) fcPatternAddInteger(p, FC_WIDTH, width);
	
	if (slant != -1)
		fcPatternAddInteger(p, FC_SLANT, slant);
	else if (!font->isItalic())
		fcPatternAddExcludingInteger(p, FC_SLANT, FC_SLANT_ITALIC);
	
	if (font->isFixedWidth())
		fcPatternAddInteger(p, FC_SPACING, FC_MONO);
	else 
		fcPatternAddExcludingInteger(p, FC_SPACING, FC_MONO);
		
	if (font->isSerif())
		fcPatternAddInteger(p, FC_SERIF, TRUE);
	else
		fcPatternAddExcludingInteger(p, FC_SERIF, TRUE);
	
	if (deleteFamily)
		delete[] family;
		
	return p;
}

GooString *GlobalParams::findSystemFontFile(GfxFont *font,
					  SysFontType *type,
					  int *fontNum, GooString *substituteFontName, GooString *base14Name)
{
	SysFontInfo *fi = NULL;
	fontpattern *p = NULL;
	GooString *path = NULL;
	GooString substituteName;
	GooString *fontName = font->getName();
	GooString *lookupName = base14Name != NULL ? base14Name : font->getName();
	if (!fontName) return NULL;
		
	lockGlobalParams;
	
	if (fontcache == NULL)
	{
		//kprintf("setup font cache...\n");
		fontcache = setupFontCache();
	}

	//kprintf("font lookup :%s:%s:%s\n", fontName->getCString(), substituteFontName != NULL ? substituteFontName->getCString() : "nosubst", base14Name != NULL ? base14Name->getCString() : "nobase14");

	if ((fi = sysFonts->find(lookupName, font->isFixedWidth(), gTrue)))
	{
		path = fi->path->copy();
		*type = fi->type;
		*fontNum = fi->fontNum;
		substituteName.Set(fi->substituteName->getCString());
	}
	else
	{
		char *s, *s2;
		char *ext;
		struct fontpattern *match;
		int matchingcriteria[FC_LAST_CRITERIA];
		
		p = fcPatternAlloc();
		buildFcPattern(p, font, base14Name);
		
		if (p == NULL)
			goto fin;

		match = fcMatch((struct fontcache*)fontcache, p, matchingcriteria);
		// if we have a match but it is a shitty one (no filename, no family)
		// then pick one of the builtins
		if (matchingcriteria[FC_FAMILY] == FALSE &&
			matchingcriteria[FC_FILE] == FALSE &&
			matchingcriteria[FC_FULLNAME] == FALSE)
		{
		
			//s = fcPatternGetString(match, FC_FILE, 0);
			//s2 = fcPatternGetString(match, FC_FULLNAME, 0);
			//kprintf("shitty mached one:%s:%s, retry\n", s, s2);
			
			if (font->isFixedWidth())
				fcPatternAddString(p, FC_FAMILY, "Courier");
			else
				fcPatternAddString(p, FC_FAMILY, "Times");
				
			match = fcMatch((struct fontcache*)fontcache, p, matchingcriteria);
		}
		
		if (match != NULL) /* TODO: think what to do in this case */
		{
			s = fcPatternGetString(match, FC_FILE, 0);
			s2 = fcPatternGetString(match, FC_FULLNAME, 0);
			
			//kprintf("mached one:%s:%s\n", s, s2);
			
			ext = strrchr(s, '.');
			
			if (s2 != NULL)
				substituteName.Set((char*)s2);
			else
			{
				// fontconfig does not extract fullname for some fonts
				// create the fullname from family and style
				s2 = fcPatternGetString(match, FC_FAMILY, 0);
				if (s2 != NULL)
				{
					substituteName.Set((char*)s2);
					s2 = fcPatternGetString(match, FC_STYLE, 0);
					if (s2 != NULL)
					{
						GooString *style = new GooString((char*)s2);
						if (style->cmp("Regular") != 0)
						{
							substituteName.append(" ");
							substituteName.append(style);
						}
						delete style;
					}
				}
			}

			if (ext == NULL)
				goto fin; /* TODO: ok, this is silly and shouldn't happen i guess */
				
			if (!strncasecmp(ext,".ttf",4) || !strncasecmp(ext, ".ttc", 4))
			{
				int weight, slant;
				GBool bold = font->isBold();
				GBool italic = font->isItalic();
				GBool oblique = gFalse;

				weight = fcPatternGetInteger(match, FC_WEIGHT, 0);
				slant = fcPatternGetInteger(match, FC_SLANT, 0);

				if (weight == FC_WEIGHT_DEMIBOLD || weight == FC_WEIGHT_BOLD
				    || weight == FC_WEIGHT_EXTRABOLD || weight == FC_WEIGHT_BLACK)
				{
					bold = gTrue;
				}
				if (slant == FC_SLANT_ITALIC)
					italic = gTrue;
				if (slant == FC_SLANT_OBLIQUE)
					oblique = gTrue;

				*fontNum = 0;
				*type = (!strncasecmp(ext,".ttc",4)) ? sysFontTTC : sysFontTTF;
				*fontNum = fcPatternGetInteger(match, FC_INDEX, 0);

				fi = new SysFontInfo(fontName->copy(), bold, italic, oblique, font->isFixedWidth(), new GooString((char*)s), *type, *fontNum, substituteName.copy());
				sysFonts->addFcFont(fi);
				path = new GooString((char*)s);
			}
			else if (!strncasecmp(ext,".pfa",4) || !strncasecmp(ext,".pfb",4)) 
			{
				int weight, slant;
				GBool bold = font->isBold();
				GBool italic = font->isItalic();
				GBool oblique = gFalse;

				weight = fcPatternGetInteger(match, FC_WEIGHT, 0);
				slant = fcPatternGetInteger(match, FC_SLANT, 0);

				if (weight == FC_WEIGHT_DEMIBOLD || weight == FC_WEIGHT_BOLD
				    || weight == FC_WEIGHT_EXTRABOLD || weight == FC_WEIGHT_BLACK)
				{
				  bold = gTrue;
				}
				if (slant == FC_SLANT_ITALIC)
					italic = gTrue;
				if (slant == FC_SLANT_OBLIQUE)
					oblique = gTrue;

				*type = (!strncasecmp(ext,".pfa",4)) ? sysFontPFA : sysFontPFB;
				*fontNum = fcPatternGetInteger(match, FC_INDEX, 0);
				fi = new SysFontInfo(fontName->copy(), bold, italic, oblique, font->isFixedWidth(), new GooString((char*)s), *type, *fontNum, substituteName.copy());

				sysFonts->addFcFont(fi);
				path = new GooString((char*)s);
			}
			else /* can there be anything else? */
			{
				printf("******Fontcache:unknown font name extension:%s\n", s);
			}
		}
	}

	if (path == NULL && (fi = sysFonts->find(fontName, font->isFixedWidth(), gFalse)))
	{
		path = fi->path->copy();
		*type = fi->type;
		*fontNum = fi->fontNum;
	}

	if (substituteFontName)
	{
		substituteFontName->Set(substituteName.getCString());
	}

	fin:

	if (p != NULL)
		fcPatternDestroy(p);

	unlockGlobalParams;
	return path;
    
}

