#ifndef AROSMESAGLUT_INTERN_H
#define AROSMESAGLUT_INTERN_H

/*
    Copyright © 1995-2006, The AROS Development Team. All rights reserved.
    $Id: arosmesaglut_intern.h 24606 2006-08-05 15:37:10Z verhaegs $
*/

#include <exec/memory.h>
#include <exec/libraries.h>
#include <aros/asmcall.h>
#include <dos/dos.h>
#include <libraries/iffparse.h>
#include <cybergraphx/cybergraphics.h>
#include <workbench/workbench.h>
#include <workbench/icon.h>
#include <graphics/view.h>

#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/icon.h>
#include <proto/iffparse.h>
#include <proto/utility.h>
#include <proto/cybergraphics.h>
#include <proto/dos.h>

#include <stddef.h>

#include <string.h>
#include <aros/libcall.h>
#include <aros/asmcall.h>

#include "internal.h"

/****************************************************************************************/

/* Constants */

struct AROSMesaGLUTBase
{
#if defined(AROSSHAREDGLUT)
    struct Library      _glutLib;
#endif
    struct List         _glutPertasklist;
    
    /* Setup default global settings ---------------------------------------*/
    
    IPTR                _glutStrokeRoman;
    IPTR                _glutStrokeMonoRoman;
    IPTR                _glutBitmap8By13;
    IPTR                _glutBitmap9By15;
    IPTR                _glutBitmapTimesRoman10;
    IPTR                _glutBitmapTimesRoman24;
    IPTR                _glutBitmapHelvetica10;
    IPTR                _glutBitmapHelvetica12;
    IPTR                _glutBitmapHelvetica18;    
};

#endif /* AROSMESAGLUT_INTERN_H */
