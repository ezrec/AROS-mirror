/***************************************************************************

 NList.mcc - New List MUI Custom Class
 Registered MUI class, Serial Number:

 Copyright (C) 1996-2004 by Gilles Masson,
                            Carsten Scholling <aphaso@aphaso.de>,
                            Przemyslaw Grunchala,
                            Sebastian Bauer <sebauer@t-online.de>,
                            Jens Langner <Jens.Langner@light-speed.de>

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 NList classes Support Site:  http://www.sf.net/projects/nlist-classes

 $Id$

***************************************************************************/

#include "private.h"

#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))

static ULONG mNL_Show(struct IClass *cl,Object *obj,struct MUIP_HandleInput *msg)
{
  ULONG retval;
  register struct NLData *data = INST_DATA(cl,obj);

  STACK_CHECK;
  D(bug("NL: mNL_Show() \n"));

  data->rp = _rp(obj);

  data->SHOW = TRUE;
  data->moves = FALSE;

  if (data->NList_Disabled)
    data->NList_Disabled = 1;

  if (data->nodraw)
    return (DoSuperMethodA(cl,obj,(Msg) msg));

  if (data->do_draw_all)
  { data->adjustbar_old = -1;
    if (data->adjustbar == -3)
      data->adjustbar = -2;
  }

  data->refreshing = FALSE;
  data->badrport = FALSE;

  data->display_ptr = NULL;
  data->drag_border = FALSE;
  data->DisplayArray[1] = (char *) -2;

  data->ScrollBarsTime = 1;

  /* GetImages must be done before DoSuperMethodA */
  GetImages(obj,data);

  data->do_updatesb = data->do_images = TRUE;

  retval = DoSuperMethodA(cl,obj,(Msg) msg);

  if (data->ScrollBarsPos == -3)
  { if (!(LIBVER(GfxBase) >= 39))
      NL_CreateImages(obj,data);
    data->ScrollBarsPos = -2;
  }

  if (data->VertPropObject)
  { if (data->NList_Smooth)
      set(data->VertPropObject,MUIA_Prop_DoSmooth, TRUE);
    else
      set(data->VertPropObject,MUIA_Prop_DoSmooth, FALSE);
  }

  GetNImage_Sizes(obj,data);
  data->do_images = TRUE;

  return (retval);
}


static ULONG mNL_Hide(struct IClass *cl,Object *obj,struct MUIP_HandleInput *msg)
{
  register struct NLData *data = INST_DATA(cl,obj);
  ULONG retval;
  STACK_CHECK;
  D(bug("NL: mNL_Hide() \n"));
/*D(bug("%lx|mNL_Hide() 1 \n",obj));*/
  data->badrport = FALSE;

  NL_ClearPointer(obj,data);

  retval = DoSuperMethodA(cl,obj,(Msg) msg);

  data->rp = NULL;
  data->SHOW = FALSE;
  data->moves = FALSE;

  return (retval);
}





