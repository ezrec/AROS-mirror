/*
 * @(#) $Header$
 *
 * BGUI library
 * commodityclass.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.5  2004/06/16 20:16:48  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
 *
 * Revision 42.4  2003/01/18 19:09:56  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.3  2000/05/29 00:40:23  bergers
 * Update to compile with AROS now. Should also still compile with SASC etc since I only made changes that test the define __AROS__. The compilation is still very noisy but it does the trick for the main directory. Maybe members of the BGUI team should also have a look at the compiler warnings because some could also cause problems on other systems... (Comparison always TRUE due to datatype (or something like that)). And please compile it on an Amiga to see whether it still works... Thanks.
 *
 * Revision 42.2  2000/05/15 19:27:01  stegerg
 * another hundreds of REG() macro replacements in func headers/protos.
 *
 * Revision 42.1  2000/05/14 23:32:47  stegerg
 * changed over 200 function headers which all use register
 * parameters (oh boy ...), because the simple REG() macro
 * doesn't work with AROS. And there are still hundreds
 * of headers left to be fixed :(
 *
 * Many of these functions would also work with stack
 * params, but since i have fixed every single one
 * I encountered up to now, I guess will have to do
 * the same for the rest.
 *
 * Revision 42.0  2000/05/09 22:08:41  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:54:06  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10  1998/02/25 21:11:50  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:07:55  mlemos
 * Ian sources
 *
 *
 */

#include "include/classdefs.h"

/*
 * Internal storage of the Hotkeys.
 */
typedef struct Key {
   struct Key     *k_Next;    /* next key       */
   struct Key     *k_Prev;    /* previous key                 */
   CxObj       *k_Object;     /* The CxObj         */
   ULONG        k_KeyID;      /* key ID         */
   ULONG        k_Flags;      /* control flags     */
}  KEY;

typedef struct {
   KEY         *kl_First;     /* first key         */
   KEY         *kl_EndMark;      /* end marker        */
   KEY         *kl_Last;      /* last key       */
}  KEYLIST;

/*
 * Object instance data.
 */
typedef struct {
   KEYLIST            cd_Hotkeys;      /* list of keys         */
   struct NewBroker   cd_Broker;       /* NewBroker structure  */
   CxObj             *cd_BrokerPtr;    /* points to broker     */
   ULONG              cd_ErrorCode;    /* error code           */
   ULONG              cd_Flags;        /* control flags        */
} CD;

#define CDF_BROKERON    (1<<0)            /* broker is active     */

/*
 * For PackBoolTags().
 */
STATIC struct TagItem BoolTags[] = {
   COMM_Unique,       NBU_UNIQUE,
   COMM_Notify,       NBU_NOTIFY,
   TAG_DONE
};

/*
 * Module prototypes.
 */

/*
STATIC ASM ULONG CommClassNew(       REG(a0) Class *, REG(a2) Object *, REG(a1) struct opSet *      );
STATIC ASM ULONG CommClassDispose(   REG(a0) Class *, REG(a2) Object *, REG(a1) Msg                     );
STATIC ASM ULONG CommClassGet(       REG(a0) Class *, REG(a2) Object *, REG(a1) struct opGet *      );
STATIC ASM ULONG CommClassAddHotkey(    REG(a0) Class *, REG(a2) Object *, REG(a1) struct cmAddHotkey *    );
STATIC ASM ULONG CommClassRemHotkey(    REG(a0) Class *, REG(a2) Object *, REG(a1) struct cmDoKeyCommand * );
STATIC ASM ULONG CommClassDisableHotkey( REG(a0) Class *, REG(a2) Object *, REG(a1) struct cmDoKeyCommand * );
STATIC ASM ULONG CommClassEnableHotkey(  REG(a0) Class *, REG(a2) Object *, REG(a1) struct cmDoKeyCommand * );
STATIC ASM ULONG CommClassEnableBroker(  REG(a0) Class *, REG(a2) Object *, REG(a1) Msg                     );
STATIC ASM ULONG CommClassDisableBroker( REG(a0) Class *, REG(a2) Object *, REG(a1) Msg                     );
STATIC ASM ULONG CommClassMsgInfo(   REG(a0) Class *, REG(a2) Object *, REG(a1) struct cmMsgInfo *     );
*/

