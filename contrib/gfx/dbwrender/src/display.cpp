// Windows display for DBWRender output files

#include <cstdio>

#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <commctrl.h>
#include <png.h>

#define MAX_LINES 1200

#define IDC_SAVE 100
#define IDC_QUIT 101

int buttonWidth = 0;
int buttonHeight = 0;

int width = 0;
int height = 0;

HDC dc = 0;
HWND wnd = 0;
HACCEL accel = 0;

void** lines = 0;
unsigned char* bits = 0;
char firstFile[MAX_PATH] = "";

void parseCommandLine(char **argv, char *args, int *numargs, int *numchars)
{
	char* cmdstart;
	char *p;
	unsigned char c;
	int inquote;
	int copychar;
	unsigned numslash;

	*numchars = 0;
	*numargs = 1;
	cmdstart = ::GetCommandLine();

	p = cmdstart;
	if (argv)
		*argv++ = args;

	if (*p == '\"')
	{
		while ((*(++p) != '\"') && (*p != '\0'))
		{
			++*numchars;
			if (args)
				*args++ = *p;
		}
		++*numchars;
		if (args)
			*args++ = '\0';

		if (*p == '\"')
				p++;
	}
	else
	{
		do
		{
			++*numchars;
			if (args)
				*args++ = *p;

			c = (unsigned char)*p++;
		}
		while (c != ' ' && c != '\0' && c != '\t');

		if (c == '\0')
			p--;
		else
		{
			if (args)
				*(args-1) = '\0';
		}
	}
	inquote = 0;

	for (;;)
	{
		if (*p)
		{
			while (*p == ' ' || *p == '\t')
				++p;
		}

		if (*p == '\0')
			break;

		if (argv)
			*argv++ = args;
		++*numargs;

		for (;;)
		{
			copychar = 1;
			numslash = 0;
			while (*p == '\\')
			{
				++p;
				++numslash;
			}
			if (*p == '\"')
			{
				if (numslash % 2 == 0)
				{
					if (inquote)
					{
						if (p[1] == '\"')
							p++;
						else
							copychar = 0;
					}
					else
						copychar = 0;

					inquote = !inquote;
				}
				numslash /= 2;
			}

			while (numslash--)
			{
				if (args)
					*args++ = '\\';
				++*numchars;
			}

			if (*p == '\0' || (!inquote && (*p == ' ' || *p == '\t')))
				break;

			if (copychar)
			{
				if (args)
					*args++ = *p;
				++*numchars;
			}
			++p;
		}

		if (args)
			*args++ = '\0';
		++*numchars;
	}

	if (argv)
		*argv++ = 0;
	++*numargs;
}

void fatal(const char* msg)
{
	MessageBox(0,msg,"Error",MB_ICONERROR|MB_OK);
	exit(1);
}

void readFile(const char* name)
{
	if (firstFile[0] == '\0')
		strcpy(firstFile,name);

	FILE* f = fopen(name,"rb");
	if (f == 0)
	{
		char path[MAX_PATH];
		sprintf(path,"%s.tmp",name);

		f = fopen(path,"rb");
		if (f == 0)
			fatal("Could not open input TMP file");
	}

	fread(&width,sizeof (int),1,f);
	fread(&height,sizeof (int),1,f);
	int len = width*3;

	for (int i = 0; i < height; i++)
	{
		int row;
		if (fread(&row,sizeof (int),1,f) != 1)
			break;

		lines[row] = malloc(len);
		if (fread(lines[row],len,1,f) != 1)
		{
			free(lines[row]);
			lines[row] = 0;
			break;
		}
	}

	fclose(f);
}

UINT_PTR CALLBACK fileDialogProc(HWND dlg, UINT msg, WPARAM, LPARAM lparam)
{
	if (msg == WM_NOTIFY)
	{
		LPOFNOTIFY notify = (LPOFNOTIFY)lparam;
		if (notify->hdr.code == CDN_INITDONE)
		{
			HWND wnd = GetParent(dlg);
			if (wnd != 0)
			{
				RECT r;
				GetWindowRect(wnd,&r);

				int w = GetSystemMetrics(SM_CXFULLSCREEN);
				int h = GetSystemMetrics(SM_CYFULLSCREEN);

				SetWindowPos(wnd,0,(w-(r.right-r.left))/2,(h-(r.bottom-r.top))/2,0,0,
					SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER);
			}
		}
	}
	return 0;
}

void prepareOFN(OPENFILENAME* ofn)
{
	OSVERSIONINFO ovi;
	ovi.dwOSVersionInfoSize=sizeof(ovi);
	GetVersionEx(&ovi);

	if (ovi.dwMajorVersion <= 4)
		ofn->lStructSize = OPENFILENAME_SIZE_VERSION_400;
	if (ovi.dwMajorVersion >= 6)
		ofn->Flags &= ~OFN_ENABLEHOOK;
}

