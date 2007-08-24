OPT PREPROCESS

MODULE 'feelin','libraries/feelin'

PROC main()
   DEF app,win
   DEF map
   
   map := ['$decorator-scheme-active',    NIL,
           '$decorator-scheme-inactive',  NIL,
           '$button-scheme',              NIL,
           NIL]

   IF feelinbase := OpenLibrary('feelin.library',FV_FEELIN_VERSION)
      app := AppObject,
         FA_Application_Title,            'DEMO',
         FA_Application_Version,          '$VER: Cold 1.00 (23-09-02)',
         FA_Application_Copyright,        '©2000-2004, Olivier LAVIALE',
         FA_Application_Author,           'Olivier LAVIALE - (gofromiel@gofromiel.com)',
         FA_Application_Description,      'Custom colors demonstration',
         FA_Application_Base,             'COLD',
         FA_Application_ResolveMapping,   map,


         FA_ColorScheme,              '<scheme shine="#BED2F0" fill="#82B4F0" dark="#003CAA" text="#FFFFFF" highlight="#2878C8" />',

         Child, win := WindowObject,
            FA_Window_Title,  'Feelin : Buttons',
            FA_Window_Open,   TRUE,
            FA_Back,          '<image type="picture" src="Feelin:Images/StripBlue.iff" mode="repeat" />',

            Child, VGroup,
               Child, VGroup, GroupFrame, FA_Frame_Title, 'Different weights', ->FA_Back,'c:FF00FF',
                  Child, HGroup,
                     Child, SimpleButton('_Haplo'),
                     Child, SimpleButton('_is'),
                     Child, SimpleButton('a L_ord'),
                  End,

                  Child, BarTitle('Youpi'),

                  Child, HGroup, FA_Group_RelSizing,TRUE,
                     Child, SimpleButton('Haplo'),
                     Child, SimpleButton('is'),
                     Child, SimpleButton('a Lord'),
                  End,

                  Child, BarObject, End,

                  Child, HGroup,
                     Child, SimpleButton('Haploooooooooooo'),
                     Child, SimpleButton('is'),
                     Child, SimpleButton('a Lord'),
                  End,
               End,

               Child, BarTitle('Inputs'),
               Child, BarTitle('Very long line name, that is very long indeed'),

               Child, HGroup, GroupFrame,
                  FA_Frame_Title,'Inputs (very long, long, long, so long frame title that will surely be cut',

                  Child, F_MakeObjA(FV_MakeObj_Button,['_Immediate',FA_InputMode,FV_InputMode_Immediate,TAG_DONE]),
                  Child, F_MakeObjA(FV_MakeObj_Button,['_Toggle',FA_InputMode,FV_InputMode_Toggle,TAG_DONE]),
                  Child, F_MakeObjA(FV_MakeObj_Button,['_Release',FA_InputMode,FV_InputMode_Release,TAG_DONE]),
               End,
            End,
         End,
     End

     IF app
         F_DoA(win,FM_Notify,[FA_Window_CloseRequest,FTRUE, app,FM_Application_Shutdown,0])

         F_DoA(app,FM_Application_Run,NIL)

         F_DisposeObj(app)
     ENDIF

     CloseLibrary(feelinbase)
   ELSE
      WriteF('Unable to open feelin.library\n')
   ENDIF
ENDPROC

