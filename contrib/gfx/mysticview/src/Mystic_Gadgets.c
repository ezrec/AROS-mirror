/*********************************************************************
----------------------------------------------------------------------

	fake gadgets

----------------------------------------------------------------------
*********************************************************************/

#ifdef __MORPHOS__
#include <public/proto/libamiga/amiga_protos.h>
#else
#include <clib/alib_protos.h>
#endif

#include <clib/macros.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include <proto/utility.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/guigfx.h>

#include "Mystic_Global.h"
#include "Mystic_Gadgets.h"


/*--------------------------------------------------------------------

	DeleteFakeGadgetList(fgl)

--------------------------------------------------------------------*/

void DeleteFakeGadgetList(struct FakeGadgetList *fgl)
{
	if (fgl)
	{
		struct FakeGadget *fg;

		while (fg = (struct FakeGadget *) RemHead(&fgl->list))
		{
			fg->inserted = FALSE;
			DeleteFakeGadget(fg);
		}

		Free(fgl);
	}
}


/*--------------------------------------------------------------------

	fgl = CreateFakeGadgetList(win, tags)

--------------------------------------------------------------------*/

struct FakeGadgetList * STDARGS CreateFakeGadgetList(struct Window *win, ...)
{
//	va_list va;
//	struct TagItem *taglist;
	struct FakeGadgetList *fgl;

/*
	va_start(va, win);
#ifdef __MORPHOS__
	taglist = (struct TagItem *) va->overflow_arg_area;
#else
	taglist = (struct TagItem *) va;
#endif
*/

	if (fgl = Malloclear(sizeof(struct FakeGadgetList)))
	{
		NewList(&fgl->list);
		fgl->numgadgets = 0;
		fgl->width = -1;
		fgl->height = -1;
		fgl->window = win;
		fgl->rp = win->RPort;
	}

	return fgl;
}


/*--------------------------------------------------------------------

	RemFakeGadget(fg)

--------------------------------------------------------------------*/

void RemFakeGadget(struct FakeGadget *fg)
{
	if (fg)
	{
		if (fg->inserted)
		{
			Remove(&fg->node);
			fg->inserted = FALSE;
		}
	}
}


/*--------------------------------------------------------------------

	DeleteFakeGadget(fg)

--------------------------------------------------------------------*/

void DeleteFakeGadget(struct FakeGadget *fg)
{
	if (fg)
	{
		RemFakeGadget(fg);
		Free(fg);
	}
}


/*--------------------------------------------------------------------

	fg = CreateFakeGadget(id, type, tags)

--------------------------------------------------------------------*/

struct FakeGadget * STDARGS CreateFakeGadget(ULONG id, ULONG type, ...)
{
	va_list va;
	struct TagItem *taglist;
	struct FakeGadget *fg;

	va_start(va, type);
#ifdef __MORPHOS__
	taglist = (struct TagItem *) va->overflow_arg_area;
#else
	taglist = (struct TagItem *) va;
#endif

	if (fg = Malloclear(sizeof(struct FakeGadget)))
	{
		fg->type = type;
		fg->id = id;
		fg->inserted = FALSE;
		fg->togglestatus = FALSE;
		fg->status = FGSTATUS_INACTIVE;

		fg->hborder = GetTagData(FGDT_HBORDER, 0, taglist);
		fg->vborder = GetTagData(FGDT_VBORDER, 0, taglist);
		fg->width = GetTagData(FGDT_WIDTH, 2, taglist);
		fg->height = GetTagData(FGDT_HEIGHT, 2, taglist);

		fg->pen_inactive = GetTagData(FGDT_INACTIVEPEN, -1, taglist);
		fg->pen_pressed = GetTagData(FGDT_PRESSEDPEN, -1, taglist);
		fg->pen_rollover = GetTagData(FGDT_ROLLOVERPEN, -1, taglist);
		fg->pen_active = GetTagData(FGDT_ACTIVEPEN, -1, taglist);
		fg->pen_activerollover = GetTagData(FGDT_ACTIVEROLLOVERPEN, -1, taglist);
		fg->pen_activepressed = GetTagData(FGDT_ACTIVEPRESSEDPEN, -1, taglist);


	//	fg->bitmap = (struct BitMap *) GetTagData(FGDT_BITMAP, NULL, taglist);

		fg->picx_inactive = GetTagData(FGDT_PICX_INACTIVE, 0, taglist);
		fg->picx_rollover = GetTagData(FGDT_PICX_ROLLOVER, 0, taglist);
		fg->picx_pressed = GetTagData(FGDT_PICX_PRESSED, 0, taglist);
		fg->picx_active = GetTagData(FGDT_PICX_ACTIVE, 0, taglist);
		fg->picx_activerollover = GetTagData(FGDT_PICX_ACTIVEROLLOVER, 0, taglist);
		fg->picx_activepressed = GetTagData(FGDT_PICX_ACTIVEPRESSED, 0, taglist);

		fg->picy_inactive = GetTagData(FGDT_PICY_INACTIVE, 0, taglist);
		fg->picy_rollover = GetTagData(FGDT_PICY_ROLLOVER, 0, taglist);
		fg->picy_pressed = GetTagData(FGDT_PICY_PRESSED, 0, taglist);
		fg->picy_active = GetTagData(FGDT_PICY_ACTIVE, 0, taglist);
		fg->picy_activerollover = GetTagData(FGDT_PICY_ACTIVEROLLOVER, 0, taglist);
		fg->picy_activepressed = GetTagData(FGDT_PICY_ACTIVEPRESSED, 0, taglist);

	}

