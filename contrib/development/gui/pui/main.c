#include <OSIncludes.h>

#ifndef _AROS
#pragma header
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pui.h"

extern ULONG HookEntry();

static void Cleanup(char *msg)
{
    WORD rc;

    if (msg)
    {
        printf("PUI: %s\n",msg);
        rc = RETURN_WARN;
    } else {
        rc = RETURN_OK;
    }

    KillClasses();
    CloseLibs();

    exit(rc);
}

static void Init(void)
{
    static struct PUI_RealFrameInfo realframedata[] =
    {
        {0,0,0,0},                /* none */
        {1,1,1,1},                /* button */
        {2,2,2,2},                /* string */
        {3,3,3,3},                /* drop */
        {1,1,1,1}                /* mono */
    };
    
    static struct PUI_FrameInfo framedata[] = 
    {
        {FRAMETYPE_NONE},                                                                                /* none */
        {FRAMETYPE_BUTTON,SHINEPEN,SHADOWPEN,BACKGROUNDPEN,FALSE,0,0,0,0,2,2,1,1},    /* Button */
        {FRAMETYPE_STRING,SHINEPEN,SHADOWPEN,BACKGROUNDPEN,FALSE,0,0,0,0,2,2,1,1},    /* String */
        {FRAMETYPE_STRING,SHINEPEN,SHADOWPEN,BACKGROUNDPEN,TRUE ,0,0,0,0,4,4,4,4},    /* Group */
        {FRAMETYPE_BUTTON,SHINEPEN,SHADOWPEN,BACKGROUNDPEN,FALSE,0,0,0,0,1,1,1,1}, /* ImageButton */
        {FRAMETYPE_BUTTON,SHINEPEN,SHADOWPEN,BACKGROUNDPEN,FALSE,0,0,0,0,2,2,1,1}, /* TextImageButton */
        {FRAMETYPE_BUTTON,SHINEPEN,SHADOWPEN,BACKGROUNDPEN,TRUE ,0,0,0,0,2,2,1,1},    /* Text */
        {FRAMETYPE_MONO,SHINEPEN,SHADOWPEN,SHADOWPEN,TRUE,0,0,0,0,2,2,2,2},            /* Cycle */
        {FRAMETYPE_BUTTON,SHINEPEN,SHADOWPEN,BACKGROUNDPEN,FALSE,0,0,0,0,2,2,1,1}    /* Prop */
    };

    WORD i,a;
    
    group_defaultspacingx = 4;
    group_defaultspacingy = 4;
    
    window_defaultspacingl = 4;
    window_defaultspacingr = 4;
    window_defaultspacingo = 4;
    window_defaultspacingu = 4;
    
    grouptitles_center = FALSE;
    grouptitles_3d = TRUE;

    popup_usewindow = FALSE;

    image_text_spacing = 4;

    /* Init Frames */
        
    memcpy((APTR)frameinfo,framedata,sizeof(framedata));
    memcpy((APTR)realframeinfo,realframedata,sizeof(realframedata));
    
    for(i=0;i<NUM_FRAMES;i++)
    {
        a = frameinfo[i].type;
        
        frameinfo[i].borderleft = realframeinfo[a].borderleft;
        frameinfo[i].borderright = realframeinfo[a].borderright;
        frameinfo[i].bordertop = realframeinfo[a].bordertop;
        frameinfo[i].borderbottom = realframeinfo[a].borderbottom;
    }

    InitRastPort(&temprp);
    InitSemaphore(&temprpsem);
}

static APTR OpenLib(char *name,LONG ver)
{
    struct Library *lib;

    if (!(lib = OpenLibrary(name,ver)))
    {
        sprintf(s,"Can't open %s V%ld!",name,ver);
        Cleanup(s);
    }
    
    return lib;
}

static void OpenLibs(void)
{
    IntuitionBase = OpenLib("intuition.library",36);
    GfxBase          = OpenLib("graphics.library",36);
    UtilityBase      = OpenLib("utility.library",36);
    DiskfontBase  = OpenLib("diskfont.library",36);
    LayersBase      = OpenLib("layers.library",36);
}

void CloseLibs(void)
{
    if (IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);
    if (GfxBase) CloseLibrary((struct Library *)GfxBase);
    if (UtilityBase) CloseLibrary(UtilityBase);
    if (DiskfontBase) CloseLibrary(DiskfontBase);
    if (LayersBase)    CloseLibrary(LayersBase);
}

