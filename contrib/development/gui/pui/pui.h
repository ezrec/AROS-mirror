/* pui.h */

/** Defines ******************************************/

#define PUI_DUMMY (TAG_USER + 0x762345)

/* boopsi messages */

#define PUIM_SETUP			PUI_DUMMY + 1
#define PUIM_MINMAX			PUI_DUMMY + 2
#define PUIM_LAYOUT			PUI_DUMMY + 3
#define PUIM_DRAW				PUI_DUMMY + 4
#define PUIM_DONOTRENDER	PUI_DUMMY + 5
#define PUIM_MAYRENDER		PUI_DUMMY + 6

/* Classes */

enum {PUIC_Area,
		PUIC_Group,
		PUIC_Button,
		PUIC_String,
		PUIC_Label,
		PUIC_SysImage,
		PUIC_CoolImage,
		PUIC_Cycle,
		PUIC_Prop,
		PUIC_NumClasses};

/* Frame Types (mapped to RealFrameTypes) */

enum {PUIV_Frame_None,
	   PUIV_Frame_Button,
		PUIV_Frame_String,
		PUIV_Frame_Group,
		PUIV_Frame_ImageButton,
		PUIV_Frame_TextImageButton,
		PUIV_Frame_Text,
		PUIV_Frame_Cycle,
		PUIV_Frame_Prop,
		NUM_FRAMES};

/* Real Frame Types */

enum	{FRAMETYPE_NONE,
		 FRAMETYPE_BUTTON,
		 FRAMETYPE_STRING,
		 FRAMETYPE_DROP,
		 FRAMETYPE_MONO,
		 NUM_REALFRAMES};

/* Align Types */

enum {PUIV_Align_Left,
		PUIV_Align_Center,
		PUIV_Align_Right};

enum {PUIV_Align_Top,
		PUIV_Align_Middle,
		PUIV_Align_Bottom};

/* Cool Image Types */

enum {PUIV_CoolImage_Disk,
		PUIV_CoolImage_Save,
		PUIV_CoolImage_Load,
		PUIV_CoolImage_Cancel,
		PUIV_CoolImage_Use,
		PUIV_CoolImage_Monitor,
		PUIV_CoolImage_QuotationMark,
		PUIV_CoolImage_ExclamationMark,
		PUIV_CoolImage_Info,
		NUM_COOLIMAGES};

/* SysImageTypes */

enum {PUIV_SysImage_CheckMark,
		NUM_SYSIMAGES};

/* Make Object Types */

enum {PUIO_None,
		PUIO_TextButton,					// LONG gadgetid,char *text 
		PUIO_ImageButton,					// LONG gadgetid,Object *image
		PUIO_TextImageButton,			// LONG gadgetid,char *text,Object *image
		PUIO_CheckMark,					// LONG gadgetid,LONG selected
		PUIO_CoolImage,					// LONG type
		PUIO_Label							// char *text,LONG textalign,LONG frametype
		};

/* Arrow Types */

enum {ARROW_LEFT,ARROW_RIGHT,ARROW_UP,ARROW_DOWN};

/* Flags */

#define PUIF_Area_Recessed					1
#define PUIF_Area_FramePhantomHoriz		2
#define PUIF_Area_FillArea					4
#define PUIF_Area_AlignLeft				8
#define PUIF_Area_AlignRight				16
#define PUIF_Area_AlignTop					32
#define PUIF_Area_AlignBottom				64
#define PUIF_Area_Layouted					128
#define PUIF_Area_ShowSelState			256
#define PUIF_Area_Hidden					512

#define PUIF_Button_Toggle					1

/* Attributes */

#define PUI_MAXMAX 1000000

#define PUI_ROOT_DUMMY				PUI_DUMMY + 100
#define PUI_AREA_DUMMY				PUI_DUMMY + 200
#define PUI_GROUP_DUMMY				PUI_DUMMY + 300
#define PUI_BUTTON_DUMMY			PUI_DUMMY + 400
#define PUI_STRING_DUMMY			PUI_DUMMY + 500
#define PUI_LABEL_DUMMY				PUI_DUMMY + 600
#define PUI_SYSIMAGE_DUMMY			PUI_DUMMY + 700
#define PUI_COOLIMAGE_DUMMY		PUI_DUMMY + 800
#define PUI_CYCLE_DUMMY				PUI_DUMMY + 900
#define PUI_PROP_DUMMY				PUI_DUMMY + 1000