STATIC ASM REGFUNCPROTO3(ULONG, CommClassNew, REGPARAM(A0, Class *, cl), REGPARAM(A2, Object *, obj), REGPARAM(A1, struct opSet *, ops));
STATIC ASM REGFUNCPROTO3(ULONG, CommClassDispose, REGPARAM(A0, Class *, cl), REGPARAM(A2, Object *, obj), REGPARAM(A1, Msg, msg));
STATIC ASM REGFUNCPROTO3(ULONG, CommClassGet, REGPARAM(A0, Class *, cl), REGPARAM(A2, Object *, obj), REGPARAM(A1, struct opGet *, opg));
STATIC ASM REGFUNCPROTO3(ULONG, CommClassAddHotkey, REGPARAM(A0, Class *, cl), REGPARAM(A2, Object *, obj), REGPARAM(A1, struct cmAddHotkey *, cah));
STATIC ASM REGFUNCPROTO3(ULONG, CommClassRemHotkey, REGPARAM(A0, Class *, cl), REGPARAM(A2, Object *, obj), REGPARAM(A1, struct cmDoKeyCommand *, com));
STATIC ASM REGFUNCPROTO3(ULONG, CommClassDisableHotkey, REGPARAM(A0, Class *, cl), REGPARAM(A2, Object *, obj), REGPARAM(A1, struct cmDoKeyCommand *, com));
STATIC ASM REGFUNCPROTO3(ULONG, CommClassEnableHotkey, REGPARAM(A0, Class *, cl), REGPARAM(A2, Object *, obj), REGPARAM(A1, struct cmDoKeyCommand *, com));
STATIC ASM REGFUNCPROTO3(ULONG, CommClassEnableBroker, REGPARAM(A0, Class *, cl), REGPARAM(A2, Object *, obj), REGPARAM(A1, Msg, msg));
STATIC ASM REGFUNCPROTO3(ULONG, CommClassDisableBroker, REGPARAM(A0, Class *, cl), REGPARAM(A2, Object *, obj), REGPARAM(A1, Msg, msg));
STATIC ASM REGFUNCPROTO3(ULONG, CommClassMsgInfo, REGPARAM(A0, Class *, cl), REGPARAM(A2, Object *, obj), REGPARAM(A1, struct cmMsgInfo*, cmi));

/*
 * Class function table.
 */
STATIC DPFUNC ClassFunc[] = {
   OM_NEW,                 (FUNCPTR)CommClassNew,
   OM_GET,                 (FUNCPTR)CommClassGet,
   OM_DISPOSE,    (FUNCPTR)CommClassDispose,
   CM_ADDHOTKEY,     (FUNCPTR)CommClassAddHotkey,
   CM_REMHOTKEY,     (FUNCPTR)CommClassRemHotkey,
   CM_DISABLEHOTKEY, (FUNCPTR)CommClassDisableHotkey,
   CM_ENABLEHOTKEY,  (FUNCPTR)CommClassEnableHotkey,
   CM_DISABLEBROKER, (FUNCPTR)CommClassDisableBroker,
   CM_ENABLEBROKER,  (FUNCPTR)CommClassEnableBroker,
   CM_MSGINFO,    (FUNCPTR)CommClassMsgInfo,
   DF_END,                 NULL
};

/*
 * Simple class initialization.
 */
makeproto Class *InitCxClass(void)
{
   return BGUI_MakeClass(CLASS_ObjectSize, sizeof(CD),
                         CLASS_DFTable,    ClassFunc,
                         TAG_DONE);
}

/*
 * Create a shiny new object.
 */
