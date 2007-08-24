/*

   This program is used to extract changes information.
   
*/

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/feelin.h>

#include <dos/dosextens.h>

#define BUF_SIZE                                1024
 
struct MyData
{
   UBYTE                            Project[BUF_SIZE];
   UBYTE                            Path[BUF_SIZE];
   UBYTE                            Version[BUF_SIZE];
};

struct FeelinBase                   *FeelinBase;
 
int main(void)
{
   if (F_FEELIN_OPEN)
   {
      struct Process *proc;

      if (proc = (struct Process *) FindTask(NULL))
      {
         struct MyData *data;
         
         if (data = F_New(sizeof (struct MyData)))
         {
            BPTR out;
            
            if (out = Open("T:feelin_changes.txt",MODE_READWRITE))
            {
               BPTR project;
               
               Seek(out,0,OFFSET_END);
    
               if (project = Open("Project.c",MODE_OLDFILE))
               {
                  BOOL ok = FALSE;
       
                  while (FGets(project,(STRPTR)(&data -> Project),BUF_SIZE))
                  {
                     if (data -> Project[0] == ' ' && data -> Project[1] == '*' && data -> Project[2] == ' ' && data -> Project[9] == '*' && data -> Project[10] == '*' && data -> Project[23] == '*')
                     {
                        ok = TRUE; break;
                     }
                  }
            
                  if (ok)
                  {
                     ULONG len;
                     
                     data -> Project[5] = 0; data -> Project[8] = 0; data -> Project[22] = 0;
                     NameFromLock(proc -> pr_CurrentDir,(STRPTR)(&data -> Path),BUF_SIZE);
                     
                     F_StrFmt(&data -> Version,"** FC_%s (%s.%s) %s ",FilePart((STRPTR)(&data -> Path)),data -> Project + 3,data -> Project + 6,data -> Project + 12);

                     if (len = F_StrLen((STRPTR)(&data -> Version)))
                     {
                        STRPTR pad = (STRPTR)(&data -> Version) + len;

                        for ( ; len < 77 ; len++)
                        {
                           *pad++ = '*';
                        }
    
                        FPrintf(out,"%s\n",&data -> Version);
                        
                        while (FGets(project,(STRPTR)(&data -> Project),BUF_SIZE))
                        {
                           if (data -> Project[0] != '*' && data -> Project[1] != '*' && data -> Project[2] != '*')
                           {
                              FPuts(out,(STRPTR)(&data -> Project));
                           }
                           else break;
                        }
                     } 
                  }
                  else
                  {
                     Printf("Unable to locale version information\n");
                  }
                  Close(project);
               }
               else
               {
                  Printf("Unable to open %s\n","Project.c");
               }
               Close(out);
            } 
            F_Dispose(data);
         }
      }
      F_FEELIN_CLOSE;
   }
 
   return 0;
}
