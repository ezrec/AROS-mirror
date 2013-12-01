// PrefsPlugins.h
// $Date$
// $Revision$


#ifndef	PREFSPLUGINS_H
#define	PREFSPLUGINS_H

#include <intuition/classusr.h>
#include "ScalosPrefs.h"

//----------------------------------------------------------------------------

BOOL PrefsPluginsInit(void);
void PrefsPluginsCleanup(void);
void CollectPrefsPlugins(struct SCAModule *app, CONST_STRPTR Path, CONST_STRPTR Pattern);
struct TagItem *GetPrefsPluginSubWindows(struct SCAModule *app);
void CreatePrefsPlugins(struct SCAModule *app);
void InsertPrefsPlugins(struct SCAModule *app, Object *pageGroup, Object *pageList);
ULONG DoMethodForAllPrefsPlugins(ULONG ArgCount, ULONG MethodID, ...);
void SetAttributeForAllPrefsPlugins(ULONG Attribute, ULONG Value);
void NotifyPrefsPluginsPageChange(ULONG ActivePage);
BOOL CheckMCCsForAllPrefsPlugins(void);

//----------------------------------------------------------------------------

#endif /* PREFSPLUGINS_H */
