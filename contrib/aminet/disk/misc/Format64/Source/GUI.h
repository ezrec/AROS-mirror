/*
 *  Source generated with GadToolsBox V1.3
 *  which is (c) Copyright 1991,92 Jaba Development
 */

#define GD_FFSGadget			       0
#define GD_NameGadget			       1
#define GD_IconGadget			       2
#define GD_QuickFmtGadget		       3
#define GD_VerifyGadget 		       4
#define GD_OKGadget			       5
#define GD_CancelGadget 		       6
#define GD_DeviceGad			       7

#define GDX_FFSGadget			       0
#define GDX_NameGadget			       1
#define GDX_IconGadget			       2
#define GDX_QuickFmtGadget		       3
#define GDX_VerifyGadget		       4
#define GDX_OKGadget			       5
#define GDX_CancelGadget		       6
#define GDX_DeviceGad			       7

#define GD_StatusGadget 		       0
#define GD_StopGadget			       1

#define GDX_StatusGadget		       0
#define GDX_StopGadget			       1

extern struct Screen	    *Scr;
extern APTR		     VisualInfo;
extern struct Window	    *PrepWnd;
extern struct Window	    *StatusWnd;
extern struct Gadget	    *PrepGList;
extern struct Gadget	    *StatusGList;
extern struct Gadget	    *PrepGadgets[8];
extern struct Gadget	    *StatusGadgets[2];
extern UWORD		     PrepLeft;
extern UWORD		     PrepTop;
extern UWORD		     PrepWidth;
extern UWORD		     PrepHeight;
extern UWORD		     StatusLeft;
extern UWORD		     StatusTop;
extern UWORD		     StatusWidth;
extern UWORD		     StatusHeight;
extern UBYTE		    *PrepWdt;
extern UBYTE		    *StatusWdt;
extern struct TextAttr	    *Font, Attr;
extern UWORD		     FontX, FontY;
extern UWORD		     OffX, OffY;

extern int SetupScreen( void );
extern void CloseDownScreen( void );
extern int OpenPrepWindow( char *);
extern void ClosePrepWindow( void );
extern void StatusRender( void );
extern int OpenStatusWindow(char *bufPointer);
extern void CloseStatusWindow( void );

typedef struct Rect
{
   UWORD left,top,width,height;
   UWORD center;
} Rect;




