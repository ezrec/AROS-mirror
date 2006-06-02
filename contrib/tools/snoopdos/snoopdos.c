/*
 *              SNOOPDOS.C                                                                                              vi:ts=4
 *
 *      Copyright (c) Eddy Carroll, September 1994.
 *
 *              The mainline, where we co-ordinate everything.
 */

#define MAIN

#ifndef __amigaos4__
#define RAWDOFMT_COPY (void (*))"\x16\xc0\x4e\x75" /* MOVE.B D0,(A3)+ | RTS */
#endif

#include "system.h"
#include "snoopdos.h"

#define OldPrint(s)             Write(Output(), s, strlen(s))
#define OldFPrint(f,s)  Write(f, s, strlen(s))

#if _PROFILE
#include "sprof.h"
#else
#define PROFILE_ON()
#define PROFILE_OFF()
#endif

#define NAME                    "SnoopDosPPC 3.8"
#define DATENAME                "May 2004"
#define DATENUM                 "24.5.2004"

#define MINSTACKSIZE            12288

char Version[]              = "$VER: " NAME " (" DATENUM ")";
char SnoopDosTitle[]    = NAME " \251 Eddy Carroll, " DATENAME ".";
char SnoopDosTitleKey[] = NAME " \251 Eddy Carroll, " DATENAME ". Hotkey=<%s>";
char CommodityTitle[]   = NAME " by Eddy Carroll";

long __stack = MINSTACKSIZE;                    /* Ensure stack size is sufficient      */

static ULONG StartPri;                          /* Start priority of our task           */
static struct StackSwapStruct  StSw;            /* For providing a larger stack         */


struct MsgPort *IoPort;
struct timerequest *TimerRequest;


#ifdef __amigaos4__
struct Library *AslBase;
struct Library *RexxSysBase;
struct Library *DiskfontBase;
struct Library *GadToolsBase;
struct Library *IntuitionBase;
struct Library *LayersBase;
struct Library *GfxBase;
struct Library *IconBase;
struct Library *UtilityBase;
struct Library *KeymapBase;
struct Library *CxBase;
struct Library *WorkbenchBase;
struct Library *AmigaGuideBase;

struct AslIFace *IAsl;
struct RexxSysIFace *IRexxSys;
struct DiskfontIFace *IDiskfont;
struct GadToolsIFace *IGadTools;
struct IntuitionIFace *IIntuition;
struct LayersIFace *ILayers;
struct GraphicsIFace *IGraphics;
struct IconIFace *IIcon;
struct UtilityIFace *IUtility;
struct KeymapIFace *IKeymap;
struct Interface *ICx;
struct CommoditiesIFace *ICommodities;
struct WorkbenchIFace *IWorkbench;
struct AmigaGuideIFace *IAmigaGuide;

#endif
//struct DosLibrary *DOSBase;              /* Need a reference or the autoinit code will build one. THOR */

#ifdef __amigaos4__

struct Library *OpenLib(char *name,long version,struct Interface **iface)
{
	struct Library *base;
	
	if( (base = OpenLibrary(name,version)) )
	{
		if(*iface = GetInterface((struct Library *)(base), "main", 1L, NULL))
		{
			return base;
		}
		CloseLibrary(base);
	}
	return NULL;
}

#endif

/*
 *              Cleanup(errcode)
 *
 *              Frees all resources allocated by the program and exits with
 *              the specified error code
 */
