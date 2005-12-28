#include <exec/types.h>
#include <intuition/intuition.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <constructor.h>

#define VERSION 1

void _INIT_4_GTKMUIBase(void) __attribute__((alias("__CSTP_init_GTKMUIBase")));
void _EXIT_4_GTKMUIBase(void) __attribute__((alias("__DSTP_cleanup_GTKMUIBase")));

struct Library *GTKMUIBase;

static void openliberror(ULONG version, const char *name)
{
	struct IntuitionBase *IntuitionBase;

	IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 0);
	if (IntuitionBase)
	{
		static struct EasyStruct panic =
		{
			sizeof(struct EasyStruct),
			0,
			"SDL startup message",
			0,
			"Abort"
		};
		ULONG args[2];

		panic.es_TextFormat = "Need version %.10ld of %.32s";
		args[0] = version;
		args[1] = (ULONG)name;
		EasyRequestArgs(NULL, &panic, NULL, args);

		CloseLibrary((struct Library *) IntuitionBase);
	}
}

static const char libname[] = "gtk-mui.library";

static CONSTRUCTOR_P(init_GTKMUIBase, 100)
{
	GTKMUIBase = (void *) OpenLibrary((STRPTR)libname, VERSION);
	if (GTKMUIBase)
	{
	}
	else
	{
		openliberror(VERSION, libname);
	}

	return (GTKMUIBase == NULL);
}

static DESTRUCTOR_P(cleanup_GTKMUIBase, 100)
{
	CloseLibrary(GTKMUIBase);
}
