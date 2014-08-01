// AROS display for DBWRender output files

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/cybergraphics.h>
#include <proto/gadtools.h>
#include <proto/asl.h>

#include <cybergraphx/cybergraphics.h>

#include <png.h>
#include <stdlib.h>
#include <string.h>

#include <png.h>

#define MAX_PATH 1024
#define MAX_LINES 1200

char *version = "display 1.0 (31.07.2014)";

LONG width;
LONG height;

APTR *lines;
APTR bits;
struct Window *window;
APTR *my_VisualInfo;
struct Menu *menuStrip;
TEXT filename[1024];
struct FileRequester *filereq;

struct NewMenu mynewmenu[] =
{
    { NM_TITLE, "Project",        0 , 0, 0, 0,},
    {  NM_ITEM, "Save As PNG...","S", 0, 0, 0,},
    {  NM_ITEM, "About",         "?", 0, 0, 0,},
    {  NM_ITEM, "Quit...",       "Q", 0, 0, 0,},
    {   NM_END, NULL,             0 , 0, 0, 0,}
};



void clean_exit(const char* msg)
{
    LONG i;

    if (window)
    {
        ClearMenuStrip(window);
        CloseWindow(window);
    }
    if (menuStrip) FreeMenus(menuStrip);
    if (my_VisualInfo) FreeVisualInfo(my_VisualInfo);

    for (i=0; i<MAX_LINES; i++)
    {
        FreeVec(lines[i]);
    }
    FreeAslRequest(filereq);
    FreeVec(lines);
    FreeVec(bits);

    if (msg)
    {
        PutStr(msg);
        PutStr("\n");
        exit(1);
    }
    exit(0);
}

void readFile(const char* name)
{
    LONG i;

    filename[0] = '\0';
    if (name == NULL)
    {
        if (filereq == NULL)
        {
            filereq = (struct FileRequester *)AllocAslRequest(ASL_FileRequest, NULL);
            if ( ! filereq) clean_exit("Can't allocate filerequester");
        }

        if (AslRequestTags(filereq, 
            ASLFR_TitleText,        "Save AS PNG...",
            ASLFR_DoPatterns,       TRUE,
            ASLFR_InitialPattern,   "#?.tmp",
            TAG_END))
        {
            strcpy(filename, filereq->rf_Dir);
            if ( ! AddPart(filename, filereq->rf_File, sizeof(filename)))
            {
                clean_exit("AddPart() failed\n");
            }
        }
        else
        {
            clean_exit(0); // requester cancelled
        }
    }
    else
    {
        strlcpy(filename, name, sizeof (filename));
    }

    BPTR f = Open(filename, MODE_OLDFILE);
    if (f == 0)
    {
        clean_exit("Could not open input TMP file");
    }

    FRead(f, &width, sizeof (LONG), 1);
    FRead(f, &height, sizeof (LONG), 1);
    if (width < 1 || width > 3000 || height < 1 || height > 3000)
    {
        Close(f);
        clean_exit("wrong dimensions");
    }

    LONG len = width * 3;

    for (i = 0; i < height; i++)
    {
        LONG row;
        if (FRead(f, &row, sizeof (LONG), 1) != 1)
        {
            break;
        }
        lines[row] = AllocVec(len, MEMF_CLEAR);
        if (lines[row] == NULL)
        {
            clean_exit("Can't allocate memory");
        }
        if (FRead(f, lines[row], len, 1) != 1)
        {
            FreeVec(lines[row]);
            lines[row] = 0;
            break;
        }
    }

    Close(f);
}

void createBitmap(void)
{
    bits = AllocVec(width * 3 * height, MEMF_CLEAR);
    if (bits == NULL)
    {
        clean_exit("Can't allocate memory");
    }

    LONG r,g,b;
    LONG x,y;

    UBYTE *dest = bits;
    for (y = 0; y < height; y++)
    {
        if (lines[y] != 0)
        {
            UBYTE *src = lines[y];
            for (x = 0; x < width; x++)
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
            for (x = 0; x < width; x++)
            {
                *(dest++) = 0;
                *(dest++) = 0;
                *(dest++) = 0;
            }
        }
    }
    WritePixelArray(bits, 0, 0, width * 3, window->RPort, 0, 0, width, height, RECTFMT_BGR24);
}

