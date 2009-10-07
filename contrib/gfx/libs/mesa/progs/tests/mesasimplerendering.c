#include <exec/types.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <intuition/screens.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>

#include <proto/timer.h>
#include <devices/timer.h>

#include <GL/arosmesa.h>
#include <GL/gl.h>

#include <stdio.h>

AROSMesaContext     glcont=NULL;
double              angle = 0.0;
double              angle_inc = 0.0;
BOOL                finished = FALSE;
struct Window *     win = NULL;
struct Device *     TimerBase = NULL;
struct timerequest  timereq;
struct MsgPort      timeport;
    
#define RAND_COL 1.0f
#define DEGREES_PER_SECOND 180.0;

void render_face()
{
    glBegin(GL_QUADS);
        glColor4f(RAND_COL , 0.0, RAND_COL, 0.3);
        glVertex3f(-0.25, -0.25, 0.0);
        glColor4f(0, RAND_COL, RAND_COL, 0.3);
        glVertex3f(-0.25, 0.25, 0.0);
        glColor4f(0 , 0, 0, 0.3);
        glVertex3f(0.25, 0.25, 0.0);
        glColor4f(RAND_COL , RAND_COL, 0, 0.3);
        glVertex3f(0.25, -0.25, 0.0);
    glEnd();

}

void render_cube()
{
    glPushMatrix();
    glRotatef(0.0, 0.0, 1.0, 0.0);
    glTranslatef(0.0, 0.0, 0.25);
    render_face();
    glPopMatrix();


    glPushMatrix();
    glRotatef(90.0, 0.0, 1.0, 0.0);
    glTranslatef(0.0, 0.0, 0.25);
    render_face();
    glPopMatrix();

    glPushMatrix();
    glRotatef(180.0, 0.0, 1.0, 0.0);
    glTranslatef(0.0, 0.0, 0.25);
    render_face();
    glPopMatrix();

    glPushMatrix();
    glRotatef(270.0, 0.0, 1.0, 0.0);
    glTranslatef(0.0, 0.0, 0.25);
    render_face();
    glPopMatrix();

    glPushMatrix();
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glTranslatef(0.0, 0.0, 0.25);
    render_face();
    glPopMatrix();

    glPushMatrix();
    glRotatef(-90.0, 1.0, 0.0, 0.0);
    glTranslatef(0.0, 0.0, 0.25);
    render_face();
    glPopMatrix();
}

