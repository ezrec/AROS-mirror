/*
 * $Header$
 * $Log$
 * Revision 1.1  2001/04/04 05:43:36  wang
 * First commit: compiles on Linux, Amiga, Windows, Windows CE, generic gcc
 *
 * Revision 1.1  1999/09/13 15:06:41  bnv
 * Initial revision
 *
 */

#include <stdlib.h>
#include <string.h>
#include <windows.h>

/*
 * These are dummy variables that are used because this module is
 *   always linked in.
 */

HINSTANCE	_hInstance;
HINSTANCE	_hPrevInstance;

POINT	_WindowOrg  = { CW_USEDEFAULT, CW_USEDEFAULT};
POINT	_WindowSize = { CW_USEDEFAULT, CW_USEDEFAULT};

POINT	_ScreenSize = { 80, 25 };		// Screen buffer dimensions
POINT	_Cursor = { 0, 0 };			// Cursor location
POINT	_Origin = { 0, 0 };			// Client area origin
LPSTR	_InactiveTitle = "(Inactive %s)";	// Inactive window title
BOOL	_AutoTracking = TRUE;			// Track cursor on Write?
BOOL	_CheckEOF = TRUE;			// Allow Ctrl-Z for EOF?
BOOL	_CheckBreak = TRUE;			// Allow Ctrl-C for break?
HFONT	_hFont;						// Current working font

char	_WindowTitle[80];			// Window title

void	_DoneWinIO(void);

BOOL	_KeyPressed(void);
int	_ReadKey(void);
WORD	_ReadBuf(char *Buffer, WORD Count);

void	gotoxy(int X, int Y);
int	wherex(void);
int	wherey(void);
void	clrscr(void);
void	clreol(void);

void	_CursorTo(int X, int Y);
void	_ScrollTo(int X, int Y);
void	_TrackCursor(void);

void	_InitWinIO(HINSTANCE,HINSTANCE,int);

// MinMaxInfo array

typedef POINT TMinMaxInfo[5];

// Scroll key definition record

typedef struct
{
	BYTE Key;
	BOOL Ctrl;
	BYTE SBar;
	BYTE Action;
} TScrollKey;

// Window I/O procedure

long PASCAL _WinIOProc(HWND Window, UINT Message,
				WPARAM WParam, LONG LParam);

// CRT window class

static WNDCLASS CrtClass =
{
	CS_HREDRAW | CS_VREDRAW, _WinIOProc, 0, 0, 0, 0, 0, 0,
	NULL, TEXT("WinIO")
};

static int	FirstLine  = 0;		// First line in circular buffer
static int	KeyCount   = 0;		// Count of keys in KeyBuffer
static BOOL	Created = FALSE;	// Window created?
static BOOL	Focused = FALSE;	// Window focused?
static BOOL	Reading = FALSE;	// Reading from window?
static BOOL	Painting = FALSE;	// Handling wm_Paint?

static HWND	CrtWindow = 0;		// CRT window handle
static char	*ScreenBuffer;		// Screen buffer pointer
static POINT	ClientSize;		// Client area dimensions
static POINT	Range;			// Scroll bar ranges
static POINT	CharSize;		// Character cell size
static int	CharAscent;		// Character ascent
static HDC	DC;			// Global device context
static PAINTSTRUCT	PS;		// Global paint structure
static HFONT	SaveFont;		// Saved device context font
static char	KeyBuffer[64];		// Keyboard type-ahead buffer

// Scroll keys table
#define SCROLLKEYCOUNT	12
static TScrollKey ScrollKeys[SCROLLKEYCOUNT]  =
	{
		{ VK_LEFT,  FALSE, SB_HORZ, SB_LINEUP },
		{ VK_RIGHT, FALSE, SB_HORZ, SB_LINEDOWN },
		{ VK_LEFT,  TRUE,  SB_HORZ, SB_PAGEUP },
		{ VK_RIGHT, TRUE,  SB_HORZ, SB_PAGEDOWN },
		{ VK_HOME,  FALSE, SB_HORZ, SB_TOP },
		{ VK_END,   FALSE, SB_HORZ, SB_BOTTOM },
		{ VK_UP,    FALSE, SB_VERT, SB_LINEUP },
		{ VK_DOWN,  FALSE, SB_VERT, SB_LINEDOWN },
		{ VK_PRIOR, FALSE, SB_VERT, SB_PAGEUP },
		{ VK_NEXT,  FALSE, SB_VERT, SB_PAGEDOWN },
		{ VK_HOME,  TRUE,  SB_VERT, SB_TOP },
		{ VK_END,   TRUE,  SB_VERT, SB_BOTTOM }
	};

