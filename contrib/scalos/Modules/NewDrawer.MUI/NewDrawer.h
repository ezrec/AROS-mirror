// NewDrawer.h
// $Date$
// $Revision$


#ifndef NEWDRAWER_H
#define	NEWDRAWER_H

#define NDP_IconCreation	0x80000001

#define d1(x)		;
#define d2(x)		x;

#define	debugLock_d1(LockName) ;
#define	debugLock_d2(LockName) \
	{\
	char xxName[200];\
	strcpy(xxName, "");\
	NameFromLock((LockName), xxName, sizeof(xxName));\
	KPrintF("%s/%s/%ld: " #LockName "=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, LockName, xxName);\
	}

extern int kprintf(CONST_STRPTR, ...);
extern int KPrintF(CONST_STRPTR, ...);

IPTR mui_getv(APTR, ULONG );

struct NewDrawer_LocaleInfo
{
	APTR li_LocaleBase;
	APTR li_Catalog;
	struct LocaleIFace *li_ILocale;
};

#endif	/* NEWDRAWER_H */
