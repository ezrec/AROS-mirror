/*
 *  Source generated with GadToolsBox V1.4
 *  which is (c) Copyright 1991,92 Jaba Development
 */

/* This is an example source which show you the initialization and handling of
   almost every kind of the new gadget types under OS 2.xx.

   Thanks to Jaba development for the GadToolsBox V 1.4 which is the GUI first
   program if designing a new interface.

   All gadgets and menuhandling will be performed by using the gadtools.library.

   No lowlevel function calls to intuition.library will be needed.

   This code is freely distributable and released as public domain.

   You use this at yourown risk. No guaranty is made for the function and anything
   else.

   But I think, it should properly run. I took care about all enforcer hits and
   had the commodores reference manual on my desk while I wrote this.

*/

// INCLUDES
    #include <exec/types.h>
    #include <intuition/intuition.h>
    #include <intuition/classes.h>
    #include <intuition/classusr.h>
    #include <intuition/imageclass.h>
    #include <intuition/gadgetclass.h>
    #include <libraries/gadtools.h>
    #include <graphics/displayinfo.h>
    #include <graphics/gfxbase.h>
    #include <clib/exec_protos.h>
    #include <clib/intuition_protos.h>
    #include <clib/gadtools_protos.h>
    #include <clib/graphics_protos.h>
    #include <clib/utility_protos.h>
    #include <string.h>

#include <aros/oldprograms.h>
#include <proto/gadtools.h>
#include <clib/alib_protos.h>
#include <exec/memory.h>
#include <stdio.h>
#include <stdlib.h>

// DEFINITIONS

    #define VERSION "$VER: GadgetTest 0.01 just a simple example"

    #define GD_Button_Gadget                       0
    #define GD_CheckBox_Gadget                     1
    #define GD_Integer_Gadget                      2
    #define GD_ListView_Gadget                     3
    #define GD_MX_Gadget                           4
    #define GD_Cycle_Gadget                        5
    #define GD_Palette_Gadget                      6
    #define GD_Scroller_Gadget                     7
    #define GD_Slider_Gadget                       8
    #define GD_String_Gadget                       9
    #define GD_Text_Gadget                         10

    #define Test__CNT                              11

    #define IB IntuitionBase
    #define GTB Library
    #define GFX GfxBase
    // These defines saves a lot of typing time

// PROTOTYPES

    static UWORD ComputeX(UWORD value);

        // this function computes the width of gadgets and offsets dependent to the
        // currently used wbench font.

    static UWORD ComputeY(UWORD value);

        // this function computes the height of gadgets and offsets dependent to the
        // currently used wbench font.

    static VOID  ComputeFont(UWORD width,UWORD height);

        // this function initializes some needed variables and structures for the
        // font independentness

    LONG SetupScreen(VOID);

        // This function locks the wbench screen and initializes a visualinfo
        // structure which is needed by creating gadgets, bevelboxes and menus

    VOID CloseDownScreen(VOID);

        // This function frees all allocated resources, closes all opened
        // libraries.

    VOID Test_Render(VOID);

        // This function draws the bevelbox and call the refreshing routins for
        // the window. This function will be called each time we got a refresh
        // window idcmp message.

    LONG OpenTest_Window(VOID);

        // This function creates all gadgets and menus and opens the window.
        // This functions must be called after the SetupScreen() function.

    VOID CloseTest_Window(VOID);

        // This function closes our window and frees all gadget and menu
        // structures that had been allocated from the gadtools.library

    int main(int argc,char **argv);

        // The main function starts all actions. The program starts and
        // exits from this function.

    VOID HandleIDCMP(VOID);

        // This function receives the idcmp messages for our window and
        // does all other actions.