/* ------------ Allocate device context ----------- */
static void
InitDeviceContext(void)
{
	if (Painting)
		DC = BeginPaint(CrtWindow, &PS);
	else
		DC = GetDC(CrtWindow);
//	SaveFont = (HFONT) SelectObject( DC, GetStockObject(SYSTEM_FIXED_FONT));
	SaveFont = (HFONT)SelectObject (DC, _hFont);
} /* InitDeviceContext */

/* ------------ Release device context ------------ */
static void
DoneDeviceContext(void)
{
	SelectObject(DC, SaveFont);
	if (Painting)
		EndPaint(CrtWindow, &PS);
	else
		ReleaseDC(CrtWindow, DC);
} /* DoneDeviceContext */

/* ---------- Show caret --------- */
static void
_ShowCursor(void)
{
	CreateCaret(CrtWindow, 0, CharSize.x, 2);
	SetCaretPos((_Cursor.x - _Origin.x) * CharSize.x,
		(_Cursor.y - _Origin.y) * CharSize.y + CharAscent);
	ShowCaret(CrtWindow);
} /* _ShowCursor */

/* ----------- Hide caret ------------ */
static void
_HideCursor(void)
{
	DestroyCaret();
} /* _HideCursor */

/* ---------- Update scroll bars --------- */
static void
SetScrollBars(void)
{
	SetScrollRange(CrtWindow, SB_HORZ, 0, max(1, Range.x), FALSE);
	SetScrollPos(CrtWindow, SB_HORZ, _Origin.x, TRUE);
	SetScrollRange(CrtWindow, SB_VERT, 0, max(1, Range.y), FALSE);
	SetScrollPos(CrtWindow, SB_VERT, _Origin.y, TRUE);
} /* SetScrollBars */

/* --------- Terminate window ----------- */
static void
Terminate(void)
{
	if (Focused && Reading)
		_HideCursor();
//?	exit(255);
} /* Terminate */

/* --------- Set cursor position ---------- */
void
_CursorTo(int X, int Y)
{
	_Cursor.x = max(0, min(X, _ScreenSize.x - 1));
	_Cursor.y = max(0, min(Y, _ScreenSize.y - 1));
}

/* ------- Scroll window to given origin -------- */
void
_ScrollTo(int X, int Y)
{
	if (Created) {
		X = max(0, min(X, Range.x));
		Y = max(0, min(Y, Range.y));
		if ((X != _Origin.x) || (Y != _Origin.y)) {
			if (X != _Origin.x)
				SetScrollPos(CrtWindow, SB_HORZ, X, TRUE);
			if (Y != _Origin.y)
				SetScrollPos(CrtWindow, SB_VERT, Y, TRUE);
			ScrollWindow(CrtWindow,
				(_Origin.x - X) * CharSize.x,
				(_Origin.y - Y) * CharSize.y, NULL, NULL);
			_Origin.x = X;
			_Origin.y = Y;
			UpdateWindow(CrtWindow);
		}
	}
} /* _ScrollTo */

/* ------ Scroll to make cursor visible --------- */
void
_TrackCursor(void)
{
	_ScrollTo(max(_Cursor.x - ClientSize.x + 1,
		min(_Origin.x, _Cursor.x)),
		max(_Cursor.y - ClientSize.y + 1,
		min(_Origin.y, _Cursor.y)));
} /* _TrackCursor */

/* ------ Return pointer to location in screen buffer ----------- */
static LPWSTR
ScreenPtr(int X, int Y)
{
	Y += FirstLine;
	if (Y >= _ScreenSize.y)
		Y -= _ScreenSize.y;
	return (ScreenBuffer+ (Y * _ScreenSize.x + X));
} /* ScreenPtr */

