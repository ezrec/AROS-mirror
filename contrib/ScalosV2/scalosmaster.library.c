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
#include <clib/alib_protos.h>
#include <clib/exec_protos.h>
#include <clib/powerpc_protos.h>
#include <clib/utility_protos.h>
#include <exec/lists.h>
#include <exec/memory.h>
#include <exec/semaphores.h>
#include <exec/nodes.h>

#include "scalos.h"
#include "scalosintern.h"
#include "MethodSenderClass.h"
#include "CompilerSupport.h"
#include "debug.h"
#include "scalos_protos.h"

/****** scalosmaster.library/--background ***********************************
*
*   This library provides a object-oriented system similar to BOOPSI and
*   a number of classes. It's not allowed to mix functions from BOOPSI and
*   Scalos. This will cause big problems.
*   On PPC the ScalosMasterBase must always be passed in r3.
*
*****************************************************************************
*/

struct MinList ClassList;
struct SignalSemaphore ClassSem;

struct SC_Class *rootclass;
struct SC_Class *threadrootclass;
struct SC_Class *methodsenderclass;

typedef unsigned long (*METHODFUNC)(struct SC_Class *cl, Object *obj, Msg msg, APTR instdata);

typedef unsigned long ASM (*CLASSFUNC)(REG(a0) struct SC_Class *cl, REG(a2) ULONG data, REG(a1) Msg msg);

/* -------------------------- Extern Routines ----------------------------- */

extern struct MethodData RootMethods;
extern struct MethodData ThreadRootMethods;

extern ULONG MethodSenderDispatcher(struct SC_Class *cl, Object *obj, Msg msg);

extern ULONG PutMethod(struct MethodSenderInst *inst, Msg msg, struct MsgPort *replyport);

/* ------------------------------------------------------------------------------------------------ */

/****** scalosmaster.library/SC_AllocMsg ************************************
*
*  NAME
*   SC_AllocMsg -- Allocate a scalos msg
*
*  SYNOPSIS
*   smsg = SC_AllocMsg( mtype, size );
*   D0                  D0     D1
*
*   struct SC_Message *SC_AllocMsg( UWORD, ULONG)
*
*  FUNCTION
*   This function is used to allocate message which will be used for the
*   internal comunications in scalos.
*
*  INPUTS
*   mtype - scalos message type
*   size - size of the message. This must be at least sizeof(struct SC_Message).
*
*  RESULT
*   smsg - a struct SC_Message or NULL if it fails
*
*  NOTE
*   This function is PRIVATE !
*
*  SEE ALSO
*
*****************************************************************************
*/

struct SC_Message *SC_AllocMsg(UWORD msgtype, ULONG size)
{
        struct SC_Message *smsg;

        if (size >= sizeof(struct SC_Message))
        {
                if (smsg = AllocVec(size, MEMF_PUBLIC | MEMF_CLEAR))
                {
                        smsg->scalosmid = SCALOS_MSGID;
                        smsg->execmsg.mn_Length = size;
                        smsg->id = msgtype;
                        return(smsg);
                }
        }
        return(NULL);
}
// /

/****** scalosmaster.library/SC_FreeMsg *************************************
*
*  NAME
*   SC_FreeMsg -- frees a SC_Message.
*
*  SYNOPSIS
*   SC_FreeMsg( smsg );
*               A0
*
*  SC_FreeMsg( struct SC_Message * )
*
*  FUNCTION
*   This functions frees a previously allocated scalos message. It will free
*   both inter- and standard messages.
*
*  INPUTS
*   smsg - scalos message to free
*
*  RESULT
*
*  NOTE
*   This function is PRIVATE !
*
*  SEE ALSO
*   SC_AllocMsg
*
*****************************************************************************
*/

void SC_FreeMsg(struct SC_Message *smsg)
{
        FreeVec(smsg);
}
// /


