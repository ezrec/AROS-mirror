/* Automatically generated header! Do not edit! */

#ifndef _INLINE_SDL_H
#define _INLINE_SDL_H

// Since we don't need amiga structs here we can avoid including them
#define __INLINE_STUB_H

#ifndef __INLINE_MACROS_H
#include <inline/macros.h>
#endif /* !__INLINE_MACROS_H */

#ifndef SDL_BASE_NAME
extern struct Library *SDLBase;
#define SDL_BASE_NAME SDLBase
#endif /* !SDL_BASE_NAME */

#define SDL_AddTimer(interval, callback, param) \
	LP3(0x1aa, SDL_TimerID, SDL_AddTimer, Uint32, interval, d0, SDL_NewTimerCallback, callback, a0, void *, param, a1, \
	, SDL_BASE_NAME)

#define SDL_AllocRW() \
	LP0(0x4e, SDL_RWops *, SDL_AllocRW, \
	, SDL_BASE_NAME)

#define SDL_AudioDriverName(namebuf, maxlen) \
	LP2(0x2be, char *, SDL_AudioDriverName, char *, namebuf, a0, int, maxlen, d0, \
	, SDL_BASE_NAME)

#define SDL_AudioInit(driver_name) \
	LP1(0x2b2, int, SDL_AudioInit, const char *, driver_name, a0, \
	, SDL_BASE_NAME)

#define SDL_AudioQuit() \
	LP0NR(0x2b8, SDL_AudioQuit, \
	, SDL_BASE_NAME)

#define SDL_BuildAudioCVT(cvt, src_format, src_channels, src_rate, dst_format, dst_channels, dst_rate) \
	LP7(0x2e2, int, SDL_BuildAudioCVT, SDL_AudioCVT *, cvt, a0, Uint16, src_format, d0, Uint8, src_channels, d1, int, src_rate, d2, Uint16, dst_format, d3, Uint8, dst_channels, d4, int, dst_rate, d5, \
	, SDL_BASE_NAME)

#define SDL_ClearError() \
	LP0NR(0x2ac, SDL_ClearError, \
	, SDL_BASE_NAME)

#define SDL_CloseAudio() \
	LP0NR(0x300, SDL_CloseAudio, \
	, SDL_BASE_NAME)

#define SDL_ConvertAudio(cvt) \
	LP1(0x2e8, int, SDL_ConvertAudio, SDL_AudioCVT *, cvt, a0, \
	, SDL_BASE_NAME)

#define SDL_ConvertSurface(src, fmt, flags) \
	LP3(0x11a, SDL_Surface *, SDL_ConvertSurface, SDL_Surface *, src, a0, SDL_PixelFormat *, fmt, a1, Uint32, flags, d0, \
	, SDL_BASE_NAME)

#define SDL_CreateCursor(data, mask, w, h, hot_x, hot_y) \
	LP6(0x27c, SDL_Cursor *, SDL_CreateCursor, Uint8 *, data, a0, Uint8 *, mask, a1, int, w, d0, int, h, d1, int, hot_x, d2, int, hot_y, d3, \
	, SDL_BASE_NAME)

#define SDL_CreateRGBSurface(flags, width, height, depth, Rmask, Gmask, Bmask, Amask) \
	LP8(0xe4, SDL_Surface *, SDL_CreateRGBSurface, Uint32, flags, d0, int, width, d1, int, height, d2, int, depth, d3, Uint32, Rmask, d4, Uint32, Gmask, d5, Uint32, Bmask, d6, Uint32, Amask, d7, \
	, SDL_BASE_NAME)

#define SDL_CreateRGBSurfaceFrom(pixels, width, height, depth, pitch, Rmask, Gmask, Bmask, Amask) \
	LP9(0xea, SDL_Surface *, SDL_CreateRGBSurfaceFrom, void *, pixels, a0, int, width, d0, int, height, d1, int, depth, d2, int, pitch, d3, Uint32, Rmask, d4, Uint32, Gmask, d5, Uint32, Bmask, d6, Uint32, Amask, d7, \
	, SDL_BASE_NAME)

