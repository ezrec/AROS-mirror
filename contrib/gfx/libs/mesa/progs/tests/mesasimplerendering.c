#include <exec/types.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <intuition/screens.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>

#include <GL/arosmesa.h>
#include <GL/gl.h>

#include <stdio.h>

AROSMesaContext glcont=NULL;

#define RAND_COL 1.0
    
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

double angle = 0.0;

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

    angle++;

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
    
    
BOOL finished = FALSE;
struct Window * win = NULL;

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
    ULONG oldminute;
    ULONG oldticks;
    struct DateStamp ds;
    
    DateStamp(&ds);
    oldminute = ds.ds_Minute;
    oldticks = ds.ds_Tick;
    
        
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
            DateStamp(&ds);
            if ((ds.ds_Minute > oldminute) || (ds.ds_Tick > (oldticks + 50)))
            {
                /* FPS counting is naive! */
                oldminute = ds.ds_Minute;
                oldticks = ds.ds_Tick;
                sprintf(title, "MesaSimpleRendering, FPS: %d", fps);
                SetWindowTitles(win, title, (UBYTE *)~0L);
                fps = 0;
            }
            fps++; 
            render();
            HandleIntuiMessages();
        }
        
        deinitmesa();
        
      
    CloseWindow(win);
    
    return 0;
}

 
