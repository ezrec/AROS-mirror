// ReadWritePrefs.h
// $Date$
// $Revision$


#ifndef READWRITEPREFS_H
#define	READWRITEPREFS_H

//-----------------------------------------------------------------

#include "ScalosPrefs.h"

//-----------------------------------------------------------------

BOOL ReadWritePrefsInit(void);
void ReadWritePrefsCleanup(void);
IPTR mui_getv(APTR, ULONG );
LONG WriteScalosPrefs(struct SCAModule *, CONST_STRPTR PrefsFileName);
LONG ReadScalosPrefs(CONST_STRPTR PrefsFileName);
void UpdateGuiFromPrefs(struct SCAModule *app);
struct PluginDef *FindPlugin(struct SCAModule *app, BPTR fLock);
void RemovePlugin(struct SCAModule *app, const struct PluginDef *pdRemove);
BOOL AddPlugin(struct SCAModule *app, CONST_STRPTR FileName);
void ResetToDefaults(struct SCAModule *app);
void ReadIconFontPrefs(struct SCAModule *app);
void WriteFontPrefs(struct SCAModule *app, CONST_STRPTR FontPrefsName);

//-----------------------------------------------------------------

#define getv(o,a) mui_getv(o,a)

//-----------------------------------------------------------------

#endif /* READWRITEPREFS_H */
