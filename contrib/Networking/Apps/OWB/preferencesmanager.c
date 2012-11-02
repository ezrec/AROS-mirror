/*
    Copyright © 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/muimaster.h>
#include <libraries/mui.h>
#include <proto/intuition.h>
#include <proto/alib.h>
#include <proto/utility.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <aros/debug.h>
#include <zune/customclasses.h>
#include <fontconfig/fontconfig.h>
#include <libraries/asl.h>

#include "WebPreferencesZune.h"
#include "browserpreferences.h"

#include "preferencesmanager_private.h"
#include "preferencesmanager.h"

#include "locale.h"

static STRPTR* getFontFamilies()
{
    STRPTR *ret = NULL;
    
    if(!FcInit())
	return NULL;
    
    FcPattern *pat = FcPatternCreate();
    if(pat)
    {
        FcObjectSet *os = FcObjectSetBuild(FC_FAMILY, NULL);
        if(os)
        {
            FcFontSet *fs = FcFontList(NULL, pat, os);
         
            if(fs)
            {
        	STRPTR *families = AllocVec(sizeof(STRPTR) * (fs->nfont + 1), MEMF_ANY);
        	if(families)
        	{
                    int j;
                    int f = 0;
                    for (j = 0; j < fs->nfont; j++)
                    {
                        FcChar8 *family;
                        if(FcPatternGetString(fs->fonts[j], FC_FAMILY, 0, &family)==FcResultMatch)
                        {
                            families[f++] = StrDup((STRPTR) family);
                        }
                    }
                    families[f] = NULL;
                    ret = families;
        	}
            }
            FcObjectSetDestroy(os);
        }
        FcPatternDestroy(pat);
    }

    return ret;
}

static void freeFontFamilies(STRPTR *families)
{
    STRPTR *current = families;
    while(*current)
    {
	FreeVec(*current);
	current++;
    }
    FreeVec(families);
}

IPTR PreferencesManager__OM_NEW(struct IClass *cl, Object *self, struct opSet *msg)
{
    Object *bt_save, *bt_use, *bt_cancel, *bt_zune, *javaScriptEnabled, *loadsImagesAutomatically;
    Object *defaultFontSize, *defaultFixedFontSize, *minimumFontSize;
    Object *allowsAnimatedImages, *allowAnimatedImageLooping;
    Object *standardFontFamily, *fixedFontFamily, *serifFontFamily, *sansSerifFontFamily, *cursiveFontFamily, *fantasyFontFamily;
    Object *decodesPNGWithDatatypes, *decodesBMPWithDatatypes, *decodesGIFWithDatatypes, *decodesJPGWithDatatypes;
    Object *downloadDestination, *requestDownloadedFileName, *httpProxy, *useHttpProxy;
    Object *preferences = NULL;
    STRPTR *fontFamilies;
    struct TagItem *tag, *tags;
    
    STRPTR titles[] = { (STRPTR) _(MSG_PreferencesManager_Main), (STRPTR) _(MSG_PreferencesManager_Content), NULL };
    
    for (tags = msg->ops_AttrList; (tag = NextTagItem(&tags)); )
    {
        switch (tag->ti_Tag)
        {
            case MUIA_PreferencesManager_Preferences:
        	preferences = (Object*) tag->ti_Data;
        	break;
            default:
                continue; /* Don't supress non-processed tags */
        }
        tag->ti_Tag = TAG_IGNORE;
    }
    
    if(!preferences)
	return (IPTR) NULL;
    
    fontFamilies = getFontFamilies();
    if(!fontFamilies)
	return (IPTR) NULL;
    
    self = (Object *) DoSuperNewTags
    (
        cl, self, NULL,
    
        MUIA_Window_Title, (IPTR)_(MSG_PreferencesManager_Title),
    	WindowContents, (IPTR)(VGroup,
    	    MUIA_InnerLeft, 5,
    	    MUIA_InnerRight, 5,
    	    MUIA_InnerTop, 5,
    	    MUIA_InnerBottom, 5,
    	    Child, (IPTR)(RegisterObject,
    	        MUIA_Register_Titles, (IPTR)titles,
    	        Child, (IPTR)(VGroup,
    	            Child, (IPTR)(VGroup, GroupFrameT(_(MSG_PreferencesManager_Downloads)),
    	                Child, (IPTR)(HGroup,
        	                Child, (IPTR)(Label2(_(MSG_PreferencesManager_SaveFilesTo))),
        	                Child, (IPTR)(PopaslObject, 
        	                    MUIA_Popasl_Type, ASL_FileRequest,
        	                    MUIA_Popstring_String, (IPTR)(downloadDestination = StringObject, MUIA_Frame, MUIV_Frame_String, End),
        	                    MUIA_Popstring_Button, (IPTR)(PopButton(MUII_PopFile)),
        	                    ASLFR_DrawersOnly, TRUE,
        	                    End),
        	            End),
        	            Child, (IPTR)(HGroup,
        	                Child, (IPTR)(Label2(_(MSG_PreferencesManager_RequestDownloadedFileNameLabel))),
        	                Child, (IPTR)(requestDownloadedFileName = MUI_MakeObject(MUIO_Checkmark, FALSE)),
        	            End),
    	            End),
    	            Child, (IPTR)(ColGroup(3), GroupFrameT(_(MSG_PreferencesManager_ProxySettings)),
        	    	    Child, (IPTR)(useHttpProxy = MUI_MakeObject(MUIO_Checkmark, FALSE)),
    	                Child, (IPTR)(Label2(_(MSG_PreferencesManager_HttpProxy))), 
    	                Child, (IPTR)(httpProxy = StringObject, MUIA_Frame, MUIV_Frame_String, MUIA_Disabled, TRUE, End),
    	                End),
    	            Child, (IPTR)(VGroup, GroupFrameT(_(MSG_PreferencesManager_ZuneSettings)),
    	                Child, (IPTR)(bt_zune = SimpleButton(_(MSG_PreferencesManager_OpenZuneSettings))),
    	                End),
    	            End),
    	        Child, (IPTR)(HGroup,
    	            Child, (IPTR)(VGroup,
        	        Child, (IPTR)(ColGroup(2), GroupFrameT(_(MSG_PreferencesManager_JavaScript)),
        	    	    Child, (IPTR)(Label2(_(MSG_PreferencesManager_EnableJavaScript))), 
        	    	    Child, (IPTR)(javaScriptEnabled = MUI_MakeObject(MUIO_Checkmark, FALSE)),
        	    	    End),
        	        Child, (IPTR)(ColGroup(2), GroupFrameT(_(MSG_PreferencesManager_Images)),
        	    	    Child, (IPTR)(Label2(_(MSG_PreferencesManager_LoadImagesAutomatically))), 
        	    	    Child, (IPTR)(loadsImagesAutomatically = MUI_MakeObject(MUIO_Checkmark, FALSE)),
        	    	    Child, (IPTR)(Label2(_(MSG_PreferencesManager_AllowAnimatedImages))), 
        	    	    Child, (IPTR)(allowsAnimatedImages = MUI_MakeObject(MUIO_Checkmark, FALSE)),
        	    	    Child, (IPTR)(Label2(_(MSG_PreferencesManager_AllowLoopedAnimation))), 
        	    	    Child, (IPTR)(allowAnimatedImageLooping = MUI_MakeObject(MUIO_Checkmark, FALSE)),
        	    	    Child, (IPTR)(Label2(_(MSG_PreferencesManager_DecodeBMPWithDatatypes))), 
        	    	    Child, (IPTR)(decodesBMPWithDatatypes = MUI_MakeObject(MUIO_Checkmark, FALSE)),
        	    	    Child, (IPTR)(Label2(_(MSG_PreferencesManager_DecodePNGWithDatatypes))), 
        	    	    Child, (IPTR)(decodesPNGWithDatatypes = MUI_MakeObject(MUIO_Checkmark, FALSE)),
        	    	    Child, (IPTR)(Label2(_(MSG_PreferencesManager_DecodeGIFWithDatatypes))), 
        	    	    Child, (IPTR)(decodesGIFWithDatatypes = MUI_MakeObject(MUIO_Checkmark, FALSE)),
        	    	    Child, (IPTR)(Label2(_(MSG_PreferencesManager_DecodeJPGWithDatatypes))), 
        	    	    Child, (IPTR)(decodesJPGWithDatatypes = MUI_MakeObject(MUIO_Checkmark, FALSE)),
        	    	    End),
        	        End),
        	    Child, (IPTR)(ColGroup(2), GroupFrameT(_(MSG_PreferencesManager_Fonts)),
        	    	Child, (IPTR)(Label2(_(MSG_PreferencesManager_DefaultFontSize))), 
        	    	Child, (IPTR)(defaultFontSize = SliderObject, End),
        	    	Child, (IPTR)(Label2(_(MSG_PreferencesManager_MinimumFontSize))), 
        	    	Child, (IPTR)(minimumFontSize = SliderObject, End),
        	    	Child, (IPTR)(Label2(_(MSG_PreferencesManager_StandardFontFamily))), 
        	    	Child, (IPTR)(PoplistObject,
        	    	    MUIA_FixWidth, 100, /* To make lists wider */
        	    	    MUIA_Poplist_Array, (IPTR)fontFamilies, 
        	    	    MUIA_Popstring_String, (IPTR)(standardFontFamily = StringObject, MUIA_Frame, MUIV_Frame_String, MUIA_Disabled, TRUE, End),
        	    	    MUIA_Popstring_Button, (IPTR)(PopButton(MUII_PopUp)),
        	    	    End),
        	    	Child, (IPTR)(Label2(_(MSG_PreferencesManager_DefaultFixedFontSize))), 
        	    	Child, (IPTR)(defaultFixedFontSize = SliderObject, End),
        	    	Child, (IPTR)(Label2(_(MSG_PreferencesManager_FixedFontFamily))), 
        	    	Child, (IPTR)(PoplistObject, 
        	    	    MUIA_Poplist_Array, (IPTR)fontFamilies, 
        	    	    MUIA_Popstring_String, (IPTR)(fixedFontFamily = StringObject, MUIA_Frame, MUIV_Frame_String, End),
        	    	    MUIA_Popstring_Button, (IPTR)(PopButton(MUII_PopUp)),
        	    	    End),
        	    	Child, (IPTR)(Label2(_(MSG_PreferencesManager_SerifFontFamily))), 
        	    	Child, (IPTR)(PoplistObject, 
        	    	    MUIA_Poplist_Array, (IPTR)fontFamilies, 
        	    	    MUIA_Popstring_String, (IPTR)(serifFontFamily = StringObject, MUIA_Frame, MUIV_Frame_String, End),
        	    	    MUIA_Popstring_Button, (IPTR)PopButton(MUII_PopUp),
        	    	    End),
        	    	Child, (IPTR)(Label2(_(MSG_PreferencesManager_SansSerifFontFamily))), 
        	    	Child, (IPTR)(PoplistObject, 
        	    	    MUIA_Poplist_Array, (IPTR)fontFamilies, 
        	    	    MUIA_Popstring_String, (IPTR)(sansSerifFontFamily = StringObject, MUIA_Frame, MUIV_Frame_String, End),
        	    	    MUIA_Popstring_Button, (IPTR)(PopButton(MUII_PopUp)),
        	    	    End),
        	    	Child, (IPTR)(Label2(_(MSG_PreferencesManager_CursiveFontFamily))), 
        	    	Child, (IPTR)(PoplistObject, 
        	    	    MUIA_Poplist_Array, (IPTR)fontFamilies, 
        	    	    MUIA_Popstring_String, (IPTR)(cursiveFontFamily = StringObject, MUIA_Frame, MUIV_Frame_String, End),
        	    	    MUIA_Popstring_Button, (IPTR)(PopButton(MUII_PopUp)),
        	    	    End),
        	    	Child, (IPTR)(Label2(_(MSG_PreferencesManager_FantasyFontFamily))), 
        	    	Child, (IPTR)(PoplistObject, 
        	    	    MUIA_Poplist_Array, (IPTR)fontFamilies, 
        	    	    MUIA_Popstring_String, (IPTR)(fantasyFontFamily = StringObject, MUIA_Frame, MUIV_Frame_String, End),
        	    	    MUIA_Popstring_Button, (IPTR)(PopButton(MUII_PopUp)),
        	    	    End),
        	    	End),
        	    End),
        	End),
	    Child, (IPTR)(HGroup,
	        Child, (IPTR)(bt_save = SimpleButton(_(MSG_PreferencesManager_Save))),
	        Child, (IPTR)(bt_use = SimpleButton(_(MSG_PreferencesManager_Use))),
	        Child, (IPTR)(bt_cancel = SimpleButton(_(MSG_PreferencesManager_Cancel))),
	        End),
	    End),
        TAG_MORE, (IPTR) msg->ops_AttrList	
    );

    if(!self)
    {
	freeFontFamilies(fontFamilies);
	return (IPTR) NULL;
    }

    struct PreferencesManager_DATA *data = (struct PreferencesManager_DATA *) INST_DATA(cl, self);
    
    data->preferences = preferences;
    data->fontFamilies = fontFamilies;
    
    /* Initialize default values and ObjectIDs */
    set(javaScriptEnabled, MUIA_Selected, XGET(preferences, MUIA_WebPreferences_JavaScriptEnabled));
    set(javaScriptEnabled, MUIA_ObjectID, 1);
    data->javaScriptEnabled = javaScriptEnabled;

    set(loadsImagesAutomatically, MUIA_Selected, XGET(preferences, MUIA_WebPreferences_LoadsImagesAutomatically));
    set(loadsImagesAutomatically, MUIA_ObjectID, 2);
    data->loadsImagesAutomatically = loadsImagesAutomatically;

    set(defaultFontSize, MUIA_Numeric_Value, XGET(preferences, MUIA_WebPreferences_DefaultFontSize));
    set(defaultFontSize, MUIA_ObjectID, 3);
    data->defaultFontSize = defaultFontSize;

    set(defaultFixedFontSize, MUIA_Numeric_Value, XGET(preferences, MUIA_WebPreferences_DefaultFixedFontSize));
    set(defaultFixedFontSize, MUIA_ObjectID, 4);
    data->defaultFixedFontSize = defaultFixedFontSize;

    set(minimumFontSize, MUIA_Numeric_Value, XGET(preferences, MUIA_WebPreferences_MinimumFontSize));
    set(minimumFontSize, MUIA_ObjectID, 5);
    data->minimumFontSize = minimumFontSize;

    set(allowsAnimatedImages, MUIA_Selected, XGET(preferences, MUIA_WebPreferences_AllowsAnimatedImages));
    set(allowsAnimatedImages, MUIA_ObjectID, 6);
    data->allowsAnimatedImages = allowsAnimatedImages;

    set(allowAnimatedImageLooping, MUIA_Selected, XGET(preferences, MUIA_WebPreferences_AllowAnimatedImageLooping));
    set(allowAnimatedImageLooping, MUIA_ObjectID, 7);
    data->allowAnimatedImageLooping = allowAnimatedImageLooping;

    set(standardFontFamily, MUIA_String_Contents, XGET(preferences, MUIA_WebPreferences_StandardFontFamily));
    set(standardFontFamily, MUIA_ObjectID, 8);
    data->standardFontFamily = standardFontFamily;

    set(fixedFontFamily, MUIA_String_Contents, XGET(preferences, MUIA_WebPreferences_FixedFontFamily));
    set(fixedFontFamily, MUIA_ObjectID, 9);
    data->fixedFontFamily = fixedFontFamily;

    set(serifFontFamily, MUIA_String_Contents, XGET(preferences, MUIA_WebPreferences_SerifFontFamily));
    set(serifFontFamily, MUIA_ObjectID, 10);
    data->serifFontFamily = serifFontFamily;

    set(sansSerifFontFamily, MUIA_String_Contents, XGET(preferences, MUIA_WebPreferences_SansSerifFontFamily));
    set(sansSerifFontFamily, MUIA_ObjectID, 11);
    data->sansSerifFontFamily = sansSerifFontFamily;

    set(cursiveFontFamily, MUIA_String_Contents, XGET(preferences, MUIA_WebPreferences_CursiveFontFamily));
    set(cursiveFontFamily, MUIA_ObjectID, 12);
    data->cursiveFontFamily = cursiveFontFamily;

    set(fantasyFontFamily, MUIA_String_Contents, XGET(preferences, MUIA_WebPreferences_FantasyFontFamily));
    set(fantasyFontFamily, MUIA_ObjectID, 13);
    data->fantasyFontFamily = fantasyFontFamily;

    set(decodesBMPWithDatatypes, MUIA_Selected, XGET(preferences, MUIA_WebPreferences_DecodesBMPWithDatatypes));
    set(decodesBMPWithDatatypes, MUIA_ObjectID, 14);
    data->decodesBMPWithDatatypes = decodesBMPWithDatatypes;

    set(decodesJPGWithDatatypes, MUIA_Selected, XGET(preferences, MUIA_WebPreferences_DecodesJPGWithDatatypes));
    set(decodesJPGWithDatatypes, MUIA_ObjectID, 15);
    data->decodesJPGWithDatatypes = decodesJPGWithDatatypes;

    set(decodesGIFWithDatatypes, MUIA_Selected, XGET(preferences, MUIA_WebPreferences_DecodesGIFWithDatatypes));
    set(decodesGIFWithDatatypes, MUIA_ObjectID, 16);
    data->decodesGIFWithDatatypes = decodesGIFWithDatatypes;

    set(decodesPNGWithDatatypes, MUIA_Selected, XGET(preferences, MUIA_WebPreferences_DecodesPNGWithDatatypes));
    set(decodesPNGWithDatatypes, MUIA_ObjectID, 17);
    data->decodesPNGWithDatatypes = decodesPNGWithDatatypes;
    
    set(downloadDestination, MUIA_String_Contents, XGET(preferences, MUIA_BrowserPreferences_DownloadDestination));
    set(downloadDestination, MUIA_ObjectID, 18);
    data->downloadDestination = downloadDestination;

    set(useHttpProxy, MUIA_ObjectID, 19);
    data->useHttpProxy = useHttpProxy;

    set(httpProxy, MUIA_ObjectID, 20);
    data->httpProxy = httpProxy;

    set(requestDownloadedFileName, MUIA_Selected, XGET(preferences, MUIA_BrowserPreferences_RequestDownloadedFileName));
    set(requestDownloadedFileName, MUIA_ObjectID, 21);
    data->requestDownloadedFileName = requestDownloadedFileName;

    /* Close window with close gadget */
    DoMethod(self, MUIM_Notify, MUIA_Window_CloseRequest, (IPTR) TRUE,
        (IPTR) self,  (IPTR) 3,
        MUIM_Set, MUIA_Window_Open, (IPTR) FALSE);

    DoMethod(bt_save, MUIM_Notify, MUIA_Pressed, FALSE,
	(IPTR) self, 1,
	MUIM_PreferencesManager_Save);

    DoMethod(bt_use, MUIM_Notify, MUIA_Pressed, FALSE,
	(IPTR) self, 1,
	MUIM_PreferencesManager_Use);

    DoMethod(bt_cancel, MUIM_Notify, MUIA_Pressed, FALSE,
	(IPTR) self, 3,
	MUIM_Set, MUIA_Window_Open, (IPTR) FALSE);

    DoMethod(bt_zune, MUIM_Notify, MUIA_Pressed, FALSE,
	(IPTR) MUIV_Notify_Application, 1,
	MUIM_Application_OpenConfigWindow);

    DoMethod(useHttpProxy, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
	(IPTR) httpProxy, 3,
	MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue);

    return (IPTR) self;
}