struct IClass *MakeCl(ClassID id,ClassID superid,struct IClass *superclass,ULONG size,ULONG flags,WORD clid,APTR dispatcher,char *name)
{
    struct IClass *rc;
    
    if (!(rc = MakeClass(id,superid,superclass,size,flags)))
    {
        sprintf(s,"Can't create %s!",name);
        Cleanup(s);
    }

    rc->cl_Dispatcher.h_Entry = HookEntry;
    rc->cl_Dispatcher.h_SubEntry = (HOOKFUNC)dispatcher;

    classes[clid] = rc;

    return rc;
}

static void MakeClasses(void)
{
    areaclass        = MakeCl(0,"gadgetclass",0,AreaInstanceSize(),0,PUIC_Area,AreaDispatcher,"pui_areaclass");
    groupclass        = MakeCl(0,0,areaclass,GroupInstanceSize(),0,PUIC_Group,GroupDispatcher,"pui_groupclass");
    buttonclass        = MakeCl(0,0,areaclass,ButtonInstanceSize(),0,PUIC_Button,ButtonDispatcher,"pui_buttonclass");
    stringclass        = MakeCl(0,0,areaclass,StringInstanceSize(),0,PUIC_String,StringDispatcher,"pui_stringclass");
    labelclass        = MakeCl(0,0,buttonclass,LabelInstanceSize(),0,PUIC_Label,LabelDispatcher,"pui_labelclass");
    sysimageclass    = MakeCl(0,0,areaclass,SysImageInstanceSize(),0,PUIC_SysImage,SysImageDispatcher,"pui_sysimageclass");
    coolimageclass    = MakeCl(0,0,areaclass,CoolImageInstanceSize(),0,PUIC_CoolImage,CoolImageDispatcher,"pui_coolimageclass");
    cycleclass        = MakeCl(0,0,areaclass,CycleInstanceSize(),0,PUIC_Cycle,CycleDispatcher,"pui_cycleclass");
    propclass        = MakeCl(0,0,areaclass,PropInstanceSize(),0,PUIC_Prop,PropDispatcher,"pui_prop");
}

void KillClasses(void)
{
    WORD i;

    for(i = PUIC_NumClasses - 1;i >= 0;i--)
    {
        if (classes[i]) FreeClass(classes[i]);
    };
}

