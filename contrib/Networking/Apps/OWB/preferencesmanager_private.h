#ifndef _PREFERENCES_PRIVATE_H_
#define _PREFERENCES_PRIVATE_H_

/*
    Copyright © 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/types.h>
#include <utility/hooks.h>
#include <libraries/mui.h>

/*** Instance data **********************************************************/
struct PreferencesManager_DATA
{
    Object *preferences;
    STRPTR *fontFamilies;
    Object *javaScriptEnabled;
    Object *loadsImagesAutomatically;
    Object *defaultFontSize;
    Object *defaultFixedFontSize;
    Object *minimumFontSize;
    Object *allowsAnimatedImages;
    Object *allowAnimatedImageLooping;
    Object *standardFontFamily, *fixedFontFamily, *serifFontFamily, *sansSerifFontFamily, *cursiveFontFamily, *fantasyFontFamily;
    Object *decodesPNGWithDatatypes, *decodesBMPWithDatatypes, *decodesGIFWithDatatypes, *decodesJPGWithDatatypes;
    Object *downloadDestination, *useHttpProxy, *httpProxy;
};

#endif /* _PREFERENCES_PRIVATE_H_ */