/* ------ Update text on cursor line -------- */
static void
ShowText(int L, int R)
{
	if (L < R) {
		InitDeviceContext();
		ExtTextOut(DC, (L - _Origin.x) * CharSize.x,
			(_Cursor.y - _Origin.y) * CharSize.y,
			ETO_OPAQUE, NULL,
			ScreenPtr(L, _Cursor.y), R - L);
//?		TextOut(DC, (L - _Origin.x) * CharSize.x,
//?			(_Cursor.y - _Origin.y) * CharSize.y,
//?			ScreenPtr(L, _Cursor.y), R - L);
		DoneDeviceContext();
	}
} /* ShowText */

/* ------ Write text buffer to window -------- */
static void
NewLine(int *L, int *R)
{
	ShowText(*L, *R);
	*L = 0;
	*R = 0;
	_Cursor.x = 0;
	++_Cursor.y;
	if (_Cursor.y == _ScreenSize.y) {
		--_Cursor.y;
		++FirstLine;
		if (FirstLine == _ScreenSize.y)
			FirstLine = 0;
		memset(ScreenPtr(0, _Cursor.y), ' ', _ScreenSize.x);
		ScrollWindow(CrtWindow, 0, -CharSize.y, NULL, NULL);
		UpdateWindow(CrtWindow);
	}
} /* NewLine */

/* --------- Write the contents of Buffer ----------- */
void
_WriteBuf(char *Buffer, WORD Count)
{
	int L, R;

	L = _Cursor.x;
	R = _Cursor.x;
	while (Count > 0) {
		if (Buffer[0] == -1)
			Buffer[0] = ' ';
		switch (Buffer[0]) {
			case 13:
				NewLine(&L, &R);
				break;
			case 10:
				break;
			case  9:
				do {
					*(ScreenPtr(_Cursor.x,_Cursor.y))=' ';
					++_Cursor.x;
					if (_Cursor.x > R)
						R = _Cursor.x;
					if (_Cursor.x == _ScreenSize.x) {
						NewLine(&L, &R);
						break;
					}
				} while (_Cursor.x % 8);
				break;
			case  8:
				if (_Cursor.x > 0) {
					--_Cursor.x;
					*(ScreenPtr(_Cursor.x,_Cursor.y))=' ';
					if (_Cursor.x < L )
						L = _Cursor.x;
					}
				break;
			case  7:
				MessageBeep(0);
				break;
			default:
				*(ScreenPtr(_Cursor.x, _Cursor.y)) = Buffer[0];
				++_Cursor.x;
				if (_Cursor.x > R)
					R = _Cursor.x;
				if (_Cursor.x == _ScreenSize.x)
					NewLine(&L, &R);
		}

		++Buffer;
		--Count;
	}
	ShowText(L, R);
	if (_AutoTracking)
		_TrackCursor();
} /* _WriteBuf */

/* ------- Write character to window ---------- */
void
_WriteChar(char Ch)
{
	_WriteBuf(&Ch, 1);
} /* _WriteChar */

/* ------ Return keyboard status -------- */
BOOL
_KeyPressed(void)
{
	MSG M;

	while (PeekMessage(&M, 0, 0, 0, PM_REMOVE)) {
		if (M.message == WM_QUIT)
			Terminate();
		TranslateMessage(&M);
		DispatchMessage(&M);
	}
	return (BOOL)( KeyCount > 0 );
} /* _KeyPressed */

/* ------- Read key from window ---------- */
int
_ReadKey(void)
{
	int readkey;

	_TrackCursor();
	if (!_KeyPressed()) {
		Reading = TRUE;
		if (Focused)
			_ShowCursor();
		do { } while (!_KeyPressed());
		if (Focused)
			_HideCursor();
		Reading = FALSE;
	}
	readkey = KeyBuffer[0];
	--KeyCount;
	memmove(KeyBuffer, KeyBuffer+1, KeyCount);
	return readkey;
} /* _ReadKey */