	return fg;
}


/*--------------------------------------------------------------------

	AddFakeGadget(fgl, fg)

--------------------------------------------------------------------*/

void AddFakeGadget(struct FakeGadgetList *fgl, struct FakeGadget *fg)
{
	if (fg && fgl)
	{
		AddTail(&fgl->list, &fg->node);
		fg->inserted = TRUE;
		fgl->numgadgets++;
	}
}


/*--------------------------------------------------------------------

	GetLayoutData(fgl, orientation, *minwidth, *minheight)

--------------------------------------------------------------------*/

static int FG_GetWidth(struct FakeGadget *fg)
{
	return (fg->width + 2 * fg->hborder);
}

static int FG_GetHeight(struct FakeGadget *fg)
{
	return (fg->height + 2 * fg->vborder);
}


void GetLayoutData(struct FakeGadgetList *fgl, int orientation, int *minwidth, int *minheight)
{
	struct FakeGadget *fgnode, *fgnextnode;
	int currentwidth, currentheight;
	int temp;

	currentwidth = 0;
	currentheight = 0;

	fgnode = (struct FakeGadget *) fgl->list.lh_Head;

	while (fgnextnode = (struct FakeGadget *) fgnode->node.ln_Succ)
	{
		switch (orientation)
		{
			case ORIENTATION_DOWN:
				temp = FG_GetWidth(fgnode);
				if (temp > currentwidth)
				{
					currentwidth = temp;
				}
				currentheight += FG_GetHeight(fgnode);
				break;

			case ORIENTATION_RIGHT:
				temp = FG_GetHeight(fgnode);
				if (temp > currentheight)
				{
					currentheight = temp;
				}
				currentwidth += FG_GetWidth(fgnode);
				break;

			default:
				break;
		}

		fgnode = fgnextnode;
	}

	if (minwidth) *minwidth = currentwidth;
	if (minheight) *minheight = currentheight;
}



/*--------------------------------------------------------------------

	success = LayoutFakeGadgetList(fgl, orientation, width, height, x, y)

--------------------------------------------------------------------*/

BOOL LayoutFakeGadgetList(struct FakeGadgetList *fgl, int orientation, int width, int height, int x, int y)
{
	BOOL success = FALSE;

	if (fgl)
	{
		struct FakeGadget *fgnode, *fgnextnode;
		int minwidth, minheight;

		fgl->x = x;
		fgl->y = y;

		GetLayoutData(fgl, orientation, &minwidth, &minheight);

//		if (minwidth <= width && minheight <= height)
		{
			int realheight = MAX(minheight, height);
			int realwidth = MAX(minwidth, width);

			float currentx, currenty;
			float xspace = 0, yspace = 0;
			int numgadgetsx, numgadgetsy;

			switch (orientation)
			{
				case ORIENTATION_DOWN:
					xspace = width;
					yspace = realheight - minheight;
					numgadgetsy = fgl->numgadgets;
					if (numgadgetsy > 1) yspace /= (float) (numgadgetsy - 1);
					break;
				case ORIENTATION_RIGHT:
					xspace = realwidth - minwidth;
					yspace = height;
					numgadgetsx = fgl->numgadgets;
					if (numgadgetsx > 1) xspace /= (float) (numgadgetsx - 1);
					break;
				default:
					break;
			}



			currentx = 0;
			currenty = 0;

			fgnode = (struct FakeGadget *) fgl->list.lh_Head;

			while (fgnextnode = (struct FakeGadget *) fgnode->node.ln_Succ)
			{
				switch (orientation)
				{
					case ORIENTATION_DOWN:
					{
						fgnode->x = (int)(currentx + (xspace - (float) FG_GetWidth(fgnode)) / 2.0);
						fgnode->y = (int) currenty;
						currenty += FG_GetHeight(fgnode) + (int) yspace;
						break;
					}

					case ORIENTATION_RIGHT:
						fgnode->x = (int) currentx;
						fgnode->y = (int)(currenty + (yspace - (float) FG_GetHeight(fgnode)) / 2.0);
						currentx += FG_GetWidth(fgnode) + (int) xspace;
						break;

					default:
						break;
				}

				fgnode = fgnextnode;
			}
			success = TRUE;
		}

	}


	return success;
}