static ULONG mNL_Import(struct IClass *cl,Object *obj,struct MUIP_Import *msg)
{
  register struct NLData *data = INST_DATA(cl,obj);
  ULONG id;
  LONG *nlie;

  if((id = (muiNotifyData(obj)->mnd_ObjectID)))
  {
    if ((nlie = (LONG *) DoMethod(msg->dataspace,MUIM_Dataspace_Find,id)) &&
        (nlie[0] == MAKE_ID('E','X','P','T')))
    {
      ULONG nliepos = 1;
      LONG numcol;
      LONG numsel;
      LONG num_widt = 0;
      LONG num_ordr = 0;
      LONG num_sels = 0;

      FULLQUIET;

      while (id)
      {
        if      (nlie[nliepos] == MAKE_ID('A','C','T','V'))
        { nliepos++;
          if (data->NList_Imports & MUIV_NList_Imports_Active)
            NL_List_Active(obj,data,nlie[nliepos],NULL,MUIV_NList_Select_On,TRUE);
          nliepos++;
/*D(bug("%lx|Import_ACTV=%ld (%ld)\n",obj,nlie[nliepos-1],(nliepos-1)));*/
          data->do_draw = TRUE;
        }
        else if (nlie[nliepos] == MAKE_ID('F','R','S','T'))
        { nliepos++;
          if (data->NList_Imports & MUIV_NList_Imports_First)
            NL_List_First(obj,data,nlie[nliepos],NULL);
          nliepos++;
/*D(bug("%lx|Import_FRST=%ld (%ld)\n",obj,nlie[nliepos-1],(nliepos-1)));*/
          data->do_draw = TRUE;
        }
        else if (nlie[nliepos] == MAKE_ID('T','I','T','L'))
        { nliepos++;
          if (data->NList_Imports & MUIV_NList_Imports_TitleMark)
          { set(obj,MUIA_NList_SortType, nlie[nliepos]);
            set(obj,MUIA_NList_TitleMark, nlie[nliepos]);
          }
          nliepos++;
/*D(bug("%lx|Import_TITL=0x%lx (%ld)\n",obj,nlie[nliepos-1],(nliepos-1)));*/
          data->do_draw_all = data->do_draw = TRUE;
        }
        else if (nlie[nliepos] == MAKE_ID('T','I','T','2'))
        { nliepos++;
          if (data->NList_Imports & MUIV_NList_Imports_TitleMark)
          { set(obj,MUIA_NList_SortType2, nlie[nliepos]);
            set(obj,MUIA_NList_TitleMark2, nlie[nliepos]);
          }
          nliepos++;
/*D(bug("%lx|Import_TIT2=0x%lx (%ld)\n",obj,nlie[nliepos-1],(nliepos-1)));*/
          data->do_draw_all = data->do_draw = TRUE;
        }
        else if (nlie[nliepos] == MAKE_ID('W','I','D','T'))
        { nliepos++;
          num_widt = nlie[nliepos++];
          for (numcol = 0; numcol < num_widt; numcol++)
          { if (data->NList_Imports & MUIV_NList_Imports_ColWidth)
              NL_ColWidth(obj,data,numcol,nlie[nliepos]);
            nliepos++;
/*D(bug("%lx|Import_WIDT(%ld)=%ld (%ld)\n",obj,numcol,nlie[nliepos-1],(nliepos-1)));*/
            data->do_draw_all = data->do_draw = TRUE;
          }
        }
        else if (nlie[nliepos] == MAKE_ID('O','R','D','R'))
        { nliepos++;
          num_ordr = nlie[nliepos++];
          for (numcol = 0; numcol < num_ordr; numcol++)
          { if (data->NList_Imports & MUIV_NList_Imports_ColOrder)
              NL_SetCol(obj,data,numcol,nlie[nliepos]);
            nliepos++;
/*D(bug("%lx|Import_ORDR(%ld)=%ld (%ld)\n",obj,numcol,nlie[nliepos-1],(nliepos-1)));*/
            data->do_draw_all = data->do_draw = TRUE;
          }
        }
        else if (nlie[nliepos] == MAKE_ID('S','E','L','S'))
        { nliepos++;
          num_sels = nlie[nliepos++];
          for (numsel = 0; numsel < num_sels; numsel++)
          { if (data->NList_Imports & MUIV_NList_Imports_Selected)
            { if ((nlie[nliepos] >= 0) && (nlie[nliepos] < data->NList_Entries))
              { SELECT2_CHGE(nlie[nliepos],TE_Select_Line);
              }
            }
            nliepos++;
/*D(bug("%lx|Import_SELS(%ld)=%ld (%ld)\n",obj,numsel,nlie[nliepos-1],(nliepos-1)));*/
            data->do_draw_all = data->do_draw = TRUE;
          }
        }
        else
          id = 0;
      }
      FULLQUIET_END;
      if (data->do_draw && _app(obj))
        DoMethod(_app(obj),MUIM_Application_PushMethod,obj,1,MUIM_NList_Trigger);
    }
  }
  return (0);
}