METHOD(CommClassNew, struct opSet *, ops)
{
   CD                *cd;
   struct TagItem    *tstate = ops->ops_AttrList, *tag;
   ULONG              rc, un = NBU_UNIQUE | NBU_NOTIFY, data;

   /*
    * First we let the superclass
    * setup an object.
    */
   if (rc = AsmDoSuperMethodA(cl, obj, (Msg)ops))
   {
      /*
       * Get the instance data.
       */
      cd = INST_DATA(cl, rc);

      /*
       * Initialize Hotkey list.
       */
      NewList((struct List *)&cd->cd_Hotkeys);

      /*
       * Get us a message port.
       */
      if (cd->cd_Broker.nb_Port = CreateMsgPort())
      {
         /*
          * Initialize the broker.
          */
         cd->cd_Broker.nb_Version = NB_VERSION;

         while (tag = NextTagItem(&tstate))
         {
            data = tag->ti_Data;
            switch (tag->ti_Tag)
            {
            case COMM_Name:
               cd->cd_Broker.nb_Name = (BYTE *)data;
               break;

            case COMM_Title:
               cd->cd_Broker.nb_Title = (BYTE *)data;
               break;

            case COMM_Description:
               cd->cd_Broker.nb_Descr = (BYTE *)data;
               break;

            case COMM_Priority:
               cd->cd_Broker.nb_Pri = (BYTE)data;
               break;

            case COMM_ShowHide:
               if (data)
                  cd->cd_Broker.nb_Flags = COF_SHOW_HIDE;
               break;
            }
         };

         /*
          * Setup broker flags.
          */
         cd->cd_Broker.nb_Unique = PackBoolTags(un, ops->ops_AttrList, BoolTags);

         /*
          * Setup broker.
          */
         if (cd->cd_BrokerPtr = CxBroker(&cd->cd_Broker, NULL))
            return rc;
      }
      /*
       * Major screwup.
       */
      AsmCoerceMethod(cl, (Object *)rc, OM_DISPOSE);
   }
   return NULL;
}
METHOD_END

/*
 * Get rid of an object.
 */
METHOD(CommClassDispose, Msg,  msg)
{
   CD                *cd = INST_DATA( cl, obj );
   struct MsgPort    *mp;
   struct Message    *tmp;
   KEY               *key;

   /*
    * Scan through the list of keys.
    */
   while ( key = ( KEY * )RemHead(( struct List * )&cd->cd_Hotkeys )) {
      /*
       * Enabled?
       */
      if ( ! ( key->k_Flags & CAHF_DISABLED ))
         /*
          * Yes. Remove it.
          */
         RemoveCxObj( key->k_Object );
      /*
       * Delete the CxObj.
       */
      DeleteCxObjAll( key->k_Object );

      /*
       * Deallocate the KEY structure.
       */
      BGUI_FreePoolMem( key );
   }

   /*
    * De-activate and delete the broker.
    */
   if ( cd->cd_BrokerPtr ) {
      if ( cd->cd_Flags & CDF_BROKERON )
         ActivateCxObj( cd->cd_BrokerPtr, FALSE );
      DeleteCxObjAll( cd->cd_BrokerPtr );
   }

   /*
    * Strip the remaining messages.
    */
   if ( mp = cd->cd_Broker.nb_Port ) {
      while ( tmp = GetMsg( mp )) ReplyMsg( tmp );
      DeleteMsgPort( mp );
   }

   /*
    * The superclass takes care of the rest.
    */
   return( AsmDoSuperMethodA( cl, obj, msg ));
}
METHOD_END

/*
 * They want something.
 */

