/*
    X-Mines / Minesweeper type game for MUI
*/

#include "MUIMine.h"
#include "MFWindow.h"
#include "MFStrings.h"


#define MUIMINE_TITLE       (STRPTR)"MUIMine"
#define MUIMINE_VERSION     (STRPTR)"$VER: MUIMine 1.2 (16.1.99)"
#define MUIMINE_COPYRIGHT   (STRPTR)"(c)1998, Geoffrey Whaite"
#define MUIMINE_AUTHOR      (STRPTR)"Geoffrey Whaite"
#define MUIMINE_BASE        (STRPTR)"MUIMINE"


#define TT_MINEFIELDIMAGE   "MINEFIELDIMAGE"
#define TT_STARTBUTTONIMAGE "STARTBUTTONIMAGE"
#define TT_MINESDIGITSIMAGE "MINESDIGITSIMAGE"
#define TT_TIMEDIGITSIMAGE  "TIMEDIGITSIMAGE"
#define TT_SAFESTART        "SAFESTART"


#define NUM_TOOLTYPES    5

STRPTR MyToolTypes[NUM_TOOLTYPES] =
{
    TT_MINEFIELDIMAGE, TT_STARTBUTTONIMAGE, TT_MINESDIGITSIMAGE,
    TT_TIMEDIGITSIMAGE, TT_SAFESTART
};



#ifndef _DCC

struct Library *MUIMasterBase = NULL;
struct Library *DataTypesBase = NULL;
#if defined(__AROS__)
struct IORequest timereq;
struct Device *TimerBase = NULL;
#endif
struct DiskObject * dobj = NULL;
char dobjname[256];

#endif /* _DCC */

#ifdef USE_ZUNE_ON_AMIGA
__near
#endif
LONG __stack = 10000;

#ifdef USE_ZUNE_ON_AMIGA

/* On AmigaOS we build a fake library base, because it's not compiled as sharedlibrary yet */
#include "muimaster_intern.h"

int openmuimaster(void)
{
    static struct MUIMasterBase_intern MUIMasterBase_instance;
    MUIMasterBase = (struct Library*)&MUIMasterBase_instance;

    MUIMasterBase_instance.sysbase = *((struct ExecBase **)4);
    MUIMasterBase_instance.dosbase = (void*)OpenLibrary("dos.library",37);
    MUIMasterBase_instance.utilitybase = (void*)OpenLibrary("utility.library",37);
    MUIMasterBase_instance.aslbase = OpenLibrary("asl.library",37);
    MUIMasterBase_instance.gfxbase = (void*)OpenLibrary("graphics.library",37);
    MUIMasterBase_instance.layersbase = OpenLibrary("layers.library",37);
    MUIMasterBase_instance.intuibase = (void*)OpenLibrary("intuition.library",37);
    MUIMasterBase_instance.cxbase = OpenLibrary("commodities.library",37);
    MUIMasterBase_instance.keymapbase = OpenLibrary("keymap.library",37);
    MUIMasterBase_instance.gadtoolsbase = OpenLibrary("gadtools.library",37);
    __zune_prefs_init(&__zprefs);

    return 1;
}

void closemuimaster(void)
{
}

#undef SysBase
#undef IntuitionBase
#undef GfxBase
#undef LayersBase
#undef UtilityBase

#endif


/*************************/
/* Init & Fail Functions */
/*************************/

static void fail(APTR app, char *str)
{
    if (app)
    {
        MUI_DisposeObject(app);
    }

#ifndef _DCC

#ifndef USE_ZUNE_ON_AMIGA
    if (MUIMasterBase)
    {
        CloseLibrary(MUIMasterBase);
    }
#else
    closemuimaster();
#endif

    if (DataTypesBase)
    {
        CloseLibrary(DataTypesBase);
    }

#if defined(__AROS__)
    if (TimerBase) CloseDevice(&timereq);
#endif
#endif

    CloseStrings();

    if (dobj)
    {
        FreeDiskObject(dobj);
    }

    if (str)
    {
        puts(str);
        exit(20);
    }

    exit(0);
}


#ifdef _DCC

int brkfunc(void) { return(0); }

int wbmain(struct WBStartup *wb_startup)
{
    extern int main(int argc, char *argv[]);
    return (main(0, (char **)wb_startup));
}

