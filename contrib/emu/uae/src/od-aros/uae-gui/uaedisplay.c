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

#define DEBUG 0
#include <zune/customclasses.h>
#include <aros/autoinit.h>
#include "uaedisplay.h"

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

            default:
                return DoSuperMethodA(CLASS, self, (Msg)message);
	}

        return TRUE;
    }),

    MUIM_Setup, struct MUIP_Setup *,
    ({
         if (!DoSuperMethodA(CLASS, self, (Msg)message))
	     return FALSE;

	 if (!GetCyberMapAttr(_screen(self)->RastPort.BitMap, CYBRMATTR_ISCYBERGFX))
	 {
	     __showerror("UAEDisplay: CyberGfx is required");
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
             __showerror("UAEDisplay: Unsupported bith depth %ld", data->bitdepth);
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
	     bug("Setting NoMenus to FALSE\n");
	     set(_win(self), MUIA_Window_NoMenus,  FALSE);
	     data->nomenu = FALSE;
	 }

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
