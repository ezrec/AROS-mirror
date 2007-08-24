OPT PREPROCESS

MODULE 'feelin','libraries/feelin'

DEF feelinbase:PTR TO feelinbase

PROC main()
   DEF app,win

   IF feelinbase := OpenLibrary('feelin.library',FV_FEELIN_VERSION)
     app := AppObject,
       Child, win := WindowObject,
         FA_Window_Title,  'Test',
         FA_Window_Open,   TRUE,

         Child, VGroup,
            Child, CycleObject,
             'FA_Cycle_Entries','(0)\nHaplo|(1)\nLa|(2)\nMain|(3)\nLove',
              End,

            Child, CycleObject,
                'FA_Cycle_Entries',['Pour ne pas être esclave','marthyres du temps','enivrez-vous sans cesse','de vin, de poésie ou de vertue','à votre guise','Il faut-être toujours ivre','pour ne pas sentir','le temps qui brise','nos épaules','et nous penche','vers la terre',NIL ],
                'FA_Cycle_EntriesType', 'Array',
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

