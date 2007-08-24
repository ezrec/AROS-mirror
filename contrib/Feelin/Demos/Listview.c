#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/feelin.h>

#include <libraries/feelin.h>

struct FeelinBase *FeelinBase;
#define UtilityBase                             FeelinBase -> Utility

STATIC struct DateTime buf_time;

///code_construct
STATIC F_HOOKM(APTR,code_construct,FS_List_Construct)
{
    if (ExNext((BPTR)(Hook -> h_Data),(struct FileInfoBlock *)(Msg -> Entry)))
    {
        struct FileInfoBlock *fib;

        if (fib = F_NewP(Msg -> Pool,sizeof (struct FileInfoBlock)))
        {
            CopyMem(Msg -> Entry,fib,sizeof (struct FileInfoBlock));

            return fib;
        }
    }
    return NULL;
}
//+
///code_destruct
STATIC F_HOOKM(void,code_destruct,FS_List_Destruct)
{
    F_Dispose(Msg -> Entry);
}
//+
///code_display
STATIC F_HOOKM(FListDisplay *,code_display,FS_List_Display)
{
    struct FileInfoBlock *fib;

    STATIC FListDisplay ld[4];
 
    if (fib = Msg -> Entry)
    {
        ULONG i;
 
        ld[0].String   = fib -> fib_FileName;
        ld[0].PreParse = NULL;

        if (fib -> fib_Size)
        {
            STATIC BYTE buf[32];

            F_StrFmt(&buf,"%ld",fib -> fib_Size);
 
            ld[1].String   = (STRPTR)(&buf);
            ld[1].PreParse = "<align=right>";
        }
        else
        {
            ld[1].String   = "(dir)";
            ld[1].PreParse = "<align=right><pens text=shine>";
        }

        CopyMem(&fib -> fib_Date,&buf_time.dat_Stamp,sizeof (struct DateStamp));
        DateToStr(&buf_time);
        
        for (i = 0 ; i < 32 ; i++)
        {
            if (buf_time.dat_StrDate[i] == ' ')
            {
                buf_time.dat_StrDate[i] = '\0'; break;
            }
            else if (buf_time.dat_StrDate[i] == '\0') break;
        }

        ld[2].String   = buf_time.dat_StrDate;
        ld[2].PreParse = "<align=center>";
        ld[3].String   = buf_time.dat_StrTime;
        ld[3].PreParse = "<align=right>";
    }
    else
    {
        ld[0].String   = "Name";
        ld[0].PreParse = "<pens style=shadow up=shine>";
        ld[1].String   = "Size";
        ld[1].PreParse = "<pens style=shadow up=shine><align=right>";
        ld[2].String   = "Date";
        ld[2].PreParse = "<pens style=shadow up=shine><align=center>";
        ld[3].String   = "Time";
        ld[3].PreParse = "<pens style=shadow up=shine><align=center>";
    }

    return (FListDisplay *)(&ld);
}
//+
///code_compare
STATIC F_HOOKM(LONG,code_compare,FS_List_Compare)
{
    return Stricmp(((struct FileInfoBlock *)(Msg -> Entry)) -> fib_FileName,
                        ((struct FileInfoBlock *)(Msg -> Other)) -> fib_FileName);
}
//+

///main
int main(void)
{
    struct FileInfoBlock *fib;
    BPTR lock;
    
    struct Hook h_cons,h_dest,h_disp,h_comp;
 
    STATIC BYTE buf_str_day[32];
    STATIC BYTE buf_str_date[128];
    STATIC BYTE buf_str_time[16];

    buf_time.dat_Format  = FORMAT_DOS;
    buf_time.dat_Flags   = DTF_SUBST;
    buf_time.dat_StrDay  = (STRPTR)(&buf_str_day);
    buf_time.dat_StrDate = (STRPTR)(&buf_str_date);
    buf_time.dat_StrTime = (STRPTR)(&buf_str_time);
    
    if (lock = Lock("PROGDIR:",ACCESS_READ))
    {
        if (fib = AllocDosObject(DOS_FIB,NULL))
        {
            if (Examine(lock,fib)) goto __ok;

            FreeDosObject(DOS_FIB,fib);
        }
        UnLock(lock);
    }

    return 0;

__ok:

    h_cons.h_Entry  = (HOOKFUNC) F_FUNCTION_GATE(code_construct);
    h_cons.h_Data   = (APTR)(lock);
    h_dest.h_Entry  = (HOOKFUNC) F_FUNCTION_GATE(code_destruct);
    h_disp.h_Entry  = (HOOKFUNC) F_FUNCTION_GATE(code_display);
    h_comp.h_Entry  = (HOOKFUNC) F_FUNCTION_GATE(code_compare);

    if (F_FEELIN_OPEN)
    {
        FObject app,win,lst,str;
        
        app = AppObject,
            FA_Application_Title,        "demo_List",
            FA_Application_Version,      "$VER: demo_List 1.0 (2003/02/24)",
            FA_Application_Copyright,    "©2003 Olivier LAVIALE",
            FA_Application_Author,       "Olivier LAVIALE (gofromiel@gofromiel.com)",
            FA_Application_Description,  "FC_List demo",
            FA_Application_Base,         "DEMOLIST",

            Child, win = WindowObject,
                FA_ID,            MAKE_ID('M','A','I','N'),
                FA_Window_Title,  "Feelin : Listview",
                FA_Window_Open,   TRUE,
                FA_Width,         200,
                FA_Height,        200,

                Child, VGroup,
                    Child, ListviewObject, "FA_Listview_List",
                        lst = ListObject,
                            FA_SetMin,                 FV_SetWidth,
                          "FA_List_ConstructHook",    &h_cons,
                          "FA_List_DestructHook",     &h_dest,
                          "FA_List_DisplayHook",      &h_disp,
                          "FA_List_CompareHook",      &h_comp,
                          "FA_List_Format",           "<col weight='50%' bar='true'/>"\
                                                                "<col bar='true' fixed='true' />"\
                                                                "<col span='2' fixed='true' weight='25%' bar='true'/>",
                        End,
                    End,
                    
                    Child, str = StringObject, FA_SetMax,FV_SetHeight, End,
                End,
            End,
        End;

        if (app)
        {
            uint32 id_InsertSingle = F_DynamicFindID("FM_List_InsertSingle");
            
            F_Set(str,(uint32) "FA_String_AttachedList",(uint32) lst);
 
            F_Do(win,FM_Notify,FA_Window_CloseRequest,TRUE,app,FM_Application_Shutdown,0);
  
            while (F_Do(lst,id_InsertSingle,fib,FV_List_Insert_Sorted));

            F_Do(app,FM_Application_Run);

            F_DisposeObj(app);
        }

        F_FEELIN_CLOSE;
    }

    if (fib)
    {
        FreeDosObject(DOS_FIB,fib);
    }
         
    if (lock)
    {
        UnLock(lock);
    }
    
    return 0;
}
//+