// STRUCTS & VARS
    struct IB              *IB                  = NULL;
    struct GTB             *GadToolsBase        = NULL;
    struct GFX             *GFX                 = NULL;
    struct Screen          *Scr                 = NULL;
    APTR                    VisualInfo          = NULL;
    struct Window          *Test_Wnd            = NULL;
    struct Gadget          *Test_GList          = NULL;
    struct Menu            *Test_Menus          = NULL;
    struct Gadget          *Test_Gadgets[11];
    struct TextAttr        *Font, Attr;
    struct List             LV_List;
    struct Remember        *RememberKey         = NULL;

    UWORD                   Test_Left           = 135;
    UWORD                   Test_Top            = 11;
    UWORD                   Test_Width          = 569;
    UWORD                   Test_Height         = 159;

    UBYTE                  *Test_Wdt = (UBYTE *)"Work Window";

    UWORD                   FontX, FontY;
    UWORD                   OffX, OffY;


    UBYTE         *MX_Gadget0Labels[] =
    {  // These are the labels for the MX gadget
        (UBYTE *)"MX....: 1",
        (UBYTE *)"2",
        (UBYTE *)"3",
        NULL
    };

    UBYTE         *Cycle_Gadget0Labels[] =
    {  // These are the labels for the Cycle gadget
        (UBYTE *)"FIRST",
        (UBYTE *)"SECOND",
        (UBYTE *)"THIRD",
        NULL
    };

    UBYTE         *ListView_Gadget0Labels[]=
    {  // these are the the node names for the Listview gadget
        "First Node"  ,
        "Second Node" ,
        "Third Node"
    };

    struct NewMenu Test_NewMenu[] =
    {  // this is the menuitem
        { NM_TITLE, (STRPTR)"Project", NULL, 0, 0L, NULL },
        { NM_ITEM, (STRPTR)"Quit", (STRPTR)"Q", 0, 0L, NULL },
        { NM_END, NULL, NULL, 0, 0L, NULL }
    };

    UWORD Test_GTypes[] =
    { // gadget types
        BUTTON_KIND,
        CHECKBOX_KIND,
        INTEGER_KIND,
        LISTVIEW_KIND,
        MX_KIND,
        CYCLE_KIND,
        PALETTE_KIND,
        SCROLLER_KIND,
        SLIDER_KIND,
        STRING_KIND,
        TEXT_KIND
    };

    struct NewGadget Test_NGad[] =
    { // structures for allocating gadgets
        { 91, 73, 202, 13, (UBYTE *)"BUTTON", NULL, GD_Button_Gadget, PLACETEXT_IN ,NULL, NULL },
        { 91, 108, 26, 11, (UBYTE *)"CHECKBOX:", NULL, GD_CheckBox_Gadget, PLACETEXT_LEFT ,NULL, NULL },
        { 379, 125, 177, 13, (UBYTE *)"INTEGER.:", NULL, GD_Integer_Gadget, PLACETEXT_LEFT ,NULL, NULL },
        { 91, 4, 202, 72, (UBYTE *)"LISTVIEW:", NULL, GD_ListView_Gadget, PLACETEXT_LEFT ,NULL, NULL },
        { 273, 111, 17, 9, NULL, NULL, GD_MX_Gadget, PLACETEXT_LEFT ,NULL, NULL },
        { 91, 91, 202, 13, (UBYTE *)"CYCLE...:", NULL, GD_Cycle_Gadget, PLACETEXT_LEFT ,NULL, NULL },
        { 379, 4, 180, 65, (UBYTE *)"PALETTE.:", NULL, GD_Palette_Gadget, PLACETEXT_LEFT ,NULL, NULL },
        { 379, 73, 178, 13, (UBYTE *)"SCROLLER:", NULL, GD_Scroller_Gadget, PLACETEXT_LEFT ,NULL, NULL },
        { 379, 91, 177, 13, (UBYTE *)"SLIDER..:", NULL, GD_Slider_Gadget, PLACETEXT_LEFT ,NULL, NULL },
        { 379, 108, 177, 13, (UBYTE *)"STRING..:", NULL, GD_String_Gadget, PLACETEXT_LEFT ,NULL, NULL },
        { 93, 141, 459, 13, (UBYTE *)"TEXT....:", NULL, GD_Text_Gadget, PLACETEXT_LEFT ,NULL, NULL }
    };

    ULONG *Test_GTags[] =
    { // the tags for the gadget creation
        (ULONG *)(TAG_DONE),

        (ULONG *)(GTCB_Checked),
        (ULONG *)TRUE,
        (ULONG *)(TAG_DONE),

        (ULONG *)(GTIN_Number),
        (ULONG *) 0,
        (ULONG *)(GTIN_MaxChars),
        (ULONG *)6,
        (ULONG *)(TAG_DONE),

        (ULONG *)(GTLV_ShowSelected),
        (ULONG *)NULL,
        (ULONG *)(GTLV_ScrollWidth),
        (ULONG *)20,
        (ULONG *)(LAYOUTA_Spacing),
        (ULONG *)2,
        (ULONG *)(TAG_DONE),

        (ULONG *)(GTMX_Labels),
        (ULONG *)&MX_Gadget0Labels[ 0 ],
        (ULONG *)(TAG_DONE),

        (ULONG *)(GTCY_Labels),
        (ULONG *)&Cycle_Gadget0Labels[ 0 ],
        (ULONG *)(TAG_DONE),

        (ULONG *)(GTPA_Depth),
        (ULONG *)2,
        (ULONG *)(GTPA_IndicatorWidth),
        (ULONG *)50,
        (ULONG *)(TAG_DONE),

        (ULONG *)(GTSC_Total),
        (ULONG *)10,
        (ULONG *)(GTSC_Arrows),
        (ULONG *)20,
        (ULONG *)(PGA_Freedom),
        (ULONG *)LORIENT_HORIZ,
        (ULONG *)(GA_RelVerify),
        (ULONG *)TRUE,
        (ULONG *)(TAG_DONE),

        (ULONG *)(PGA_Freedom),
        (ULONG *)LORIENT_HORIZ,
        (ULONG *)(GA_RelVerify),
        (ULONG *)TRUE,
        (ULONG *)(TAG_DONE),

        (ULONG *)(GTST_MaxChars),
        (ULONG *)256,
        (ULONG *)(TAG_DONE),

        (ULONG *)(GTTX_Text),
        (ULONG *)"Click a Gadget or use menu Quit to exit...",
        (ULONG *)(GTTX_Border),
        (ULONG *)TRUE,
        (ULONG *)(TAG_DONE)
    };

