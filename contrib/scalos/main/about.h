// about.h
// $Date$
// $Revision$

#ifndef ABOUT_H
#define ABOUT_H

#include <exec/types.h>
#include <exec/ports.h>

#include <graphics/gfx.h>
#include <intuition/intuition.h>

struct AboutBitMap
	{
	struct RastPort abm_RastPort;
	struct Layer_Info *abm_LayerInfo;
	struct Layer *abm_Layer;
	struct BitMap *abm_BitMap;
	};

struct AboutGadgetDef
	{
	CONST_STRPTR agd_ImageNameNormal;
	CONST_STRPTR agd_ImageNameSelected;
	CONST_STRPTR agd_ImageNameDisabled;
	LONG agd_GadgetTextID;
	LONG agd_GadgetID;
	LONG agd_FallbackSysImage;
	};

struct AboutGadgetInfo
	{
	struct Node agi_Node;
	Object *agi_Image;
	struct Gadget *agi_Gadget;
	ULONG agi_Width;
	ULONG agi_Height;
	BOOL agi_Themed;
	};

struct	AboutInfo
{
	WORD			abi_wInnerleft;	        /* some dimension info about the about window */
	WORD			abi_wInnerright;
	WORD			abi_wInnertop;
	WORD			abi_wInnerbottom;
	WORD			abi_wInnerwidth;
	WORD			abi_wInnerheight;
	WORD			abi_XOffset;
	struct Window		*abi_Window;        	/* the actual about window */
	struct AboutBitMap	abi_Bitmap;	    	/* this structure holds all information for the bitmap that the scrolling gets done on */
	struct Rectangle 	abi_lScrollsize;
	struct ttDef		*abi_ttd;
	struct DatatypesImage 	*abi_Logo;	  	// Scalos Logo for about window
	struct DatatypesImage 	*abi_Background;  	// Background for about window scrolling area
	struct DatatypesImage	*abi_BackfillImage;	// optional Datatypes image used to backfill the About window
	ULONG			abi_LineCount;		// +jl+ 20010512 continouous line counter
	BOOL			abi_Initial;		// flag: fill about rectangle on initial call
	BOOL			abi_DoScroll;		// flag: scroll continuously

	LONG 			abi_bmOffset;
	LONG 			abi_lCurrentline;	/* The current line counter */

	CONST_STRPTR		abi_ttDesc;

	struct TextFont		*abi_Font;		// Font used for rendering

	struct List		abi_GadgetList;
};


/* The speed that the about information scrolls at */
#define SCROLLSPEED	1

#endif	/* ABOUT_H */

