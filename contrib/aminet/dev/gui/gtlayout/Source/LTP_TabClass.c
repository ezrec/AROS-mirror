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

#include <exec/memory.h>
#include <hardware/blit.h>

#include <clib/alib_protos.h>	/* For Coerce/Do/DoSuperMethod */

#include <stdarg.h>

/*****************************************************************************/

#include "Assert.h"

#ifdef DO_TAB_KIND	/* Support code */

#define TIA_Labels		(TAG_USER+0x90000)
#define TIA_Font		(TAG_USER+0x90001)
#define TIA_Screen		(TAG_USER+0x90002)
#define TIA_Index		(TAG_USER+0x90003)
#define TIA_DrawInfo	(TAG_USER+0x90004)
#define TIA_SizeType	(TAG_USER+0x90005)

STATIC VOID
DrawCap(struct RastPort *RPort,LONG Left,LONG Top,LONG Width,LONG Height,BOOL Flip)
{
	LONG x,y,da,MidX,MidY,r,Full,Push,Mult;

	Full = Height;

	Height = Width;

	MidX = Left;
	MidY = Top + Height - 1;

	r = Height - 1;

	x = 0;
	y = r;
	da = r - 1;

	if(Flip)
	{
		Push = Width - 1;
		Mult = -1;
	}
	else
	{
		Push = 0;
		Mult = 1;
	}

	do
	{
		if(da < 0)
		{
			y--;

			da += y * 2;
		}

		WritePixel(RPort,Left + Push + Mult * (x + MidX - Left),-y + MidY);
		WritePixel(RPort,Left + Push + Mult * (y + MidX - Left),-x + MidY);

		da -= (1 + x * 2);

		x++;
	}
	while(x <= y);

	Left += Width - 1 - Push;

	LTP_DrawLine(RPort,Left,Top + Height,Left,Top + Full - 1);
}

STATIC VOID
RenderTabs(struct RastPort *RPort,struct Gadget *gadget,TabInfo *Info,UWORD *Pens,LONG Pull)
{
	LONG	Width,Height;
	LONG	i;
	LONG	Pen;

	Width	= Info->TabWidth;
	Height	= Info->TabHeight;

	SetRast(&Info->RPort,Pens[BACKGROUNDPEN]);

	for(i = 0 ; i < 4 ; i++)
	{
		if(i & 1)
		{
			if(Pens[SHINEPEN] == Pens[SHADOWPEN])
				Pen = BACKGROUNDPEN;
			else
				Pen = SHADOWPEN;
		}
		else
			Pen = SHINEPEN;

		LTP_SetAPen(&Info->RPort,Pens[Pen]);

		LTP_DrawLine(&Info->RPort,0,Height - 1 - i,gadget->Width - 1 - Info->Thick * i,Height - 1 - i);
	}

	for(i = Info->Count - 1 ; i >= 0 ; i--)
	{
		if(i != Info->Current)
			BltMaskBitMapRastPort(Info->Tabs[i].BitMap,0,0,&Info->RPort,Info->Tabs[i].Left,4,Width,Height - 6,ABC|ABNC|ANBC,Info->Mask);
	}

	BltMaskBitMapRastPort(Info->Tabs[Info->Current].BitMap,0,0,&Info->RPort,Info->Tabs[Info->Current].Left,0,Width,Height - Pull,ABC|ABNC|ANBC,Info->Mask);

	WaitBlit();

	BltBitMapRastPort(Info->BitMap,0,0,RPort,gadget->LeftEdge,gadget->TopEdge + gadget->Height - (Info->TabHeight + 2),gadget->Width,Info->TabHeight,0xC0);
}