#define SDL_CreateThread(fn, data) \
	LP2FP(0x306, SDL_Thread *, SDL_CreateThread, __fpt, fn, a0, void *, data, a1, \
	, SDL_BASE_NAME, int (*__fpt)(void *))

#define SDL_CreateYUVOverlay(width, height, format, display) \
	LP4(0x126, SDL_Overlay *, SDL_CreateYUVOverlay, int, width, d0, int, height, d1, Uint32, format, d2, SDL_Surface *, display, a0, \
	, SDL_BASE_NAME)

#define SDL_Delay(ms) \
	LP1NR(0x19e, SDL_Delay, Uint32, ms, d0, \
	, SDL_BASE_NAME)

#define SDL_DisplayFormat(surface) \
	LP1(0x60, SDL_Surface *, SDL_DisplayFormat, SDL_Surface *, surface, a0, \
	, SDL_BASE_NAME)

#define SDL_DisplayFormatAlpha(surface) \
	LP1(0x120, SDL_Surface *, SDL_DisplayFormatAlpha, SDL_Surface *, surface, a0, \
	, SDL_BASE_NAME)

#define SDL_DisplayYUVOverlay(overlay, dstrect) \
	LP2(0x138, int, SDL_DisplayYUVOverlay, SDL_Overlay *, overlay, a0, SDL_Rect *, dstrect, a1, \
	, SDL_BASE_NAME)

#define SDL_EnableKeyRepeat(delay, interval) \
	LP2(0x24c, int, SDL_EnableKeyRepeat, int, delay, d0, int, interval, d1, \
	, SDL_BASE_NAME)

#define SDL_EnableUNICODE(enable) \
	LP1(0x246, int, SDL_EnableUNICODE, int, enable, d0, \
	, SDL_BASE_NAME)

#define SDL_EventState(type, state) \
	LP2(0x1e0, Uint8, SDL_EventState, Uint8, type, d0, int, state, d1, \
	, SDL_BASE_NAME)

#define SDL_FillRect(dst, dstrect, color) \
	LP3(0x6c, int, SDL_FillRect, SDL_Surface *, dst, a0, SDL_Rect *, dstrect, a1, Uint32, color, d0, \
	, SDL_BASE_NAME)

#define SDL_Flip(screen) \
	LP1(0x78, int, SDL_Flip, SDL_Surface *, screen, a0, \
	, SDL_BASE_NAME)

#define SDL_FreeCursor(cursor) \
	LP1NR(0x28e, SDL_FreeCursor, SDL_Cursor *, cursor, a0, \
	, SDL_BASE_NAME)

#define SDL_FreeRW(area) \
	LP1NR(0x54, SDL_FreeRW, SDL_RWops *, area, a0, \
	, SDL_BASE_NAME)

#define SDL_FreeSurface(surface) \
	LP1NR(0x66, SDL_FreeSurface, SDL_Surface *, surface, a0, \
	, SDL_BASE_NAME)

#define SDL_FreeWAV(audio_buf) \
	LP1NR(0x2dc, SDL_FreeWAV, Uint8 *, audio_buf, a0, \
	, SDL_BASE_NAME)

#define SDL_FreeYUVOverlay(overlay) \
	LP1NR(0x13e, SDL_FreeYUVOverlay, SDL_Overlay *, overlay, a0, \
	, SDL_BASE_NAME)

#define SDL_GL_GetAttribute(attr, value) \
	LP2(0x156, int, SDL_GL_GetAttribute, SDL_GLattr, attr, d0, int*, value, a0, \
	, SDL_BASE_NAME)

#define SDL_GL_GetProcAddress(proc) \
	LP1(0x14a, void *, SDL_GL_GetProcAddress, const char*, proc, a0, \
	, SDL_BASE_NAME)

#define SDL_GL_LoadLibrary(path) \
	LP1(0x144, int, SDL_GL_LoadLibrary, const char *, path, a0, \
	, SDL_BASE_NAME)

#define SDL_GL_Lock() \
	LP0NR(0x168, SDL_GL_Lock, \
	, SDL_BASE_NAME)

#define SDL_GL_SetAttribute(attr, value) \
	LP2(0x150, int, SDL_GL_SetAttribute, SDL_GLattr, attr, d0, int, value, d1, \
	, SDL_BASE_NAME)

