#include <proto/dos.h>
#include <proto/exec.h>
#include <dos/bptr.h>
#include <dos/dos.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <exec/tasks.h>

static LONG get_default_stack_size()
{
	struct CommandLineInterface *cli = Cli();
	return cli->cli_DefaultStack * CLI_DEFAULTSTACK_UNIT;
}

int main(int argc, char **argv)
{
	struct ETask *et;
	char *fname = "SYS:Utilities/Clock";
	char *full = "";
	int lastresult;
	
	if(fname) {

		BPTR seglist = LoadSeg(fname);
		if(seglist)
		{
			SetProgramName(fname);
			lastresult=RunCommand(seglist,get_default_stack_size(),
					full,strlen(full));
			UnLoadSeg(seglist);
		}
	}
	
	printf("lastresult: %d", lastresult);
	et = GetETask(FindTask(NULL));
	if (et)
		printf(", et_Result: %d", et->et_Result);
	printf("\n");

	exit(0);
}
