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
/* --------------- M68k Functions --------------------- */
ULONG SC_DoMethodA(Object *obj, Msg msg);
ULONG SC_DoMethod(Object *obj, ULONG arg1, ...);
ULONG SC_DoClassMethodA(struct SC_Class *cl, char *id, ULONG data, Msg msg);
ULONG SC_DoClassMethod(struct SC_Class *cl, char *id,  ULONG data, ULONG arg1, ...);
struct SC_Class *SC_MakeClass(char *id, char *superid, struct SC_Class *superclass,HOOKFUNC disp, struct SC_MethodData *mdata, ULONG instsize, ULONG flags);
ULONG SC_FreeClass(struct SC_Class *class);
Object *SC_NewObjectA(struct SC_Class *class, char *id, struct TagItem *taglist);
Object *SC_NewObject(struct SC_Class *class, char *id, ULONG tag1, ...);
void SC_DisposeObject(Object *obj);
struct SC_Class *SC_MakeSimpleClass( char *id, char *superid, struct SC_Class *super, struct SC_MethodData *mdata, ULONG instsize, ULONG flags);
void SC_SetAttrsA( Object *obj , struct TagItem *taglist);
void SC_SetAttrs(Object *obj, ULONG tag1, ...);
ULONG SC_GetAttr( ULONG id, Object *obj , ULONG *attr);
ULONG SC_DoMsgMethodA(struct MsgPort *replyport, Object *obj, Msg msg);
ULONG SC_DoMsgMethod(struct MsgPort *replyport, Object *obj, ULONG msgdata, ...);
struct SC_Message *SC_AllocMsg(UWORD msgtype, ULONG size);
void SC_FreeMsg(struct SC_Message *smsg);
ULONG SC_IsScalosMsg(struct Message *msg);
ULONG SC_DoSuperMethodA(struct SC_Class *cl, Object *obj, Msg msg);
ULONG SC_DoSuperMethod(struct SC_Class *cl, Object *obj, ULONG arg1, ...);
APTR SC_AllocVec(ULONG size, ULONG memtype);
void SC_FreeVec(APTR address);
struct SC_Message *SC_AllocInterMsg(UWORD msgtype, ULONG size, struct MsgPort *replyport);
void SC_PutInterMsg(struct MsgPortPPC *msgport, struct SC_Message *smsg);

/* --------------- PPC Functions ---------------------- */
ULONG SC_DoMethodAPPC(Object *obj, Msg msg);
ULONG SC_DoMethodPPC(Object *obj, ULONG arg1, ...);
ULONG SC_DoClassMethodAPPC(struct SC_Class *cl, char *id, ULONG data, Msg msg);
ULONG SC_DoClassMethodPPC(struct SC_Class *cl, char *id,  ULONG data, ULONG arg1, ...);
struct SC_Class *SC_MakeClassPPC(char *id, char *superid, struct SC_Class *superclass,HOOKFUNC disp, struct SC_MethodData *mdata, ULONG instsize, ULONG flags);
ULONG SC_FreeClassPPC(struct SC_Class *class);
Object *SC_NewObjectAPPC(struct SC_Class *class, char *id, struct TagItem *taglist);
Object *SC_NewObjectPPC(struct SC_Class *class, char *id, ULONG tag1, ...);
void SC_DisposeObjectPPC(Object *obj);
struct SC_Class *SC_MakeSimpleClass( char *id, char *superid, struct SC_Class *super, struct SC_MethodData *mdata, ULONG instsize, ULONG flags);
void SC_SetAttrsAPPC( Object *obj , struct TagItem *taglist);
void SC_SetAttrsPPC(Object *obj, ULONG tag1, ...);
ULONG SC_GetAttrPPC( ULONG id, Object *obj , ULONG *attr);
ULONG SC_DoMsgMethodAPPC(struct MsgPortPPC *replyport, Object *obj, Msg msg);
ULONG SC_DoMsgMethodPPC(struct MsgPortPPC *replyport, Object *obj, ULONG msgdata, ...);
struct SC_Message *SC_AllocMsgPPC(UWORD msgtype, ULONG size);
void SC_FreeMsgPPC(struct SC_Message *smsg);
ULONG SC_IsScalosMsgPPC(struct Message *msg);
ULONG SC_DoSuperMethodAPPC(struct SC_Class *cl, Object *obj, Msg msg);
ULONG SC_DoSuperMethodPPC(struct SC_Class *cl, Object *obj, ULONG arg1, ...);
APTR SC_AllocVecPPC(ULONG size, ULONG memtype);
void SC_FreeVecPPC(APTR address);
struct SC_Message *SC_AllocInterMsgPPC(UWORD msgtype, ULONG size, struct MsgPortPPC *replyport);
void SC_PutInterMsgPPC(struct MsgPort *msgport, struct SC_Message *smsg);
