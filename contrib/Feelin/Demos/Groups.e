OPT PREPROCESS

MODULE 'feelin','libraries/feelin'

PROC main()
   DEF app,win

   IF feelinbase := OpenLibrary('feelin.library',FV_FEELIN_VERSION)
     app := AppObject,
       Child, win := WindowObject,
         FA_Window_Title,  'Feelin : Groups',
         FA_Window_Open,   TRUE,

         Child, VGroup,
            Child, HGroup, FA_Group_PageTitle,'Types', FA_Group_SameSize,TRUE,
              Child, HGroup, GroupFrame, FA_Frame_Title, 'Horizontal',
                Child, rectObj(),
                Child, rectObj(),
                Child, rectObj(),
              End,

              Child, VGroup, GroupFrame, FA_Frame_Title, 'Vertical',
                Child, rectObj(),
                Child, rectObj(),
                Child, rectObj(),
              End,

              Child, VGroup, GroupFrame, FA_Frame_Title, 'Array', FA_Group_Rows, 3,
                Child, rectObj(),
                Child, rectObj(),
                Child, rectObj(),
                Child, rectObj(),
                Child, rectObj(),
                Child, rectObj(),
                Child, rectObj(),
                Child, rectObj(),
                Child, rectObj(),
              End,
            End,

            Child, VGroup, FA_Group_PageTitle,'Sizes',
              Child, HGroup, GroupFrame, FA_Frame_Title, 'Different Weights',
                Child, textObj( '25 kg',[FA_Weight, 25, NIL]),
                Child, textObj( '50 kg',[FA_Weight, 50, NIL]),
                Child, textObj( '75 kg',[FA_Weight, 75, NIL]),
                Child, textObj('100 kg',[FA_Weight,100, NIL]),
              End,

              Child, HGroup, GroupFrame, FA_Frame_Title, 'Fixed & Variable Sizes', FA_SetMax,FV_SetHeight,
                Child, textObj('fixed',[FA_SetMax,FV_SetWidth, NIL]),
                Child, textObj('free'),
                Child, textObj('fixed',[FA_SetMax,FV_SetWidth, NIL]),
                Child, textObj('free'),
                Child, textObj('fixed',[FA_SetMax,FV_SetWidth, NIL]),
              End,

              Child, HGroup, GroupFrame, FA_Frame_Title, 'Relative Sizes', FA_Group_RelSizing,TRUE,
                Child, textObj('1'),
                Child, textObj('12'),
                Child, textObj('123'),
                Child, textObj('1234'),
                Child, textObj('12345'),
              End,

              Child, HGroup, GroupFrame, FA_Frame_Title, 'Same Width', FA_Group_SameWidth,TRUE,
                Child, textObj('1'),
                Child, textObj('12'),
                Child, textObj('123'),
                Child, textObj('1234'),
              End,
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
   ELSE
     WriteF('Unable to open feelin.library\n')
   ENDIF
ENDPROC

PROC textObj(name,tags=NIL)
   RETURN   TextObject,
         DontChain,
         FA_SetMax,        FV_SetHeight,
         FA_Frame,         '$text-frame',
         FA_Back,          '$text-back',
         FA_Text,          name,
         FA_Text_PreParse, '<align=center>',
         TAG_MORE,         tags,
         End
ENDPROC
PROC rectObj()
   RETURN   AreaObject,
         DontChain,
         FA_Frame,   '$text-frame',
         FA_Back,    '$text-back',
         End
ENDPROC
