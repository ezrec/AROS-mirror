/*
    Copyright © 1997-2006, The AROS Development Team. All rights reserved.
    $Id$

    Desc: DOpus library initialization code.
    Lang: English.
*/

#include <stddef.h>
#include <exec/libraries.h>
#include <exec/lists.h>
#include <aros/symbolsets.h>
#include <proto/exec.h>

/* #include "initstruct.h" */
//#include "dopuslib.h"
#include "initstruct.h"
#include LC_LIBDEFS_FILE

#define DEBUG 0
#include <aros/debug.h>


extern UBYTE pdb_cycletop[];
extern UBYTE pdb_cyclebot[];
extern UBYTE pdb_check[];

#warning Remove DOpusBase as global variable
/* AROS: DOpusBase global var shadows the parameter with the same name */

void set_global_dopusbase(struct DOpusBase *base)
{
	DOpusBase = base;
}

static int GM_UNIQUENAME(Init)(LIBBASETYPEPTR LIBBASE)
{
    /* This function is single-threaded by exec by calling Forbid. */

    /* We have to use a function to set the global var DOpusBase
       since it shadows the LIBBASE parameter (LIBBASE is a #define
       for DOpusBase
    */
    
    set_global_dopusbase(LIBBASE);

    /* ----------------- */
    
    LIBBASE->DOSBase = DOSBase;
    LIBBASE->IntuitionBase = IntuitionBase;
    LIBBASE->GfxBase = GfxBase;
    LIBBASE->LayersBase = (struct LayersBase *)LayersBase;
    
//    init_dopus_library();
    
    LIBBASE->pdb_cycletop = (PLANEPTR)&pdb_cycletop;
    LIBBASE->pdb_cyclebot = (PLANEPTR)&pdb_cyclebot;
kprintf("DOPUS_INIT: ct: %p, cb: %p\n"
    , LIBBASE->pdb_cycletop
    , LIBBASE->pdb_cyclebot
);
    LIBBASE->pdb_check = (PLANEPTR)&pdb_check;
    
    LIBBASE->pdb_Flags = 0;

    /* You would return NULL here if the init failed. */
    return TRUE;
}

ADD2INITLIB(GM_UNIQUENAME(Init), 0)