static ULONG mNL_Export(struct IClass *cl,Object *obj,struct MUIP_Export *msg)
{
  register struct NLData *data = INST_DATA(cl,obj);
  ULONG id;

  if((id = (muiNotifyData(obj)->mnd_ObjectID)))
  {
    ULONG nliesize = 0;
    ULONG nliepos = 0;
    LONG pos;
    LONG numsel = 0;
    LONG numcol;
    LONG num_widt = 0;
    LONG num_ordr = 0;
    LONG num_sels = 0;
    LONG n_actv = 0;
    LONG n_frst = 0;
    LONG n_titr = 0;
    LONG n_tit2 = 0;
    LONG n_widt = 0;
    LONG n_ordr = 0;
    LONG n_sels = 0;

/*D(bug("%lx|Exports=%lx Dataspace=%lx ...\n",obj,data->NList_Imports,msg->dataspace));*/

    if (data->NList_Exports & MUIV_NList_Exports_Active)
      n_actv = 2;
    if (data->NList_Exports & MUIV_NList_Exports_First)
      n_frst = 2;
    if (data->NList_Exports & MUIV_NList_Exports_TitleMark)
      n_titr = n_tit2 = 2;
    if (data->NList_Exports & MUIV_NList_Exports_ColWidth)
    { for (numcol = 0; numcol < data->numcols; numcol++)
      { if (data->cols[numcol].col >= num_widt)
          num_widt = data->cols[numcol].col + 1;
      }
      n_widt = num_widt + 2;
    }
    if (data->NList_Exports & MUIV_NList_Exports_ColOrder)
    { num_ordr = data->numcols;
      n_ordr = num_ordr + 2;
    }
    if ((data->NList_Exports & MUIV_NList_Exports_Selected) &&
        (data->NList_TypeSelect == MUIV_NList_TypeSelect_Line))
    { NL_List_Select(obj,data,MUIV_NList_Select_All,MUIV_NList_Select_All,MUIV_NList_Select_Ask,&numsel);
      if (numsel > 0)
        num_sels = numsel;
      n_sels = num_sels + 2;
    }

    nliesize = (n_actv + n_frst + n_titr + n_tit2 + n_widt + n_ordr + n_sels + 3) * 4;

    if (data->nlie)
    { NL_Free(data,(void *)data->nlie,"NL_Export");
      data->nlie = NULL;
    }

    if((data->nlie = (LONG *) NL_Malloc(data,nliesize,"NL_Export")))
    {
      nliepos = 0;
      data->nlie[nliepos++] = MAKE_ID('E','X','P','T');

      if (data->NList_Exports & MUIV_NList_Exports_Active)
      { data->nlie[nliepos++] = MAKE_ID('A','C','T','V');
        data->nlie[nliepos++] = data->NList_Active;
/*D(bug("%lx|Export_ACTV=%ld (%ld)\n",obj,data->nlie[nliepos-1],(nliepos-1)));*/
      }
      if (data->NList_Exports & MUIV_NList_Exports_First)
      { data->nlie[nliepos++] = MAKE_ID('F','R','S','T');
        data->nlie[nliepos++] = data->NList_First;
/*D(bug("%lx|Export_FRST=%ld (%ld)\n",obj,data->nlie[nliepos-1],(nliepos-1)));*/
      }
      if (data->NList_Exports & MUIV_NList_Exports_TitleMark)
      { data->nlie[nliepos++] = MAKE_ID('T','I','T','L');
        data->nlie[nliepos++] = data->NList_TitleMark;
/*D(bug("%lx|Export_TITL=0x%lx (%ld)\n",obj,data->nlie[nliepos-1],(nliepos-1)));*/
      }
      if (data->NList_Exports & MUIV_NList_Exports_TitleMark)
      { data->nlie[nliepos++] = MAKE_ID('T','I','T','2');
        data->nlie[nliepos++] = data->NList_TitleMark2;
/*D(bug("%lx|Export_TIT2=0x%lx (%ld)\n",obj,data->nlie[nliepos-1],(nliepos-1)));*/
      }
      if (data->NList_Exports & MUIV_NList_Exports_ColWidth)
      { data->nlie[nliepos++] = MAKE_ID('W','I','D','T');
        data->nlie[nliepos++] = num_widt;
        for (numcol = 0; numcol < num_widt; numcol++)
        { data->nlie[nliepos++] = (LONG) NL_ColWidth(obj,data,numcol,MUIV_NList_ColWidth_Get);
/*D(bug("%lx|Export_WIDT(%ld)=%ld (%ld)\n",obj,numcol,data->nlie[nliepos-1],(nliepos-1)));*/
        }
      }
      if (data->NList_Exports & MUIV_NList_Exports_ColOrder)
      { data->nlie[nliepos++] = MAKE_ID('O','R','D','R');
        data->nlie[nliepos++] = num_ordr;
        for (numcol = 0; numcol < num_ordr; numcol++)
        { data->nlie[nliepos++] = NL_ColumnToCol(obj,data,numcol);
/*D(bug("%lx|Export_ORDR(%ld)=%ld (%ld)\n",obj,numcol,data->nlie[nliepos-1],(nliepos-1)));*/
        }
      }
      if ((data->NList_Exports & MUIV_NList_Exports_Selected) &&
          (data->NList_TypeSelect == MUIV_NList_TypeSelect_Line))
      { struct  MUIP_NList_NextSelected nlns;
        data->nlie[nliepos++] = MAKE_ID('S','E','L','S');
        data->nlie[nliepos++] = num_sels;
        pos = MUIV_NList_NextSelected_Start;
        numsel = 0;
        nlns.pos = &pos;
        while ((numsel < num_sels) && mNL_List_NextSelected(cl,obj,&nlns) && (pos != MUIV_NList_NextSelected_End))
        { data->nlie[nliepos++] = pos;
/*D(bug("%lx|Export_SELS(%ld)=%ld (%ld)\n",obj,numsel,data->nlie[nliepos-1],(nliepos-1)));*/
          numsel++;
        }
        while (numsel < num_sels)
        { data->nlie[nliepos++] = -1;
/*D(bug("%lx|Export_SELS(%ld)=%ld ! (%ld)\n",obj,numsel,data->nlie[nliepos-1],(nliepos-1)));*/
          numsel++;
        }
      }
      data->nlie[nliepos++] = MAKE_ID('E','N','D','.');
/*D(bug("%lx|Export_END. (%ld)\n",obj,(nliepos-1)));*/

      DoMethod(msg->dataspace,MUIM_Dataspace_Add,data->nlie,nliesize,id);
/*D(bug("%lx|Exports=%lx Dataspace=%lx done.\n",obj,data->NList_Imports,msg->dataspace));*/
    }
  }
  return (0);
}

