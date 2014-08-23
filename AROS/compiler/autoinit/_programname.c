#include <aros/symbolsets.h>
#include <aros/startup.h>
#include <proto/autoinit.h>


char *_ProgramName = NULL;

void __initprogramname(struct ExecBase *SysBase)
{
    ProgramNameInit((STRPTR *)&_ProgramName);

    __startup_entries_next();

    return;
}

ADD2SET(__initprogramname, PROGRAM_ENTRIES, -20);
