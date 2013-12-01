#ifndef SCALOS_GADGETBAR_H
#define SCALOS_GADGETBAR_H
/*
**  $VER: GadgetBar.h 41.5 (06 Jan 2006 20:00:13)
**
** $Date$
** $Revision$
**
**	Scalos.library include
**
**   (C) Copyright 2000-2007 The Scalos Team
**  All Rights Reserved
**
*/

/****************************************************************************/

#if defined(__GNUC__) && !defined(mc68000)
#pragma pack(2)
#endif /* __GNUC__ */

// +jl+ 20011029
// Tags for GadgetBar class

#define	SCALOSGADGETBAR_BASE		0x80530400

enum GBposition	
	{
	GBPOS_Top, 			// add GadgetBar at window top
	GBPOS_Bottom 			// add GadgetBar at window bottom
	};


#define	GB_FIXED_WIDTH	~0

/****************************************************************************/

// GadgetBar methods

#define	GBCL_UPDATEMEMBER		(SCALOSGADGETBAR_BASE+1001)	// update GadgetBar member
#define	GBCL_HANDLEMOUSEWHEEL           (SCALOSGADGETBAR_BASE+1002)	// mouse wheel event, uses gpInput

/****************************************************************************/

// GadgetBar attributes

#define	GBDTA_WindowTask		(SCALOSGADGETBAR_BASE+1)	// [I..] struct ScaWindowTask *
#define	GBDTA_Position			(SCALOSGADGETBAR_BASE+2)	// [I.G] enum GBposition
#define	GBDTA_TopSpace			(SCALOSGADGETBAR_BASE+3)	// [ISG] space at top edge
#define	GBDTA_BottomSpace		(SCALOSGADGETBAR_BASE+4)	// [ISG] space at bottom edge
#define	GBDTA_LeftSpace			(SCALOSGADGETBAR_BASE+5)	// [ISG] space at left edge
#define	GBDTA_RightSpace		(SCALOSGADGETBAR_BASE+6)	// [ISG] space at right edge
#define	GBDTA_BetweenSpace		(SCALOSGADGETBAR_BASE+7)	// [ISG] space between images
#define	GBDTA_BGPen			(SCALOSGADGETBAR_BASE+8)	// [ISG] background fill pen
#define	GBDTA_Weight			(SCALOSGADGETBAR_BASE+9)	// [...] for OM_ADDMEMBER, child weight, default=GB_FIXED_WIDTH
									// object weight for non-fixed width, set to GB_FIXED_WIDTH for fixed-width image
#define	GBDTA_BackgroundImageName	(SCALOSGADGETBAR_BASE+10)	// [IS.] CONST_STRPTR file name for (datatypes) backfill image
#define	GBDTA_LastActive		(SCALOSGADGETBAR_BASE+11)  	// [..G] (ULONG) GadgetID of most recent active member
#define GBDTA_BackfillHook              (SCALOSGADGETBAR_BASE+12)  	// [...] (struct Hook *) used to forward backfill hook to children
#define GBDTA_Hidden              	(SCALOSGADGETBAR_BASE+13)  	// [ISG] (ULONG) Flag for hidden sub-gadgets (hidden due to lack of space)

/****************************************************************************/

// definition to keep compatible with old typo
#define DBDTA_BackgroundImageName       GBDTA_BackgroundImageName

/****************************************************************************/

// struct for OM_ADDMEMBER
struct opGBAddMember
	{
	ULONG MethodID;
	struct GadgetInfo *opam_GInfo;
	Object *opam_Object;
	struct TagItem *opam_AttrList;
	};

// struct for OM_REMMEMBER
struct opGBRemMember
	{
	ULONG MethodID;
	struct GadgetInfo *oprm_GInfo;
	Object *oprm_Object;
	};

// struct for GBCL_UPDATEMEMBER
struct opGBUpdateMember
	{
	ULONG MethodID;
	struct GadgetInfo *opum_GInfo;
	Object *opum_Object;		// Member object to update
	struct TagItem *opum_AttrList;	// currently unused
	};

/****************************************************************************/

// Tags for GadgetBarImageClass

#define	GBIDTA_WindowTask		(SCALOSGADGETBAR_BASE+101)	// [I..] struct ScaWindowTask *

/****************************************************************************/

// Tags for GadgetBarTextClass

#define	GBTDTA_Text			(SCALOSGADGETBAR_BASE+201)	// [ISG] STRPTR - Text to display
#define	GBTDTA_DrawMode			(SCALOSGADGETBAR_BASE+202)	// [ISG] ULONG - DrawMode for text (JAM1, JAM2)
#define	GBTDTA_Justification		(SCALOSGADGETBAR_BASE+203)	// [ISG] ULONG - Justification for text (GACT_STRINGLEFT, GACT_STRINGRIGHT, GACT_STRINGCENTER)
#define	GBTDTA_TextFont			(SCALOSGADGETBAR_BASE+204)	// [ISG] struct TextFont * - Font for text
#define	GBTDTA_TextPen			(SCALOSGADGETBAR_BASE+205)	// [ISG] ULONG - Text Pen
#define	GBTDTA_BgPen			(SCALOSGADGETBAR_BASE+206)	// [ISG] ULONG - Background Pen (used if GBTDTA_DrawMode = JAM2)
#define GBTDTA_SoftStyle		(SCALOSGADGETBAR_BASE+207)	// [ISG] ULONG - SoftStyle for text (bold/italic/...)
#define	GBTDTA_TTFont			(SCALOSGADGETBAR_BASE+208)	// [ISG] struct TTFontFamily * - TTengine Font for text

/****************************************************************************/

#if defined(__GNUC__) && !defined(mc68000)
#pragma pack()
#endif /* __GNUC__ */

#endif	/* SCALOS_GADGETBAR_H */