void render()
{
    glLoadIdentity();
    glClearColor(0.3, 0.3, 0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
    glEnable(GL_BLEND);

    angle += angle_inc;

    glPushMatrix();
    glRotatef(angle, 0.0, 1.0, 0.0);
    glTranslatef(0.0, 0.0, 0.25);
    glRotatef(angle, 1.0, 0.0, 1.0);
    render_cube();
    glPopMatrix();

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    AROSMesaSwapBuffers(glcont);
}    

void initmesa()
{
    struct TagItem attributes [ 14 ]; /* 14 should be more than enough :) */
    int i = 0;
    
    attributes[i].ti_Tag = AMA_Window;      attributes[i++].ti_Data = (IPTR)win;
    attributes[i].ti_Tag = AMA_Left;        attributes[i++].ti_Data = win->BorderLeft;
    attributes[i].ti_Tag = AMA_Top;         attributes[i++].ti_Data = win->BorderTop;
    attributes[i].ti_Tag = AMA_Bottom;      attributes[i++].ti_Data = win->BorderBottom;
    attributes[i].ti_Tag = AMA_Right;       attributes[i++].ti_Data = win->BorderRight;

    // double buffer ?
    attributes[i].ti_Tag = AMA_DoubleBuf;   attributes[i++].ti_Data = GL_TRUE;

    // RGB(A) Mode ?
    attributes[i].ti_Tag = AMA_RGBMode;     attributes[i++].ti_Data = GL_TRUE;
    
    // done...
    attributes[i].ti_Tag    = TAG_DONE;

    glcont = AROSMesaCreateContext(attributes);
    if (glcont)
        AROSMesaMakeCurrent(glcont);
    else
        finished = TRUE; /* Failure. Stop */
}

void deinitmesa()
{
    if (glcont) AROSMesaDestroyContext(glcont);
}

static int init_timerbase()
{
    timeport.mp_Node.ln_Type   = NT_MSGPORT;
    timeport.mp_Node.ln_Pri    = 0;
    timeport.mp_Node.ln_Name   = NULL;
    timeport.mp_Flags          = PA_IGNORE;
    timeport.mp_SigTask        = FindTask(NULL);
    timeport.mp_SigBit         = 0;
    NEWLIST(&timeport.mp_MsgList);

    timereq.tr_node.io_Message.mn_Node.ln_Type    = NT_MESSAGE;
    timereq.tr_node.io_Message.mn_Node.ln_Pri     = 0;
    timereq.tr_node.io_Message.mn_Node.ln_Name    = NULL;
    timereq.tr_node.io_Message.mn_ReplyPort       = &timeport;
    timereq.tr_node.io_Message.mn_Length          = sizeof (timereq);

    if(OpenDevice("timer.device",UNIT_VBLANK,(struct IORequest *)&timereq,0) == 0)
    {
        TimerBase = (struct Device *)timereq.tr_node.io_Device;
        return 1;
    }
    else
    {
        return 0;
    }
}


static void deinit_timerbase()
{
    if (TimerBase != NULL)
        CloseDevice((struct IORequest *)&timereq);
}


void HandleIntuiMessages(void)
{
    struct IntuiMessage *msg;

    while((msg = (struct IntuiMessage *)GetMsg(win->UserPort)))
    {
        switch(msg->Class)
        {
            case IDCMP_CLOSEWINDOW:
            finished = TRUE;
            break;
        }
        ReplyMsg((struct Message *)msg);
    }
}

/*
** Open a simple window using OpenWindowTagList()
*/
int main(int argc, char **argv)
{
    ULONG fps = 0;
    TEXT title[100];
    
    struct timeval tv;
    UQUAD lastmicrosecs = 0L;
    UQUAD currmicrosecs = 0L;
    UQUAD fpsmicrosecs = 0L;
    
    init_timerbase();
    
    GetSysTime(&tv);
    lastmicrosecs = tv.tv_secs * 1000000 + tv.tv_micro;
    fpsmicrosecs = lastmicrosecs;
        
    win = OpenWindowTags(0,
                        WA_Title, (IPTR)"MesaSimpleRendering",
                        WA_CloseGadget, TRUE,
                        WA_DragBar, TRUE,
                        WA_DepthGadget, TRUE,
                        WA_Left, 10,
                        WA_Top, 10,
                        WA_InnerWidth, 300,
                        WA_InnerHeight, 300,
                        WA_Activate, TRUE,
                        WA_RMBTrap, TRUE,
                        WA_SimpleRefresh, TRUE,
                        WA_NoCareRefresh, TRUE,
                        WA_IDCMP, IDCMP_CLOSEWINDOW,
                        TAG_DONE);
             
    initmesa();
        
    while(!finished)
    {
        GetSysTime(&tv);
        currmicrosecs = tv.tv_secs * 1000000 + tv.tv_micro;
        
        if (currmicrosecs - fpsmicrosecs > 1000000)
        {
            /* FPS counting is naive! */
            fpsmicrosecs += 1000000;
            sprintf(title, "MesaSimpleRendering, FPS: %d", fps);
            SetWindowTitles(win, title, (UBYTE *)~0L);
            fps = 0;
        }
        
        angle_inc = ((double)(currmicrosecs - lastmicrosecs) / 1000000.0) * DEGREES_PER_SECOND;
        lastmicrosecs = currmicrosecs;
        
        fps++; 
        render();
        HandleIntuiMessages();
    }
    
    deinitmesa();
    
    deinit_timerbase();
      
    CloseWindow(win);
    
    return 0;
}

 