IPTR PreferencesManager__OM_DISPOSE(struct IClass *cl, Object *obj, Msg msg)
{
    struct PreferencesManager_DATA *data = (struct PreferencesManager_DATA *) INST_DATA(cl, obj);
    
    freeFontFamilies(data->fontFamilies);
    
    return DoSuperMethodA(cl,obj,msg);
}

IPTR PreferencesManager__MUIM_PreferencesManager_Save(Class *cl, Object *obj, Msg message)
{
    DoMethod(obj, MUIM_PreferencesManager_Use);
    DoMethod(_app(obj), MUIM_Application_Save, MUIV_Application_Save_ENVARC);
    return TRUE;
}

static void SetPreferenceData(Class *cl, Object *obj)
{
    struct PreferencesManager_DATA *data = (struct PreferencesManager_DATA *) INST_DATA(cl, obj);
    set(data->preferences, MUIA_WebPreferences_JavaScriptEnabled, XGET(data->javaScriptEnabled, MUIA_Selected));
    set(data->preferences, MUIA_WebPreferences_LoadsImagesAutomatically, XGET(data->loadsImagesAutomatically, MUIA_Selected));
    set(data->preferences, MUIA_WebPreferences_DefaultFontSize, XGET(data->defaultFontSize, MUIA_Numeric_Value));
    set(data->preferences, MUIA_WebPreferences_DefaultFixedFontSize, XGET(data->defaultFixedFontSize, MUIA_Numeric_Value));
    set(data->preferences, MUIA_WebPreferences_MinimumFontSize, XGET(data->minimumFontSize, MUIA_Numeric_Value));
    set(data->preferences, MUIA_WebPreferences_AllowsAnimatedImages, XGET(data->allowsAnimatedImages, MUIA_Selected));
    set(data->preferences, MUIA_WebPreferences_AllowAnimatedImageLooping, XGET(data->allowAnimatedImageLooping, MUIA_Selected));
    set(data->preferences, MUIA_WebPreferences_StandardFontFamily, XGET(data->standardFontFamily, MUIA_String_Contents));
    set(data->preferences, MUIA_WebPreferences_FixedFontFamily, XGET(data->fixedFontFamily, MUIA_String_Contents));
    set(data->preferences, MUIA_WebPreferences_SerifFontFamily, XGET(data->serifFontFamily, MUIA_String_Contents));
    set(data->preferences, MUIA_WebPreferences_SansSerifFontFamily, XGET(data->sansSerifFontFamily, MUIA_String_Contents));
    set(data->preferences, MUIA_WebPreferences_CursiveFontFamily, XGET(data->cursiveFontFamily, MUIA_String_Contents));
    set(data->preferences, MUIA_WebPreferences_FantasyFontFamily, XGET(data->fantasyFontFamily, MUIA_String_Contents));
    set(data->preferences, MUIA_WebPreferences_DecodesPNGWithDatatypes, XGET(data->decodesPNGWithDatatypes, MUIA_Selected));
    set(data->preferences, MUIA_WebPreferences_DecodesJPGWithDatatypes, XGET(data->decodesJPGWithDatatypes, MUIA_Selected));
    set(data->preferences, MUIA_WebPreferences_DecodesGIFWithDatatypes, XGET(data->decodesGIFWithDatatypes, MUIA_Selected));
    set(data->preferences, MUIA_WebPreferences_DecodesBMPWithDatatypes, XGET(data->decodesBMPWithDatatypes, MUIA_Selected));
    set(data->preferences, MUIA_BrowserPreferences_DownloadDestination, XGET(data->downloadDestination, MUIA_String_Contents));
    set(data->preferences, MUIA_BrowserPreferences_RequestDownloadedFileName, XGET(data->requestDownloadedFileName, MUIA_Selected));
    set(data->preferences, MUIA_WebPreferences_CookieJarFileName, "PROGDIR:cookies.db"); /* Hardcoded for now */
    if(XGET(data->useHttpProxy, MUIA_Selected))
	setenv("http_proxy", (const char *)XGET(data->httpProxy, MUIA_String_Contents), 1);
    else
	unsetenv("http_proxy");
}

IPTR PreferencesManager__MUIM_PreferencesManager_Use(Class *cl, Object *obj, Msg message)
{
    SetPreferenceData(cl, obj);
    DoMethod(_app(obj), MUIM_Application_Save, MUIV_Application_Save_ENV);
    set(obj, MUIA_Window_Open, FALSE);
    return TRUE;
}

IPTR PreferencesManager__MUIM_PreferencesManager_Load(Class *cl, Object *obj, Msg message)
{
    DoMethod(_app(obj), MUIM_Application_Load, MUIV_Application_Load_ENV);
    SetPreferenceData(cl, obj);
    return TRUE;
}

ZUNE_CUSTOMCLASS_5(
    PreferencesManager, NULL, MUIC_Window, NULL,
    OM_NEW, struct opSet*,
    OM_DISPOSE, Msg,
    MUIM_PreferencesManager_Load, Msg,
    MUIM_PreferencesManager_Use, Msg,
    MUIM_PreferencesManager_Save, Msg
)