#endif


#ifdef __SASC
int CXBRK(void) { return(0); }
int _CXBRK(void) { return(0); }
void chkabort(void) {}
#endif


static VOID init(VOID)
{
#ifdef _DCC

    onbreak(brkfunc);

#endif

#ifndef _DCC

#ifndef USE_ZUNE_ON_AMIGA
    if (!(MUIMasterBase = OpenLibrary(MUIMASTER_NAME,MUIMASTER_VMIN)))
#else
#define MUIMASTER_NAME "muimaster.library"
	  if (!openmuimaster())
#endif
    {
        fail(NULL, "Failed to open "MUIMASTER_NAME".");
    }

    if (!(DataTypesBase = OpenLibrary("datatypes.library", 0)))
    {
        fail(NULL, "Failed to open datatypes.library.");
    }

#if defined(__AROS__)
    OpenDevice("timer.device", 0, &timereq, 0);
    TimerBase = timereq.io_Device;
#endif
#endif

    OpenStrings();
}



/****************************************************************************

    support functions

****************************************************************************/


#if !defined __SASC && !defined __AROS__
static VOID stccpy(char *dest,char *source,int len)
{
    strncpy(dest,source,len);
    dest[len-1]='\0';
}
#endif


#ifndef USE_ZUNE_ON_AMIGA

#ifdef __AROS__
IPTR __stdargs DoSuperNew(struct IClass *cl,Object *obj,IPTR tag1,...)
{
    AROS_SLOWSTACKTAGS_PRE(tag1);
    retval = DoSuperMethod(cl, obj, OM_NEW, AROS_SLOWSTACKTAGS_ARG(tag1), NULL);
    AROS_SLOWSTACKTAGS_POST;
}

#else

IPTR __stdargs DoSuperNew(struct IClass *cl,Object *obj,IPTR tag1,...)
{
    return(DoSuperMethod(cl,obj,OM_NEW,&tag1,NULL));
}

#endif

#endif

/*
    function :    loads a picture file and remap for a screen using datatypes

    parameters :  data = pointer to struct LoadBitMapData which has its
                         FileName field initialized to the picture file
                         name and its Screen field initialized to the
                         screen the bitmap is to be remapped to

    return :      LBMERR_NONE if the function succeded, error code otherwise

                    If the function succedes the DTObject, BitMapHeader
                    and BitMap fields of data are set to the picture data

                    If the function fails the DTObject, BitMapHeader and
                    BitMap fields of the struct LoadBitMapData are set to
                    NULL
*/
#define PROG_DIR            "PROGDIR:"
#define PROG_IMAGE_DIR      "PROGDIR:Images"

