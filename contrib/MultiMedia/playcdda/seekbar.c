/* Copyright 2010-2011 Fredrik Wikstrom. All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS `AS IS'
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
*/

#include "seekbar.h"
#include <proto/utility.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/alib.h>

struct SeekBarData {
	ULONG level;
	ULONG min, max;
};

static AROS_UFP3(IPTR, SeekBar_Dispatch,
	AROS_UFPA(Class *, cl, A0),
	AROS_UFPA(Object *, o, A2),
	AROS_UFPA(Msg, msg, A1)
);

struct MUI_CustomClass *SeekBar_CreateClass (void) {
	return MUI_CreateCustomClass(NULL, MUIC_Prop, NULL, sizeof(struct SeekBarData), SeekBar_Dispatch);
}

void SeekBar_DeleteClass (struct MUI_CustomClass *cl) {
	MUI_DeleteCustomClass(cl);
}

static IPTR SeekBar_New(Class *cl, Object *o, struct opSet *ops);
static IPTR SeekBar_Set(Class *cl, Object *o, struct opSet *ops);
static IPTR SeekBar_Get(Class *cl, Object *o, struct opGet *opg);

static AROS_UFH3(IPTR, SeekBar_Dispatch,
	AROS_UFHA(Class *, cl, A0),
	AROS_UFHA(Object *, o, A2),
	AROS_UFHA(Msg, msg, A1)
)
{
	AROS_USERFUNC_INIT
	switch (msg->MethodID) {
		case OM_NEW: return SeekBar_New(cl, o, (struct opSet *)msg);
		case OM_SET: return SeekBar_Set(cl, o, (struct opSet *)msg);
		case OM_GET: return SeekBar_Get(cl, o, (struct opGet *)msg);
	}
	return DoSuperMethodA(cl, o, msg);
	AROS_USERFUNC_EXIT
}

static IPTR SeekBar_New(Class *cl, Object *o, struct opSet *ops) {
	Object *res;
	struct TagItem tags[5] = {
		{ MUIA_Prop_Slider,  TRUE                    },
		{ MUIA_Prop_Horiz,   TRUE                    },
		{ MUIA_Prop_Entries, 65536                   },
		{ MUIA_Prop_Visible, 1                       },
		{ TAG_MORE,          (IPTR)ops->ops_AttrList },
	};
	ops->ops_AttrList = tags;
	res = (Object *)DoSuperMethodA(cl, o, (Msg)ops);
	ops->ops_AttrList = (struct TagItem *)tags[4].ti_Data;
	return (IPTR)res;
}

static IPTR SeekBar_Set(Class *cl, Object *o, struct opSet *ops) {
	struct SeekBarData *data = INST_DATA(cl, o);
	struct TagItem *tstate, *tag;
	IPTR tidata;
	IPTR res = 0;
	
	data->level = XGET(o, MUIA_Prop_First) + data->min;
	tstate = ops->ops_AttrList;
	while ((tag = NextTagItem(&tstate))) {
		tidata = tag->ti_Data;
		switch (tag->ti_Tag) {
			case MUIA_Slider_Min:
				if (data->min != tidata) {
					data->min = tidata;
					res++;
				}
				break;
			case MUIA_Slider_Max:
				if (data->max != tidata) {
					data->max = tidata;
					res++;
				}
				break;
			case MUIA_Slider_Level:
				if (data->level != tidata) {
					data->level = tidata;
					res++;
				}
				break;
		}
	}
	if (res) {
		SetSuperAttrs(cl, o,
			MUIA_Prop_Entries,	data->max - data->min + 1,
			MUIA_Prop_First,	data->level - data->min,
			TAG_END);
	}
	res += DoSuperMethodA(cl, o, (Msg)ops);
	return res;
}

static IPTR SeekBar_Get(Class *cl, Object *o, struct opGet *opg) {
	struct SeekBarData *data = INST_DATA(cl, o);
	IPTR *storage = opg->opg_Storage;
	switch (opg->opg_AttrID) {
		case MUIA_Slider_Min:
			*storage = data->min;
			break;
		case MUIA_Slider_Max:
			*storage = data->max;
			break;
		case MUIA_Slider_Level:
			data->level = XGET(o, MUIA_Prop_First) + data->min;
			*storage = data->level;
			break;
		default:
			return DoSuperMethodA(cl, o, (Msg)opg);
	}
	return 1;
}
