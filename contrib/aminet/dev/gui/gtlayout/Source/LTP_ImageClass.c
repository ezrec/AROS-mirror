/*
**	GadTools layout toolkit
**
**	Copyright © 1993-1998 by Olaf `Olsen' Barthel
**		Freely distributable.
**
**	:ts=4
*/

#ifndef _GTLAYOUT_GLOBAL_H
#include "gtlayout_global.h"
#endif

/*****************************************************************************/

#include <intuition/classes.h>

#include <clib/alib_protos.h>	/* For Coerce/Do/DoSuperMethod */

/*****************************************************************************/

#include "Assert.h"

/*****************************************************************************/


STATIC ULONG
ImageClassDraw(struct Image *image,struct impDraw *drawMsg,ImageInfo *imageInfo)
{
	LTP_DrawBox(drawMsg->imp_RPort,drawMsg->imp_DrInfo,image->LeftEdge + drawMsg->imp_Offset.X,image->TopEdge + drawMsg->imp_Offset.Y,image->Width,image->Height,drawMsg->imp_State == IDS_SELECTED,drawMsg->imp_State == IDS_DISABLED,imageInfo);

	return(TRUE);
}


/*****************************************************************************/


STATIC ULONG
ImageClassErase(struct Image *image,struct impErase *eraseMsg)
{
	LTP_EraseBox(eraseMsg->imp_RPort,eraseMsg->imp_Offset.X + image->LeftEdge,eraseMsg->imp_Offset.Y + image->TopEdge,image->Width,image->Height);

	return(TRUE);
}


/*****************************************************************************/

STATIC ULONG
ImageClassNew(struct IClass *class,Object *object,struct opSet *SetMethod)
{
	struct TagItem *Item;

	if(Item = FindTagItem(IIA_ImageType,SetMethod->ops_AttrList))
	{
		struct Image *NewImage;

		if(NewImage = (struct Image *)DoSuperMethodA(class,object,(Msg)SetMethod))
		{
			struct ImageInfo	*MoreInfo = INST_DATA(class,NewImage);
			struct TagItem		*List = SetMethod->ops_AttrList;

			memset(MoreInfo,0,sizeof(struct ImageInfo));

			MoreInfo->ImageType = Item->ti_Data;

			while(Item = NextTagItem(&List))
			{
				switch(Item->ti_Tag)
				{
					case IIA_GlyphWidth:
						MoreInfo->GlyphWidth = Item->ti_Data;
						break;

					case IIA_GlyphHeight:
						MoreInfo->GlyphHeight = Item->ti_Data;
						break;

					case IIA_Lines:
						MoreInfo->Lines = (STRPTR *)Item->ti_Data;
						break;

					case IIA_LineCount:
						MoreInfo->LineCount = Item->ti_Data;
						break;

					case IIA_KeyStroke:
						MoreInfo->KeyStroke = (STRPTR)Item->ti_Data;
						break;

					case IIA_Emboss:
						MoreInfo->Emboss = Item->ti_Data;
						break;

					case IIA_Label:
						MoreInfo->Label = (STRPTR)Item->ti_Data;
						break;

					case IIA_Font:
						if(!(MoreInfo->Font = OpenFont((struct TextAttr *)Item->ti_Data)))
						{
							CoerceMethod(class,object,OM_DISPOSE);

							return(NULL);
						}

						break;
				}
			}

			if(V39)
			{
				if(!(MoreInfo->FrameImage = (struct Image *)NewObject(NULL,FRAMEICLASS,
					IA_Left,		NewImage->LeftEdge,
					IA_Top,			NewImage->TopEdge,
					IA_Width,		NewImage->Width,
					IA_Height,		NewImage->Height,
					IA_FrameType,	FRAME_BUTTON,
				TAG_DONE)))
				{
					CoerceMethod(class,object,OM_DISPOSE);

					return(NULL);
				}

					/* Check if the class has been replaced, assuming that the
					 * replacement will live in memory rather than in ROM space.
					 */

				if(TypeOfMem(OCLASS(MoreInfo->FrameImage)->cl_Dispatcher.h_Entry))
					MoreInfo->UseFrame = TRUE;
			}

			return((ULONG)NewImage);
		}
	}

	return(NULL);
}


/*****************************************************************************/


STATIC VOID
ImageClassDispose(struct IClass *class,Object *object,Msg msg)
{
	struct ImageInfo *MoreInfo = INST_DATA(class,object);

	if(MoreInfo->Font)
		CloseFont(MoreInfo->Font);

	if(MoreInfo->FrameImage)
		DisposeObject(MoreInfo->FrameImage);
}


/*****************************************************************************/

#ifndef __AROS__
ULONG SAVE_DS ASM
LTP_ImageDispatch(REG(a0) struct IClass *class,REG(a2) Object *object,REG(a1) Msg msg)
#else
AROS_UFH3(ULONG, LTP_ImageDispatch,
	  AROS_UFHA(struct IClass *, class, A0),
	  AROS_UFHA(Object *, object, A2),
	  AROS_UFHA(Msg, msg, A1)
)
#endif
{
        AROS_USERFUNC_INIT
	
	switch(msg->MethodID)
	{
		case IM_ERASE:
			return(ImageClassErase((struct Image *)object,(struct impErase *)msg));

		case IM_DRAW:
			return(ImageClassDraw((struct Image *)object,(struct impDraw *)msg,(ImageInfo *)INST_DATA(class,object)));

		case OM_NEW:
			return(ImageClassNew(class,object,(struct opSet *)msg));

		case OM_DISPOSE:
			ImageClassDispose(class,object,msg);

			// Falls down to the default case...
	}

	return(DoSuperMethodA(class,object,msg));
    
        AROS_USERFUNC_EXIT
}
