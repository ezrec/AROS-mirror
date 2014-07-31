// AROS display for DBWRender output files

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/cybergraphics.h>

#include <cybergraphx/cybergraphics.h>

#include <stdlib.h>

#include <png.h>

#define MAX_PATH 1024
#define MAX_LINES 1200

LONG width = 0;
LONG height = 0;

APTR* lines = 0;
APTR bits = 0;
struct Window *window;

void clean_exit(const char* msg)
{
    LONG i;
    if (window) CloseWindow(window);
    for (i=0; i<MAX_LINES; i++)
    {
        FreeVec(lines[i]);
    }
    FreeVec(lines);
    FreeVec(bits);

    if (msg)
    {
        PutStr(msg);
        Flush(Output());
        exit(1);
    }
    exit(0);
}

void readFile(const char* name)
{
    LONG i;

    BPTR f = Open(name, MODE_OLDFILE);
    if (f == 0)
    {
        clean_exit("Could not open input TMP file");
    }

    FRead(f, &width, sizeof (LONG), 1);
    FRead(f, &height, sizeof (LONG), 1);
    LONG len = width*3;

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

    fclose(f);
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
                r = *(src+(2*width));
                g = *(src+(1*width));
                b = *(src+(0*width));
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
    WritePixelArray(bits, 0, 0, width * 3, window->RPort, 0, 0, width, height, RECTFMT_RGB);
}

void createWindow(void)
{
    window = OpenWindowTags(NULL,
        //WA_Left,          0,
        //WA_Top,           0,
        WA_InnerWidth,    width,
        WA_InnerHeight,   height,
        WA_Title,         "DBWRender Display",
        WA_Activate,      TRUE,
        WA_CloseGadget,   TRUE,
        WA_DragBar,       TRUE,
        WA_GimmeZeroZero, TRUE,
        WA_DepthGadget,   TRUE,
        WA_SmartRefresh,  TRUE,
        WA_IDCMP,         IDCMP_CLOSEWINDOW,
        TAG_END);

    if (! window) clean_exit("Can't open window\n");

}

void msgLoop(void)
{
    struct IntuiMessage *imsg;
    struct MsgPort *port = window->UserPort;
    ULONG iclass;
    BOOL terminated = FALSE;

    while (! terminated)
    {
        Wait(1L << port->mp_SigBit);

        while ((imsg = (struct IntuiMessage *)GetMsg(port)) != NULL)
        {
            iclass = imsg->Class;
            ReplyMsg((struct Message *)imsg);
            switch (iclass)
            {
                case IDCMP_CLOSEWINDOW:
                    terminated = TRUE;
                    break;
            }
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

    if (argc == 2)
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
