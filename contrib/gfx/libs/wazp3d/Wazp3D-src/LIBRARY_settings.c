#include <graphics/text.h>
#include <intuition/intuition.h>
#include <libraries/gadtools.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/exec.h>

struct MyButton{
    BOOL ON;
    UBYTE name[30];
    };
#define NLOOP(nbre) for(n=0;n<nbre;n++)
extern IPTR Wazp3DPrefsNameTag;
static struct TextAttr Topaz80 = { "topaz.font", 8, 0, 0, };

static void
process_window_events(struct Window *mywin,struct MyButton *ButtonList)
{
struct IntuiMessage *imsg;
struct Gadget *gad;
BOOL  terminated = FALSE;

while (!terminated)
    {
    Wait (1 << mywin->UserPort->mp_SigBit);

    /* Use GT_GetIMsg() and GT_ReplyIMsg() for handling */
    /* IntuiMessages with GadTools gadgets.             */
    while ((!terminated) && (imsg = GT_GetIMsg(mywin->UserPort)))
        {
        /* GT_ReplyIMsg() at end of loop */

        switch (imsg->Class)
            {
            case IDCMP_GADGETUP:       /* Buttons only report GADGETUP */
                gad = (struct Gadget *)imsg->IAddress;
            ButtonList[gad->GadgetID].ON=!ButtonList[gad->GadgetID].ON;
                break;
            case IDCMP_CLOSEWINDOW:
                terminated = TRUE;
                break;
            case IDCMP_REFRESHWINDOW:
                /* This handling is REQUIRED with GadTools. */
                GT_BeginRefresh(mywin);
                GT_EndRefresh(mywin, TRUE);
                break;
            }
        /* Use the toolkit message-replying function here... */
        GT_ReplyIMsg(imsg);
        }
    }
}

void
LibSettings(struct MyButton *ButtonList,WORD ButtonCount)
{
struct Screen    *mysc;
struct Window    *mywin;
struct Gadget    *glist, *gad;
struct NewGadget ng;
void             *vi;
WORD n,x,y;
glist = NULL;

if ( (mysc = LockPubScreen(NULL)) != NULL )
    {
    if ( (vi = GetVisualInfo(mysc, TAG_END)) != NULL )
        {
        /* GadTools gadgets require this step to be taken */
        gad = CreateContext(&glist);

NLOOP(ButtonCount)
{
	x=220 + mysc->WBorLeft;
	y=12*n + 6 + mysc->WBorTop + (mysc->Font->ta_YSize + 1);
	if((ButtonCount/2) < n)
		{x=x+220; y=y-ButtonCount/2*12;}

        /* create a button gadget centered below the window title */
        ng.ng_TextAttr   = &Topaz80;
        ng.ng_VisualInfo = vi;
        ng.ng_LeftEdge   = x;
        ng.ng_TopEdge    = y;
        ng.ng_Width      = 24;
        ng.ng_Height     = 12;
        ng.ng_GadgetText = ButtonList[n].name;
        ng.ng_GadgetID   = n;
        ng.ng_Flags      = 0;
	gad = CreateGadget(CHECKBOX_KIND, gad, &ng, GT_Underscore, '_',  GTCB_Scaled, TRUE,GTCB_Checked,ButtonList[n].ON,TAG_DONE);
}

        if (gad != NULL)
            {
            if ( (mywin = OpenWindowTags(NULL,
                    WA_Title,  Wazp3DPrefsNameTag ,
                    WA_Gadgets, (ULONG)  glist,      WA_AutoAdjust,    TRUE,
                    WA_InnerWidth,       260+220,      WA_InnerHeight,    20+12*ButtonCount/2,
                    WA_DragBar,    TRUE,      WA_DepthGadget,   TRUE,
                    WA_Activate,   TRUE,      WA_CloseGadget,   TRUE,
                    WA_IDCMP, IDCMP_CLOSEWINDOW |IDCMP_REFRESHWINDOW | BUTTONIDCMP,
                    WA_PubScreen,  (ULONG) mysc,
                    TAG_END)) != NULL )
                {
                GT_RefreshWindow(mywin, NULL);

                process_window_events(mywin,ButtonList);

                CloseWindow(mywin);
                }
            }
        /* FreeGadgets() must be called after the context has been
        ** created.  It does nothing if glist is NULL
        */
        FreeGadgets(glist);
        FreeVisualInfo(vi);
        }

    UnlockPubScreen(NULL, mysc);
    }
}
