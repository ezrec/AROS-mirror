/*
 * @(#) $Header$
 *
 * BGUI library
 * arexxclass.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.3  2004/06/16 20:16:48  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
 *
 * Revision 42.2  2000/08/08 20:57:26  chodorowski
 * Minor fixes to build on Amiga.
 *
 * Revision 42.1  2000/05/15 19:27:00  stegerg
 * another hundreds of REG() macro replacements in func headers/protos.
 *
 * Revision 42.0  2000/05/09 22:08:13  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:21:32  mlemos
 * Bumped to revision 41.11
 *
 * Revision 41.1  2000/05/09 19:53:44  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.1  1998/10/12 01:32:38  mlemos
 * Initial revision adapted from Ian sources.
 *
 *
 *
 */

/*
**      Include necessary headers.
**/
#include "include/classdefs.h"

/*
**      Object instance data. Every object
**      created from this class has this
**      data.
**/
typedef struct {
   struct MsgPort     *ad_Port;       /* Host message port.         */
   UBYTE              *ad_HostName;   /* Host name.                 */
   UBYTE              *ad_FileExt;    /* File extention.            */
   ULONG               ad_PendCnt;    /* Messages still un-replied. */
   REXXCOMMAND        *ad_Commands;   /* Host command list.         */
   struct RDArgs      *ad_DOSParser;  /* ReadArgs() parser.         */
}  AD;

/*
**      Class data.
**/
typedef struct {
   struct Library *RexxSysLibraryBase;
}  CD;

#define RexxSysBase ((CD *)cl->cl_UserData)->RexxSysLibraryBase

/*
**      Duplicate a string.
**/
STATIC UBYTE *DupStr( AD *ad, UBYTE *str )
{
        UBYTE                   *dup = BGUI_AllocPoolMem( strlen( str ) + 1 );

        /*
        **      Allocation OK?
        **/
        if ( dup )
                /*
                **      Copy the string.
                **/
                strcpy( dup, str );

        return( dup );
}

/*
**      Reply a rexx command.
**/
STATIC VOID ReplyRexxCommand(Class *cl, struct RexxMsg *rxm, LONG prim, LONG sec, UBYTE *res )
{
        UBYTE           buf[ 16 ];

        /*
        **      Result wanted?
        **/
        if ( rxm->rm_Action & RXFF_RESULT ) {
                /*
                **      Primary result?
                **/
                if ( ! prim ) {
                        /*
                        **      We setup the secondary result to
                        **      the result string when one was passed.
                        **/
                        sec = res ? ( LONG )CreateArgstring( res, strlen( res )) : 0L;
                } else {
                        /*
                        **      Primary result bigger than 0?
                        **/
                        if ( prim > 0 ) {
                                /*
                                **      Setup the result field
                                **      to point to a string containing
                                **      the secondary result number.
                                **/
                                sprintf( buf, "%ld", sec );
                                res = buf;
                        } else {
                                /*
                                **      Negate primary result and
                                **      setup the result field to
                                **      the secondary result.
                                **/
                                prim = -prim;
                                res = ( UBYTE * )sec;
                        }

                        /*
                        **      Setup ARexx it's "RC2" variable to
                        **      the result.
                        **/
                        SetRexxVar( rxm, "RC2", res, strlen( res ));

                        /*
                        **      Clear secondary result.
                        **/
                        sec = 0;
                }
        } else if ( prim < 0 )
                /*
                **      Negate primary result.
                **/
                prim = -prim;

        /*
        **      Setup result fields.
        **/
        rxm->rm_Result1 = prim;
        rxm->rm_Result2 = sec;

        /*
        **      Reply the RexxMsg.
        **/
        ReplyMsg(( struct Message * )rxm );
}

/*
**      Free a RexxMsg command.
**/
STATIC VOID FreeRexxCommand(Class *cl, struct RexxMsg *rxm )
{
        /*
        **      Delete the result Argstring.
        **/
        if ( ! rxm->rm_Result1 && rxm->rm_Result2 )
                DeleteArgstring(( UBYTE * )rxm->rm_Result2 );

        /*
        **      Close input handle.
        **/
        if ( rxm->rm_Stdin && rxm->rm_Stdin != Input()) {
                /*
                **      If the output handle is the
                **      same as the input handle we
                **      can safely clear it.
                **/
                if ( rxm->rm_Stdout == rxm->rm_Stdin )
                        rxm->rm_Stdout = NULL;
                Close( rxm->rm_Stdin );
                rxm->rm_Stdin = NULL;
        }

        /*
        **      Close output handle if it is not
        **      the same as Stdin.
        **/
        if ( rxm->rm_Stdout && rxm->rm_Stdout != Output()) {
                Close( rxm->rm_Stdout );
                rxm->rm_Stdout = NULL;
        }

        /*
        **      Delete the command Argstring.
        **/
        DeleteArgstring(( UBYTE * )ARG0( rxm ));
        /*
        **      Delete the message itself.
        **/
        DeleteRexxMsg( rxm );
}

