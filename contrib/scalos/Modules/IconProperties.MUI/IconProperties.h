// IconProperties.h 
// 11 Aug 2004 20:42:01

#ifndef ICONPROPERTIES_H 
#define	ICONPROPERTIES_H

struct IconProperties_LocaleInfo
{
	APTR li_LocaleBase;
	APTR li_Catalog;
	struct LocaleIFace *li_ILocale;
};

#endif	/* ICONPROPERTIES_H */
