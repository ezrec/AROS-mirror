#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/feelin.h>

#include <workbench/startup.h>
#include <libraries/feelin.h>
#include <feelin/xmlapplication.h>

struct FeelinBase *FeelinBase;

#ifdef __AROS__
#define _WBenchMsg WBenchMsg
#endif

extern struct WBStartup *_WBenchMsg;

int32 main()
{
    if (F_FEELIN_OPEN)
    {
        STRPTR file;
   
        if (_WBenchMsg)
        {
            uint32 narg = _WBenchMsg -> sm_NumArgs;
            struct WBArg *arg = _WBenchMsg -> sm_ArgList;

            //F_Log(0,"WB 0x%08lx - Num %ld - List 0x%08lx",_WBenchMsg,narg,arg);

            if (file = F_New(1024))
            {
                while (narg--)
                {
                    NameFromLock(arg -> wa_Lock,file,1023);
                    AddPart(file,arg -> wa_Name,1023);

                    //F_Log(0,"WB (%s)(0x%08lx)",buf,arg -> wa_Lock);

                    arg++;
                }
            }
        }
        else
        {
            uint32 args[] = { (uint32)("Hello.xml"), 0};
            APTR rdargs;
          
            if (rdargs = ReadArgs("SRC=SOURCE",(int32 *)(&args),NULL))
            {
                FreeArgs(rdargs);
            }
            else
            {
                Printf("This program builds applications from XML sources. By default, it builds an application from 'Hello.xml' but you can supply any XML source.\n");
            }
         
            file = F_StrNew(NULL,"%s",*args);
        }
        

        if (file)
        {
            FObject xapp = XMLApplicationObject,

                "Source", file,
                "SourceType", "File",

                End;
                
            if (xapp)
            {
               F_Do(xapp,(uint32) "Run");
               F_DisposeObj(xapp);
            }
         
            F_Dispose(file);
        }
        
        F_FEELIN_CLOSE;
    }
    return 0;
}
 