/* ------ Read text buffer from window --------- */
WORD
_ReadBuf(char *Buffer, WORD Count)
{
	unsigned char Ch;
	WORD I;

	I = 0;
	do {
		Ch = _ReadKey();
		if (Ch == 8) {
			if (I > 0) {
				--I;
				_WriteChar(8);
			}
		} else
		if (Ch >= 32) {
			if (I < Count) {
				Buffer[I++] = Ch;
				_WriteChar(Ch);
			}
		}
	} while (!((Ch == 13) || (_CheckEOF && (Ch == 26))));
	if (I < Count - 2) {
		Buffer[I++] = Ch;
		if (Ch == 13) {
			Buffer[I++] = 10;
			_WriteChar(13);
		}
	}
	_TrackCursor();
	return I;
} /* _ReadBuf */

/* -------- Set cursor position ------------- */
void
gotoxy(int X, int Y)
{
	_CursorTo(X - 1, Y - 1);
} /* gotoxy */

/* ------- Return cursor X position --------- */
int
wherex(void)
{
	return (_Cursor.x + 1);
} /* wherex */

/* ------- Return cursor Y position ------- */
int
wherey(void)
{
	return(_Cursor.y + 1);
} /* wherey */

/* ------ Clear screen ------ */
void
clrscr(void)
{
	memset(ScreenBuffer, ' ', _ScreenSize.x * _ScreenSize.y);
	_Cursor.x = 0;
	_Cursor.y = 0;
	_Origin.x = 0;
	_Origin.y = 0;
	SetScrollBars();
	InvalidateRect(CrtWindow, NULL, TRUE);
	UpdateWindow(CrtWindow);
} /* clrscr */

/* ------ Clear to end of line --------- */
void
clreol(void)
{
	memset(ScreenPtr(_Cursor.x,_Cursor.y),' ',_ScreenSize.x-_Cursor.x);
	ShowText(_Cursor.x, _ScreenSize.x);
} /* clreol */

/* ---- WM_CREATE message handler ---- */
static void
WindowCreate(void)
{
	Created = TRUE;
	ScreenBuffer = (char *) malloc(_ScreenSize.x * _ScreenSize.y);
	memset(ScreenBuffer, ' ', _ScreenSize.x * _ScreenSize.y);
	if (!_CheckBreak)
		EnableMenuItem(GetSystemMenu(CrtWindow, FALSE), SC_CLOSE,
			MF_DISABLED | MF_GRAYED);
} /* WindowCreate */

/* ---- WM_PAINT message handler ----- */
static void
WindowPaint(void)
{
	int X1, X2, Y1, Y2;

	Painting = TRUE;
	InitDeviceContext();
	X1 = max(0, PS.rcPaint.left / CharSize.x + _Origin.x);
	X2 = min(_ScreenSize.x,
		(PS.rcPaint.right + CharSize.x - 1) / CharSize.x + _Origin.x);
	Y1 = max(0, PS.rcPaint.top / CharSize.y + _Origin.y);
	Y2 = min(_ScreenSize.y,
		(PS.rcPaint.bottom + CharSize.y - 1) / CharSize.y + _Origin.y);
	while (Y1 < Y2) {
		TextOut(DC, (X1 - _Origin.x) * CharSize.x, (Y1 - _Origin.y) * CharSize.y,
		ScreenPtr(X1, Y1), X2 - X1);
		++Y1;
	}
	DoneDeviceContext();
	Painting = FALSE;
} /* WindowPaint */

/* ---- WM_VSCROLL and WM_HSCROLL message handler ----- */
static int
GetNewPos(int Pos, int Page, int Range, int Action, int Thumb)
{
	switch (Action) {
		case SB_LINEUP:
			return(Pos - 1);
		case SB_LINEDOWN:
			return(Pos + 1);
		case SB_PAGEUP:
			return(Pos - Page);
		case SB_PAGEDOWN:
			return(Pos + Page);
		case SB_TOP:
			return(0);
		case SB_BOTTOM:
			return(Range);
		case SB_THUMBPOSITION:
			return(Thumb);
		default:
			return(Pos);
	}
} /* GetNewPos */

