/****************************************************************************
*									    *
* Init.c							    970319  *
*									    *
* Open Libs and Screen							    *
*									    *
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <exec/types.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>
#include <utility/tagitem.h>
#ifndef __AROS__
#include <libraries/asl.h>
#endif

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#ifndef __AROS__
#include <proto/asl.h>
#endif
#include <proto/graphics.h>

#include "MyTypes.h"
#include "Init.h"
#include "Clean.h"

#define DefWidth 1152
#define DefHeight 864
extern int32 MyWidth;
extern int32 MyHeight;
extern int32 HalfWidth;
extern int32 HalfHeight;
extern int32 QuaterWidth;
extern int32 QuaterHeight;
       int32 MyDepth;
extern int32 MouseX, MouseY;

fix   ColLight = 1.0;
int32 MaxCol   = 256;

struct IntuitionBase * IntuitionBase = NULL;
struct GfxBase	* GfxBase = NULL;
struct Library	* AslBase = NULL;
pCleanNode	  IntNode = NULL,GfxNode = NULL,AslNode = NULL,
		  ScreenNode = NULL,Window1Node = NULL,Window2Node = NULL,
		  BitMapNode = NULL;
struct Screen	* MyScreen = NULL;
struct Window	* MyWindow1 = NULL,* MyWindow2 = NULL;
struct RastPort * MyRastPort = NULL,* RastPort1 = NULL,* RastPort2 = NULL,
		* InfoRastPort = NULL, * MyTmpRast = NULL,Tmp1Rast,Tmp2Rast,TmpInfoRast;
struct BitMap	* MyBitMap;
uint8		* ColMap = NULL;

struct TagItem MyScreenTags[] =
		{
		    {SA_Width,320},
		    {SA_Height,256},
		    {SA_Depth,8},
		    {SA_Overscan,0},
		    {SA_AutoScroll,TRUE},
		    {SA_DisplayID,0},
		    {TAG_DONE}
		};

struct TagItem MyWD1Tags[] =
		{
		    {WA_Left,0},
		    {WA_Top,0},
		    {WA_Width,320},
		    {WA_Height,64},
		    {WA_CustomScreen,0},
		    {WA_Borderless,TRUE},
		    {TAG_DONE}
		};

struct TagItem MyWD2Tags[] =
		{
		    {WA_Left,0},
		    {WA_Top,64},
		    {WA_Width,320},
		    {WA_Height,192},
		    {WA_CustomScreen,0},
		    {WA_Backdrop,TRUE},
		    {WA_Borderless,TRUE},
		    {WA_IDCMP,IDCMP_MOUSEBUTTONS|IDCMP_MOUSEMOVE|IDCMP_RAWKEY|IDCMP_VANILLAKEY},
		    {WA_ReportMouse,TRUE},
		    {WA_RMBTrap,TRUE},
		    {WA_Activate,TRUE},
		    {TAG_DONE}
		};

UWORD		 pens[]={ ~0 };

int16 InitLibs(void)
{
    IntuitionBase = (struct IntuitionBase *)OpenLibrary((uint8 *)"intuition.library",39);
    if (NULL != IntuitionBase)
    {
	IntNode = EnterCleanNode (CleanUpLib,IntuitionBase,0);
	GfxBase = (struct GfxBase *)OpenLibrary((uint8 *)"graphics.library",39);
	if (NULL != GfxBase)
	{
	    GfxNode = EnterCleanNode (CleanUpLib,GfxBase,0);
#ifndef __AROS__
	    AslBase = OpenLibrary((uint8 *)"asl.library",39);
	    if (NULL != AslBase)
	    {
		AslNode = EnterCleanNode (CleanUpLib,AslBase,0);
#endif
		return 0;
#ifndef __AROS__
	    }
	    else
	    {
		/* What about IntNode ? Why do you remove them anyway ? */
		RemoveCleanNode(GfxNode);
		RemoveCleanNode(AslNode);
		CleanExit("Could not open ASL.library");
	    }
#endif
	}
	else
	    RemoveCleanNode(IntNode);
        CleanExit("Could not open GFX.library!");
    }
    else
	CleanExit("Could not open INT.library!");

    return 0;
}