STATIC VOID
SetMethod(struct IClass *class,struct Gadget *gadget,struct opSet *SetInfo)
{
	TabInfo			*Info = INST_DATA(class,gadget);
	struct TagItem	*_Tag;
	BOOL			 NeedRefresh = FALSE;
	BOOL			 Disabled;

	Disabled = (BOOL)((gadget->Flags & GFLG_DISABLED) != 0);

	if (_Tag = FindTagItem(GA_Disabled,SetInfo->ops_AttrList))
	{
		if(_Tag->ti_Data && !Disabled || !_Tag->ti_Data && Disabled)
			NeedRefresh = TRUE;
	}

	if(_Tag = FindTagItem(TIA_Index,SetInfo->ops_AttrList))
	{
		LONG Index = _Tag->ti_Data;

		if(Index >= Info->Count)
			Index = Info->Count - 1;

		if(Index != Info->Current)
		{
			Info->Initial = Info->Current = Index;

			NeedRefresh = TRUE;
		}
	}

	if(NeedRefresh)
	{
		struct RastPort *RPort;

		if(RPort = ObtainGIRPort(SetInfo->ops_GInfo))
		{
			DoMethod((Object *)gadget,GM_RENDER,SetInfo->ops_GInfo,RPort,GREDRAW_REDRAW);

			ReleaseGIRPort(RPort);
		}
	}
}

STATIC ULONG
RenderMethod(struct IClass *class,struct Gadget *gadget,struct gpRender *RenderInfo)
{
	TabInfo *Info = INST_DATA(class,gadget);

	RenderTabs(RenderInfo->gpr_RPort,gadget,Info,RenderInfo->gpr_GInfo->gi_DrInfo->dri_Pens,RenderInfo->gpr_Redraw == GREDRAW_UPDATE ? 2 : 0);

	return(TRUE);
}

STATIC VOID
DisposeMethod(struct IClass *class,struct Gadget *gadget,Msg msg)
{
	TabInfo *Info = INST_DATA(class,gadget);

	if(Info->Mask || Info->Tabs || Info->BitMap)
		WaitBlit();

	if(Info->Mask)
	{
		FreeRaster(Info->Mask,Info->TabWidth,Info->TabHeight);
	}

	if(Info->Tabs)
	{
		LONG i;

		for(i = 0 ; i < Info->Count ; i++)
			LTP_DeleteBitMap(Info->Tabs[i].BitMap,TRUE);

		FreeVec(Info->Tabs);
	}

	LTP_DeleteBitMap(Info->BitMap,FALSE);
}