void Test(void)
{
    static LONG MapString1ToString2[] =
    {
        STRINGA_TextVal,STRINGA_TextVal,
        TAG_DONE
    };
    static LONG MapString2ToString1[] =
    {
        STRINGA_TextVal,STRINGA_TextVal,
        TAG_DONE
    };
    static char *cyclelabels[] =
    {
        "1",
        "2",
        "3",
        "4",
        "5",
        "6",
        "7",
        "8",
        "9",
        "10",
        "11",
        "12",
        "13",
        "14",
        "15",
        "16",
        "17",
        "18",
        "19",
        "20",
        "21",
        "22",
        "23",
        "24",
        "25",
        "26",
        "27",
        "28",
        "29",
        "30",
        "31",
        "32",
        "33",
        "34",
        "35",
        "36",
        "37",
        "38",
        "39",
        "40",
        "41",
        "42",
        "43",
        "44",
        "45",
        "46",
        "47",
        "48",
        "49",
        "50",
        "51",
        "52",
        "53",
        "54",
        "55",
        "56",
        "57",
        "58",
        "59",
        "60",
        0
    };

    struct Screen *scr;
    struct Window *win;
    APTR vi;
    Object *obj,*string1,*string2;
    struct Gadget *gad,*gad2,*page;
    struct IntuiMessage *msg;
    WORD gadid;
    BOOL quitme;
    
    if ((scr = LockPubScreen(0)))
    {
//        puts("Screen locking OK!");
        
        if ((vi = PUI_GetVisualInfoA(scr,0)))
        {
//            puts("GetVisualInfo OK!");

            if ((obj = RootObjectH(vi),
                                Child,VGroup,
                                    GroupFrame,
                                    Child,PropObject,
                                        PropFrame,
                                        GA_ID,777,
                                        PUIA_Prop_Horiz,TRUE,
                                        PUIA_Prop_Entries,10,
                                        PUIA_Prop_Visible,1,
                                        End,
                                    Child,HGroup,
//                                        PUIA_Group_SameWidth,TRUE,
                                        Child,ImageButton(10,CoolSaveImage),
                                        Child,ImageButton(11,CoolLoadImage),
                                        Child,ImageButton(12,CoolUseImage),
                                        Child,ImageButton(13,CoolCancelImage),
                                        Child,ImageButton(14,CoolDiskImage),
                                        Child,ImageButton(15,CoolMonitorImage),
                                        Child,ImageButton(16,CoolInfoImage),
                                        Child,ImageButton(17,CoolExclamationMarkImage),
                                        Child,ImageButton(18,CoolQuotationMarkImage),
                                        Child,TextButton(0,"hallo"),
                                        End,
//                                    Child,TextImageButton(1,"Hund",CoolLoadImage),
                                    Child,HGroup,
//                                        PUIA_Group_SameSize,TRUE,
                                        Child,ButtonLabel("_Label"),
                                        Child,ButtonObject,ButtonFrame,RelVerify,Immediate,PUIA_Button_Text,"Button",GA_ID,234,End,
                                        Child,ButtonObject,
                                            ButtonFrame,
                                            PUIA_Button_Text,"SuperButton",
//                                            PUIA_FillArea,FALSE,
                                            PUIA_Button_Toggle,TRUE,
                                            PUIA_Button_Image,CoolImageObject,
                                                PUIA_CoolImage_Which,PUIV_CoolImage_Info,
                                                End,
                                            End,
                                        Child,ButtonObject,
                                            ButtonFrame,
                                            PUIA_Button_Text,"SilberTest",
                                            PUIA_Button_Image,SysImageObject,
                                                PUIA_SysImage_Which,PUIV_SysImage_CheckMark,
                                                End,
                                            End,
                                        Child,CheckMark(100,TRUE),
                                        End,
                                    Child,ButtonObject,
                                        ButtonFrame,
                                        PUIA_Button_Text,"_Kasten",
//                                        PUIA_FillArea,FALSE,
                                        PUIA_Button_Image,CoolImageObject,
                                            PUIA_CoolImage_Which,PUIV_CoolImage_Monitor,
                                            End,
                                        End,
    
                                    Child,ButtonObject,ButtonFrame,RelVerify,Immediate,PUIA_Button_Text,"Drei",PUIA_Button_Toggle,TRUE,GA_ID,3,End,
                                    Child,page = (struct Gadget *)HGroup,
                                        GA_ID,111,
                                        GroupFrameTL("Hallo"),
                                        PUIA_Group_PageMode,TRUE,
                                        PUIA_Group_ActivePage,0,
    //                                    PUIA_Group_SameWidth,TRUE,
                                        Child,ButtonObject,ButtonFrame,RelVerify,PUIA_Button_Text,"100000",PUIA_Button_TextStyle,FSF_BOLD,End,
                                        Child,ButtonObject,ButtonFrame,RelVerify,PUIA_Button_Text,"1000",End,
                                        Child,ButtonObject,ButtonFrame,RelVerify,PUIA_Button_Text,"100",End,
#if 0
                                        Child,ButtonObject,
                                            ButtonFrame,
                                            PUIA_HorizAlign,PUIV_Align_Left,
                                            PUIA_Button_Image,CoolImageObject,
                                                PUIA_CoolImage_Which,PUIV_CoolImage_Cancel,
                                                End,
                                            End,
#endif
                                        End,
                                    End,
                                Child,PropObject,
                                    PropFrame,
                                    GA_ID,888,
                                    PUIA_Prop_Entries,100,
                                    PUIA_Prop_Visible,10,
                                    PUIA_Prop_First,50,
                                    End,
#if 1
                               Child,VGroup,
                                    GroupFrameT("SuperTitel"),
                                    PUIA_HorizWeight,0,
                                    PUIA_VertAlign,PUIV_Align_Bottom,
                                    Child,HGroup,
                                        Child,ButtonObject,ButtonFrame,RelVerify,PUIA_Button_Text,"C_ybertest",GA_ID,100,End,
                                        Child,CheckMark(101,FALSE),
                                        End,
                                    Child,string1 = StringObject,GA_ID,456,StringFrame,PUIA_String_Contents,"String-Gadget",ICA_MAP,MapString1ToString2,End,
                                    Child,string2 = StringObject,GA_ID,457,StringFrame,PUIA_String_Contents,"Test-Gadget",ICA_MAP,MapString2ToString1,End,
                                    Child,StringObject,GA_ID,457,StringFrame,PUIA_String_Contents,"3. string-Gadget",ICA_MAP,MapString1ToString2,End,
                                    Child,StringObject,GA_ID,458,StringFrame,PUIA_String_Contents,"4. string-Gadget",ICA_MAP,MapString2ToString1,End,
                                    Child,HGroup,
                                  //      PUIA_Group_SameWidth,TRUE,
                                        Child,VGroup,
                                            Child,ButtonObject,GA_ID,601,ButtonFrame,PUIA_Button_Text,"1.",End,
                                            Child,ButtonObject,ButtonFrame,PUIA_Button_Text,"4",End,
                                            Child,ButtonObject,ButtonFrame,PUIA_Button_Text,"7",End,
                                            End,
                                        Child,VGroup,
                                            Child,ButtonObject,GA_ID,602,ButtonFrame,PUIA_Button_Text,"2",End,
                                            Child,ButtonObject,ButtonFrame,PUIA_Button_Text,"5",End,
                                            Child,ButtonObject,ButtonFrame,PUIA_Button_Text,"8",End,
                                            End,
                                        Child,VGroup,
                                            Child,ButtonObject,GA_ID,603,ButtonFrame,PUIA_Button_Text,"3",End,
                                            Child,ButtonObject,ButtonFrame,PUIA_Button_Text,"6",End,
                                            Child,ButtonObject,ButtonFrame,PUIA_Button_Text,"9*",End,
                                            End,
                                        End,
                                    Child,CycleObject,
                                        ButtonFrame,
                                        PUIA_Cycle_Entries,cyclelabels,
                                        End,
                                    End,
#endif
                                End))
            {
//                puts("RootObject OK!");

/*                SetAttrs(string1,ICA_TARGET,string2,
                                      TAG_DONE);
                SetAttrs(string2,ICA_TARGET,string1,
                                      TAG_DONE);*/

//                ((struct Gadget *)string1)->NextGadget = (struct Gadget *)string2;

                if ((win = OpenWindowTags(0,
                    WA_InnerWidth,PUI_InnerWidth(obj),
                    WA_InnerHeight,PUI_InnerHeight(obj),
                    WA_Title,"PUI Test",
                    WA_Flags,WFLG_CLOSEGADGET|WFLG_DRAGBAR|WFLG_CLOSEGADGET|
                                WFLG_ACTIVATE|WFLG_DEPTHGADGET|WFLG_SIZEGADGET,
                    WA_IDCMP,IDCMP_CLOSEWINDOW|IDCMP_GADGETUP|IDCMP_GADGETDOWN|
                                IDCMP_NEWSIZE|IDCMP_REFRESHWINDOW,
                    WA_SimpleRefresh,TRUE,
                    WA_SizeBBottom,TRUE,
                    WA_BackFill,LAYERS_NOBACKFILL,
                    WA_Gadgets,obj,
                    TAG_DONE)))
                {
		#ifdef _AROS
		    {
		    	/* hack because of font "bug" in AROS gagdet code */
			
		    	struct DrawInfo *dri = GetScreenDrawInfo(win->WScreen);
			
			SetFont(win->RPort, dri->dri_Font);
			if (dri) FreeScreenDrawInfo(scr, dri);
		    }
		#endif
                    PUI_SetWindowLimits(win,obj);

                    gad = (struct Gadget *)obj;
                    ScreenToFront(win->WScreen);

                    quitme = FALSE;
                    while (!quitme)
                    {
                        WaitPort(win->UserPort);
                        while ((msg = PUI_GetIMsg(win->UserPort)))
                        {
                            switch (msg->Class)
                            {
                                case IDCMP_CLOSEWINDOW:
                                    quitme = TRUE;
                                    break;
                                
                                case IDCMP_GADGETDOWN:
                                    printf("GADGETDOWN: %ld\n",msg->Code);
                                    break;
                                
                                case IDCMP_GADGETUP:
                                    printf("GADGETUP: %ld\n",msg->Code);
                                    
                                    gadid = msg->Code;
                                    if (gadid >= 601 && gadid <= 603)
                                    {
                                        SetGadgetAttrs(page,win,0,PUIA_Group_ActivePage,gadid - 601,
                                                                          TAG_DONE);
                                    }
                                    break;
                                    
                                case IDCMP_REFRESHWINDOW:
                                    BeginRefresh(win);
                                    EndRefresh(win,TRUE);
                                    break;
                            }

                            PUI_ReplyIMsg(msg);
                        }
                    }

                    RemoveGadget(win,(struct Gadget *)obj);
                    CloseWindow(win);
                }
                
                DisposeObject(obj);
            }
            PUI_FreeVisualInfo(vi);
        }
        
        UnlockPubScreen(0,scr);
    }
}

void main(void)
{
    OpenLibs();
    Init();
    MakeClasses();
    Test();

    Cleanup(0);
}