int LoadBitMap(struct LoadBitMapData * data)
{
    int ret = LBMERR_NONE;

    BPTR lock;
    STRPTR fname, afname = NULL;

    /*
        check for correctly initialized parameters
    */
    if (data == NULL  ||  data->FileName == NULL  ||  data->Screen == NULL)
    {
        return LBMERR_PARAM;
    }

    /*
        initialize return fields
    */
    data->DTObject = NULL;
    data->BitMapHeader = NULL;
    data->BitMap = NULL;

    fname = data->FileName;

    /*
        try to locate the file, first try the file name as given and if that
        fails and the file name specifies a name only then look for a file
        with that name in the 'PROGDIR:Images' sub-drectory
    */
    lock = Lock(fname, ACCESS_READ);
    if (lock == BNULL)
    {
        /*
            could not locate the file name as given, if the file name does
            not specify a device of volume (i.e. does no contain a ':')
            then try looking for it in the PROGDIR:
        */
        STRPTR p = fname;
        while (*p != ':'  &&  *p != 0)  p++;
        if (*p == 0)
        {
            /*
                allocate a buffer to build the file name in
            */
            int l = strlen(fname) + strlen(PROG_DIR) + 4;
            afname = AllocVec(l, 0);
            if (afname)
            {
                /*
                    build the file name for the PROGDIR:
                */
                strcpy(afname, PROG_DIR);
                if (AddPart(afname, fname, l))
                {
                    /*
                        attempt to lock the file to establish its
                        existence
                    */
                    lock = Lock(afname, ACCESS_READ);
                }
                if (lock == BNULL)
                {
                    /*
                        could not lock the file, free the file
                        name buffer, we will try the "images"
                        directory later
                    */
                    FreeVec(afname);
                    afname = NULL;
                }
            }
            else
            {
                ret = LBMERR_ALLOC;
            }

            /*
                if we still have not located the file check if the
                name specifies the file name part only and if it
                does then try the "PROGDIR:images" directory
            */
            if (lock == BNULL)
            {
                p = FilePart(fname);
                if (p == fname)
                {
                    /*
                        allocate buffer to build the file name in
                    */
                    l = strlen(fname) + strlen(PROG_IMAGE_DIR) + 4;
                    afname = AllocVec(l, 0);
                    if (afname)
                    {
                        if (ret == LBMERR_ALLOC)
                        {
                            ret = LBMERR_NONE;
                        }
                        /*
                            build the file name for the "PROGDIR:images"
                            directory
                        */
                        strcpy(afname, PROG_IMAGE_DIR);
                        if (AddPart(afname, fname, l))
                        {
                            /*
                                attempt to lock the file to establish
                                its existence
                            */
                            lock = Lock(afname, ACCESS_READ);
                        }
                        if (lock == BNULL)
                        {
                            /*
                                could not lock the file, free the
                                file name buffer
                            */
                            FreeVec(afname);
                            afname = NULL;
                        }
                    }
                    else
                    {
                        ret = LBMERR_ALLOC;
                    }
                }
            }
        }
    }

    /*
        check if the picture file was found, if it was NOT then set the
        return error code
    */
    if (lock == BNULL)
    {
        /*
            if the error code was not set then set it to a 'no file' error
        */
        if (ret == LBMERR_NONE)
        {
            ret = LBMERR_NOFILE;
        }
    }
    else
    {
        UnLock(lock);   // release lock now that the file has been found

        /*
            try to load the file as a picture datatype
        */
        if ((data->DTObject = NewDTObject((afname) ? afname : fname,
                                                DTA_SourceType       ,DTST_FILE,
                                                DTA_GroupID          ,GID_PICTURE,
                                                PDTA_Remap           ,TRUE,
                                                PDTA_Screen          ,data->Screen,
                                                PDTA_FreeSourceBitMap,TRUE,
                                                PDTA_DestMode        ,PMODE_V43,
                                                PDTA_UseFriendBitMap ,TRUE,
                                                OBP_Precision        ,PRECISION_IMAGE,
                                                TAG_DONE)))
        {
            /*
                picture datatype loaded ok, now layout to screen
            */
            if (DoMethod(data->DTObject, DTM_PROCLAYOUT, NULL, 1))
            {
                /*
                    get the bit map header and the bit map from the datatype
                */
                GetDTAttrs(data->DTObject,
                                PDTA_BitMapHeader, &data->BitMapHeader,
                                TAG_DONE);
                if (data->BitMapHeader)
                {
                    GetDTAttrs(data->DTObject, PDTA_DestBitMap, &data->BitMap, TAG_DONE);
                    if (!data->BitMap)
                    {
                        GetDTAttrs(data->DTObject, PDTA_BitMap, &data->BitMap, TAG_DONE);
                        if (!data->BitMap)
                        {
                            data->BitMapHeader = NULL;
                            ret = LBMERR_BITMAP;
                        }
                    }
                }
                else
                {
                    ret = LBMERR_BITMAPHEADER;
                }
            }
            else
            {
                ret = LBMERR_LAYOUT;
            }

            /*
                dispose of the datatype object if an error occured
            */
            if (ret != LBMERR_NONE)
            {
                DisposeDTObject(data->DTObject);
                data->DTObject = NULL;
            }
        }
        else
        {
            ret = LBMERR_DTLOAD;
        }
    }

    /*
        free the file name if allocated
    */
    if (afname)
    {
        FreeVec(afname);
    }

    return ret;
}


/*
    function :    compares a ToolType entry with a ToolType name

    parameters :  TTName = name of the ToolType to compare for
                  TTEntry = ToolType entry string

    return :      TRUE if the ToolType entry is for the given ToolType name
*/
BOOL CompareToolTypeName(STRPTR TTName, STRPTR TTEntry)
{
    STRPTR p1, p2;
    char ch1, ch2;

    p1 = TTName; p2 = TTEntry;
    while (*p1)
    {
        ch1 = ToLower(*p1++);
        ch2 = ToLower(*p2++);
        if (ch1 != ch2)
        {
            return FALSE;
        }
    }

    return (BOOL)(*p2 == '='  ||  *p2 == 0);
}


