
/* Copyright (c) Nate Robins, 1997. */

/* This program is freely distributable without licensing fees
   and is provided without guarantee or warrantee expressed or
   implied. This program is -not- in the public domain. */


#include "glutint.h"
#include "glutstroke.h"
#include "glutbitmap.h"
#if defined(__CYGWIN32__)
typedef MINMAXINFO* LPMINMAXINFO;
#else
#include <sys/timeb.h>
#endif

extern StrokeFontRec glutStrokeRoman, glutStrokeMonoRoman;
extern BitmapFontRec glutBitmap8By13, glutBitmap9By15, glutBitmapTimesRoman10, glutBitmapTimesRoman24, glutBitmapHelvetica10, glutBitmapHelvetica12, glutBitmapHelvetica18;

int
gettimeofday(struct timeval* tp, void* tzp)
{
#if defined(__CYGWIN32__)
  /* gettimeofday() is not to be implemented in cygwin.dll beta 18 (cf. _syslist.h) */
  /* we use GetTickCount() = number of milliseconds since windoze was started       */
  /* /Stephane Rehel, Nov.16.1997                                                   */
  long tc= GetTickCount();
  tp->tv_sec= tc / 1000L;
  tp->tv_sec= (tc % 1000L) * 1000L;
#else
  struct timeb tb;

  ftime(&tb);
  tp->tv_sec = tb.time;
  tp->tv_usec = tb.millitm * 1000;
#endif

  /* 0 indicates that the call succeeded. */
  return 0;
}

void*
__glutFont(void *font)
{
  switch((int)font) {
  case (int)GLUT_STROKE_ROMAN:
    return &glutStrokeRoman;
  case (int)GLUT_STROKE_MONO_ROMAN:
    return &glutStrokeMonoRoman;
  case (int)GLUT_BITMAP_9_BY_15:
    return &glutBitmap9By15;
  case (int)GLUT_BITMAP_8_BY_13:
    return &glutBitmap8By13;
  case (int)GLUT_BITMAP_TIMES_ROMAN_10:
    return &glutBitmapTimesRoman10;
  case (int)GLUT_BITMAP_TIMES_ROMAN_24:
    return &glutBitmapTimesRoman24;
  case (int)GLUT_BITMAP_HELVETICA_10:
    return &glutBitmapHelvetica10;
  case (int)GLUT_BITMAP_HELVETICA_12:
    return &glutBitmapHelvetica12;
  case (int)GLUT_BITMAP_HELVETICA_18:
    return &glutBitmapHelvetica18;
  }
}

int
__glutGetTransparentPixel(Display * dpy, XVisualInfo * vinfo)
{
  /* the transparent pixel on Win32 is always index number 0.  So if
     we put this routine in this file, we can avoid compiling the
     whole of layerutil.c which is where this routine normally comes
     from. */
  return 0;
}

void
__glutAdjustCoords(Window parent, int* x, int* y, int* width, int* height)
{
  RECT rect;

  /* adjust the window rectangle because Win32 thinks that the x, y,
     width & height are the WHOLE window (including decorations),
     whereas GLUT treats the x, y, width & height as only the CLIENT
     area of the window. */
  rect.left = *x; rect.top = *y;
  rect.right = *x + *width; rect.bottom = *y + *height;

  /* must adjust the coordinates according to the correct style
     because depending on the style, there may or may not be
     borders. */
  AdjustWindowRect(&rect, WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
		   (parent ? WS_CHILD : WS_OVERLAPPEDWINDOW),
		   FALSE);
  /* FALSE in the third parameter = window has no menu bar */

  /* readjust if the x and y are offscreen */
  if(rect.left < 0)
    *x = 0;
  else
    *x = rect.left;

  if(rect.top < 0)
    *y = 0;
  else
    *y = rect.top;

  *width = rect.right - rect.left;	/* adjusted width */
  *height = rect.bottom - rect.top;	/* adjusted height */
}

extern unsigned __glutMenuButton;
extern GLUTidleCB __glutIdleFunc;
extern GLUTtimer *__glutTimerList;
extern void handleTimeouts(void);
extern GLUTmenuItem *__glutGetUniqueMenuItem(GLUTmenu * menu, int unique);
static HMENU __glutHMenu;

