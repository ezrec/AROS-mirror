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

#include <stddef.h>

/*****************************************************************************/

#include "Assert.h"

/*****************************************************************************/

ObjectNode *
LTP_CreateObjectNode(LayoutHandle *handle,LONG type,ULONG id,STRPTR label)
{
	ObjectNode *node;
	ULONG size,extra;

	if(type < BUTTON_KIND || (type > TEXT_KIND && type < INCREMENTER_KIND) || type > IMAGE_KIND)
		return(NULL);

	switch(type)
	{
		#ifdef DO_GAUGE_KIND
		{
			case GAUGE_KIND:

				extra = sizeof(GaugeExtra);
				break;
		}
		#endif

		#ifdef DO_TAPEDECK_KIND
		{
			case TAPEDECK_KIND:

				extra = sizeof(TapeDeckExtra);
				break;
		}
		#endif	/* DO_TAPEDECK_KIND */

		#ifdef DO_LEVEL_KIND
		{
			case LEVEL_KIND:

				extra = sizeof(LevelExtra);
				break;
		}
		#endif	/* DO_LEVEL_KIND */

		case BUTTON_KIND:

			extra = sizeof(ButtonExtra);
			break;

		case XBAR_KIND:
		case YBAR_KIND:

			extra = sizeof(BarExtra);
			break;

		case BOX_KIND:

			extra = sizeof(BoxExtra);
			break;

		case FRAME_KIND:

			extra = sizeof(FrameExtra);
			break;

		case IMAGE_KIND:

			extra = sizeof(ImageExtra);
			break;

		case PICKER_KIND:

			extra = sizeof(PickerExtra);
			break;

		case INCREMENTER_KIND:

			extra = sizeof(IncrementerExtra);
			break;

		case LISTVIEW_KIND:

			extra = sizeof(ListExtra);
			break;

		case MX_KIND:

			extra = sizeof(RadioExtra);
			break;

		case TEXT_KIND:

			extra = sizeof(TextExtra);
			break;

		case NUMBER_KIND:

			extra = sizeof(NumberExtra);
			break;

		case CYCLE_KIND:

			extra = sizeof(CycleExtra);
			break;

		#ifdef DO_POPUP_KIND
		{
			case POPUP_KIND:

				extra = sizeof(PopupExtra);
				break;
		}
		#endif

		#ifdef DO_TAB_KIND
		{
			case TAB_KIND:

				extra = sizeof(TabExtra);
				break;
		}
		#endif

		case PALETTE_KIND:

			extra = sizeof(PalExtra);
			break;

		case SLIDER_KIND:

			extra = sizeof(SliderExtra);
			break;

		case SCROLLER_KIND:

			extra = sizeof(ScrollerExtra);
			break;

		case INTEGER_KIND:

			extra = sizeof(IntegerExtra);
			break;

		case PASSWORD_KIND:
		case STRING_KIND:
		case FRACTION_KIND:

			extra = sizeof(StringExtra);
			break;

		case HORIZONTAL_KIND:
		case VERTICAL_KIND:

			extra = sizeof(GroupExtra);
			break;

		#ifdef DO_BOOPSI_KIND
		{
			case BOOPSI_KIND:

				extra = sizeof(BOOPSIExtra);
				break;
		}
		#endif	/* DO_BOOPSI_KIND */

		default:

			extra = 0;
			break;
	}

	size = offsetof(ObjectNode,Special) + extra;

	if(node = LTP_Alloc(handle,size))
	{
		node->Type			= type;
		node->Label			= label;
		node->ID			= id;
		node->PointBack		= node;
		node->LayoutSpace	= 1;

		switch(type)
		{
			case LISTVIEW_KIND:

				node->LabelPlace				= PLACE_ABOVE;
				node->Lines						= 1;
				node->Current 					= ~0;
				node->Special.List.LinkID		= -1;
				node->Special.List.AutoPageID	= -1;

				break;

			case PASSWORD_KIND:
			case STRING_KIND:
			case FRACTION_KIND:

				node->Special.String.LayoutHandle 	= handle;
				node->Special.String.LinkID			= -1;
				node->Special.String.MaxChars		= 256;

				break;

			case INTEGER_KIND:

				node->Special.Integer.LayoutHandle	= handle;
				node->Min							= -2147483647;
				node->Max							= 2147483647;
				node->Special.Integer.MaxChars		= 11;

				break;

			#ifdef DO_BOOPSI_KIND
			{
				case BOOPSI_KIND:

					node->Special.BOOPSI.Link = NIL_LINK;
					break;
			}
			#endif	/* DO_BOOPSI_KIND */

			#ifdef DO_TAPEDECK_KIND
			{
				case TAPEDECK_KIND:

					node->LabelPlace = PLACE_BELOW;
					break;
			}
			#endif	/* DO_TAPEDECK_KIND */

			case MX_KIND:

				node->LabelPlace					= PLACE_RIGHT;
				node->Special.Radio.TitlePlace		= PLACETEXT_LEFT;
				node->Special.Radio.AutoPageID		= -1;

				break;

			#ifdef DO_POPUP_KIND
			{
				case POPUP_KIND:

					node->Special.Popup.AutoPageID = -1;
					break;
			}
			#endif

			#ifdef DO_TAB_KIND
			{
				case TAB_KIND:

					node->Special.Tab.AutoPageID = -1;
					break;
			}
			#endif

			case CYCLE_KIND:

				node->Special.Cycle.AutoPageID = -1;
				break;

			case SCROLLER_KIND:

				node->Special.Scroller.Visible		= 2;
				node->Special.Scroller.RelVerify	= TRUE;
				node->Special.Scroller.Immediate	= TRUE;
				break;

			case SLIDER_KIND:

				node->Special.Slider.LevelPlace		= PLACETEXT_LEFT;
				node->Special.Slider.LevelFormat	= LTP_NumberFormat;	/* either %ld or %lD, according to locale */

				break;

			case FRAME_KIND:

				node->Special.Frame.DrawBox			= TRUE;
				node->Special.Frame.Dummy.GadgetID	= id;
				break;

			case BOX_KIND:

				node->Special.Box.DrawBox	= TRUE;
				node->Special.Box.TextPen	= -1;
				node->Special.Box.BackPen	= -1;
				break;

			case TEXT_KIND:

				node->Special.Text.FrontPen = node->Special.Text.BackPen = -1;
				break;

			case NUMBER_KIND:

				node->Special.Number.FrontPen = node->Special.Number.BackPen = -1;
				node->Special.Number.MaxNumberLen = 10;
				break;

			case HORIZONTAL_KIND:

				node->Special.Group.Horizontal = TRUE;

				/* Fall through to... */

			case VERTICAL_KIND:

				node->Type			= GROUP_KIND;
				node->LabelPlace	= PLACE_IN;

				break;

			#ifdef DO_LEVEL_KIND
			{
				case LEVEL_KIND:

					node->Special.Level.LevelPlace	= PLACETEXT_LEFT;
					node->LabelPlace				= PLACE_LEFT;
					node->Special.Level.LevelFormat	= LTP_NumberFormat;	/* either %ld or %lD, according to locale */
					node->Special.Level.Freedom		= FREEHORIZ;
					node->Lines						= 2;

					break;
			}
			#endif	/* DO_LEVEL_KIND */

			#ifdef DO_GAUGE_KIND
			{
				case GAUGE_KIND:

					node->Special.Gauge.LastPercentage = -1;
					break;
			}
			#endif	/* DO_GAUGE_KIND */
		}
	}

	return(node);
}
