// TestWBArgs.c
// 25 Apr 2001 17:28:53

#include <workbench/startup.h>
#include <stdio.h>
#include <stdlib.h>
#include <proto/dos.h>


int main(int argc, char *argv[])
{
	char Path[512];
	struct WBStartup *Wbs;
	struct MsgPort *fsPort;
	ULONG n;

	Wbs = (struct WBStartup *) argv;

	if (argc > 0)
		{
		fprintf(stderr, "Please start this programn from Workbench\n");
		return 5;
		}

	printf("NumArgs = %ld\n", Wbs->sm_NumArgs);

	GetCurrentDirName(Path, sizeof(Path));
	printf("CurrentDir = <%s>\n", Path);

	fsPort = GetFileSysTask();
	if (fsPort)
		{
		printf("default filesystem = <%s>\n", ((struct Task *) fsPort->mp_SigTask)->tc_Node.ln_Name);
		}

	for (n=0; n<Wbs->sm_NumArgs; n++)
		{
		struct WBArg *arg = &Wbs->sm_ArgList[n];

		NameFromLock(arg->wa_Lock, Path, sizeof(Path));
		printf("arg[%ld]  Lock=%08lx <%s>  Name=<%s>\n", n, arg->wa_Lock, Path, arg->wa_Name);
		}
}