/*
**      Create a RexxMsg command.
**/
STATIC struct RexxMsg *CreateRexxCommand(Class *cl, AD *ad, UBYTE *comname, BPTR handle )
{
        struct RexxMsg                  *rxm;

        /*
        **      Create the RexxMsg.
        **/
        if ( rxm = CreateRexxMsg( ad->ad_Port, ad->ad_FileExt, ad->ad_HostName )) {
                /*
                **      Create the Argstring.
                **/
                if ( rxm->rm_Args[ 0 ] = CreateArgstring( comname, strlen( comname ))) {
                        /*
                        **      Setup action flags.
                        **/
                        rxm->rm_Action = RXCOMM | RXFF_RESULT;
                        /*
                        **      Setup file handles.
                        **/
                        rxm->rm_Stdin  = rxm->rm_Stdout = handle;
                        return( rxm );
                }
                /*
                **      Argstring creation failed.
                **/
                DeleteRexxMsg( rxm );
        }
        return( NULL );
}

/*
**      Send a RexxMsg command to the
**      ARexx server.
**/
STATIC struct RexxMsg *CommandToRexx( AD *ad, struct RexxMsg *rxm )
{
        struct MsgPort          *rxp;

        /*
        **      Try to find the "REXX"
        **      message port.
        **/
        Forbid();

        if ( ! ( rxp = FindPort( RXSDIR ))) {
                /*
                **      Oops. ARexx server
                **      not active.
                **/
                Permit();
                return( NULL );
        }

        /*
        **      Send off the message.
        **/
        PutMsg( rxp, &rxm->rm_Node );

        Permit();

        /*
        **      Increase pending counter.
        **/
        ad->ad_PendCnt++;

        return( rxm );
}

/*
**      Send a command to the ARexx server.
**/
STATIC struct RexxMsg *SendRexxCommand(Class *cl, AD *ad, UBYTE *comname, BPTR handle)
{
   struct RexxMsg     *rxm;

   /*
   **      Create a RexxMsg command and
   **      send it off to the ARexx server.
   **/
   if (rxm = CreateRexxCommand(cl, ad, comname, handle))
      return CommandToRexx(ad, rxm);

   return NULL;
}

STATIC REXXCOMMAND *FindRXCommand( AD *ad, UBYTE *comname, UWORD len )
{
        REXXCOMMAND             *rc = ad->ad_Commands;

        while ( rc->rc_Func ) {
                if ( ! strnicmp( comname, rc->rc_Name, len ) && isspace( comname[ strlen( rc->rc_Name ) ] ))
                        return( rc );
                rc++;
        }

        return( NULL );
}