int16 RequestScreen(void)
{
#ifndef __AROS__
    struct ScreenModeRequester * MyScreenRequester;
    struct TagItem MyScrReqTags[] =
	 {
	     {ASLSM_InitialDisplayWidth,320},
	     {ASLSM_InitialDisplayHeight,256},
	     {ASLSM_InitialDisplayDepth,8},
	     {ASLSM_DoWidth,TRUE},
	     {ASLSM_DoHeight,TRUE},
	     {ASLSM_DoDepth,TRUE},
	     {ASLSM_DoOverscanType,TRUE},
	     {ASLSM_DoAutoScroll,TRUE},
	     {ASLSM_MinWidth,64},
	     {ASLSM_MaxWidth,16387},
	     {ASLSM_MinHeight,64},
	     {ASLSM_MaxHeight,2048},
	     {ASLSM_MinDepth,1},
	     {ASLSM_MaxDepth,24},
	     {TAG_DONE}
	 };

    if ((MyScreenRequester = (struct ScreenModeRequester *)
	AllocAslRequest(ASL_ScreenModeRequest,MyScrReqTags)))
    {
	if (AslRequest(MyScreenRequester, NULL))
	{
	    MyScreenTags[0].ti_Data=((MyScreenRequester->sm_DisplayWidth)>>5)<<5;
	    MyScreenTags[1].ti_Data=MyScreenRequester->sm_DisplayHeight;
	    MyScreenTags[2].ti_Data=MyScreenRequester->sm_DisplayDepth;
	    MyScreenTags[3].ti_Data=MyScreenRequester->sm_OverscanType;
	    MyScreenTags[4].ti_Data=MyScreenRequester->sm_AutoScroll;
	    MyScreenTags[5].ti_Data=MyScreenRequester->sm_DisplayID;

	    MyWD1Tags[2].ti_Data=((MyScreenRequester->sm_DisplayWidth)>>5)<<5;

	    MyWD2Tags[2].ti_Data=((MyScreenRequester->sm_DisplayWidth)>>5)<<5;
	    MyWD2Tags[3].ti_Data=MyScreenRequester->sm_DisplayHeight-64;

	    MyWidth=((MyScreenRequester->sm_DisplayWidth)>>5)<<5;
	    printf("%ld\n",MyWidth);
	    MyHeight=MyScreenRequester->sm_DisplayHeight-64;
	    HalfWidth=MyWidth/2;
	    HalfHeight=MyHeight/2;
	    QuaterWidth=MyWidth/4;
	    QuaterHeight=MyHeight/4;
	    MouseX = HalfWidth;
	    MouseY = HalfHeight;

	    MyDepth=MyScreenRequester->sm_DisplayDepth;
	    MaxCol=1<<MyDepth;
	    ColLight=(fix)MaxCol/256.0;
	}
	else
	{
	    CleanExit("Screen Mode Request Failed\n");
	}
	FreeAslRequest(MyScreenRequester);
    }
    else
    {
	CleanExit("No Asl Screen Mode Request\n");
    }
#else
    MyScreenTags[0].ti_Data=320;//(256>>5)<<5;
    MyScreenTags[1].ti_Data=240;//256+64;
    MyScreenTags[2].ti_Data=8;
    MyScreenTags[3].ti_Data=OSCAN_STANDARD;
    MyScreenTags[4].ti_Data=FALSE;
    MyScreenTags[5].ti_Data=0x00008004;

    MyWD1Tags[2].ti_Data=MyScreenTags[0].ti_Data;

    MyWD2Tags[2].ti_Data=MyScreenTags[0].ti_Data;
    MyWD2Tags[3].ti_Data=MyScreenTags[1].ti_Data-64;

    MyWidth=MyWD1Tags[2].ti_Data;
    printf("Width %ld\n",MyWidth);
    MyHeight=MyWD2Tags[3].ti_Data;
    HalfWidth=MyWidth/2;
    HalfHeight=MyHeight/2;
    QuaterWidth=MyWidth/4;
    QuaterHeight=MyHeight/4;

    MyDepth=8;
    MaxCol=1<<MyDepth;
    ColLight=(fix)MaxCol/256.0;
#endif

    return 0;
}

