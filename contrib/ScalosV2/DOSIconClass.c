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
#include <clib/utility_protos.h>
#include <clib/alib_protos.h>
#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <clib/icon_protos.h>
#include <clib/locale_protos.h>
#include <dos/dos.h>
#include <exec/memory.h>
#include <string.h>
#include <stdio.h>

#include "Scalos.h"
#include "DOSIconClass.h"
#include "ScalosIntern.h"
#include "CompilerSupport.h"

#include "scalos_protos.h"
#include "Debug.h"
#include "SubRoutines.h"


// size of formated date string
#define DATESIZE 40

/****** DOSIcon.scalos/--background *****************************************
*
*   DOSIcon class is the super class of all AmigaDOS objects.
*   Getting an attribute which wasn't initialized before, the DOSIcon will
*   trying to get the information from disk. Which means we would only need
*   to give a name if you init a new DOSIcon all other information will get
*   the object itself, but if you have more specific info you should give it
*   as init args. This would speedup the handling of these object.
*
*****************************************************************************
*/
// /
/****** DOSIcon.scalos/SCCA_DOSIcon_Lock ************************************
*
*  NAME
*   SCCA_DOSIcon_Lock -- (V40) I.G (BPTR)
*
*  FUNCTION
*   This attribute is the dir where the file is relative to. NULL means it's
*   relative to the tasks current dir. The lock will be freed after disposing
*   the object, so use DupLock(). The result of getting this attribute must
*   be freed using UnLock();
*   Default: NULL
*
*****************************************************************************
*/
// /
/****** DOSIcon.scalos/SCCA_DOSIcon_Size ************************************
*
*  NAME
*   SCCA_DOSIcon_Size -- (V40) I.G (ULONG)
*
*  FUNCTION
*   This attribute represents the size of the DOS object. Getting this
*   attribute will fail if the DOS object is a directory.
*
*****************************************************************************
*/
// /
/****** DOSIcon.scalos/SCCA_DOSIcon_Protection ******************************
*
*  NAME
*   SCCA_DOSIcon_Protection -- (V40) ISG (ULONG)
*
*  FUNCTION
*   The value of this attribute are the AmigaDOS protection bits.
*
*****************************************************************************
*/
// /
/****** DOSIcon.scalos/SCCA_DOSIcon_Date ************************************
*
*  NAME
*   SCCA_DOSIcon_Date -- (V40) ISG (struct DateStamp *)
*
*  FUNCTION
*   This attribute has the date of the DOS object.
*
*****************************************************************************
*/
// /
/****** DOSIcon.scalos/SCCA_DOSIcon_Comment *********************************
*
*  NAME
*   SCCA_DOSIcon_Comment -- (V40) ISG (char *)
*
*  FUNCTION
*   The DOS objects comment can be get and set using this attribute. You'll
*   got a pointer to an object internal string. If you use GetString method
*   the returned string must be freed like all strings returned from
*   GetString.
*
*****************************************************************************
*/
// /
/****** DOSIcon.scalos/SCCA_DOSIcon_OwnerUID ********************************
*
*  NAME
*   SCCA_DOSIcon_Protection -- (V40) ISG (WORD)
*
*  FUNCTION
*   If the DOS object is from a multi-user filesystem, this attribute
*   reflects the user ID. Getting a string from this attribute will return
*   the user name for this DOS object.
*
*****************************************************************************
*/
// /
/****** DOSIcon.scalos/SCCA_DOSIcon_OwnerGID ********************************
*
*  NAME
*   SCCA_DOSIcon_Protection -- (V40) ISG (WORD)
*
*  FUNCTION
*   If the DOS object is from a multi-user filesystem, this attribute
*   reflects the group ID. Getting a string from this attribute will return
*   the group name for this DOS object.
*
*****************************************************************************
*/
// /
/****** DOSIcon.scalos/SCCA_DOSIcon_FileType ********************************
*
*  NAME
*   SCCA_DOSIcon_FileType -- (V40) I.G (LONG)
*
*  FUNCTION
*   To find out with type this DOS object is this attribute can be used.
*   The DOS object is a file if the type is negative (type < 0). These values
*   are defined:
*       SCCA_DOSIcon_FileType_Root - root directory of a device
*       SCCA_DOSIcon_FileType_Dir - normal directory
*       SCCA_DOSIcon_FileType_SoftLinkDir - soft linked directory
*       SCCA_DOSIcon_FileType_LinkDir - hard linked directory
*       SCCA_DOSIcon_FileType_File - normal file
*       SCCA_DOSIcon_FileType_LinkFile - hard linked file
*       SCCA_DOSIcon_FileType_SoftLinkFile - soft linked file
*
*****************************************************************************
*/
// /