#define SDL_GL_SwapBuffers() \
	LP0NR(0x15c, SDL_GL_SwapBuffers, \
	, SDL_BASE_NAME)

#define SDL_GL_Unlock() \
	LP0NR(0x16e, SDL_GL_Unlock, \
	, SDL_BASE_NAME)

#define SDL_GL_UpdateRects(numrects, rects) \
	LP2NR(0x162, SDL_GL_UpdateRects, int, numrects, d0, SDL_Rect*, rects, a0, \
	, SDL_BASE_NAME)

#define SDL_GetAppState() \
	LP0(0x29a, Uint8, SDL_GetAppState, \
	, SDL_BASE_NAME)

#define SDL_GetAudioStatus() \
	LP0(0x2ca, SDL_audiostatus, SDL_GetAudioStatus, \
	, SDL_BASE_NAME)

#define SDL_GetClipRect(surface, rect) \
	LP2NR(0x114, SDL_GetClipRect, SDL_Surface *, surface, a0, SDL_Rect *, rect, a1, \
	, SDL_BASE_NAME)

#define SDL_GetCursor() \
	LP0(0x288, SDL_Cursor *, SDL_GetCursor, \
	, SDL_BASE_NAME)

#define SDL_GetError() \
	LP0(0x2a6, char *, SDL_GetError, \
	, SDL_BASE_NAME)

#define SDL_GetEventFilter() \
	LP0(0x1da, SDL_EventFilter, SDL_GetEventFilter, \
	, SDL_BASE_NAME)

#define SDL_GetGammaRamp(red, green, blue) \
	LP3(0xc6, int, SDL_GetGammaRamp, Uint16 *, red, d0, Uint16 *, green, d1, Uint16 *, blue, d2, \
	, SDL_BASE_NAME)

#define SDL_GetKeyName(key) \
	LP1(0x264, char *, SDL_GetKeyName, SDLKey, key, d0, \
	, SDL_BASE_NAME)

#define SDL_GetKeyState(numkeys) \
	LP1(0x252, Uint8 *, SDL_GetKeyState, int *, numkeys, a0, \
	, SDL_BASE_NAME)

#define SDL_GetModState() \
	LP0(0x258, SDLMod, SDL_GetModState, \
	, SDL_BASE_NAME)

#define SDL_GetMouseState(x, y) \
	LP2(0x26a, Uint8, SDL_GetMouseState, int *, x, a0, int *, y, a1, \
	, SDL_BASE_NAME)

#define SDL_GetRGB(pixel, format, r, g, b) \
	LP5NR(0xd8, SDL_GetRGB, Uint32, pixel, d0, SDL_PixelFormat *, format, a0, Uint8 *, r, d1, Uint8 *, g, d2, Uint8 *, b, d3, \
	, SDL_BASE_NAME)

#define SDL_GetRGBA(pixel, format, r, g, b, a) \
	LP6NR(0xde, SDL_GetRGBA, Uint32, pixel, d0, SDL_PixelFormat *, format, a0, Uint8 *, r, d1, Uint8 *, g, d2, Uint8 *, b, d3, Uint8 *, a, d4, \
	, SDL_BASE_NAME)

#define SDL_GetRelativeMouseState(x, y) \
	LP2(0x270, Uint8, SDL_GetRelativeMouseState, int *, x, a0, int *, y, a1, \
	, SDL_BASE_NAME)

#define SDL_GetThreadID(thread) \
	LP1(0x312, Uint32, SDL_GetThreadID, SDL_Thread *, thread, a0, \
	, SDL_BASE_NAME)

#define SDL_GetTicks() \
	LP0(0x198, Uint32, SDL_GetTicks, \
	, SDL_BASE_NAME)

#define SDL_GetVideoInfo() \
	LP0(0x8a, const SDL_VideoInfo *, SDL_GetVideoInfo, \
	, SDL_BASE_NAME)

#define SDL_GetVideoSurface() \
	LP0(0xa8, SDL_Surface *, SDL_GetVideoSurface, \
	, SDL_BASE_NAME)