/*--------------------------------------------------------------------

	DrawFakeGadget(win, fg, x, y, bm)

--------------------------------------------------------------------*/

void DrawFakeGadget(struct Window *win, struct FakeGadget *fg, int x, int y, struct BitMap *bitmap)
{
	struct RastPort *rp = win->RPort;

	if (rp && fg)
	{
		if (y + fg->y + fg->vborder + fg->height < win->Height - win->BorderBottom
			&&	x + fg->x + fg->hborder + fg->width < win->Width - win->BorderRight)
		{
			if (bitmap)
			{
				ULONG py = 0;
				ULONG px = 0;

				switch (fg->status)
				{
					case FGSTATUS_INACTIVE:
						px = fg->togglestatus ? fg->picx_active : fg->picx_inactive;
						py = fg->togglestatus ? fg->picy_active : fg->picy_inactive;
						break;
					case FGSTATUS_PRESSED:
						px = fg->togglestatus ? fg->picx_activepressed : fg->picx_pressed;
						py = fg->togglestatus ? fg->picy_activepressed : fg->picy_pressed;
						break;
					case FGSTATUS_ROLLOVER:
						px = fg->togglestatus ? fg->picx_activerollover : fg->picx_rollover;
						py = fg->togglestatus ? fg->picy_activerollover : fg->picy_rollover;
						break;
				}

				BltBitMapRastPort(bitmap, px, py, rp, x + fg->x + fg->hborder,
					y + fg->y + fg->vborder, fg->width, fg->height, 0xc0);

			}
			else
			{
				LONG pen = -1;

				switch (fg->status)
				{
					case FGSTATUS_INACTIVE:
						pen = fg->togglestatus ? fg->pen_active : fg->pen_inactive;
						break;
					case FGSTATUS_PRESSED:
						pen = fg->togglestatus ? fg->pen_activepressed : fg->pen_pressed;
						break;
					case FGSTATUS_ROLLOVER:
						pen = fg->togglestatus ? fg->pen_activerollover : fg->pen_rollover;
						break;
				}

				if (pen != -1)
				{
					SetAPen(rp, pen);

					RectFill(rp, x + fg->x + fg->hborder, y + fg->y + fg->vborder,
						x + fg->x + fg->width - 1, y + fg->y + fg->height - 1);
				}
			}

			fg->visible = TRUE;
		}
		else
		{
			fg->visible = FALSE;
		}
	}
}



/*--------------------------------------------------------------------

	DrawFakeGadgetList(fgl, bitmap)

--------------------------------------------------------------------*/

void DrawFakeGadgetList(struct FakeGadgetList *fgl, struct BitMap *bm)
{
	if (fgl)
	{
		struct FakeGadget *fgnode, *fgnextnode;
		fgnode = (struct FakeGadget *) fgl->list.lh_Head;

		while (fgnextnode = (struct FakeGadget *) fgnode->node.ln_Succ)
		{
			DrawFakeGadget(fgl->window, (struct FakeGadget *) fgnode, fgl->x, fgl->y, bm);
			fgnode = fgnextnode;
		}
	}
}



/*--------------------------------------------------------------------

	fg = CheckFakeGadget(fgl, imsg)

--------------------------------------------------------------------*/

struct FakeGadget *CheckFakeGadget(struct FakeGadgetList *fgl, struct IntuiMessage *imsg)
{
	int mx, my;
	struct FakeGadget *gadget = NULL;

