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
#include <proto/exec.h>
#include <dos/dos.h>
#include <string.h>
#include <exec/memory.h>

#include "Scalos.h"
#include "DOSDirIconClass.h"
#include "ScalosIntern.h"

#include "scalos_protos.h"
#include "Debug.h"
#include "SubRoutines.h"

/****** DOSDirIcon.scalos/--background **************************************
*
*   This class is used for DOS directories
*
*****************************************************************************
*/
// /

/****** DOSDirIcon.scalos/SCCM_Icon_Entry ***********************************
*
*  NOTES
*   Not all available values from SCCM_Icon_Entry are implemented in this
*   class. Available values are:
*       SCCV_Icon_Entry_First - initiales the list and selectes the first
*                               entry
*       SCCV_Icon_Entry_Next - selectes the next entry
*       SCCV_Icon_Entry_None - release the lock of the doslist and frees
*                              temporary memory
*
*   After SCCV_Icon_Entry_First the DosList is locked. You should quickly
*   parse through the list and release the lock with SCCV_Icon_Entry_None.
*
*  SEE ALSO
*   Icon.scalos/SCCM_Icon_Entry
*
*****************************************************************************
*/
static ULONG DOSDirIcon_Entry(struct SC_Class *cl, Object *obj, struct SCCP_Icon_Entry *msg, struct DOSDirIconInst *inst)
{
        if (msg->Pos == SCCV_Icon_Entry_First)
        {
                BPTR lock = get(obj,SCCA_DOSIcon_Lock);
                BPTR oldlock;

                if (lock)
                        oldlock = CurrentDir(lock);

                if ((inst->lock = Lock((char *) get(obj,SCCA_Icon_Name),SHARED_LOCK)))
                {
                        if (lock)
                        {
                                CurrentDir(oldlock);
                                UnLock(lock);
                        }

                        if ((inst->fib = AllocDosObject(DOS_FIB,NULL)))
                        {
                                if (Examine(inst->lock,inst->fib) && ExNext(inst->lock,inst->fib))
                                        return(TRUE);

                                FreeDosObject(DOS_FIB,inst->fib);
                                inst->fib = NULL;
                        }
                        UnLock(inst->lock);
                }
                else
                {
                        if (lock)
                        {
                                CurrentDir(oldlock);
                                UnLock(lock);
                        }
                }
        }

        if ((msg->Pos == SCCV_Icon_Entry_Next) && inst->fib)
        {
                if (ExNext(inst->lock,inst->fib))
                        return(TRUE);
        }

        if ((msg->Pos == SCCV_Icon_Entry_None) && inst->fib)
        {
                FreeDosObject(DOS_FIB,inst->fib);
                inst->fib = NULL;
                UnLock(inst->lock);
                return(TRUE);
        }

        return(FALSE);

}
// /

/** Icon_GetObject
*/
static Object *DOSDirIcon_GetObject(struct SC_Class *cl, Object *obj, struct SCCP_Icon_GetObject *msg, struct DOSDirIconInst *inst)
{
        if (inst->fib)
        {
                char *classname;

                if (inst->fib->fib_DirEntryType < 0)
                        classname = SCC_DOSFILEICON_NAME;
                else
                        classname = SCC_DOSDIRICON_NAME;

                return(SC_NewObject(NULL,classname,SCCA_Icon_Name,(char *) &inst->fib->fib_FileName,
                                                        SCCA_DOSIcon_FileType,inst->fib->fib_DirEntryType,
                                                        SCCA_DOSIcon_Protection,inst->fib->fib_Protection,
                                                        SCCA_DOSIcon_Date,inst->fib->fib_Date,
                                                        SCCA_DOSIcon_Size,inst->fib->fib_Size,
                                                        SCCA_DOSIcon_Comment,(char *) &inst->fib->fib_Comment,
                                                        SCCA_DOSIcon_OwnerUID,inst->fib->fib_OwnerUID,
                                                        SCCA_DOSIcon_OwnerGID,inst->fib->fib_OwnerGID,
                                                        TAG_DONE));
        }
        return(NULL);

}
// /

/** Get one attribute of all of the getable attributes
*/
static ULONG DOSDirIcon_Get( struct SC_Class *cl, Object *obj, struct opGet *msg, struct DOSDirIconInst *inst )
{
        if (msg->opg_AttrID == SCCA_Icon_Table)
        {
                Object *table;

                if ((table = SC_NewObject(NULL,SCC_TABLE_NAME,TAG_DONE)))
                {
                        SC_DoMethod(table,SCCM_Table_Add,SCCA_Icon_Name,"Name",SCCV_Table_Type_String,0);
                        SC_DoMethod(table,SCCM_Table_Add,SCCA_DOSIcon_Size,"Size",SCCV_Table_Type_Long,0);
                        SC_DoMethod(table,SCCM_Table_Add,SCCA_DOSIcon_Protection,"Protection",SCCV_Table_Type_Long,0);
                        SC_DoMethod(table,SCCM_Table_Add,SCCA_DOSIcon_Date,"Date",SCCV_Table_Type_Date,0);
                        SC_DoMethod(table,SCCM_Table_Add,SCCA_DOSIcon_Comment,"Comment",SCCV_Table_Type_String,0);
                        SC_DoMethod(table,SCCM_Table_Add,SCCA_DOSIcon_OwnerUID,"UserID",SCCV_Table_Type_Long,0);
                        SC_DoMethod(table,SCCM_Table_Add,SCCA_DOSIcon_OwnerGID,"GroupID",SCCV_Table_Type_Long,0);
                        SC_DoMethod(table,SCCM_Table_Add,SCCA_DOSIcon_FileType,"Type",SCCV_Table_Type_FileType,0);
                        *(msg->opg_Storage) = (LONG) table;
                        return(TRUE);
                }
        }

        return(SC_DoSuperMethodA(cl, obj, (Msg) msg));
}
// /

/*-------------------------- MethodList --------------------------------*/

struct SC_MethodData DOSDirIconMethods[] =
{
        { OM_GET,(ULONG) DOSDirIcon_Get, 0, 0, NULL },
        { SCCM_Icon_GetObject,(ULONG) DOSDirIcon_GetObject, 0, 0, NULL },
        { SCCM_Icon_Entry,(ULONG) DOSDirIcon_Entry, 0, 0, NULL },
        { SCMETHOD_DONE, NULL, 0, 0, NULL }
};

