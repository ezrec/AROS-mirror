#include <stdio.h>
#include "SDL_types.h"
#include "SDL_inline.h"
#include <exec/types.h>
#include <inline/exec.h>

struct Library *SDLBase=NULL;
extern struct ExecBase *SysBase;

int SDL_Init(unsigned long flags)
{
	if(!SDLBase)
		if(!(SDLBase=OpenLibrary("SDL.library",0L)))
			return -1;

	return SDL_RealInit(flags);
}

void SDL_Quit(void)
{
	if(SDLBase) {
		SDL_RealQuit();
		CloseLibrary(SDLBase);
		SDLBase=NULL;
	}
}
