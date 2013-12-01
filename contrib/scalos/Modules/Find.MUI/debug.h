// debug.h
// $Date$
// $Revision$

#ifndef DEBUG_H
#define	DEBUG_H

//----------------------------------------------------------------------------

#define	d1(x)	;
#define	d2(x)	x;

#define	debugLock_d1(LockName) ;
#define	debugLock_d2(LockName) \
	{\
	char xxName[200];\
	strcpy(xxName, "");\
	NameFromLock((LockName), xxName, sizeof(xxName));\
	KPrintF("%s/%s/%ld: " #LockName "=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, LockName, xxName);\
	}

// from debug.lib
extern int kprintf(const char *fmt, ...);
extern int KPrintF(const char *fmt, ...);

//----------------------------------------------------------------------------

#endif /* DEBUG_H */
