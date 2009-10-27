/**
 * Scout - The Amiga System Monitor
 *
 *------------------------------------------------------------------
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *------------------------------------------------------------------
 *
 * @author Andreas Gelhausen
 * @author Richard Körber <rkoerber@gmx.de>
 */

#if defined(__AROS__)
    #define NO_INLINE_STDARG
    // UrltextObject doesn't work without this
#endif
/*
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <mui/Urltext_mcc.h>
#include <tblib/tblib.h>
#include "scout.h"
#include "scout_about.h"
#include "scout_cat.h"
#include "scout_customclasses.h"
#include "scout_extras.h"
#include "scout_tags.h"
#include "scout_version.h"
*/
#include "system_headers.h"

struct AboutWinData {
    APTR awd_dummy;
};

STATIC APTR URLObject( void )
{
    APTR url;

    if (CheckMCCVersion(MUIC_Urltext, 18, 0, FALSE)) {
        url = UrltextObject, MUIA_Urltext_Url, HOMEPAGE, End;
    } else {
        url = TextObject, MUIA_Text_Contents, HOMEPAGE, End;
    }

    return url;
}

STATIC ULONG mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR abouttext,okButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_Window_Title, txtAboutTitle,
        WindowContents, VGroup,
            MUIA_Background, MUII_RequesterBack,
            Child, VGroup,
                TextFrame,
                MUIA_Background, MUII_TextBack,
                Child, abouttext = TextObject, End,
                Child, HGroup,
                    Child, MyHSpace(0),
                    Child, URLObject(),
                    Child, MyHSpace(0),
                End,
                Child, MyVSpace(4),
            End,
            Child, MyVSpace(4),
            Child, HGroup, MUIA_Group_SameWidth, TRUE,
                Child, MyHSpace(0),
                Child, okButton = MakeButton(txtMUIContinue),
                Child, MyHSpace(0),
            End,
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        set(obj, MUIA_Window_ActiveObject, okButton);
        MySetContents(abouttext, txtAboutText, vstring, release, COPYRIGHT, portname);

        DoMethod(obj,      MUIM_Notify, MUIA_Window_CloseRequest, TRUE,  MUIV_Notify_Application, 2, MUIM_Application_ReturnID, 1);
        DoMethod(okButton, MUIM_Notify, MUIA_Pressed,             FALSE, MUIV_Notify_Application, 2, MUIM_Application_ReturnID, 1);
    }

    return (ULONG)obj;
}

STATIC ULONG mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    set(obj, MUIA_Window_Open, FALSE);

    return DoSuperMethodA(cl, obj, msg);
}

STATIC ULONG mAbout( struct IClass *cl,
                     Object *obj,
                     Msg msg )
{
    APTR app;
    BOOL done = FALSE;
    ULONG result = 0;

    app = (APTR)xget(obj, MUIA_ApplicationObject);
    set(app, MUIA_Application_Sleep, TRUE);
    set(obj, MUIA_Window_Open, TRUE);

    while (!done) {
        ULONG signals;

        switch (DoMethod(app, MUIM_Application_NewInput, &signals)) {
            case MUIV_Application_ReturnID_Quit:
            case 1:
                done = TRUE;
                break;

            default:
                break;
        }

        if (!done && signals != 0) Wait(signals);
    }

    set(obj, MUIA_Window_Open, FALSE);
    set(app, MUIA_Application_Sleep, FALSE);

    return result;
}

DISPATCHER(AboutWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:              return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:          return (mDispose(cl, obj, (APTR)msg));
        case MUIM_AboutWin_About: return (mAbout(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}
DISPATCHER_END

APTR MakeAboutWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct AboutWinData), DISPATCHER_REF(AboutWinDispatcher));
}

