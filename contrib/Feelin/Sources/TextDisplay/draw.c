#include "Private.h"

struct in_TD_Compagnion
{
	struct RastPort                 *rp;
	uint16                           x;
	uint16                           y;
	FRect                           *rect;
	uint32                          *pens;
	struct TextFont                 *default_font;

	struct TextFont                 *backup_font;
	uint8                            backup_mode;
	uint8                            backup_apen;
	uint8                            backup_bpen;

	int8                             lead_done;

	struct TextFont                 *font;

	/**/

	FTDAdjust                        adjust;

	/***/

	FTDLine                         *line;
	FTDChunkText                    *chunk;                  // used to obtain last context when drawing dots.
	
	uint16                           rem_chars;
	uint16                           rem_image;
	uint16                           rem_space;              // should only be used once per line. once used, set it to 0;
};

typedef struct in_TD_Compagnion                 FTDCompagnion;

///td_text_draw

/*

	Substract the number of characters draw to  Compagnion.rem_chars  (don't
	care  if  dots  are  drawn or not, as they are the very last part of the
	line.

	Compagnion.x sera est mis à jour avec la valeur de "rp -> cp_x"

*/

void td_text_draw(STRPTR text,uint32 chars,FTDCompagnion *comp)
{
	if ((text != NULL) && chars)
	{
		struct RastPort *rp = comp -> rp;
		FTDChunkText *chunk = comp -> chunk;

		uint16 x = comp -> x;
		uint16 y = comp -> y + comp -> line -> baseline;

//      F_Log(0,"draw with 0x%08lx '%s/%ld'",font,font -> tf_Message.mn_Node.ln_Name,font -> tf_YSize);

		SetFont(rp,comp -> font);
		SetSoftStyle(rp,chunk -> style,ALL);//AskSoftStyle(rp));

		if ((chunk -> pens) != NULL)
		{
			if ((chunk -> pens -> shadow) != NULL)
			{
				_APen(chunk -> pens -> shadow -> Pen);
				_Move(x+1,y+1);
				_Text(text,chars);
			}

			if ((chunk -> pens -> light) != NULL)
			{
				_APen(chunk -> pens -> light -> Pen);
				_Move(x-1,y-1);
				_Text(text,chars);
			}
		}
	
		if ((chunk -> font != NULL) && (chunk -> font -> color != NULL))
		{
			_APen(chunk -> font -> color -> Pen);
		}
		else if ((chunk -> pens != NULL) && (chunk -> pens -> text != NULL))
		{
			_APen(chunk -> pens -> text -> Pen);
		}
		else
		{
			_APen(comp -> pens[FV_Pen_Text]);
		}
 
		_Move(x,y);
		_Text(text,chars);

		comp -> x = rp -> cp_x;

		if ((chunk -> pens) != NULL)
		{
			if ((chunk -> pens -> down) != NULL)
			{
				_APen(chunk -> pens -> down -> Pen);
				_Move(x+1,y+1);
				_Text(text,chars);
			}

			if ((chunk -> pens -> up) != NULL)
			{
				_APen(chunk -> pens -> up -> Pen);
				_Move(x-1,y-1);
				_Text(text,chars);
			}
		}
	}
}
//+
///td_compagnion_new
FTDCompagnion * td_compagnion_new(FClass *Class,FObject Obj,struct FS_TextDisplay_Draw *Msg)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);
	FTDCompagnion *compagnion;
	STRPTR er = NULL;

	if ((compagnion = F_New(sizeof (FTDCompagnion))) != NULL)
	{
		if (LOD -> Render != NULL)
		{
			FTDColor *color;
 
			compagnion -> pens = LOD -> Render -> Palette -> Pens;

			for (color = (FTDColor *)(LOD -> ColorsList.Head) ; color ; color = color -> Next)
			{
				if (color -> Type == FV_TD_COLOR_PEN)
				{
					color -> Pen = LOD -> Render -> Palette -> Pens[(ULONG)(color -> Spec)];
				}
			}

			if ((Msg -> Rect) != NULL)
			{
				if (Msg -> Rect -> x1 < Msg -> Rect -> x2 &&
					 Msg -> Rect -> y1 < Msg -> Rect -> y2)
				{
					compagnion -> rect = Msg -> Rect;
					compagnion -> x    = Msg -> Rect -> x1;
					compagnion -> y    = Msg -> Rect -> y1;
					
					if (FF_TD_EXTRA_TOPLEFT & LOD -> Flags)
					{
						compagnion -> x++;
						compagnion -> y++;
					}

					compagnion -> adjust.limit_w = Msg -> Rect -> x2 - Msg -> Rect -> x1 + 1;

					if ((compagnion -> rp = (struct RastPort *) F_Get(LOD -> Render,FA_Render_RPort)) != NULL)
					{
						compagnion -> backup_font = compagnion -> rp -> Font;
						compagnion -> backup_mode = GetDrMd(compagnion -> rp);
						compagnion -> backup_apen = GetAPen(compagnion -> rp);
						compagnion -> backup_bpen = GetBPen(compagnion -> rp);

						if ((compagnion -> default_font = LOD -> Font) != NULL)
						{
							compagnion -> font = compagnion -> default_font;
							compagnion -> adjust.font = compagnion -> default_font; 

							return compagnion;
						}
						else er = "TextFont is NULL";

					} else er = "RastPort is NULL";
				}
			}
			else er = "FRect is NULL";
		}
		else er = "FRender is NULL";
	}
	else er = "Unable to create FTDCompagnion";

	F_Dispose(compagnion);

	if (er)
	{
		F_Log(FV_LOG_DEV,er);
	}

	return NULL;
}
//+
///td_compagnion_dispose
void td_compagnion_dispose(FTDCompagnion *Compagnion)
{
	if (Compagnion != NULL)
	{
		if (Compagnion -> rp != NULL)
		{
			SetFont(Compagnion -> rp,Compagnion -> backup_font);
			SetABPenDrMd(Compagnion -> rp,Compagnion -> backup_apen,Compagnion -> backup_bpen,Compagnion -> backup_mode);
		}

		F_Dispose(Compagnion);
	}
}
//+