/****** scalosmaster.library/SC_IsScalosMsg *********************************
*
*  NAME
*   SC_IsScalosMsg -- finds out if a message is a scalos message.
*
*  SYNOPSIS
*   mtype = SC_IsScalosMsg( msg );
*   D0                      A0
*
*  ULONG SC_IsScalosMsg( struct Message * )
*
*  FUNCTION
*   This functions check a message if it's a scalos message.
*
*  INPUTS
*   msg - exec message to check
*
*  RESULT
*   mtype - scalos message type or 0 if it's not a scalos message
*
*  NOTE
*
*  SEE ALSO
*   SC_AllocMsg, SC_FreeMsg
*
*****************************************************************************
*/

ULONG SC_IsScalosMsg(struct Message *msg)
{
        if (msg->mn_Length >= sizeof(struct SC_Message))
                if (((struct SC_Message *) msg)->scalosmid == SCALOS_MSGID)
                        return((ULONG) ((struct SC_Message *) msg)->id);
        return(0);
}
// /

/****** scalosmaster.library/SC_DoMethodA ***********************************
*
*  NAME
*   SC_DoMethodA -- Do a method for an object.
*   SC_DoMethod -- Varargs stub for SC_DoMethodA.
*
*  SYNOPSIS
*   result = SC_DoMethodA( obj, msg );
*   D0                     A0   A1
*
*   ULONG SC_DoMethodA( Object *, Msg )
*
*   result = SC_DoMethod( obj, method, ... );
*
*   ULONG SC_DoMethod( Object *, ULONG, ... )
*
*  FUNCTION
*   This function executes a specific method for an object.
*
*  INPUTS
*   obj - Object to execute the method for.
*   msg - Method and method attributes. Depends on the object.
*
*  RESULT
*   depends on the object
*
*  SEE ALSO
*   intuition/classusr.h
*
*****************************************************************************
*/

ULONG SC_DoMethodA(Object *obj, Msg msg)
{
        if (obj)
        {
                return(((CLASSFUNC) SCOCLASS(obj)->Dispatcher.h_Entry) (SCOCLASS(obj), (ULONG) obj, msg));
        }
        return(0);
}
// /

/****** scalosmaster.library/SC_DoClassMethodA ******************************
*
*  NAME
*   SC_DoClassMethodA -- Do a method for a class.
*   SC_DoClassMethod -- Varargs stub for SC_DoClassMethodA.
*
*  SYNOPSIS
*   result = SC_DoClassMethodA( class, classname, data, msg );
*   D0                          A0     A2         D0    A1
*
*   ULONG SC_DoClassMethodA( struct SC_Class *, char *, ULONG, Msg )
*
*   result = SC_DoClassMethod( class, classname, data, method, ... );
*
*   ULONG SC_DoClassMethod( struct SC_Class *, char *, ULONG, ULONG, ... )
*
*  FUNCTION
*   This function executes a method for a class. It's only allowed to use
*   class methods if the documentation of the class allows that.
*
*  INPUTS
*   class - Class to execute the method or NULL
*   classname - A name of a class of class is NULL.
*   data - Depends on the method.
*   msg - Method and method attributes. Depends on the object.
*
*  RESULT
*   depends on the class
*
*  SEE ALSO
*   intuition/classusr.h
*
*****************************************************************************
*/

ULONG SC_DoClassMethodA(struct SC_Class *cl, char *id, ULONG data, Msg msg)
{
        if (!(cl))
        {
                struct SC_Class *classnode;
                ObtainSemaphoreShared(&ClassSem);
                for (classnode = (struct SC_Class *) ClassList.mlh_Head; classnode->Dispatcher.h_MinNode.mln_Succ; classnode = (struct SC_Class *) classnode->Dispatcher.h_MinNode.mln_Succ)
                {
                        if (!(Stricmp(classnode->ClassName, id)))
                        {
                                cl = classnode;
                                break;
                        }
                }
                ReleaseSemaphore(&ClassSem);
        }
        if (cl)
                return(((CLASSFUNC) cl->Dispatcher.h_Entry) (cl, data, msg));
        else
                return(0);
}
// /

