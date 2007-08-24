;/*
   F_Create.rexx EXE F_Classes
   QUIT
   _________________________________________________________________________

   F_Classes 2.00 (2005/05/30)

   Written by Olivier LAVIALE (gofromiel@gofromiel.com)

   This tool is for debuggin purpose only  !!  It  will  print  all  classes
   currently available in memory.
*/

#include <exec/execbase.h>
#if !defined(__AROS__)
#include "Feelin:Sources/Feelin/Private.h"
#else
#include "Sources/feelin/Private.h"
#endif

#undef SysBase
#undef DOSBase

#include <proto/exec.h>
#include <proto/dos.h>

extern struct ExecBase             *SysBase;
struct in_FeelinBase               *FeelinInternalBase;

struct MyArgs
{
   LONG                             ShowMethods;
   LONG                             ShowAttributes;
   LONG                             ShowValues;
   LONG                             dummy;
};

void main(void)
{
   if (!FindName(&SysBase -> LibList,"feelin.library"))
   {
      Printf("Feelin is not running.\n");

      return;
   }

   if (FeelinInternalBase = (struct in_FeelinBase *) OpenLibrary("feelin.library",FV_FEELIN_VERSION))
   {
#define FeelinBase FeelinInternalBase
      struct MyArgs args[] = { FALSE, FALSE, FALSE, 0 };
      APTR rdargs;

      if (rdargs = ReadArgs("M=METHODS/S,A=ATTRIBUTES/S,V=VALUES/S",(LONG *)(&args),NULL))
      {
         struct in_FeelinClass *Class;
         ULONG nc=0,ns=0,nl=0;

         F_OPool(FeelinInternalBase -> ClassesPool);

         for (Class = (struct in_FeelinClass *)(FeelinInternalBase -> Classes.Head) ; Class ; Class = (struct in_FeelinClass *)(Class -> Public.Next))
         {
            nc++;

            Printf("(0x%08lx) %-18.s ",Class,Class -> Public.Name);
            if (Class -> DynamicID) {    Printf("(%06lx)",Class -> DynamicID); }
            else                    {    Printf("(static)"); ns++; };
            Printf(" [1mUsed[0m %4ld - [1mSize[0m %4ld",Class -> Public.UserCount,Class -> Public.LODSize);
            if (Class -> Module)       { Printf(" - [1mBase[0m 0x%08lx",Class -> Module); nl++; }
            else                         Printf("                  ");
            if (Class -> Public.Super)   Printf(" - [1mSuper[0m %s",Class -> Public.Super -> Name);
            Printf("\n");

            if (SetSignal(0L,SIGBREAKF_CTRL_C) & SIGBREAKF_CTRL_C)
            {
               Printf("*** Break\n"); break;
            }

            if (args -> ShowMethods)
            {
               FClassMethod *entry = Class -> Public.Methods;

               if (entry)
               {
                  for ( ; entry -> Function ; entry++)
                  {
                     Printf("   >> Method 0x%08lx (0x%08lx)",entry -> ID,entry -> Function);
                     
                     if (entry -> Name)
                     {
                        Printf(" - %s",entry -> Name);
                     }
                     Printf("\n");
                  }
               }
               else
               {
                  Printf("   >> No Methods\n");
               }
            }
         
            if (args -> ShowAttributes)
            {
               FClassAttribute *entry = Class -> Public.Attributes;
               
               if (entry)
               {
                  for ( ; entry -> Name ; entry++)
                  {
                     Printf("   >> Attribute 0x%08lx (%s)\n",entry -> ID,entry -> Name);
                     
                     if (args -> ShowValues)
                     {
                        FClassAttributeValue *val = entry -> Values;

                        if (val)
                        {
                           for ( ; val -> Name ; val++)
                           {
                              Printf("      >> Value (%s)(%ld)\n",val -> Name,val -> Value);
                           }
                        }
                     }
                  }
               }
               else
               {
                  Printf("   >> No Attributes\n");
               }
            }
         }

         Printf("\nTOTAL: %ld [1mClasses[0m (%ld static, %ld externals)\n",nc,ns,nl);

         F_RPool(FeelinInternalBase -> ClassesPool);

         FreeArgs(rdargs);
      }
      else
      {
         PrintFault(IoErr(),NULL);
      }

      F_FEELIN_CLOSE;
   }
}
