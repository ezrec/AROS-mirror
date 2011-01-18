/*
 * @(#) $Header$
 *
 * MultiColumn.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1995 Jaba Development.
 * (C) Copyright 1995 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.2  2004/06/17 07:38:47  chodorowski
 * Added missing REGFUNC_END.
 *
 * Revision 42.1  2000/05/15 19:29:50  stegerg
 * replacements for REG macro.
 *
 * Revision 42.0  2000/05/09 22:19:56  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:33:46  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:59:02  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.3  1999/02/19 05:03:56  mlemos
 * Added support to build with Storm C.
 *
 * Revision 1.1.2.2  1998/04/27 00:13:01  mlemos
 * Ensured that the stack is never less than 8000 bytes.
 *
 * Revision 1.1.2.1  1998/02/28 17:45:30  mlemos
 * Ian sources
 *
 *
 */

/*
dcc MultiColumn.c -proto -mi -ms -mRR -lbgui
quit
*/

long __stack=8000;

#include "DemoCode.h"

#include <dos/exall.h>
#include <utility/hooks.h>

#include <clib/macros.h>

#include <string.h>
#include <stdio.h>

/*
**      This is the data were going to add
**      to the listview object. It is a simple
**      structure in which the file-information
**      is stored. This data is created in the
**      LISTV_Resource hook from a pointer
**      to a ExAllData structure.
**/
typedef struct {
        UBYTE                   fi_FileName[ 108 ];
        UBYTE                   fi_Size[ 11 ];
        UBYTE                   fi_Date[ 32 ];
        BOOL                    fi_IsDir;
} FILEINFO;

/*
**      The LISTV_Resource hook is used to create
**      the FILEINFO structures from a struct ExAllData
**      at create time. At delete time the FILEINFO
**      structure is simply deallocated.
**/
//SAVEDS ASM APTR ResourceHookFunc( REG(a0) struct Hook *hook, REG(a2) Object *obj, REG(a1) struct lvResource *lvr )
SAVEDS ASM REGFUNC3(APTR, ResourceHookFunc,
	REGPARAM(A0, struct Hook *, hook),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct lvResource *, lvr))
{
        struct ExAllData        *ead;
        FILEINFO                *fi;
        struct DateTime          dt;
        APTR                     return_code = NULL;

        /*
        **      What must we do?
        **/
        switch ( lvr->lvr_Command ) {

                case    LVRC_MAKE:
                        /*
                        **      Create a FILEINFO structure.
                        **      BGUI has passed us a pointer to a
                        **      ExAllData structure. Here we
                        **      convert it to a FILEINFO structure
                        **      which, eventually, get's added to
                        **      the listview.
                        **/
                        if ( fi = ( FILEINFO * )AllocVec( sizeof( FILEINFO ), MEMF_PUBLIC )) {
                                /*
                                **      Pick up the ExAllData.
                                **/
                                ead = ( struct ExAllData * )lvr->lvr_Entry;
                                /*
                                **      Copy the name.
                                **/
                                strcpy( &fi->fi_FileName[ 0 ], ead->ed_Name );
                                /*
                                **      Format the size text. We can do all sorts of
                                **      fancy stuff here like using the locale.library
                                **      formatting stuff but hey, it's just a demo ;)
                                **/
                                if ( ead->ed_Type < 0 ) {
                                        fi->fi_IsDir = FALSE;
                                        sprintf( &fi->fi_Size[ 0 ], "%ld", ead->ed_Size );
                                } else {
                                        fi->fi_IsDir = TRUE;
                                        strcpy( &fi->fi_Size[ 0 ], "(dir)" );
                                }
                                /*
                                **      Convert the date to a string.
                                **/
                                dt.dat_Stamp    = *(( struct DateStamp * )&ead->ed_Days );
                                dt.dat_Format   = FORMAT_CDN;
                                dt.dat_Flags    = DTF_SUBST | DTF_FUTURE;
                                dt.dat_StrDay   = NULL;
                                dt.dat_StrDate  = &fi->fi_Date[ 0 ];
                                dt.dat_StrTime  = NULL;
                                /*
                                **      Format date.
                                **/
                                DateToStr( &dt );
                                /*
                                **      Return a pointer to the created
                                **      FILEINFO structure.
                                **/
                                return_code = ( APTR )fi;
                        }
                        break;

                case    LVRC_KILL:
                        /*
                        **      Simply deallocate the FILEINFO
                        **      structure which has been created with
                        **      LVRC_MAKE above.
                        **/
                        FreeVec( lvr->lvr_Entry );
                        break;
        }
        /*
        **      Pointer to FILEINFO or NULL.
        **/
        return( return_code );
}
REGFUNC_END