STATIC ULONG
NewMethod(struct IClass *class,struct Gadget *gadget,struct opSet *SetInfo)
{
	if(gadget = (struct Gadget *)DoSuperMethodA(class,(Object *)gadget,(Msg)SetInfo))
	{
		TabInfo			*Info = INST_DATA(class,gadget);
		struct TagItem	*Tag,*TagList = SetInfo->ops_AttrList;
		struct TextAttr	*FontAttr;
		LONG			 Width,Height;
		struct DrawInfo	*DrawInfo;
		STRPTR			*Labels;
		struct Screen	*Screen;

		Width		= 0;
		Height		= 0;

		FontAttr	= NULL;
		DrawInfo	= NULL;
		Labels		= NULL;
		Screen		= NULL;

		memset(Info,0,sizeof(TabInfo));

		while(Tag = NextTagItem(&TagList))
		{
			switch(Tag->ti_Tag)
			{
				case GA_Width:
					Width = Tag->ti_Data;
					break;

				case GA_Height:
					Height = Tag->ti_Data;
					break;

				case TIA_Labels:
					Labels = (STRPTR *)Tag->ti_Data;
					break;

				case TIA_Font:
					FontAttr = (struct TextAttr *)Tag->ti_Data;
					break;

				case TIA_Screen:
					Screen = (struct Screen *)Tag->ti_Data;
					break;

				case TIA_Index:
					Info->Current = Tag->ti_Data;
					break;

				case TIA_DrawInfo:
					DrawInfo = (struct DrawInfo *)Tag->ti_Data;
					break;
			}
		}

		if(Labels)
		{
			while(Labels[Info->Count])
				Info->Count++;
		}

		if(Screen && DrawInfo && FontAttr && Width && Height && Labels && Info->Count)
		{
			struct TextFont *Font;

			if(Font = OpenFont(FontAttr))
			{
				struct RastPort *RPort = &Info->RPort;

				InitRastPort(RPort);

				SetFont(RPort,Font);

				if(Height >= RPort->TxHeight + 11)
				{
					LONG i,Len,MaxWidth,Remain;
					LONG Lean;

					if(Info->Current < 0)
						Info->Current = 0;
					else
					{
						if(Info->Current >= Info->Count)
							Info->Current = Info->Count - 1;
					}

					Lean = (TextLength(RPort,"O",1) + 1) / 2;

					Info->Thick	= (DrawInfo->dri_Resolution.Y + DrawInfo->dri_Resolution.X - 1) / DrawInfo->dri_Resolution.X;

					if(Info->Thick < 1)
						Info->Thick = 1;

					for(i = MaxWidth = 0 ; i < Info->Count ; i++)
					{
						if((Len = TextLength(RPort,Labels[i],strlen(Labels[i]))) > MaxWidth)
							MaxWidth = Len;
					}

					MaxWidth = Info->Thick + (Lean + Info->Thick - 1 + Info->Thick + MaxWidth + Info->Thick + Lean + Info->Thick - 1) + Info->Thick;

					Remain = (Width - MaxWidth - 5 * Info->Thick) / (Info->Thick * 4);

					if(Remain >= Info->Count - 1)
					{
						if(Info->Tabs = (TabEntry *)AllocVec(sizeof(TabEntry) * Info->Count,MEMF_ANY|MEMF_CLEAR))
						{
							STATIC const BYTE PenNumbers[] =
							{
								BACKGROUNDPEN,
								SHINEPEN,
								SHADOWPEN,
								TEXTPEN
							};

							struct BitMap	 Mask;
							LONG			 Depth;
							LONG			 MaxPen;
							UWORD			*Pens = DrawInfo->dri_Pens;
							BOOL			 GotIt = TRUE;

							memset(&Mask,0,sizeof(Mask));

							for(i = 0, MaxPen = -1 ; i < sizeof(PenNumbers) ; i++)
							{
								if(Pens[PenNumbers[i]] > MaxPen)
									MaxPen = Pens[PenNumbers[i]];
							}

							Depth = 8;	/* For the sake of the compiler initialize this. */

							for(i = 1 ; i <= 8 ; i++)
							{
								if(MaxPen < (1 << i))
								{
									Depth = i;
									break;
								}
							}

							Info->TabWidth	= MaxWidth;
							Info->TabHeight	= 4 + RPort->TxHeight + 5;

							InitBitMap(&Mask,1,Info->TabWidth,Info->TabHeight);

							Mask.Planes[1] = NULL;

							for(i = 0 ; GotIt && i < 2 ; i++)
							{
								if(!(Mask.Planes[i] = (PLANEPTR)AllocRaster(Info->TabWidth,Info->TabHeight)))
									GotIt = FALSE;
							}

							for(i = 0 ; GotIt && i < Info->Count ; i++)
							{
								if(!(Info->Tabs[i].BitMap = LTP_CreateBitMap(Info->TabWidth,Info->TabHeight,Depth,NULL,TRUE)))
									GotIt = FALSE;
							}

							if(GotIt)
							{
								Depth = LTP_GetDepth(Screen->RastPort.BitMap);

								if(!(Info->BitMap = LTP_CreateBitMap(gadget->Width,Info->TabHeight,Depth,Screen->RastPort.BitMap,FALSE)))
									GotIt = FALSE;
							}

							if(GotIt)
							{
								struct TmpRas	 TmpRas;
								LONG			 j,Len,Offset,Pos;

								InitTmpRas(&TmpRas,Mask.Planes[1],RASSIZE(Info->TabWidth,Info->TabHeight));

								RPort->TmpRas	= &TmpRas;
								RPort->BitMap	= &Mask;

								SetRast(RPort,0);

								LTP_SetPens(RPort,1,0,JAM1);

								DrawCap(RPort,0,0,Lean,Info->TabHeight,TRUE);
								LTP_DrawLine(RPort,Lean,0,Info->TabWidth - (Lean + 1),0);

								DrawCap(RPort,Info->TabWidth - Lean,0,Lean,Info->TabHeight,FALSE);
								LTP_DrawLine(RPort,0,Info->TabHeight - 1,Info->TabWidth - 1,Info->TabHeight - 1);

								Flood(RPort,1,Info->TabWidth / 2,Info->TabHeight / 2);

								LTP_SetAPen(RPort,0);

								for(i = 0 ; i < 2 ; i++)
								{
									for(j = 0 ; j < Info->Thick ; j++)
										WritePixel(RPort,j,Info->TabHeight - 1 - i);

									for(j = 0 ; j < Info->Thick ; j++)
										WritePixel(RPort,Info->TabWidth - 1 - j,Info->TabHeight - 1 - i);
								}

								WaitBlit();

								FreeRaster(Mask.Planes[1],Info->TabWidth,Info->TabHeight);

									// *VERY* important; leaving a dead TmpRas around will
									// cause problems with Text() later as Gfx may use
									// RPort->TmpRas for buffering.

								RPort->TmpRas = NULL;

								Offset = (Width - Info->Thick * 5) / Info->Count;

								while(Offset > 0 && Offset * (Info->Count - 1) + Info->TabWidth >= Width - Info->Thick * 5)
									Offset--;

								if(Offset > Info->TabWidth + Info->Thick)
									Offset = Info->TabWidth + Info->Thick;

								Pos = 2 * Info->Thick;

								for(i = 0 ; i < Info->Count ; i++)
								{
									RPort->BitMap = Info->Tabs[i].BitMap;

									Info->Tabs[i].Left = Pos;
									Pos += Offset;

									SetRast(RPort,Pens[BACKGROUNDPEN]);

									LTP_SetAPen(RPort,Pens[SHINEPEN]);

									for(j = 0 ; j < Info->Thick ; j++)
										DrawCap(RPort,Info->Thick + j,0,Lean,Info->TabHeight,TRUE);

									LTP_DrawLine(RPort,Info->Thick + Lean,0,Info->TabWidth - (Lean + 1) - Info->Thick,0);

									LTP_SetAPen(RPort,Pens[SHADOWPEN]);

									for(j = 0 ; j < Info->Thick ; j++)
										DrawCap(RPort,Info->TabWidth - Lean - j - Info->Thick,0,Lean,Info->TabHeight,FALSE);

									LTP_SetAPen(RPort,Pens[TEXTPEN]);
									Len = strlen(Labels[i]);

									LTP_PrintText(RPort,Labels[i],Len,(Info->TabWidth - TextLength(RPort,Labels[i],Len)) / 2,2);
								}

								Info->Mask = Mask.Planes[0];

								Info->Offset = Offset;

								Info->RPort.BitMap = Info->BitMap;

								CloseFont(Font);

								return((ULONG)gadget);
							}
						}
					}
				}

				CloseFont(Font);
			}
		}

		CoerceMethod(class,(Object *)gadget,OM_DISPOSE);
	}

	return(0);
}