/****************************************************************************

    main program

****************************************************************************/

int main(int argc, char *argv[])
{
    STRPTR minefieldimage, startbuttonimage,
           minesdigitsimage, timedigitsimage;
    BOOL safestart;
    APTR app, window;
    struct MUI_CustomClass *mccMFWindow;

    init();

    if (!(mccMFWindow = CreateMFWindowClass()))
    {
        fail(NULL,"Could not create MFWindow class.");
    }

    /*
        get the disk object (icon) for the program, if argc == 0 then
        the program was launched from Workbench and we use the first
        argument in the startup message's argument list
    */
    dobjname[0] = 0;
    if (argc == 0)
    {
        struct WBStartup * wbsu = (struct WBStartup *)argv;
        if (wbsu)
        {
            struct WBArg * wbarg = wbsu->sm_ArgList;
            if (wbarg)
            {
                BPTR oldlock = CurrentDir(wbarg->wa_Lock);
                dobj = GetDiskObject(wbarg->wa_Name);
                CurrentDir(oldlock);
                if (NameFromLock(wbarg->wa_Lock, dobjname, 256))
                {
                    if (!AddPart(dobjname, wbarg->wa_Name, 256))
                    {
                        dobjname[0] = 0;
                    }
                }
            }
        }
    }
    /*
        if argc != 0 then the program was execcuted from a CLI in this
        case get the program directory and program name from dos and
        use these to get the disk object, if that fails then try to
        get the disk object using argv[0] (the command name that
        launched the program).
    */
    else
    {
        BPTR lock;
        char progname[32];

        lock = GetProgramDir();
        if (lock  &&  GetProgramName(progname, 32))
        {
            BPTR oldlock = CurrentDir(lock);
            dobj = GetDiskObject(progname);
            CurrentDir(oldlock);
            if (NameFromLock(lock, dobjname, 256))
            {
                if (!AddPart(dobjname, progname, 256))
                {
                    dobjname[0] = 0;
                }
            }
        }

        if (dobj == NULL)
        {
            dobj = GetDiskObject(argv[0]);
            strcpy(dobjname, argv[0]);
        }
    }

    /*
        if we successfully got the disk object then search the tool types
        for MUIMine's options
    */
    if (dobj)
    {
        minefieldimage = FindToolType(dobj->do_ToolTypes, TT_MINEFIELDIMAGE);
        startbuttonimage = FindToolType(dobj->do_ToolTypes, TT_STARTBUTTONIMAGE);
        minesdigitsimage = FindToolType(dobj->do_ToolTypes, TT_MINESDIGITSIMAGE);
        timedigitsimage = FindToolType(dobj->do_ToolTypes, TT_TIMEDIGITSIMAGE);
        safestart = (FindToolType(dobj->do_ToolTypes, TT_SAFESTART) != NULL);
    }
    /*
        if no disk object then set MUIMine's options to defaults
    */
    else
    {
        minefieldimage   = startbuttonimage =
        minesdigitsimage = timedigitsimage  = NULL;
        safestart = FALSE;
    }

    /*
        create the application object
    */
    app = ApplicationObject,
        MUIA_Application_Title      , MUIMINE_TITLE,
        MUIA_Application_Version    , MUIMINE_VERSION,
        MUIA_Application_Copyright  , MUIMINE_COPYRIGHT,
        MUIA_Application_Author     , MUIMINE_AUTHOR,
        MUIA_Application_Description, GetStr(MSG_GAME_DESCRIPTION),
        MUIA_Application_Base       , MUIMINE_BASE,
        MUIA_Application_DiskObject , dobj,

        SubWindow, window = NewObject(mccMFWindow->mcc_Class,NULL,
            MUIA_MFWindow_MineFieldImage, minefieldimage,
            MUIA_MFWindow_StartButtonImage, startbuttonimage,
            MUIA_MFWindow_MinesDigitsImage, minesdigitsimage,
            MUIA_MFWindow_TimeDigitsImage, timedigitsimage,
            MUIA_MFWindow_SafeStart, safestart,
            TAG_DONE),

        End;

    if (!app)
    {
        fail(app,"Failed to create Application.");
    }

    DoMethod(window, MUIM_Notify,MUIA_Window_CloseRequest, TRUE,
                app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

/*
** This is the ideal input loop for an object oriented MUI application.
** Everything is encapsulated in classes, no return ids need to be used,
** we just check if the program shall terminate.
** Note that MUIM_Application_NewInput expects sigs to contain the result
** from Wait() (or 0). This makes the input loop significantly faster.
*/

    SetAttrs(window, MUIA_Window_Open, TRUE, TAG_DONE);

    {
        IPTR sigs = 0;

        while (DoMethod(app, MUIM_Application_NewInput, &sigs)
                        != MUIV_Application_ReturnID_Quit)
        {
            if (sigs)
            {
                sigs = Wait(sigs | SIGBREAKF_CTRL_C);
                if (sigs & SIGBREAKF_CTRL_C) break;
            }
        }
    }

    SetAttrs(window, MUIA_Window_Open, FALSE, TAG_DONE);

    /*
        update Tool Types if we have a disk object
    */
    if (dobj && dobjname[0])
    {
        STRPTR pstr, minefieldimagett, startbuttonimagett,
               minesdigitsimagett, timedigitsimagett, safestarttt;
        IPTR ss;
        int i;
        BOOL updatett = FALSE;

        /*
            get the mine field image file name, check if it
            has changed and create a new tool type for it
        */
        minefieldimagett = NULL;
        if (GetAttr(MUIA_MFWindow_MineFieldImage, window, (IPTR *)&pstr))
        {
            if ((pstr != NULL  ||  minefieldimage != NULL)  &&
                (pstr == NULL  ||  minefieldimage == NULL  ||
                 strcmpi(pstr, minefieldimage)))
            {
                updatett = TRUE;
            }
            if (pstr)
            {
                i = strlen(pstr) + strlen(TT_MINEFIELDIMAGE) + 2;
                minefieldimagett = (STRPTR)AllocVec(i, 0);
                if (minefieldimagett)
                {
                    sprintf(minefieldimagett,
                            "%s=%s", TT_MINEFIELDIMAGE, pstr);
                }
            }
        }

        /*
            get the start button image file name, check if it
            has changed and create a new tool type for it
        */
        startbuttonimagett = NULL;
        if (GetAttr(MUIA_MFWindow_StartButtonImage, window, (IPTR *)&pstr))
        {
            if ((pstr != NULL  ||  startbuttonimage != NULL)  &&
                (pstr == NULL  ||  startbuttonimage == NULL  ||
                 strcmpi(pstr, startbuttonimage)))
            {
                updatett = TRUE;
            }
            if (pstr)
            {
                i = strlen(pstr) + strlen(TT_STARTBUTTONIMAGE) + 2;
                startbuttonimagett = (STRPTR)AllocVec(i, 0);
                if (startbuttonimagett)
                {
                    sprintf(startbuttonimagett,
                            "%s=%s", TT_STARTBUTTONIMAGE, pstr);
                }
            }
        }

        /*
            get the mines digits image file name, check if it
            has changed and create a new tool type for it
        */
        minesdigitsimagett = NULL;
        if (GetAttr(MUIA_MFWindow_MinesDigitsImage, window, (IPTR *)&pstr))
        {
            if ((pstr != NULL  ||  minesdigitsimage != NULL)  &&
                (pstr == NULL  ||  minesdigitsimage == NULL  ||
                 strcmpi(pstr, minesdigitsimage)))
            {
                updatett = TRUE;
            }
            if (pstr)
            {
                i = strlen(pstr) + strlen(TT_MINESDIGITSIMAGE) + 2;
                minesdigitsimagett = (STRPTR)AllocVec(i, 0);
                if (minesdigitsimagett)
                {
                    sprintf(minesdigitsimagett,
                            "%s=%s", TT_MINESDIGITSIMAGE, pstr);
                }
            }
        }

        /*
            get the time digits image file name, check if it
            has changed and create a new tool type for it
        */
        timedigitsimagett = NULL;
        if (GetAttr(MUIA_MFWindow_TimeDigitsImage, window, (IPTR *)&pstr))
        {
            if ((pstr != NULL  ||  timedigitsimage != NULL)  &&
                (pstr == NULL  ||  timedigitsimage == NULL  ||
                 strcmpi(pstr, timedigitsimage)))
            {
                updatett = TRUE;
            }
            if (pstr)
            {
                i = strlen(pstr) + strlen(TT_TIMEDIGITSIMAGE) + 2;
                timedigitsimagett = (STRPTR)AllocVec(i, 0);
                if (timedigitsimagett)
                {
                    sprintf(timedigitsimagett,
                            "%s=%s", TT_TIMEDIGITSIMAGE, pstr);
                }
            }
        }

        /*
            get the safe start option, check if it has changed
            and create a new tool type for it
        */
        safestarttt = NULL;
        if (GetAttr(MUIA_MFWindow_SafeStart, window, &ss))
        {
            if ((safestart && !ss)  ||  (!safestart && ss))
            {
                updatett = TRUE;
            }
            if (ss)
            {
                i = strlen(TT_SAFESTART) + 1;
                safestarttt = (STRPTR)AllocVec(i, 0);
                if (safestarttt)
                {
                    strcpy(safestarttt, TT_SAFESTART);
                    updatett = TRUE;
                }
            }
        }

        /*
            check if we need to update the tool types
        */
        if (updatett)
        {
            int i;
            char ** newtt;
            char ** ptt;
            char ** oldtt = (APTR)dobj->do_ToolTypes;

            /*
                determine the maximum number of tool types required
                by assuming that none of our own tool types exist we
                need our number of tool types plus one for the array
                terminator plus the number of existing tool types
            */
            for (i = NUM_TOOLTYPES + 1, ptt = oldtt; *ptt != NULL; i++, ptt++);

            /*
                attempt to allocate the tool type array
            */
            newtt = (char **)AllocVec(i * sizeof(char *), 0);
            if (newtt)
            {
                char ** p1 = newtt;
                ptt = oldtt;

                /*
                    step through the old tool types comparing the names to
                    out tool types and if the tool type is not one of ours
                    then put it in the new tool type array
                */
                while (*ptt != NULL)
                {
                    BOOL ourtt = FALSE;
                    for (i = 0; i < NUM_TOOLTYPES; i++)
                    {
                        ourtt = CompareToolTypeName(MyToolTypes[i], *ptt);
                        if (ourtt)
                        {
                            break;
                        }
                    }
                    if (!ourtt)
                    {
                        *p1++ = *ptt;
                    }
                    ptt++;
                }

                /*
                    now place our tool types in the new tool type array
                */
                if (minefieldimagett)
                {
                    *p1++ = minefieldimagett;
                }
                if (startbuttonimagett)
                {
                    *p1++ = startbuttonimagett;
                }
                if (minesdigitsimagett)
                {
                    *p1++ = minesdigitsimagett;
                }
                if (timedigitsimagett)
                {
                    *p1++ = timedigitsimagett;
                }
                if (safestarttt)
                {
                    *p1++ = safestarttt;
                }
                /*
                    terminate the new tool type array
                */
                *p1 = NULL;

                /*
                    put the new tool type array in the disk object and
                    write the disk object to the disk
                */
                dobj->do_ToolTypes = (APTR)newtt;
                PutDiskObject(dobjname, dobj);
                dobj->do_ToolTypes = (APTR)oldtt;

                FreeVec(newtt);
            }

        }

        /*
            deallocate our tool type strings
        */
        if (minefieldimagett)
        {
            FreeVec((APTR)minefieldimagett);
        }
        if (startbuttonimagett)
        {
            FreeVec((APTR)startbuttonimagett);
        }
        if (minesdigitsimagett)
        {
            FreeVec((APTR)minesdigitsimagett);
        }
        if (timedigitsimagett)
        {
            FreeVec((APTR)timedigitsimagett);
        }

    } /* if (dobj) */

/*
** Shut down...
*/

    MUI_DisposeObject(app);             /* dispose all objects. */
    DeleteMFWindowClass(mccMFWindow);   /* delete the custom class. */
    fail(NULL,NULL);                    /* exit, app is already disposed. */

    return 0;
}

