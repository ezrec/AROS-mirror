/* devsupp.h: */
#ifndef DEVSUPP_H
#define DEVSUPP_H

#include "debug.h"

extern char g_device[80];
extern short g_unit;
extern unsigned long g_memory_type;
extern short g_map_to_lowercase;
extern short g_use_rock_ridge;
extern int g_trackdisk;
extern int g_std_buffers;
extern int g_file_buffers;
extern CDROM *g_cd;

#if !defined(NDEBUG) || defined(DEBUG_SECTORS)
extern PORT *Dbport;
extern PORT *Dback;
extern short DBDisable;
extern MSG DummyMsg;

#if !(defined(__AROS__) || defined(__MORPHOS__))
void dbinit (void);
void dbuninit (void);
void dbprintf (char *, ...);
#endif
#endif

#ifdef __AROS__
int Get_Startup (struct FileSysStartupMsg *);
#else
int Get_Startup (LONG p_startup);
#endif

#define HANDLER_VERSION "cdrom.handler 1.15 (2002-07-07)\n"

int Handle_Control_Packet (ULONG p_type, ULONG p_par1, ULONG p_par2);

#endif /* DEVSUPP_H */