/****** scalosmaster.library/SC_DoSuperMethodA ******************************
*
*  NAME
*   SC_DoSuperMethodA -- Do a method for a superclass of a class.
*   SC_DoSuperMethod -- Varargs stub for SC_DoSuperMethodA.
*
*  SYNOPSIS
*   result = SC_DoSuperMethodA( class, object, msg );
*   D0                          A0     A2   A1
*
*   ULONG SC_DoSuperMethodA( struct SC_Class *, Object *, Msg )
*
*   result = SC_DoSuperMethod( class, object, method, ... );
*
*   ULONG SC_DoClassMethod( struct SC_Class *, Object *, ULONG, ... )
*
*  FUNCTION
*   This function calls the superclass of a class. It's one of the main
*   function of a object oriented programming system.
*
*  INPUTS
*   class - the method will be called for the superclass of this class
*   object - the object to call the method for
*   msg - Method and method attributes. Depends on the object.
*
*  RESULT
*   depends on the class and the method
*
*  SEE ALSO
*   intuition/classusr.h
*
*****************************************************************************
*/

ULONG SC_DoSuperMethodA(struct SC_Class *cl, Object *obj, Msg msg)
{
        return(((CLASSFUNC) (cl->Super)->Dispatcher.h_Entry) ((cl->Super), (ULONG) obj, msg));
}
// /

/****** scalosmaster.library/SC_DoMsgMethodA ********************************
*
*  NAME
*   SC_DoMsgMethodA -- Do a asyncron method for an object.
*   SC_DoMsgMethod -- Varargs stub for SC_DoMsgMethodA.
*
*  SYNOPSIS
*   succ = SC_DoMsgMethodA( replyport, object, msg );
*   D0                      A0       A2      A1
*
*   ULONG SC_DoMsgMethodA( struct MsgPort *, Object *, Msg )
*
*   succ = SC_DoMsgMethod( replyport, object, arg1, ... );
*
*   ULONG SC_DoMsgMethod( struct MsgPort *, Object *, ULONG, ... )
*
*  FUNCTION
*   With this functions it's possible to send a method with a message to
*   a ThreadRoot subclass object and wait for the reply yourself.
*
*  INPUTS
*   replyport - messageport to send the reply to
*   object - object where the method should be executed
*   msg - method information 
*
*  RESULT
*   TRUE - if the message was successfully sent
*   FALSE - if the message could not been sent because of low memory
*           conditions or simple the object isn't from a threadroot subclass
*
*  SEE ALSO
*
*****************************************************************************
*/

ULONG SC_DoMsgMethodA(struct MsgPort *replyport, Object *obj, Msg msg)
{
        if (((struct MethodSenderInst *) obj)->senderid == SCALOS_SENDERID)
                return(PutMethod((struct MethodSenderInst *) obj, msg, replyport));

        return(FALSE);
}
// /

/****** scalosmaster.library/SC_MakeClass ***********************************
*
*  NAME
*   SC_MakeClass -- makes a new class and adds to the class list
*
*  SYNOPSIS
*   class = SC_MakeClass( classname, supername, superclass, dispatcher, methoddata, instsize, flags);
*   D0                    A0         A1         A2          A3          A4          D0        D1
*
*   struct SC_Class *SC_MakeClass( char *, char *, struct SC_Class *, HOOKFUNC, struct SC_MethodData *, ULONG, ULONG )
*
*  FUNCTION
*   This function makes a new class and makes it available for other
*   programs.
*   The dispatcher will be called with a pointer to the SC_Class structure
*   in A0/r3, a pointer to the methodmessage in A1/r4 and a pointer to the
*   object, or data for class methods, in A2/r5.
*
*  INPUTS
*   classname - name of the new class of NULL for a private class
*   supername - name of the superclass or NULL for a private superclass
*   superclass - superclass of supername is NULL
*   dispatcher - pointer to the dispatcher function
*   methoddata - pointer to a methoddata structure with all infos about the
*                methods of your class
*   instsize - size of the instance variables
*   flags - currently 0
*
*  RESULT
*   class - pointer to a SC_Class structure of the new class or NULL if
*           class generating failes
*
*  SEE ALSO
*
*****************************************************************************
*/

