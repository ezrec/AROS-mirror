#ifndef VPDF_FONTCACHE_H
#define VPDF_FONTCACHE_H

/* custom fontconfig types */

#ifdef __cplusplus
extern "C" {
#endif

enum {
	FC_ETYPE_STRING = 1,
	FC_ETYPE_INTEGER,
	FC_ETYPE_POINTER
};


#define FC_WEIGHT_THIN		    0
#define FC_WEIGHT_EXTRALIGHT	    40
#define FC_WEIGHT_ULTRALIGHT	    FC_WEIGHT_EXTRALIGHT
#define FC_WEIGHT_LIGHT		    50
#define FC_WEIGHT_BOOK		    75
#define FC_WEIGHT_REGULAR	    80
#define FC_WEIGHT_NORMAL	    FC_WEIGHT_REGULAR
#define FC_WEIGHT_MEDIUM	    100
#define FC_WEIGHT_DEMIBOLD	    180
#define FC_WEIGHT_SEMIBOLD	    FC_WEIGHT_DEMIBOLD
#define FC_WEIGHT_BOLD		    200
#define FC_WEIGHT_EXTRABOLD	    205
#define FC_WEIGHT_ULTRABOLD	    FC_WEIGHT_EXTRABOLD
#define FC_WEIGHT_BLACK		    210
#define FC_WEIGHT_HEAVY		    FC_WEIGHT_BLACK
#define FC_WEIGHT_EXTRABLACK	    215
#define FC_WEIGHT_ULTRABLACK	    FC_WEIGHT_EXTRABLACK

#define FC_SLANT_ROMAN		    0
#define FC_SLANT_ITALIC		    100
#define FC_SLANT_OBLIQUE	    110

#define FC_WIDTH_ULTRACONDENSED	    50
#define FC_WIDTH_EXTRACONDENSED	    63
#define FC_WIDTH_CONDENSED	    75
#define FC_WIDTH_SEMICONDENSED	    87
#define FC_WIDTH_NORMAL		    100
#define FC_WIDTH_SEMIEXPANDED	    113
#define FC_WIDTH_EXPANDED	    125
#define FC_WIDTH_EXTRAEXPANDED	    150
#define FC_WIDTH_ULTRAEXPANDED	    200

#define FC_PROPORTIONAL		    0
#define FC_DUAL			    90
#define FC_MONO			    100
#define FC_CHARCELL		    110

enum {
	FC_FAMILY = 0,
	//FC_FAMILYLANG,
	FC_FULLNAME,
	//FC_FULLNAMELANG,
	FC_STYLE,
	//FC_STYLELANG,
	FC_FILE,
	FC_INDEX, /* index of a face in font file */
	FC_WEIGHT,
	FC_WIDTH,
	FC_SLANT,
	//FC_DECORATIVE,
	//FC_CHARSET,
	FC_LANG,
	//FC_PROPORTIONAL,
	FC_SPACING,
	FC_SERIF,
};

static int fcelementpriority[] = {
	99,
	//{FC_FAMILYLANG,	9},
	98,
	//{FC_FULLNAMELANG,	8},
	97,
	//{FC_STYLELANG,	7},
	100,
	0,
	96,
	95,
	94,
	//{FC_DECORATIVE,	93},
	//{FC_CHARSET,		6},
	92,
	//{FC_PROPORTIONAL,	91},
	90,
	89,
};


struct fontpattern
{
	struct MinNode n;
	struct List entries;
};

union patternvalue
{
	int i;
	char *s;
	void *p;
};

struct patternentry
{
	struct MinNode n;
	int element;
	int type;
	int id;					/* multiple elements can be on the list */
	int excluding;			/* for patterns used as match criteria */
	union patternvalue value;
};

int fcPatternAddString(struct fontpattern *pat, int element, char *value);
int fcPatternAddInteger(struct fontpattern *pat, int element, int value);
int fcPatternAddExcludingInteger(struct fontpattern *pat, int element, int value);
char *fcPatternGetString(struct fontpattern *pat, int element, int id);
int fcPatternGetInteger(struct fontpattern *pat, int element, int id);
struct fontpattern *fcPatternAlloc(void);
void fcPatternDestroy(struct fontpattern *fd);
struct fontcache *fcCreate(char *directories[]);
void fcDestroy(struct fontcache *cache);
struct fontpattern *fcMatch(struct fontcache *cache, struct fontpattern *pat);

struct fontcache *fcLoad(char *fname);
int fcSave(struct fontcache *fontcache, char *fname);

#ifdef __cplusplus
}
#endif

#endif