//STATIC ASM ULONG CommClassGet( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct opGet *opg )
STATIC ASM REGFUNC3(ULONG, CommClassGet,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct opGet *, opg))
{
   CD       *cd = ( CD * )INST_DATA( cl, obj );
   ULONG        rc = 1L, *store = opg->opg_Storage;

   switch ( opg->opg_AttrID ) {

      case  COMM_SigMask:
         *store = ( ULONG )( 1L << cd->cd_Broker.nb_Port->mp_SigBit );
         break;

      case  COMM_ErrorCode:
         *store = cd->cd_ErrorCode;
         break;

      default:
         rc = AsmDoSuperMethodA( cl, obj, ( Msg )opg );
         break;
   }
   return( rc );
}
REGFUNC_END

/*
 * Search for a key by it's ID.
 */
//STATIC ASM KEY *FindKey( REG(a0) KEYLIST *kl, REG(d0) ULONG keyID )
STATIC ASM REGFUNC2(KEY *, FindKey,
	REGPARAM(A0, KEYLIST *, kl),
	REGPARAM(D0, ULONG, keyID))
{
   KEY         *key;

   for ( key = kl->kl_First; key->k_Next; key = key->k_Next ) {
      if ( keyID == key->k_KeyID )
         return( key );
   }
   return( NULL );
}
REGFUNC_END

/*
 * Add a hotkey to the list.
 */
 
//STATIC ASM ULONG CommClassAddHotkey( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct cmAddHotkey *cah )
STATIC ASM REGFUNC3(ULONG, CommClassAddHotkey,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct cmAddHotkey *, cah))
{
   CD       *cd = ( CD * )INST_DATA( cl, obj );
   KEY         *key;

   /*
    * Deactivate the broker if active.
    */
   if ( cd->cd_Flags & CDF_BROKERON )
      ActivateCxObj( cd->cd_BrokerPtr, FALSE );

   /*
    * Key ID's must be unique.
    */
   if ( ! FindKey( &cd->cd_Hotkeys, cah->cah_KeyID )) {
      /*
       * Allocate a KEY structure.
       */
      if ( key = ( KEY * )BGUI_AllocPoolMem( sizeof( KEY ))) {
         /*
          * Setup the key.
          */
         if ( key->k_Object = HotKey( cah->cah_InputDescription, cd->cd_Broker.nb_Port, cah->cah_KeyID )) {
            /*
             * Initialize the KEY structure.
             */
            key->k_Flags = cah->cah_Flags;
            key->k_KeyID = cah->cah_KeyID;

            /*
             * Add the key when necessary.
             */
            if ( ! ( key->k_Flags & CAHF_DISABLED ))
               AttachCxObj( cd->cd_BrokerPtr, key->k_Object );

            /*
             * An error occured?
             */
            if ( ! CxObjError( key->k_Object )) {
               /*
                * No. Add the key to the list.
                */
               AddTail(( struct List * )&cd->cd_Hotkeys, ( struct Node * )key );

               /*
                * Clear the error code.
                */
               cd->cd_ErrorCode = CMERR_OK;

               /*
                * Activate the broker if necessary.
                */
               if ( cd->cd_Flags & CDF_BROKERON )
                  ActivateCxObj( cd->cd_BrokerPtr, TRUE );
               return 1;
            } else
               /*
                * Error from CxObjError();
                */
               cd->cd_ErrorCode = CMERR_CXOBJERROR;
            /*
             * Remove and dispose of the Cx
             * object.
             */
            if ( ! ( key->k_Flags & CAHF_DISABLED ))
               RemoveCxObj( key->k_Object );
            DeleteCxObjAll( key->k_Object );
         } else
            cd->cd_ErrorCode = CMERR_KEY_CREATION;
         /*
          * Free memory.
          */
         BGUI_FreePoolMem( key );
      } else
         cd->cd_ErrorCode = CMERR_NO_MEMORY;
   } else
      cd->cd_ErrorCode = CMERR_KEYID_IN_USE;

   /*
    * Re-activate the broker if necessary.
    */
   if ( cd->cd_Flags & CDF_BROKERON )
      ActivateCxObj( cd->cd_BrokerPtr, TRUE );

   return 0;
}
REGFUNC_END

/*
 * Remove a hotkey from the list.
 */