struct SC_Class *SC_MakeClass(char *id, char *superid, struct SC_Class *superclass,HOOKFUNC disp, struct SC_MethodData *mdata, ULONG instsize, ULONG flags)
{
        struct SC_Class *newclass;
        struct SC_Class *newsuperclass;

        if ((!(newsuperclass = superclass)) && superid)
        {
                struct SC_Class *classnode;
                ObtainSemaphoreShared(&ClassSem);
                for (classnode = (struct SC_Class *) ClassList.mlh_Head; classnode->Dispatcher.h_MinNode.mln_Succ; classnode = (struct SC_Class *) classnode->Dispatcher.h_MinNode.mln_Succ)
                {
                        if (!(Stricmp(classnode->ClassName, superid)))
                        {
                                newsuperclass = classnode;
                                break;
                        }
                }
                ReleaseSemaphore(&ClassSem);
        }

        if (newclass = (struct SC_Class *) AllocVec(sizeof(struct SC_Class), MEMF_PUBLIC | MEMF_CLEAR))
        {
                newclass->Dispatcher.h_Entry = disp;
                newclass->Methods = mdata;
                newclass->Super = newsuperclass;
                newclass->ClassName = id;
                newclass->InstSize = (instsize+3) & ~3;
                newclass->Flags = flags & ~SCCLF_PPC;
                if (newsuperclass)
                {
                        newclass->InstOffset = newsuperclass->InstOffset + newsuperclass->InstSize;
                        newsuperclass->SubclassCount += 1;
                }
                ObtainSemaphore(&ClassSem);
                AddHead((struct List *) &ClassList, (struct Node *) newclass);
                ReleaseSemaphore(&ClassSem);
                return(newclass);
        }
        return(NULL);
}
// /

/****** scalosmaster.library/SC_FreeClass ***********************************
*
*  NAME
*   SC_FreeClass -- free a class and removes it from the classlist
*
*  SYNOPSIS
*   success = SC_FreeClass( class );
*   D0                      A0
*   r3                      r4
*
*   ULONG SC_FreeClass( struct SC_Class * )
*
*  FUNCTION
*   This function removes the class from the classlist and frees all data.
*
*  INPUTS
*   class - the class to free
*
*  RESULT
*   FALSE - If the free fails. This happens if there are subclasses or
*           objects of this class.
*   TRUE - if anything was successful
*
*  SEE ALSO
*   SC_MakeClass
*
*****************************************************************************
*/

ULONG SC_FreeClass(struct SC_Class *class)
{
        ULONG ret = FALSE;

        ObtainSemaphore(&ClassSem);
        if (!(class->SubclassCount) && !(class->ObjectCount))
        {
                Remove((struct Node *) class);
                if (class->Super)
                        class->Super->SubclassCount -= 1;
                FreeVec(class);
                ret = TRUE;
        }
        ReleaseSemaphore(&ClassSem);
        return(ret);
}
// /

/****** scalosmaster.library/SC_NewObjectA **********************************
*
*  NAME
*   SC_NewObjectA -- Generate a new object.
*   SC_NewObject -- Varargs stub for SC_NewObjectA.
*
*  SYNOPSIS
*   object = SC_NewObjectA( class, classname, taglist );
*   D0                      A0         A1     A2
*
*   Object *SC_NewObjectA( struct SC_Class *, char *, struct TagItem * )
*
*   object = SC_NewObject( class, classname, tag1, ... );
*
*   Object *SC_NewObject( struct SC_Class *, char *, ULONG, ... )
*
*  FUNCTION
*   A new object of a specified class will be generated.
*
*  INPUTS
*   class - class to generate an object from or NULL
*   classname - class name if class is NULL
*   taglist - taglist with the init attributes
*
*  RESULT
*   object - an object or NULL if the generation fails
*
*  SEE ALSO
*
*****************************************************************************
*/

