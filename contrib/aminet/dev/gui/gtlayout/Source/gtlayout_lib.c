
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

#include <exec/execbase.h>
#include <exec/resident.h>

#include <dos/dos.h>	/* For BPTR definition */

/*****************************************************************************/

#include "Assert.h"

#include "gtlayout.library_rev.h"

/*****************************************************************************/

/****** gtlayout.library/--version-- ******************************************
*
*   NOTES
*	This document describes gtlayout.library v5.12 or higher. Do not assume that
*	previous library releases support the same features.
*
******************************************************************************
*
*/

/****** gtlayout.library/--background-- ******************************************
*
*   NOTES
*	1. General information
*
*	1.1 Purpose
*
*	The GUI code included in this archive helps to create user interfaces
*	using gadtools.library with a minimum of effort. The code
*	automatically takes care of the font to be used, making the user
*	interface font independent. Localizing support is built right into
*	the code, just install a callback hook and pass numeric IDs for the
*	gadget labels: the code will invoke your hook in order to get the
*	text required. Keystroke activation of gadgets is also taken care
*	of, in fact the code will -- unless told not to do so -- assign keyboard
*	shortcuts to the gadgets created all on its own. Every effort has been
*	made to make the code reentrant, so it can be put into a shared library.
*	If a user interface does not fit onto a screen provisions are made to
*	choose a smaller font and to rescale the window contents until they fit.
*	Last but not least the user interface code offers transparent extensions
*	to the standard gadtools.library objects, such as LISTVIEW_KIND objects
*	which respond to double-clicks or STRING_KIND objects which can be used
*	to enter password text as they will not display the characters entered.
*
*
*	1.2 Distribution
*
*	The code is *free*, you don't need to pay any money to use it, nor
*	do you need to quote my name in the documentation, the program or
*	anywhere else. You are allowed to make changes to the code, but if
*	you stumble across any bugs or even know how to fix them, please
*	let me know. It does not matter whether you intend to sell a program
*	to use the code, use the code in shareware, gift-ware, freeware or
*	etc.-ware programs: the code still remains royalty-free.
*
*
*	1.3 Caveats
*
*	The code is pretty large, about 80K-100K bytes in size. Not all
*	gadtools.library type objects are supported, notably
*	GENERIC_KIND objects. The code is not as flexible as
*	gadtools.library, so certain things which are easily done using
*	gadtools.library may be pretty difficult or even impossible.
*	The code is written entirely in `C' and requires SAS/C to
*	compile. Some parts of the code are highly recursive; I haven't
*	tested how much stack they might require in certain cases,
*	but I recommend that you don't overuse the grouping feature.
*	The data structures required to create and maintain the
*	user interface are huge, a single window might require more
*	than 4K-6K of memory. Proportional font support only works
*	well starting with Kickstart v39 and up, v2.04 will probably
*	not look quite that pretty.
*
*
*	2. Programming
*
*	2.1 Client libraries required for link library version
*
*	You need to have SysBase and GadToolsBase initialized in order to make
*	use of the code, i.e. your code has to do
*	WaitPort()...GT_GetIMsg()...GT_ReplyIMsg all on its own. The code makes
*	use of the memory pools introduced in exec.library v39, but calls the
*	equivalent routines in amiga.lib. Note: as of this writing the pools
*	code in amiga.lib v40.14 is broken, so you need to link with Mike
*	Sinz' fixed pools.lib.
*
*
*	2.2 Invocation procedure
*
*	The typical invocation procedure looks roughly like this:
*
*	   LT_Init();	// only for link library version
*	   :
*	   :
*	      LT_CreateHandleTags();
*	         LT_New();
*	         :
*	         :
*	         LT_New();
*	            LT_Build();
*	               LT_HandleInput();
*	      LT_DeleteHandle();
*	   :
*	   :
*	   LT_Exit();	// only for link library version
*
*	You need to call LT_Init() only once in your program, it will initialize
*	the libraries and global data structures required by the user interface
*	code. When you are finished with the user interface and your program is
*	about to exit you need to call LT_Exit() or memory will get lost.
*	Note that LT_Init() is not protected against multiple invocations. If
*	called repeatedly memory will get lost which can never be reclaimed.
*	However, LT_Exit() is protected against multiple invocations, you can
*	also call it before ever giving LT_Init() a call, but I doubt this
*	would make much sense. If you are using the shared gtlayout.library
*	no call to LT_Init()/LT_Exit() is necessary as these calls are already
*	wrapped into the library opening code.
*	   Before you can actually start building a window layout a call to
*	LT_CreateHandleTags() needs to be made. You need to pass in a pointer
*	to the Screen your user interface window is to be opened on and,
*	optionally, a few tags to control the look and performance of the
*	interface. *Never* close the screen in question before calling
*	LT_DeleteHandle() or nasty things will happen. For public screens
*	the code will try to lock the screen in question. With the handle
*	LT_CreateHandleTags() returned you can call LT_New() to build the
*	user interface. When finished a call to LT_Build() will finally
*	open a window and place the gadgets inside. A pointer to the
*	Window created will be returned, ready to be used for the
*	WaitPort()...GT_GetIMsg()...LT_HandleInput()...GT_ReplyIMsg()
*	loop. When finished, a call to LT_DeleteHandle() will close the
*	window and release all the memory associated with it. The design
*	of the interface code is similar to the corresponding calls in
*	gadtools.library, i.e. you don't need to worry about LT_New()
*	failing to allocate memory for the objects required. When it comes
*	to LT_Build() the code will know about any trouble which would
*	show up during previous invocations of LT_New(). In essence,
*	if LT_Build() returns NULL something is wrong.
*
*
*	2.3 Hierarchic grouping
*
*	The basic building block of the user interface is a group, either
*	a horizontal or a vertical group. Adding gadgets or other objects
*	to a horizontal group will place them side by side from left to
*	right. A vertical group causes objects to be place from top to
*	bottom in one straight line. Groups help to arrange objects
*	neatly stacked, centered and properly aligned with other
*	members of the group.
*
*	MUCHO IMPORTANTE: there is a bug lurking in the code which I never
*	had the luck to find and fix. One would expect to create
*	user interface structures like this:
*
*	   <group start>
*	      <button>
*	      <list>
*	      <group start>
*	         <slider>
*	         <text>
*	      <group end>
*	      <button>
*	   <group end>
*
*	However, it is in fact not possible to mix gadgets and groups.
*	Thus, the user interface structure would have to look like this:
*
*	   <group start>
*	      <group start>
*	         <button>
*	         <list>
*	      <group end>
*	      <group start>
*	         <slider>
*	         <text>
*	      <group end>
*	      <group start>
*	         <button>
*	      <group end>
*	   <group end>
*
*	Or in other words: groups only mix with other groups.
*
*	You build groups using three different object types. In this
*	context `object type' refers to a specific numeric value the
*	LT_New() routine knows which will cause it to add another leaf
*	to the user interface structure tree. Here is an example:
*
*	   struct LayoutHandle *Handle;
*
*	   if(Handle = LT_CreateHandleTags(NULL,
*	      LAHN_AutoActivate,FALSE,
*	   TAG_DONE))
*	   {
*	      struct Window *Window;
*
*	      LT_New(Handle,
*	         LA_Type,      VERTICAL_KIND,  \* A vertical group. *\
*	         LA_LabelText, "Main group",   \* Group title text. *\
*	      TAG_DONE);
*	      {
*	         LT_New(Handle,
*	            LA_Type,      BUTTON_KIND, \* A plain button. *\
*	            LA_LabelText, "A button",
*	            LA_ID,        11,
*	         TAG_DONE);
*
*	         LT_New(Handle,
*	            LA_Type,      XBAR_KIND,   \* A separator bar. *\
*	         TAG_DONE);
*
*	         LT_New(Handle,
*	            LA_Type,      BUTTON_KIND, \* A plain button. *\
*	            LA_LabelText, "Another button",
*	            LA_ID,        22,
*	         TAG_DONE);
*
*	         LT_New(Handle,
*	            LA_Type,      END_KIND,    \* This ends the current group. *\
*	         TAG_DONE);
*	      }
*
*	      if(Window = LT_Build(Handle,
*	        LAWN_Title,     "Window title",
*	        LAWN_IDCMP,     IDCMP_CLOSEWINDOW,
*	        WA_CloseGadget, TRUE,
*	      TAG_DONE))
*	      {
*	          struct IntuiMessage *Message;
*	          ULONG                MsgQualifier,
*	                               MsgClass;
*	          UWORD                MsgCode;
*	          struct Gadget       *MsgGadget;
*	          BOOL                 Done = FALSE;
*
*	          do
*	          {
*	              WaitPort(Window->UserPort);
*
*	              while(Message = GT_GetIMsg(Window->UserPort))
*	              {
*	                 MsgClass     = Message->Class;
*	                 MsgCode      = Message->Code;
*	                 MsgQualifier = Message->Qualifier;
*	                 MsgGadget    = Message->IAddress;
*
*	                 GT_ReplyIMsg(Message);
*
*	                 LT_HandleInput(Handle,MsgQualifier,&MsgClass,
*	                     &MsgCode,&MsgGadget);
*
*	                 switch(MsgClass)
*	                 {
*	                    case IDCMP_CLOSEWINDOW:
*
*	                        Done = TRUE;
*	                        break;
*
*	                    case IDCMP_GADGETUP:
*
*	                        switch(MsgGadget->GadgetID)
*	                        {
*	                            case 11: printf("First gadget\n");
*	                                     break;
*
*	                            case 22: printf("Second gadget\n");
*	                                     break;
*	                        }
*
*	                        break;
*	                 }
*	              }
*	          }
*	          while(!Done);
*	      }
*
*	      LT_DeleteHandle(Handle);
*	   }
*
*	The example creates one single group, places a few objects inside,
*	calls the layout routine, handles the input and finally cleans
*	things up again. This example also shows that you *need* at
*	least one group in your tree (to form the root) in order to get
*	things to work.
*	   The input loop requires you to call LT_HandleInput() in order
*	to get the user interface code to filter out certain events and
*	to update internal information. The data passed in must have
*	been processed via the gadtools.library routines. You *must not*
*	call LT_HandleInput() before GT_ReplyIMsg() is called since the
*	routine may call intuition.library and gadtools.library routines
*	which in turn might lead to a system lock-up if the message
*	has not been processed yet. The first thing to do after LT_HandleInput()
*	has done whatever was necessary to the data you passed in is
*	examine the MsgClass variable. The user interface code will
*	`fake' certain message events using the variables passed in,
*	*do not* use any other data gathered from the original
*	IntuiMessage. The MsgClass may include event types you did
*	not ask for, i.e. the IDCMP flags of the window opened
*	will be set according to the objects you added to the window.
*	Also, the IDCMP_IDCMPUPDATE message class will show up for
*	certain objects. More on this later in this document.
*
*
*	2.4 Setting and getting object attributes
*
*	The mechanism to update and query object attributes does not
*	exactly match the familiar gadtools.library interface. In
*	fact, the routine to change gadget attributes will forward
*	the tagitem list passed in to gadtools.library/GT_SetGadgetAttrs().
*	On the other hand the routine to query object attributes does
*	not work like gadtools.library/GT_GetGadgetAttrs(). The
*	user interface code assumes that all objects it can handle and
*	create posess certain attributes unique to the type of the
*	object in question. For example, the unique attribute of a
*	STRING_KIND object would be a pointer to the string it
*	`contains'. The unique attribute of a SLIDER_KIND object is
*	the current slider position. The LT_GetAttributes() routine
*	will return this attribute, but also accept a tagitem list
*	to fill in for certain special tag values.
*
*
*	2.5 Extra data
*
*	Once a LayoutHandle has been created the interface code will
*	provide you with a number of information concerning the screen
*	the handle has been attached to. This information includes
*	the DrawInfo structure of the screen, the VisualInfo data
*	and the Screen address. This information is read-only.
*
*
*	2.6 Menus
*
*	With a LayoutHandle available a routine called LT_LayoutMenuTags()
*	will create a standard Intuition menu structure via gadtools.library
*	which can be passed to LT_Build(). Note that this
*	routine does not modify any data passed in, it does neither
*	attach the menu created to the LayoutHandle passed in,
*	nor does it change the NewMenu table.
*
*
*	2.7 Localization
*
*	All object and menu creation routines support localization via
*	a Hook callback interface, i.e. you can pass a pointer to an
*	initialized Hook structure to LT_CreateHandleTags() which will
*	later be used to supply label and list text for objects
*	created. The Hook callback routine is called in the following
*	fashion:
*
*	   String = HookFunc(struct Hook *Hook,struct LayoutHandle *Handle,LONG ID)
*	     D0                            A0                         A2        A1
*
*	Or in other words: a locale string ID is passed in, the routine is supposed
*	to look up the string to match this ID and to return it.
*
*
*	2.8 Object types to generate IDCMP_IDCMPUPDATE events
*
*	Certain objects convey extra information which is merged into the `fake'
*	input stream passed to the client calling LT_HandleInput(). These objects
*	are:
*
*	   STRING_KIND
*	   TEXT_KIND
*	   PALETTE_KIND
*
*	      The user pressed the `select' button which belongs
*	      to this gadget. The MsgGadget pointer indicates the
*	      STRING_KIND/TEXT_KIND/PALETTE_KIND object the `select'
*	      button belongs to.
*
*	   LISTVIEW_KIND
*
*	      The user double-clicked on an entry. The entry number
*	      is returned in the MsgCode variable. The MsgGadget
*	      pointer indicates the LISTVIEW_KIND object the user
*	      has clicked on.
*
*
*	2.9 Keystroke activation
*
*	Unless forbidden via the the LA_NoKey tag item the user interface
*	code will pick the keyboard shortcuts for all gadgets on its own.
*	The currently active global console keymap will be checked at the
*	time when LT_Init() is called in order to make sure subsequent
*	calls to LT_Build() will use only keys the user can press on
*	the keyboard. Double-dead keys are also excluded from the
*	table created. This avoids problems with gadget labels such as
*	"יייי" which would require the user to hit two keys in a row to
*	activate the gadget.
*	   If the window created happens to feature a close gadget
*	pressing the `Esc' key will cause the client to receive
*	an IDCMP_CLOSEWINDOW event.
*	   A single LISTVIEW_KIND object may receive special treatment
*	if the LALV_CursorKey tag is used: the user will be able to
*	operate the listview using the cursor keys. Note: this
*	will also keep the user interface code from choosing a
*	special keystroke from the gadget label.
*	   The user will be able to operate a single BUTTON_KIND
*	object using the return key if the LABT_ReturnKey tag is
*	used. A recessed frame will be drawn around the button hit
*	box to indicate its special status.
*	   Pressing the Tab key can be bound to operate a cycle or
*	mx kind object.
*
*	3. Credits
*
*	The original design is based upon the user interface layout code used by
*	`term' 3.1. I put the first version of the layout routines together back
*	in Summer 1993 when I wanted to write the follow-up to `term' v3.4.
*
*	Martin Taillefer rewrote large parts of the code, added new routines and
*	generally improved the performance of the layout process. I owe Martin
*	much for the ideas he put into the library.
*
*	Kai Iske, Christoph Feck, Stefan Becker, Michael Barsoom, Sven Stullich
*	and Mark Ritter helped to iron out the remaining bugs and piled up bug
*	reports and enhancement requests.
*
******************************************************************************
*
*/

