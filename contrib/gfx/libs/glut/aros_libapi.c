/*
    Copyright © 1995-2003, The AROS Development Team. All rights reserved.
    $Id$

*/

#include <utility/utility.h> /* this must be before icon_intern.h */

#include <aros/symbolsets.h>

#include <GL/glut.h>

#include <aros/debug.h>

#if !defined(AROSSHAREDGLUT)
#include "arosmesaglut_intern.h"
#define LIBBASETYPE struct AROSMesaGLUTBase
#define LIBBASETYPEPTR struct AROSMesaGLUTBase *
LIBBASETYPE *AMGLInternalBase;
#else
#include LC_LIBDEFS_FILE
#endif

/****************************************************************************************/

#if !defined(AROSSHAREDGLUT)
int AMGLInit(LIBBASETYPEPTR lh)
#else
static int AMGLInit(LIBBASETYPEPTR lh)
#endif
{
    D(bug("[sharedGLUT] In LibInit(base @ %p)\n", lh));

    NewList((struct List *)&lh->_glutPertasklist);

    return TRUE;
}

#if !defined(AROSSHAREDGLUT)
int AMGLOpen(LIBBASETYPEPTR lh)
#else
static int AMGLOpen(LIBBASETYPEPTR lh)
#endif
{
    struct AROSMesaGLUT_TaskNode *newOpener = NULL;

    D(bug("[sharedGLUT] In LibOpen(base @ %p)\n", lh));

    if ((newOpener = AllocVec(sizeof(struct AROSMesaGLUT_TaskNode), MEMF_CLEAR|MEMF_PUBLIC)))
    {
       newOpener->AMGLUTTN_Task = FindTask(NULL);
       NewList((struct List *)&newOpener->AMGLUTTN_WindowList);
    
       newOpener->AMGLUTTN_MsgPort = CreateMsgPort();
    
       AddTail((struct List *)&lh->_glutPertasklist, &newOpener->AMGLUTTN_Node);    
       return TRUE;
    }
    return FALSE;
}

#if defined(AROSSHAREDGLUT)
static int AMGLExpunge(LIBBASETYPEPTR lh)
{
    D(bug("[sharedGLUT] In LibExpunge(base @ %p)\n", lh));

    return TRUE;
}

ADD2INITLIB(AMGLInit, 0);
ADD2OPENLIB(AMGLOpen, 0);
ADD2EXPUNGELIB(AMGLExpunge, 0);
#endif

