struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct Window *Window;
struct IntuiMessage *msg;

ULONG class;
USHORT code;

void open_lib()
{
  IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library",0L);
  GfxBase = (struct GfxBase *) OpenLibrary("graphics.library",0L);
}

void open_window()
{
    Window = OpenWindowTags (NULL
	, WA_Title,	    "Quad"
	, WA_Left,	    0
	, WA_Top,	    0
	, WA_Width,	    536
	, WA_Height,	    121
	, WA_IDCMP,	    IDCMP_RAWKEY
			    | IDCMP_REFRESHWINDOW
			    | IDCMP_MOUSEBUTTONS
			    | IDCMP_GADGETUP
			    | IDCMP_CLOSEWINDOW
	, WA_SimpleRefresh, TRUE
	, WA_Gadgets,	    &Start
	, TAG_END
    );
  ActivateWindow(Window);
}

void close_window()
{
  CloseWindow(Window);
}

void close_lib()
{
  CloseLibrary((struct Library *)IntuitionBase);
  CloseLibrary((struct Library *)GfxBase);
}