	struct FakeGadget *fg, *fgnextnode;
	fg = (struct FakeGadget *) fgl->list.lh_Head;

	mx = imsg->MouseX - fgl->x - fg->hborder;
	my = imsg->MouseY - fgl->y - fg->vborder;

	while (fgnextnode = (struct FakeGadget *) fg->node.ln_Succ)
	{
		if (fg->visible
				&& mx >= fg->x
				&& mx < fg->x + fg->width
				&& my >= fg->y
				&& my < fg->y + fg->height)
		{
			gadget = fg;
			break;
		}

		fg = fgnextnode;
	}

	return gadget;
}


/*--------------------------------------------------------------------

	id = HandleGadgetListEvent(fgl, imsg, bm)

--------------------------------------------------------------------*/

ULONG HandleGadgetListEvent(struct FakeGadgetList *fgl, struct IntuiMessage *imsg, struct BitMap *bm)
{
	ULONG id = INVALID_FAKEGADGET;

	if (fgl && bm)
	{
		struct FakeGadget *gadget;

		gadget = CheckFakeGadget(fgl, imsg);

		if (imsg->Class == IDCMP_MOUSEMOVE)
		{
			if (gadget)
			{
				if (gadget == fgl->pressedgadget)
				{
					if (gadget->status != FGSTATUS_PRESSED)
					{
						gadget->status = FGSTATUS_PRESSED;
						DrawFakeGadget(fgl->window, gadget, fgl->x, fgl->y, bm);
					}
				}
				else
				{
					if (!fgl->pressedgadget)
					{
						switch (gadget->status)
						{
							case FGSTATUS_INACTIVE:

								if (fgl->rollovergadget)
								{
									fgl->rollovergadget->status = FGSTATUS_INACTIVE;
									DrawFakeGadget(fgl->window, fgl->rollovergadget, fgl->x, fgl->y, bm);
								}

								gadget->status = FGSTATUS_ROLLOVER;
								DrawFakeGadget(fgl->window, gadget, fgl->x, fgl->y, bm);
								fgl->rollovergadget = gadget;
								break;
							default:
								break;
						}
					}
				}
			}
			else
			{
				if (fgl->pressedgadget)
				{
					if (fgl->pressedgadget->status != FGSTATUS_INACTIVE)
					{
						fgl->pressedgadget->status = FGSTATUS_INACTIVE;
						DrawFakeGadget(fgl->window, fgl->pressedgadget, fgl->x, fgl->y, bm);
					}
				}

				if (fgl->rollovergadget)
				{
					fgl->rollovergadget->status = FGSTATUS_INACTIVE;
					DrawFakeGadget(fgl->window, fgl->rollovergadget, fgl->x, fgl->y, bm);
					fgl->rollovergadget = NULL;
				}
			}
		}

		if (imsg->Class == IDCMP_MOUSEBUTTONS)
		{
			if (imsg->Code == SELECTDOWN)
			{
				if (gadget)
				{
					switch (gadget->status)
					{
						case FGSTATUS_ROLLOVER:
						case FGSTATUS_INACTIVE:
							gadget->status = FGSTATUS_PRESSED;
							DrawFakeGadget(fgl->window, gadget, fgl->x, fgl->y, bm);
							fgl->pressedgadget = gadget;
							fgl->rollovergadget = NULL;
							break;
						default:
							break;
					}
				}
			}
			else if (imsg->Code == SELECTUP)
			{
				if (gadget)
				{
					if (gadget == fgl->pressedgadget)
					{
						if (gadget->type == FGADTYPE_TOGGLE)
						{
							gadget->togglestatus = !gadget->togglestatus;
						}
						id = gadget->id;
					}

					switch (gadget->status)
					{
						case FGSTATUS_PRESSED:
							gadget->status = FGSTATUS_ROLLOVER;
							DrawFakeGadget(fgl->window, gadget, fgl->x, fgl->y, bm);
							fgl->rollovergadget = gadget;
							break;
						default:
							break;
					}
				}
				if (fgl->pressedgadget)
				{
					fgl->pressedgadget->status = FGSTATUS_INACTIVE;
					DrawFakeGadget(fgl->window, fgl->pressedgadget, fgl->x, fgl->y, bm);
					fgl->pressedgadget = NULL;
				}
			}
		}
	}

	return id;
}