/*
**      Execute a command.
**/
STATIC VOID DoRXCommand(Class *cl, AD *ad, struct RexxMsg *rxm )
{
   struct RexxMsg                  *rm;
   REXXCOMMAND                     *rco;
   REXXARGS                        *ra;
   UBYTE                           *comname, *args, *tmp, *result = NULL;
   LONG                             rc = 20, rc2 = 0;
   UWORD                            numargs = 0, len = 0;

   /*
   **      Allocate memory for the command
   **      name and it's argument string.
   **/
   if ( ! ( comname = ( UBYTE * )BGUI_AllocPoolMem( strlen(( UBYTE * )ARG0( rxm )) + 2 ))) {
           rc2 = ERROR_NO_FREE_STORE;
           return;
   }

   /*
   **      Copy command name and argument string.
   **/
   strcpy( comname, ( UBYTE * )ARG0( rxm ));

   /*
   **      ReadArgs() requires the argument
   **      string to end with a newline.
   **/
   strcat( comname, "\n" );

   /*
   **      Find the length of the command
   **      the start of the arguments.
   **/
   args = comname;
   while ( isspace( *args )) args++;
   tmp  = args;
   while ( ! isspace( *args )) { len++; args++; }

   /*
   **      Find the command.
   **/
   if ( rco = FindRXCommand( ad, tmp, len )) {
      /*
      **      Allocate REXXARGS structure.
      **/
      if ( ra = ( REXXARGS * )BGUI_AllocPoolMem( sizeof( REXXARGS ))) {
         /*
         **      Count number of expected args.
         **/
         if ( rco->rc_ArgTemplate ) {
            tmp = rco->rc_ArgTemplate;
            while ( *tmp != '\n' ) {
                    if ( *tmp++ == ',' ) numargs++;
            }
            numargs++;
            /*
            **      Allocate arg array.
            **/
            if ( ra->ra_ArgList = ( ULONG * )BGUI_AllocPoolMem( numargs * sizeof( ULONG ))) {
               /*
               **      Setup RDArgs.
               **/
               ad->ad_DOSParser->RDA_Source.CS_Buffer = args;
               ad->ad_DOSParser->RDA_Source.CS_Length = strlen( args );
               ad->ad_DOSParser->RDA_Source.CS_CurChr = 0;
               ad->ad_DOSParser->RDA_DAList           = NULL;
               ad->ad_DOSParser->RDA_Buffer           = NULL;
               /*
               **      Parse args.
               **/
               if ( ReadArgs( rco->rc_ArgTemplate, (LONG *)ra->ra_ArgList, ad->ad_DOSParser )) {
                  /*
                  **      Call the REXX routine.
                  **/
                  ( rco->rc_Func )( ra, rxm );

                  rc     = ra->ra_RC;
                  rc2    = ra->ra_RC2;
                  result = ra->ra_Result;

                  FreeArgs( ad->ad_DOSParser );
               } else {
                  rc = 10;
                  rc2 = IoErr();
               }
               BGUI_FreePoolMem( ra->ra_ArgList );
            } else
                    rc2 = ERROR_NO_FREE_STORE;
         } else {
            /*
            **      No args.
            **/
            ( rco->rc_Func )( ra, rxm );

            rc     = ra->ra_RC;
            rc2    = ra->ra_RC2;
            result = ra->ra_Result;
         }
         BGUI_FreePoolMem( ra );
      } else
              rc2 = ERROR_NO_FREE_STORE;
   } else {
      /*
      **      Not found in our list?
      **      Maybe it's a script.
      **/
      if ( rm = CreateRexxCommand(cl, ad, ( UBYTE * )ARG0( rxm ), NULL )) {
              /*
              **      Save original message.
              **/
              rm->rm_Args[ 15 ] = ( STRPTR )rxm;
              /*
              **      Let the REXX server see what
              **      it can do with this.
              **/
              if ( ! CommandToRexx( ad, rm ))
                      rc2 = ERROR_NOT_IMPLEMENTED;
      } else
              rc2 = ERROR_NO_FREE_STORE;

      goto byeBye;
   }

   ReplyRexxCommand(cl, rxm, rc, rc2, result );

   byeBye:

   BGUI_FreePoolMem( comname );
}

