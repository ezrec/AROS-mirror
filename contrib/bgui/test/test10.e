/*
 *      MULTISELECT.E
 *
 *      (C) Copyright 1995 Jaba Development.
 *      (C) Copyright 1995 Jan van den Baard.
 *          All Rights Reserved.
 *
 *      On 22 May 96, a dead-end loop bug corrected by Dominique Dutoit.
 *      Updated on 11-Aug-96
 *      JT: changes marked with "!!!"
 */

OPT OSVERSION=37
OPT PREPROCESS

MODULE 'libraries/bgui',
       'libraries/bguim',
       'libraries/gadtools',
       'bgui',
       'bgui/bgui_image',
       'bgui/bgui_obsolete',
       'tools/boopsi',
       'utility/tagitem',
        'intuition/intuition',
        'intuition/screens',
       'intuition/classes',
       'intuition/classusr',
       'intuition/gadgetclass'
/*
**      Object ID's.
**/
#define ID_SHOW                 1
#define ID_QUIT                 2
#define ID_ALL                  3
#define ID_NONE                 4

ENUM ID_T1=5,ID_T2 -> !!!

PROC main()

    DEF wo_window, window:PTR TO window,go_quit, go_show, go_list, go_shift, go_all, go_none,
        signal, rc, str,
        running = TRUE

   /*
   **      Open the library.
   **/
   IF bguibase := OpenLibrary( 'bgui.library', BGUIVERSION )
        /*
         *      Create the window object.
         */
        wo_window := WindowObject,
                WINDOW_Title,           'Multi-Selection Demo',
                WINDOW_AutoAspect,      TRUE,
                WINDOW_SmartRefresh,    TRUE,
                WINDOW_RMBTrap,         TRUE,
                WINDOW_ScaleWidth,      30,
                WINDOW_ScaleHeight,     30,
                WINDOW_AutoKeyLabel,    TRUE,
                WINDOW_PubScreenName,   'Workbench', -> !!!
                WINDOW_MasterGroup,
                        VGroupObject, HOffset( 4 ), VOffset( 4 ), Spacing( 4 ), GROUP_BackFill, SHINE_RASTER,
                                StartMember,
                                        VGroupObject, HOffset( 4 ), VOffset( 4 ),
                                                FRM_Type,               FRTYPE_BUTTON,
                                                FRM_Recessed,           TRUE,
                                                StartMember,
                                                        go_list := ListviewObject,
                                                                LISTV_EntryArray,               ['\ecThis listview object has multi-',
                                                                '\ecselection turned on. You can',
                                                                '\ecmulti-select the items by holding',
                                                                '\ec down the SHIFT-key while clicking',
                                                                '\econ the different items or by clicking',
                                                                '\econ an entry and dragging the mouse',
                                                                '\ecup or down.',
                                                                '\ec',
                                                                '\ecIf you check the No SHIFT checbox',
                                                                '\ecyou can multi-select the items without',
                                                                '\ecusing the SHIFT key',NIL],
                                                                LISTV_MultiSelect,              TRUE,
                                                        EndObject,
                                                EndMember,
                                                StartMember,
                                                        HGroupObject,
                                                                StartMember, go_all  := KeyButton( '_All',  ID_ALL  ), EndMember,
                                                                StartMember, go_none := KeyButton( 'N_one', ID_NONE ), EndMember,
                                                        EndObject, FixMinHeight,
                                                EndMember,
                                        EndObject,
                                EndMember,
                                StartMember,
                                        HGroupObject, HOffset( 4 ), VOffset( 4 ),
                                                FRM_Type,               FRTYPE_BUTTON,
                                                FRM_Recessed,           TRUE,
                                                VarSpace( DEFAULT_WEIGHT ),
                                                StartMember, go_shift := KeyCheckBox( '_No SHIFT:', FALSE, 0 ), EndMember,
                                                VarSpace( DEFAULT_WEIGHT ),
                                        EndObject, FixMinHeight,
                                EndMember,
                                StartMember,VGroupObject,NormalSpacing, -> !!!
                                	StartMember,PrefButton('_1 Close&Reopen on BGUITEST',ID_T1),EndMember,
                                	StartMember,PrefButton('_2 Close, wait-return & reopen',ID_T2),EndMember,
                                EndObject,FixMinHeight,EndMember, -> !!!
                                StartMember,
                                        HGroupObject, Spacing( 4 ),
                                                StartMember, go_show := KeyButton( '_Show', ID_SHOW ), EndMember,
                                                VarSpace( DEFAULT_WEIGHT ),
                                                StartMember, go_quit := KeyButton( '_Quit', ID_QUIT ), EndMember,
                                        EndObject, FixMinHeight,
                                EndMember,
                        EndObject,
        EndObject

        /*
        **      Object created OK?
        **/
        IF wo_window
            /*
            **      Add notification.
            **/
            IF AddMap( go_shift, go_list, [ GA_SELECTED, LISTV_MultiSelectNoShift, TAG_END ]:tagitem )
                /*
                **      try to open the window.
                **/
                IF window := WindowOpen( wo_window )
                    /*
                    **      Obtain it's wait mask.
                    **/
                    GetAttr( WINDOW_SigMask, wo_window, {signal} )
                    /*
                    **      Event loop...
                    **/
                    WHILE running = TRUE
                        Wait( signal )
                        /*
                        **      Handle events.
                        **/
                        WHILE ( rc := HandleEvent( wo_window )) <> WMHI_NOMORE
                            /*
                            **      Evaluate return code.
                            **/
                            SELECT rc

                                    CASE    WMHI_CLOSEWINDOW
                                            running := FALSE
                                    CASE    ID_QUIT
                                            running := FALSE
                                    CASE    ID_ALL
                                            SetGadgetAttrsA(go_list, window, NIL, [LISTV_SelectMulti, LISTV_Select_All, TAG_END] )

                                    CASE    ID_NONE
                                            SetGadgetAttrsA(go_list, window, NIL, [LISTV_Deselect, -1, TAG_END] )

                                    CASE    ID_SHOW

                                            /*
                                            **      Simply dump all selected entries
                                            **      to the console.
                                            **/
                                            IF str := FirstSelected( go_list )
                                                    REPEAT
                                                            WriteF( '\s\n', str + 2 )
                                                                              ->^^^ A bug fixed by Peter Mysliwy
                                                            str := NextSelected( go_list, str )
                                                                                          
                                                    /* THE BUG !!!!!
                                                    ** The original line was : UNTIL str
                                                    ** Such statement cause a dead-end loop as str is filled with NIL
                                                    */
                                                    UNTIL str = NIL
                                            ELSE
                                                    /*
                                                    **      Oops. There are no selected
                                                    **      entries.
                                                    **/
                                                    WriteF( 'No selections made!\n' )
                                            ENDIF

                                    CASE ID_T1
                                            WindowClose(wo_window)
                                            SetAttrsA(wo_window,[WINDOW_PubScreenName,'BGUITEST',0])
                                            IF (window := WindowOpen(wo_window)) = NIL
                                                    running := FALSE
                                            ENDIF

                                    CASE ID_T2
                                            WindowClose(wo_window)
                                            WriteF('Press return to reopen> ')
                                            Inp(stdout)
                                            IF (window := WindowOpen(wo_window)) = NIL
                                                    running := FALSE
                                            ENDIF
                            ENDSELECT
                        ENDWHILE
                    ENDWHILE
                ELSE
                    WriteF( 'Could not open the window\n' )
                ENDIF
            ELSE
                WriteF( 'Unable to add notification\n' )
            ENDIF
            /*
            **      Disposing of the window object will
            **      also close the window if it is
            **      already opened and it will dispose of
            **      all objects attached to it.
            **/
            DisposeObject( wo_window )
        ELSE
             WriteF( 'Could not create the window object\n' )
        ENDIF
        CloseLibrary(bguibase)
   ELSE
        WriteF( 'Could not open the bgui.library\n' )
   ENDIF
ENDPROC
