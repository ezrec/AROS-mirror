#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/cybergraphics.h>
#include <proto/intuition.h>
#include <proto/utility.h>

#include <exec/types.h>
#include <exec/memory.h>
#include <dos/dos.h>
#include <cybergraphx/cybergraphics.h>
#include <utility/tagitem.h>

#include <zune/customclasses.h>

//#include </work/AROS/workbench/libs/muimaster/customclasses.h>

#include "aros-gui.h"

#include "sysconfig.h"
#include "sysdeps.h"

#include "config.h"
#include "options.h"
#include "uae.h"
#include "gui.h"

#define _between(a,x,b)  ((x)>=(a) && (x)<=(b))
#define _isinobject(x,y) (_between(_mleft(self),(x),_mright (self)) \
                          && _between(_mtop(self) ,(y),_mbottom(self)))

ZUNE_CUSTOMCLASS_INLINE_10
(
    UAEDisplay, NULL, MUIC_Area, NULL,
    {
        int   width;
        int   height;
	int   modifysize;

	void *memory;
	int   bytesperrow;
	int   bytesperpix;
	int   bitdepth;
	int   maxwidth;
	int   maxheight;

	int   update_top;
	int   update_bottom;

	void (*eventhandler)(struct IntuiMessage *);

	int   nomenu;

	struct MUI_EventHandlerNode ehnode;
    },

    __MUIM_UAEDisplay_ReallocMemory, Msg,
    ({
         if (data->memory) FreeVec(data->memory);
	 data->memory = NULL;

         if (data->width && data->height)
	 {
	     data->memory = AllocVec
	     (
	 	 data->width * data->height * data->bytesperpix, MEMF_CLEAR
	     );
	     if (!data->memory)
	     {
	         SetIoErr(ERROR_NO_FREE_STORE);
	         return FALSE;
	     }
	 }

	 data->bytesperrow = data->bytesperpix * data->width;

	 return TRUE;
    }),

    OM_NEW, struct opSet *,
    ({
	 self = (Object *)DoSuperNewTags
	 (
	     CLASS, self, NULL,
	     MUIA_FillArea, FALSE,
	     TAG_MORE,      (IPTR)message->ops_AttrList
	 );

         data = INST_DATA(CLASS, self);

	 struct TagItem *tags, *tag;

         for (tags = message->ops_AttrList; (tag = NextTagItem(&tags)); )
         {
             switch (tag->ti_Tag)
             {
                 case MUIA_UAEDisplay_Width:
                     data->width = tag->ti_Data;
                     break;

		 case MUIA_UAEDisplay_Height:
                     data->height = tag->ti_Data;
                     break;

	         case MUIA_UAEDisplay_EventHandler:
	             data->eventhandler = (APTR)tag->ti_Data;
		     break;
             }
         }

	 return (IPTR)self;
    }),

    OM_SET, struct opSet *,
    ({
	 struct TagItem *tags, *tag;

	 BOOL sizechanged = FALSE;

	 for (tags = message->ops_AttrList; (tag = NextTagItem(&tags)); )
         {
             switch (tag->ti_Tag)
             {
                 case MUIA_UAEDisplay_Width:
		     if (data->width != (int)tag->ti_Data)
		     {
                         data->width = tag->ti_Data;
			 sizechanged = TRUE;
	             }
                     break;

		 case MUIA_UAEDisplay_Height:
		     if (data->height != (int)tag->ti_Data)
		     {
                         data->height = tag->ti_Data;
			 sizechanged  = TRUE;
	             }
                     break;

		 case MUIA_UAEDisplay_EventHandler:
	             data->eventhandler = (APTR)tag->ti_Data;
		     break;
             }
         }

	 if (sizechanged)
	     if (!DoMethod(self, __MUIM_UAEDisplay_ReallocMemory))
	         return FALSE;

	 /* Make the gadget resize according to the new values */
	 DoMethod(_parent(self), MUIM_Group_InitChange);
	 data->modifysize = TRUE;
	 DoMethod(_parent(self), MUIM_Group_ExitChange);
	 DoMethod(_parent(self), MUIM_Group_InitChange);
	 data->modifysize = FALSE;
	 DoMethod(_parent(self), MUIM_Group_ExitChange);

         return DoSuperMethodA(CLASS, self, (Msg)message);
    }),

    OM_GET, struct opGet *,
    ({
        switch (message->opg_AttrID)
	{
	    case MUIA_UAEDisplay_Memory:
	        *(message->opg_Storage) = (IPTR)data->memory;
		break;

	    case MUIA_UAEDisplay_BytesPerPix:
	        *(message->opg_Storage) = (IPTR)data->bytesperpix;
		break;

	    case MUIA_UAEDisplay_BytesPerRow:
	        *(message->opg_Storage) = (IPTR)data->bytesperrow;
		break;

	    case MUIA_UAEDisplay_BitDepth:
	        *(message->opg_Storage) = (IPTR)data->bitdepth;
		break;

	    case MUIA_UAEDisplay_Width:
	        *(message->opg_Storage) = (IPTR)data->width;
		break;

	    case MUIA_UAEDisplay_Height:
	        *(message->opg_Storage) = (IPTR)data->height;
		break;

	    case MUIA_UAEDisplay_MaxWidth:
	        *(message->opg_Storage) = (IPTR)data->maxwidth;
		break;

	    case MUIA_UAEDisplay_MaxHeight:
	        *(message->opg_Storage) = (IPTR)data->maxheight;
		break;

	    case MUIA_UAEDisplay_EventHandler:
	        *(message->opg_Storage) = (IPTR)data->eventhandler;
		break;
	}

        return DoSuperMethodA(CLASS, self, (Msg)message);
    }),

    MUIM_Setup, struct MUIP_Setup *,
    ({
         if (!DoSuperMethodA(CLASS, self, (Msg)message))
	     return FALSE;

	 if (!GetCyberMapAttr(_screen(self)->RastPort.BitMap, CYBRMATTR_ISCYBERGFX))
	 {
	     fprintf(stderr, "**error**: CyberGfx is required\n");
	     return FALSE;
	 }

         data->ehnode.ehn_Object = self;
         data->ehnode.ehn_Class  = CLASS;
         data->ehnode.ehn_Events = IDCMP_MOUSEBUTTONS | IDCMP_MOUSEMOVE | IDCMP_RAWKEY |
	                           IDCMP_ACTIVEWINDOW;

	 DoMethod(_win(self), MUIM_Window_AddEventHandler, (IPTR)&data->ehnode);

	 data->bitdepth = GetCyberMapAttr(_screen(self)->RastPort.BitMap, CYBRMATTR_DEPTH);

         if(data->bitdepth <= 8)
         {
             fprintf(stderr, "**error**: Unsupported bith depth %ld\n", data->bitdepth);
	     goto error;
         }

	 data->maxwidth    = _screen(self)->Width;
	 data->maxheight   = _screen(self)->Height;
         data->bytesperpix = GetCyberMapAttr(_screen(self)->RastPort.BitMap, CYBRMATTR_BPPIX);

	 if (!DoMethod(self, __MUIM_UAEDisplay_ReallocMemory))
	     goto error;

	 return TRUE;

     error:

	 CoerceMethod(CLASS, self, MUIM_Cleanup);

	 return FALSE;
    }),

    MUIM_Cleanup, struct MUIP_Cleanup *,
    ({
	 DoMethod(_win(self), MUIM_Window_RemEventHandler, (IPTR)&data->ehnode);

	 if (data->memory)
	     FreeVec(data->memory);

	 return DoSuperMethodA(CLASS, self, (Msg)message);
    }),

    MUIM_HandleEvent, struct MUIP_HandleEvent *,
    ({
         if (!message->imsg || !data->eventhandler)
	     return 0;

	 if (_isinobject(message->imsg->MouseX, message->imsg->MouseY))
	 {
	     message->imsg->MouseX -= _mleft(self);
	     message->imsg->MouseY -= _mtop(self);

	     if (!data->nomenu)
	     {
	         set(_win(self), MUIA_Window_NoMenus,  TRUE);
		 data->nomenu = TRUE;
	     }

	     data->eventhandler(message->imsg);

	     return MUI_EventHandlerRC_Eat;
	 }

         if (data->nomenu)
	 {
	     set(_win(self), MUIA_Window_NoMenus,  FALSE);
	     data->nomenu = FALSE;
	 }


//	 printf("Received an event NOT for me: %d\n", message->muikey);

	 return 0;
    }),

    MUIM_AskMinMax, struct MUIP_AskMinMax *,
    ({
        DoSuperMethodA(CLASS, self, (Msg)message);

	if (data->modifysize)
	{
            message->MinMaxInfo->MinWidth  += data->width;
            message->MinMaxInfo->MinHeight += data->height;
	}
	else
        {
	    message->MinMaxInfo->MinWidth  += 0;
            message->MinMaxInfo->MinHeight += 0;
        }

	message->MinMaxInfo->DefWidth  += data->width;
        message->MinMaxInfo->DefHeight += data->height;
        message->MinMaxInfo->MaxWidth  += data->width;
        message->MinMaxInfo->MaxHeight += data->height;

	return TRUE;
    }),

    MUIM_Draw, struct MUIP_Draw *,
    ({
        WORD top, bottom;

	if (!_mwidth(self) || !_mheight(self) || !data->memory)
	    return TRUE;

	if (message->flags & MADF_DRAWUPDATE)
	{
	    top    = data->update_top;
	    bottom = data->update_bottom;
	}
	else
	{
	    top    = 0;
            bottom = data->height-1;
	}

	if (bottom >= _mheight(self))
	    bottom = _mheight(self)-1;

	if (top > bottom)
	    return TRUE;

        WritePixelArray
        (
            data->memory, 0, top, data->bytesperrow,
            _rp(self), _mleft(self), _mtop(self) + top,
	    _mwidth(self), bottom - top + 1, RECTFMT_RAW
        );

        return TRUE;
    }),

    MUIM_UAEDisplay_Update, struct MUIP_UAEDisplay_Update *,
    ({

	if (message->top >= data->height || message->bottom <= 0)
	    return TRUE;

	if (message->top <= 0)
	    message->top = 0;

        data->update_top    = message->top;
	data->update_bottom = message->bottom;

	MUI_Redraw(self, MADF_DRAWUPDATE);

        return TRUE;
    })
);