/*
**      The listview will have three columns.
**
**      Name            Size            Date
**
**      The following globals will contain the maximum
**      width of each of these columns.
**/
UWORD   MaxName, MaxSize, MaxDate;

/*
**      This global stores the total width of the
**      listview drawing area.
**/
UWORD   TotalWidth = 0;

/*
**      This boolean determines wether the hook must
**      re-compute the column sizes.
**/
BOOL    ReCompCols = TRUE;

/*
**      We use 16 pixels as a minimum inner-column spacing.
**/
#define INNER_SPACE                     16

/*
**      This routine re-computes the minimum column
**      sizes when necessary.
**/
VOID ReComputeColumns( struct RastPort *rp, Object *obj, UWORD list_width )
{
        FILEINFO                        *fi;
        UWORD                            tmp, total;

        /*
        **      A re-computation is necessary when:
        **
        **      1) The ReCompCols flag is TRUE.
        **      2) The with of the listview has changed.
        **/
        if ( ReCompCols || ( TotalWidth != list_width )) {
                /*
                **      Our listview also has a title entry.
                **      Here we compute the default column
                **      sizes accoording to this title.
                **/
                MaxName = TextLength( rp, "Name:", 6 ) + INNER_SPACE;
                MaxSize = TextLength( rp, "Size:", 6 ) + INNER_SPACE;
                MaxDate = TextLength( rp, "Date:", 6 );
                /*
                **      Now we loop through the entries to find
                **      out the largest width of the three columns.
                **/
                if ( fi = ( FILEINFO * )FirstEntry( obj )) {
                        /*
                        **      Loop until all are done.
                        **/
                        while ( fi ) {
                                /*
                                **      Compute width of the Name: column
                                **      for this entry.
                                **/
                                tmp = TextLength( rp, &fi->fi_FileName[ 0 ], strlen( &fi->fi_FileName[ 0 ] )) + INNER_SPACE;
                                /*
                                **      Is it bigger than the last one?
                                **      If so store it.
                                **/
                                if ( tmp > MaxName ) MaxName = tmp;
                                /*
                                **      Compute width of the Size: column
                                **      for this entry.
                                **/
                                tmp = TextLength( rp, &fi->fi_Size[ 0 ], strlen( &fi->fi_Size[ 0 ] )) + INNER_SPACE;
                                /*
                                **      Is it bigger than the last one?
                                **      If so store it.
                                **/
                                if ( tmp > MaxSize ) MaxSize = tmp;
                                /*
                                **      Compute width of the Date: column
                                **      for this entry.
                                **/
                                tmp = TextLength( rp, &fi->fi_Date[ 0 ], strlen( &fi->fi_Date[ 0 ] ));
                                /*
                                **      Is it bigger than the last one?
                                **      If so store it.
                                **/
                                if ( tmp > MaxDate ) MaxDate = tmp;
                                /*
                                **      Pick up the next entry.
                                **/
                                fi = ( FILEINFO * )NextEntry( obj, fi );
                        }
                }
                /*
                **      Compute the total columns width.
                **/
                total = MaxName + MaxSize + MaxDate;
                /*
                **      If there's room left over we
                **      distribute it between the columns so
                **      we get a nice even spacing between
                **      them.
                **/
                if ( list_width > total ) {
                        MaxName += ( list_width - total ) >> 1;
                        MaxSize += ( list_width - total ) >> 1;
                }
                /*
                **      All done. Set the re-compute flag to
                **      FALSE and store the list width.
                **/
                ReCompCols = FALSE;
                TotalWidth = list_width;
        }
}

