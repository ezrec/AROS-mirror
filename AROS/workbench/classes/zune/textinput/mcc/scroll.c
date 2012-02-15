/*
** $Id$
*/

#include "mystring.h"
#include "textinput_mcc.h"
#include "textinput_cfg.h"

struct Data {
	APTR sco;
	APTR scroll_h, scroll_v;
	APTR bt_edit;
	APTR grp_editcontainer;
	APTR group2;
	ULONG noinput, usewinborder;
	ULONG disposescroll_h, disposescroll_v;
	ULONG informinprogress;
	ULONG newsetup;
	struct MUI_EventHandlerNode ehn;
};

#define BSETTAG(t) (tag->ti_Data?TRUE:FALSE)

#ifndef MBX

#include "rev.h"

#define UserLibID "$VER: Textinputscroll.mcc " LVERTAG " © 1997-2001 Oliver Wagner <owagner@vapor.com>, All Rights Reserved\r\n"
#define CLASS "Textinputscroll.mcc"
#define SUPERCLASS MUIC_Group
#define _Dispatcher dispfunc
#define ClassInit
#define MASTERVERSION 18

#include "mccheader.h"

static APTR __stdargs DoSuperNew( struct IClass *class, APTR obj, ULONG tag1, ... )
{
	return( (APTR)DoSuperMethod( class, obj, OM_NEW, &tag1, NULL ) );
}

#endif


DECCONST
{
	struct Data *data;
	//struct TagItem *tag, *tagp;
	APTR sco, scroll_h = NULL, scroll_v, grp_editcontainer = NULL, group2 = NULL, dummy = NULL;
	int usewinborder = GetTagData( MUIA_Textinputscroll_UseWinBorder, FALSE, msg->ops_AttrList );
	int usewinborderattr = usewinborder ? MUIA_Prop_UseWinBorder : TAG_IGNORE ;
	int vertonly = GetTagData( MUIA_Textinputscroll_VertScrollerOnly, FALSE, msg->ops_AttrList );
	int noinput = GetTagData( MUIA_Textinput_NoInput, FALSE, msg->ops_AttrList );
	int setmin = GetTagData( MUIA_Textinput_SetMin, FALSE, msg->ops_AttrList );
	int setvmin = GetTagData( MUIA_Textinput_SetVMin, FALSE, msg->ops_AttrList );
	int cyclechain = TAG_IGNORE;
	struct TagItem *tag;
	if((tag=FindTagItem( MUIA_CycleChain, msg->ops_AttrList )))
	{
		if( tag->ti_Data )
			cyclechain = MUIA_CycleChain;
		if( noinput )
			tag->ti_Tag = TAG_IGNORE;
	}
	if( vertonly )
	{
		obj = (APTR) DoSuperNew( cl, obj,
			MUIA_Group_Horiz, TRUE,
			MUIA_Group_Spacing, 0,
			cyclechain, noinput ? 1 : 0,

			Child, sco = TextinputObject, MUIA_Textinput_SetMin, setmin, MUIA_Textinput_SetVMin, setvmin, TAG_MORE, msg->ops_AttrList ),
			Child, scroll_v = ScrollbarObject, usewinborderattr, MUIV_Prop_UseWinBorder_Right,End,

			TAG_END
		);
	}
	else if( noinput )
	{
		obj = (APTR) DoSuperNew( cl, obj,
			MUIA_Group_Horiz, TRUE,
			MUIA_Group_Spacing, 0,
			Child, group2 = VGroup,
				MUIA_Group_Spacing, 0,
				Child, sco = TextinputObject, MUIA_Textinput_SetMin, setmin, MUIA_Textinput_SetVMin, setvmin, TAG_MORE, msg->ops_AttrList ),
				Child, scroll_h = ScrollbarObject, MUIA_Group_Horiz, TRUE, usewinborderattr, MUIV_Prop_UseWinBorder_Bottom, End,
			End,
			Child, scroll_v = ScrollbarObject, usewinborderattr, MUIV_Prop_UseWinBorder_Right,End,

			TAG_END
		);
	}
	else
	{
		obj = (APTR) DoSuperNew( cl, obj,
			MUIA_Group_Columns, 2, MUIA_Group_Spacing, 0,
			cyclechain, noinput ? 1 : 0,

			Child, sco = TextinputObject, MUIA_Textinput_SetMin, setmin, MUIA_Textinput_SetVMin, setvmin, TAG_MORE, msg->ops_AttrList ),
			Child, scroll_v = ScrollbarObject, usewinborderattr, MUIV_Prop_UseWinBorder_Right,End,
			Child, scroll_h = ScrollbarObject, MUIA_Group_Horiz, TRUE, usewinborderattr, MUIV_Prop_UseWinBorder_Bottom, End,
			Child, grp_editcontainer = HGroup,
			 Child, dummy = RectangleObject,
			 End,
			End,

			TAG_END
		);
	}

	if( !obj )
		return( NULL );

	set( sco, MUIA_Textinput_TSCO, obj );

	DoMethod( scroll_v, MUIM_Notify, MUIA_Prop_First, MUIV_EveryTime,
		sco, 3, MUIM_Set, MUIA_Textinput_TopOffset, MUIV_TriggerValue
	);

	if( !vertonly )
	{
		DoMethod( scroll_h, MUIM_Notify, MUIA_Prop_First, MUIV_EveryTime,
			sco, 3, MUIM_Set, MUIA_Textinput_LeftOffset, MUIV_TriggerValue
		);

	}

	data = INST_DATA( cl, obj );

	data->sco = sco;
	data->scroll_h = scroll_h;
	data->scroll_v = scroll_v;
	data->group2 = group2;
	data->grp_editcontainer = grp_editcontainer;
	data->bt_edit=dummy;
	data->noinput = noinput;
	data->usewinborder = usewinborder;

	if( data->noinput )
	{
		data->ehn.ehn_Object = obj;
		data->ehn.ehn_Class = cl;
		data->ehn.ehn_Events = IDCMP_RAWKEY;
	}
	else
		muiAreaData( obj )->mad_Flags |= MADF_KNOWSACTIVE;

	return( (ULONG)obj );
}