static void sigchldhandler(int foo)
{
}

Object *uaedisplay;
static Object *application;
static ULONG uaedisplay_signals;

int gui_init (void)
{
    Object *window;

    application = ApplicationObject,
        SubWindow, (IPTR)window = WindowObject,
            MUIA_Window_Title,    (IPTR)"UAE for AROS",
            MUIA_Window_Activate, TRUE,

            WindowContents, (IPTR)VGroup,
                Child, (IPTR)uaedisplay = UAEDisplayObject,
		    MUIA_UAEDisplay_Width,  1,
                End,
	    End,
  	    MUIA_Window_DefaultObject, (IPTR)uaedisplay,
        End,
    End;
    if (!application)
        return -2;

    /* Just in case abort() is invoked, in which case we have to make
       sure that the application is disposed.  */
    atexit(gui_exit);

    set(window, MUIA_Window_Open, TRUE);
    DoMethod
    (
        window,
        MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
        (IPTR)application, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit
    );

    if
    (
        DoMethod(application, MUIM_Application_NewInput, (IPTR) &uaedisplay_signals) ==
	(IPTR)MUIV_Application_ReturnID_Quit
    )
    {
        uae_quit();
    }

    SetTaskPri(FindTask(NULL), -5);
    return 0;
}

int gui_update (void)
{
    return 0;
}

void gui_exit (void)
{
    if (application)
    {
        MUI_DisposeObject(application);
	application = NULL;
    }
}

void gui_fps (int x)
{
}

void gui_led (int led, int on)
{
}

void gui_filename (int num, const char *name)
{
}

static void getline (char *p)
{
}


void gui_handle_events (void)
{
    if (uaedisplay_signals)
    {
	ULONG signals_got = SetSignal(0,0);

	uaedisplay_signals = signals_got & (uaedisplay_signals | SIGBREAKF_CTRL_C);

	if
	(
	    uaedisplay_signals
	  &&(
	        (uaedisplay_signals & SIGBREAKF_CTRL_C)
              ||(DoMethod(application, MUIM_Application_NewInput, (IPTR) &uaedisplay_signals) ==
                 (IPTR)MUIV_Application_ReturnID_Quit)
	    )
	)
	{
            uae_quit();
	}
    }
}

void gui_changesettings (void)
{
}

void gui_update_gfx (void)
{
}

void gui_lock (void)
{
}

void gui_unlock (void)
{
}