/// OM_NEW
METHOD(ARexxClassNew, struct opSet *,ops)
{
   struct TagItem     *attr = ops->ops_AttrList;
   struct MsgPort     *mp;
   AD                 *ad;
   ULONG               rc, *ecode, err, ext = 1;
   UBYTE               unique_name[80], *tmp;

   /*
    * Let the superclass set us up...
    */
   if (rc = DoSuperMethodA(cl, obj, (Msg)ops))
   {
      /*
       * Get the instance data.
       */
      ad = INST_DATA(cl, rc);

#ifdef POOLS
      /*
       * Create a memory pool.
       */
      if (ad->ad_MemPool = LibCreatePool(MEMF_PUBLIC | MEMF_CLEAR, 4096, 4096))
      {
#endif
      /*
       * Create port if a
       * host name is supplied.
       */
      if (ad->ad_HostName = (UBYTE *)GetTagData(AC_HostName, NULL, attr))
      {
         /*
          * Name valid?
          */
         if (*ad->ad_HostName)
         {
            /*
             * Make the name unique.
             */
            Forbid();
            do {
               sprintf(unique_name, "%s.%ld", ad->ad_HostName, ext++);
            }  while ((mp = FindPort(unique_name)) && ext <= 99);
            Permit();

            /*
             * Name unique?
             */
            if (!mp)
            {
               /*
                * Copy the name.
                */
               if (ad->ad_HostName = DupStr(ad, unique_name))
               {
                  /*
                   * Uppercase it.
                   */
                  tmp = ad->ad_HostName;
                  do {
                     *tmp = toupper(*tmp);
                  }  while (*tmp++);

                  /*
                   * Create the port.
                   */
                  if (ad->ad_Port = CreateMsgPort())
                  {
                     /*
                      *      Initialize and add the port.
                      */
                     ad->ad_Port->mp_Node.ln_Name = ad->ad_HostName;
                     ad->ad_Port->mp_Node.ln_Pri  = 0;
                     AddPort(ad->ad_Port);
                  } else err = RXERR_OUT_OF_MEMORY;
               } else err = RXERR_OUT_OF_MEMORY;
            } else err = RXERR_PORT_ALREADY_EXISTS;
         } else err = RXERR_NO_PORT_NAME;
      } else err = RXERR_NO_PORT_NAME;

      /*
       * Do we have a port now?
       */
      if (ad->ad_Port)
      {
         /*
          * Find the commandlist.
          */
         if (ad->ad_Commands = (REXXCOMMAND *)GetTagData(AC_CommandList, NULL, attr))
         {
            /*
             * Setup the AmigaDOS parser.
             */
            if (ad->ad_DOSParser = (struct RDArgs *)AllocDosObject(DOS_RDARGS, NULL))
            {
               ad->ad_DOSParser->RDA_Flags = RDAF_NOPROMPT;
               /*
                * Obtain file extention.
                */
               ad->ad_FileExt = (UBYTE *)GetTagData(AC_FileExtention, (ULONG)"rexx", attr);

               return rc;

            } else err = RXERR_OUT_OF_MEMORY;
         } else err = RXERR_NO_COMMAND_LIST;
      }
#ifdef POOLS
      } else err = RXERR_OUT_OF_MEMORY;
#endif
      /*
       * See if we got error storage.
       */
      ecode = (ULONG *)GetTagData(AC_ErrorCode, NULL, attr);
      if (ecode) *ecode = err;

      /*
       *      Bliep error...bliep....
       */
      CoerceMethod(cl, (Object *)rc, OM_DISPOSE);
   }
   return 0;
}
METHOD_END
///
/// OM_DISPOSE
METHOD(ARexxClassDispose, Msg, msg)
{
   AD                      *ad = ( AD * )INST_DATA( cl, obj );
   struct RexxMsg          *rxm;

   /*
    * Do we have a port?
    */
   if (ad->ad_Port)
   {
      /*
       * Remove the port from the public list
       */
      RemPort(ad->ad_Port);
      /*
       * Wait for and handle all messages still pending.
       */
      while (ad->ad_PendCnt)
      {
         /*
          * Wait for a message.
          */
         WaitPort(ad->ad_Port);
         /*
          * Get messages.
          */
         while (rxm = (struct RexxMsg *)GetMsg(ad->ad_Port))
         {
            /*
             * Replied message?
             */
            if (rxm->rm_Node.mn_Node.ln_Type == NT_REPLYMSG)
            {
               /*
                * Free the message and decrease the
                * pending counter.
                */
               FreeRexxCommand(cl, rxm);
               ad->ad_PendCnt--;
            }
            else
            {
               /*
                * Tell'm we're getting out of here.
                */
               ReplyRexxCommand(cl, rxm, -20, (LONG)"Host object closing down", NULL);
            };
         };
      };
      /*
       * In case there are no messages pending we still
       * need to reply all that is waiting at the port.
       */
      while (rxm = (struct RexxMsg *)GetMsg(ad->ad_Port))
         ReplyRexxCommand(cl, rxm, -20, (LONG)"Host object closing down", NULL);

      /*
       *      Delete the port.
       */
      DeleteMsgPort(ad->ad_Port);
   }

   /*
    *      Delete the AmigaDOS parser.
    */
   if (ad->ad_DOSParser)
      FreeDosObject(DOS_RDARGS, ad->ad_DOSParser);

   /*
    *      Delete the port name.
    */
   if (ad->ad_HostName)
      BGUI_FreePoolMem(ad->ad_HostName);

#ifdef POOLS
   /*
    * Free the pool.
    */
   LibDeletePool(ad->ad_MemPool);
#endif

   /*
    * Let the superclass do the rest.
    */
   return DoSuperMethodA(cl, obj, msg);
}
METHOD_END
///
/// OM_GET
METHOD(ARexxClassGet, struct opGet *, opg)
{
   AD         *ad = INST_DATA(cl, obj);
   ULONG       rc = 1;

   /*
   **      What do they want?
   **/
   switch (opg->opg_AttrID)
   {
   case AREXX_HostName:
      *(opg->opg_Storage) = (ULONG)ad->ad_HostName;
      break;

   case AREXX_SignalMask:
      *(opg->opg_Storage) = (1 << ad->ad_Port->mp_SigBit);
      break;

   default:
      rc = DoSuperMethodA(cl, obj, (Msg)opg);
      break;
   }
   return rc;
}
METHOD_END
///
/// AREXXM_EXECUTE
METHOD(ARexxClassExecute, struct acmExecute *, acme)
{
        AD                  *ad = INST_DATA( cl, obj );
        REXXCOMMAND         *rco;
        REXXARGS            *ra;
        struct RexxMsg      *rxm;
        UBYTE               *args, *tmp, *result = NULL, *com;
        UWORD                numargs = 0, len = 0;
        ULONG                r = 20, r2;

        /*
        **      Allocate a private copy of the command string.
        **/
        if ( com = ( UBYTE * )BGUI_AllocPoolMem( strlen( acme->acme_CommandString ) + 2 )) {
                /*
                **      Make a copy terminated with a newline.
                **/
                strcpy( com, acme->acme_CommandString );
                strcat( com, "\n" );
                /*
                **      Find the length of the command
                **      and the start of the arguments.
                **/
                args = com;
                while ( isspace( *args ))   args++;
                tmp = args;
                while ( ! isspace( *args )) { len++; args++; }
                /*
                **      Look up the command.
                **/
                if ( rco = FindRXCommand( ad, tmp, len )) {
                        /*
                        **      Allocate a REXXARGS structure.
                        **/
                        if ( ra = ( REXXARGS * )BGUI_AllocPoolMem( sizeof( REXXARGS ))) {
                                /*
                                **      Args expected?
                                **/
                                if ( rco->rc_ArgTemplate ) {
                                        /*
                                        **      Count the expected number of arguments.
                                        **/
                                        tmp = rco->rc_ArgTemplate;
                                        while ( *tmp != '\n' ) {
                                                if ( *tmp++ != ',' ) numargs++;
                                        }
                                        numargs++;
                                        /*
                                        **      Allocate space to parse the args.
                                        **/
                                        if ( ra->ra_ArgList = ( ULONG * )BGUI_AllocPoolMem( numargs * sizeof( ULONG ))) {
                                                /*
                                                **      Setup the parser.
                                                **/
                                                ad->ad_DOSParser->RDA_Source.CS_Buffer = args;
                                                ad->ad_DOSParser->RDA_Source.CS_Length = strlen( args );
                                                ad->ad_DOSParser->RDA_Source.CS_CurChr = 0;
                                                ad->ad_DOSParser->RDA_DAList           = NULL;
                                                ad->ad_DOSParser->RDA_Buffer           = NULL;
                                                /*
                                                **      Parse the args.
                                                **/
                                                if ( ReadArgs( rco->rc_ArgTemplate, (LONG *)ra->ra_ArgList, ad->ad_DOSParser )) {
                                                        /*
                                                        **      Run command.
                                                        **/
                                                        ( rco->rc_Func )( ra, NULL );
                                                        /*
                                                        **      Store results.
                                                        **/
                                                        r      = ra->ra_RC;
                                                        r2     = ra->ra_RC2;
                                                        result = ra->ra_Result;
                                                        FreeArgs( ad->ad_DOSParser );
                                                } else {
                                                        r  = 10;
                                                        r2 = IoErr();
                                                }
                                                /*
                                                **      Deallocate arg list.
                                                **/
                                                BGUI_FreePoolMem( ra->ra_ArgList );
                                        } else
                                                r2 = ERROR_NO_FREE_STORE;
                                } else {
                                        /*
                                        **      Run command.
                                        **/
                                        ( rco->rc_Func )( ra, NULL );
                                        /*
                                        **      Store results.
                                        **/
                                        r      = ra->ra_RC;
                                        r2     = ra->ra_RC2;
                                        result = ra->ra_Result;
                                }
                                /*
                                **      Deallocate the REXXARGS structure.
                                **/
                                BGUI_FreePoolMem( ra );
                        } else
                                r2 = ERROR_NO_FREE_STORE;
                        /*
                        **      When not passed to the rexx server
                        **      we must close the given IO handle
                        **      ourselves.
                        **/
                        if ( acme->acme_IO ) Close( acme->acme_IO );
                } else {
                        /*
                        **      We do not send the new-line to
                        **      the ARexx server.
                        **/
                        com[ strlen( com ) - 1 ] = '\0';
                        /*
                        **      Unknown commands are shipped
                        **      off to the REXX server.
                        **/
                        if ( rxm = CreateRexxCommand(cl, ad, com, acme->acme_IO )) {
                                if ( ! CommandToRexx( ad, rxm ))
                                        r2 = ERROR_NOT_IMPLEMENTED;
                                else
                                        r = r2 = 0L;
                        } else
                                r2 = ERROR_NO_FREE_STORE;
                }
                /*
                **      Deallocate the command copy.
                **/
                BGUI_FreePoolMem( com );
        } else
                r2 = ERROR_NO_FREE_STORE;

        /*
        **      Put the results into their
        **      storage spaces.
        **/
        if ( acme->acme_RC     ) *( acme->acme_RC     ) = r;
        if ( acme->acme_RC2    ) *( acme->acme_RC2    ) = r2;
        if ( acme->acme_Result ) *( acme->acme_Result ) = result;

        return( 1L );
}
METHOD_END
///
/// AREXXM_HANDLE_EVENT
METHOD(ARexxClassHandleEvent, Msg, msg)
{
   struct RexxMsg          *rxm, *org;
   AD                      *ad = INST_DATA(cl, obj);
   ULONG                    rc = 1;

   /*
    * Get the messages from the port.
    */
   while (rxm = (struct RexxMsg *)GetMsg(ad->ad_Port))
   {
      /*
       *      A Rexx command?
       */
      if ((rxm->rm_Action & RXCODEMASK) != RXCOMM)
         ReplyMsg((struct Message *)rxm);
      else if (rxm->rm_Node.mn_Node.ln_Type == NT_REPLYMSG)
      {
         /*
          *      Reply'd message. See if it was started
          *      as a script.
          */
         if (org = (struct RexxMsg *)rxm->rm_Args[15])
         {
            if (rxm->rm_Result1)
               ReplyRexxCommand(cl, org, 20, ERROR_NOT_IMPLEMENTED, NULL);
            else
               ReplyRexxCommand(cl, org, 0, 0, (UBYTE *)rxm->rm_Result2);
         };
         /*
          * Free the message and decrease the pending counter.
          */
         FreeRexxCommand(cl, rxm);
         ad->ad_PendCnt--;
      }
      else if (ARG0(rxm))
         /*
          * Execute message.
          */
         DoRXCommand(cl, ad, rxm);
      else
         ReplyMsg((struct Message *)rxm);
   }
   return rc;
}
METHOD_END
///

