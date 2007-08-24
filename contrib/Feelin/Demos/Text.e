OPT PREPROCESS

MODULE 'feelin','libraries/feelin'

PROC main()
   DEF app,win

   IF feelinbase := OpenLibrary('feelin.library',FV_FEELIN_VERSION)
     app := AppObject,
       Child, win := WindowObject,
         FA_Window_Title, 'Feelin : Text',
         FA_Window_Open,   TRUE,

         Child, HGroup,
            Child, TextObject, FA_ChainToCycle,    FALSE,
                          FA_MinWidth,        30,
                          FA_SetMin,          FV_SetHeight,
                          FA_Text,            {__Text},
                          FA_Text_VCenter,          TRUE,
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

__Text:
   CHAR '<align="center"><spacing=5>Normal text\n',
      '<pens text=8>Highlighted text</pens>\n',
      '<pens shadow="dark">',
      '<font color="#FF0000" face=Diamond size=12>C',
      '<font color="#FF9000">o</font>',
      '<font color="#D9FF00">l</font>',
      '<font color="#48FF00">o</font>',
      '<font color="#00FF48">r</font>',
      '<font color="#00FFD9">e</font>',
      '<font color="#0090FF">d</font> ',
      '<font color="#0000FF">!</font></font></pens><br>',
      '<font face="Helvetica" size=9>M</font>',
      '<font face="Helvetica" size=11>u</font>',
      '<font face="Helvetica" size=13>l</font>',
      '<font face="Helvetica" size=15>t</font>',
      '<font face="Helvetica" size=18>i</font>',
      '<font face="Diamond" size=20>p</font>',
      '<font face="Garnet" size=16>l</font>',
      '<font face="Opal" size=12>e</font><br>',
      '<image type="picture" src="Feelin:Images/FileChooser/back.png"><br>',
      '<image type="picture" src="feelin:resources/system/logo.png">',
      '<hr width=50px size=3 shadow="halfshadow" shine="halfshine">',

      '<pens style="emboss">Embossed text</pens>, <pens style="ghost">Ghost text</pens>\n',
      '<pens style="light">Some Light</pens>, <pens style="shadow">Some Shadow</pens>\n',
      '<pens style="glow">Glowing</pens>\n',
      '<pens up=1 shadow=4>Emboss & Shadow</pens><br>',

      '<i>Italic</i>, <b>Bold</b>, <u>Underlined</u>, Normal, <i><b><u>Mixed</u></b></i><hr>',
      '<align=left>A very, very, very long line that will be nicely truncated\n',
      '<align=center>A very, very, very long line that will be nicely truncated\n',
      '<align=right>A very, very, very long line that will be nicely truncated',
      NIL