#define PUIA_Frame					PUI_DUMMY + 1
#define PUIA_FrameRecessed			PUI_DUMMY + 2
#define PUIA_FrameTitle				PUI_DUMMY + 3
#define PUIA_FrameTitleAlign		PUI_DUMMY + 4
#define PUIA_FramePhantomHoriz	PUI_DUMMY + 5
#define PUIA_FillArea				PUI_DUMMY + 6
#define PUIA_HorizWeight			PUI_DUMMY + 7
#define PUIA_VertWeight				PUI_DUMMY + 8
#define PUIA_Weight					PUI_DUMMY + 9
#define PUIA_HorizAlign				PUI_DUMMY + 10
#define PUIA_VertAlign				PUI_DUMMY + 11
#define PUIA_ShowSelState			PUI_DUMMY + 12
#define PUIA_Parent					PUI_DUMMY + 13

#define PUIA_Area_RenderInfo		PUI_AREA_DUMMY + 1

#define PUIA_Root_VisualInfo		PUI_ROOT_DUMMY + 1
#define PUIA_Root_Dimensions		PUI_ROOT_DUMMY + 2

#define PUIA_Group_IsRoot			PUI_GROUP_DUMMY + 1
#define PUIA_Group_Horiz			PUI_GROUP_DUMMY + 2
#define PUIA_Group_Spacing			PUI_GROUP_DUMMY + 3
#define PUIA_Group_Child			PUI_GROUP_DUMMY + 4
#define PUIA_Group_SameSize		PUI_GROUP_DUMMY + 5
#define PUIA_Group_SameWidth		PUI_GROUP_DUMMY + 6
#define PUIA_Group_SameHeight		PUI_GROUP_DUMMY + 7
#define PUIA_Group_PageMode		PUI_GROUP_DUMMY + 8
#define PUIA_Group_ActivePage		PUI_GROUP_DUMMY + 9
#define PUIA_Group_ChildList		PUI_GROUP_DUMMY + 10

#define PUIA_Button_Text			PUI_BUTTON_DUMMY + 1
#define PUIA_Button_Image			PUI_BUTTON_DUMMY + 2
#define PUIA_Button_TextStyle		PUI_BUTTON_DUMMY + 3
#define PUIA_Button_Toggle			PUI_BUTTON_DUMMY + 4
#define PUIA_Button_TextAlign		PUI_BUTTON_DUMMY + 5

#define PUIA_String_Contents		PUI_STRING_DUMMY + 1
#define PUIA_String_MaxChars		STRINGA_MaxChars

#define PUIA_Label_Text				PUIA_Button_Text
#define PUIA_Label_Image			PUIA_Button_Image
#define PUIA_Label_TextStyle		PUIA_Button_TextStyle
#define PUIA_Label_TextAlign		PUIA_Button_TextAlign

#define PUIA_SysImage_Which		SYSIA_Which

#define PUIA_CoolImage_Which		PUI_COOLIMAGE_DUMMY + 1

#define PUIA_Cycle_Entries			PUI_CYCLE_DUMMY + 1
#define PUIA_Cycle_Active			PUI_CYCLE_DUMMY + 2

#define PUIA_Prop_Entries			PGA_Total
#define PUIA_Prop_First				PGA_Top
#define PUIA_Prop_Visible			PGA_Visible
#define PUIA_Prop_Horiz				PUI_PROP_DUMMY + 1


/** Variablen ****************************************/

#ifndef NOVARS

#include "vars.h"

#endif


/** Strukturen ***************************************/

struct PUI_RenderInfo
{
	struct DrawInfo			*dri;
	struct Screen				*scr;
	struct ColorMap			*cm;
	struct BitMap				*bm;
	struct Layer_Info			*li;
	struct TextFont			*font;
	UWORD							*pens;
	struct RastPort			temprp;
	struct SignalSemaphore	sem;
	
	WORD							winborderl;
	WORD							winborderr;
	WORD							winbordero;
	WORD							winborderu;
	WORD							fontheight;
	WORD							fontbaseline;
	WORD							titleheight;
	WORD							maxclientwidth;
	WORD							maxclientheight;
	WORD							scrwidth;
	WORD							scrheight;
};

struct PUI_MinMax
{
	LONG	minwidth,minheight;
	LONG	maxwidth,maxheight;
	LONG	defwidth,defheight;
};

struct PUI_FrameInfo
{
	WORD	type;
	WORD	pen1;
	WORD	pen2;
	WORD	pen3;
	WORD	recessed;
	BYTE	borderleft;
	BYTE	borderright;
	BYTE	bordertop;
	BYTE	borderbottom;
	BYTE	spaceleft;
	BYTE	spaceright;
	BYTE	spacetop;
	BYTE	spacebottom;
};