/* ------ WindowScroll ------- */
static void
WindowScroll(int Which, int Action, int Thumb)
{
	int X, Y;

	X = _Origin.x;
	Y = _Origin.y;
	switch (Which) {
		case SB_HORZ:
			X = GetNewPos(X, ClientSize.x / 2, Range.x, Action, Thumb);
			break;
		case SB_VERT:
			Y = GetNewPos(Y, ClientSize.y, Range.y, Action, Thumb);
	}
	_ScrollTo(X, Y);
} /* WindowScroll */

/* ------ WM_SIZE message handler -------- */
static void
WindowResize(int X, int Y)
{
	if (Focused && Reading)
		_HideCursor();
	ClientSize.x = X / CharSize.x;
	ClientSize.y = Y / CharSize.y;
	Range.x = max(0, _ScreenSize.x - ClientSize.x);
	Range.y = max(0, _ScreenSize.y - ClientSize.y);
	_Origin.x = min(_Origin.x, Range.x);
	_Origin.y = min(_Origin.y, Range.y);
	SetScrollBars();
	if (Focused && Reading)
		_ShowCursor();
} /* WindowResize */

/* ------ WM_GETMINMAXINFO message handler ------ */
static void
WindowMinMaxInfo(TMinMaxInfo* MinMaxInfo)
{
	int X, Y;
	TEXTMETRIC Metrics;

	InitDeviceContext();
	GetTextMetrics(DC, &Metrics);
	CharSize.x = Metrics.tmMaxCharWidth;
	CharSize.y = Metrics.tmHeight + Metrics.tmExternalLeading;
	CharAscent = Metrics.tmAscent;
	X = min(_ScreenSize.x * CharSize.x + GetSystemMetrics(SM_CXVSCROLL),
		GetSystemMetrics(SM_CXSCREEN)) + GetSystemMetrics(SM_CXFRAME) * 2;
	Y = min(_ScreenSize.y * CharSize.y + GetSystemMetrics(SM_CYHSCROLL) +
		GetSystemMetrics(SM_CYCAPTION), GetSystemMetrics(SM_CYSCREEN)) +
		GetSystemMetrics(SM_CYFRAME) * 2;
		(*MinMaxInfo)[1].x = X;
	(*MinMaxInfo)[1].y = Y;
	(*MinMaxInfo)[3].x = CharSize.x * 16 + GetSystemMetrics(SM_CXVSCROLL) +
		GetSystemMetrics(SM_CXFRAME) * 2;
	(*MinMaxInfo)[3].y = CharSize.y * 4 + GetSystemMetrics(SM_CYHSCROLL) +
		GetSystemMetrics(SM_CYFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION);
	(*MinMaxInfo)[4].x = X;
	(*MinMaxInfo)[4].y = Y;
	DoneDeviceContext();
} /* WindowMinMaxInfo */

/* ----- WM_CHAR message handler ----- */
static void
WindowChar(char Ch)
{
	if (_CheckBreak  && (Ch == 3))
		Terminate();
	if (KeyCount < sizeof(KeyBuffer)) {
		KeyBuffer[KeyCount] = Ch;
		++KeyCount;
	}
} /* WindowChar */

/* ------ WM_KEYDOWN message handler ------ */
static void
WindowKeyDown(BYTE KeyDown)
{
	BOOL CtrlDown;
	int I;

	if (_CheckBreak && (KeyDown == VK_CANCEL))
		Terminate();
	CtrlDown = (BOOL) (GetKeyState(VK_CONTROL < 0));
	for (I = 0; I < SCROLLKEYCOUNT; ++I) {
		if (ScrollKeys[I].Key == KeyDown && ScrollKeys[I].Ctrl == CtrlDown) {
			WindowScroll(ScrollKeys[I].SBar, ScrollKeys[I].Action, 0);
			return;
		}
	}
} /* WindowKeyDown */

/* ----- WM_SETFOCUS message handler ------ */
static void
WindowSetFocus(void)
{
	Focused = TRUE;
	if (Reading)
		_ShowCursor();
} /* WindowSetFocus */

/* ----- WM_KILLFOCUS message handler ----- */
static void
WindowKillFocus(void)
{
	if (Reading)
		_HideCursor();
	Focused = FALSE;
} /* WindowKillFocus */