// FUNCTIONS

    static UWORD ComputeX( UWORD value )
    { // this function computes the x value dependent to your system font
        return(( UWORD )(( FontX * value ) / 8 ));
    }

    static UWORD ComputeY( UWORD value )
    { // this function computes the y value dependent to your system font
        return(( UWORD )(( FontY * value ) / 8 ));
    }

    static void ComputeFont( UWORD width, UWORD height )
    { // this function computes the width and height values dependent to your
      // system font and initializes the Font and Attr vars.

        Font = &Attr;
        Font->ta_Name = (STRPTR)GfxBase->DefaultFont->tf_Message.mn_Node.ln_Name;
        Font->ta_YSize = FontY = GfxBase->DefaultFont->tf_YSize;
        FontX = GfxBase->DefaultFont->tf_XSize;

        OffX = Scr->WBorLeft;
        OffY = Scr->RastPort.TxHeight + Scr->WBorTop + 1;

        if ( width && height )
        {
            if (( ComputeX( width ) + OffX + Scr->WBorRight ) > Scr->Width )
                goto UseTopaz;
            if (( ComputeY( height ) + OffY + Scr->WBorBottom ) > Scr->Height )
                goto UseTopaz;
        }
        return;

        UseTopaz:

        Font->ta_Name = (STRPTR)"topaz.font";
        FontX = FontY = Font->ta_YSize = 8;
    }

    LONG SetupScreen( VOID )
    { // this alloctes a lock for the current default public screen
        if ( ! ( Scr = LockPubScreen( NULL ))) return( 1L );
        ComputeFont( 0, 0 );
        if ( ! ( VisualInfo = GetVisualInfo( Scr, TAG_DONE ))) return( 2L );
        return( 0L );
    }

    VOID CloseDownScreen( VOID )
    { // this frees all allocated resources
        if ( VisualInfo )
        {
            FreeVisualInfo( VisualInfo );
            VisualInfo = NULL;
        }

        if (IB) CloseLibrary((struct Library *)IB);
        if (GadToolsBase) CloseLibrary((struct Library *)GadToolsBase);
        if (GFX) CloseLibrary((struct Library *)GFX);
    }

    void Test_Render( void )
    { // this refreshes the window and the gadgets
        ComputeFont( Test_Width, Test_Height );

        DrawBevelBox( Test_Wnd->RPort, OffX + ComputeX( 3 ),
            OffY + ComputeY( 2 ),
            ComputeX( 563 ),
            ComputeY( 155 ),
            GT_VisualInfo, VisualInfo, TAG_DONE );
    }

    LONG OpenTest_Window( void )
    { // this creates the gadgets and opens the window
        struct NewGadget     ng;
        struct Gadget       *g;
        struct Node         *nd;
        UWORD                lc, tc;
        UWORD                wleft = Test_Left, wtop = Test_Top, ww, wh;

        ComputeFont( Test_Width, Test_Height );

        ww = ComputeX( Test_Width );
        wh = ComputeY( Test_Height );

        if (( wleft + ww + OffX + Scr->WBorRight ) > Scr->Width ) wleft = Scr->Width - ww;
        if (( wtop + wh + OffY + Scr->WBorBottom ) > Scr->Height ) wtop = Scr->Height - wh;

        NewList(&LV_List); // ask Exec to clear out our list

        if (!(nd=(struct Node *)AllocRemember(&RememberKey,sizeof(struct Node),MEMF_CLEAR|MEMF_PUBLIC)))
            return(100);

        nd->ln_Name = ListView_Gadget0Labels[0];
        nd->ln_Type = NT_UNKNOWN;

        AddTail(&LV_List,nd);

        if (!(nd=(struct Node *)AllocRemember(&RememberKey,sizeof(struct Node),MEMF_CLEAR|MEMF_PUBLIC)))
            return(101);

        nd->ln_Name = ListView_Gadget0Labels[1];
        nd->ln_Type = NT_UNKNOWN;

        AddTail(&LV_List,nd);

        if (!(nd=(struct Node *)AllocRemember(&RememberKey,sizeof(struct Node),MEMF_CLEAR|MEMF_PUBLIC)))
            return(102);

        nd->ln_Name = ListView_Gadget0Labels[2];
        nd->ln_Type = NT_UNKNOWN;

        AddTail(&LV_List,nd);


        if ( ! ( g = CreateContext( &Test_GList ))) return( 20L );


        for( lc = 0, tc = 0; lc < Test__CNT; lc++ )
        {

            CopyMem((char * )&Test_NGad[ lc ], (char * )&ng, (long)sizeof( struct NewGadget ));

            ng.ng_VisualInfo = VisualInfo;
            ng.ng_TextAttr   = Font;
            ng.ng_LeftEdge   = OffX + ComputeX( ng.ng_LeftEdge );
            ng.ng_TopEdge    = OffY + ComputeY( ng.ng_TopEdge );
            ng.ng_Width      = ComputeX( ng.ng_Width );
            ng.ng_Height     = ComputeY( ng.ng_Height);

printf("%s - %d\n",__FUNCTION__,__LINE__);

            Test_Gadgets[ lc ] = g = CreateGadgetA((ULONG)Test_GTypes[ lc ], g, &ng, ( struct TagItem * )&Test_GTags[ tc ] );

            while( Test_GTags[ tc ] ) tc += 2;
            tc++;

            if ( NOT g ) return( Test_Gadgets[tc]->GadgetID + 1 );
        }
printf("%s - %d\n",__FUNCTION__,__LINE__);

        if ( ! ( Test_Menus = CreateMenus( Test_NewMenu, GTMN_FrontPen, 0L, TAG_DONE )))
            return( 30L );

        LayoutMenus( Test_Menus, VisualInfo, TAG_DONE );

        if ( ! ( Test_Wnd = OpenWindowTags( NULL,
                        WA_Left,          wleft,
                        WA_Top,           wtop,
                        WA_Width,         ww + OffX + Scr->WBorRight,
                        WA_Height,        wh + OffY + Scr->WBorBottom,

                        WA_IDCMP,
                        BUTTONIDCMP|        CHECKBOXIDCMP|    INTEGERIDCMP|
                        LISTVIEWIDCMP|      MXIDCMP|          CYCLEIDCMP|
                        PALETTEIDCMP|       SCROLLERIDCMP|    ARROWIDCMP|
                        SLIDERIDCMP|        STRINGIDCMP|      IDCMP_MENUPICK|
                        IDCMP_CLOSEWINDOW|  IDCMP_REFRESHWINDOW,

                        WA_Flags,
                        WFLG_DRAGBAR|       WFLG_DEPTHGADGET|   WFLG_CLOSEGADGET|
                        WFLG_SMART_REFRESH,

                        WA_Gadgets,       Test_GList,
                        WA_Title,         Test_Wdt,
                        WA_ScreenTitle,   "GadToolsBox v1.4 © 1991,92 ",
                        WA_PubScreen,     Scr,
                        TAG_DONE )))
            return( 40L );

        SetMenuStrip( Test_Wnd, Test_Menus );

        GT_SetGadgetAttrs(Test_Gadgets[GD_ListView_Gadget],Test_Wnd,NULL,
            GTLV_Labels,&LV_List,
            TAG_DONE);

        GT_RefreshWindow( Test_Wnd, NULL );
        Test_Render();

        UnlockPubScreen(NULL,Scr);
        // The window is opened so we doesn`t need the lock anymore

        return( 0L );
    }

    void CloseTest_Window( void )
    { // this closes the test window and frees some structures
        struct Node *nd;

        if ( Test_Menus      )
        {
            ClearMenuStrip( Test_Wnd );
            FreeMenus( Test_Menus );
            Test_Menus = NULL;
        }

        if ( Test_Wnd        )
        {
            CloseWindow( Test_Wnd );
            Test_Wnd = NULL;
        }

        if ( Test_GList      )
        {
            FreeGadgets( Test_GList );
            Test_GList = NULL;
        }

        Forbid();
        while ((nd=RemHead(&LV_List)));
        Permit();
        FreeRemember(&RememberKey,TRUE);
    }

    int main(int argc,char **argv)
    {

        LONG Error=0L;   // Error indicator
        __unused BPTR wbout=0L;

        if (!(IB=(struct IB *)OpenLibrary("intuition.library",37L)))
        { // first checks if we `re running on an OS 2.xx machine

            Write(Output(),"Oops you need OS 2.xx to run this program\n",42);
            exit(0);

        }

        if (argc > 1)
        { // we got any more argument
            PutStr(VERSION);
            PutStr("\nThis is just a simple demo program which should show you\n"
                   "how you can create and handle the OS 2.xx gadgets and takes\n"
                   "other arguments than its name\n");
            CloseDownScreen();
            exit(0);
        }

        // now open all other libraries and check the success

        if (!(GadToolsBase=(struct GTB *)OpenLibrary("gadtools.library",37L)))
        {
	    PutStr ("Opening gadtools.library failed\n");
            CloseDownScreen();
            exit(20);
        }

        if (!(GFX=(struct GFX *)OpenLibrary("graphics.library",37L)))
        {
	    PutStr ("Opening graphics.library failed\n");
            CloseDownScreen();
            exit(15);
        }

        // initialize out screen lock and visual info
        Error = SetupScreen();

        switch (Error)
        {
            case 1:
                //  we got no lock for the wbench screen
                PutStr("No lock !\n");
                if (Scr) UnlockPubScreen( NULL, Scr );
                CloseDownScreen();
                break;
            case 2:
                // we got no visual info structure
                PutStr("No VisualInfo !\n");
                if (Scr) UnlockPubScreen( NULL, Scr );
                CloseDownScreen();
                break;
            default:
                // no error !
                break;
        }

        Error = OpenTest_Window();

        switch (Error)
        {
            case 0:
                // no error !
                break;
            case 20:
                // CreateContext() failed
                PutStr("Could not create context !\n");
                CloseTest_Window();
                if (Scr) UnlockPubScreen( NULL, Scr );
                CloseDownScreen();
                exit(5);
                break;
            case 30:
                // no menu
                PutStr("Could not create menu !\n");
                CloseTest_Window();
                if (Scr) UnlockPubScreen( NULL, Scr );
                CloseDownScreen();
                exit(5);
                break;
            case 40:
                // no window
                PutStr("Could not open window !\n");
                CloseTest_Window();
                if (Scr) UnlockPubScreen( NULL, Scr );
                CloseDownScreen();
                exit(5);
                break;
            default:
                // we got any gadget id
                Printf("Error: %ld gadget could not be created\n",Error);
                CloseTest_Window();
                if (Scr) UnlockPubScreen( NULL, Scr );
                CloseDownScreen();
                exit(5);
                break;
        }

        HandleIDCMP();
        CloseTest_Window();
        CloseDownScreen();
        exit(0);
    }

    VOID HandleIDCMP(VOID)
    { // this handles all inputs
        struct IntuiMessage *Msg;     // the received Message
        LONG Class,Code,SelGadgetID = 0;  // vars to copy Message`s contents to
        struct Gadget *SelGadget = 0;     // the selected gadget
        BOOL ENDTEST=FALSE;           // end indicator
        BOOL CHECKGADGET=TRUE;        // whether the checkbox is checked or not
        char *res=NULL;

        // main loop

        while (!(ENDTEST))
        {
            WaitPort(Test_Wnd->UserPort);
            while ((Msg=GT_GetIMsg(Test_Wnd->UserPort)))
            { // as long as we got any message
              // copy all informations and then reply the message

                Class = Msg->Class;
                Code  = Msg->Code;
                if ((Class == IDCMP_GADGETUP) || (Class == IDCMP_GADGETDOWN))
                { // enforcer is watching you !

                    if ((SelGadget = (struct Gadget *)Msg->IAddress))
                        SelGadgetID = SelGadget->GadgetID;
                }
                GT_ReplyIMsg(Msg);

                // reaction

                switch (Class)
                {
                    case IDCMP_MENUPICK:
                        ENDTEST=TRUE;
                        break;
                    case IDCMP_GADGETUP:
                    case IDCMP_GADGETDOWN:
                        res=NULL;
                        switch (SelGadgetID)
                        {

                            case GD_Button_Gadget  :
                                PutStr("You clicked the button gadget\n");
                                break;
                            case GD_CheckBox_Gadget:
                                if (CHECKGADGET) CHECKGADGET=FALSE;
                                else CHECKGADGET=TRUE;
                                Printf("You clicked the CheckBox gadget state: %s\n",
                                CHECKGADGET ? "Checked" : "NOT CHECKED");
                                break;
                            case GD_Integer_Gadget :
                                res=((struct StringInfo *)SelGadget->SpecialInfo)->Buffer;
                                if (res) Printf("You insert >%s< in the Integer gadget\n",res);
                                break;
                            case GD_ListView_Gadget:
                                Printf("You clicked the ListView gadget entry: %s\n",ListView_Gadget0Labels[Code]);
                                break;
                            case GD_MX_Gadget      :
                                Printf("You clicked the MX gadget active: %s\n",MX_Gadget0Labels[Code]);
                                break;
                            case GD_Cycle_Gadget   :
                                Printf("You clicked the Cycle gadget selected: %s\n",Cycle_Gadget0Labels[Code]);
                                break;
                            case GD_Palette_Gadget :
                                Printf("You clicked the Palette gadget colour: %ld\n",Code);
                                break;
                            case GD_Scroller_Gadget:
                                Printf("You clicked the Scroller gadget level: %ld\n",Code);
                                break;
                            case GD_Slider_Gadget  :
                                Printf("You clicked the Slider gadget state: %ld\n",Code);
                                break;
                            case GD_String_Gadget  :
                                res=((struct StringInfo *)SelGadget->SpecialInfo)->Buffer;
                                if (res) Printf("You insert >%s< in the String gadget\n",res);
                                break;
                            default:
                                // never see this
                                break;
                        }
                        // a gadget message
                        break;
                    case IDCMP_CLOSEWINDOW:
                        // user ask us to quit
                        ENDTEST=TRUE;
                        break;
                    case IDCMP_REFRESHWINDOW:
                        // our window needs a refresh
                        Test_Render();
                        break;
                    default:
                        // none of them about
                        break;
                }
            }
        }

        Forbid();

        // first clear out all messages
        while ((Msg=GT_GetIMsg(Test_Wnd->UserPort))) GT_ReplyIMsg(Msg);

        // no more messages will be received
        ModifyIDCMP(Test_Wnd,0L);

        Permit();
    }
