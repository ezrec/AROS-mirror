/******************************************************************************/
/*                                                                            */
/* includes                                                                   */
/*                                                                            */
/******************************************************************************/

#include <exec/types.h>
#include <exec/execbase.h>
#include <dos/dosextens.h>
#include <inline/exec.h>
#include <libnix/stabs.h>

/******************************************************************************/
/*                                                                            */
/* exports                                                                    */
/*                                                                            */
/******************************************************************************/

const BYTE LibName[]="SDL.library";
const BYTE LibIdString[]="$VER: SDL.library 12.5 (13.10.2002) Amiga port of SDL 1.2.5 by Gabriele Greco";

const UWORD LibVersion=12;
const UWORD LibRevision=5;

/******************************************************************************/
/*                                                                            */
/* global declarations                                                        */
/*                                                                            */
/******************************************************************************/

struct Library *myLibPtr;

struct ExecBase *SysBase;
struct DosLibrary *__DOSBase,*DOSBase;
struct Library *__UtilityBase,*__MathIeeeDoubBasBase,*__MathIeeeSingBasBase,*__MathIeeeDoubTransBase;

/******************************************************************************/
/*                                                                            */
/* user library initialization                                                */
/*                                                                            */
/* !!! CAUTION: This function may run in a forbidden state !!!                */
/*                                                                            */
/******************************************************************************/

int __UserLibInit(struct Library *myLib)
{
  /* setup your library base - to access library functions over *this* basePtr! */

  myLibPtr = myLib;

  /* required !!! */
  SysBase=*(struct ExecBase **)4;

  kprintf("***SDL.library init\n");

  if(!(DOSBase=(struct DosLibrary *)OpenLibrary("dos.library",39L))) {
	kprintf("unable to open dos.library\n");
	return 1;
  }

  __DOSBase=DOSBase;

  if(!(__UtilityBase=OpenLibrary("utility.library",37L))) {
        kprintf("***unable to open utility.library.\n");
	CloseLibrary((struct Library *)__DOSBase);
	return 1;
  }


  if(!(__MathIeeeSingBasBase=OpenLibrary("mathieeesingbas.library",33L))) {
        kprintf("***unable to open mathieeesingbas.library.\n");
	CloseLibrary(__UtilityBase);
	CloseLibrary((struct Library *)__DOSBase);
	return 1;
  }

  if(!(__MathIeeeDoubBasBase=OpenLibrary("mathieeedoubbas.library",33L))) {
        kprintf("***unable to open mathieeedoubbas.library.\n");
	CloseLibrary(__MathIeeeSingBasBase);
	CloseLibrary(__UtilityBase);
	CloseLibrary((struct Library *)__DOSBase);
	return 1;
  }

  if(!(__MathIeeeDoubTransBase=OpenLibrary("mathieeedoubtrans.library",33L))) {
        kprintf("***unable to open mathdoubtransbase.library.\n");
	CloseLibrary(__MathIeeeSingBasBase);
	CloseLibrary(__MathIeeeDoubBasBase);
	CloseLibrary(__UtilityBase);
	CloseLibrary((struct Library *)__DOSBase);
	return 1;
  }

  __initstdio();
  __initstdfio();
  __initamigapath();
  __initclock();

  return 0;
}

/******************************************************************************/
/*                                                                            */
/* user library cleanup                                                       */
/*                                                                            */
/* !!! CAUTION: This function runs in a forbidden state !!!                   */
/*                                                                            */
/******************************************************************************/

void __UserLibCleanup()
{
  kprintf("***SDL.library cleanup\n");

  __exitmalloc();
  __exitstdio();
  __exitstdfio();
  
  CloseLibrary(__MathIeeeDoubTransBase);
  CloseLibrary(__MathIeeeDoubBasBase);
  CloseLibrary(__MathIeeeSingBasBase);
  CloseLibrary(__UtilityBase);
  CloseLibrary((struct Library *)DOSBase);

}

/******************************************************************************/
/*                                                                            */
/* library dependent function(s)                                              */
/*                                                                            */
/******************************************************************************/

// General
ADDTABL_1(SDL_Init,d0); /* One Argument in d0 */
ADDTABL_0(SDL_Quit);
ADDTABL_1(SDL_InitSubSystem,d0);
ADDTABL_1(SDL_QuitSubSystem,d0);
ADDTABL_1(SDL_WasInit,d0);

// RWops
ADDTABL_2(SDL_RWFromFile,a0,a1);
ADDTABL_2(SDL_RWFromFP,a0,d0);
ADDTABL_2(SDL_RWFromMem,a0,d0);
ADDTABL_0(SDL_AllocRW);
ADDTABL_1(SDL_FreeRW,a0);