/** OpenIcon
*/
static APTR DOSIcon_OpenIcon(struct SC_Class *cl, Object *obj, struct SCCP_Icon_OpenIcon *msg, struct DOSIconInst *inst)
{
        BPTR oldcurrentdir;
        BPTR currentdir = get(obj,SCCA_DOSIcon_Lock);
        char buffer[110];
        BPTR fh;
        char *filename = (char *) get(obj,SCCA_Icon_Name);

        if (currentdir)
                oldcurrentdir = CurrentDir(inst->currentdir);
        else if (inst->path)
                oldcurrentdir = CurrentDir(Lock(inst->path,SHARED_LOCK));

        strcpy(buffer,filename);
        strcat(buffer,msg->ext);

        fh = Open(buffer,MODE_OLDFILE);
        
        if (currentdir)
                CurrentDir(oldcurrentdir);
        else if (inst->path)
                UnLock(CurrentDir(oldcurrentdir));

        return((APTR) fh);
}
// /

/** ReadIcon
*/
static ULONG DOSIcon_ReadIcon(struct SC_Class *cl, Object *obj, struct SCCP_Icon_ReadIcon *msg, struct DOSIconInst *inst)
{
        return(Read((BPTR) msg->handle, msg->mem, msg->size));
}
// /

/** CloseIcon
*/
static void DOSIcon_CloseIcon(struct SC_Class *cl, Object *obj, struct SCCP_Icon_CloseIcon *msg, struct DOSIconInst *inst)
{
        Close((BPTR) msg->handle);
}
// /

/** Init a graphic object
*/

static ULONG DOSIcon_Init(struct SC_Class *cl, Object *obj, struct SCCP_Init *msg, struct DOSIconInst *inst)
{
        if (SC_DoSuperMethodA(cl,obj, (Msg) msg))
        {
                struct TagItem *tags = msg->ops_AttrList;
                struct TagItem **taglist = &tags;
                struct TagItem *tag;
                APTR pathend;
                int  pathsize;

                while (tag = NextTagItem(taglist))
                {
                        switch (tag->ti_Tag)
                        {

                          case SCCA_DOSIcon_Lock:
                                inst->currentdir = tag->ti_Data;
                                break;

                          case SCCA_DOSIcon_Size:
                                inst->size = tag->ti_Data;
                                inst->flags |= DOSIF_SIZE;
                                break;

                          case SCCA_DOSIcon_Protection:
                                inst->prot = tag->ti_Data;
                                inst->flags |= DOSIF_PROT;
                                break;

                          case SCCA_DOSIcon_Date:
                                inst->date = *((struct DateStamp *) tag->ti_Data);
                                inst->flags |= DOSIF_DATE;
                                break;

                          case SCCA_DOSIcon_Comment:
                                inst->comment = AllocCopyString((char *) tag->ti_Data);
                                inst->flags |= DOSIF_COMMENT;
                                break;

                          case SCCA_DOSIcon_OwnerUID:
                                inst->owneruid = tag->ti_Data;
                                inst->flags |= DOSIF_UID;
                                break;

                          case SCCA_DOSIcon_OwnerGID:
                                inst->ownergid = tag->ti_Data;
                                inst->flags |= DOSIF_GID;
                                break;

                          case SCCA_DOSIcon_FileType:
                                inst->type = tag->ti_Data;
                                inst->flags |= DOSIF_TYPE;
                                break;

                          case SCCA_Icon_Path:
                                pathend = PathPart((APTR) tag->ti_Data);
                                pathsize = (int) pathend - (int) tag->ti_Data;
                                if (pathsize && (inst->path = (AllocVec(pathsize + 2,MEMF_CLEAR))))
                                {
                                        strncpy(inst->path, (char *) tag->ti_Data, pathsize);
                                        SC_SetAttrs(obj,SCCA_Icon_Name,FilePart((APTR) tag->ti_Data),TAG_DONE);
                                }
                                break;
                        }
                }
                return(TRUE);
        }
        return(FALSE);
}
// /