void Cleanup(int errcode)
{
        CleanupHotKey();
        CleanupWindows();
        CleanupAGuide();
        CleanupIcons();
        CleanupLocale();
        CleanupRexxPort();
        CleanupPatches();
        CleanupBuffers();

        if (TimerRequest)               {
                CloseDevice(&TimerRequest->tr_node);
                DeleteIORequest((struct IORequest *)TimerRequest);
        }
        if (IoPort)                     DeleteMsgPort(IoPort);
#ifdef __amigaos4__
        if (IAsl)                    DropInterface((struct Interface*)IAsl);
        if (IRexxSys)                DropInterface((struct Interface*)IRexxSys);
        if (IDiskfont)               DropInterface((struct Interface*)IDiskfont);
        if (IGadTools)               DropInterface((struct Interface*)IGadTools);
        if (IIntuition)              DropInterface((struct Interface*)IIntuition);
        if (ILayers)                 DropInterface((struct Interface*)ILayers);
        if (IGraphics)               DropInterface((struct Interface*)IGraphics);
        if (IIcon)                   DropInterface((struct Interface*)IIcon);
        if (IUtility)                DropInterface((struct Interface*)IUtility);
        if (IKeymap)                 DropInterface((struct Interface*)IKeymap);
        if (ICommodities)            DropInterface((struct Interface*)ICommodities);
#endif

        if (AslBase)                    CloseLibrary(AslBase);
        if (RexxSysBase)                CloseLibrary(RexxSysBase);
        if (DiskfontBase)               CloseLibrary(DiskfontBase);
        if (GadToolsBase)               CloseLibrary(GadToolsBase);
        if (IntuitionBase)              CloseLibrary(IntuitionBase);
        if (LayersBase)                 CloseLibrary(LayersBase);
        if (GfxBase)                    CloseLibrary(GfxBase);
        if (IconBase)                   CloseLibrary(IconBase);
        if (UtilityBase)                CloseLibrary(UtilityBase);
        if (KeymapBase)                 CloseLibrary(KeymapBase);
        if (CxBase)                     CloseLibrary(CxBase);

        /*
         *              Restore our requester status and task priority before we
         *              exit, in case we were run from a CLI prompt
         */
        *TaskWindowPtr = SaveWindowPtr;
        SetTaskPri(FindTask(NULL), StartPri);

        if (StSw.stk_Lower) {
                StackSwap(&StSw);
                FreeMem(StSw.stk_Lower,MINSTACKSIZE+8);
        }

        exit(errcode);
}

/*
 *              OpenLibs()
 *
 *              Opens all the main libraries we'll need. We must have intuition,
 *              graphics, and gadtools, but everything else we can manage without.
 *
 *              dos.library and exec.library will already be open at this point.
 *
 */