#define SDL_RealInit(flags) \
	LP1(0x1e, int, SDL_Init, Uint32, flags, d0, \
	, SDL_BASE_NAME)

#define SDL_InitSubSystem(flags) \
	LP1(0x2a, int, SDL_InitSubSystem, Uint32, flags, d0, \
	, SDL_BASE_NAME)

#define SDL_JoystickClose(joystick) \
	LP1NR(0x240, SDL_JoystickClose, SDL_Joystick *, joystick, a0, \
	, SDL_BASE_NAME)

#define SDL_JoystickEventState(state) \
	LP1(0x222, int, SDL_JoystickEventState, int, state, d0, \
	, SDL_BASE_NAME)

#define SDL_JoystickGetAxis(joystick, axis) \
	LP2(0x228, Sint16, SDL_JoystickGetAxis, SDL_Joystick *, joystick, a0, int, axis, d0, \
	, SDL_BASE_NAME)

#define SDL_JoystickGetBall(joystick, ball, dx, dy) \
	LP4(0x234, int, SDL_JoystickGetBall, SDL_Joystick *, joystick, a0, int, ball, d0, int *, dx, a1, int *, dy, a2, \
	, SDL_BASE_NAME)

#define SDL_JoystickGetButton(joystick, button) \
	LP2(0x23a, Uint8, SDL_JoystickGetButton, SDL_Joystick *, joystick, a0, int, button, d0, \
	, SDL_BASE_NAME)

#define SDL_JoystickGetHat(joystick, hat) \
	LP2(0x22e, Uint8, SDL_JoystickGetHat, SDL_Joystick *, joystick, a0, int, hat, d0, \
	, SDL_BASE_NAME)

#define SDL_JoystickIndex(joystick) \
	LP1(0x1fe, int, SDL_JoystickIndex, SDL_Joystick *, joystick, a0, \
	, SDL_BASE_NAME)

#define SDL_JoystickName(device_index) \
	LP1(0x1ec, const char *, SDL_JoystickName, int, device_index, d0, \
	, SDL_BASE_NAME)

#define SDL_JoystickNumAxes(joystick) \
	LP1(0x204, int, SDL_JoystickNumAxes, SDL_Joystick *, joystick, a0, \
	, SDL_BASE_NAME)

#define SDL_JoystickNumBalls(joystick) \
	LP1(0x20a, int, SDL_JoystickNumBalls, SDL_Joystick *, joystick, a0, \
	, SDL_BASE_NAME)

#define SDL_JoystickNumButtons(joystick) \
	LP1(0x216, int, SDL_JoystickNumButtons, SDL_Joystick *, joystick, a0, \
	, SDL_BASE_NAME)

#define SDL_JoystickNumHats(joystick) \
	LP1(0x210, int, SDL_JoystickNumHats, SDL_Joystick *, joystick, a0, \
	, SDL_BASE_NAME)

#define SDL_JoystickOpen(device_index) \
	LP1(0x1f2, SDL_Joystick *, SDL_JoystickOpen, int, device_index, d0, \
	, SDL_BASE_NAME)

#define SDL_JoystickOpened(device_index) \
	LP1(0x1f8, int, SDL_JoystickOpened, int, device_index, d0, \
	, SDL_BASE_NAME)

#define SDL_JoystickUpdate() \
	LP0NR(0x21c, SDL_JoystickUpdate, \
	, SDL_BASE_NAME)

#define SDL_KillThread(thread) \
	LP1NR(0x31e, SDL_KillThread, SDL_Thread *, thread, a0, \
	, SDL_BASE_NAME)

#define SDL_Linked_Version() \
	LP0(0x324, const SDL_version *, SDL_Linked_Version, \
	, SDL_BASE_NAME)

#define SDL_ListModes(format, flags) \
	LP2(0xb4, SDL_Rect **, SDL_ListModes, SDL_PixelFormat *, format, a0, Uint32, flags, d0, \
	, SDL_BASE_NAME)

#define SDL_LoadBMP_RW(src, freesrc) \
	LP2(0x5a, SDL_Surface *, SDL_LoadBMP_RW, SDL_RWops *, src, a0, int, freesrc, d0, \
	, SDL_BASE_NAME)