/** Set one or more setable attribute(s)
 */
static void DOSIcon_Set( struct SC_Class *cl, Object *obj, struct opSet *msg, struct DOSIconInst *inst )
{
        struct TagItem *tag;

        if (tag = FindTagItem(SCCA_DOSIcon_Lock,msg->ops_AttrList))
        {
                if (inst->currentdir)
                        UnLock(inst->currentdir);
                inst->currentdir = (BPTR) tag->ti_Data;
        }

        SC_DoSuperMethodA(cl,obj,(Msg) msg);
}
// /

/** get FIB if some information is missing
*/

static BOOL getfib(struct DOSIconInst *inst, Object *obj)
{
        BPTR lock = (BPTR) get(obj,SCCA_DOSIcon_Lock);
        BPTR oldlock, mylock;
        struct FileInfoBlock *fib;


        if (fib = AllocDosObject(DOS_FIB,NULL))
        {

                if (lock)
                        oldlock = CurrentDir(lock);

                if (mylock = Lock((char *) get(obj,SCCA_Icon_Name),SHARED_LOCK))
                {

                        if (Examine(mylock,fib))
                        {
                                inst->size = fib->fib_Size;
                                inst->prot = fib->fib_Protection;
                                inst->owneruid = fib->fib_OwnerUID;
                                inst->ownergid = fib->fib_OwnerGID;
                                if (inst->comment)
                                        FreeVec(inst->comment);

                                inst->comment = AllocCopyString((char *) &fib->fib_Comment);
                                inst->type = fib->fib_DirEntryType;
                                inst->date = fib->fib_Date;

                                inst->flags = 0;        // clear all flags because we've now all infos

                                UnLock(mylock);

                                if (lock)
                                        CurrentDir(oldlock);

                                FreeDosObject(DOS_FIB,fib);

                                return(TRUE);
                        }
                        UnLock(mylock);
                }

                if (lock)
                        CurrentDir(oldlock);
                FreeDosObject(DOS_FIB,fib);
        }
        return(FALSE);

}
// /