struct PUI_RealFrameInfo
{
	BYTE	borderleft;
	BYTE	borderright;
	BYTE	bordertop;
	BYTE	borderbottom;
};

struct SetupMsg
{
	ULONG		MethodID;
	struct	GadgetInfo		*gi;
	struct	PUI_RenderInfo	*ri;
	Object	*root;
};

struct MinMaxMsg
{
	ULONG		MethodID;
	struct	GadgetInfo	*gi;
	struct	PUI_MinMax *minmax;	
};

struct LayoutMsg
{
	ULONG		MethodID;
	struct	GadgetInfo	*gi;
	struct	IBox	box;
};

struct DrawMsg
{
	ULONG		MethodID;
	struct	GadgetInfo *gi;
	struct	RastPort *rp;
	LONG		x,y;
	LONG		state;
};

/* instance data */

struct PUI_AreaData
{
	struct PUI_MinMax			minmax;
	struct IBox					box;
	struct PUI_RenderInfo	*ri;
	Object						*root;
	Object						*parent;
	char							*frametitle;
	BYTE							offsetl;
	BYTE							offsetr;
	BYTE							offseto;
	BYTE							offsetu;
	ULONG							flags;
	WORD							layoutwidth;
	WORD							layoutheight;
	WORD							horizweight;
	WORD							vertweight;
	WORD							renderlock;
	BYTE							frametype;
	BYTE							frametitlealign;
};



/** Protos *******************************************/

/* main */

void CloseLibs(void);
void KillClasses(void);

/* misc */

void DrawFrame(Object *obj,struct RastPort *rp,struct IBox *box,WORD id,BOOL rec);
void DrawLine(struct RastPort *rp,WORD x1,WORD y1,WORD x2,WORD y2);
void DrawArrow(struct RastPort *rp,WORD x1,WORD y1,WORD type);
void RectFillBox(struct RastPort *rp,struct IBox *box);
void MyTextExtent(struct TextFont *font,char *text,WORD len,WORD style,WORD *width,WORD *height);
LONG SelectWidth(struct IBox *box,struct PUI_MinMax *minmax);
LONG SelectHeight(struct IBox *box,struct PUI_MinMax *minmax);
LONG SelectLeft(Object *obj,struct IBox *box,struct PUI_MinMax *minmax);
LONG SelectTop(Object *obj,struct IBox *box,struct PUI_MinMax *minmax);
void CorrectMinMax(struct PUI_MinMax *minmax);
void ShrinkBox(struct IBox *box,WORD left,WORD top,WORD right,WORD bottom);
LONG GreatestMinWidth(struct MinList *list);
LONG GreatestMinHeight(struct MinList *list);
void RenderObject_Update(Object *obj,struct GadgetInfo *gi);
void RenderObject_Redraw(Object *obj,struct GadgetInfo *gi);
Object *NextTabCycleObject(Object *actobj);
Object *PrevTabCycleObject(Object *actobj);

/* api */

APTR PUI_GetVisualInfoA(struct Screen *scr,struct TagItem *tags);
void PUI_FreeVisualInfo(APTR vi);
Object *PUI_NewRootObjectA(APTR vi,struct TagItem *tags);
Object *PUI_NewRootObject(APTR vi,Tag tag, ...);
Object *PUI_NewObjectA(LONG cl,struct TagItem *tags);
Object *PUI_NewObject(LONG cl,Tag tag, ...);
Object *PUI_MakeObjectA(LONG type,LONG *params);
Object *PUI_MakeObject(LONG type, ...);
LONG PUI_MayRender(Object *obj);
LONG PUI_WindowWidth(Object *obj);
LONG PUI_WindowHeight(Object *obj);
LONG PUI_InnerWidth(Object *obj);
LONG PUI_InnerHeight(Object *obj);
void PUI_SetWindowLimits(struct Window *win,Object *obj);
struct IntuiMessage *PUI_GetIMsg(struct MsgPort *msgport);
void PUI_ReplyIMsg(struct IntuiMessage *intuimsg);

/* ButtonClass */

ULONG ButtonInstanceSize(void);
ULONG ButtonDispatcher(struct IClass *cl,Object *obj,Msg msg);

/* GroupClass */

ULONG GroupInstanceSize(void);
ULONG GroupDispatcher(struct IClass *cl,Object *obj,Msg msg);

/* AreaClass */