DECDEST
{
	GETDATA;
	if( data->disposescroll_v )
		MUI_DisposeObject( data->scroll_v );
	if( data->disposescroll_h )
		MUI_DisposeObject( data->scroll_h );
	return( DOSUPER );
}

DECMMETHOD( Textinputscroll_Inform )
{
	GETDATA;

	if( !msg->xv || !msg->yv || data->informinprogress )
		return( 0 );

	data->informinprogress = TRUE;

	if( data->noinput && !data->usewinborder )
	{
#if 0
		if( ( !data->disposescroll_v && msg->ys <= msg->yv ) || ( data->disposescroll_v && msg->ys > msg->yv ) )
		{
			if( DoMethod( obj, MUIM_Group_InitChange ) )
			{
				data->disposescroll_v ^= 1;
				DoMethod( obj, data->disposescroll_v ? OM_REMMEMBER : OM_ADDMEMBER, data->scroll_v );
				DoMethod( obj, MUIM_Group_ExitChange );
			}
		}
		if( data->scroll_h && ( ( !data->disposescroll_h && msg->xs <= msg->xv ) || ( data->disposescroll_h && msg->xs > msg->xv ) ) )
		{
			Object *grp = data->group2;
			if( DoMethod( grp, MUIM_Group_InitChange ) )
			{
				data->disposescroll_h ^= 1;
				DoMethod( grp, data->disposescroll_h ? OM_REMMEMBER : OM_ADDMEMBER, data->scroll_h );
				DoMethod( grp, MUIM_Group_ExitChange );
			}
		}
#endif
	}

	if( data->scroll_h && !data->disposescroll_h )
	{
		SetAttrs( data->scroll_h,
			MUIA_NoNotify, TRUE,
			MUIA_Prop_Entries, msg->xs,
			MUIA_Prop_First, msg->xo,
			MUIA_Prop_Visible, msg->xv,
			TAG_DONE
		);
		if( data->bt_edit )
			set( data->bt_edit, MUIA_Disabled, msg->noedit );
	}
	if( !data->disposescroll_v )
	{
		SetAttrs( data->scroll_v,
			MUIA_NoNotify, TRUE,
			MUIA_Prop_Entries, msg->ys,
			MUIA_Prop_First, msg->yo,
			MUIA_Prop_Visible, msg->yv,
			TAG_DONE
		);
	}
	data->informinprogress = FALSE;
	return( 0 );
}