#define SDL_LoadWAV_RW(src, freesrc, spec, audio_buf, audio_len) \
	LP5(0x2d6, SDL_AudioSpec *, SDL_LoadWAV_RW, SDL_RWops *, src, a0, int, freesrc, d0, SDL_AudioSpec *, spec, a1, Uint8 **, audio_buf, a2, Uint32 *, audio_len, a3, \
	, SDL_BASE_NAME)

#define SDL_LockAudio() \
	LP0NR(0x2f4, SDL_LockAudio, \
	, SDL_BASE_NAME)

#define SDL_LockSurface(surface) \
	LP1(0xf0, int, SDL_LockSurface, SDL_Surface *, surface, a0, \
	, SDL_BASE_NAME)

#define SDL_LockYUVOverlay(overlay) \
	LP1(0x12c, int, SDL_LockYUVOverlay, SDL_Overlay *, overlay, a0, \
	, SDL_BASE_NAME)

#define SDL_MapRGB(format, r, g, b) \
	LP4(0x96, Uint32, SDL_MapRGB, SDL_PixelFormat *, format, a0, Uint8, r, d0, Uint8, g, d1, Uint8, b, d2, \
	, SDL_BASE_NAME)

#define SDL_MapRGBA(format, r, g, b, a) \
	LP5(0x9c, Uint32, SDL_MapRGBA, SDL_PixelFormat *, format, a0, Uint8, r, d0, Uint8, g, d1, Uint8, b, d2, Uint8, a, d3, \
	, SDL_BASE_NAME)

#define SDL_MixAudio(dst, src, len, volume) \
	LP4NR(0x2ee, SDL_MixAudio, Uint8 *, dst, a0, const Uint8 *, src, a1, Uint32, len, d0, int, volume, d1, \
	, SDL_BASE_NAME)

#define SDL_NumJoysticks() \
	LP0(0x1e6, int, SDL_NumJoysticks, \
	, SDL_BASE_NAME)

#define SDL_OpenAudio(desired, obtained) \
	LP2(0x2c4, int, SDL_OpenAudio, SDL_AudioSpec *, desired, a0, SDL_AudioSpec *, obtained, a1, \
	, SDL_BASE_NAME)

#define SDL_PauseAudio(pause_on) \
	LP1NR(0x2d0, SDL_PauseAudio, int, pause_on, d0, \
	, SDL_BASE_NAME)

#define SDL_PeepEvents(events, numevents, action, mask) \
	LP4(0x1c8, int, SDL_PeepEvents, SDL_Event *, events, a0, int, numevents, d0, SDL_eventaction, action, d1, Uint32, mask, d2, \
	, SDL_BASE_NAME)

#define SDL_PollEvent(event) \
	LP1(0x1bc, int, SDL_PollEvent, SDL_Event *, event, a0, \
	, SDL_BASE_NAME)

#define SDL_PumpEvents() \
	LP0NR(0x1b6, SDL_PumpEvents, \
	, SDL_BASE_NAME)

#define SDL_PushEvent(event) \
	LP1(0x1ce, int, SDL_PushEvent, SDL_Event *, event, a0, \
	, SDL_BASE_NAME)

#define SDL_RealQuit() \
	LP0NR(0x24, SDL_Quit, \
	, SDL_BASE_NAME)

#define SDL_QuitSubSystem(flags) \
	LP1NR(0x30, SDL_QuitSubSystem, Uint32, flags, d0, \
	, SDL_BASE_NAME)

#define SDL_RWFromFP(fp, autoclose) \
	LP2(0x42, SDL_RWops *, SDL_RWFromFP, FILE *, fp, a0, int, autoclose, d0, \
	, SDL_BASE_NAME)

#define SDL_RWFromFile(file, mode) \
	LP2(0x3c, SDL_RWops *, SDL_RWFromFile, const char *, file, a0, const char *, mode, a1, \
	, SDL_BASE_NAME)

#define SDL_RWFromMem(mem, size) \
	LP2(0x48, SDL_RWops *, SDL_RWFromMem, void *, mem, a0, int, size, d0, \
	, SDL_BASE_NAME)