//$$$Sensei
static ULONG mNL_List_Construct( struct IClass *cl, Object *obj, struct MUIP_NList_Construct *msg )
{

    APTR entry = msg->entry;

    if( entry )
    {

        struct NLData   *data   = INST_DATA( cl, obj );
        struct Hook     *hook   = data->NList_ConstructHook;

        if( hook )
        {

				APTR	pool	= msg->pool;

				if( data->NList_ConstructHook2 )
					 entry = (APTR) MyCallHookPktA(obj,hook,entry,pool);
            else
					 entry = (APTR) MyCallHookPkt(obj,TRUE,hook,pool,entry);

        }

    }

    return( (ULONG) entry );

}

//$$$Sensei
static ULONG mNL_List_Destruct( struct IClass *cl, Object *obj, struct MUIP_NList_Destruct *msg )
{

    APTR entry = msg->entry;

    if( entry )
    {

        struct NLData   *data   = INST_DATA( cl, obj );
        struct Hook     *hook   = data->NList_DestructHook;

        if( hook )
        {

            APTR pool = msg->pool;

            if( data->NList_DestructHook2 )
                MyCallHookPktA(obj,hook,entry,pool);
            else
                MyCallHookPkt(obj,TRUE,hook,pool,entry);

        }

    }

    return( 0 );

}