void OpenLibs(void)
{
        APTR oldwinptr = *TaskWindowPtr;

#ifndef __amigaos4__
        if (SysBase->LibNode.lib_Version < 37) {
                if (WBenchMsg) {
                        /*
                         *              Open a text window to display error message in.
                         */
                        BPTR file = Open("CON:0/0/520/80/" NAME, MODE_NEWFILE);


                        if (file) {
                                char ch;

                                OldFPrint(file,
                                                  "\nSorry, " NAME " requires Workbench 2.04 or above."
                                                  "\n\nPress RETURN to close this window. ");
                                Read(file, &ch, 1);
                                Close(file);
                        }
                } else {
                        /*
                         *              Just print it in the CLI
                         */
                        OldPrint("Sorry, " NAME " requires Workbench 2.04 or above.\n");
                }
                Cleanup(50);
        }
#endif
#ifdef __amigaos4__
        IntuitionBase = (void *)OpenLib("intuition.library", 37,(struct Interface **)&IIntuition);
        GfxBase       = (void *)OpenLib("graphics.library",  37,(struct Interface **)&IGraphics);
        LayersBase        = OpenLib("layers.library",          37,(struct Interface **)&ILayers);
        GadToolsBase  = OpenLib("gadtools.library",    37,(struct Interface **)&IGadTools);
        IconBase      = OpenLib("icon.library",        0,(struct Interface **)&IIcon);
        RexxSysBase   = (void *)OpenLib("rexxsyslib.library",  0,(struct Interface **)&IRexxSys);
        UtilityBase   = OpenLib("utility.library",     0,(struct Interface **)&IUtility);
        KeymapBase        = OpenLib("keymap.library",          37,(struct Interface **)&IKeymap);
        CxBase= OpenLib("commodities.library",          37,(struct Interface **)&ICommodities);
#else
        IntuitionBase = (void *)OpenLibrary("intuition.library", 37);
        GfxBase       = (void *)OpenLibrary("graphics.library",  37);
        LayersBase        = OpenLibrary("layers.library",          37);
        GadToolsBase  = OpenLibrary("gadtools.library",    37);
        IconBase      = OpenLibrary("icon.library",        0);
        RexxSysBase   = (void *)OpenLibrary("rexxsyslib.library",  0);
        UtilityBase   = OpenLibrary("utility.library",     0);
        KeymapBase        = OpenLibrary("keymap.library",          37);
#endif
        /*
         *              We don't need diskfont.library to run, but just in case we're
         *              run after booting with no startup-sequence, we disable AmigaDOS
         *              requesters so that diskfont.library doesn't ask us to insert
         *              volume ENV:
         */
        *TaskWindowPtr = (APTR)-1;
#ifdef __amigaos4__
        DiskfontBase   = OpenLib("diskfont.library",    0,(struct Interface **)&IDiskfont);
#else
        DiskfontBase   = OpenLibrary("diskfont.library",    0);
#endif
        *TaskWindowPtr = oldwinptr;
#ifndef __amigaos4__
        CxBase             = OpenLibrary("commodities.library", 37);
#endif
        if (!IntuitionBase || !GfxBase || !LayersBase) {
                OldPrint("Couldn't open intuition, graphics or layers libraries!\n");
                Cleanup(50);
        }
        if (!GadToolsBase || !UtilityBase || !KeymapBase) {
                OldPrint("Couldn't open gadtools, utilities or keymap library!\n");
                Cleanup(50);
        }

        IoPort        = CreateMsgPort();
        if (IoPort == NULL) {
                OldPrint("Couldn't create a message port for the timer.device!\n");
                Cleanup(50);
        }

        TimerRequest    = (struct timerequest *)CreateIORequest(IoPort,sizeof(struct timerequest));
        if (IoPort == NULL) {
                OldPrint("Couldn't create an IORequest for the timer.device!\n");
                Cleanup(50);
        }

        if (OpenDevice("timer.device",UNIT_VBLANK,(struct IORequest*)TimerRequest,0)) {
                DeleteIORequest((struct IORequest *)TimerRequest);
                TimerRequest = NULL;
                OldPrint("Couldn't open the timer.device!\n");
                Cleanup(50);
        }

}

/*
 *              My own version of sprintf, using the exec routine. Saves pulling
 *              in a lot of unnecessary library routines (5-10K!)
 */
#ifdef __amigaos4__
int VARARGS68K mysprintf(char *buf, char *fmt, ...)
{
	va_list args;
	va_startlinear(args, fmt);

	RawDoFmt(fmt, va_getlinearva(args, void *), NULL, buf);

	return(strlen(buf));
}
#else
void mysprintf(char *outstr, char *fmtstr, ...)
{
        RawDoFmt(fmtstr, &fmtstr+1, RAWDOFMT_COPY, outstr);
}
#endif

/*
 *              InstallSettings(set, which)
 *
 *              Installs the specified set of settings into CurSettings, and
 *              adjusts any other variables that may be required to reflect
 *              these new settings.
 *
 *              This includes updating the appropriate windows if necessary.
 */
