// WindowProperties.h 
// $Date$
// $Revision$

#ifndef WINDOWPROPERTIES_H 
#define	WINDOWPROPERTIES_H 

//--------------------------------------------------------------------

// Values for Icon min size limits
enum IconSizesMin
{
	ICONSIZEMIN_Unlimited = 0,
	ICONSIZEMIN_16,
	ICONSIZEMIN_24,
	ICONSIZEMIN_32,
	ICONSIZEMIN_48,
	ICONSIZEMIN_64,
	ICONSIZEMIN_96,
	ICONSIZEMIN_128,
};

//--------------------------------------------------------------------

// Values for Icon max size limits
enum IconSizesMax
{
	ICONSIZEMAX_16 = 0,
	ICONSIZEMAX_24,
	ICONSIZEMAX_32,
	ICONSIZEMAX_48,
	ICONSIZEMAX_64,
	ICONSIZEMAX_96,
	ICONSIZEMAX_128,
	ICONSIZEMAX_Unlimited,
};

//--------------------------------------------------------------------

struct  WindowProperties_LocaleInfo
{
	APTR li_LocaleBase;
	APTR li_Catalog;
	struct LocaleIFace *li_ILocale;
};

//--------------------------------------------------------------------

#define	debugLock_d1(LockName) ;
#define	debugLock_d2(LockName) \
	{\
	char xxName[200];\
	strcpy(xxName, "");\
	NameFromLock((LockName), xxName, sizeof(xxName));\
	kprintf(__FILE__ "/" __FUNC__ "/%ld: " #LockName "=%08lx <%s>\n", __LINE__, LockName, xxName);\
	}

#if defined(__SASC)
int snprintf(char *, size_t, const char *, /*args*/ ...);
int vsnprintf(char *, size_t, const char *, va_list ap);
#endif /* __SASC */

#endif	/* WINDOWPROPERTIES_H  */
