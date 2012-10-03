/*
    Example for event handling of intuition windows

*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

#include <stdlib.h>
#include <stdio.h>

static struct Window *window;

static void clean_exit(CONST_STRPTR s);
static void handle_events(void);


int main(void)
{
    window = OpenWindowTags(NULL,
        WA_Left,          400,
        WA_Top,           70,
        WA_InnerWidth,    300,
        WA_InnerHeight,   300,
        WA_Title,         "Intuition events",
        WA_Activate,      TRUE,
        WA_RMBTrap,       TRUE, // handle right mouse button as normal mouse button
        WA_CloseGadget,   TRUE,
        WA_DragBar,       TRUE,
        WA_GimmeZeroZero, TRUE,
        WA_DepthGadget,   TRUE,
        WA_NoCareRefresh, TRUE, // we don't want to listen to refresh messages
        WA_IDCMP,         IDCMP_CLOSEWINDOW | IDCMP_MOUSEBUTTONS | 
                          IDCMP_VANILLAKEY | IDCMP_RAWKEY,
        TAG_END);

    if (! window) clean_exit("Can't open window\n");
    
    puts("\nPress 'r' to disable VANILLAKEY");
    
    handle_events();
    
    clean_exit(NULL);

    return 0;
}


static void handle_events(void)
{
    struct IntuiMessage *imsg;
    struct MsgPort *port = window->UserPort;

    ULONG signals;
    
    ULONG iclass;
    UWORD code;
    UWORD qualifier;
    WORD mousex, mousey;
    
    BOOL terminated = FALSE;
    
    while (! terminated)
    {
        signals = Wait(1L << port->mp_SigBit);

        while ((imsg = (struct IntuiMessage *)GetMsg(port)) != NULL)
        {
            iclass = imsg->Class;
            code = imsg->Code;
            qualifier = imsg->Qualifier;
            mousex = imsg->MouseX;
            mousey = imsg->MouseY;
            
            /*
                After we have stored the necessary values from the message
                in variables we can immediately reply the message. Note
                that this is only possible because we have no VERIFY events.
            */
            ReplyMsg((struct Message *)imsg);       

            switch (iclass)
            {
                case IDCMP_CLOSEWINDOW:
                    puts("IDCMP_CLOSEWINDOW");
                    terminated = TRUE;
                    break;
                case IDCMP_MOUSEBUTTONS:
                    switch (code)
                    {
                        case SELECTDOWN:
                            printf("left mouse button down");
                            break;
                        case SELECTUP:
                            printf("left mouse button up");
                            break;
                        case MENUDOWN:
                            printf("right mouse button down");
                            break;
                        case MENUUP:
                            printf("right mouse button up");
                            break;
                        case MIDDLEDOWN:
                            printf("middle mouse button down");
                            break;
                        case MIDDLEUP:
                            printf("middle mouse button up");
                            break;
                    }
                    printf(" at %d %d\n", mousex, mousey);
                    break;
                case IDCMP_VANILLAKEY:
                    printf("Vanillakey %d %c\n", code, code);
                    if (code == 'r')
                    {
                        ModifyIDCMP(window, window->IDCMPFlags &= ~IDCMP_VANILLAKEY);
                        puts("RAWKEY only");
                    }
                    break;
                case IDCMP_RAWKEY:
                    printf("Rawkey %d %c\n", code, code);
                    break;
            }
        }
    }
}


static void clean_exit(CONST_STRPTR s)
{
    if (s) puts(s);

    // Give back allocated resourses
    if (window) CloseWindow(window);

    exit(0);
}
