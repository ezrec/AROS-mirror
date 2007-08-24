#include "Private.h"

#ifdef __AROS__
#define chip
#endif

STATIC chip uint16 __pointer[] =
{
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 
}; 

/***************************************************************************/
/*** Private ***************************************************************/ 
/***************************************************************************/
 
///PH_Open 
BOOL PH_Open(FClass *Class,FObject Obj)
{ 
	struct LocalObjectData *LOD = F_LOD(Class,Obj); 
 
	if (!LOD -> Text   ||
		 !LOD -> Window || LOD -> Bubble) return FALSE;

	/* FBubble contains all related graphic structure. FBubble is used  only
	once  at  a  time, when bubble pops up. FBubble is created on the fly to
	keep LocalObjectData as small as possible. */

	LOD -> Bubble = F_New(sizeof (FBubble));
  
	if (LOD -> Bubble)
	{ 
		FObject app = (FObject) F_Get(LOD -> Window,FA_Parent);
		FObject dsp = (FObject) F_Get(app,FA_Application_Display);
		STRPTR scheme_spec;

		InitRastPort(&LOD -> Bubble -> RPort);

		/* This FC_Render object is used by the FC_TextDisplay object to draw
		text of the right rastport. */

		LOD -> Bubble -> Render = RenderObject,
			
			FA_Render_Display,      dsp,
			FA_Render_Application,  app,
			FA_Render_Window,       LOD -> Window,
			FA_Render_RPort,        &LOD -> Bubble -> RPort,
			FA_Render_Palette,      LOD -> Bubble -> Scheme,
			
			End;
			
		if (LOD -> Bubble -> Render)
		{

			/* Resolve preferences */

			LOD -> Bubble -> Font = (APTR) F_Do(app,FM_Application_OpenFont,Obj,LOD -> p_Font);
			scheme_spec = (STRPTR) F_Do(app,FM_Application_Resolve,LOD -> p_Scheme,NULL);
			LOD -> Bubble -> Scheme = (FPalette *) F_Do(dsp,FM_CreateColorScheme,scheme_spec,F_Get(LOD -> Window,FA_ColorScheme));

			if (LOD -> Bubble -> Scheme)
			{
				LOD -> Bubble -> Render -> Palette = LOD -> Bubble -> Scheme;

				LOD -> Bubble -> TD = TextDisplayObject,

					FA_TextDisplay_Font,     LOD -> Bubble -> Font,
					FA_TextDisplay_Contents, LOD -> Text,
					FA_TextDisplay_Shortcut, FALSE,

					End;

				if (F_Do(LOD -> Bubble -> TD,FM_TextDisplay_Setup,LOD -> Bubble -> Render))
				{
					struct Screen *scr  = (APTR) F_Get(dsp,(ULONG) "FA_Display_Screen");

					WORD           mse_x = scr -> MouseX,
										mse_y = scr -> MouseY;

					WORD           bbl_x,bbl_y;
					UWORD          bbl_w,bbl_h;

					LOD -> Bubble -> text_w = F_Get(LOD -> Bubble -> TD,FA_TextDisplay_Width);
					LOD -> Bubble -> text_h = F_Get(LOD -> Bubble -> TD,FA_TextDisplay_Height);

					bbl_w = FV_BUBBLE_DATA_W * 2 + LOD -> Bubble -> text_w + SHADOW;
					bbl_h = MAX(FV_BUBBLE_DATA_H * 2,HSPACE * 2 + LOD -> Bubble -> text_h) + SHADOW + FV_ARROW_H;

					if (bbl_w < scr -> Width && bbl_h < scr -> Height)
					{

						/* 'arw_l' is set to TRUE when the arrow is on  the  left
						edge of the bubble, 'arw_t' is set to TRUE when the arrow
						is on the top edge of the bubble. Thus, using 'arw_l' and
						'arw_t' I can pick the right arrow position (FV_ARROW_??)
						*/

						BOOL           arw_l;
						BOOL           arw_t;
						UBYTE          arw_p;

						/* first I search where the arrow can be put (top/bottom,
						left/right) */

						if (mse_x > scr -> Width / 2)
						{
							arw_l = FALSE; bbl_x = mse_x - bbl_w + FV_ARROW_OFF + FV_ARROW_W + 1;
						}
						else
						{
							arw_l = TRUE;  bbl_x = mse_x - FV_ARROW_OFF - FV_ARROW_W + 1;
						}

						/* Adding or subtracting  2  to  mouse  Y  coordinate  is
						useful  to  avoid  bubble  window to open under mouse hot
						spot. Thus, mouse button clicks  are  made  on  the  main
						window, not bubble window.

						[???] The problem is that bubble's shadow is part of  the
						window,  and  if  the  shadow  is  too big, the bubble is
						shifted too much and the arrow position is no correct */

						if (mse_y > scr -> Height / 2)
						{
							arw_t = FALSE; mse_y -= 2 ; bbl_y = mse_y - bbl_h + 1;
						}
						else
						{
							arw_t = TRUE;  mse_y += 2 ; bbl_y = mse_y;
						}

						/* now I can set arrow positon */

						if (arw_t)     arw_p = (arw_l) ? FV_ARROW_TL : FV_ARROW_TR;
						else           arw_p = (arw_l) ? FV_ARROW_BL : FV_ARROW_BR;

						/* Checking bubble coordinates. I have already checked if
						the  bubble  fits in the screen, now I check if using the
						arrow  spot  is  not  a  problem.  If  the  arrow   makes
						coordinates invalid it is removed. */

						if (bbl_x < 0)                      { bbl_x = 0;                         arw_p = FV_ARROW_NONE; }
						if (bbl_y < 0)                      { bbl_y = 0;                         arw_p = FV_ARROW_NONE; }
						if (bbl_x + bbl_w > scr -> Width)   { bbl_x = scr -> Width - bbl_w + 1;  arw_p = FV_ARROW_NONE; }
						if (bbl_y + bbl_h > scr -> Height)  { bbl_y = scr -> Height - bbl_h + 1; arw_p = FV_ARROW_NONE; }

						if (arw_p == FV_ARROW_NONE)
						{
							if (arw_t == FALSE)
							bbl_y += FV_ARROW_H;
							bbl_h -= FV_ARROW_H;
						}

						/* Now that we know bubble's dimensions we  can  allocate
						the  bubble's  bitmap.  This  bitmap  will be used by the
						bubble's window as super-bitmap. The  bitmap  pointer  is
						directly saved in bubble's rastport structure */
							
						LOD -> Bubble -> RPort.BitMap = AllocBitMap(bbl_w,bbl_h,scr -> RastPort.BitMap -> Depth,BMF_MINPLANES,scr -> RastPort.BitMap);
  
						if (LOD -> Bubble -> RPort.BitMap)
						{
							struct Window *win = (struct Window *) F_Get(LOD -> Bubble -> Render -> Window,FA_Window);

							PH_Draw(Class,Obj,scr,bbl_x,bbl_y,bbl_w,bbl_h,arw_p);
							
							LOD -> Bubble -> Win = OpenWindowTags(NULL,
								
								WA_Left,          bbl_x,
								WA_Top,           bbl_y,
								WA_Width,         bbl_w,
								WA_Height,        bbl_h,
								WA_Borderless,    TRUE,
								WA_PubScreen,     scr,
    	    	    	    	    	    	    	    #ifndef __AROS__								
								WA_SuperBitMap,   LOD -> Bubble -> RPort.BitMap,
							    #endif
								WA_AutoAdjust,    TRUE,
								
								TAG_DONE);

							if (LOD -> Bubble -> Win)
							{
							#ifdef __AROS__
							    	BltBitMapRastPort(LOD -> Bubble -> RPort.BitMap, 0, 0,
								    	    	  LOD -> Bubble -> Win -> RPort, 0, 0,
										  bbl_w, bbl_h, 192);
							#endif
								LOD -> Bubble -> Pointer = win -> Pointer;
								SetPointer(win,__pointer,4,16,0,0);

								return TRUE;
							}
						}
					}
				}
			}
		}
	}

	F_Log(FV_LOG_USER,"Unable to create / open window");
 
	PH_Close(Obj,LOD); 
	 
	return NULL; 
} 
//+ 
///PH_Close 
void PH_Close(struct FeelinClass *Class,APTR Obj)
{ 
	struct LocalObjectData *LOD = F_LOD(Class,Obj); 
 
	if (LOD -> Bubble)
	{ 
		if (LOD -> Bubble -> Render)
		{
			if (LOD -> Bubble -> Scheme)
			{
				if (LOD -> Bubble -> TD)
				{
					if (LOD -> Bubble -> RPort.BitMap)
					{
						if (LOD -> Bubble -> Win)
						{
							struct Window *win = (APTR) F_Get(LOD -> Bubble -> Render -> Window,FA_Window);

							CloseWindow(LOD -> Bubble -> Win);

							SetWindowPointer(win,WA_Pointer,LOD -> Bubble -> Pointer,TAG_DONE);
						}

						FreeBitMap(LOD -> Bubble -> RPort.BitMap); LOD -> Bubble -> RPort.BitMap = NULL;
					}

					F_Do(LOD -> Bubble -> TD,FM_TextDisplay_Cleanup);
					F_DisposeObj(LOD -> Bubble -> TD); LOD -> Bubble -> TD = NULL;
				}

				F_Do(LOD -> Bubble -> Render -> Display,FM_RemPalette,LOD -> Bubble -> Scheme); LOD -> Bubble -> Scheme = NULL;
			}
			F_DisposeObj(LOD -> Bubble -> Render); LOD -> Bubble -> Render = NULL;
		}

		if (LOD -> Bubble -> Font)
		{
			CloseFont(LOD -> Bubble -> Font); LOD -> Bubble -> Font = NULL;
		}
		
	#ifdef __AROS__
		DeinitRastPort(&LOD -> Bubble -> RPort);
	#endif	
		F_Dispose(LOD -> Bubble); LOD -> Bubble = NULL;
	} 
} 
//+ 