Object *SC_NewObjectA(struct SC_Class *cl, char *id, struct TagItem *taglist)
{
        if (!(cl))
        {
                struct SC_Class *classnode;
                ObtainSemaphoreShared(&ClassSem);
                for (classnode = (struct SC_Class *) ClassList.mlh_Head; classnode->Dispatcher.h_MinNode.mln_Succ; classnode = (struct SC_Class *) classnode->Dispatcher.h_MinNode.mln_Succ)
                {
                        if (!(Stricmp(classnode->ClassName, id)))
                        {
                                cl = classnode;
                                break;
                        }
                }
                ReleaseSemaphore(&ClassSem);
        }
        if (cl)
                return((Object *) SC_DoClassMethod(cl, NULL, (ULONG) cl,OM_NEW,taglist));
        else
        {
                return(NULL);
        }
}

// /

/****** scalosmaster.library/SC_DisposeObject *******************************
*
*  NAME
*   SC_DisposeObject -- disposes an object
*
*  SYNOPSIS
*   SC_DisposeObject( object );
*   D0                A0
*
*   SC_DisposeObject( Object * )
*
*  FUNCTION
*   This function disposes an object.
*
*  INPUTS
*   object - object to be disposed
*
*  RESULT
*
*  SEE ALSO
*   SC_NewObjectA
*
*****************************************************************************
*/
void SC_DisposeObject(Object *obj)
{
        SC_DoMethod(obj,OM_DISPOSE);
}
// /

/****** scalosmaster.library/SC_MakeSimpleClass *****************************
*
*  NAME
*   SC_MakeSimpleClass -- makes a new class very easy
*
*  SYNOPSIS
*   class = SC_MakeSimpleClass( classname, supername, superclass, methoddata, instsize, flags);
*   D0                          A0         A1         A2          A3          D0        D1
*
*   struct SC_Class *SC_MakeSimpleClass( char *, char *, struct SC_Class *, struct SC_MethodData *, ULONG, ULONG )
*
*  FUNCTION
*   This function is similar to SC_MakeClass, but installes a standard
*   dispatcher which uses the SC_MethodData UserData field as function-
*   pointer. All function args will be on the stack. The function prototype
*   looks like that:
*   ULONG MethodFunc(struct SC_Class *cl, Object *obj, Msg msg, UBYTE *inst);
*   inst - is a pointer to the instance data of your object. If you provide
*   a function for SCMETHOD_DONE it will always be called if no method
*   function was found in the SC_MethodData array.
*
*  INPUTS
*   classname - name of the new class of NULL for a private class
*   supername - name of the superclass or NULL for a private superclass
*   superclass - superclass of supername is NULL
*   methoddata - pointer to a methoddata structure with all infos about the
*                methods of your class and method functions.
*   instsize - size of the instance variables
*   flags - currently 0
*
*  RESULT
*   class - pointer to a SC_Class structure of the new class or NULL if
*           class generating failes
*
*  NOTE
*   *   obj and inst variables aren't valid for OM_NEW !
*
*  SEE ALSO
*   SC_MakeClass
*
*****************************************************************************
*/

static SAVEDS ASM ULONG dispatcher( REG(a0) struct SC_Class *cl, REG(a1) Msg msg, REG(a2) Object *obj )
{
        struct SC_MethodData *array = cl->Methods;

        for (array = cl->Methods; array->MethodID != SCMETHOD_DONE; array++)
        {
                if ((array->MethodID == msg->MethodID) && array->UserData)
                        return( ((METHODFUNC) array->UserData)( cl, obj, msg, SCINST_DATA(cl,obj) ));
        }

        if (array->UserData) // if function is defined there
                return( ((METHODFUNC) array->UserData)( cl, obj, msg, SCINST_DATA(cl,obj) ));
        else
                return( SC_DoSuperMethodA( cl, obj, msg ) );
}

struct SC_Class *SC_MakeSimpleClass( char *id, char *superid, struct SC_Class *super, struct SC_MethodData *mdata, ULONG instsize, ULONG flags)
{
        return(SC_MakeClass(id,superid,super, (HOOKFUNC) dispatcher,mdata,instsize,flags));
}
// /