#ifndef __AROS__
/*****************************************************************************/

	// First executable location of this library, must return an
	// error to the unsuspecting caller

LONG
ReturnError(VOID)
{
	return(-1);
}

/*****************************************************************************/

struct GTLayoutBase
{
	struct Library			LibNode;

	struct Library *		ExecBase;
	BPTR					LibSegment;
	struct SignalSemaphore	LockSemaphore;
};

#define SysBase GTLayoutBase->ExecBase

/*****************************************************************************/
struct GTLayoutBase *GTLayoutBase;
/*****************************************************************************/

STATIC struct Library * LIBENT
LibInit(
	REG(d0) struct GTLayoutBase *	_GTLayoutBase,
	REG(a0) BPTR					Segment,
	REG(a6) struct ExecBase *		ExecBase)
{
	struct Library *result = NULL;

        GTLayoutBase = _GTLayoutBase;
        
	GTLayoutBase->ExecBase = (struct Library *)ExecBase;

#if !defined(CPU_ANY) && !defined(CPU_any)
	if(ExecBase->AttnFlags & AFF_68020)
#endif	// CPU_ANY


	if(ExecBase->LibNode.lib_Version >= 37)
	{
		GTLayoutBase->LibNode.lib_Revision = REVISION;

		GTLayoutBase->LibSegment = Segment;

		InitSemaphore(&GTLayoutBase->LockSemaphore);

		result = GTLayoutBase;
	}

		/* If the initialization failed, we will have to release
		 * the memory allocated for the library base and
		 * vector. This is an often overlooked requirement for
		 * RTF_AUTOINIT libraries/resources/devices which
		 * return NULL in the initialization procedure.
		 */

	if(result == NULL)
	{
		FreeMem((BYTE *)GTLayoutBase - GTLayoutBase->LibNode.lib_NegSize,
		        GTLayoutBase->LibNode.lib_NegSize + GTLayoutBase->LibNode.lib_PosSize);
	}

	return(result);
}