/*
 * Class function table.
 */
STATIC DPFUNC ClassFunc[] = {
   OM_NEW,               (FUNCPTR)ARexxClassNew,
   OM_DISPOSE,           (FUNCPTR)ARexxClassDispose,
   OM_GET,               (FUNCPTR)ARexxClassGet,
   AREXXM_HANDLE_EVENT,  (FUNCPTR)ARexxClassHandleEvent,
   AREXXM_EXECUTE,       (FUNCPTR)ARexxClassExecute,
   DF_END
};

METHOD(ARexxClassMethodNew, Msg, msg)
{
   return((ULONG)((((CD *)cl->cl_UserData)->RexxSysLibraryBase=OpenLibrary("rexxsyslib.library",36L))!=NULL));
}
METHOD_END

METHOD(ARexxClassMethodDispose, Msg, msg)
{
   CD *class_data=(CD *)cl->cl_UserData;

   if(class_data->RexxSysLibraryBase)
   {
      CloseLibrary(class_data->RexxSysLibraryBase);
      class_data->RexxSysLibraryBase=NULL;
   }
   return 1;
}
METHOD_END

/*
 * Class methods function table.
 */
STATIC DPFUNC ClassMethods[] = {
   OM_NEW,               (FUNCPTR)ARexxClassMethodNew,
   OM_DISPOSE,           (FUNCPTR)ARexxClassMethodDispose,
   DF_END
};

/*
 * Initialize the class.
 */
makeproto Class *InitArexxClass(void)
{
   return(BGUI_MakeClass(CLASS_DFTable,         ClassFunc,
                         CLASS_ObjectSize,      sizeof(AD),
                         CLASS_ClassSize,       sizeof(CD),
                         CLASS_ClassDFTable,    ClassMethods,
                         TAG_DONE));
}
