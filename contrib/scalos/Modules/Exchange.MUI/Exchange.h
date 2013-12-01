// Exchange.h
// $Date$
// $Revision$

#ifndef EXCHANGE_MODULE_H
#define EXCHANGE_MODULE_H

//----------------------------------------------------------------------------

#include <devices/inputevent.h>

//----------------------------------------------------------------------------

#define	VERSION_MAJOR	40
#define	VERSION_MINOR	2

#define	VERS_MAJOR	STR(VERSION_MAJOR)
#define	VERS_MINOR	STR(VERSION_MINOR)

//----------------------------------------------------------------------------

#if 0
struct PrivateCxObj
{
	struct Node     pco_Node;
	UBYTE           pco_Flags;
	UBYTE           pco_dummy1;
	struct MinList  pco_SubList;
	APTR            pco_dummy2;
	TEXT            pco_Name[CBD_NAMELEN];
	TEXT            pco_Title[CBD_TITLELEN];
	TEXT            pco_Descr[CBD_DESCRLEN];
	struct Task *   pco_Task;
	struct MsgPort *pco_Port;
	ULONG           pco_dummy3;
	WORD            pco_dummy4;
};
#endif

// Special Value in pco_Flags
#ifndef COF_ACTIVE
#define COF_ACTIVE	0x02
#endif

#if !defined(__AROS__)

#ifdef __PPC__
#pragma pack(2)
#endif

struct BrokerCopy
{
	struct Node bc_Node;
	char bc_Name[CBD_NAMELEN];
	char bc_Title[CBD_TITLELEN];
	char bc_Descr[CBD_DESCRLEN];
	struct Task *bc_Task;
	APTR bc_Addr;
	LONG bc_Dummy2;
	UWORD bc_Flags;
};

#ifdef __PPC__
#pragma pack()
#endif

#endif // !__AROS__

//----------------------------------------------------------------------------

#if defined(__SASC)
int snprintf(char *, size_t, const char *, /*args*/ ...);
int vsnprintf(char *, size_t, const char *, va_list ap);
#endif /* __SASC */

//----------------------------------------------------------------------------

struct Exchange_LocaleInfo
{
	APTR li_LocaleBase;
	APTR li_Catalog;
	struct LocaleIFace *li_ILocale;
};

#endif	/* EXCHANGE_MODULE_H */