void InstallSettings(Settings *set, int which)
{
        int resetwin    = 0;
        int redrawwin   = 0;

        if (which & SET_FUNC) {
                /*
                 *              First, go down through the new function options and
                 *              update the function window to match the new set of
                 *              options. For speed, we only update those that are
                 *              different.
                 */
                if (FuncWindow) {
                        ShowFuncOpts(set->Func.Opts, FIRST_BOOL_GADGET, LAST_BOOL_GADGET);
                        ResetFuncToSelected();
                        if (strcmp(CurSettings.Func.Pattern, set->Func.Pattern) != 0) {
                                GT_SetGadgetAttrs(Gadget[GID_MATCHNAME], FuncWindow, NULL,
                                                                  GTST_String,  set->Func.Pattern,
                                                                  TAG_DONE);
                        }
                }
                CurSettings.Func = set->Func;
                LoadFuncSettings(&set->Func);
        }
        if (which & SET_SETUP) {
                struct SetupSettings *curset = &CurSettings.Setup;
                struct SetupSettings *newset = &set->Setup;
                char oldwinfont[MAX_SHORT_LEN+2];
                char oldbuffont[MAX_SHORT_LEN+2];
                int  oldwinsize;
                int  oldbufsize;

                if (!CxBase)
                        newset->HideMethod = HIDE_NONE;

                if (SetWindow) {
                        /*
                         *              Update settings window to reflect new gadget settings
                         *
                         *              While we can optimise some of our update methods, we
                         *              have to blindly ALWAYS update string gadgets since
                         *              we have no way of knowing if they have been changed
                         *              by the user in a way that was not yet detected by
                         *              SnoopDos (e.g. entering a new value and forgetting
                         *              to press RETURN).
                         */
                        if (curset->HideMethod != newset->HideMethod)
                                GT_SetGadgetAttrs(Gadget[GID_HIDEMETHOD], SetWindow, NULL,
                                                                  GTCY_Active,          newset->HideMethod,
                                                                  TAG_DONE);

                        if (curset->ScreenType != newset->ScreenType)
                                GT_SetGadgetAttrs(Gadget[GID_OPENON], SetWindow, NULL,
                                                                  GTCY_Active,          newset->ScreenType,
                                                                  TAG_DONE);

                        if (curset->LogMode != newset->LogMode)
                                GT_SetGadgetAttrs(Gadget[GID_LOGMODE], SetWindow, NULL,
                                                                  GTCY_Active,          newset->LogMode,
                                                                  TAG_DONE);

                        if (curset->FileIOType != newset->FileIOType)
                                GT_SetGadgetAttrs(Gadget[GID_FILEIO], SetWindow, NULL,
                                                                  GTCY_Active,          newset->FileIOType,
                                                                  TAG_DONE);

                        GT_SetGadgetAttrs(Gadget[GID_BUFFERSIZE], SetWindow, NULL,
                                                          GTIN_Number,          newset->BufferSize,
                                                          TAG_DONE);

                        GT_SetGadgetAttrs(Gadget[GID_HOTKEY], SetWindow, NULL,
                                                          GTST_String,          newset->HotKey,
                                                          TAG_DONE);

                        GT_SetGadgetAttrs(Gadget[GID_SCREENNAME], SetWindow, NULL,
                                                          GTST_String,          newset->ScreenName,
                                                          TAG_DONE);

                        GT_SetGadgetAttrs(Gadget[GID_LOGFILE], SetWindow, NULL,
                                                          GTST_String,          newset->LogFile,
                                                          TAG_DONE);

                        GT_SetGadgetAttrs(Gadget[GID_BUFFORMAT], SetWindow, NULL,
                                                          GTST_String,  newset->BufferFormat,
                                                          TAG_DONE);

                        GT_SetGadgetAttrs(Gadget[GID_LOGFORMAT], SetWindow, NULL,
                                                          GTST_String,  newset->LogfileFormat,
                                                          TAG_DONE);
                }
                if (strcmp(curset->HotKey, newset->HotKey) != 0) {
                        InstallHotKey(newset->HotKey);
                        SetMainHideState(newset->HideMethod);
                }
                if (newset->HideMethod != curset->HideMethod) {
                        if (newset->HideMethod == HIDE_NONE)
                                CleanupHotKey();
                        else {
                                curset->HideMethod = newset->HideMethod;
                                InstallHotKey(newset->HotKey);
                        }
                        SetMainHideState(newset->HideMethod);
                }
                if (curset->BufferSize != newset->BufferSize) {
                        ClearWindowBuffer();
                        if (!SetTotalBufferSize(newset->BufferSize * 1024,
                                                                                                                        SETBUF_FORCENEW)) {
                                ShowError(MSG(MSG_ERROR_NOBUFMEM), newset->BufferSize);
                                Cleanup(20);
                        }
                }
                if (strcmp(curset->BufferFormat, newset->BufferFormat)) {
                        strcpy(curset->BufferFormat, newset->BufferFormat);
                        InstallNewFormat(NEW_STRING);
                }

                /*
                 *              Since the font settings may have been reset to the default
                 *              fonts (i.e. size = 0) we need to change them to actual
                 *              font names before we check if they've changed. Thus, we
                 *              must save the existing names first (since InitFonts()
                 *              modifies the current settings directly and would trash
                 *              them).
                 */
                strcpy(oldwinfont, curset->WindowFont);
                strcpy(oldbuffont, curset->BufferFont);
                oldwinsize = curset->WinFontSize;
                oldbufsize = curset->BufFontSize;

                if (curset->LogMode != newset->LogMode)
                        SetLogGadget(newset->LogMode, LG_REFRESH);

                *curset = *newset;                              /* Copy in new settings */

                if (curset->WinFontSize == 0 || curset->BufFontSize == 0)
                        InitFonts();

                if (strcmp(oldbuffont, curset->BufferFont) != 0 ||
                                   oldbufsize != curset->BufFontSize    ||
                        strcmp(oldwinfont, curset->WindowFont) != 0 ||
                                   oldwinsize != curset->WinFontSize)
                {
                        /*
                         *              Got a font change so update font info
                         *              accordingly and set flag to say we need
                         *              to reset the main window.
                         */
                        extern struct TextAttr WindowFontAttr;
                        extern struct TextAttr BufferFontAttr;

                        WindowFontAttr.ta_YSize = curset->WinFontSize;
                        BufferFontAttr.ta_YSize = curset->BufFontSize;
                        resetwin = 1;
                        if (SetWindow) {
                                GT_SetGadgetAttrs(Gadget[GID_WINDOWFONT], SetWindow, NULL,
                                                                  GTTX_Text, GetFontDesc(WindowFontDesc,
                                                                                                                 WindowFontName,
                                                                                                                 WindowFontSize),
                                                                  TAG_DONE);

                                GT_SetGadgetAttrs(Gadget[GID_BUFFERFONT], SetWindow, NULL,
                                                                  GTTX_Text, GetFontDesc(BufferFontDesc,
                                                                                                                 BufferFontName,
                                                                                                                 BufferFontSize),
                                                                  TAG_DONE);
                        }
                        PurgeFuncGadgets = 1;     /* Ensure function window gets updated */
                        if (!FuncWindow)
                                CloseFunctionWindow();  /* Free gadgets now if window closed */
                }

                /*
                 *              Now update everything to take advantage of the new settings
                 */
        }
        if ((which & SET_MAIN) == SET_MAIN) {
                int updatefunc = set->StackLimit != CurSettings.StackLimit;

                if (CurSettings.SimpleRefresh != set->SimpleRefresh)
                        resetwin = 1;

                if (!resetwin && set->TextSpacing != BoxInterGap)
                        SetTextSpacing(set->TextSpacing);
                else if (set->RightAlign != RightAligned && !resetwin && MainWindow) {
                        RightAligned = set->RightAlign;
                        ShowBuffer(TopSeq, DISPLAY_ALL);
                }
                if (set->ShowStatus != StatusLine || set->ShowGadgets != GadgetsLine)
                        redrawwin = 1;

                CurSettings = *set;                                     /* Copy all settings                     */
                InitFonts();                                            /* Make sure fonts are current   */
                SetMenuOptions();                                       /* Update main menu if necessary */

                if (updatefunc) {
                        /*
                         *              If we changed our stack limit, re-install all the
                         *              function settings ... this has the side effect of
                         *              fixing up the stack on the way
                         */
                        LoadFuncSettings(&set->Func);
                }
        }
        if (resetwin) {
                ReOpenMainWindow();
        } else if (redrawwin) {
           RecalcMainWindow(MainWindow->Width, MainWindow->Height, REDRAW_GADGETS);
        }
}

