/* -- --------------------------------------------------------------- -- *
 * -- Programname.........: Area.e                                    -- *
 * -- Description.........: Translation from "Area.c" written by      -- *
 * --                       Jan van den Baard.                        -- *
 * -- Author..............: Daniel Kasmeroglu (alias Deekah)          -- *
 * -- Version.............: 0.1     (07.03.1997)                      -- *
 * -- --------------------------------------------------------------- -- */

-> JT: changed lines marked with !!!

     /* -- ------------------------------------------------- -- *
      * --                  Compiler-Option's                -- *
      * -- ------------------------------------------------- -- */

OPT REG = 5              -> activate register-optimisation
OPT PREPROCESS           -> enable preprocessor


     /* -- ------------------------------------------------- -- *
      * --                      E-Module                     -- *
      * -- ------------------------------------------------- -- */

MODULE 'intuition/intuition',
       'intuition/screens',
       'intuition/classusr',
       'intuition/classes',
       'intuition/gadgetclass',
       'libraries/bguim',
       'libraries/bgui',
       'utility/tagitem',
       'graphics/gfx',
       'tools/boopsi'

MODULE 'bgui',
       'bgui/bgui_image'


     /* -- ------------------------------------------------- -- *
      * --                     Constant's                    -- *
      * -- ------------------------------------------------- -- */

ENUM GID_AREA = 1,
     GID_QUIT


     /* -- ------------------------------------------------- -- *
      * --                    Structure's                    -- *
      * -- ------------------------------------------------- -- */

OBJECT demogui
  owindow      : PTR TO object
  window       : PTR TO window
  gadgets[ 2 ] : ARRAY OF LONG
ENDOBJECT


     /* -- ------------------------------------------------- -- *
      * --                      Method's                     -- *
      * -- ------------------------------------------------- -- */

PROC dem_Constructor(fixaspect_tag) OF demogui /* !!! */

  self.gadgets[ GID_AREA - 1 ] := AreaObject,
                                    ButtonFrame,
                                    AREA_MinWidth,      40,
                                    AREA_MinHeight,     10,
                                    GA_ID,              GID_AREA,
                                  EndObject


  self.gadgets[ GID_QUIT - 1 ] := PrefButton( '_Quit', GID_QUIT )

  self.owindow := WindowObject,
                    WINDOW_Title,           'Areaclass demo.',
                    WINDOW_AutoAspect,      TRUE,
                    WINDOW_SmartRefresh,    TRUE,
                    WINDOW_AutoKeyLabel,    TRUE,
                    WINDOW_CloseOnEsc,      TRUE,
                    WINDOW_ScaleWidth,      40,
                    WINDOW_ScaleHeight,     50,
                    WINDOW_MasterGroup,
                      HGroupObject, /* !!! */
                        NormalOffset,
                        NormalSpacing,
                        ShineRaster,
                        StartMember,
                          self.gadgets[ GID_AREA - 1 ],
                          fixaspect_tag,$00010001, /* !!! */
                        EndMember,
                        StartMember,
                          self.gadgets[ GID_QUIT - 1 ],
                          FixMinHeight,
                        EndMember,
                      EndObject,
                  EndObject

ENDPROC


PROC dem_StartInterface() OF demogui
DEF sta_area : PTR TO ibox
DEF sta_rc,sta_running,sta_signal

  IF self.owindow <> NIL

    self.window := WindowOpen( self.owindow )
    IF self.window <> NIL

      GetAttr( WINDOW_SigMask, self.owindow, {sta_signal} )

      sta_running := TRUE
      WHILE sta_running = TRUE

        Wait( sta_signal )

        WHILE (sta_rc := HandleEvent( self.owindow )) <> WMHI_NOMORE

handleMsg:
          SELECT sta_rc
          CASE WMHI_CLOSEWINDOW ; sta_running := FALSE
          CASE GID_QUIT         ; sta_running := FALSE
          CASE GID_AREA

            GetAttr( AREA_AreaBox, self.gadgets[ GID_AREA - 1 ], {sta_area} )
            sta_rc := glo_RenderMandel( self.window, sta_area, self.owindow )
            IF sta_rc <> 0 THEN JUMP handleMsg

          ENDSELECT

        ENDWHILE

      ENDWHILE

      WindowClose( self.owindow )

    ENDIF

  ENDIF

ENDPROC


PROC end() OF demogui
  IF self.owindow <> NIL THEN DisposeObject( self.owindow )
ENDPROC


     /* -- ------------------------------------------------- -- *
      * --                    Procedure's                    -- *
      * -- ------------------------------------------------- -- */

-> // This routine is equivalent to the program "IntMandel.e" but
-> // all time I use this routine I'm getting shit if the area
-> // is too small. If the area is big enough (see my scale-values)
-> // I'm getting the correct example.

PROC glo_RenderMandel( ren_win : PTR TO window, ren_ibox : PTR TO ibox, ren_owindow )
DEF ren_zr,ren_zi,ren_ar,ren_ai
DEF ren_dr,ren_di,ren_sr,ren_si
DEF ren_st,ren_x,ren_y,ren_i,ren_rc
DEF ren_xsize,ren_ysize,ren_cols

  ren_cols  := Shl( 1, ren_win.wscreen.bitmap.depth ) - 1
  ren_xsize := ren_ibox.width
  ren_ysize := ren_ibox.height

  ren_sr    := $400000 / ren_xsize
  ren_si    := $300000 / ren_ysize
  ren_st    := $140000 * -2
  ren_zi    := $160000

  FOR ren_y := ren_ysize - 1 TO 0 STEP -1

    ren_zi := ren_zi - ren_si
    ren_zr := ren_st

    FOR ren_x := 0 TO ren_xsize - 1

      ren_i  := 0
      ren_ar := ren_zr
      ren_ai := ren_zi

      REPEAT

        ren_dr := Shr( ren_ar, 10 )
        ren_di := Shr( ren_ai, 10 )
        ren_ai := ren_dr * 2 * ren_di + ren_zi
        ren_dr := ren_dr * ren_dr
        ren_di := ren_di * ren_di
        ren_ar := ren_dr -ren_di + ren_zr
        ren_i++

      UNTIL (ren_i > 25) OR (ren_dr + ren_di > $400000)

      SetAPen( ren_win.rport, ren_i AND ren_cols )
      WritePixel( ren_win.rport, ren_x + ren_ibox.left, ren_y + ren_ibox.top )

      WHILE (ren_rc := HandleEvent( ren_owindow )) <> WMHI_NOMORE

        SELECT ren_rc
        CASE WMHI_CLOSEWINDOW ; RETURN ren_rc
        CASE GID_AREA         ; RETURN ren_rc
        CASE GID_QUIT         ; RETURN ren_rc
        ENDSELECT

      ENDWHILE

      ren_zr   := ren_zr + ren_sr

    ENDFOR

  ENDFOR

ENDPROC 0


     /* -- ------------------------------------------------- -- *
      * --                     Hauptprogramm                 -- *
      * -- ------------------------------------------------- -- */

PROC main() HANDLE
DEF ma_object : PTR TO demogui

  ma_object := NIL

  bguibase  := OpenLibrary( 'bgui.library', 41 )
  IF bguibase <> NIL

    NEW ma_object.dem_Constructor(IF arg[]<>"-" THEN LGO_FixAspect ELSE LGO_TAGDONE) /* !!! */
    ma_object.dem_StartInterface()

  ENDIF

EXCEPT DO

  IF ma_object <> NIL THEN END ma_object
  IF bguibase  <> NIL THEN CloseLibrary( bguibase )

ENDPROC