LONG WINAPI __glutWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  POINT         point;			/* Point structure. */
  PAINTSTRUCT   ps;			/* Paint structure. */
  LPMINMAXINFO  minmax;			/* Minimum/maximum info structure. */
  GLUTwindow*   window;			/* GLUT window associated with message. */
  GLUTmenu*     menu;			/* GLUT menu associated with message. */
  int x, y, width, height, key;
  int button = -1;

  switch(msg) {
  case WM_CREATE:
    return 0;

  case WM_CLOSE:
    PostQuitMessage(0);
    return 0;

  case WM_DESTROY:
#if 0
    /* TODO: need to add this. */
    purgeStaleWindow(hwnd);
#endif
    return 0;

  case WM_PAINT:
    window = __glutGetWindow(hwnd);
    if (window) {
      BeginPaint(hwnd, &ps);		/* Must have this for some Win32 reason. */
#if TAKE_THIS_OUT
      if (window->colormap) {
	SelectPalette(window->hdc, window->colormap->cmap, FORCE_FOREGROUND);
	RealizePalette(window->hdc);	       /* Remap the custom palette. */
      }
#endif
      EndPaint(hwnd, &ps);
      if (window->win == hwnd) {
	__glutPostRedisplay(window, GLUT_REPAIR_WORK);
      } else if (window->overlay && window->overlay->win == hwnd) {
	__glutPostRedisplay(window, GLUT_OVERLAY_REPAIR_WORK);
      }
    }
    return 0;

  case WM_SYSCHAR:
  case WM_CHAR:
    window = __glutGetWindow(hwnd);
    if (!window) {
      break;
    }
    /* Win32 is dumb and sends these messages only to the parent
       window.  Therefore, find out if we're in a child window and
       call the child windows keyboard callback if we are. */
    if (window->parent) {
	GetCursorPos(&point);
	ScreenToClient(hwnd, &point);
	hwnd = ChildWindowFromPoint(hwnd, point);
	window = __glutGetWindow(hwnd);
    }
    if (window->keyboard) {
      GetCursorPos(&point);
      ScreenToClient(window->win, &point);
      __glutSetWindow(window);
      __glutModifierMask = 0;
      if (GetKeyState(VK_SHIFT) < 0)	/* < 0 = high order bit is on */
	__glutModifierMask |= ShiftMask;
      if (GetKeyState(VK_CONTROL) < 0)
	__glutModifierMask |= ControlMask;
      if (GetKeyState(VK_MENU) < 0)
	__glutModifierMask |= Mod1Mask;
      window->keyboard((char)wParam, point.x, point.y);
      __glutModifierMask = (unsigned int) ~0;
    }
    return 0;

  case WM_SYSKEYDOWN:
  case WM_KEYDOWN:
    window = __glutGetWindow(hwnd);
    if (!window) {
      break;
    }
    /* Win32 is dumb and sends these messages only to the parent
       window.  Therefore, find out if we're in a child window and
       call the child windows keyboard callback if we are. */
    if (window->parent) {
	GetCursorPos(&point);
	ScreenToClient(hwnd, &point);
	hwnd = ChildWindowFromPoint(hwnd, point);
	window = __glutGetWindow(hwnd);
    }
    if (window->special) {
      switch (wParam) {
	/* *INDENT-OFF* */
	/* function keys */
	case VK_F1:     key = GLUT_KEY_F1; break;
	case VK_F2:     key = GLUT_KEY_F2; break;
	case VK_F3:     key = GLUT_KEY_F3; break;
	case VK_F4:     key = GLUT_KEY_F4; break;
	case VK_F5:     key = GLUT_KEY_F5; break;
	case VK_F6:     key = GLUT_KEY_F6; break;
	case VK_F7:     key = GLUT_KEY_F7; break;
	case VK_F8:     key = GLUT_KEY_F8; break;
	case VK_F9:     key = GLUT_KEY_F9; break;
	case VK_F10:    key = GLUT_KEY_F10; break;
	case VK_F11:    key = GLUT_KEY_F11; break;
	case VK_F12:    key = GLUT_KEY_F12; break;
	/* directional keys */
	case VK_LEFT:   key = GLUT_KEY_LEFT; break;
	case VK_UP:     key = GLUT_KEY_UP; break;
	case VK_RIGHT:  key = GLUT_KEY_RIGHT; break;
	case VK_DOWN:   key = GLUT_KEY_DOWN; break;
	/* *INDENT-ON* */

	case VK_PRIOR:
	  /* VK_PRIOR is Win32's Page Up */
	  key = GLUT_KEY_PAGE_UP;
	  break;
	case VK_NEXT:
	  /* VK_NEXT is Win32's Page Down */
	  key = GLUT_KEY_PAGE_DOWN;
	  break;
	case VK_HOME:
	  key = GLUT_KEY_HOME;
	  break;
	case VK_END:
	  key = GLUT_KEY_END;
	  break;
	case VK_INSERT:
	  key = GLUT_KEY_INSERT;
	  break;
	default:
	  goto defproc;
      }
      GetCursorPos(&point);
      ScreenToClient(window->win, &point);
      __glutSetWindow(window);
      __glutModifierMask = 0;
      if (GetKeyState(VK_SHIFT) < 0)	/* < 0 = high order bit is on */
	__glutModifierMask |= ShiftMask;
      if (GetKeyState(VK_CONTROL) < 0)
	__glutModifierMask |= ControlMask;
      if (GetKeyState(VK_MENU) < 0)
	__glutModifierMask |= Mod1Mask;
      window->special(key, point.x, point.y);
      __glutModifierMask = (unsigned int) ~0;
    }
    return 0;

  case WM_LBUTTONDOWN:
    button = GLUT_LEFT_BUTTON;
  case WM_MBUTTONDOWN:
    if (button < 0)
      button = GLUT_MIDDLE_BUTTON;
  case WM_RBUTTONDOWN:
    if (button < 0)
      button = GLUT_RIGHT_BUTTON;

    /* finish the menu if we get a button down message (user must have
       cancelled the menu). */
    if (__glutMappedMenu) {
      /* TODO: take this out once the menu on middle mouse stuff works
	 properly. */
      if (button == GLUT_MIDDLE_BUTTON)
	return 0;
      GetCursorPos(&point);
      ScreenToClient(hwnd, &point);
      __glutItemSelected = NULL;
      __glutFinishMenu(hwnd, point.x, point.y);
      return 0;
    }

    /* set the capture so we can get mouse events outside the window */
    SetCapture(hwnd);

    /* Win32 doesn't return the same numbers as X does when the mouse
       goes beyond the upper or left side of the window.  roll the
       Win32's 0..2^16 pointer co-ord range to 0 +/- 2^15. */
    x = LOWORD(lParam);
    y = HIWORD(lParam);
    if(x & 1 << 15) x -= (1 << 16);
    if(y & 1 << 15) y -= (1 << 16);
	
    window = __glutGetWindow(hwnd);
    if (window) {
      menu = __glutGetMenuByNum(window->menu[button]);
      if (menu) {
	point.x = LOWORD(lParam); point.y = HIWORD(lParam);
	ClientToScreen(window->win, &point);
	__glutMenuButton = button == GLUT_RIGHT_BUTTON ? TPM_RIGHTBUTTON :
                           button == GLUT_LEFT_BUTTON  ? TPM_LEFTBUTTON :
                           0x0001;
	__glutStartMenu(menu, window, point.x, point.y, x, y);
      } else if (window->mouse) {
	
        __glutSetWindow(window);
	__glutModifierMask = 0;
	if (GetKeyState(VK_SHIFT) < 0)	/* < 0 = high order bit is on. */
	  __glutModifierMask |= ShiftMask;
	if (GetKeyState(VK_CONTROL) < 0)
	  __glutModifierMask |= ControlMask;
	if (GetKeyState(VK_MENU) < 0)
	  __glutModifierMask |= Mod1Mask;
	window->mouse(button, GLUT_DOWN, x, y);
	__glutModifierMask = (unsigned int)~0;
      } else {
	/* Stray mouse events.  Ignore. */
      }
    }
    return 0;

  case WM_LBUTTONUP:
    button = GLUT_LEFT_BUTTON;
  case WM_MBUTTONUP:
    if (button < 0)
      button = GLUT_MIDDLE_BUTTON;
  case WM_RBUTTONUP:
    if (button < 0)
      button = GLUT_RIGHT_BUTTON;

    /* Bail out if we're processing a menu. */
    if (__glutMappedMenu) {
      GetCursorPos(&point);
      ScreenToClient(hwnd, &point);
      /* if we're getting the middle button up signal, then something
	 on the menu was selected. */
      if (button == GLUT_MIDDLE_BUTTON) {
	return 0;
	/* For some reason, the code below always returns -1 even
	   though the point IS IN THE ITEM!  Therefore, just bail out if
	   we get a middle mouse up.  The user must select using the
	   left mouse button.  Stupid Win32. */
#if 0
 	int item = MenuItemFromPoint(hwnd, __glutHMenu, point);
 	printf("item = %d %d %d\n", item, point.x, point.y);
 	if (item != -1)
 	  __glutItemSelected = (GLUTmenuItem*)GetMenuItemID(__glutHMenu, item);
 	else
 	  __glutItemSelected = NULL;
 	__glutFinishMenu(hwnd, point.x, point.y);
#endif
      } else {
	__glutItemSelected = NULL;
	__glutFinishMenu(hwnd, point.x, point.y);
      }
      return 0;
    }

    /* Release the mouse capture. */
    ReleaseCapture();

    window = __glutGetWindow(hwnd);
    if (window && window->mouse) {
      /* Win32 doesn't return the same numbers as X does when the
	 mouse goes beyond the upper or left side of the window.  roll
	 the Win32's 0..2^16 pointer co-ord range to 0 +/- 2^15. */
      x = LOWORD(lParam);
      y = HIWORD(lParam);
      if(x & 1 << 15) x -= (1 << 16);
      if(y & 1 << 15) y -= (1 << 16);

      __glutSetWindow(window);
      __glutModifierMask = 0;
      if (GetKeyState(VK_SHIFT) < 0)	/* < 0 = high order bit is on */
	__glutModifierMask |= ShiftMask;
      if (GetKeyState(VK_CONTROL) < 0)
	__glutModifierMask |= ControlMask;
      if (GetKeyState(VK_MENU) < 0)
	__glutModifierMask |= Mod1Mask;
      window->mouse(button, GLUT_UP, x, y);
      __glutModifierMask = (unsigned int)~0;
    } else {
      /* Window might have been destroyed and all the
	 events for the window may not yet be received. */
    }
    return 0;

  case WM_ENTERMENULOOP:
    /* KLUDGE: create a timer that fires every 100 ms when we start a
       menu so that we can still process the idle & timer events (that
       way, the timers will fire during a menu pick and so will the
       idle func. */
    SetTimer(hwnd, 1, 1, NULL);
    return 0;

  case WM_TIMER:
#if 0
    /* if the timer id is 2, then this is the timer that is set up in
       the main glut message processing loop, and we don't want to do
       anything but acknowledge that we got it.  It is used to prevent
       CPU spiking when an idle function is installed. */
    if (wParam == 2)
      return 0;
#endif

    /* only worry about the idle function and the timeouts, since
       these are the only events we expect to process during
       processing of a menu. */
    /* we no longer process the idle functions (as outlined in the
       README), since drawing can't be done until the menu has
       finished...it's pretty lame when the animation goes on, but
       doesn't update, so you get this weird jerkiness. */
#if 0
     if (__glutIdleFunc)
       __glutIdleFunc();
#endif
    if (__glutTimerList)
      handleTimeouts();
    return 0;

  case WM_EXITMENULOOP:
    /* nuke the above created timer...we don't need it anymore, since
       the menu is gone now. */
    KillTimer(hwnd, 1);
    return 0;

  case WM_MENUSELECT:
    if (lParam != 0)
      __glutHMenu = (HMENU)lParam;
    return 0;

  case WM_COMMAND:
    if (__glutMappedMenu) {
      if (GetSubMenu(__glutHMenu, LOWORD(wParam)))
	__glutItemSelected = NULL;
      else
	__glutItemSelected =
	  __glutGetUniqueMenuItem(__glutMappedMenu, LOWORD(wParam));
      GetCursorPos(&point);
      ScreenToClient(hwnd, &point);
      __glutFinishMenu(hwnd, point.x, point.y);
    }
    return 0;

  case WM_MOUSEMOVE:
    if (!__glutMappedMenu) {
      window = __glutGetWindow(hwnd);
      if (window) {
          /* If motion function registered _and_ buttons held *
             down, call motion function...  */
	x = LOWORD(lParam);
	y = HIWORD(lParam);

	/* Win32 doesn't return the same numbers as X does when the
	   mouse goes beyond the upper or left side of the window.
	   roll the Win32's 0..2^16 pointer co-ord range to 0..+/-2^15. */
	if(x & 1 << 15) x -= (1 << 16);
	if(y & 1 << 15) y -= (1 << 16);

	if (window->motion && wParam &
            (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON)) {
	  __glutSetWindow(window);
	  window->motion(x, y);
	}
	/* If passive motion function registered _and_
	   buttons not held down, call passive motion
	   function...  */
	else if (window->passive &&
		 ((wParam &
		   (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON)) ==
		  0)) {
	  __glutSetWindow(window);
	  window->passive(x, y);
	}
      }
    } else {
      /* Motion events are thrown away when a pop up menu is
	 active. */
    }
    return 0;

  case WM_GETMINMAXINFO:
    /* this voodoo is brought to you by Win32 (again).  It allows the
       window to be bigger than the screen, and smaller than 100x100
       (although it doesn't seem to help the y minimum). */
    minmax = (LPMINMAXINFO)lParam;
    minmax->ptMaxSize.x = __glutScreenWidth;
    minmax->ptMaxSize.y = __glutScreenHeight;
    minmax->ptMinTrackSize.x = 0;
    minmax->ptMinTrackSize.y = 0;
    minmax->ptMaxTrackSize.x = __glutScreenWidth +
      GetSystemMetrics(SM_CXSIZE) * 2;
    minmax->ptMaxTrackSize.y = __glutScreenHeight +
      GetSystemMetrics(SM_CXSIZE) * 2 + GetSystemMetrics(SM_CYCAPTION);
    return 0;

  case WM_SIZE:
    window = __glutGetWindow(hwnd);
    if (window) {
#if 0
      if (window->win != hwnd) {
	/* Ignore ConfigureNotify sent to the overlay planes.
	   GLUT could get here because overlays select for
	   StructureNotify events to receive DestroyNotify. */
	break;
      }
#endif
      width = LOWORD(lParam);
      height = HIWORD(lParam);
      if (width != window->width || height != window->height) {
#if 0
 	if (window->overlay) {
 	  XResizeWindow(__glutDisplay, window->overlay->win, width, height);
 	}
#endif
	window->width = width;
	window->height = height;
	__glutSetWindow(window);
	/* Do not execute OpenGL out of sequence with respect
	   to the SetWindowPos request! */
	GdiFlush();
	window->reshape(width, height);
	window->forceReshape = FALSE;
	/* A reshape should be considered like posting a
	   repair request. */
	__glutPostRedisplay(window, GLUT_REPAIR_WORK);
      }
    }
    return 0;

  case WM_SETCURSOR:
    /* If the cursor is not in the client area, then we want to send
       this message to the default window procedure ('cause its
       probably in the border or title, and we don't handle that
       cursor.  otherwise, set our cursor.  Win32 makes us set the
       cursor every time the mouse moves (DUMB!). */
    if(LOWORD(lParam) != HTCLIENT)
      goto defproc;
    window = __glutGetWindow(hwnd);
    if (window) {
      /* Since Win32 allows the parent to control a child windows
	 cursor, if the cursor is in a child of this window, bail
	 out. */
      GetCursorPos(&point);
      ScreenToClient(hwnd, &point);
      if (hwnd != ChildWindowFromPoint(hwnd, point))
	break;
      __glutCurrentWindow = window;
      glutSetCursor(window->cursor);
    }
    /* TODO: check out the info in DevStudio on WM_SETCURSOR in the
       DefaultAction section. */
    return 1;

  case WM_SETFOCUS:
    window = __glutGetWindow(hwnd);
    if (window) {
      if (window->entry) {
	window->entryState = WM_SETFOCUS;
	__glutSetWindow(window);
	window->entry(GLUT_ENTERED);
	/* XXX Generation of fake passive notify?  See how    much
	   work the X11 code does to support fake passive    notify
	   callbacks. */
      }
    }
    return 0;

  case WM_KILLFOCUS:
    window = __glutGetWindow(hwnd);
    if (window) {
      if (window->entry) {
	window->entryState = WM_KILLFOCUS;
	__glutSetWindow(window);
	window->entry(GLUT_LEFT);
      }
    }
    return 0;

  case WM_ACTIVATE:
    window = __glutGetWindow(hwnd);
    /* make sure we re-select the correct palette if needed */
    if (LOWORD(wParam)) {
      PostMessage(hwnd, WM_PALETTECHANGED, 0, 0);
    }
    if (window) {
      GLUTwindow* child;
      int visState;
      visState = !IsIconic(window->win);
      if (visState) {			/* Not iconic. */
	visState = IsWindowVisible(window->win);
      }
      if (visState != window->visState) {
	if (window->windowStatus) {
	  window->visState = visState;
	  __glutSetWindow(window);
	  window->windowStatus(visState);
	}
	/* Since Win32 only sends an activate for the toplevel window,
	   update the visibility for all the child windows. */
	child = window->children;
	while (child) {
	  child->visState = visState;
	  if (child->windowStatus) {
	    child->visState = visState;
	    __glutSetWindow(child);
	    child->windowStatus(visState);
	  }
	  child = child->siblings;
	}
      }
    }
    return 0;

  /* Colour Palette Management */
  case WM_PALETTECHANGED:
    if (hwnd == (HWND)wParam)  /* don't respond to the message that we sent! */
      break;
    /* fall through to WM_QUERYNEWPALETTE */

  case WM_QUERYNEWPALETTE:
    window = __glutGetWindow(hwnd);
    if (window && window->colormap) {
      UnrealizeObject(window->colormap->cmap);
      SelectPalette(window->hdc, window->colormap->cmap, FALSE);
      RealizePalette(window->hdc);
      return TRUE;
    }
    return FALSE;

#if 0
  /* Miscellaneous messages (don't really need to enumerate them,
     but it's good to know what you're not getting sometimes.) */
  case WM_NCHITTEST:
    /* this event is generated by every mouse move event. */
    goto defproc;
  case WM_NCMOUSEMOVE:
    goto defproc;
  case WM_NCACTIVATE:
    goto defproc;
  case WM_NCPAINT:
    goto defproc;
  case WM_NCCALCSIZE:
    goto defproc;
  case WM_NCCREATE:
    goto defproc;
  case WM_NCDESTROY:
    goto defproc;
  case WM_NCLBUTTONDOWN:
    goto defproc;
  case WM_SETTEXT:
    goto defproc;
  case WM_GETTEXT:
    goto defproc;
  case WM_ACTIVATEAPP:
    goto defproc;
  case WM_GETICON:
    goto defproc;
  case WM_ERASEBKGND:
    goto defproc;
  case WM_WINDOWPOSCHANGING:
    goto defproc;
  case WM_WINDOWPOSCHANGED:
    goto defproc;
  case WM_MOUSEACTIVATE:
    goto defproc;
  case WM_SHOWWINDOW:
    goto defproc;
  case WM_MOVING:
    goto defproc;
  case WM_MOVE:
    goto defproc;
  case WM_KEYUP:
    goto defproc;
  case WM_CAPTURECHANGED:
    goto defproc;
  case WM_SYSCOMMAND:
    goto defproc;
  case WM_ENTERSIZEMOVE:
    goto defproc;
  case WM_ENTERIDLE:
    goto defproc;
#endif

  default:
    goto defproc;
  }

defproc:
  return DefWindowProc(hwnd, msg, wParam, lParam);
}
