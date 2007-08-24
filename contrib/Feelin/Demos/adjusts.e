OPT PREPROCESS

MODULE 'feelin','libraries/feelin'

DEF feelinbase:PTR TO feelinbase

PROC main()
   DEF app,win

   IF feelinbase := OpenLibrary('feelin.library',FV_FEELIN_VERSION)
     app := AppObject,
       Child, win := WindowObject,
         FA_Window_Title,  'Feelin : Adjust',
         FA_Window_Open,   TRUE,
         FA_Width, '60%',

         Child, Page,
            Child, F_NewObjA('AdjustColor',NIL),
            Child, F_NewObjA('AdjustImage',NIL),
            Child, F_NewObjA('AdjustFrame',NIL),
            Child, F_NewObjA('AdjustScheme',NIL),
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