STATIC ULONG
InputMethod(struct IClass *class,struct Gadget *gadget,struct gpInput *InputInfo)
{
	TabInfo	*Info	= INST_DATA(class,gadget);
	ULONG	 Result	= GMR_MEACTIVE;
	LONG	 x,y;
	BOOL	 Done	= FALSE;

	x = InputInfo->gpi_Mouse.X;
	y = InputInfo->gpi_Mouse.Y;

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

				if(x >= 0 && x < gadget->Width && y >= 0 && y < gadget->Height)
				{
					Result = GMR_NOREUSE | GMR_VERIFY;

					*InputInfo->gpi_Termination = Info->Current;
				}
				else
					Result = GMR_NOREUSE;
			}
		}
	}

	if(Result == GMR_NOREUSE)
	{
		struct RastPort	*RPort;

		if(RPort = ObtainGIRPort(InputInfo->gpi_GInfo))
		{
			Info->Current = Info->Initial;

			DoMethod((Object *)gadget,GM_RENDER,InputInfo->gpi_GInfo,RPort,GREDRAW_REDRAW);

			ReleaseGIRPort(RPort);
		}
	}

	if(!Done)
	{
		if(x >= 0 && x < gadget->Width && y >= 0 && y < gadget->Height)
		{
			LONG Index,i;

			for(Index = -1, i = Info->Count - 1 ; i >= 0 ; i--)
			{
				if(x >= Info->Tabs[i].Left && x < Info->Tabs[i].Left + Info->TabWidth)
					Index = i;
			}

			if(Index != Info->Current && Index != -1)
			{
				struct RastPort	*RPort;

				if(RPort = ObtainGIRPort(InputInfo->gpi_GInfo))
				{
					Info->Current = Index;

					DoMethod((Object *)gadget,GM_RENDER,InputInfo->gpi_GInfo,RPort,GREDRAW_UPDATE);

					ReleaseGIRPort(RPort);
				}
			}
		}
	}

	return(Result);
}