void save(void)
{
	char file[MAX_PATH];
	strcpy(file,firstFile);

	char* p = strrchr(file,'\\');
	if (p == 0)
		p = file;
	else
		p++;
	char* x = strrchr(p,'.');
	if (x == 0)
		x = p+strlen(p);
	strcpy(x,".png");

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof ofn);
	ofn.lStructSize = sizeof ofn;
	ofn.hwndOwner = wnd;
	ofn.lpstrFilter = "PNG Image Files (*.png)\0*.png\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = file;
	ofn.nMaxFile = sizeof file;
	ofn.lpstrTitle = "Save as a PNG Image";
	ofn.Flags = OFN_ENABLEHOOK|OFN_ENABLESIZING|OFN_EXPLORER|OFN_HIDEREADONLY;
	ofn.lpfnHook = fileDialogProc;
	prepareOFN(&ofn);

	if (GetSaveFileName(&ofn))
	{
		FILE* f = fopen(file,"wb");
		if (f == 0)
			fatal("Could not open ouput PNG file");

		png_structp pngp = png_create_write_struct
			(PNG_LIBPNG_VER_STRING,(png_voidp)0,0,0);
		if (pngp == 0)
			fatal("Could not create PNG write structure");
		png_infop infop = png_create_info_struct(pngp);
		if (infop == 0)
			fatal("Could not create PNG info structure");

		if (setjmp(png_jmpbuf(pngp)))
			fatal("Could not write PNG file");

		png_init_io(pngp,f);
		png_set_IHDR(pngp,infop,width,height,8,PNG_COLOR_TYPE_RGB,PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT,PNG_FILTER_TYPE_DEFAULT);
		png_set_bgr(pngp);

		png_bytep* rows = (png_bytep*)malloc(sizeof(png_bytep)*height);
		for (int y = 0; y < height; y++)
			rows[y] = bits+(3*y*width);
		png_set_rows(pngp,infop,rows);

		png_write_png(pngp,infop,PNG_TRANSFORM_IDENTITY,0);

		free(rows);
		png_destroy_write_struct(&pngp,&infop);
		fclose(f);
	}
}

LRESULT CALLBACK windowProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_ERASEBKGND:
		{
			RECT r,cr;
			GetClientRect(wnd,&cr);
			CopyRect(&r,&cr);

			r.bottom = buttonHeight;
			SetBkColor((HDC)wparam,GetSysColor(COLOR_BTNFACE));
			ExtTextOut((HDC)wparam,0,0,ETO_OPAQUE,&r,0,0,0);

			r.top = buttonHeight;
			r.bottom = cr.bottom;
			SetBkColor((HDC)wparam,RGB(0,0,0));
			ExtTextOut((HDC)wparam,0,0,ETO_OPAQUE,&r,0,0,0);
		}
		return 1;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			BeginPaint(wnd,&ps);
			BitBlt(ps.hdc,0,buttonHeight,width,height,dc,0,0,SRCCOPY);
			EndPaint(wnd,&ps);
		}
		return 0;
	case WM_COMMAND:
		{
			switch (LOWORD(wparam)) 
			{ 
			case IDC_SAVE:
				save();
				return 0;
			case IDC_QUIT:
				PostQuitMessage(0);
				return 0;
			}
		}
		break;
	}
	return DefWindowProc(wnd,msg,wparam,lparam);
}

void createBitmap(void)
{
	HDC wdc = GetDC(wnd);
	dc = CreateCompatibleDC(wdc);
	ReleaseDC(wnd,dc);

	BITMAPINFO info;
	ZeroMemory(&info,sizeof (BITMAPINFO));
	info.bmiHeader.biSize = sizeof (BITMAPINFOHEADER);
	info.bmiHeader.biPlanes = 1;
	info.bmiHeader.biBitCount = 24;
	info.bmiHeader.biCompression = BI_RGB;
	info.bmiHeader.biWidth = width;
	info.bmiHeader.biHeight = height * -1;

	HBITMAP bitmap = CreateDIBSection(dc,&info,DIB_RGB_COLORS,(void**)&bits,0,0);
	if (bitmap == 0)
		fatal("Could not create DIB section");

	int r,g,b;
	unsigned char* dest = bits;
	for (int y = 0; y < height; y++)
	{
		if (lines[y] != 0)
		{
			unsigned char* src = (unsigned char*)(lines[y]);
			for (int x = 0; x < width; x++)
			{
				r = *(src+(0*width));
				g = *(src+(1*width));
				b = *(src+(2*width));
				src++;

				*(dest++) = b<<1;
				*(dest++) = g<<1;
				*(dest++) = r<<1;
			}
		}
		else
		{
			for (int x = 0; x < width; x++)
			{
				*(dest++) = 0;
				*(dest++) = 0;
				*(dest++) = 0;
			}
		}
	}

	SelectObject(dc,bitmap);
}

