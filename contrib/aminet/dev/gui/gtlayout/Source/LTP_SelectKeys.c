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

#include "Assert.h"

#ifdef DO_PICKSHORTCUTS	/* Support code */

VOID
LTP_SelectKeys(LayoutHandle *handle,ObjectNode *group)
{
	LONG			bunch;
	ObjectNode *	node;
	ULONG			page;
	STRPTR *		lines;

	SCANPAGE(group,node,page)
	{
		bunch = 0;
		lines = NULL;

		switch(node->Type)
		{
			case GROUP_KIND:

				LTP_SelectKeys(handle,node);
				break;

			case TEXT_KIND:

				if(!node->Special.Text.UsePicker)
					break;

			case PASSWORD_KIND:
			case STRING_KIND:
			case FRACTION_KIND:

				if(node->Type != TEXT_KIND && node->Special.String.UsePicker)
					bunch = 255;

			case BUTTON_KIND:

				if(node->Type == BUTTON_KIND)
				{
					if(!node->NoKey && node->Special.Button.Lines)
						lines = node->Special.Button.Lines;
				}

			case BOOPSI_KIND:
			case CHECKBOX_KIND:
			case INTEGER_KIND:

				bunch++;

#ifdef DO_POPUP_KIND
			case POPUP_KIND:
#endif	/* DO_POPUP_KIND */
#ifdef DO_TAB_KIND
			case TAB_KIND:
#endif	/* DO_TAB_KIND */
#ifdef DO_LEVEL_KIND
			case LEVEL_KIND:
#endif	/* DO_LEVEL_KIND */
#ifdef DO_TAPEDECK_KIND
			case TAPEDECK_KIND:
#endif	/* DO_TAPEDECK_KIND */

			case LISTVIEW_KIND:
			case MX_KIND:
			case CYCLE_KIND:
			case PALETTE_KIND:
			case SCROLLER_KIND:
			case SLIDER_KIND:

				bunch++;

				if(node->Label && !node->Key && !node->NoKey)
				{
					LONG	i,len,code,glyph;
					STRPTR	label;

					do
					{
						if(lines)
						{
							if(!(label = *lines++))
								break;
						}
						else
							label = node->Label;

						len = strlen(label);

						for(i = 0 ; i < len ; i++)
						{
							glyph	= ToLower(label[i]);
							code	= -1;

							switch(bunch)
							{
								case 1:	// Support increment & decrement

									if(LTP_Keys[0][glyph] && LTP_Keys[1][glyph])
										code = LTP_Keys[0][glyph];

									break;

										// Any one will do

								case 2:

									if(LTP_Keys[0][glyph])
										code = LTP_Keys[0][glyph];
									else
									{
										if(LTP_Keys[1][glyph] && LTP_Keys[0][glyph])
										{
											if(ToLower(LTP_Keys[1][glyph]) == glyph)
												code = LTP_Keys[0][glyph];
										}
									}

									break;
							}

							if(code != -1 && !handle->Keys[code])
							{
								if(lines)
								{
									node->Special.Button.KeyStroke = &label[i];

									lines = NULL;
								}
								else
								{
									STRPTR newLabel;

									if(newLabel = LTP_Alloc(handle,len + 2))
									{
										node->Label = newLabel;

										if(i)
											CopyMem(label,newLabel,i);

										newLabel[i] = '_';

										strcpy(&newLabel[i + 1],label + i);
									}
								}

								node->Key = code;

								handle->Keys[code] = handle->Keys[LTP_Keys[1][code]] = TRUE;

								break;
							}
						}
					}
					while(lines);
				}

				break;
		}
	}
}

#endif	/* DO_PICKSHORTCUTS */
