/*
**	Matrix.c
**
**	Single character entry
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

#ifdef __AROS__
#include <aros/asmcall.h>
# ifdef Dispatch
#  undef Dispatch
# endif
AROS_UFP3(STATIC ULONG, Dispatch,
 AROS_UFPA(Class *        , class , A0),
 AROS_UFPA(Object *       , obj, A2),
 AROS_UFPA(Msg            , msg, A1));
#endif


#define MIA_CellWidth	(TAG_USER+0x70000)
#define MIA_CellHeight	(TAG_USER+0x70001)
#define MIA_Width		(TAG_USER+0x70002)
#define MIA_Height		(TAG_USER+0x70003)
#define MIA_Labels		(TAG_USER+0x70004)
#define MIA_NumLabels	(TAG_USER+0x70005)
#define MIA_Font		(TAG_USER+0x70006)
#define MIA_Index		(TAG_USER+0x70007)

typedef struct MatrixInfo
{
	UWORD			 CellWidth,
					 CellHeight;
	UWORD			 Width,
					 Height;
	UWORD			 PixelWidth,
					 PixelHeight;
	WORD			 CurrentX,
					 CurrentY;
	STRPTR			*Labels;
	LONG			 NumLabels;
	struct TextFont	*Font;
} MatrixInfo;

STATIC Class		*MatrixClass;
STATIC Object		*MatrixGadget;

STATIC UBYTE		 TitleBuffer[256];

STATIC VOID
DrawCellText(struct RastPort *RPort,LONG Left,LONG Top,LONG Width,LONG Height,STRPTR Label)
{
	struct TextExtent Extent;
	LONG Len;

	if(Len = TextFit(RPort,Label,strlen(Label),&Extent,NULL,1,Width,32767))
		PlaceText(RPort,Left + (Width - Extent.te_Width) / 2,Top + (Height - Extent.te_Height) / 2,Label,Len);
}

STATIC VOID
DrawCell(struct RastPort *RPort,LONG PixelLeft,LONG PixelTop,UWORD *Pens,MatrixInfo *Info,LONG x,LONG y,BOOL Highlight)
{
	UWORD Shine,Shadow,Text,Fill;
	LONG Left,Top;
	LONG Index;

	Left	= PixelLeft	+ Info->CellWidth	* x;
	Top		= PixelTop	+ Info->CellHeight	* y;

	if(Highlight)
	{
		Shine	= SHADOWPEN;
		Shadow	= SHINEPEN;
		Text	= FILLTEXTPEN;
		Fill	= FILLPEN;
	}
	else
	{
		Shine	= SHINEPEN;
		Shadow	= SHADOWPEN;
		Text	= TEXTPEN;
		Fill	= BACKGROUNDPEN;
	}

	SetDrMd(RPort,JAM1);

	SetAPen(RPort,Pens[Shine]);
	Move(RPort,Left,Top + Info->CellHeight - 1);
	Draw(RPort,Left,Top);
	Draw(RPort,Left + Info->CellWidth - 1,Top);

	SetAPen(RPort,Pens[Shadow]);
	Move(RPort,Left + 1,Top + Info->CellHeight - 1);
	Draw(RPort,Left + Info->CellWidth - 1,Top + Info->CellHeight - 1);
	Draw(RPort,Left + Info->CellWidth - 1,Top + 1);

	SetAPen(RPort,Pens[Fill]);
	RectFill(RPort,Left + 1,Top + 1,Left + Info->CellWidth - 2,Top + Info->CellHeight - 2);

	if((Index = y * Info->Width + x) < Info->NumLabels)
	{
		SetAPen(RPort,Pens[Text]);
		DrawCellText(RPort,Left + 1,Top + 1,Info->CellWidth - 2,Info->CellHeight - 2,Info->Labels[Index]);
	}
}

STATIC VOID
DrawGrid(struct RastPort *RPort,LONG PixelLeft,LONG PixelTop,UWORD *Pens,MatrixInfo *Info)
{
	LONG i,j,Left,Top,Bottom,Strange,Charm;

	SetDrMd(RPort,JAM1);

	Left	= PixelLeft;
	Top		= PixelTop;
	Bottom	= PixelTop + Info->PixelHeight - 1;

	Strange	= Pens[SHINEPEN];
	Charm	= Pens[SHADOWPEN];

	for(i = 0 ; i < Info->Width ; i++)
	{
		SetAPen(RPort,Strange);
		Move(RPort,Left,Top);
		Draw(RPort,Left,Bottom);

		Left += Info->CellWidth;

		SetAPen(RPort,Charm);
		Move(RPort,Left - 1,Top);
		Draw(RPort,Left - 1,Bottom);
	}

	Top = PixelTop;

	for(j = 0 ; j < Info->Height ; j++)
	{
		Left = PixelLeft;

		for(i = 0 ; i < Info->Width ; i++)
		{
			SetAPen(RPort,Strange);
			Move(RPort,Left,Top);
			Draw(RPort,Left + Info->CellWidth - 1,Top);

			SetAPen(RPort,Charm);
			Move(RPort,Left + 1,Top + Info->CellHeight - 1);
			Draw(RPort,Left + Info->CellWidth - 2,Top + Info->CellHeight - 1);

			Left += Info->CellWidth;
		}

		Top += Info->CellHeight;
	}
}

STATIC VOID
NotifyIndex(Class *class,Object *object,struct GadgetInfo *GInfo,struct opUpdate *msg,LONG Index)
{
	struct TagItem Tags[3];

	Tags[0].ti_Tag	= MIA_Index;
	Tags[0].ti_Data	= Index;
	Tags[1].ti_Tag	= GA_ID;
	Tags[1].ti_Data	= ((struct Gadget *)object)->GadgetID;
	Tags[2].ti_Tag	= TAG_DONE;

	DoSuperMethod(class,object,OM_NOTIFY,Tags,GInfo,((msg->MethodID == OM_UPDATE) ? (msg->opu_Flags) : 0L));
}

STATIC BOOL
SetMethod(Class *class,Object *object,struct opSet *SetInfo)
{
	struct TagItem * Tag_;
	MatrixInfo *Info;

	Info = INST_DATA(class,object);

	if(Tag_ = FindTagItem(MIA_Index,SetInfo->ops_AttrList))
	{
		struct RastPort *RPort;

		if(RPort = ObtainGIRPort(SetInfo->ops_GInfo))
		{
			UWORD *Pens;
			LONG Index;

			Index	= (LONG)Tag_->ti_Data;
			Pens	= SetInfo->ops_GInfo->gi_DrInfo->dri_Pens;

			SetFont(RPort,Info->Font);

			if(Info->CurrentX != -1 && Info->CurrentY !=-1)
				DrawCell(RPort,((struct Gadget *)object)->LeftEdge,((struct Gadget *)object)->TopEdge,Pens,Info,Info->CurrentX,Info->CurrentY,FALSE);

			if(Index >= 0 && Index < Info->Width * Info->Height)
			{
				Info->CurrentX = Index % Info->Width;
				Info->CurrentY = Index / Info->Width;

				DrawCell(RPort,((struct Gadget *)object)->LeftEdge,((struct Gadget *)object)->TopEdge,Pens,Info,Info->CurrentX,Info->CurrentY,TRUE);

				NotifyIndex(class,object,SetInfo->ops_GInfo,(struct opUpdate *)SetInfo,Index);
			}
			else
			{
				Info->CurrentX = Info->CurrentY = -1;

				NotifyIndex(class,object,SetInfo->ops_GInfo,(struct opUpdate *)SetInfo,-1);
			}

			ReleaseGIRPort(RPort);

			return(TRUE);
		}
	}

	return(FALSE);
}

STATIC ULONG
RenderMethod(Class *class,Object *object,struct gpRender *RenderInfo)
{
	LONG i,x,y,Left,Top;
	MatrixInfo *Info;
	UWORD *Pens;

	Info = INST_DATA(class,object);
	Pens = RenderInfo->gpr_GInfo->gi_DrInfo->dri_Pens;

	SetFont(RenderInfo->gpr_RPort,Info->Font);

	Left	= ((struct Gadget *)object)->LeftEdge;
	Top		= ((struct Gadget *)object)->TopEdge;

	SetAPen(RenderInfo->gpr_RPort,Pens[BACKGROUNDPEN]);
	FillBox(RenderInfo->gpr_RPort,Left,Top,((struct Gadget *)object)->Width,((struct Gadget *)object)->Height);

	DrawGrid(RenderInfo->gpr_RPort,Left,Top,Pens,Info);

	SetAPen(RenderInfo->gpr_RPort,Pens[TEXTPEN]);

	for(i = 0 ; i < Info->NumLabels ; i++)
	{
		x = (i % Info->Width) * Info->CellWidth;
		y = (i / Info->Width) * Info->CellHeight;

		DrawCellText(RenderInfo->gpr_RPort,Left + x,Top + y,Info->CellWidth,Info->CellHeight,Info->Labels[i]);
	}

	return(TRUE);
}

STATIC VOID
DisposeMethod(Class *class,Object *object,Msg UnusedMsg)
{
	MatrixInfo *Info = INST_DATA(class,object);

	if(Info->Font)
		CloseFont(Info->Font);
}

STATIC ULONG
NewMethod(Class *class,Object *object,struct opSet *SetInfo)
{
	if(object = (Object *)DoSuperMethodA(class,object,(Msg)SetInfo))
	{
		struct TagItem	*Tag,*TagList;
		LONG			 Width,Height;
		MatrixInfo		*Info;
		struct TextAttr	*Font;

		Info	= INST_DATA(class,object);
		TagList	= SetInfo->ops_AttrList;
		Width	= 0;
		Height	= 0;
		Font	= NULL;

		memset(Info,0,sizeof(MatrixInfo));

		while(Tag = NextTagItem(&TagList))
		{
			switch(Tag->ti_Tag)
			{
				case MIA_CellWidth:

					Info->CellWidth = Tag->ti_Data;
					break;

				case MIA_CellHeight:

					Info->CellHeight = Tag->ti_Data;
					break;

				case MIA_Width:

					Info->Width = Tag->ti_Data;
					break;

				case MIA_Height:

					Info->Height = Tag->ti_Data;
					break;

				case MIA_Labels:

					Info->Labels = (STRPTR *)Tag->ti_Data;
					break;

				case MIA_NumLabels:

					Info->NumLabels = Tag->ti_Data;
					break;

				case MIA_Font:

					Font = (struct TextAttr *)Tag->ti_Data;
					break;

				case GA_Width:

					Width = Tag->ti_Data;
					break;

				case GA_Height:

					Height = Tag->ti_Data;
					break;
			}
		}

		if(Info->Labels && !Info->NumLabels)
		{
			while(Info->Labels[Info->NumLabels])
				Info->NumLabels++;
		}

		if(Font && Info->CellWidth && Info->CellHeight)
		{
			if(!Info->Width)
				Info->Width = Width / Info->CellWidth;

			if(!Info->Height)
				Info->Height = Height / Info->CellHeight;

			if(Info->Width && Info->Height)
			{
				if(Info->Font = OpenFont(Font))
				{
					Info->CurrentX		= -1;
					Info->CurrentY		= -1;
					Info->PixelWidth	= Info->Width	* Info->CellWidth;
					Info->PixelHeight	= Info->Height	* Info->CellHeight;

					return((ULONG)object);
				}
			}
		}

		CoerceMethod(class,object,OM_DISPOSE);
	}

	return(0);
}

STATIC ULONG
InputMethod(Class *class,Object *object,struct gpInput *InputInfo)
{
	MatrixInfo *Info;
	ULONG Result;
	UWORD *Pens;
	BOOL Done;
	LONG x,y;

	Info	= INST_DATA(class,object);
	Result	= GMR_MEACTIVE;
	Pens	= InputInfo->gpi_GInfo->gi_DrInfo->dri_Pens;
	Done	= FALSE;

	if(InputInfo->gpi_IEvent->ie_Class == IECLASS_RAWMOUSE)
	{
		if(InputInfo->gpi_IEvent->ie_Code == MENUDOWN)
		{
			Result	= GMR_NOREUSE;
			Done	= TRUE;
		}
		else
		{
			if(InputInfo->gpi_IEvent->ie_Code == SELECTUP)
			{
				Done = TRUE;

				if(Info->CurrentX != -1 && Info->CurrentY != -1)
				{
					Result = GMR_REUSE | GMR_VERIFY;

					*InputInfo->gpi_Termination = Info->CurrentY * Info->Width + Info->CurrentX;
				}
				else
					Result = GMR_NOREUSE;
			}
		}

		if(Done && Info->CurrentX != -1 && Info->CurrentY != -1)
		{
			struct RastPort	*RPort;

			if(RPort = ObtainGIRPort(InputInfo->gpi_GInfo))
			{
				SetFont(RPort,Info->Font);

				DrawCell(RPort,((struct Gadget *)object)->LeftEdge,((struct Gadget *)object)->TopEdge,Pens,Info,Info->CurrentX,Info->CurrentY,FALSE);

				Info->CurrentX	= -1;
				Info->CurrentY	= -1;

				ReleaseGIRPort(RPort);

				NotifyIndex(class,object,InputInfo->gpi_GInfo,(struct opUpdate *)InputInfo,-1);
			}
		}
	}

	if(!Done)
	{
		x = InputInfo->gpi_Mouse.X / Info->CellWidth;
		y = InputInfo->gpi_Mouse.Y / Info->CellHeight;

		if(InputInfo->gpi_Mouse.X < 0 || InputInfo->gpi_Mouse.Y < 0 || x >= Info->Width || y >= Info->Height)
			x = y = -1;

		if(x != Info->CurrentX || y != Info->CurrentY)
		{
			struct RastPort	*RPort;

			if(RPort = ObtainGIRPort(InputInfo->gpi_GInfo))
			{
				SetFont(RPort,Info->Font);

				if(Info->CurrentX != -1 && Info->CurrentY != -1)
					DrawCell(RPort,((struct Gadget *)object)->LeftEdge,((struct Gadget *)object)->TopEdge,Pens,Info,Info->CurrentX,Info->CurrentY,FALSE);

				Info->CurrentX	= x;
				Info->CurrentY	= y;

				if(x != -1 && y != -1)
				{
					DrawCell(RPort,((struct Gadget *)object)->LeftEdge,((struct Gadget *)object)->TopEdge,Pens,Info,Info->CurrentX,Info->CurrentY,TRUE);

					NotifyIndex(class,object,InputInfo->gpi_GInfo,(struct opUpdate *)InputInfo,y * Info->Width + x);
				}
				else
					NotifyIndex(class,object,InputInfo->gpi_GInfo,(struct opUpdate *)InputInfo,-1);

				ReleaseGIRPort(RPort);
			}
		}
	}

	return(Result);
}

STATIC ULONG
ActiveMethod(Class *class,Object *object,struct gpInput *InputInfo)
{
	struct RastPort	*RPort;

	if(RPort = ObtainGIRPort(InputInfo->gpi_GInfo))
	{
		MatrixInfo *Info;
		UWORD *Pens;

		Info = INST_DATA(class,object);
		Pens = InputInfo->gpi_GInfo->gi_DrInfo->dri_Pens;

		SetFont(RPort,Info->Font);

		if(Info->CurrentX != -1 && Info->CurrentY !=-1)
			DrawCell(RPort,((struct Gadget *)object)->LeftEdge,((struct Gadget *)object)->TopEdge,Pens,Info,Info->CurrentX,Info->CurrentY,FALSE);

		Info->CurrentX = InputInfo->gpi_Mouse.X / Info->CellWidth;
		Info->CurrentY = InputInfo->gpi_Mouse.Y / Info->CellHeight;

		if(Info->CurrentX > Info->Width - 1)
			Info->CurrentX = Info->Width - 1;
		else
		{
			if(Info->CurrentX < 0)
				Info->CurrentX = 0;
		}

		if(Info->CurrentY > Info->Height - 1)
			Info->CurrentY = Info->Height - 1;
		else
		{
			if(Info->CurrentY < 0)
				Info->CurrentY = 0;
		}

		DrawCell(RPort,((struct Gadget *)object)->LeftEdge,((struct Gadget *)object)->TopEdge,Pens,Info,Info->CurrentX,Info->CurrentY,TRUE);

		NotifyIndex(class,object,InputInfo->gpi_GInfo,(struct opUpdate *)InputInfo,Info->CurrentY * Info->Width + Info->CurrentX);

		ReleaseGIRPort(RPort);

		return(GMR_MEACTIVE);
	}
	else
		return(GMR_NOREUSE);
}

STATIC ULONG
InactiveMethod(Class *class,Object *object,struct gpGoInactive *InactiveInfo)
{
	MatrixInfo *Info = INST_DATA(class,object);

	if(Info->CurrentX != -1 && Info->CurrentY != -1)
	{
		struct RastPort	*RPort;

		if(RPort = ObtainGIRPort(InactiveInfo->gpgi_GInfo))
		{
			SetFont(RPort,Info->Font);

			DrawCell(RPort,((struct Gadget *)object)->LeftEdge,((struct Gadget *)object)->TopEdge,InactiveInfo->gpgi_GInfo->gi_DrInfo->dri_Pens,Info,Info->CurrentX,Info->CurrentY,FALSE);

			ReleaseGIRPort(RPort);

			NotifyIndex(class,object,InactiveInfo->gpgi_GInfo,(struct opUpdate *)InactiveInfo,-1);
		}
	}

	Info->CurrentX = -1;
	Info->CurrentY = -1;

	return(0);
}

STATIC ULONG
HitTestMethod(Class *class,Object *object,struct gpHitTest *HitInfo)
{
	MatrixInfo *Info = INST_DATA(class,object);

	if(HitInfo->gpht_Mouse.X < Info->PixelWidth && HitInfo->gpht_Mouse.Y < Info->PixelHeight)
		return(GMR_GADGETHIT);
	else
		return(0);
}

#ifndef __AROS__
STATIC ULONG SAVE_DS ASM
Dispatch(REG(a0) Class *class,REG(a2) Object *object,REG(a1) Msg msg)
#else
AROS_UFH3(STATIC ULONG, Dispatch,
 AROS_UFHA(Class *        , class , A0),
 AROS_UFHA(Object *       , object, A2),
 AROS_UFHA(Msg            , msg, A1))
#endif
{
#ifdef __AROS__
    AROS_USERFUNC_INIT
#endif
	switch(msg->MethodID)
	{
		case OM_NEW:

			return(NewMethod(class,object,(struct opSet *)msg));

		case OM_UPDATE:
		case OM_SET:

			if(SetMethod(class,object,(struct opSet *)msg))
				return(TRUE);
			else
				break;

		case GM_RENDER:

			return(RenderMethod(class,object,(struct gpRender *)msg));

		case GM_HITTEST:

			return(HitTestMethod(class,object,(struct gpHitTest *)msg));

		case GM_GOINACTIVE:

			return(InactiveMethod(class,object,(struct gpGoInactive *)msg));

		case GM_GOACTIVE:

			return(ActiveMethod(class,object,(struct gpInput *)msg));

		case GM_HANDLEINPUT:

			return(InputMethod(class,object,(struct gpInput *)msg));

		case OM_DISPOSE:

			DisposeMethod(class,object,msg);
	}

	return(DoSuperMethodA(class,object,msg));
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

STATIC VOID
DeleteMatrixClass(VOID)
{
	if(MatrixClass)
	{
		FreeClass(MatrixClass);
		MatrixClass = NULL;
	}
}

STATIC Class *
CreateMatrixClass(VOID)
{
	if(MatrixClass = MakeClass(NULL,GADGETCLASS,NULL,sizeof(MatrixInfo),0))
		InitHook(&MatrixClass->cl_Dispatcher,(HOOKFUNC)Dispatch,NULL);

	return(MatrixClass);
}

VOID
CloseMatrixWindow()
{
	CheckItem(MEN_MATRIX_WINDOW,FALSE);

	if(MatrixWindow)
	{
		PutWindowInfo(WINDOW_CHARTAB,MatrixWindow->LeftEdge,MatrixWindow->TopEdge,MatrixWindow->Width,MatrixWindow->Height);

		ClearMenuStrip(MatrixWindow);

		CloseWindowSafely(MatrixWindow);

		MatrixWindow = NULL;
	}

	if(MatrixGadget)
	{
		DisposeObject(MatrixGadget);

		MatrixGadget = NULL;
	}

	DeleteMatrixClass();
}

BOOL
DispatchMatrixWindow(ULONG *MsgClass,UWORD MsgCode,ULONG MsgQualifier,UBYTE Char)
{
	if(MatrixWindow)
	{
		switch(*MsgClass)
		{
			case IDCMP_RAWKEY:

				if(!(MsgCode & IECODE_UP_PREFIX) && !(MsgQualifier & IEQUALIFIER_REPEAT) && !(MsgCode >= CURSOR_UP_CODE && MsgCode <= F10_CODE) && !(MsgCode >= HELP_CODE && MsgCode <= RAMIGA_CODE))
				{
					SetGadgetAttrs((struct Gadget *)MatrixGadget,MatrixWindow,NULL,
						MIA_Index,	Char,
					TAG_DONE);

					Delay(TICKS_PER_SECOND / 10);

					SetGadgetAttrs((struct Gadget *)MatrixGadget,MatrixWindow,NULL,
						MIA_Index,	-1,
					TAG_DONE);
				}

				break;

			case IDCMP_CLOSEWINDOW:

				*MsgClass = 0;

				return(TRUE);

			case IDCMP_GADGETUP:

				*MsgClass = 0;

				Char = MsgCode;

				SerWrite(&Char,1);

				break;

			case IDCMP_IDCMPUPDATE:

				*MsgClass = 0;

				if((WORD)MsgCode < 0)
					SetWindowTitles(MatrixWindow,LocaleString(MSG_MATRIX_WINDOW_TITLE),(STRPTR)~0);
				else
				{
					UBYTE	OctalBuffer[6],BinBuffer[10];
					LONG	i,Code;

					OctalBuffer[0] = '0';

					for(Code = MsgCode, i = 0 ; i < 3 ; i++)
					{
						OctalBuffer[2 - i + 1] = '0' + (Code & 7);

						Code = Code >> 3;
					}

					OctalBuffer[4] = 0;

					BinBuffer[0] = '%';

					for(Code = MsgCode, i = 0 ; i < 8 ; i++)
					{
						BinBuffer[7 - i + 1] = '0' + (Code & 1);

						Code = Code >> 1;
					}

					BinBuffer[9] = 0;

					LimitedSPrintf(sizeof(TitleBuffer),TitleBuffer,"%s [%ld · $%02lx · %s · %s]",LocaleString(MSG_MATRIX_WINDOW_TITLE),MsgCode,MsgCode,OctalBuffer,BinBuffer);

					SetWindowTitles(MatrixWindow,TitleBuffer,(STRPTR)~0);
				}

				break;
		}
	}

	return(FALSE);
}

struct Window *
OpenMatrixWindow(struct Window *Parent)
{
	if(MatrixWindow)
	{
		WindowToFront(MatrixWindow);

		ActivateWindow(MatrixWindow);

		return(MatrixWindow);
	}
	else
	{
		LONG i,MaxWidth,Width,Height;
		struct RastPort RastPort;
		struct TextFont *Font;

		InitRastPort(&RastPort);
		Font = OpenFont((struct TextAttr *)&TextAttr);
		SetFont(&RastPort,Font);

		for(i = 0, MaxWidth = 0 ; i < 256 ; i++)
		{
			if((Width = TextLength(&RastPort,CharCodes[i],strlen(CharCodes[i]))) > MaxWidth)
				MaxWidth = Width;
		}

		Width	= 2 + MaxWidth + 2;
		Height	= 2 + RastPort.TxHeight + 2;

		if(Parent->WScreen->WBorTop + Parent->WScreen->Font->ta_YSize + 1 + Parent->WScreen->WBorBottom + 16 * Height > Parent->WScreen->Height || Parent->WScreen->WBorLeft + 16 * Width + Parent->WScreen->WBorRight > Parent->WScreen->Width)
		{
			DisplayBeep(Parent->WScreen);

			return(NULL);
		}

		if(CreateMatrixClass())
		{
			STATIC Tag Map[] = { MIA_Index, ICSPECIAL_CODE, TAG_DONE };

			if(MatrixGadget = NewObject(MatrixClass,NULL,
				GA_Left,			Parent->WScreen->WBorLeft + 2,
				GA_Top,				Parent->WScreen->WBorTop + Parent->WScreen->Font->ta_YSize + 1 + 2,
				GA_Width,			16 * Width,
				GA_Height,			16 * Height,
				GA_RelVerify,		TRUE,

				ICA_TARGET,			ICTARGET_IDCMP,
				ICA_MAP,			Map,

				MIA_Width,			16,
				MIA_Height,			16,
				MIA_CellWidth,		Width,
				MIA_CellHeight,		Height,
				MIA_Labels,			CharCodes,
				MIA_NumLabels,		256,
				MIA_Font,			&TextAttr,
			TAG_DONE))
			{
				LONG Left,Top,DummyWidth,DummyHeight;

				Left = Top = DummyWidth = DummyHeight = 0;

				GetWindowInfo(WINDOW_CHARTAB,&Left,&Top,&DummyWidth,&DummyHeight,0,0);

				if(MatrixWindow = OpenWindowTags(NULL,
					WA_Top,				Left,
					WA_Left,			Top,
					WA_Title,			LocaleString(MSG_MATRIX_WINDOW_TITLE),
					WA_DepthGadget,		TRUE,
					WA_CloseGadget,		TRUE,
					WA_DragBar,			TRUE,
					WA_InnerWidth,		2 + (Width * 16) + 2,
					WA_InnerHeight,		2 + (Height * 16) + 2,
					WA_Activate,		TRUE,
					WA_Gadgets,			MatrixGadget,
					OpenWindowTag,		Parent->WScreen,
					WA_NewLookMenus,	TRUE,
					BackfillTag,		&BackfillHook,
					WA_SimpleRefresh,	TRUE,

					AmigaGlyph ? WA_AmigaKey  : TAG_IGNORE, AmigaGlyph,
					CheckGlyph ? WA_Checkmark : TAG_IGNORE, CheckGlyph,
				TAG_DONE))
				{
					MatrixWindow->UserPort = Parent->UserPort;

					if(ModifyIDCMP(MatrixWindow,Parent->IDCMPFlags | IDCMP_CLOSEWINDOW | IDCMP_GADGETUP | IDCMP_IDCMPUPDATE | IDCMP_REFRESHWINDOW))
					{
						SetMenuStrip(MatrixWindow,Menu);

						CheckItem(MEN_MATRIX_WINDOW,TRUE);

						return(MatrixWindow);
					}

					MatrixWindow->UserPort = NULL;

					CloseWindow(MatrixWindow);

					MatrixWindow = NULL;
				}

				DisposeObject(MatrixGadget);

				MatrixGadget = NULL;
			}

			DeleteMatrixClass();
		}

		CheckItem(MEN_MATRIX_WINDOW,FALSE);

		return(NULL);
	}
}