/*
**      The default DrawInfo pens. Just in case we don't
**      get them from the system.
**/
UWORD   DefDriPens[] = {
        0, 1, 1, 2, 1, 3, 1, 0, 2, 1, 2, 1 };

/*
**      Listview rendering hook. Here's where the magic starts ;)
**/
//SAVEDS ASM UBYTE *DisplayHookFunc( REG(a0) struct Hook *hook, REG(a2) Object *obj, REG(a1) struct lvRender *lvr )
SAVEDS ASM REGFUNC3(UBYTE *, DisplayHookFunc,
	REGPARAM(A0, struct Hook *, hook),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct lvRender *, lvr))
{
        struct TextExtent               te;
        UBYTE                          *str;
        FILEINFO                       *fi = ( FILEINFO * )lvr->lvr_Entry;
        UWORD                          *pens, numc, w, l, cw, h;

        /*
        **      Pick up the DrawInfo pen array.
        **/
        pens = lvr->lvr_DrawInfo ? lvr->lvr_DrawInfo->dri_Pens : DefDriPens;
        /*
        **      Pick up the width of the list.
        **/
        w = lvr->lvr_Bounds.MaxX - lvr->lvr_Bounds.MinX + 1;
        /*
        **      Pick up the list left-edge;
        **/
        l = lvr->lvr_Bounds.MinX;
        /*
        **      Pick up the height of the entry.
        **/
        h = lvr->lvr_Bounds.MaxY - lvr->lvr_Bounds.MinY + 1;

        /*
        **      First we render the background.
        **/
        SetAPen( lvr->lvr_RPort, lvr->lvr_State == LVRS_SELECTED ? pens[ FILLPEN ] : pens[ BACKGROUNDPEN ] );
        SetDrMd( lvr->lvr_RPort, JAM1 );

        RectFill( lvr->lvr_RPort, lvr->lvr_Bounds.MinX,
                                  lvr->lvr_Bounds.MinY,
                                  lvr->lvr_Bounds.MaxX,
                                  lvr->lvr_Bounds.MaxY );

        /*
        **      When we are passed a NULL entry pointer
        **      we are presumed to render the title. If your
        **      listview does not have a title simply
        **      recompute the columns and return NULL.
        **      We have a title so here we go.
        **/
        if ( ! fi ) {
                /*
                **      Recompute the column sizes. The routine
                **      itself will decide if it's necessary.
                **/
                ReComputeColumns( lvr->lvr_RPort, obj, w );
                /*
                **      Set the pen for the title-entry.
                **/
                SetAPen( lvr->lvr_RPort, pens[ FILLPEN ] );
        } else {
                /*
                **      Set the pen for a non-title entry. Ofcourse
                **      we can (should?) differenciate between normal and
                **      selected here but I wont ;)
                **/
                if ( fi->fi_IsDir ) SetAPen( lvr->lvr_RPort, pens[ HIGHLIGHTTEXTPEN ] );
                else                SetAPen( lvr->lvr_RPort, pens[ TEXTPEN ] );
        }
        /*
        **      Obtain Name: column width. We check it against the
        **      total list width so we do not go outside the
        **      given area.
        **/
        cw = MIN( MaxName, w );

        /*
        **      Pick up the name string or, when this
        **      is a title call, the string "Name:".
        **/
        str = fi ? &fi->fi_FileName[ 0 ] : ( UBYTE * )"Name:";

        /*
        **      Compute the number of character we
        **      can render.
        **/
        numc = TextFit( lvr->lvr_RPort, str, strlen( str ), &te, NULL, 0, cw, h );

        /*
        **      If the number of characters is
        **      0 we can stop right here and now.
        **/
        if ( ! numc ) return( NULL );

        /*
        **      Move to the correct position
        **      and render the text.
        **/
        Move( lvr->lvr_RPort, l, lvr->lvr_Bounds.MinY + lvr->lvr_RPort->TxBaseline );
        Text( lvr->lvr_RPort, str, numc );

        /*
        **      Adjust the left-edge and width to
        **      get past the Name: column.
        **/
        l += cw;
        w  = MAX(( WORD )( w - cw ), 0 );

        /*
        **      Obtain Size: column width. We check it against the
        **      total list width so we do not go outside the
        **      given area.
        **/
        cw = MIN( MaxSize, w );

        /*
        **      Pick up the size string or, when this
        **      is a title call, the string "Size:".
        **/
        str = fi ? &fi->fi_Size[ 0 ] : ( UBYTE * )"Size:";

        /*
        **      Compute the number of character we
        **      can render.
        **/
        numc = TextFit( lvr->lvr_RPort, str, strlen( str ), &te, NULL, 0, cw, h );

        /*
        **      If the number of characters is
        **      0 we can stop right here and now.
        **/
        if ( ! numc ) return( NULL );

        /*
        **      Move to the correct position
        **      and render the text.
        **/
        Move( lvr->lvr_RPort, l, lvr->lvr_Bounds.MinY + lvr->lvr_RPort->TxBaseline );
        Text( lvr->lvr_RPort, str, numc );

        /*
        **      Adjust the left-edge and width to
        **      get past the Size: column.
        **/
        l += cw;
        w  = MAX(( WORD )( w - cw ), 0 );

        /*
        **      Obtain Date column width. We check it against the
        **      total list width so we do not go outside the
        **      given area.
        **/
        cw = MIN( MaxDate, w );

        /*
        **      Pick up the date string or, when this
        **      is a title call, the string "Date:".
        **/
        str = fi ? &fi->fi_Date[ 0 ] : ( UBYTE * )"Date:";

        /*
        **      Compute the number of character we
        **      can render.
        **/
        numc = TextFit( lvr->lvr_RPort, str, strlen( str ), &te, NULL, 0, cw, h );

        /*
        **      If the number of characters is
        **      0 we can stop right here and now.
        **/
        if ( ! numc ) return( NULL );

        /*
        **      Move to the correct position
        **      and render the text.
        **/
        Move( lvr->lvr_RPort, l, lvr->lvr_Bounds.MinY + lvr->lvr_RPort->TxBaseline );
        Text( lvr->lvr_RPort, str, numc );

        /*
        **      Return NULL. This is important. If we return a non-NULL
        **      value the listview class will think it is a pointer to
        **      the text to render and try to render it.
        **/
        return( NULL );
}
REGFUNC_END