STATIC ULONG
ActiveMethod(struct IClass *class,struct Gadget *gadget,struct gpInput *InputInfo)
{
	TabInfo			*Info = INST_DATA(class,gadget);
	struct RastPort	*RPort;

	if(RPort = ObtainGIRPort(InputInfo->gpi_GInfo))
	{
		LONG Current,i;
		LONG x;

		x = InputInfo->gpi_Mouse.X;

		for(Current = -1, i = Info->Count - 1 ; i >= 0 ; i--)
		{
			if(x >= Info->Tabs[i].Left && x < Info->Tabs[i].Left + Info->TabWidth)
				Current = i;
		}

		Info->Initial = Info->Current;

		if(Current != Info->Current && Current != -1)
		{
			Info->Current = Current;

			DoMethod((Object *)gadget,GM_RENDER,InputInfo->gpi_GInfo,RPort,GREDRAW_UPDATE);
		}

		ReleaseGIRPort(RPort);

		return(GMR_MEACTIVE);
	}
	else
		return(GMR_NOREUSE);
}

STATIC ULONG
InactiveMethod(struct IClass *class,struct Gadget *gadget,struct gpGoInactive *InactiveInfo)
{
	TabInfo			*Info = INST_DATA(class,gadget);
	struct RastPort	*RPort;
	LONG			 Index;

	if(InactiveInfo->gpgi_Abort)
		Index = Info->Initial;
	else
		Index = Info->Current;

	if(RPort = ObtainGIRPort(InactiveInfo->gpgi_GInfo))
	{
		Info->Current = Index;

		DoMethod((Object *)gadget,GM_RENDER,InactiveInfo->gpgi_GInfo,RPort,GREDRAW_REDRAW);

		ReleaseGIRPort(RPort);
	}

	return(0);
}