#define SDL_RemoveTimer(t) \
	LP1(0x1b0, SDL_bool, SDL_RemoveTimer, SDL_TimerID, t, d0, \
	, SDL_BASE_NAME)

#define SDL_SaveBMP_RW(surface, dst, freedst) \
	LP3(0xfc, int, SDL_SaveBMP_RW, SDL_Surface *, surface, a0, SDL_RWops *, dst, a1, int, freedst, d0, \
	, SDL_BASE_NAME)

#define SDL_SetAlpha(surface, flag, alpha) \
	LP3(0x108, int, SDL_SetAlpha, SDL_Surface *, surface, a0, Uint32, flag, d0, Uint8, alpha, d1, \
	, SDL_BASE_NAME)

#define SDL_SetClipRect(surface, rect) \
	LP2(0x10e, SDL_bool, SDL_SetClipRect, SDL_Surface *, surface, a0, const SDL_Rect *, rect, a1, \
	, SDL_BASE_NAME)

#define SDL_SetColorKey(surface, flag, key) \
	LP3(0x102, int, SDL_SetColorKey, SDL_Surface *, surface, a0, Uint32, flag, d0, Uint32, key, d1, \
	, SDL_BASE_NAME)

#define SDL_SetColors(surface, colors, firstcolor, ncolors) \
	LP4(0xcc, int, SDL_SetColors, SDL_Surface *, surface, a0, SDL_Color *, colors, a1, int, firstcolor, d0, int, ncolors, d1, \
	, SDL_BASE_NAME)

#define SDL_SetCursor(cursor) \
	LP1NR(0x282, SDL_SetCursor, SDL_Cursor *, cursor, a0, \
	, SDL_BASE_NAME)

#define SDL_SetError(fmt, args...) \
	({unsigned long _tags[] = { args }; SDL_SetErrorA(fmt, (void *)_tags);})	

#define SDL_SetErrorA(fmt, arglist) \
	LP2NR(0x2a0, SDL_SetErrorA, const char *, fmt, a0, unsigned long *, arglist, a1, \
	, SDL_BASE_NAME)

#define SDL_SetEventFilter(filter) \
	LP1NR(0x1d4, SDL_SetEventFilter, SDL_EventFilter, filter, d0, \
	, SDL_BASE_NAME)

#define SDL_SetGamma(red, green, blue) \
	LP3(0xba, int, SDL_SetGamma, float, red, d0, float, green, d1, float, blue, d2, \
	, SDL_BASE_NAME)

#define SDL_SetGammaRamp(red, green, blue) \
	LP3(0xc0, int, SDL_SetGammaRamp, Uint16 *, red, d0, Uint16 *, green, d1, Uint16 *, blue, d2, \
	, SDL_BASE_NAME)

#define SDL_SetModState(modstate) \
	LP1NR(0x25e, SDL_SetModState, SDLMod, modstate, d0, \
	, SDL_BASE_NAME)

#define SDL_SetPalette(surface, flags, colors, firstcolor, ncolors) \
	LP5(0xd2, int, SDL_SetPalette, SDL_Surface *, surface, a0, int, flags, d0, SDL_Color *, colors, a1, int, firstcolor, d1, int, ncolors, d2, \
	, SDL_BASE_NAME)

#define SDL_SetTimer(interval, callback) \
	LP2(0x1a4, int, SDL_SetTimer, Uint32, interval, d0, SDL_TimerCallback, callback, a0, \
	, SDL_BASE_NAME)

#define SDL_SetVideoMode(width, height, bpp, flags) \
	LP4(0x90, SDL_Surface *, SDL_SetVideoMode, int, width, d0, int, height, d1, int, bpp, d2, Uint32, flags, d3, \
	, SDL_BASE_NAME)

#define SDL_ShowCursor(toggle) \
	LP1(0x294, int, SDL_ShowCursor, int, toggle, d0, \
	, SDL_BASE_NAME)

#define SDL_SoftStretch(src, srcrect, dest, dstrect) \
	LP4(0x32a, int, SDL_SoftStretch, SDL_Surface *, src, a0, SDL_Rect *, srcrect, a1, SDL_Surface *, dest, a2, SDL_Rect *, dstrect, a3, \
	, SDL_BASE_NAME)

