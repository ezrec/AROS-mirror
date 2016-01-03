// Preferences-aros_init.c


#include <proto/exec.h>

#include "Preferences_base.h"
#include "Preferences.h"


static BOOL Initlib(struct PreferencesBase *PreferencesBase)
{
    PreferencesBase->prb_Initialized = FALSE; //TODO: Opening library PreferencesBase seems to be called anyway even if this is TRUE. Why does it happen?

    if (!PreferencesInit(PreferencesBase))
    {
        return FALSE;
    }

    return TRUE;
}

static BOOL Openlib(struct PreferencesBase *PreferencesBase)
{
    if (!PreferencesBase->prb_Initialized)
    {
        if (!PreferencesOpen(PreferencesBase))
        {
            return FALSE;
        }

        PreferencesBase->prb_Initialized = TRUE;
    }

    return TRUE;
}

static BOOL Expungelib(struct PreferencesBase *PreferencesBase)
{
    PreferencesCleanup(PreferencesBase);
    return TRUE;
}

ADD2INITLIB(Initlib, 0);
ADD2OPENLIB(Openlib, 0);
ADD2EXPUNGELIB(Expungelib, 0); 