BOOL
LTP_ObtainTabSize(struct IBox *Box,...)
{
	va_list			 VarArgs;
	struct TagItem	*TagList,*Tag;
	struct TextAttr	*FontAttr;
	struct DrawInfo	*DrawInfo;
	STRPTR			*Labels;
	LONG			 Count;
	LONG			 SizeType;
	BOOL			 Success;

	Success = FALSE;

	va_start(VarArgs,Box);

	TagList = (struct TagItem *)VarArgs;

	SizeType = GDOMAIN_NOMINAL;
	FontAttr = NULL;
	DrawInfo = NULL;
	Labels = NULL;

	while(Tag = NextTagItem(&TagList))
	{
		switch(Tag->ti_Tag)
		{
			case TIA_Labels:
				Labels = (STRPTR *)Tag->ti_Data;
				break;

			case TIA_Font:
				FontAttr = (struct TextAttr *)Tag->ti_Data;
				break;

			case TIA_DrawInfo:
				DrawInfo = (struct DrawInfo *)Tag->ti_Data;
				break;

			case TIA_SizeType:
				SizeType = Tag->ti_Data;
				break;
		}
	}

	Count = 0;

	if(Labels)
	{
		while(Labels[Count])
			Count++;
	}

	if(DrawInfo && FontAttr && Labels && Count)
	{
		struct TextFont *Font;

		if(Font = OpenFont(FontAttr))
		{
			struct RastPort	RastPort,*RPort = &RastPort;
			LONG			i,Len,MaxWidth;
			LONG			Lean,Thick;

			InitRastPort(RPort);

			SetFont(RPort,Font);

			Box->Left	= 0;
			Box->Top	= 0;
			Box->Height	= RPort->TxHeight + 11;

			Lean = (TextLength(RPort,"O",1) + 1) / 2;

			Thick = (DrawInfo->dri_Resolution.Y + DrawInfo->dri_Resolution.X - 1) / DrawInfo->dri_Resolution.X;

			if(Thick < 1)
				Thick = 1;

			for(i = MaxWidth = 0 ; i < Count ; i++)
			{
				if((Len = TextLength(RPort,Labels[i],strlen(Labels[i]))) > MaxWidth)
					MaxWidth = Len;
			}

			MaxWidth = 2 + (Lean + Thick - 1 + Thick + MaxWidth + Thick + Lean + Thick - 1) + 2;

			if(SizeType == GDOMAIN_MINIMUM && Count > 1)
				Box->Width = 2 * Thick + MaxWidth + (Count - 1) * (Thick * 4) + Thick * 3;
			else
				Box->Width = 2 * Thick + Count * (MaxWidth + Thick) + Thick * 2;

			CloseFont(Font);

			Success = TRUE;
		}
	}

	va_end(VarArgs);

	return(Success);
}

#ifndef __AROS__
ULONG SAVE_DS ASM
LTP_TabClassDispatcher(REG(a0) struct IClass *class,REG(a2) Object *object,REG(a1) Msg msg)
#else
AROS_UFH3(ULONG, LTP_TabClassDispatcher,
	  AROS_UFHA(struct IClass *, class, A0),
	  AROS_UFHA(Object *, object, A2),
	  AROS_UFHA(Msg, msg, A1)
)
#endif
{
        AROS_USERFUNC_INIT
	
	switch(msg->MethodID)
	{
		case OM_NEW:
			return(NewMethod(class,(struct Gadget *)object,(struct opSet *)msg));

		case OM_UPDATE:
		case OM_SET:
			SetMethod(class,(struct Gadget *)object,(struct opSet *)msg);
			break;

		case GM_RENDER:
			return(RenderMethod(class,(struct Gadget *)object,(struct gpRender *)msg));

		case GM_HITTEST:
			return(GMR_GADGETHIT);

		case GM_GOINACTIVE:
			return(InactiveMethod(class,(struct Gadget *)object,(struct gpGoInactive *)msg));

		case GM_HANDLEINPUT:
			return(InputMethod(class,(struct Gadget *)object,(struct gpInput *)msg));

		case GM_GOACTIVE:
			return(ActiveMethod(class,(struct Gadget *)object,(struct gpInput *)msg));

		case OM_DISPOSE:
			DisposeMethod(class,(struct Gadget *)object,msg);

			// Falls down through to the default case...
	}

	return(DoSuperMethodA(class,object,msg));
    
        AROS_USERFUNC_EXIT
}

#endif	/* DO_TAB_KIND */
