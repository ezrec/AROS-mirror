// :ts=4 (Tabsize)

/*
** Amiga Workbench® Replacement
**
** (C) Copyright 1999,2000 Aliendesign
** Stefan Sommerfeld, Jörg Rebenstorf
**
** Redistribution and use in source and binary forms are permitted provided that
** the above copyright notice and this paragraph are duplicated in all such
** forms and that any documentation, advertising materials, and other
** materials related to such distribution and use acknowledge that the
** software was developed by Aliendesign.
** THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
** WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
** MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
*/
#include <proto/utility.h>
#include <proto/alib.h>
#include <proto/dos.h>
#include <dos/dos.h>

#include "Scalos.h"
#include "DOSFileIconClass.h"
#include "ScalosIntern.h"

#include "scalos_protos.h"
#include "Debug.h"
#include "SubRoutines.h"

/****** DOSFileIcon.scalos/--background *************************************
*
*   An object of the DOSFileIcon class represents a AmigaDOS file.
*
*****************************************************************************
*/
// /

/** Open the file
*/
static ULONG DOSFileIcon_Open(struct SC_Class *cl, Object *obj, struct SCCP_Icon_Open *msg, struct DOSFileIconInst *inst)
{
        BPTR currentdir = (BPTR) get(obj,SCCA_DOSIcon_Lock);
        BPTR oldcurrentdir;
        BPTR fh;

        if (currentdir)
                oldcurrentdir = CurrentDir(currentdir);

        fh = Open((char *) get(obj,SCCA_Icon_Name),msg->type);

        if (currentdir)
                CurrentDir(oldcurrentdir);

        return(fh);
}
// /

/** Close the file
*/
static void DOSFileIcon_Close(struct SC_Class *cl, Object *obj, struct SCCP_Icon_Close *msg, struct DOSFileIconInst *inst)
{
        Close(msg->handle);
}
// /

/** Read from file
*/
static ULONG DOSFileIcon_Read(struct SC_Class *cl, Object *obj, struct SCCP_Icon_Read *msg, struct DOSFileIconInst *inst)
{
        return(Read(msg->handle,msg->memory,msg->size));
}
// /


/*-------------------------- MethodList --------------------------------*/

struct SC_MethodData DOSFileIconMethods[] =
{
        { SCCM_Icon_Open,(ULONG)                DOSFileIcon_Open, 0, 0, NULL },
        { SCCM_Icon_Read,(ULONG)                DOSFileIcon_Read, 0, 0, NULL },
        { SCCM_Icon_Close,(ULONG)               DOSFileIcon_Close, 0, 0, NULL },
        { SCMETHOD_DONE, NULL, 0, 0, NULL }
};