/*
 *              MainLoop()
 *
 *              Opens the main window, then processes all incoming events
 */
void MainLoop(void)
{
        if (QuitFlag)
                return;

        if (HideOnStartup) {
                HideSnoopDos();
        } else {
                if (!ShowSnoopDos()) {
                        ShowError(MSG(MSG_ERROR_INITGUI));
                        return;
                }
        }

        while (!QuitFlag) {
                ULONG mask;

                PROFILE_OFF();
                mask = Wait(MainWindowMask              |
                                        FuncWindowMask          |
                                        FormWindowMask          |
                                        SetWindowMask           |
                                        NewEventMask            |
                                        ScanDosListMask         |
                                        AmigaGuideMask          |
                                        CommodityMask           |
                                        RexxPortMask        |
                                        WorkbenchMask           |
                                        SIGBREAKF_CTRL_C        |
                                        SIGBREAKF_CTRL_D        |
                                        SIGBREAKF_CTRL_E        |
                                        SIGBREAKF_CTRL_F);
                PROFILE_ON();

                if (mask & MainWindowMask)              HandleMainMsgs();
                if (mask & FuncWindowMask)              HandleFuncMsgs();
                if (mask & FormWindowMask)              HandleFormatMsgs();
                if (mask & SetWindowMask)               HandleSettingsMsgs();
                if (mask & NewEventMask)                HandleNewEvents();
                if (mask & ScanDosListMask)             UpdateDeviceList(SCANDEV_DELAY);
                if (mask & AmigaGuideMask)              HandleAGuideMsgs();
                if (mask & CommodityMask)               HandleHotKeyMsgs();
                if (mask & RexxPortMask)        HandleRexxMsgs();
                if (mask & WorkbenchMask)               HandleWorkbenchMsgs();
                if (mask & SIGBREAKF_CTRL_C)    QuitFlag = 1;
                if (mask & SIGBREAKF_CTRL_D)    SetMonitorMode(MONITOR_DISABLED);
                if (mask & SIGBREAKF_CTRL_E)    SetMonitorMode(MONITOR_NORMAL);
                if (mask & SIGBREAKF_CTRL_F)    ShowSnoopDos();
        }
        CleanupWindows();
}

