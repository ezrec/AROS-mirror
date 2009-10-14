/*
    $VER: F_TPG 1.00 (2004/12/18) by Olivier LAVIALE (gofromiel@gofromiel.com)
    __________________________________________________________________________

    This little tool is very useful to quickly test the look of a preference
    group.  Instead  of  launching  the  preference  editor,  the  requested
    preference group is created in a simple window. Since preference  groups
    are defined in libraries, name is case-sensitive.

*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/feelin.h>
#include <proto/feelinclass.h>

#include <libraries/feelin.h>

struct FeelinBase                  *FeelinBase;

int main()
{
    if (F_FEELIN_OPEN)
    {
        IPTR args[] = { (IPTR)"Area", 0 };
        APTR rdargs;

        if ((rdargs = ReadArgs("CLASS/A",(IPTR *)(&args),NULL)))
        {
            STRPTR name;

            if ((name = F_StrNew(NULL,"LIBS:Feelin/%s.fc",args[0])))
            {
                struct Library *FeelinClassBase = OpenLibrary(name,0);

                if (FeelinClassBase)
                {
                    FClass *cc = F_CreateClassA(NULL,F_Query(FV_Query_PrefsTags,FeelinBase));

                    if (cc)
                    {
                        STRPTR win_name = F_StrNew(NULL,"PreferenceGroup : %s",args[0]);

                        if (win_name)
                        {
                            FObject app,win,grp;

                            app = AppObject,
                                Child, win = WindowObject,
                                    FA_Window_Title,  win_name,
                                    FA_Window_Open,   TRUE,

                                    Child, grp = F_NewObj(cc -> Name,FA_Frame,NULL,TAG_DONE),
                                End,
                            End;

                            if (app)
                            {
                                FObject prefs = (FObject) F_Get(app,FA_Application_Preference);

                                if (prefs)
                                {
                                    F_Do(grp,(ULONG) "FM_PreferenceGroup_Load",prefs,F_DynamicFindID("FM_Preference_Resolve"),F_DynamicFindID("FM_Preference_ResolveInt"));
                                }

                                F_Do(win,FM_Notify,FA_Window_CloseRequest,TRUE,app,FM_Application_Shutdown,0);
                                F_Do(app,FM_Application_Run);
                                F_DisposeObj(app);
                            }

                            F_Dispose(win_name);
                        }

                        F_DeleteClass(cc);
                    }

                    CloseLibrary(FeelinClassBase);
                }
                else
                {
                    Printf("Unable to open %s\n",name);
                }

                F_Dispose(name);
            }

            FreeArgs(rdargs);
        }
        else
        {
            PrintFault(IoErr(),NULL);
        }

        F_FEELIN_CLOSE;
    }
    else Printf("Unable to open %s %ld\n","feelin.library",FV_FEELIN_VERSION);

    return 0;
}