///TD_Draw
F_METHODM(uint32,TD_Draw,FS_TextDisplay_Draw)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);

	FTDLine *line;
	FTDChunk *chunk;
	FTDCompagnion *compagnion;

/*
	ULONG db_color = 2;
*/
	if ((compagnion = td_compagnion_new(Class,Obj,Msg)) != NULL)
	{
		struct RastPort *rp = compagnion -> rp;

		_DrMd(JAM1); 

		/* line by line, chunk by chunk */

		for (line = (FTDLine *) LOD -> LineList.Head ; line ; line = line -> Next)
		{
			compagnion -> line = line; 
 
/*
			_APen(30);
			_Move(compagnion -> rect -> x1,compagnion -> y + compagnion -> line -> baseline);
			_Draw(compagnion -> rect -> x2,compagnion -> y + compagnion -> line -> baseline);
*/
/*
			_APen(db_color++);
			_Boxf(compagnion -> rect -> x1,compagnion -> y,
					compagnion -> rect -> x2,compagnion -> y + line -> Height - 1);
*/
			if (LOD -> limit_w != compagnion -> adjust.limit_w)
			{
				compagnion -> adjust.font = compagnion -> default_font;

				td_line_adjust(line,&compagnion -> adjust); 
			}

			/* if 'adjust_width' is equal to zero there is nothing to draw, or
			nothing can be drawn, not even dots */

			if (line -> adjust_width == 0 && !line -> hr)
			{
				compagnion -> y += line -> Height + line -> spacing; continue;
			}

			compagnion -> lead_done = FALSE;
			compagnion -> rem_chars = line -> adjust_chars;
			compagnion -> rem_image = line -> adjust_image;
			compagnion -> rem_space = line -> space_loss;

			switch (line -> align)
			{
				case FV_TD_LINE_ALIGN_RIGHT:  compagnion -> x = compagnion -> rect -> x2 - line -> adjust_width + 1; break;
				case FV_TD_LINE_ALIGN_CENTER: compagnion -> x = (compagnion -> adjust.limit_w - line -> adjust_width) / 2 + compagnion -> rect -> x1; break;
				default:                      compagnion -> x = compagnion -> rect -> x1;
			}

			for (chunk = (FTDChunk *) line -> ChunkList.Head ; chunk ; chunk = chunk -> Next)
			{
				switch (chunk -> Type)
				{
///FV_TD_CHUNK_TEXT
					case FV_TD_CHUNK_TEXT:
					{
						compagnion -> chunk = (FTDChunkText *)(chunk); 

						if (_td_chunk_text_font && _td_chunk_text_font -> setup_font)
						{
							compagnion -> font = _td_chunk_text_font -> setup_font;
						}
						else
						{
							compagnion -> font = compagnion -> default_font;
						}

						if (!compagnion -> rem_chars) break;

						if (FF_TD_CHUNK_TEXT_SHORTCUT & chunk -> Flags)
						{
							UWORD short_w = td_text_extent(LOD -> Font,((FTDChunkText *)(chunk)) -> text,1,&compagnion -> adjust.te); 
 
							_APen(compagnion -> pens[FV_Pen_Highlight]);
							_Move(compagnion -> x,compagnion -> y + compagnion -> adjust.font -> tf_Baseline + 2);
							_Draw(compagnion -> x + short_w - 1,compagnion -> y + compagnion -> adjust.font -> tf_Baseline + 2);
						}

						if (line -> align == FV_TD_LINE_ALIGN_JUSTIFY)
						{
							ULONG len = MIN(compagnion -> rem_chars,_td_chunk_text_chars);

							STRPTR str = _td_chunk_text_text;
							STRPTR bgn = str;
							STRPTR end = str + len;

							if (compagnion -> lead_done == FALSE)
							{
								UWORD std_space_w = td_text_extent(compagnion -> font," ",1,&compagnion -> adjust.te);

								while (*str == ' ') str++;

//                                 FPrintf(FeelinBase -> Console,"ADD #1 (%ld)\n",(str - bgn) * comp.space_w);

								compagnion -> x += (str - bgn) * std_space_w;
								compagnion -> rem_chars -= (str - bgn);

								compagnion -> lead_done = TRUE;
							}

//                              FPrintf(FeelinBase -> Console,"GO >>>\n");

							while (str < end)
							{
								if (*str == ' ')
								{
									str++;

									compagnion -> rem_chars--;
									compagnion -> x += line -> space_width + compagnion -> rem_space; compagnion -> rem_space = 0;
								}
								else
								{
									bgn = str;

									while (*str != ' ' && str < end) str++;

									compagnion -> rem_chars -= (str - bgn);

									td_text_draw(bgn,str - bgn,compagnion);
								}
							}
						}
						else
						{
							if (_td_chunk_text_text && compagnion -> rem_chars)
							{
								UWORD chars = MIN(compagnion -> rem_chars,_td_chunk_text_chars);

//                        compagnion -> text = ((FTDChunkText *)(chunk)) -> text;
//                        compagnion -> chars = MIN(compagnion -> rem_chars,_td_chunk_text_chars);
								compagnion -> rem_chars -= chars;

								td_text_draw(_td_chunk_text_text,chars,compagnion);
							}
						}
					}
					break;
//+
///FV_TD_CHUNK_IMAGE
					case FV_TD_CHUNK_IMAGE:
					{
						if (compagnion -> rem_image)
						{
							FRect r;

//                              F_Log(0,"imageW %ld - remainW %ld - dots %s",chunk -> Width,comp.rem_image,(comp.draw_dots) ? "Yes" : "No");

							r.x1 = compagnion -> x; r.x2 = r.x1 + MIN(compagnion -> rem_image,chunk -> Width - 1);
							r.y1 = compagnion -> y; r.y2 = r.y1 + chunk -> Height - 1;

							compagnion -> rem_image = r.x2 - r.x1 + 1;
							compagnion -> x = r.x2 + 1;

							F_Do(_td_chunk_image_object,FM_ImageDisplay_Draw,LOD -> Render,&r,0);
						}
					}
					break;
//+
				}

				if (compagnion -> rem_chars == 0 &&
					 compagnion -> rem_image == 0)
				{
					break;
				}
			}

			if ((line -> Width > compagnion -> adjust.limit_w) && compagnion -> chunk)
			{
				td_text_draw("...",3,compagnion);
			}

			/* draw horizontal rule */

			if (line -> hr != NULL)
			{
				WORD  x;
				UWORD w; 
				WORD  y = compagnion -> y + line -> Height - line -> hr -> size - line -> hr -> spacing + line -> spacing / 2;

				if (line -> hr -> percent)
				{
					w = (compagnion -> rect -> x2 - compagnion -> rect -> x1 + 1) * line -> hr -> width / 100;
				}
				else
				{
					w = line -> hr -> width;
				}

				w = MIN(compagnion -> adjust.limit_w,w);

				switch (line -> hr -> align)
				{
					case FV_TD_HR_CENTER:   x = ((compagnion -> rect -> x2 - compagnion -> rect -> x1 + 1) - w) / 2 + compagnion -> rect -> x1; break;
					case FV_TD_HR_RIGHT:    x = compagnion -> rect -> x2 - w + 1; break;
					default:                x = compagnion -> rect -> x1; break;
				}

				if (line -> hr -> noshade)
				{
					_APen(compagnion -> pens[line -> hr -> shine]);
					_Boxf(x,y,x + w - 1,y + line -> hr -> size - 1);
				}
				else
				{
					_APen(compagnion -> pens[line -> hr -> shadow]);
					_Move(x,y + line -> hr -> size - 2); _Draw(x,y); _Draw(x + w - 1,y);

					_APen(compagnion -> pens[line -> hr -> shine]);
					_Move(x,y + line -> hr -> size - 1); _Draw(x + w - 1,y + line -> hr -> size - 1); _Draw(x + w - 1,y+1);
				}
			}

			compagnion -> y += line -> Height + line -> spacing;
		}

		/* Drawing done. I Save limit values, so I won't have to adjust lines
		if the same values are used the next draw */

		LOD -> limit_w = compagnion -> adjust.limit_w;
		LOD -> limit_h = compagnion -> adjust.limit_h;

		td_compagnion_dispose(compagnion);
	}

	return LOD -> Width;
}
//+