int16 InitScreen(void)
{
    int16 a;

    RequestScreen();

    if (NULL != (MyScreen = OpenScreenTagList(NULL,MyScreenTags)))
    {
	ScreenNode = EnterCleanNode( CleanUpScreen, MyScreen, 0);

	for (a=0;a<MaxCol;a++)
	{
	    SetRGB32(&MyScreen->ViewPort,a,MyColor.RGB[a][0],
					   MyColor.RGB[a][1],
					   MyColor.RGB[a][2]);
	}

	MyWD1Tags[4].ti_Data = (IPTR)MyScreen;
	MyWD2Tags[4].ti_Data = (IPTR)MyScreen;

	if (NULL != (MyWindow1 = OpenWindowTagList(NULL,MyWD1Tags)))
	{
	    Window1Node = EnterCleanNode( CleanUpWindow, MyWindow1, 0);

	    InfoRastPort = MyWindow1 -> RPort;

	    /* Init Tmp Rast */

	    memcpy(&TmpInfoRast,InfoRastPort,sizeof(TmpInfoRast));

	    TmpInfoRast.Layer=0;

	    if (NULL != (MyBitMap = (struct BitMap *)AllocBitMap(MyWidth,1,MyDepth,0,InfoRastPort->BitMap)))
	    {
		BitMapNode = EnterCleanNode( CleanUpBitMap, MyBitMap, 0);

		TmpInfoRast.BitMap=MyBitMap;

		if (NULL != (MyWindow2 = OpenWindowTagList(NULL,MyWD2Tags)))
		{
		    Window2Node = EnterCleanNode( CleanUpWindow, MyWindow2, 0);

		    MyRastPort = MyWindow2 -> RPort;
		    RastPort1 = MyRastPort; /* Be prepared for double buffering */
		    RastPort2 = MyRastPort;

		    WindowPort = MyWindow2->UserPort;

		    /* Init Tmp Rast */

		    memcpy(&Tmp1Rast,MyRastPort,sizeof(Tmp1Rast));
		    memcpy(&Tmp1Rast,MyRastPort,sizeof(Tmp1Rast));

		    Tmp1Rast.Layer=0;
		    Tmp1Rast.BitMap=MyBitMap;
		    Tmp2Rast.Layer=0;
		    Tmp2Rast.BitMap=MyBitMap;

		    MyTmpRast = &Tmp1Rast;

		    if (NULL != (ColMap = (uint8 *)MemAlloc(MyWidth*(MyHeight+5))))
		    {
			return 0;
		    }
		    else
		    {
			RemoveCleanNode(BitMapNode);
			RemoveCleanNode(Window2Node);
			RemoveCleanNode(Window1Node);
			RemoveCleanNode(ScreenNode);
			CleanExit("Could not allocate ColMap\n");
		    }
		}
		else
		{
		    RemoveCleanNode(BitMapNode);
		    RemoveCleanNode(Window1Node);
		    RemoveCleanNode(ScreenNode);
		    CleanExit("Could not open Game Window\n");
		}
	    }
	    else
	    {
		RemoveCleanNode(Window1Node);
		RemoveCleanNode(ScreenNode);
		CleanExit("Could not open BitMap\n");
	    }
	}
	else
	{
	    RemoveCleanNode(ScreenNode);
	    CleanExit("Could not open Info Window\n");
	}
    }
    else
	CleanExit("Could not open Screen\n");

    return 0;
}

void CleanupScreen(void)
{
    if (NULL != Window2Node)
	RemoveCleanNode(Window2Node);
    if (NULL != BitMapNode)
	RemoveCleanNode(BitMapNode);
    if (NULL != Window1Node)
	RemoveCleanNode(Window1Node);
    if (NULL != ScreenNode)
	RemoveCleanNode(ScreenNode);
}

void CleanupLibs(void)
{
#ifndef __AROS__
    if (NULL != AslNode)
	RemoveCleanNode(AslNode);
#endif
    if (NULL != GfxNode)
	RemoveCleanNode(GfxNode);
    if (NULL != IntNode)
	RemoveCleanNode(IntNode);
}
