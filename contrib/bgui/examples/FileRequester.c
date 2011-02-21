/*
 * @(#) $Header$
 *
 * FileRequester.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1995 Jaba Development.
 * (C) Copyright 1995 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.0  2000/05/09 22:19:46  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:33:36  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:58:54  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.1  1998/02/28 17:45:16  mlemos
 * Ian sources
 *
 *
 */

/* Execute me to compile with DICE V3.0.
dcc FileRequester.c -proto -mi -ms -mRR -3.0 -lbgui
quit
*/

#include "DemoCode.h"

#include <libraries/asl.h>
#include <proto/utility.h>

/*
**      Bulk GetAttr(). This really belongs in amiga.lib... I think.
**/
ULONG GetAttrs( Object *obj, Tag tag1, ... )
{
	AROS_SLOWSTACKTAGS_PRE_AS(tag1, ULONG)
        const struct TagItem    *tstate = AROS_SLOWSTACKTAGS_ARG(tag1);
        struct TagItem          *tag;
        ULONG                    num = 0L;

        while (( tag = NextTagItem( &tstate )))
                num += GetAttr( tag->ti_Tag, obj, ( IPTR * )tag->ti_Data );

        retval = num;

        AROS_SLOWSTACKTAGS_POST
}

/*
**      Put up a simple requester.
**/
ULONG ReqA( UBYTE *gadgets, UBYTE *body, IPTR *args)
{
        struct bguiRequest      req = { };

        req.br_GadgetFormat     = gadgets;
        req.br_TextFormat       = body;
        req.br_Flags            = BREQF_AUTO_ASPECT;
        req.br_Underscore       = '_';

        return BGUI_RequestA( NULL, &req, args);
}
#define Req(gadgets, body, ...) \
	({ IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
	    ReqA(gadgets, body, __args); })


VOID StartDemo( void )
{
        Object          *filereq;
        ULONG            rc;
        IPTR             dr, fl, pt, pa, l, t, w, h;

        /*
        **      Present a simple requester.
        **
        **      NOTE: For some reason the size of the requester is not
        **            saved when RTPatch (from reqtools) is active. When
        **            I have some time left I'll take a look and try to
        **            find out why.
        **/
        if ( ! Req( "_Continue|_Go Away!", ISEQ_C "This demo shows you how to use the BGUI\n"
                                           "BOOPSI ASL interface. It pops a simple Load requester\n"
                                           "and let's you play with it. After this the requester\n"
                                           "type is changed into a Save requester. Ofcourse the\n"
                                           "current path, requester size and position etc. is saved\n"
                                           "while the object remains valid." ))
                return;
        /*
        **      Create a filerequester object.
        **/
        filereq = FileReqObject, ASLFR_DoPatterns, TRUE, EndObject;

        /*
        **      Object OK?
        **/
        if ( filereq ) {
                /*
                **      Pop the requester.
                **/
                if ( ! ( rc = DoRequest( filereq ))) {
                        /*
                        **      Obtain attribute values.
                        **/
                        GetAttrs( filereq, FRQ_Drawer,          &dr,
                                           FRQ_File,            &fl,
                                           FRQ_Pattern,         &pt,
                                           FRQ_Path,            &pa,
                                           FRQ_Left,            &l,
                                           FRQ_Top,             &t,
                                           FRQ_Width,           &w,
                                           FRQ_Height,          &h,
                                           TAG_END );
                        /*
                        **      Dump 'm on the console.
                        **/
                        Tell( "Drawer                : %s\n"
                              "File                  : %s\n"
                              "Pattern               : %s\n"
                              "Path                  : %s\n"
                              "Left,Top,Width,Height : %ld,%ld,%ld,%ld\n",
                              dr, fl, pt, pa, l, t, w, h );
                } else if ( rc == FRQ_CANCEL )
                        /*
                        **      Requester canceled.
                        **/
                        Tell("Canceled\n" );
                else
                        /*
                        **      Error.
                        **/
                        Tell("Error %ld\n", rc );

                /*
                **      Make it a save requester.
                **/
                if ( ! ( rc = SetAttrs( filereq, ASLFR_DoSaveMode, TRUE, TAG_END ))) {
                        /*
                        **      Pop the requester.
                        **/
                        if ( ! ( rc = DoRequest( filereq ))) {
                                /*
                                **      Obtain attribute values.
                                **/
                                GetAttrs( filereq, FRQ_Drawer,          &dr,
                                                   FRQ_File,            &fl,
                                                   FRQ_Pattern,         &pt,
                                                   FRQ_Path,            &pa,
                                                   FRQ_Left,            &l,
                                                   FRQ_Top,             &t,
                                                   FRQ_Width,           &w,
                                                   FRQ_Height,          &h,
                                                   TAG_END );
                                /*
                                **      Dump 'm to the console.
                                **/
                                Tell( "Drawer                : %s\n"
                                      "File                  : %s\n"
                                      "Pattern               : %s\n"
                                      "Path                  : %s\n"
                                      "Left,Top,Width,Height : %ld,%ld,%ld,%ld\n",
                                      dr, fl, pt, pa, l, t, w, h );
                        } else if ( rc == FRQ_CANCEL )
                                /*
                                **      Requester canceled.
                                **/
                                Tell("Canceled\n" );
                        else
                                /*
                                **      Error.
                                **/
                                Tell("Error %ld\n", rc );
                } else
                        /*
                        **      Error with SetAttrs().
                        **/
                        Tell( "Error %ld\n", rc );
                /*
                **      Dump the filerequester object.
                **/
                DisposeObject( filereq);
        } else
                Tell( "unable to create filerequester object.\n" );
}