ULONG AreaInstanceSize(void);
ULONG AreaDispatcher(struct IClass *cl,Object *obj,Msg msg);

/* StringClass */

ULONG StringInstanceSize(void);
ULONG StringDispatcher(struct IClass *cl,Object *obj,Msg msg);

/* LabelClass */

ULONG LabelInstanceSize(void);
ULONG LabelDispatcher(struct IClass *cl,Object *obj,Msg msg);

/* SysImageClass */

ULONG SysImageInstanceSize(void);
ULONG SysImageDispatcher(struct IClass *cl,Object *obj,Msg msg);

/* CoolImageClass */

ULONG CoolImageInstanceSize(void);
ULONG CoolImageDispatcher(struct IClass *cl,Object *obj,Msg msg);

/* CycleClass */

ULONG CycleInstanceSize(void);
ULONG CycleDispatcher(struct IClass *cl,Object *obj,Msg msg);

/* PropClass */

ULONG PropInstanceSize(void);
ULONG PropDispatcher(struct IClass *cl,Object *obj,Msg msg);

/* remap.asm */

#ifdef _AROS
extern void RemapBytes(APTR mem,APTR remaptable,LONG size);
#else
extern "ASM" void RemapBytes(register __a0 APTR mem,register __a1 APTR remaptable,register __d0 LONG size);
#endif

/* p2c.asm */

#ifdef _AROS
extern void Planar2Chunky(APTR source,APTR dest,WORD breite,WORD planes);
#else
extern "ASM" void Planar2Chunky(APTR source,APTR dest,WORD breite,WORD planes);
#endif


/** Makros *******************************************/

#define RootObject(x)				PUI_NewRootObject(x
#define RootObjectV(x)				PUI_NewRootObject(x
#define RootObjectH(x)				PUI_NewRootObject(x,PUIA_Group_Horiz,TRUE

#define AreaObject					PUI_NewObject(PUIC_Area
#define ButtonObject					PUI_NewObject(PUIC_Button
#define StringObject					PUI_NewObject(PUIC_String
#define LabelObject					PUI_NewObject(PUIC_Label
#define SysImageObject				PUI_NewObject(PUIC_SysImage
#define CoolImageObject				PUI_NewObject(PUIC_CoolImage
#define CycleObject					PUI_NewObject(PUIC_Cycle
#define PropObject					PUI_NewObject(PUIC_Prop

#define VGroup							PUI_NewObject(PUIC_Group
#define HGroup							PUI_NewObject(PUIC_Group,PUIA_Group_Horiz,TRUE

#define Child							PUIA_Group_Child
#define End								TAG_DONE)

#define ButtonFrame					PUIA_Frame,PUIV_Frame_Button
#define StringFrame					PUIA_Frame,PUIV_Frame_String
#define GroupFrame					PUIA_Frame,PUIV_Frame_Group
#define ImageButtonFrame			PUIA_Frame,PUIV_Frame_ImageButton
#define TextImageButtonFrame		PUIA_Frame,PUIV_Frame_TextImageButton
#define TextFrame						PUIA_Frame,PUIV_Frame_Text
#define PropFrame						PUIA_Frame,PUIV_Frame_Prop

#define ButtonFrame0					ButtonFrame,PUIA_FramePhantomHoriz,TRUE
#define StringFrame0					StringFrame,PUIA_FramePhantomHoriz,TRUE
#define GroupFrame0					GroupFrame,PUIA_FramePhantomHoriz,TRUE
#define ImageButtonFrame0			ImageButtonFrame,PUIA_FramePhantomHoriz,TRUE
#define TextImageButtonFrame0		TextImageButtonFrame,PUIA_FramePhantomHoriz,TRUE
#define TextFrame0					TextFrame,PUIA_FramePhantomHoriz,TRUE

#define GroupFrameT(x)				GroupFrame,PUIA_FrameTitle,(x)
#define GroupFrameTL(x)				GroupFrameT(x),PUIA_FrameTitleAlign,PUIV_Align_Left
#define GroupFrameTC(x)				GroupFrameT(x),PUIA_FrameTitleAlign,PUIV_Align_Center
#define GroupFrameTR(x)				GroupFrameT(x),PUIA_FrameTitleAlign,PUIV_Align_Right

#define RelVerify						GA_RelVerify,TRUE
#define Immediate						GA_Immediate,TRUE


#define TextButton(a,x)				PUI_MakeObject(PUIO_TextButton,(a),(x))
#define ImageButton(a,x)			PUI_MakeObject(PUIO_ImageButton,(a),(x))
#define TextImageButton(a,x,y)	PUI_MakeObject(PUIO_TextImageButton,(a),(x),(y))
#define CheckMark(a,x)				PUI_MakeObject(PUIO_CheckMark,(a),(x))

