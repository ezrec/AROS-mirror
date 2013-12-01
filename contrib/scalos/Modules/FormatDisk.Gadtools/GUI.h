// Gui.h
// $Date$
// $Revision$

/*
 *  Source generated with GadToolsBox V1.3
 *  which is (c) Copyright 1991,92 Jaba Development
 */

#ifndef  GUI_H
#define  GUI_H

#include <graphics/text.h>

// #define max(a,b) ((a) > (b) ? (a) : (b))

#define GD_DeviceGad			0
#define GD_NameGadget			1
#define GD_FFSGadget			2
#define GD_IntlGadget			3
#define GD_VerifyGadget			4
#define GD_IconGadget			5
#define GD_DirCacheGadget		6	// +jmc+
#define GD_TrashCanNameGadget		7	// +jmc+
#define GD_OKGadget			8
#define GD_QuickFmtGadget		9	// +jmc+
#define GD_CancelGadget			10

#define GDX_DeviceGad			0
#define GDX_NameGadget			1
#define GDX_FFSGadget			2
#define GDX_IntlGadget			3
#define GDX_VerifyGadget		4
#define GDX_IconGadget			5
#define GDX_DirCacheGadget		6	// +jmc+
#define GDX_TrashCanNameGadget		7	// +jmc+
#define GDX_OKGadget			8
#define GDX_QuickFmtGadget		9	// +jmc+
#define GDX_CancelGadget		10


#define GD_StatusGadget 		0
#define GD_StopGadget			1

#define GDX_StatusGadget		0
#define GDX_StopGadget			1

//-----------------------------------------------------------------------------

typedef struct Rect
{
	UWORD left,top,width,height;
	UWORD center;
} Rect;

//-----------------------------------------------------------------------------

extern struct Window	    *PrepWnd;
extern struct Window	    *StatusWnd;
extern struct Gadget	    *PrepGadgets[11];	// Number gadgets.
extern struct Gadget	    *StatusGadgets[2];

extern int SetupScreen( void );
extern void CloseDownScreen( void );
extern int OpenPrepWindow( char *, BOOL, BOOL, BOOL);
extern void ClosePrepWindow( void );
extern void StatusRender( void );
extern int OpenStatusWindow(char *bufPointer);
extern void CloseStatusWindow( void );

extern BOOL NsdSupport;
extern BOOL QuickFmt;
extern BOOL Verify;
extern BOOL Icon;

extern char NewVolumeName[64];

extern Rect box;

extern ULONG FindUnderscoredToLower(STRPTR text);

//-----------------------------------------------------------------------------

#define d1(x)		;
#define d2(x)		x;

extern int kprintf(CONST_STRPTR, ...);
extern int KPrintF(CONST_STRPTR, ...);


#endif /* GUI_H */
