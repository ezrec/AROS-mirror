// wb39proto.h
// $Date$
// $Revision$


#ifndef	SCALOS_HELPER_H_INCLUDED
#define	SCALOS_HELPER_H_INCLUDED

#include <defs.h>

ULONG MenuOpenParentWindow(struct ScaWindowStruct *swi);
ULONG MenuUnselectAll(struct ScaWindowStruct *swi);
ULONG MenuSetViewModeByIcon(struct ScaWindowStruct *swi);
ULONG MenuSetViewModeByName(struct ScaWindowStruct *swi);
ULONG MenuSetViewModeBySize(struct ScaWindowStruct *swi);
ULONG MenuSetViewModeByDate(struct ScaWindowStruct *swi);
ULONG MenuSetViewModeByType(struct ScaWindowStruct *swi);
ULONG MenuShowAllFiles(struct ScaWindowStruct *swi);
BOOL ParseWBPrefs(CONST_STRPTR filename);
BOOL ReadScalosPrefs(void);
struct ScaWindowStruct *GetIconNodeOpenWindow(BPTR dirLock, struct ScaIconNode *sIcon);
struct ScaIconNode *GetIconByName(struct ScaWindowStruct *swi, CONST_STRPTR IconName);
struct ScaWindowStruct *FindWindowByLock(BPTR xLock);
void MakeIconVisible(struct ScaWindowStruct *swi, struct ScaIconNode *sIcon);
void MoveIcon(struct ScaWindowStruct *swi, struct ScaIconNode *sIcon, LONG x, LONG y);
void SelectIcon(struct ScaWindowStruct *swi, struct ScaIconNode *sIcon, BOOL Selected);
struct ScaWindowStruct *FindWindowByName(CONST_STRPTR WinName);
BOOL CloseScalosWindow(struct ScaWindowStruct *swi);
BOOL UpdateScalosWindow(struct ScaWindowStruct *swi);
struct ScaWindowStruct *GetNextWindow(struct ScaWindowStruct *swi);
struct ScaWindowStruct *GetPrevWindow(struct ScaWindowStruct *swi);
BOOL DeleteDirectory(CONST_STRPTR Path);
BOOL RemScalosIcon(BPTR Lock, CONST_STRPTR Name);
BOOL isIconSelected(const struct ScaIconNode *icon);
struct ScaRootList *GetScalosRootList(void);
ULONG MenuNewDrawer(CONST_STRPTR Name);
ULONG MenuUpdateScalosWindow(struct ScaWindowStruct *swi);
ULONG MenuCloseScalosWindow(struct ScaWindowStruct *swi);
CONST_STRPTR GetIconName(struct ScaIconNode *in);
struct ScaWindowStruct *WaitOpen(struct ScaWindowStruct *ws);


#endif /* SCALOS_HELPER_H_INCLUDED */