int main(int argc, char **argv)
{
        struct Process *myproc = (struct Process *)(FindTask(NULL));
        ULONG  stacksize;

        TaskWindowPtr = &(myproc->pr_WindowPtr);
        SaveWindowPtr = *TaskWindowPtr;
        StartPri      = FindTask(NULL)->tc_Node.ln_Pri;
        /*
         * Because some people never read manuals...
         * exchange the stack if we are too low.
         *
         */

        stacksize = (ULONG)myproc->pr_Task.tc_SPUpper-(ULONG)myproc->pr_Task.tc_SPLower;
        if (stacksize < MINSTACKSIZE) {
                StSw.stk_Lower          =       AllocMem(MINSTACKSIZE+8,MEMF_PUBLIC);
                if (StSw.stk_Lower == NULL) {
                        return 30;              /* Yikes! */
                }
                StSw.stk_Upper          =       (ULONG)StSw.stk_Lower + MINSTACKSIZE;
                StSw.stk_Pointer        =       (void *)StSw.stk_Upper;

                StackSwap(&StSw);
        } else  {
                StSw.stk_Lower          = NULL;
        }

        OpenLibs();

        InitSettings();
        InitTextTable();
        InitRexxPort();
        InitBuffers();
        CheckSegTracker();
        /*
         *              Now parse our startup options, read default configuration files
         *              and so on.
         */
        if (!ParseStartupOpts(argc, argv))
                Cleanup(0);

        RestoreSettings = CurSettings;  /* Record for later restoration */
        MainLoop();
        SetMonitorMode(MONITOR_DISABLED);
        Cleanup(0);

        return 0;
}