// video
ADDTABL_2(SDL_LoadBMP_RW,a0,d0);
ADDTABL_1(SDL_DisplayFormat,a0);
ADDTABL_1(SDL_FreeSurface,a0);
ADDTABL_3(SDL_FillRect,a0,a1,d0);
ADDTABL_4(SDL_UpperBlit,a0,d0,a1,d1);
ADDTABL_1(SDL_Flip,a0);
ADDTABL_3(SDL_UpdateRects,a0,d0,a1);
ADDTABL_5(SDL_UpdateRect,a0,d0,d1,d2,d3);
ADDTABL_0(SDL_GetVideoInfo);
ADDTABL_4(SDL_SetVideoMode,d0,d1,d2,d3);
ADDTABL_4(SDL_MapRGB,a0,d0,d1,d2);
ADDTABL_5(SDL_MapRGBA,a0,d0,d1,d2,d3);
ADDTABL_2(SDL_VideoDriverName,a0,d0);
ADDTABL_0(SDL_GetVideoSurface);
ADDTABL_4(SDL_VideoModeOK,d0,d1,d2,d3);
ADDTABL_2(SDL_ListModes,a0,d0);
ADDTABL_3(SDL_SetGamma,d0,d1,d2);
ADDTABL_3(SDL_SetGammaRamp,d0,d1,d2);
ADDTABL_3(SDL_GetGammaRamp,d0,d1,d2);
ADDTABL_4(SDL_SetColors,a0,a1,d0,d1);
ADDTABL_5(SDL_SetPalette,a0,d0,a1,d1,d2);
ADDTABL_5(SDL_GetRGB,d0,a0,d1,d2,d3);
ADDTABL_6(SDL_GetRGBA,d0,a0,d1,d2,d3,d4);
ADDTABL_8(SDL_CreateRGBSurface,d0,d1,d2,d3,d4,d5,d6,d7);
ADDTABL_9(SDL_CreateRGBSurfaceFrom,a0,d0,d1,d2,d3,d4,d5,d6,d7);
ADDTABL_1(SDL_LockSurface,a0);
ADDTABL_1(SDL_UnlockSurface,a0);
ADDTABL_3(SDL_SaveBMP_RW,a0,a1,d0);
ADDTABL_3(SDL_SetColorKey,a0,d0,d1);
ADDTABL_3(SDL_SetAlpha,a0,d0,d1);
ADDTABL_2(SDL_SetClipRect,a0,a1);
ADDTABL_2(SDL_GetClipRect,a0,a1);
ADDTABL_3(SDL_ConvertSurface,a0,a1,d0);
ADDTABL_1(SDL_DisplayFormatAlpha,a0);

// overlay
ADDTABL_4(SDL_CreateYUVOverlay,d0,d1,d2,a0);
ADDTABL_1(SDL_LockYUVOverlay,a0);
ADDTABL_1(SDL_UnlockYUVOverlay,a0);
ADDTABL_2(SDL_DisplayYUVOverlay,a0,a1);
ADDTABL_1(SDL_FreeYUVOverlay,a0);

// GL
ADDTABL_1(SDL_GL_LoadLibrary,a0);
ADDTABL_1(SDL_GL_GetProcAddress,a0);
ADDTABL_2(SDL_GL_SetAttribute,d0,d1);
ADDTABL_2(SDL_GL_GetAttribute,d0,a0);
ADDTABL_0(SDL_GL_SwapBuffers);
ADDTABL_2(SDL_GL_UpdateRects,d0,a0);
ADDTABL_0(SDL_GL_Lock);
ADDTABL_0(SDL_GL_Unlock);

// WM
ADDTABL_2(SDL_WM_SetCaption,a0,a1);
ADDTABL_2(SDL_WM_GetCaption,a0,a1);
ADDTABL_2(SDL_WM_SetIcon,a0,a1);
ADDTABL_0(SDL_WM_IconifyWindow);
ADDTABL_1(SDL_WM_ToggleFullScreen,a0);
ADDTABL_1(SDL_WM_GrabInput,a0);

// timer
ADDTABL_0(SDL_GetTicks);
ADDTABL_1(SDL_Delay,d0);
ADDTABL_2(SDL_SetTimer,d0,a0);
ADDTABL_3(SDL_AddTimer,d0,a0,a1);
ADDTABL_1(SDL_RemoveTimer,d0);