//$$$Sensei
// Function directly stolen from original NList_Compare().
static ULONG mNL_List_Compare( struct IClass *cl, Object *obj, struct MUIP_NList_Compare *msg )
{
	struct NLData	*data	= INST_DATA( cl, obj );
	APTR	s1	= msg->entry1;
	APTR	s2	= msg->entry2;
	if( data->NList_CompareHook )
	{
		if( data->NList_CompareHook2 )
			return ((ULONG) MyCallHookPktA(obj,data->NList_CompareHook,s1,s2,msg->sort_type1,msg->sort_type2));
		else
			return ((ULONG) MyCallHookPkt(obj,TRUE,data->NList_CompareHook,s2,s1));
	}
	else
		return ((ULONG) Stricmp(s1,s2));
}

//$$$Sensei
static ULONG mNL_List_Display( struct IClass *cl, Object *obj, struct MUIP_NList_Display *msg )
{
	struct NLData *data = INST_DATA( cl, obj );

	if( data->NList_DisplayHook )
	{
		if( data->NList_DisplayHook2 )
		{
			// data->DisplayArray[0] = (char *) useptr;
			return MyCallHookPkt(obj,FALSE,data->NList_DisplayHook,obj,data->DisplayArray);
		}
		else
		{
			APTR useptr = data->DisplayArray[ 0 ];
			data->DisplayArray[0] = (char *) data->NList_PrivateData;
			return MyCallHookPkt(obj,TRUE,data->NList_DisplayHook,&data->DisplayArray[2],useptr);
		}
	}
	return( 0 );
}
 
#define FS       (data = INST_DATA(cl,obj)); (NotNotify = data->DoNotify)

