#include <exec/types.h>

#include "kernellibs.h"
#include "debug.h"

#include "callfuncs.c"

struct LibCall Exec_Funcs[]=
{
  {NULL, "open"},
  {NULL, "close"},
  {NULL, "null"},
  {NULL, "null"},
  {call_A5_VOID, 	"Supervisor"},
  {call_A0_A1_A2, 	"PrepareContext"},
  {NULL, 		"Private2"},
  {call_A0_VOID,	"Reschedule"},
  {call_VOID, 		"Switch"},
  {call_VOID, 		"Dispatch"},
  {call_VOID, 		"Exception"},
  {call_D0_D1_VOID, 	"InitCode"},
  {call_A1_A2_D0_VOID,	"InitStruct"},
  {call_A0_A1_A2_D0_D1, "MakeLibrary"},
  {call_A0_A1_A2, 	"MakeFunctions"},
  {call_A1, 		"FindResident"},
  {call_A1_D1, 		"InitResident"},
  {call_D7_VOID, 	"Alert"},
  {call_D0_VOID, 	"Debug"},
  {call_VOID, 		"Disable"},
  {call_VOID, 		"Enable"},
  {call_VOID,	 	"Forbid"},
  {call_VOID, 		"Permit"},
  {call_D0_D1, 		"SetSR"},
  {call_VOID, 		"SuperState"},
  {call_D0_VOID, 	"UserState"},
  {call_D0_A1, 		"SetIntVector"},
  {call_D0_A1_VOID, 	"AddIntServer"},
  {call_D0_A1_VOID, 	"RemIntServer"},
  {call_A1_VOID, 	"Cause"},
  {call_A0_D0, 		"Allocate"},
  {call_A0_A1_D0_VOID, 	"Deallocate"},
  {call_D0_D1, 		"AllocMem"},
  {call_D0_D1, 		"AllocAbs"},
  {call_A1_D0_VOID, 	"FreeMem"},
  {call_D1, 		"AvailMem"},
  {call_A0, 		"AllocEntry"},
  {call_A0_VOID, 	"FreeEntry"},
  {call_A0_A1_A2, 	"Insert"},
  {call_A0_A1_VOID, 	"AddHead"},
  {call_A0_A1_VOID, 	"AddTail"},
  {call_A1_VOID,	"Remove"},
  {call_A0,		"RemHead"},
  {call_A0,		"RemTail"},
  {call_A0_A1_VOID,	"Enqueue"},
  {call_A0_A1,		"FindName"},
  {call_A1_A2_A3, 	"AddTask"},
  {call_A1_VOID,	"RemTask"},
  {call_A1,		"FindTask"},
  {call_A1_D0,		"SetTaskPri"},
  {call_D0_D1,		"SetSignal"},
  {call_D0_D1,		"SetExcept"},
  {call_D0,		"Wait"},
  {call_A1_D0_VOID,	"Signal"},
  {call_D0,		"AllocSignal"},
  {call_D0_VOID,	"FreeSignal"},
  {call_D0,		"AllocTrap"},
  {call_D0_VOID,	"FreeTrap"},
  {call_A1_VOID,	"AddPort"},
  {call_A1_VOID,	"RemPort"},
  {call_A0_A1_VOID,	"PutMsg"},
  {call_A0,		"GetMsg"},
  {call_A1_VOID,	"ReplyMsg"},
  {call_A0,		"WaitPort"},
  {call_A1,		"FindPort"},
  {call_A1_VOID,	"AddLibrary"},
  {call_A1_VOID,	"RemLibrary"},
  {call_A1_D0,		"OldOpenLibrary"},
  {call_A1_VOID,	"CloseLibrary"},
  {call_A1_A0_D0,	"SetFunction"},
  {call_A1_VOID,	"SumLibrary"},
  {call_A1_VOID,	"AddDevice"},
  {call_A1_VOID,	"RemDevice"},
  {call_A0_D0_A1_D1,	"OpenDevice"},
  {call_A1_VOID,	"CloseDevice"},
  {call_A1,		"DoIO"},
  {call_A1_VOID,	"SendIO"},
  {call_A1,		"CheckIO"},
  {call_A1,		"WaitIO"},
  {call_A1,		"AbortIO"},
  {call_A1_VOID,	"AddResource"},
  {call_A1_VOID,	"RemResource"},
  {call_A1,		"OpenResource"},
  {call_VOID,		"RawIOInit"},
  {call_NOPARM,		"RawMayGetChar"},
  {call_D0_VOID,	"RawPutChar"}
};




struct Libbases Libbasetable[]=
{
  {NULL, Exec_Funcs},
  {-1, -1}
};


extern struct ExecBase * SysBase;

void init_libbases(void)
{
  /* fetch the library bases */ 
  Libbasetable[0].baseptr = (struct Library *)SysBase;

}

void dolibcall()
{
  int i = 0;
  BOOL found = FALSE;
  while (-1 != (LONG)(Libbasetable[i].baseptr))
  {
    if (CPU.A[6] == (ULONG)Libbasetable[i].baseptr)
    {
      ULONG offset;
      /* It's this entry */
      offset = (CPU.PC-CPU.A[6])/6 - 1;
SHOW_OPCODE("Doing libcall to function %s\n",(Libbasetable[i].libcall)[offset].funcname);
      (Libbasetable[i].libcall)[offset].func(*(ULONG *)(CPU.PC+2));
      found = TRUE;
      
      /* do a fake RTS so we get back into m68k code */
      CPU.PC = *(ULONG *)(CPU.A[7]);
      CPU.A[7] += 4;
      CPU.instsize = 0;
      break;
    }
    i++;
  }
}