// events
ADDTABL_0(SDL_PumpEvents);
ADDTABL_1(SDL_PollEvent,a0);
ADDTABL_1(SDL_WaitEvent,a0);
ADDTABL_4(SDL_PeepEvents,a0,d0,d1,d2);
ADDTABL_1(SDL_PushEvent,a0);
ADDTABL_1(SDL_SetEventFilter,d0);
ADDTABL_0(SDL_GetEventFilter);
ADDTABL_2(SDL_EventState,d0,d1);

// joy
ADDTABL_0(SDL_NumJoysticks);
ADDTABL_1(SDL_JoystickName,d0);
ADDTABL_1(SDL_JoystickOpen,d0);
ADDTABL_1(SDL_JoystickOpened,d0);
ADDTABL_1(SDL_JoystickIndex,a0);
ADDTABL_1(SDL_JoystickNumAxes,a0);
ADDTABL_1(SDL_JoystickNumBalls,a0);
ADDTABL_1(SDL_JoystickNumHats,a0);
ADDTABL_1(SDL_JoystickNumButtons,a0);
ADDTABL_0(SDL_JoystickUpdate);
ADDTABL_1(SDL_JoystickEventState,d0);
ADDTABL_2(SDL_JoystickGetAxis,a0,d0);
ADDTABL_2(SDL_JoystickGetHat,a0,d0);
ADDTABL_4(SDL_JoystickGetBall,a0,d0,a1,a2);
ADDTABL_2(SDL_JoystickGetButton,a0,d0);
ADDTABL_1(SDL_JoystickClose,a0);

// keyboard
ADDTABL_1(SDL_EnableUNICODE,d0);
ADDTABL_2(SDL_EnableKeyRepeat,d0,d1);
ADDTABL_1(SDL_GetKeyState,a0);
ADDTABL_0(SDL_GetModState);
ADDTABL_1(SDL_SetModState,d0);
ADDTABL_1(SDL_GetKeyName,d0);

// mouse
ADDTABL_2(SDL_GetMouseState,a0,a1);
ADDTABL_2(SDL_GetRelativeMouseState,a0,a1);
ADDTABL_2(SDL_WarpMouse,d0,d1);
ADDTABL_6(SDL_CreateCursor,a0,a1,d0,d1,d2,d3);
ADDTABL_1(SDL_SetCursor,a0);
ADDTABL_0(SDL_GetCursor);
ADDTABL_1(SDL_FreeCursor,a0);
ADDTABL_1(SDL_ShowCursor,d0);

// app
ADDTABL_0(SDL_GetAppState);

// error
ADDTABL_2(SDL_SetError,a0,a1);
ADDTABL_0(SDL_GetError);
ADDTABL_0(SDL_ClearError);

// audio
ADDTABL_1(SDL_AudioInit,a0);
ADDTABL_0(SDL_AudioQuit);
ADDTABL_2(SDL_AudioDriverName,a0,d0);
ADDTABL_2(SDL_OpenAudio,a0,a1);
ADDTABL_0(SDL_GetAudioStatus);
ADDTABL_1(SDL_PauseAudio,d0);
ADDTABL_5(SDL_LoadWAV_RW,a0,d0,a1,a2,a3);
ADDTABL_1(SDL_FreeWAV,a0);
ADDTABL_7(SDL_BuildAudioCVT,a0,d0,d1,d2,d3,d4,d5);
ADDTABL_1(SDL_ConvertAudio,a0);
ADDTABL_4(SDL_MixAudio,a0,a1,d0,d1);
ADDTABL_0(SDL_LockAudio);
ADDTABL_0(SDL_UnlockAudio);
ADDTABL_0(SDL_CloseAudio);

// thread
ADDTABL_2(SDL_CreateThread,a0,a1);
ADDTABL_0(SDL_ThreadID);
ADDTABL_1(SDL_GetThreadID,a0);
ADDTABL_2(SDL_WaitThread,a0,a1);
ADDTABL_1(SDL_KillThread,a0);

// version
ADDTABL_0(SDL_Linked_Version);

// extensions
ADDTABL_4(SDL_SoftStretch,a0,a1,a2,a3);
/******************************************************************************/
/*                                                                            */
/* endtable marker (required!)                                                */
/*                                                                            */
/******************************************************************************/

ADDTABL_END();

long _WBenchMsg;

void exit(int a)
{
	kprintf("***WARNING exit call from within the library!***\n");

	for(;;);
}

#if 0
char *getenv(char *a)
{
	kprintf("provata getenv(%s)\n",a);

	return NULL;
}
#endif

/******************************************************************************/
/*                                                                            */
/* end of simplelib.c                                                         */
/*                                                                            */
/******************************************************************************/
