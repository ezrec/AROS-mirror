/****************************************************************************/
/* ArrowString class                                                        */
/* By Michal Rybinski                                                       */
/****************************************************************************/

/*
#include "../common/mui_misc.h"
#include "../common/mui_help.h"
#include "../common/help.h"
#include "../macros/vapor.h"
#include <emul/emulregs.h>
*/

#include <proto/alib.h>
#include <proto/muimaster.h>
#include <proto/utility.h>

#include <string.h>
#include <private/vapor/vapor.h>
#include "arrowstring_class.h"
#include "util.h"


struct Data
{
	Object *ST_Number;
	Object *BT_Increase;
	Object *BT_Decrease;

	ULONG Step;
	ULONG Min;
};

IPTR ArrowString_New(struct IClass *cl, Object *obj,  struct opSet *msg)
{
	struct Data tmp;

	memset(&tmp, 0, sizeof(tmp));

	obj = (Object *) DoSuperNew(cl, obj, MUIA_Group_Horiz, TRUE, MUIA_Group_HorizSpacing, 0,
		Child, tmp.ST_Number = StringObject, StringFrame,
			MUIA_CycleChain, TRUE,
			MUIA_String_Accept, "0123456789",
			MUIA_String_MaxLen, 5,
			MUIA_FixWidthTxt, "123456",
			MUIA_String_Format, MUIV_String_Format_Right,
			MUIA_String_Contents, (IPTR) "1",
		End,
		Child, tmp.BT_Increase = ImageObject, ImageButtonFrame,
			MUIA_InputMode, MUIV_InputMode_RelVerify,
			MUIA_Image_Spec, MUII_ArrowUp,
			MUIA_Image_FreeVert, TRUE,
			MUIA_Background, MUII_ButtonBack,
			MUIA_ShowSelState, FALSE,
		End,
		Child, tmp.BT_Decrease = ImageObject, ImageButtonFrame,
			MUIA_InputMode, MUIV_InputMode_RelVerify,
			MUIA_Image_Spec, MUII_ArrowDown,
			MUIA_Image_FreeVert, TRUE,
			MUIA_Background, MUII_ButtonBack,
			MUIA_ShowSelState, FALSE,
		End,

		TAG_MORE, msg->ops_AttrList);

	if (obj)
	{
		struct Data *data = INST_DATA(cl, obj);
		struct TagItem *ti;
		
		memcpy(data, &tmp, sizeof(tmp));

		data->Step = 1;
		data->Min =1;

		if ((ti = FindTagItem(MA_ArrowString_Step, msg->ops_AttrList)))
		{
			data->Step = ti->ti_Data;
		}
		
		
		if ((ti = FindTagItem(MA_ArrowString_Min, msg->ops_AttrList)))
		{
			data->Min = ti->ti_Data;
		}

		DoMethod(data->BT_Increase, MUIM_Notify, MUIA_Timer, MUIV_EveryTime,
			obj, 1, MM_ArrowString_Increase);

		DoMethod(data->BT_Decrease, MUIM_Notify, MUIA_Timer, MUIV_EveryTime,
			obj, 1, MM_ArrowString_Decrease);

		DoMethod(data->ST_Number, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
			obj, 3, MUIM_Set, MA_ArrowString_Changed, TRUE);

		return (IPTR)obj;
	}

	return (0);
}

IPTR ArrowString_Get(struct IClass *cl, Object *obj, struct opGet *msg)
{
	switch (msg->opg_AttrID)
	{
		case MA_ArrowString_Changed:
		{
			return TRUE;
		}
	}

	return(DoSuperMethodA(cl, obj, (APTR)msg));
}

IPTR ArrowString_Increase(struct IClass *cl, Object *obj)
{
	struct Data *data = INST_DATA(cl, obj);
	ULONG no = xget(data->ST_Number, MUIA_String_Integer);

	set(data->ST_Number, MUIA_String_Integer, no + data->Step);
	set(obj, MA_ArrowString_Changed, TRUE);

	return (0);
}

IPTR ArrowString_Decrease(struct IClass *cl, Object *obj)
{
	struct Data *data = INST_DATA(cl, obj);
	ULONG no = xget(data->ST_Number, MUIA_String_Integer);

	if (no > data->Min)
	{
		set(data->ST_Number, MUIA_String_Integer, no - data->Step);
		set(obj, MA_ArrowString_Changed, TRUE);
	}

	return (0);
}

IPTR ArrowString_Dispose(struct IClass *cl, Object *obj, Msg msg)
{
	return(DoSuperMethodA(cl, obj, (Msg)msg));
}

BEGINMTABLE
		case OM_NEW                   : return(ArrowString_New         (cl, obj, (APTR)msg));
		case OM_DISPOSE               : return(ArrowString_Dispose     (cl, obj, (APTR)msg));
		case OM_GET                   : return(ArrowString_Get         (cl, obj, (APTR)msg));
		case MM_ArrowString_Increase  : return(ArrowString_Increase    (cl, obj));
		case MM_ArrowString_Decrease  : return(ArrowString_Decrease    (cl, obj));
ENDMTABLE

DECSUBCLASS_NC(MUIC_Group, ArrowStringClass)