DECMMETHOD( HandleEvent )
{
	GETDATA;
	ULONG returnbits = 0;

	if( !data->disposescroll_v )
		returnbits = DoMethod( data->scroll_v, MUIM_HandleInput, msg->imsg, msg->muikey );
	if( data->scroll_h && !data->disposescroll_h )
		returnbits |= DoMethod( data->scroll_h, MUIM_HandleInput, msg->imsg, msg->muikey );

	return( returnbits );
}

DECGET
{
	switch( msg->opg_AttrID )
	{
		case MUIA_Version:
			*msg->opg_Storage = VERSION;
			return( TRUE );

		case MUIA_Revision:
			*msg->opg_Storage = REVISION;
			return( TRUE );
	}
	return( DOSUPER );
}

DECMMETHOD( Setup )
{
	GETDATA;

	if( data->grp_editcontainer && !data->usewinborder )
	{
		Object *bt_edit;
		char *s = NULL;
		data->newsetup = FALSE;
		DoMethod( obj, MUIM_GetConfigItem, MUICFG_Textinput_ButtonImage, &s );
		if( !s || !*s )
			s = "1:21";
		bt_edit = ImageObject,
			MUIA_Image_Spec, s,
			MUIA_Image_FreeHoriz, TRUE,
			MUIA_Image_FreeVert, TRUE,
			MUIA_Weight, 1,
			ButtonFrame,
			MUIA_InputMode, MUIV_InputMode_RelVerify,
		End;
		if( bt_edit )
		{
			DoMethod( data->grp_editcontainer, OM_REMMEMBER, data->bt_edit );
			DoMethod( data->grp_editcontainer, OM_ADDMEMBER, bt_edit );
			MUI_DisposeObject(data->bt_edit);
			data->bt_edit=bt_edit;
			DoMethod( bt_edit, MUIM_Notify, MUIA_Pressed, FALSE, data->sco, 2, MUIM_Textinput_ExternalEdit, 0 );
			set( bt_edit, MUIA_Disabled, data->noinput );
		}
	}

	if( !DOSUPER )
		return( 0 );
	data->newsetup = TRUE;
	//if( data->noinput )
	//	DoMethod( _win( obj ), MUIM_Window_AddEventHandler, &data->ehn );

	if( data->scroll_v )
		set( data->scroll_v, MUIA_Prop_DeltaFactor, _font( data->sco )->tf_YSize );
	if( data->scroll_h )
		set( data->scroll_h, MUIA_Prop_DeltaFactor, _font( data->sco )->tf_XSize );

	return( TRUE );
}

DECMMETHOD( Cleanup )
{
//	GETDATA;
	//if( data->noinput )
	//	DoMethod( _win( obj ), MUIM_Window_RemEventHandler, &data->ehn );
	return( DOSUPER );
}

DECMMETHOD( Show )
{
//	GETDATA;
	ULONG rc=DOSUPER;
	return( rc );
}

DECMMETHOD( Notify )
{
	GETDATA;

	return( DoMethodA( data->sco, msg ) );
}

DECMMETHOD( GoActive )
{
	GETDATA;
	if( !data->noinput )
	{
		set( _win( obj ), MUIA_Window_ActiveObject, data->sco );
		return( 0 );
	}
	return( DOSUPER );
}

#ifdef MBX
DISPATCHERNAME_BEGIN(Textinputscroll)
#else
__asm __saveds ULONG dispfunc(register __a0 Class *cl,register __a2 Object *obj,register __a1 Msg msg)
#endif
{
	switch( msg->MethodID )
	{
		DEFMETHOD( OM_NEW )
		DEFMETHOD( OM_GET )
		DEFMETHOD( OM_DISPOSE )
		DEFMETHOD( MUIM_Setup )
		DEFMETHOD( MUIM_Cleanup )
		DEFMETHOD( MUIM_Show )
		DEFMETHOD( MUIM_HandleEvent )
		DEFMETHOD( MUIM_Notify )
		DEFMETHOD( MUIM_GoActive )
		DEFMETHOD( MUIM_Textinputscroll_Inform )
	}
	return( DOSUPER );
}
#ifdef MBX
DISPATCHER_END
#endif


#ifdef MBX
ULONG TextinputscrollGetSize(VOID)
{
	return(sizeof(struct Data));
}
#endif



#ifndef MBX
BOOL ClassInitFunc( struct Library *base )
{
	if( MUIMasterBase->lib_Version >= 20 )
		ThisClass->mcc_Class->cl_ID = "Textinputscroll.mcc";

	return TRUE;
}
#endif
