// Rename.h
// $Date$
// $Revision$


#ifndef RENAME_H
#define	RENAME_H

#define d1(x)		;
#define d2(x)		x;

#define	debugLock_d1(LockName) ;
#define	debugLock_d2(LockName) \
	{\
	char xxName[200];\
	strcpy(xxName, "");\
	NameFromLock((LockName), xxName, sizeof(xxName));\
	kprintf(__FILE__ "/%s/%ld: " #LockName "=%08lx <%s>\n", __FUNC__, __LINE__, LockName, xxName);\
	}


extern int kprintf(CONST_STRPTR, ...);
extern int KPrintF(CONST_STRPTR, ...);

struct Rename_LocaleInfo
{
	APTR li_LocaleBase;
	APTR li_Catalog;
	struct LocaleIFace *li_ILocale;
};

#endif	/* RENAME_H */