/** Get one attribute of all of the getable attributes
*/
static ULONG DOSIcon_Get( struct SC_Class *cl, Object *obj, struct opGet *msg, struct DOSIconInst *inst )
{
        if (msg->opg_AttrID == SCCA_DOSIcon_Lock)
        {
                if (inst->currentdir)
                        *(msg->opg_Storage) = (LONG) DupLock(inst->currentdir);
                else
                        *(msg->opg_Storage) = NULL;
                return(TRUE);
        }

        if (msg->opg_AttrID == SCCA_DOSIcon_Size)
        {
                if (!(inst->flags & DOSIF_SIZE) && !(getfib(inst,obj)))
                        return(FALSE);

                *(msg->opg_Storage) = inst->size;
                return(TRUE);
        }

        if (msg->opg_AttrID == SCCA_DOSIcon_Protection)
        {
                if (!(inst->flags & DOSIF_PROT) && !(getfib(inst,obj)))
                        return(FALSE);

                *(msg->opg_Storage) = inst->prot;
                return(TRUE);
        }

        if (msg->opg_AttrID == SCCA_DOSIcon_Date)
        {
                if (!(inst->flags & DOSIF_DATE) && !(getfib(inst,obj)))
                        return(FALSE);

                *(msg->opg_Storage) = (LONG) &inst->date;
                return(TRUE);
        }

        if (msg->opg_AttrID == SCCA_DOSIcon_Comment)
        {
                if (!(inst->flags & DOSIF_COMMENT) && !(getfib(inst,obj)))
                        return(FALSE);

                *(msg->opg_Storage) = (ULONG) inst->comment;
                return(TRUE);
        }

        if (msg->opg_AttrID == SCCA_DOSIcon_OwnerUID)
        {
                if (!(inst->flags & DOSIF_UID) && !(getfib(inst,obj)))
                        return(FALSE);

                *(msg->opg_Storage) = inst->owneruid;
                return(TRUE);
        }

        if (msg->opg_AttrID == SCCA_DOSIcon_OwnerGID)
        {
                if (!(inst->flags & DOSIF_GID) && !(getfib(inst,obj)))
                        return(FALSE);

                *(msg->opg_Storage) = inst->ownergid;
                return(TRUE);
        }

        if (msg->opg_AttrID == SCCA_DOSIcon_FileType)
        {
                if (!(inst->flags & DOSIF_TYPE) && !(getfib(inst,obj)))
                        return(FALSE);

                *(msg->opg_Storage) = inst->type;
                return(TRUE);
        }

        return(SC_DoSuperMethodA(cl, obj, (Msg) msg));
}
// /

/** exit routine
*/
static void DOSIcon_Exit( struct SC_Class *cl, Object *obj, Msg msg, struct DOSIconInst *inst )
{
        if (inst->currentdir)
                UnLock(inst->currentdir);
        if (inst->comment)
                FreeVec(inst->comment);
        if (inst->path)
                FreeVec(inst->path);

        SC_DoSuperMethodA(cl,obj,msg);
}
// /

/** GetString
*/

void ASM PutChar(REG(a0) struct Hook *h, REG(a1) char c, REG(a2) struct Locale *loc)
{
        *((char *) h->h_Data)++ = c;
}