/* ----- WM_DESTROY message handler ------ */
static void
WindowDestroy(void)
{
	free(ScreenBuffer);
	_Cursor.x = 0;
	_Cursor.y = 0;
	_Origin.x = 0;
	_Origin.y = 0;
	PostQuitMessage(0);
	Created = FALSE;
} /* WindowDestroy */

/* --------- LoVal --------- */
int
LoVal(LONG LVal)
{
	return (((unsigned *)&LVal)[0]);
} /* LoVal */

/* -------- HiVal ------- */
int
HiVal(LONG LVal)
{
	return (((unsigned *)&LVal)[1]);
} /* HiVal */

/* ------ WindowIO window procedure ------ */

long PASCAL
_WinIOProc(HWND Window, UINT Message, WPARAM WParam, LONG LParam)
{
	CrtWindow = Window;
	switch (Message) {
		case WM_CREATE:
			WindowCreate();
			break;
		case WM_PAINT:
			WindowPaint();
			break;
		case WM_VSCROLL:
			WindowScroll(SB_VERT, WParam, LoVal(LParam));
			break;
		case WM_HSCROLL:
			WindowScroll(SB_HORZ, WParam, LoVal(LParam));
			break;
		case WM_SIZE:
			WindowResize(LoVal(LParam), HiVal(LParam));
			break;
		case WM_GETMINMAXINFO:
			WindowMinMaxInfo((TMinMaxInfo *) LParam);
			break;
		case WM_CHAR:
			WindowChar((char)WParam);
			break;
		case WM_KEYDOWN:
			WindowKeyDown((BYTE)WParam);
			break;
		case WM_SETFOCUS:
			WindowSetFocus();
			break;
		case WM_KILLFOCUS:
			WindowKillFocus();
			break;
		case WM_DESTROY:
			WindowDestroy();
			break;
		default:
			return DefWindowProc(Window, Message, WParam, LParam);
	}
	return 0L;
} /* _WinIOProc */

/* ------- _CreateWinIO ------- */
void
_CreateWinIO(void)
{
	if (_hPrevInstance == 0) {
		CrtClass.hInstance = _hInstance;
		CrtClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		CrtClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		CrtClass.hbrBackground = GetStockObject(WHITE_BRUSH);
		RegisterClass(&CrtClass);
	}
	GetModuleFileName(_hInstance, _WindowTitle, sizeof(_WindowTitle));
	OemToAnsi(_WindowTitle, _WindowTitle);
} /* _CreateWinIO */

/* ----- Create window if required ----- */
void
_InitWinIO(HINSTANCE hInst, HINSTANCE hPrev, int cmdShow)
{
	LOGFONT	lf;
	
	_hInstance	= hInst;
	_hPrevInstance	= hPrev;

	_CreateWinIO();
	if (!Created) {
		CrtWindow = CreateWindow(
				CrtClass.lpszClassName,
				_WindowTitle,
				WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL,
				_WindowOrg.x, _WindowOrg.y,
				_WindowSize.x, _WindowSize.y,
				0,
				0,
				_hInstance,
				NULL);
		ShowWindow(CrtWindow, cmdShow);
		UpdateWindow(CrtWindow);

		// For the first time create the font (WCE)
		memset ((char *)&lf, 0, sizeof(lf));
		lf.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;
		lf.lfHeight = 13;
		_hFont = CreateFontIndirect (&lf);
	}
} /* _InitWinIO */

/* ----- Destroy window if required ----- */
void
_DoneWinIO(void)
{
	if (Created)
		DestroyWindow(CrtWindow);
	exit(0);
} /* _DoneWinIO */

/* ------ WinIO unit exit procedure ----- */
void
_ExitWinIO(void)
{
	char *P;
	MSG Message;
	char Title[128];

	if (Created) {
		P = _WindowTitle;
		wsprintf(Title, _InactiveTitle, (char *) P);
		SetWindowText(CrtWindow, Title);
		GetSystemMenu(CrtWindow, TRUE);
		_CheckBreak = FALSE;
		while (GetMessage(&Message, 0, 0, 0)) {
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
	}
} /* _ExitWinIO */