void save(void)
{
    LONG y;

    if (filereq == NULL)
    {
        filereq = (struct FileRequester *)AllocAslRequest(ASL_FileRequest, NULL);
        if ( ! filereq) clean_exit("Can't allocate filerequester");
    }

    if (AslRequestTags(filereq, 
        ASLFR_TitleText,        "Save AS PNG...",
        ASLFR_DoPatterns,       TRUE,
        ASLFR_DoSaveMode,       TRUE,
        ASLFR_InitialFile,      "",
        ASLFR_InitialPattern,   "",
        TAG_END))
    {
        strcpy(filename, filereq->rf_Dir);
        if ( ! AddPart(filename, filereq->rf_File, sizeof(filename)))
        {
            clean_exit("AddPart() failed\n");
        }
        FILE *f = fopen(filename, "wb");
        if (f == 0)
            clean_exit("Could not open output PNG file");

        png_structp pngp = png_create_write_struct(PNG_LIBPNG_VER_STRING, (png_voidp)0, 0, 0);
        if (pngp == 0)
            clean_exit("Could not create PNG write structure");
        png_infop infop = png_create_info_struct(pngp);
        if (infop == 0)
            clean_exit("Could not create PNG info structure");

        if (setjmp(png_jmpbuf(pngp)))
            clean_exit("Could not write PNG file");

        png_init_io(pngp, f);
        png_set_IHDR(pngp, infop, width, height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
        png_set_bgr(pngp);

        png_bytep* rows = AllocVec(sizeof(png_bytep) * height, MEMF_ANY);
        for (y = 0; y < height; y++)
            rows[y] = bits+(3 * y * width);
        png_set_rows(pngp, infop, rows);

        png_write_png(pngp, infop, PNG_TRANSFORM_IDENTITY, 0);

        FreeVec(rows);
        png_destroy_write_struct(&pngp, &infop);
        fclose(f);
    }
}

void showAbout(void)
{
    struct EasyStruct myES =
    {
        sizeof (struct EasyStruct),
        0,
        "About",
        "Viewer for DBWRender TMP files",
        "OK"
    };
    EasyRequestArgs(NULL, &myES, NULL, NULL);
}

void createWindow(void)
{
    window = OpenWindowTags(NULL,
        //WA_Left,          0,
        WA_Top,           20,
        WA_InnerWidth,    width,
        WA_InnerHeight,   height,
        WA_Title,         "DBWRender Display",
        WA_Activate,      TRUE,
        WA_CloseGadget,   TRUE,
        WA_DragBar,       TRUE,
        WA_GimmeZeroZero, TRUE,
        WA_DepthGadget,   TRUE,
        WA_SmartRefresh,  TRUE,
        WA_IDCMP,         IDCMP_CLOSEWINDOW | IDCMP_MENUPICK,
        TAG_END);

    if (! window) clean_exit("Can't open window");

    if (! (my_VisualInfo = GetVisualInfo(window->WScreen, TAG_END)))
    {
        clean_exit("Can't get visualinfo");
    }
    if (! (menuStrip = CreateMenus(mynewmenu, TAG_END)))
    {
        clean_exit("Can't create menus");
    }
    if (! (LayoutMenus(menuStrip, my_VisualInfo, TAG_END)))
    {
        clean_exit("Can't layout menus");
    }
    if (! (SetMenuStrip(window, menuStrip)))
    {
        clean_exit("Can't set menustrip");
    }
}

void msgLoop(void)
{
    struct IntuiMessage *imsg;
    struct MsgPort *port = window->UserPort;
    ULONG iclass;
    UWORD menuNumber;
    UWORD menuNum;
    UWORD itemNum;
    struct MenuItem *item;
    BOOL terminated = FALSE;

    while (! terminated)
    {
        Wait(1L << port->mp_SigBit);

        while ((imsg = (struct IntuiMessage *)GetMsg(port)) != NULL)
        {
            iclass = imsg->Class;
            switch (iclass)
            {
                case IDCMP_CLOSEWINDOW:
                    terminated = TRUE;
                    break;
                case IDCMP_MENUPICK:
                    menuNumber = imsg->Code;
                    while ((menuNumber != MENUNULL) && (!terminated))
                    {
                        item = ItemAddress(menuStrip, menuNumber);

                        menuNum = MENUNUM(menuNumber);
                        itemNum = ITEMNUM(menuNumber);

                        if (menuNum == 0)
                        {
                            switch (itemNum)
                            {
                                case 0:
                                    save();
                                    break;
                                case 1:
                                    showAbout();
                                    break;
                                case 2:
                                    terminated = TRUE;
                                    break;
                            }
                        }
                        menuNumber = item->NextSelect;
                    }
                    break;
            }
            ReplyMsg((struct Message *)imsg);
        }
    }
}

int main(int argc, char **argv)
{
    lines = AllocVec(MAX_LINES * sizeof (APTR), MEMF_CLEAR);
    if (lines == NULL)
    {
        clean_exit("Can't allocate memory");
    }

    if (argc < 2)
    {
        readFile(NULL);
    }
    else if (argc == 2)
    {
        readFile(argv[1]);
    }
    else
    {
        clean_exit("Usage: display <file>");
    }

    createWindow();
    createBitmap();
    msgLoop();
    clean_exit(0);
    return 0;
}
