OPT PREPROCESS

MODULE 'feelin','libraries/feelin'

#define Image(x,i) ImageObject, 'InputMode',i, FA_SetMax,FV_SetBoth, 'FA_Image_Spec',x, FA_Back,FI_None, End
 
PROC main()
   DEF app,win
   DEF map
   
   map := ['$decorator-scheme-active',    NIL,
           '$decorator-scheme-inactive',  '<scheme shine="fill" fill="halfshadow" dark="shadow" />', NIL]

   IF feelinbase := OpenLibrary('feelin.library',FV_FEELIN_VERSION)
      app := AppObject,
         FA_Application_ResolveMapping,map,
         FA_ColorScheme,  '<scheme shine="#7878A0" fill="#555577" dark="#000000" />',

         Child, win := WindowObject,
            FA_Back,          '<image type="picture" src="Feelin:Images/StripPurple.iff" mode="repeat" />',
            FA_Window_Title,  'Feelin : Brushes',
            FA_Window_Open,   TRUE,

            Child, VGroup,
                Child, HGroup,
                    Child, ImageObject,
                       FA_SetMax,FV_SetBoth,
                      'FA_Image_Spec',  '<image type="brush" src="Grimy.bsh" />',
                       FA_ColorScheme,  '<scheme shine="#FFDCA0" fill="#AA7864" />',
                    End,
                    
                    Child, ImageObject,
                      'FA_Image_Spec',  '<image type="picture" src="Feelin:images/Grimy.bsh" mode="scale" filter="bilinear" />',
                    End,
                End,

               Child, HGroup,
                  Child, Image('#FF0000;#00FF00','Toggle'),
                  Child, Image('<image type="brush" src="ArrowUp.fb0" />','Toggle'),
                  Child, Image('<image type="brush" src="Dalle.bsh" />','Release'),
                  Child, F_MakeObjA(FV_MakeObj_Checkbox,[TRUE,FA_ColorScheme,'<scheme shine="#FFDCA0" fill="#AA7864" />',TAG_DONE]),
                  Child, Checkbox(TRUE),
                  Child, Image('<image type="brush" src="Be/ArrowDown.fb0" />','Release'),
               End,
            End,
         End,
      End

      IF app
         F_DoA(win,FM_Notify,[FA_Window_CloseRequest,FTRUE,app,FM_Application_Shutdown,0])
         F_DoA(app,FM_Application_Run,NIL)
         F_DisposeObj(app)
      ENDIF

      CloseLibrary(feelinbase)
   ENDIF
ENDPROC