static char *DOSIcon_GetString( struct SC_Class *cl, Object *obj, struct SCCP_Icon_GetString *msg, struct DOSIconInst *inst )
{
        char *retstring;

        if (msg->Attr == SCCA_DOSIcon_Size)
        {
                if (!(inst->flags & DOSIF_SIZE) && !(getfib(inst,obj)))
                        return(NULL);

                if (retstring = (char *) AllocVec(12,MEMF_ANY)) // 12 is the max for a number
                {

                        sprintf(retstring,"%ld",inst->size);
                        return(retstring);
                }
                return(NULL);
        }

        if (msg->Attr == SCCA_DOSIcon_Protection)
        {
                if (!(inst->flags & DOSIF_PROT) && !(getfib(inst,obj)))
                                return(NULL);

                if (retstring = (char *) AllocVec(10,MEMF_ANY)) // 10 for "----rwed"
                {
                        retstring[0] = (inst->prot & (1<<7))        ? 'H' : '-';
                        retstring[1] = (inst->prot & FIBF_SCRIPT)   ? 'S' : '-';
                        retstring[2] = (inst->prot & FIBF_PURE)     ? 'P' : '-';
                        retstring[3] = (inst->prot & FIBF_ARCHIVE)  ? 'A' : '-';
                        retstring[4] = (inst->prot & FIBF_READ)     ? '-' : 'R';
                        retstring[5] = (inst->prot & FIBF_WRITE)    ? '-' : 'W';
                        retstring[6] = (inst->prot & FIBF_EXECUTE)  ? '-' : 'E';
                        retstring[7] = (inst->prot & FIBF_DELETE)   ? '-' : 'D';
                        retstring[8] = 0;           // string termination
                        return(retstring);
                }
                return(NULL);
        }

        if (msg->Attr == SCCA_DOSIcon_Date)
        {
                struct Hook hook = { 0,0,(void *) PutChar,0,0 };

                if (!(inst->flags & DOSIF_DATE) && !(getfib(inst,obj)))
                                return(NULL);

                if (hook.h_Data = AllocVec(DATESIZE,MEMF_ANY | MEMF_CLEAR))
                {
                        struct Locale *loc;
                        if (loc = OpenLocale(NULL))
                        {
                                FormatDate(NULL,loc->loc_ShortDateTimeFormat,&inst->date,&hook);
                                CloseLocale(loc);
                                return(hook.h_Data);
                        }
                        FreeVec(hook.h_Data);
                }
                return(NULL);
        }

        if (msg->Attr == SCCA_DOSIcon_Comment)
        {
                if (!(inst->flags & DOSIF_COMMENT) && !(getfib(inst,obj)))
                                return(NULL);

                return(AllocCopyString(inst->comment));
        }

        if (msg->Attr == SCCA_DOSIcon_OwnerUID)
        {
                if (!(inst->flags & DOSIF_UID) && !(getfib(inst,obj)))
                                return(NULL);

                return(AllocCopyString("not supported yet"));
        }

        if (msg->Attr == SCCA_DOSIcon_OwnerGID)
        {
                if (!(inst->flags & DOSIF_GID) && !(getfib(inst,obj)))
                                return(NULL);

                return(AllocCopyString("not supported yet"));
        }

        if (msg->Attr == SCCA_DOSIcon_FileType)
        {
                if (!(inst->flags & DOSIF_TYPE) && !(getfib(inst,obj)))
                                return(NULL);

                switch (inst->type)
                {

                  case SCCA_DOSIcon_FileType_Root:
                        retstring = "Root";
                        break;

                  case SCCA_DOSIcon_FileType_Dir:
                        retstring = "Directory";
                        break;

                  case SCCA_DOSIcon_FileType_SoftLinkDir :
                        retstring = "Dir (softlink)";
                        break;

                  case SCCA_DOSIcon_FileType_LinkDir:
                        retstring = "Dir (hardlink)";
                        break;

                  case SCCA_DOSIcon_FileType_File:
                        retstring = "File";
                        break;

                  case SCCA_DOSIcon_FileType_LinkFile :
                        retstring = "File (hardlink)";
                        break;

                  case SCCA_DOSIcon_FileType_SoftLinkFile :
                        retstring = "File (softlink)";
                        break;
                }
                return(AllocCopyString(retstring));
        }

        return((char *) SC_DoSuperMethodA(cl, obj, (Msg) msg));
}
// /

/*-------------------------- MethodList --------------------------------*/

struct SC_MethodData DOSIconMethods[] =
{
        { OM_SET,(ULONG)                        DOSIcon_Set, 0, 0, NULL },               // derived from RootClass
        { OM_GET,(ULONG)                        DOSIcon_Get, 0, 0, NULL },               // derived from RootClass
        { SCCM_Init,(ULONG)                     DOSIcon_Init, 0, 0, NULL },              // derived from RootClass
        { SCCM_Icon_OpenIcon,(ULONG)            DOSIcon_OpenIcon, 0, 0, NULL },           // derived from RootClass
        { SCCM_Icon_ReadIcon,(ULONG)            DOSIcon_ReadIcon, 0, 0, NULL },           // derived from RootClass
        { SCCM_Icon_CloseIcon,(ULONG)           DOSIcon_CloseIcon, 0, 0, NULL },          // derived from RootClass
        { SCCM_Icon_GetString,(ULONG)           DOSIcon_GetString, 0, 0, NULL },         // derived from RootClass
        { SCCM_Exit,(ULONG)                     DOSIcon_Exit, 0, 0, NULL },              // derived from RootClass
        { SCMETHOD_DONE, NULL, 0, 0, NULL }
};