DISPATCHERPROTO(_Dispatcher)
{
  DISPATCHER_INIT
  
  register struct NLData *data = NULL;
  ULONG retval = 0;
  ULONG NotNotify = ~0;

  switch (msg->MethodID)
  {
    case OM_NEW                        :     retval =                   mNL_New(cl,obj,(APTR)msg); break;
    case OM_DISPOSE                    :     retval =               mNL_Dispose(cl,obj,(APTR)msg); break;
    case MUIM_Setup                    :     retval =                 mNL_Setup(cl,obj,(APTR)msg); break;
    case MUIM_Cleanup                  :     retval =               mNL_Cleanup(cl,obj,(APTR)msg); break;
    case MUIM_AskMinMax                :     retval =             mNL_AskMinMax(cl,obj,(APTR)msg); break;
    case MUIM_Show                     :     retval =                  mNL_Show(cl,obj,(APTR)msg); break;
    case MUIM_Hide                     :     retval =                  mNL_Hide(cl,obj,(APTR)msg); break;
    case MUIM_Draw                     :     retval =                  mNL_Draw(cl,obj,(APTR)msg); break;
    case MUIM_HandleInput              :     retval =           mNL_HandleInput(cl,obj,(APTR)msg); break;
    case MUIM_HandleEvent              :     retval =           mNL_HandleEvent(cl,obj,(APTR)msg); break;
    case OM_SET                        : FS; retval =                   mNL_Set(cl,obj,(APTR)msg); break;
    case OM_GET                        : FS; retval =                   mNL_Get(cl,obj,(APTR)msg); break;
    case MUIM_Notify                   :     retval =                mNL_Notify(cl,obj,(APTR)msg); break;
    case MUIM_DragQuery                :     retval =             mNL_DragQuery(cl,obj,(APTR)msg); break;
    case MUIM_DragBegin                :     retval =             mNL_DragBegin(cl,obj,(APTR)msg); break;
    case MUIM_DragReport               :     retval =            mNL_DragReport(cl,obj,(APTR)msg); break;
    case MUIM_DragFinish               :     retval =            mNL_DragFinish(cl,obj,(APTR)msg); break;
    case MUIM_DragDrop                 :     retval =              mNL_DragDrop(cl,obj,(APTR)msg); break;
    case MUIM_CreateDragImage          :     retval =       mNL_CreateDragImage(cl,obj,(APTR)msg); break;
    case MUIM_DeleteDragImage          :     retval =       mNL_DeleteDragImage(cl,obj,(APTR)msg); break;
    case MUIM_ContextMenuBuild         :     retval =      mNL_ContextMenuBuild(cl,obj,(APTR)msg); break;
    case MUIM_ContextMenuChoice        :     retval =     mNL_ContextMenuChoice(cl,obj,(APTR)msg); break;
    case MUIM_Import                   :     retval =                mNL_Import(cl,obj,(APTR)msg); break;
    case MUIM_Export                   :     retval =                mNL_Export(cl,obj,(APTR)msg); break;
    case MUIM_NList_ContextMenuBuild   :     retval = 0; break;
    case MUIM_NList_Trigger            :     retval =               mNL_Trigger(cl,obj,(APTR)msg); break;
    case MUIM_List_Sort :
    case MUIM_NList_Sort               : FS; retval =             mNL_List_Sort(cl,obj,(APTR)msg); break;
    case MUIM_NList_Sort2              : FS; retval =            mNL_List_Sort2(cl,obj,(APTR)msg); break;
    case MUIM_NList_Sort3              : FS; retval =            mNL_List_Sort3(cl,obj,(APTR)msg); break;
    case MUIM_List_Insert :
    case MUIM_NList_Insert             : FS; retval =           mNL_List_Insert(cl,obj,(APTR)msg); break;
    case MUIM_List_InsertSingle :
    case MUIM_NList_InsertSingle       : FS; retval =     mNL_List_InsertSingle(cl,obj,(APTR)msg); break;
    case MUIM_List_GetEntry :
    case MUIM_NList_GetEntry           : FS; retval =         mNL_List_GetEntry(cl,obj,(APTR)msg); break;
    case MUIM_List_Clear :
    case MUIM_NList_Clear              : FS; retval =            mNL_List_Clear(cl,obj,(APTR)msg); break;
    case MUIM_List_Jump :
    case MUIM_NList_Jump               : FS; retval =             mNL_List_Jump(cl,obj,(APTR)msg); break;
    case MUIM_List_Select :
    case MUIM_NList_Select             : FS; retval =           mNL_List_Select(cl,obj,(APTR)msg); break;
    case MUIM_NList_SelectChange       :     retval = 0; break;
    case MUIM_List_TestPos             : FS; retval =       mNL_List_TestPosOld(cl,obj,(APTR)msg); break;
    case MUIM_NList_TestPos            : FS; retval =          mNL_List_TestPos(cl,obj,(APTR)msg); break;
    case MUIM_List_Redraw :
    case MUIM_NList_Redraw             : FS; retval =           mNL_List_Redraw(cl,obj,(APTR)msg); break;
    case MUIM_List_Exchange :
    case MUIM_NList_Exchange           : FS; retval =         mNL_List_Exchange(cl,obj,(APTR)msg); break;
    case MUIM_List_Move :
    case MUIM_NList_Move               : FS; retval =             mNL_List_Move(cl,obj,(APTR)msg); break;
    case MUIM_List_NextSelected :
    case MUIM_NList_NextSelected       : FS; retval =     mNL_List_NextSelected(cl,obj,(APTR)msg); break;
    case MUIM_NList_PrevSelected       : FS; retval =     mNL_List_PrevSelected(cl,obj,(APTR)msg); break;
    case MUIM_List_Remove :
    case MUIM_NList_Remove             : FS; retval =           mNL_List_Remove(cl,obj,(APTR)msg); break;
    case MUIM_List_CreateImage :
    case MUIM_NList_CreateImage        : FS; retval =           mNL_CreateImage(cl,obj,(APTR)msg); break;
    case MUIM_List_DeleteImage :
    case MUIM_NList_DeleteImage        : FS; retval =           mNL_DeleteImage(cl,obj,(APTR)msg); break;
    case MUIM_NList_CopyToClip         : FS; retval =            mNL_CopyToClip(cl,obj,(APTR)msg); break;
    case MUIM_NList_UseImage           : FS; retval =              mNL_UseImage(cl,obj,(APTR)msg); break;
    case MUIM_NList_ReplaceSingle      : FS; retval =    mNL_List_ReplaceSingle(cl,obj,(APTR)msg); break;
    case MUIM_NList_InsertWrap         : FS; retval =       mNL_List_InsertWrap(cl,obj,(APTR)msg); break;
    case MUIM_NList_InsertSingleWrap   : FS; retval = mNL_List_InsertSingleWrap(cl,obj,(APTR)msg); break;
    case MUIM_NList_GetEntryInfo       : FS; retval =     mNL_List_GetEntryInfo(cl,obj,(APTR)msg); break;
    case MUIM_NList_GetSelectInfo      : FS; retval =    mNL_List_GetSelectInfo(cl,obj,(APTR)msg); break;
    case MUIM_NList_CopyTo             : FS; retval =                mNL_CopyTo(cl,obj,(APTR)msg); break;
    case MUIM_NList_DropType           : FS; retval =              mNL_DropType(cl,obj,(APTR)msg); break;
    case MUIM_NList_DropDraw           : FS; retval =              mNL_DropDraw(cl,obj,(APTR)msg); break;
    case MUIM_NList_DropEntryDrawErase : FS; retval =    mNL_DropEntryDrawErase(cl,obj,(APTR)msg); break;
    case MUIM_NList_RedrawEntry        : FS; retval =      mNL_List_RedrawEntry(cl,obj,(APTR)msg); break;
    case MUIM_NList_DoMethod           : FS; retval =         mNL_List_DoMethod(cl,obj,(APTR)msg); break;
    case MUIM_NList_ColWidth           : FS; retval =         mNL_List_ColWidth(cl,obj,(APTR)msg); break;
    case MUIM_NList_ColToColumn        : FS; retval =           mNL_ColToColumn(cl,obj,(APTR)msg); break;
    case MUIM_NList_ColumnToCol        : FS; retval =           mNL_ColumnToCol(cl,obj,(APTR)msg); break;
    case MUIM_NList_SetColumnCol       : FS; retval =          mNL_SetColumnCol(cl,obj,(APTR)msg); break;
    case MUIM_NList_GetPos             :     retval =           mNL_List_GetPos(cl,obj,(APTR)msg); break;
    case MUIM_NList_QueryBeginning     :     retval = 0; break;
    case MUIM_NList_Construct          :     retval =        mNL_List_Construct(cl,obj,(APTR)msg); break;
    case MUIM_NList_Destruct           :     retval =         mNL_List_Destruct(cl,obj,(APTR)msg); break;
    case MUIM_NList_Compare            :     retval =          mNL_List_Compare(cl,obj,(APTR)msg); break;
    case MUIM_NList_Display            :     retval =          mNL_List_Display(cl,obj,(APTR)msg); break;
/*
 *   case MUIM_DrawBackground :
 *   case MUIM_HandleInput :
 *   case MUIM_GetConfigItem :
 *   case 0x80424d50 :
 *   case 0x8042845b :
 *   case 0x8042549a :
 *   case 0x8042f8a4 :  / * just before drag start ! * /
 *     break;
 */
     default:
/*       D(bug("NL: MethodID= 0x%lx \n",msg->MethodID));*/
       return(DoSuperMethodA(cl,obj,msg));
       break;
  }
  if (~NotNotify && data)
  { ULONG DoNotify = ~NotNotify & data->DoNotify & data->Notify;
    if (data->SETUP && DoNotify)
    {
/*D(bug("%lx|        DoNotify= %lx",obj,DoNotify));*/

      do_notifies(DoNotify);

/*
 *DoNotify = ~NotNotify & data->DoNotify & data->Notify;
 *D(bug(" -> %lx (%lx)\n",DoNotify,data->DoNotify & data->Notify));
 */
    }
  }
  return (retval);
  
  DISPATCHER_EXIT
}

