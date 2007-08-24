OPT PREPROCESS

MODULE 'feelin','libraries/feelin'

DEF feelinbase:PTR TO feelinbase

PROC main()
   DEF app,win,entries

   entries := ['Olivier','Lotan','Haplo','Gofromiel',NIL]

   IF feelinbase := OpenLibrary('feelin.library',FV_FEELIN_VERSION)
      app := AppObject,
         Child, win := WindowObject,
            FA_Window_Title,  'Feelin : Radio',
            FA_Window_Open,   TRUE,

            Child, VGroup,
               Child, RadioObject, GroupFrame, FA_Frame_Title,'Horizontal',
                  FA_Horizontal, TRUE,
                 'FA_Radio_Active', 2,
                 'FA_Radio_Entries', entries,
                  End,

               Child, HGroup,
                  Child, RadioObject, GroupFrame, FA_Frame_Title,'Vertical',
                     FA_Horizontal, FALSE,
                    'FA_Radio_PreParse','<align=center>',
                    'FA_Radio_Entries',entries,
                     End,

                  Child, RadioObject, GroupFrame, FA_Frame_Title,'Array',
                     FA_Group_Columns, 2,
                    'FA_Radio_Entries',entries,
                     End,
               End,
            End,
         End,
      End

      IF app
         F_DoA(win,FM_Notify,[FA_Window_CloseRequest,FTRUE, app, FM_Application_Shutdown,0])
         F_DoA(app,FM_Application_Run,NIL)
         F_DisposeObj(app)
      ELSE
         PrintF('Unable to create application\n')
      ENDIF

      CloseLibrary(feelinbase)
   ENDIF
ENDPROC