void createButton(HINSTANCE instance, HFONT font, const char* text, DWORD id, int x)
{
	HWND btn = CreateWindowEx(0,"Button",text,
		WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_TEXT,x*buttonWidth,0,buttonWidth,buttonHeight,
		wnd,(HMENU)id,instance,0);
	if (btn == 0)
		fatal("Could not create button");
	SendMessage(btn,WM_SETFONT,(WPARAM)font,0);
}

HWND createWindow(HINSTANCE instance)
{
	WNDCLASSEX wndClass;
	ZeroMemory(&wndClass,sizeof(WNDCLASSEX));
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = 0;
	wndClass.lpfnWndProc = windowProc;
	wndClass.hInstance = instance;
	wndClass.hIcon = LoadIcon(GetModuleHandle(NULL),MAKEINTRESOURCE(100));
	wndClass.hCursor = LoadCursor(0,IDC_ARROW);
	wndClass.lpszClassName = "DBWRender";
	if (RegisterClassEx(&wndClass) == 0)
		fatal("Could not create window class");

	wnd = CreateWindowEx(WS_EX_CLIENTEDGE,"DBWRender","DBW Render Display",
		WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN,0,0,width,height,
		0,0,instance,0);
	if (wnd == 0)
		fatal("Could not create window");

	NONCLIENTMETRICS ncm;
	ZeroMemory(&ncm,sizeof ncm);
	ncm.cbSize = sizeof ncm;
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS,ncm.cbSize,&ncm,0);

	HFONT font = CreateFont(ncm.lfMessageFont.lfHeight,0,0,0,FW_NORMAL,0,0,0,
		ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
		DEFAULT_PITCH,ncm.lfMessageFont.lfFaceName);
	if (font == 0)
		fatal("Could not create font");

	HDC dc = GetWindowDC(wnd);
	SIZE size;
	GetTextExtentPoint32(dc,"  Save  ",8,&size);
	buttonWidth = size.cx;
	buttonHeight = (int)(1.25*size.cy);
	ReleaseDC(wnd,dc);

	createButton(instance,font,"&Save",IDC_SAVE,0);
	createButton(instance,font,"E&xit",IDC_QUIT,1);

	ACCEL accelt[2] = {
		{ FALT,'s',IDC_SAVE },
		{ FALT,'x',IDC_QUIT }};
	accel = CreateAcceleratorTable(accelt,2);
	if (accel == 0)
		fatal("Could not create accelerator table");

	RECT wr, cr;
	GetWindowRect(wnd,&wr);
	GetClientRect(wnd,&cr);
	int w = width+(wr.right-wr.left)-(cr.right-cr.left);
	int h = height+buttonHeight+(wr.bottom-wr.top)-(cr.bottom-cr.top);

	int mw = GetSystemMetrics(SM_CXFULLSCREEN);
	int mh = GetSystemMetrics(SM_CYFULLSCREEN);
	int x = w < mw ? (mw-w)/2 : 0;
	int y = h < mh ? (mh-h)/2 : 0;
	MoveWindow(wnd,x,y,w,h,0);

	ShowWindow(wnd,SW_SHOW);
	return wnd;
}

void msgLoop(void)
{
	MSG msg;
	while (true)
	{
		GetMessage(&msg,0,0,0);
		if (msg.message == WM_QUIT)
			exit(0);

		if (TranslateAccelerator(wnd,accel,&msg) == 0)
			TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void promptReadFiles(void)
{
	char files[4096];
	files[0] = '\0';

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof ofn);
	ofn.lStructSize = sizeof ofn;
	ofn.lpstrFilter = "DBW Render Output Files (*.tmp)\0*.tmp\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = files;
	ofn.nMaxFile = sizeof files;
	ofn.lpstrTitle = "Open DBW Render Output Files";
	ofn.Flags = OFN_ALLOWMULTISELECT|OFN_ENABLEHOOK|OFN_ENABLESIZING|
		OFN_EXPLORER|OFN_FILEMUSTEXIST|OFN_HIDEREADONLY;
	ofn.lpfnHook = fileDialogProc;
	prepareOFN(&ofn);

	if (GetOpenFileName(&ofn))
	{
		char* p = files+strlen(files)+1;
		if (*p == '\0')
			readFile(files);
		else
		{
			char path[MAX_PATH];
			while (*p != '\0')
			{
				sprintf(path,"%s\\%s",files,p);
				readFile(path);
				p += strlen(p)+1;
			}
		}
	}
	else
		exit(0);
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR cmdLine, int)
{
	int argc = 0;
	int nc = 0;
	parseCommandLine(0,0,&argc,&nc);
	char** argv = (char**)malloc((argc*sizeof(char*))+(nc*sizeof(char)));
	parseCommandLine(argv,((char*)argv)+(argc*sizeof(char*)),&argc,&nc);
	argc--;

	InitCommonControls();
	lines = (void**)calloc(MAX_LINES,sizeof (void*));

	if (argc > 1)
	{
		for (int i = 1; i < argc; i++)
			readFile(argv[i]);
	}
	else
		promptReadFiles();

	createWindow(instance);
	createBitmap();
	msgLoop();
	return 0;
}
