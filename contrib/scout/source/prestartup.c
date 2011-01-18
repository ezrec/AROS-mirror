#include <exec/execbase.h>
#include <exec/types.h>
#include <SDI_compiler.h>

#if defined(__AROS__)
extern ULONG SAVEDS realstartup(struct ExecBase *sBase);

AROS_UFH3(__startup static ULONG, startup,
	  AROS_UFHA(char *, argstr, A0),
	  AROS_UFHA(ULONG, argsize, D0),
	  AROS_UFHA(struct ExecBase *, sBase, A6))
{
    AROS_USERFUNC_INIT

    return realstartup(sBase);

    AROS_USERFUNC_EXIT
}
#elif defined(__amigaos4__)
#error AmigaOS4 uses the _start() entry
#else
extern ULONG SAVEDS realstartup(void);

ULONG startup(void)
{
	return realstartup();
}
#endif