/*
**      The comparrison hook. We do a simple name, dir/file
**      comparrison here.
**/
//SAVEDS ASM LONG CompareHookFunc( REG(a0) struct Hook *hook, REG(a2) Object *obj, REG(a1) struct lvCompare *lvc )
SAVEDS ASM REGFUNC3(LONG, CompareHookFunc,
	REGPARAM(A0, struct Hook *, hook),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct lvCompare *,lvc))
{
        FILEINFO                *a = ( FILEINFO * )lvc->lvc_EntryA;
        FILEINFO                *b = ( FILEINFO * )lvc->lvc_EntryB;

        /*
        **      First we do a type comparrison to get the
        **      directories at the top of the list.
        **/
        if ( a->fi_IsDir && ! b->fi_IsDir      ) return( -1L );
        else if ( ! a->fi_IsDir && b->fi_IsDir ) return( 1L );

        /*
        **      Otherwise we do a simple, case insensitive,
        **      name string comparrison.
        **/
        return( stricmp( &a->fi_FileName[ 0 ], &b->fi_FileName[ 0 ] ));
}
REGFUNC_END

/*
**      A IDCMP hook for the window which allows us
**      to control the listview from the keyboard.
**/
//SAVEDS ASM VOID ScrollHookFunc( REG(a0) struct Hook *hook, REG(a2) Object *obj, REG(a1) struct IntuiMessage *msg )
SAVEDS ASM REGFUNC3(VOID, ScrollHookFunc,
	REGPARAM(A0, struct Hook *, hook),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct IntuiMessage *, msg))
{
        struct Window                   *window;
        Object                          *lv_obj = ( Object * )hook->h_Data;

        /*
        **      Obtain window pointer.
        **/
        GetAttr( WINDOW_Window,        obj,    ( ULONG * )&window );

        /*
        **      What key is pressed?
        **/
        switch ( msg->Code ) {

                case    0x4C:
                        /*
                        **      UP              - Move entry up.
                        **      SHIFT + UP      - Move page up.
                        **      CTRL + UP       - Move to the top.
                        **/
                        if ( msg->Qualifier & ( IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT ))
                                SetGadgetAttrs(( struct Gadget * )lv_obj, window, NULL, LISTV_Select, LISTV_Select_Page_Up, TAG_END );
                        else if ( msg->Qualifier & IEQUALIFIER_CONTROL )
                                SetGadgetAttrs(( struct Gadget * )lv_obj, window, NULL, LISTV_Select, LISTV_Select_First, TAG_END );
                        else
                                SetGadgetAttrs(( struct Gadget * )lv_obj, window, NULL, LISTV_Select, LISTV_Select_Previous, TAG_END );
                        break;

                case    0x4D:
                        /*
                        **      DOWN            - Move entry down.
                        **      SHIFT + DOWN    - Move page down.
                        **      CTRL + DOWN     - Move to the end.
                        **/
                        if ( msg->Qualifier & ( IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT ))
                                SetGadgetAttrs(( struct Gadget * )lv_obj, window, NULL, LISTV_Select, LISTV_Select_Page_Down, TAG_END );
                        else if ( msg->Qualifier & IEQUALIFIER_CONTROL )
                                SetGadgetAttrs(( struct Gadget * )lv_obj, window, NULL, LISTV_Select, LISTV_Select_Last, TAG_END );
                        else
                                SetGadgetAttrs(( struct Gadget * )lv_obj, window, NULL, LISTV_Select, LISTV_Select_Next, TAG_END );

                        break;

                case    0x43:
                case    0x44:
                        /*
                        **      RETURN or ENTER - Report the listview ID to the event handler.
                        **/
                        DoMethod( obj, WM_REPORT_ID, (( struct Gadget * )lv_obj )->GadgetID, 0L );
                        break;
        }
} 
REGFUNC_END

