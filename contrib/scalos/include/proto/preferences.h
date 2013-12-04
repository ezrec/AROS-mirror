#ifndef PROTO_PREFERENCES_H
#define PROTO_PREFERENCES_H

#ifdef __AROS__
#error You must include AROS headers
#endif

#include <exec/types.h>
extern struct Library *PreferencesBase;
#ifdef __amigaos4__
#include <interfaces/preferences.h>
extern struct PreferencesIFace *IPreferences;
#endif

#include <clib/preferences_protos.h>

#ifdef __amigaos4__
	#ifdef __USE_INLINE__
		#include <inline4/preferences.h>
	#endif
#elif defined(__GNUC__)
	#ifdef __AROS__
		#include <defines/preferences.h>
	#else
		#include <inline/preferences.h>
	#endif
#elif defined(VBCC)
	#include <inline/preferences_protos.h>
#else
	#include <pragmas/preferences_pragmas.h>
#endif /* __VBCC__ */

#endif /* PROTO_PREFERENCES_H */