#define SDL_ThreadID() \
	LP0(0x30c, Uint32, SDL_ThreadID, \
	, SDL_BASE_NAME)

#define SDL_UnlockAudio() \
	LP0NR(0x2fa, SDL_UnlockAudio, \
	, SDL_BASE_NAME)

#define SDL_UnlockSurface(surface) \
	LP1NR(0xf6, SDL_UnlockSurface, SDL_Surface *, surface, a0, \
	, SDL_BASE_NAME)

#define SDL_UnlockYUVOverlay(overlay) \
	LP1NR(0x132, SDL_UnlockYUVOverlay, SDL_Overlay *, overlay, a0, \
	, SDL_BASE_NAME)

#define SDL_UpdateRect(screen, x, y, w, h) \
	LP5NR(0x84, SDL_UpdateRect, SDL_Surface *, screen, a0, Sint32, x, d0, Sint32, y, d1, Uint32, w, d2, Uint32, h, d3, \
	, SDL_BASE_NAME)

#define SDL_UpdateRects(screen, numrects, rects) \
	LP3NR(0x7e, SDL_UpdateRects, SDL_Surface *, screen, a0, int, numrects, d0, SDL_Rect *, rects, a1, \
	, SDL_BASE_NAME)

#define SDL_UpperBlit(src, srcrect, dst, dstrect) \
	LP4(0x72, int, SDL_UpperBlit, SDL_Surface *, src, a0, SDL_Rect *, srcrect, d0, SDL_Surface *, dst, a1, SDL_Rect *, dstrect, d1, \
	, SDL_BASE_NAME)

#define SDL_VideoDriverName(namebuf, maxlen) \
	LP2(0xa2, char *, SDL_VideoDriverName, char *, namebuf, a0, int, maxlen, d0, \
	, SDL_BASE_NAME)

#define SDL_VideoModeOK(width, height, bpp, flags) \
	LP4(0xae, int, SDL_VideoModeOK, int, width, d0, int, height, d1, int, bpp, d2, Uint32, flags, d3, \
	, SDL_BASE_NAME)

#define SDL_WM_GetCaption(title, icon) \
	LP2NR(0x17a, SDL_WM_GetCaption, char **, title, a0, char **, icon, a1, \
	, SDL_BASE_NAME)

#define SDL_WM_GrabInput(mode) \
	LP1(0x192, SDL_GrabMode, SDL_WM_GrabInput, SDL_GrabMode, mode, d0, \
	, SDL_BASE_NAME)

#define SDL_WM_IconifyWindow() \
	LP0(0x186, int, SDL_WM_IconifyWindow, \
	, SDL_BASE_NAME)

#define SDL_WM_SetCaption(title, icon) \
	LP2NR(0x174, SDL_WM_SetCaption, const char *, title, a0, const char *, icon, a1, \
	, SDL_BASE_NAME)

#define SDL_WM_SetIcon(icon, mask) \
	LP2NR(0x180, SDL_WM_SetIcon, SDL_Surface *, icon, a0, Uint8 *, mask, a1, \
	, SDL_BASE_NAME)

#define SDL_WM_ToggleFullScreen(surface) \
	LP1(0x18c, int, SDL_WM_ToggleFullScreen, SDL_Surface *, surface, a0, \
	, SDL_BASE_NAME)

#define SDL_WaitEvent(event) \
	LP1(0x1c2, int, SDL_WaitEvent, SDL_Event *, event, a0, \
	, SDL_BASE_NAME)

#define SDL_WaitThread(thread, status) \
	LP2NR(0x318, SDL_WaitThread, SDL_Thread *, thread, a0, int *, status, a1, \
	, SDL_BASE_NAME)

#define SDL_WarpMouse(x, y) \
	LP2NR(0x276, SDL_WarpMouse, Uint16, x, d0, Uint16, y, d1, \
	, SDL_BASE_NAME)

#define SDL_WasInit(flags) \
	LP1(0x36, Uint32, SDL_WasInit, Uint32, flags, d0, \
	, SDL_BASE_NAME)

#endif /* !_INLINE_SDL_H */