/*
**      The hook structures as added to the
**      listview object and window object.
**
**      If your compiler complaints about the
**      HOOKFUNC typedef uncomment the below line.
**/

/* typedef ULONG (*HOOKFUNC)(); */

struct Hook ResourceHook = { NULL, NULL, (HOOKFUNC)ResourceHookFunc, NULL, NULL };
struct Hook DisplayHook  = { NULL, NULL, (HOOKFUNC)DisplayHookFunc,  NULL, NULL };
struct Hook CompareHook  = { NULL, NULL, (HOOKFUNC)CompareHookFunc,  NULL, NULL };
struct Hook ScrollHook   = { NULL, NULL, (HOOKFUNC)ScrollHookFunc,   NULL, NULL };

/*
**      Scan the directory "name".
**/
VOID ScanDirectory( UBYTE *name, Object *obj )
{
        struct ExAllControl             *eac;
        struct ExAllData                *ead, *tmp;
        struct FileInfoBlock            *fib;
        BPTR                             lock;
        BOOL                             ismore;

        /*
        **      We need to recompute the columns.
        **/
        ReCompCols = TRUE;

        /*
        **      Get a lock to the directory.
        **/
        if ( lock = Lock( name, ACCESS_READ )) {
                /*
                **      Allocate a FileInfoBlock structure.
                **/
                if ( fib = ( struct FileInfoBlock * )AllocDosObject( DOS_FIB, NULL )) {
                        /*
                        **      Examine the lock.
                        **/
                        if ( Examine( lock, fib )) {
                                /*
                                **      Is this a directory?
                                **/
                                if ( fib->fib_DirEntryType > 0 ) {
                                        /*
                                        **      Allocate ExAll() control structure.
                                        **/
                                        if ( eac = ( struct ExAllControl * )AllocDosObject( DOS_EXALLCONTROL, NULL )) {
                                                /*
                                                **      Set key to NULL.
                                                **/
                                                eac->eac_LastKey = NULL;
                                                /*
                                                **      Allocate ExAll() buffer.
                                                **/
                                                if ( ead = ( struct ExAllData * )AllocVec( 10 * sizeof( struct ExAllData ), MEMF_PUBLIC )) {
                                                        /*
                                                        **      Read directory.
                                                        **/
                                                        do {
                                                                /*
                                                                **      Fill buffer.
                                                                **/
                                                                ismore = ExAll( lock, ead, 10 * sizeof( struct ExAllData ), ED_DATE, eac );

                                                                /*
                                                                **      Errors? Done?
                                                                **/
                                                                if (( ! ismore ) && ( IoErr() != ERROR_NO_MORE_ENTRIES ))
                                                                        break;

                                                                /*
                                                                **      Entries read?
                                                                **/
                                                                if ( ! eac->eac_Entries )
                                                                        continue;

                                                                /*
                                                                **      Pick up data pointer.
                                                                **/
                                                                tmp = ead;

                                                                /*
                                                                **      Add the entries.
                                                                **/
                                                                do {
                                                                        AddEntry( NULL, obj, tmp, LVAP_SORTED );
                                                                        /*
                                                                        **      Next...
                                                                        **/
                                                                        tmp = tmp->ed_Next;
                                                                } while ( tmp );
                                                        } while ( ismore );
                                                        /*
                                                        **      Deallocate ExAll() buffer.
                                                        **/
                                                        FreeVec( ead );
                                                }
                                                /*
                                                **      Deallocate ExAll() control structure.
                                                **/
                                                FreeDosObject( DOS_EXALLCONTROL, eac );
                                        }
                                }
                        }
                        /*
                        **      Deallocate FileInfoBlock structure.
                        **/
                        FreeDosObject( DOS_FIB, fib );
                }
                /*
                **      Release lock.
                **/
                UnLock( lock );
        }
}

