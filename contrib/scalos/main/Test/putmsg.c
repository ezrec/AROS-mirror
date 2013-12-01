// PutMsg.c
// 04 Oct 2001 17:39:08


#include <exec/types.h>
#include <dos/dos.h>
#include <proto/exec.h>
#include <clib/exec_protos.h>

#include <stdlib.h>
#include <stdio.h>


extern ULONG newPutMsg();
extern ULONG origPutMsg;


main()
{
	origPutMsg = (ULONG) SetFunction((struct Library *) SysBase, -366, (ULONG (* const )()) newPutMsg);

	printf("PutMsg patched.\nPress Ctrl+C to abort.\n");

	Wait(SIGBREAKF_CTRL_C);

	SetFunction((struct Library *) SysBase, -366, (ULONG (* const )()) origPutMsg);

	printf("PutMsg restored.\n");
}