//STATIC ASM ULONG CommClassRemHotkey( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct cmDoKeyCommand *com )
STATIC ASM REGFUNC3(ULONG, CommClassRemHotkey,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct cmDoKeyCommand *, com))
{
   CD       *cd = ( CD * )INST_DATA( cl, obj );
   KEY         *key;
   ULONG        rc = 0L;

   /*
    * Deactivate the broker if necessary.
    */
   if ( cd->cd_Flags & CDF_BROKERON )
      ActivateCxObj( cd->cd_BrokerPtr, FALSE );

   /*
    * Locate the key.
    */
   if ( key = FindKey( &cd->cd_Hotkeys, com->cdkc_KeyID )) {
      /*
       * Remove it from the list.
       */
      Remove(( struct Node * )key );

      /*
       * Remove the CxObj when necessary.
       */
      if ( ! ( key->k_Flags & CAHF_DISABLED ))
         RemoveCxObj( key->k_Object );

      /*
       * Delete the CxObj.
       */
      DeleteCxObjAll( key->k_Object );

      /*
       * Deallocate the KEY structure.
       */
      BGUI_FreePoolMem( key );
      rc = 1L;
   }

   /*
    * Activate the broker if necessary.
    */
   if ( cd->cd_Flags & CDF_BROKERON )
      ActivateCxObj( cd->cd_BrokerPtr, TRUE );

   return( rc );
}
REGFUNC_END

/*
 * Disable a hotkey.
 */
//STATIC ASM ULONG CommClassDisableHotkey( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct cmDoKeyCommand *com )
STATIC ASM REGFUNC3(ULONG, CommClassDisableHotkey,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct cmDoKeyCommand *, com))
{
   CD       *cd = ( CD * )INST_DATA( cl, obj );
   KEY         *key;
   ULONG        rc = 0L;

   /*
    * Deactivate the broker if necessary.
    */
   if ( cd->cd_Flags & CDF_BROKERON )
      ActivateCxObj( cd->cd_BrokerPtr, FALSE );

   /*
    * Locate the key.
    */
   if ( key = FindKey( &cd->cd_Hotkeys, com->cdkc_KeyID )) {
      /*
       * Only disable when the key is enabled.
       */
      if ( ! ( key->k_Flags & CAHF_DISABLED )) {
         /*
          * Remove the key.
          */
         RemoveCxObj( key->k_Object );

         /*
          * Mark it as disabled.
          */
         key->k_Flags |= CAHF_DISABLED;
         rc = 1;
      }
   }

   /*
    * Activate the broker if necessary.
    */
   if ( cd->cd_Flags & CDF_BROKERON )
      ActivateCxObj( cd->cd_BrokerPtr, TRUE );

   return( rc );
}
REGFUNC_END

/*
 * Enable a hotkey.
 */

//STATIC ASM ULONG CommClassEnableHotkey( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct cmDoKeyCommand *com )
STATIC ASM REGFUNC3(ULONG, CommClassEnableHotkey,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct cmDoKeyCommand *, com))
{
   CD       *cd = ( CD * )INST_DATA( cl, obj );
   KEY         *key;
   ULONG        rc = 0L;

   /*
    * Deactivate the broker if necessary.
    */
   if ( cd->cd_Flags & CDF_BROKERON )
      ActivateCxObj( cd->cd_BrokerPtr, FALSE );

   /*
    * Locate the key.
    */
   if ( key = FindKey( &cd->cd_Hotkeys, com->cdkc_KeyID )) {
      /*
       * Only enable when the key is disabled.
       */
      if ( key->k_Flags & CAHF_DISABLED ) {
         /*
          * Add the key.
          */
         AttachCxObj( cd->cd_BrokerPtr, key->k_Object );

         /*
          * Clear the disabled bit.
          */
         key->k_Flags &= ~CAHF_DISABLED;
         rc = 1L;
      }
   }

   /*
    * Activate the broker if necessary.
    */
   if ( cd->cd_Flags & CDF_BROKERON )
      ActivateCxObj( cd->cd_BrokerPtr, TRUE );

   return( rc );
}
REGFUNC_END