/*
**      And were off...
**/
#define ID_QUIT                 1
#define ID_LIST                 2
#define ID_NEWDIR               3

VOID StartDemo( void )
{
        struct RDArgs                   *ra;
        struct Window                   *win;
        Object                          *WO_DirWin, *GO_DirList, *GO_Quit, *GO_NewDir;
        FILEINFO                        *fi;
        LONG                             dname = 0L;
        ULONG                            signal, rc;
        UBYTE                            name[ 512 ], *ptr;
        BOOL                             running = TRUE;

        /*
        **      Parse command line?
        **/
        if ( ra = ReadArgs( "NAME", &dname, NULL )) {
                /*
                **      Copy the name into the buffer.
                **/
                if ( dname ) strcpy( name, ( UBYTE * )dname );
                else         name[ 0 ] = 0;
                /*
                **      Create the listview object.
                **/
                GO_DirList = ListviewObject,
                        LISTV_ResourceHook,             &ResourceHook,
                        LISTV_DisplayHook,              &DisplayHook,
                        LISTV_TitleHook,                &DisplayHook,
                        LISTV_CompareHook,              &CompareHook,
                        GA_ID,                          ID_LIST,
                EndObject;
                /*
                **      Put it in the IDCMP hook.
                **/
                ScrollHook.h_Data = ( APTR )GO_DirList;
                /*
                **      Create the window.
                **/
                WO_DirWin = WindowObject,
                        WINDOW_Title,           "MultiCol",
                        WINDOW_RMBTrap,         TRUE,
                        WINDOW_ScaleWidth,      50,
                        WINDOW_ScaleHeight,     30,
                        WINDOW_AutoAspect,      TRUE,
                        WINDOW_SmartRefresh,    TRUE,
                        WINDOW_IDCMPHookBits,   IDCMP_RAWKEY,
                        WINDOW_IDCMPHook,       &ScrollHook,
                        WINDOW_MasterGroup,
                                VGroupObject, HOffset( 4 ), VOffset( 4 ), Spacing( 4 ), GROUP_BackFill, SHINE_RASTER,
                                        StartMember,
                                                VGroupObject,
                                                        StartMember, GO_DirList, EndMember,
                                                        StartMember,
                                                                GO_NewDir = KeyString( NULL, name, 512, ID_NEWDIR ), FixMinHeight,
                                                        EndMember,
                                                EndObject,
                                        EndMember,
                                        StartMember,
                                                HGroupObject,
                                                        VarSpace( DEFAULT_WEIGHT ),
                                                        StartMember, GO_Quit = KeyButton( "_Quit", ID_QUIT ), EndMember,
                                                        VarSpace( DEFAULT_WEIGHT ),
                                                EndObject, FixMinHeight,
                                        EndMember,
                                EndObject,
                EndObject;

                /*
                **      Window created OK?
                **/
                if ( WO_DirWin ) {
                        /*
                        **      Add gadget key.
                        **/
                        if ( GadgetKey( WO_DirWin, GO_Quit, "q" )) {
                                /*
                                **      Open the window.
                                **/
                                if ( win = WindowOpen( WO_DirWin )) {
                                        /*
                                        **      Obtain signal mask.
                                        **/
                                        GetAttr( WINDOW_SigMask, WO_DirWin, &signal );
                                        /*
                                        **      Read in the directory.
                                        **/
                                        WindowBusy( WO_DirWin );
                                        ScanDirectory( name, GO_DirList );
                                        WindowReady( WO_DirWin );
                                        /*
                                        **      Refresh the list.
                                        **/
                                        RefreshList( win, GO_DirList );
                                        /*
                                        **      Poll messages...
                                        **/
                                        do {
                                                Wait( signal );
                                                while (( rc = HandleEvent( WO_DirWin )) != WMHI_NOMORE ) {
                                                        switch ( rc ) {

                                                                case    ID_QUIT:
                                                                case    WMHI_CLOSEWINDOW:
                                                                        running = FALSE;
                                                                        break;

                                                                case    ID_LIST:
                                                                        /*
                                                                        **      Get selected entry.
                                                                        **/
                                                                        if ( fi = ( FILEINFO * )FirstSelected( GO_DirList )) {
                                                                                /*
                                                                                **      Is the entry a directory?
                                                                                **/
                                                                                if ( fi->fi_IsDir ) {
                                                                                        /*
                                                                                        **      AddPart() the name to the buffer.
                                                                                        **/
                                                                                        AddPart( name, &fi->fi_FileName[ 0 ], 512 );
                                                                                        /*
                                                                                        **      Refresh the string gadget.
                                                                                        **/
                                                                                        SetGadgetAttrs(( struct Gadget * )GO_NewDir, win, NULL, STRINGA_TextVal, name, TAG_END );
                                                                                        /*
                                                                                        **      Re-read the list.
                                                                                        **/
                                                                                        refreshDir:
                                                                                        WindowBusy( WO_DirWin );
                                                                                        ClearList( win, GO_DirList );
                                                                                        ScanDirectory( name, GO_DirList );
                                                                                        RefreshList( win, GO_DirList );
                                                                                        WindowReady( WO_DirWin );
                                                                                }
                                                                        }
                                                                        break;

                                                                case    ID_NEWDIR:
                                                                        /*
                                                                        **      Copy the new name to the buffer.
                                                                        **/
                                                                        GetAttr( STRINGA_TextVal, GO_NewDir, ( ULONG * )&ptr );
                                                                        strcpy( name, ptr );
                                                                        goto refreshDir;
                                                        }
                                                }
                                        } while ( running );
                                }
                        }
                        /*
                        **      Kill the object.
                        **/
                        DisposeObject( WO_DirWin );
                }
                /*
                **      Delete the ReadArgs structure.
                **/
                FreeArgs( ra );
        }
}
