/*
**	$Id$
**
**	:ts=8
*/

/****************************************************************************/

#include <dos/dos.h>

#include <clib/exec_protos.h>
#include <clib/dos_protos.h>

#include <pragmas/exec_pragmas.h>
#include <pragmas/dos_pragmas.h>

extern void kprintf(const char *,...);

/****************************************************************************/

static int _indent_level;

/****************************************************************************/

void
_INDENT(void)
{
	int i;

	for(i = 0 ; i < _indent_level ; i++)
		kprintf("   ");
}

void
_SHOWVALUE(
	unsigned long value,
	int size,
	const char *name,
	const char *file,
	int line)
{
	char *fmt;

	switch(size)
	{
		case 1:

			fmt = "%s:%ld:%s = %ld, 0x%02lx";
			break;

		case 2:

			fmt = "%s:%ld:%s = %ld, 0x%04lx";
			break;

		default:

			fmt = "%s:%ld:%s = %ld, 0x%08lx";
			break;
	}

	_INDENT();

	kprintf(fmt,file,line,name,value,value);

	if(size == 1 && value < 256)
	{
		if(value < ' ' || (value >= 127 && value < 160))
			kprintf(", '\\x%02lx'",value);
		else
			kprintf(", '%lc'",value);
	}

	kprintf("\n");
}

/****************************************************************************/

void
_SHOWSTRING(
	const char *string,
	const char *name,
	const char *file,
	int line)
{
	_INDENT();
	kprintf("%s:%ld:%s = 0x%08lx \"%s\"\n",file,line,name,string,string);
}

/****************************************************************************/

void
_SHOWMSG(
	const char *string,
	const char *file,
	int line)
{
	_INDENT();
	kprintf("%s:%ld:%s\n",file,line,string);
}

/****************************************************************************/

void
_ENTER(
	const char *file,
	int line,
	const char *function)
{
	_INDENT();
	kprintf("%s:%ld:Entering %s\n",file,line,function);
	_indent_level++;
}

void
_LEAVE(
	const char *file,
	int line,
	const char *function)
{
	_indent_level--;
	_INDENT();
	kprintf("%s:%ld: Leaving %s\n",file,line,function);
}

void
_RETURN(
	const char *file,
	int line,
	const char *function,
	unsigned long result)
{
	_indent_level--;
	_INDENT();
	kprintf("%s:%ld: Leaving %s (result 0x%08lx, %ld)\n",file,line,function,result,result);
}

/****************************************************************************/

void
_ASSERT(
	int x,
	const char *xs,
	const char *file,
	int line,
	const char *function)
{
#ifdef CONFIRM
	STATIC BOOL ScrollMode	= FALSE;
	STATIC BOOL BatchMode	= FALSE;

	if(BatchMode == FALSE)
	{
		if(x == 0)
		{
			kprintf("%s:%ld:Expression `%s' failed assertion in %s().\n",
			        file,
			        line,
			        xs,
			        function);

			if(ScrollMode == FALSE)
			{
				ULONG Signals;

				SetSignal(0,SIGBREAKF_CTRL_C | SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_E);

				kprintf(" ^C to continue, ^D to enter scroll mode, ^E to enter batch mode\r");

				Signals = Wait(SIGBREAKF_CTRL_C | SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_E);

				if(Signals & SIGBREAKF_CTRL_D)
				{
					ScrollMode = TRUE;

					kprintf("Ok, entering scroll mode\033[K\n");
				}
				else if (Signals & SIGBREAKF_CTRL_E)
				{
					BatchMode = TRUE;

					kprintf("Ok, entering batch mode\033[K\n");
				}
				else
				{
					/* Continue */

					kprintf("\033[K\r");
				}
			}
		}
	}
#else
	if(x == 0)
	{
		_INDENT();
		kprintf("%s:%ld:Expression `%s' failed assertion in %s().\n",
		        file,
		        line,
		        xs,
		        function);
	}
#endif	/* CONFIRM */
}
