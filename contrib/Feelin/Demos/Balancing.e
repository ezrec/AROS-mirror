OPT PREPROCESS

MODULE 'feelin','libraries/feelin'

PROC main()
   DEF app,win,g,b,font

   font := 'Garnet/16'

   IF feelinbase := OpenLibrary('feelin.library',FV_FEELIN_VERSION)
      app := AppObject,
         FA_Application_Title,        'Balancing',
         FA_Application_Version,      '$VER: Balancing 1.00 (2002/09/23)',
         FA_Application_Copyright,    '©2000-2004, Olivier Laviale',
         FA_Application_Author,       'Olivier LAVIALE - (gofromiel@gofromiel.com)',
         FA_Application_Description,  'Show balancing groups',
         FA_Application_Base,         'BALANCING',

         Child, win := WindowObject,
            FA_ID,           "MAIN",
            FA_Window_Title, 'Feelin : Balancing',
            FA_Window_Open,   TRUE,

            Child, g := VGroup,
               Child, HGroup,
                  Child, colorbutton('#FF0000;highlight','Red'),
                  Child, colorbutton('#FFFF00;highlight','Yellow'),
                  Child, BalanceID("BAL1"),
                  Child, colorbutton('#00FF00;highlight','Green',[FA_SetMax,FV_SetBoth,NIL]),
                  Child, colorbutton('#00FFFF;highlight','Cian'),
                  Child, colorbutton('#0000FF;highlight','Blue'),
               End,

               Child, BalanceID("BAL2"),

               Child, HGroup, FA_MaxWidth,300,
                  Child, colorbutton('#FFFFFF;highlight','White',[FA_ColorScheme,'<scheme text="c:000000" />', NIL]),
                  Child, BalanceID("BAL3"),
                  Child, colorbutton('#000000;highlight','Black',[FA_ColorScheme,'<scheme text="c:FFFFFF" />', NIL]),
               End,

               Child, colorbutton('#FF00FF;highlight','Magenta'),

               Child, BarObject, End,

               Child, b := TextObject,
                  FA_InputMode,        FV_InputMode_Toggle,
                  FA_SetMax,           FV_SetBoth,
                  FA_Frame,            '<frame id="#21" padding-width="6" padding-height="3" />\n<frame id="3" padding-width="6" padding-height="3" />',
                  FA_Back,             'fill;halfdark',
                  FA_Text,             'T_oggle FA_Balance_QuickDraw',
                  FA_Text_PreParse,    '<pens shadow=halfshadow up=shine>',
                  FA_Text_AltPreParse, '<pens text=shine>',
                  FA_Text_VCenter,     TRUE,
               End,
            End,
         End,
      End

     IF app
       F_DoA(b,FM_Notify,[FA_Selected,FV_Notify_Always,g,FM_Set,4,'FA_Balance_QuickDraw',FV_Notify_Value,FA_Group_Forward,TRUE])
       F_DoA(win,FM_Notify,[FA_Window_CloseRequest,FTRUE,app,FM_Application_Shutdown,0])
       F_DoA(app,FM_Application_Run,NIL)
       F_DisposeObj(app)
     ENDIF

     CloseLibrary(feelinbase)
   ENDIF
ENDPROC

PROC colorbutton(ispec,label,tags=NIL)
   RETURN TextObject,
        FA_Back,            ispec,
        FA_InputMode,       FV_InputMode_Release,
        FA_Font,            'Garnet/16',
        FA_Text,            label,
        FA_Text_VCenter,    TRUE,
        FA_Text_PreParse,   '<align=center>',
        TAG_MORE,           tags,
        End
ENDPROC