#define Label(a)						PUI_MakeObject(PUIO_Lable,(A),PUIV_Align_Right,PUIV_Frame_None)
#define ButtonLabel(a)				PUI_MakeObject(PUIO_Label,(a),PUIV_Align_Right,PUIV_Frame_Button)
#define StringLabel(a)				PUI_MakeObject(PUIO_Label,(a),PUIV_Align_Right,PUIV_Frame_String)
#define GroupLabel(a)				PUI_MakeObject(PUIO_Label,(a),PUIV_Align_Right,PUIV_Frame_Group)
#define ImageButtonLabel(a)		PUI_MakeObject(PUIO_Label,(a),PUIV_Align_Right,PUIV_Frame_ImageButton)
#define TextImageButtonLabel(a)	PUI_MakeObject(PUIO_Label,(a),PUIV_Align_Right,PUIV_Frame_TextImageButton)
#define TextLabel(a)					PUI_MakeObject(PUIO_Label,(a),PUIV_Align_Right,PUIV_Frame_Text)

#define CLabel(a)						PUI_MakeObject(PUIO_Lable,(A),PUIV_Align_Center,PUIV_Frame_None)
#define CButtonLabel(a)				PUI_MakeObject(PUIO_Label,(a),PUIV_Align_Center,PUIV_Frame_Button)
#define CStringLabel(a)				PUI_MakeObject(PUIO_Label,(a),PUIV_Align_Center,PUIV_Frame_String)
#define CGroupLabel(a)				PUI_MakeObject(PUIO_Label,(a),PUIV_Align_Center,PUIV_Frame_Group)
#define CImageButtonLabel(a)		PUI_MakeObject(PUIO_Label,(a),PUIV_Align_Center,PUIV_Frame_ImageButton)
#define CTextImageButtonLabel(a)	PUI_MakeObject(PUIO_Label,(a),PUIV_Align_Center,PUIV_Frame_TextImageButton)
#define CTextLabel(a)				PUI_MakeObject(PUIO_Label,(a),PUIV_Align_Center,PUIV_Frame_Text)

#define LLabel(a)						PUI_MakeObject(PUIO_Lable,(A),PUIV_Align_Left,PUIV_Frame_None)
#define LButtonLabel(a)				PUI_MakeObject(PUIO_Label,(a),PUIV_Align_Left,PUIV_Frame_Button)
#define LStringLabel(a)				PUI_MakeObject(PUIO_Label,(a),PUIV_Align_Left,PUIV_Frame_String)
#define LGroupLabel(a)				PUI_MakeObject(PUIO_Label,(a),PUIV_Align_Left,PUIV_Frame_Group)
#define LImageButtonLabel(a)		PUI_MakeObject(PUIO_Label,(a),PUIV_Align_Left,PUIV_Frame_ImageButton)
#define LTextImageButtonLabel(a)	PUI_MakeObject(PUIO_Label,(a),PUIV_Align_Left,PUIV_Frame_TextImageButton)
#define LTextLabel(a)				PUI_MakeObject(PUIO_Label,(a),PUIV_Align_Left,PUIV_Frame_Text)

#define CoolDiskImage				PUI_MakeObject(PUIO_CoolImage,PUIV_CoolImage_Disk)
#define CoolSaveImage				PUI_MakeObject(PUIO_CoolImage,PUIV_CoolImage_Save)
#define CoolLoadImage				PUI_MakeObject(PUIO_CoolImage,PUIV_CoolImage_Load)
#define CoolCancelImage				PUI_MakeObject(PUIO_CoolImage,PUIV_CoolImage_Cancel)
#define CoolUseImage					PUI_MakeObject(PUIO_CoolImage,PUIV_CoolImage_Use)
#define CoolMonitorImage			PUI_MakeObject(PUIO_CoolImage,PUIV_CoolImage_Monitor)
#define CoolQuotationMarkImage	PUI_MakeObject(PUIO_CoolImage,PUIV_CoolImage_QuotationMark)
#define CoolExclamationMarkImage	PUI_MakeObject(PUIO_CoolImage,PUIV_CoolImage_ExclamationMark)
#define CoolInfoImage				PUI_MakeObject(PUIO_CoolImage,PUIV_CoolImage_Info)




/** Variablen ****************************************/

/** Variablen ****************************************/

/** Variablen ****************************************/