/****** scalosmaster.library/SC_SetAttrsA ***********************************
*
*  NAME
*   SC_SetAttrsA -- sets attributes for an object
*   SC_SetAttrs -- Varargs stub for SC_SetAttrsA
*
*  SYNOPSIS
*   SC_SetAttrsA( object, taglist );
*                 A0      A1
*
*   SC_SetAttrsA( Object *, struct TagItem *)
*
*   SC_SetAttrs( object, tag1, ... );
*
*   SC_SetAttrs( Object *, ULONG, .... )
*
*  FUNCTION
*   This function set attributes for an object.
*
*  INPUTS
*   object - object for the attributes
*   taglist - attributes to set
*
*  RESULT
*
*  SEE ALSO
*
*****************************************************************************
*/

void SC_SetAttrsA( Object *obj , struct TagItem *taglist)
{
        SC_DoMethod(obj, OM_SET, taglist);
}
// /

/****** scalosmaster.library/SC_GetAttr *************************************
*
*  NAME
*   SC_GetAttr -- gets an attribute from an object
*
*  SYNOPSIS
*   succ = SC_GetAttr( id, object, attr );
*   D0                 D0  A0      A1
*
*   ULONG SC_GetAttrs( Object *, ULONG, ULONG *)
*
*  FUNCTION
*   This function get an attribute from an object.
*
*  INPUTS
*   id - attribute id
*   object - object to get the attribute from
*   attr - pointer to a ULONG to store the result
*
*  RESULT
*   TRUE - if the get was succesful
*   FALSE - if the id was unknown, the value store is not defined
*
*  SEE ALSO
*
*****************************************************************************
*/

ULONG SC_GetAttr( ULONG id, Object *obj , ULONG *attr)
{
        return(SC_DoMethod(obj, OM_GET, id, attr));
}
// /

/* --------------- These function will be replaced by compiler ones, if it's a library ------------ */
ULONG SC_DoMethod(Object *obj, ULONG arg1, ...)
{
        return(SC_DoMethodA(obj,(Msg) &arg1));
}

ULONG SC_DoClassMethod(struct SC_Class *cl, char *id,  ULONG data, ULONG arg1, ...)
{
        return(SC_DoClassMethodA(cl, id, data, (Msg) &arg1));
}

void SC_SetAttrs(Object *obj, ULONG tag1, ...)
{
        SC_SetAttrsA(obj,(struct TagItem *) &tag1);
}

ULONG SC_DoMsgMethod(struct MsgPort *replyport, Object *obj, ULONG msgdata, ...)
{
        return(SC_DoMsgMethodA(replyport, obj, (Msg) &msgdata));
}

ULONG SC_DoSuperMethod(struct SC_Class *cl, Object *obj, ULONG arg1, ...)
{
        return(SC_DoSuperMethodA(cl, obj , (Msg) &arg1));
}

Object *SC_NewObject(struct SC_Class *cl, char *classname, ULONG arg1, ...)
{
        return(SC_NewObjectA(cl, classname , (struct TagItem *) &arg1));
}

/* ---------------------------------------------------------------------- */

/* ---------------------------------- LibraryStuff ----------------------- */


ULONG ScalosMasterInit(void)
{
        NewList((struct List *) &ClassList);
        InitSemaphore(&ClassSem);
        rootclass = SC_MakeSimpleClass(SCC_ROOT_NAME,NULL,NULL,(struct SC_MethodData *) &RootMethods, 0, 0);
        threadrootclass = SC_MakeSimpleClass(SCC_THREADROOT_NAME,NULL,NULL,(struct SC_MethodData *) &ThreadRootMethods, 0, 0);
        methodsenderclass = SC_MakeClass(SCC_METHODSENDER_NAME,SCC_ROOT_NAME,NULL,(HOOKFUNC) MethodSenderDispatcher,NULL,sizeof(struct MethodSenderInst),0);
        return(TRUE);
}

void ScalosMasterExit(void)
{
        SC_FreeClass(methodsenderclass);
        SC_FreeClass(threadrootclass);
        SC_FreeClass(rootclass);

}