/*****************************************************************************/

STATIC struct Library * LIBENT
LibOpen(REG(d0) ULONG version,
        REG(a6) struct GTLayoutBase *GTLayoutBase)
{
	struct Library *result = GTLayoutBase;
	UWORD openCnt;

	openCnt = GTLayoutBase->LibNode.lib_OpenCnt;

	GTLayoutBase->LibNode.lib_OpenCnt++;
	GTLayoutBase->LibNode.lib_Flags &= ~LIBF_DELEXP;

	ObtainSemaphore(&GTLayoutBase->LockSemaphore);

	if(openCnt == 0)
	{
		if(!LT_Init())
		{
			LT_Exit();

			result = NULL;
		}
	}

	ReleaseSemaphore(&GTLayoutBase->LockSemaphore);

	if(result == NULL)
		GTLayoutBase->LibNode.lib_OpenCnt--;

	return(result);
}

/*****************************************************************************/

STATIC BPTR LIBENT
LibExpunge(REG(a6) struct GTLayoutBase *GTLayoutBase)
{
	BPTR result;

	if(GTLayoutBase->LibNode.lib_OpenCnt == 0 && GTLayoutBase->LibSegment != NULL)
	{
		result = GTLayoutBase->LibSegment;

		Remove((struct Node *)GTLayoutBase);

		FreeMem((BYTE *)GTLayoutBase - GTLayoutBase->LibNode.lib_NegSize,
		        GTLayoutBase->LibNode.lib_NegSize + GTLayoutBase->LibNode.lib_PosSize);
	}
	else
	{
		result = NULL;

		GTLayoutBase->LibNode.lib_Flags |= LIBF_DELEXP;
	}

	return(result);
}