/*
 * Enable the broker.
 */
//STATIC ASM ULONG CommClassEnableBroker( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) Msg msg )
STATIC ASM REGFUNC3(ULONG, CommClassEnableBroker,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, Msg, msg))
{
   CD       *cd = ( CD * )INST_DATA( cl, obj );
   ULONG        rc = 0L;

   /*
    * Only enable when the broker is disabled.
    */
   if ( ! ( cd->cd_Flags & CDF_BROKERON )) {
      /*
       * Activate the broker.
       */
      ActivateCxObj( cd->cd_BrokerPtr, TRUE );

      /*
       * Mark us as active.
       */
      cd->cd_Flags |= CDF_BROKERON;
      rc = 1L;
   }
   return( rc );
}
REGFUNC_END

/*
 * Disable the broker.
 */
//STATIC ASM ULONG CommClassDisableBroker( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) Msg msg )
STATIC ASM REGFUNC3(ULONG, CommClassDisableBroker,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, Msg, msg))
{
   CD       *cd = ( CD * )INST_DATA( cl, obj );
   ULONG        rc = 0L;

   /*
    * Only disable when the broker is enabled.
    */
   if ( cd->cd_Flags & CDF_BROKERON ) {
      /*
       * Deactivate the broker.
       */
      ActivateCxObj( cd->cd_BrokerPtr, FALSE );

      /*
       * Clear the enabled bit.
       */
      cd->cd_Flags &= ~CDF_BROKERON;
      rc = 1L;
   }
   return( rc );
}
REGFUNC_END

/*
 * Poll broker port.
 */
//STATIC ASM ULONG CommClassMsgInfo( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct cmMsgInfo *cmi )
STATIC ASM REGFUNC3(ULONG, CommClassMsgInfo,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct cmMsgInfo*, cmi))
{
   CD          *cd = ( CD * )INST_DATA( cl, obj );
   CxMsg          *msg;
   ULONG           rc = 0, type, id, data;

   /*
    * Get a message from the port.
    */
   if ( msg = ( CxMsg * )GetMsg( cd->cd_Broker.nb_Port )) {
      /*
       * Get message data.
       */
      type = CxMsgType(msg);
      id   = CxMsgID(msg);
      data = (ULONG)CxMsgData(msg);

      /*
       * Save data where ever possible.
       */
      if ( cmi->cmi_Info.Type ) *( cmi->cmi_Info.Type ) = type;
      if ( cmi->cmi_Info.ID   ) *( cmi->cmi_Info.ID   ) = id;
      if ( cmi->cmi_Info.Data ) *( cmi->cmi_Info.Data ) = data;

      /*
       * Setup V40 return codes.
       */
      switch (type)
      {
      case CXM_IEVENT:
         rc = id;
         break;

      case CXM_COMMAND:
         switch(id)
         {
         case CXCMD_KILL:
            rc = CMMI_KILL;
            break;

         case CXCMD_DISABLE:
            AsmDoMethod(obj, CM_DISABLEBROKER);
            rc = CMMI_DISABLE;
            break;

         case CXCMD_ENABLE:
            AsmDoMethod(obj, CM_ENABLEBROKER);
            rc = CMMI_ENABLE;
            break;

         case CXCMD_UNIQUE:
            rc = CMMI_UNIQUE;
            break;

         case CXCMD_APPEAR:
            rc = CMMI_APPEAR;
            break;

         case CXCMD_DISAPPEAR:
            rc = CMMI_DISAPPEAR;
            break;
         }
         break;
      }

      /*
       * Reply the message.
       */
      ReplyMsg(( struct Message *)msg );
   } else
      /*
       * No more messages.
       */
      rc = CMMI_NOMORE;

   return( rc );
}
REGFUNC_END