/*****************************************************************************/

STATIC BPTR LIBENT
LibClose(REG(a6) struct GTLayoutBase *GTLayoutBase)
{
	BPTR result;

	ObtainSemaphore(&GTLayoutBase->LockSemaphore);

	if(GTLayoutBase->LibNode.lib_OpenCnt == 1)
	{
		LT_Exit();
	}

	GTLayoutBase->LibNode.lib_OpenCnt--;

	ReleaseSemaphore(&GTLayoutBase->LockSemaphore);

	if(GTLayoutBase->LibNode.lib_OpenCnt == 0 && (GTLayoutBase->LibNode.lib_Flags & LIBF_DELEXP))
                result = LibExpunge(GTLayoutBase);
	else
		result = NULL;

	return(result);
}

/*****************************************************************************/

STATIC LONG
LibNull(VOID)
{
	return(NULL);
}

/*****************************************************************************/


STATIC const APTR LibVectors[] =
{
	LibOpen,
	LibClose,
	LibExpunge,
	LibNull,
	LT_LevelWidth,
	LT_DeleteHandle,
	LT_CreateHandle,
	LT_CreateHandleTagList,
	LT_Rebuild,
	LT_HandleInput,
	LT_BeginRefresh,
	LT_EndRefresh,
	LT_GetAttributesA,
	LT_SetAttributesA,
	LT_AddA,
	LT_NewA,
	LT_EndGroup,
	LT_LayoutA,
	LT_LayoutMenusA,
	LibNull,	/* There used to be a FRACTION_KIND support routine here. */
	LibNull,	/* There used to be a FRACTION_KIND support routine here. */
	LibNull,	/* There used to be a FRACTION_KIND support routine here. */
	LT_LabelWidth,
	LT_LabelChars,
	LT_LockWindow,
	LT_UnlockWindow,
	LT_DeleteWindowLock,
	LT_ShowWindow,
	LT_Activate,
	LT_PressButton,
	LT_GetCode,
	LT_GetIMsg,
	LT_ReplyIMsg,
	LT_BuildA,
	LT_RebuildTagList,
	LT_UpdateStrings,

#ifdef DO_MENUS
	LT_DisposeMenu,
	LT_NewMenuTemplate,
	LT_NewMenuTagList,
	LT_MenuControlTagList,
	LT_GetMenuItem,
	LT_FindMenuCommand,
#else
	LibNull,
	LibNull,
	LibNull,
	LibNull,
	LibNull,
	LibNull,
#endif /* DO_MENUS */

	LT_NewLevelWidth,
	LT_Refresh,
	LT_CatchUpRefresh,
	LT_GetWindowUserData,

	(APTR)-1
};

/*****************************************************************************/

struct LibInitData
{
	ULONG	lid_DataSize;
	APTR	lid_Table;
	APTR	lid_Data;
	APTR	lid_InitRoutine;
};

const struct LibInitData LibInitTab =
{
	sizeof(struct GTLayoutBase),
	LibVectors,
	NULL,
	LibInit
};

/*****************************************************************************/

#ifdef CPU_ANY
#define CPU_TYPE "Generic 68k version"
#else
#define CPU_TYPE "MC68020/030/040/060 version"
#endif	/* CPU_ANY */

const STRPTR VersTag = "$VER: " VERS " (" DATE ") " CPU_TYPE;

/*****************************************************************************/

const struct Resident RomTag =
{
	RTC_MATCHWORD,
	&RomTag,
	&RomTag + 1,
	RTF_AUTOINIT,
	VERSION,
	NT_LIBRARY,
	0,
	"gtlayout.library",
	VSTRING,
	&LibInitTab
};
#else /* AROS */
