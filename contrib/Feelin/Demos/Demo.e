OPT PREPROCESS

MODULE 'feelin','libraries/feelin','a4',
      'utility/hooks',
      'graphics/gfxmacros','graphics/rastport'

PROC main()
   DEF app,win,
      p1,g1,myback[256]:STRING,hook:hook

   ->    grp1,grp2,grp3,b1,b2,b3, p1,p2,p3,p4, g1,g2,g3, s1, i=NIL,inc=TRUE

   sys_SGlob()

   hook.entry := {draw_hook}
   hook.data  := NIL

   StringF(myback,'<image type="hook" src="#%08lx" />',hook)

   IF feelinbase := OpenLibrary('feelin.library',FV_FEELIN_VERSION)
     app := AppObject,
       FA_Application_Base,'DEMO',

       Child, win := WindowObject,
         FA_Window_Title, 'Feelin : Demo',
         FA_Window_Open,   TRUE,

         Child, VGroup,
            Child, Page,
->PROC About
              Child, F_NewObjA('Crawler',[FA_Group_PageTitle, 'About', GaugeFrame, 'FA_Crawler_Delay',40,
                Child, TextObject,
                  FA_Font,                   'Helvetica/11',
                  FA_Back,                   myback,
                  FA_Frame_InnerLeft,                 20,
                  FA_Frame_InnerTop,                  5,
                  FA_Frame_InnerRight,                20,
                  FA_Frame_InnerBottom,               5,
                  FA_ChainToCycle,           FALSE,
                  FA_Text,                   {__CrawText},
                  End,
              End,
->ENDPROC
->PROC Objects
              Child, Page, FA_Group_PageTitle, 'Object',
->PROC Objects / Buttons
                Child, HGroup, FA_Group_PageTitle,'Simple Buttons', GroupFrame, FA_Frame_Title,'Buttons with differents Inputs',
                  Child, button('_Immediate',FV_InputMode_Immediate),
                  Child, button('_Toggle',FV_InputMode_Toggle),
                  Child, button('_Release',FV_InputMode_Release),
                End,
->ENDPROC
->PROC Objects / Gauges
                Child, HGroup, FA_Group_PageTitle, 'Gauges',
                  Child, VGroup,
                     Child, Gauge(TRUE,0,100, 25),
                     Child, Gauge(TRUE,0,100, 50),
                     Child, Gauge(TRUE,0,100, 75),
                     Child, Gauge(TRUE,0,100,100),
                     Child, Gauge(TRUE,0,100,  0),
                  End,

                  Child, BarObject, End,

                  Child, VGroup, FA_SetMax,FV_SetHeight,
                     Child, Gauge(TRUE,0,100,  0),
                     Child, Gauge(TRUE,0,100,100),
                     Child, Gauge(TRUE,0,100,  0),
                     Child, Gauge(TRUE,0,100,100),
                  End,

                  Child, BarObject, End,

                  Child, HGroup, FA_Weight, 25,
                     Child, Gauge(FALSE,0,100,  25),
                     Child, Gauge(FALSE,0,100,  50),
                     Child, Gauge(FALSE,0,100,  75),
                     Child, Gauge(FALSE,0,100, 100),
                  End,
                End,
->ENDPROC
->PROC Objects / Props
                Child, HGroup, FA_Group_PageTitle, 'Proportionals',
                  Child, VGroup,
                     Child, Prop(TRUE,32,16,0),
                     Child, Prop(TRUE,32,08,0),
                     Child, Prop(TRUE,32,04,0),
                     Child, Prop(TRUE,32,02,0),
                  End,

                  Child, VGroup, FA_SetMax,FV_SetHeight,
                     Child, Prop(TRUE,32,16,0),
                     Child, Prop(TRUE,32,08,0),
                     Child, Prop(TRUE,32,04,0),
                     Child, Prop(TRUE,32,02,0),
                  End,

                  Child, HGroup,
                     Child, Prop(FALSE,32,16,0),
                     Child, Prop(FALSE,32,08,0),
                     Child, Prop(FALSE,32,04,0),
                     Child, Prop(FALSE,32,02,0),
                  End,
                End,
->ENDPROC
->PROC Objects / Sliders
                Child, HGroup, FA_Group_PageTitle, 'Sliders',
                  Child, VGroup, FA_SetMax,FV_SetHeight,
                     Child, SliderObject, FA_Horizontal,TRUE, SliderFrame,FA_Back,FI_Shine,  'FA_Numeric_Max',10, 'FA_Numeric_Value', 0, End,
                     Child, SliderObject, FA_Horizontal,TRUE, SliderFrame,FA_Back,FI_Fill,   'FA_Numeric_Max',10, 'FA_Numeric_Value', 5, End,
                     Child, SliderObject, FA_Horizontal,TRUE, SliderFrame,FA_Back,FI_Shadow, 'FA_Numeric_Max',10, 'FA_Numeric_Value',10, End,

                     Child, BarObject, End,
->                           Child, AreaObject, FA_Back,FI_Highlight, FA_FixHeight,3, End,

                     Child, SliderObject, FA_Horizontal,TRUE, SliderFrame,FA_Back,FI_Dark, 'FA_Numeric_Max',  3, 'FA_Numeric_Min',    1, 'FA_Numeric_Value',  1, End,
                     Child, SliderObject, FA_Horizontal,TRUE, SliderFrame,FA_Back,FI_Highlight, 'FA_Numeric_Max',  3, 'FA_Numeric_Min', -3, 'FA_Numeric_Value',  0, End,
                  End,

                  Child, BarObject, End,

                  Child, HGroup, FA_Weight,50,
                     Child, SliderObject, FA_ChainToCycle,FALSE, SliderFrame,FA_Back,FI_Dark,  'FA_Numeric_Max',  32, End,
                     Child, SliderObject, SliderFrame,FA_Back,FI_Shine, 'FA_Numeric_Max',  3, 'FA_Numeric_Min', -3, End,
                  End,
                End,
->ENDPROC
              End,
->ENDPROC
->PROC Groups
              Child, Page, FA_Group_PageTitle,'Groups',
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

                  Child, VGroup, GroupFrame, FA_Frame_Title, 'Array', FA_Group_Rows,3,
                     Child, rectObj(), Child, rectObj(), Child, rectObj(),
                     Child, rectObj(), Child, rectObj(), Child, rectObj(),
                     Child, rectObj(), Child, rectObj(), Child, rectObj(),
                  End,
                End,

                Child, VGroup, FA_Group_PageTitle, 'Layout',
                  Child, HGroup, GroupFrame, FA_Frame_Title, 'Different Weights',
                     Child, textObj( '25 kg',[FA_Weight, 25, NIL]),
                     Child, textObj( '50 kg',[FA_Weight, 50, NIL]),
                     Child, textObj( '75 kg',[FA_Weight, 75, NIL]),
                     Child, textObj('100 kg',[FA_Weight,100, NIL]),
                  End,

                  Child, HGroup, GroupFrame, FA_Frame_Title, 'Fixed & Variable Sizes',
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
->ENDPROC
->PROC Special
              Child, Page, FA_Group_PageTitle,'Special',
->PROC Special / Rasters
                Child, VGroup, FA_Group_PageTitle,'Rasters', FA_Group_VSpacing, 0,
                  Child, HGroup, FA_Group_HSpacing, 0,
                     Child, rast(FI_Highlight),
                     Child, rast(FI_Shine_Highlight),
                     Child, rast(FI_HalfShine_Highlight),
                     Child, rast(FI_Fill_Highlight),
                     Child, rast(FI_HalfShadow_Highlight),
                     Child, rast(FI_Shadow_Highlight),
                     Child, rast(FI_HalfDark_Highlight),
                     Child, rast(FI_Dark_Highlight),
                  End,
                  Child, HGroup, FA_Group_HSpacing, 0,
                     Child, rast(FI_Shine),
                     Child, rast(FI_Shine_HalfShine),
                     Child, rast(FI_Shine_Fill),
                     Child, rast(FI_Shine_HalfShadow),
                     Child, rast(FI_Shine_Shadow),
                     Child, rast(FI_Shine_HalfDark),
                     Child, rast(FI_Shine_Dark),
                  End,

                  Child, HGroup, FA_Group_HSpacing, 0,
                     Child, rast(FI_HalfShine),
                     Child, rast(FI_HalfShine_Fill),
                     Child, rast(FI_HalfShine_HalfShadow),
                     Child, rast(FI_HalfShine_Shadow),
                     Child, rast(FI_HalfShine_HalfDark),
                     Child, rast(FI_HalfShine_Dark),
                  End,

                  Child, HGroup, FA_Group_HSpacing, 0,
                     Child, rast(FI_Fill),
                     Child, rast(FI_Fill_HalfShadow),
                     Child, rast(FI_Fill_Shadow),
                     Child, rast(FI_Fill_HalfDark),
                     Child, rast(FI_Fill_Dark),
                  End,

                  Child, HGroup, FA_Group_HSpacing, 0,
                     Child, rast(FI_HalfShadow),
                     Child, rast(FI_HalfShadow_Shadow),
                     Child, rast(FI_HalfShadow_HalfDark),
                     Child, rast(FI_HalfShadow_Dark),
                  End,

                  Child, HGroup, FA_Group_HSpacing, 0,
                     Child, rast(FI_Shadow),
                     Child, rast(FI_Shadow_HalfDark),
                     Child, rast(FI_Shadow_Dark),
                  End,

                  Child, HGroup, FA_Group_HSpacing, 0,
                     Child, rast(FI_HalfDark),
                     Child, rast(FI_HalfDark_Dark),
                  End,

                  Child, rast(FI_Dark),
                End,
->ENDPROC
->PROC Special / Text Engine
                Child, GroupObject, FA_Group_PageTitle,'Text Engine',
                  Child, TextObject,
                         FA_ChainToCycle,    FALSE,
                         FA_SetMax,          FV_SetHeight,
                         FA_Text,            {__Text},
                         FA_SetMin,           FV_SetHeight,
                         End,
                End,
->ENDPROC
              End,
->ENDPROC
            End,
         End,
       End,
     End

     IF app
/*
       F_DoA(b1,FM_Notify,[FA_Pressed,FALSE,p,3,FM_Set,FA_Page_Active,grp1])
       F_DoA(b2,FM_Notify,[FA_Pressed,FALSE,p,3,FM_Set,FA_Page_Active,grp2])
       F_DoA(b3,FM_Notify,[FA_Pressed,FALSE,p,3,FM_Set,FA_Page_Active,grp3])
       F_DoA(p4,FM_Notify,[FA_Prop_First,FV_Notify_Always,p3,3,FM_Set,FA_Prop_First,FV_Notify_Value])
       F_DoA(p3,FM_Notify,[FA_Prop_First,FV_Notify_Always,p2,3,FM_Set,FA_Prop_First,FV_Notify_Value])
       F_DoA(p2,FM_Notify,[FA_Prop_First,FV_Notify_Always,p1,3,FM_Set,FA_Prop_First,FV_Notify_Value])
       F_DoA(g1,FM_Notify,[FA_Numeric_Value,FV_Notify_Always,g3,5,FM_Set,FA_Numeric_Value,FV_Notify_Value,FA_Numeric_Multiply,2])
       F_DoA(g1,FM_Notify,[FA_Numeric_Value,FV_Notify_Always,s1,5,FM_Set,FA_Numeric_Value,FV_Notify_Value,FA_Numeric_Multiply,2])
*/
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
         FA_ChainToCycle,  FALSE,
         FA_SetMax,        FV_SetHeight,
         FA_Frame,         '$text-frame',
         FA_Back,          FI_HalfShadow,
         FA_Text,          name,
         FA_Text_PreParse, '<align=center>',
         2,                tags,
         End
ENDPROC
PROC rectObj()
   RETURN   AreaObject,
         FA_ChainToCycle,  FALSE,
         FA_Frame,         '$text-frame',
         FA_Back,          FI_HalfShadow,
         End
ENDPROC
PROC rast(rast)
   RETURN   AreaObject,
         FA_ChainToCycle,  FALSE,
         FA_Back,          rast,
         End
ENDPROC
PROC button(label,input)
   RETURN TextObject,
        FA_SetMax,          FV_SetHeight,
        FA_Frame,           '$button-frame',
        FA_Font,            '$button-font',
        FA_Back,            '$button-back',
        FA_SetMin,          FV_SetHeight,
        FA_InputMode,       input,
        FA_Text,            label,
        FA_Text_PreParse,   '<align=center>',
        End
ENDPROC

PROC draw_hook(msg=A1:PTR TO FS_ImageDisplay_HookDraw)
   DEF x1,y1,x2,y2,x3,rp

   sys_RGlob()

   x1 := msg.region.x1 ; x3 := msg.region.x2 ; x2 := x3 - x1 / 3 + x1
   y1 := msg.region.y1 ; y2 := msg.region.y2 ; rp := msg.render.rport

   _APen(msg.render.palette.pens[FV_Pen_HalfShadow]) ; _Boxf(x1,y1,x2,y2)
   _APen(msg.render.palette.pens[FV_Pen_Fill])       ; _Boxf(x2+1,y1,x3,y2)
ENDPROC

->PROC Objects / Strings
/*
                Child, VGroup, NoFrame, FA_FixedHeight,TRUE,
                  Child, BarTitle('Numeric string'),
                  Child, F_MakeObjA(FO_String,['1234',32],[FA_String_Accept,{__Num}, NIL]),
                  Child, BarTitle('Hexadecimal string'),
                  Child, F_MakeObjA(FO_String,['FF123a',32],[FA_String_Accept,{__Hex}, NIL]),
                  Child, BarTitle('Alphabetic strings & Alignement'),

                  Child, HGroup, NoFrame,
                     Child, F_MakeObjA(FO_String,['Lotan is my Lord',80],[FA_String_Reject,{__Num}, FA_String_Format,FV_String_Format_Left,   NIL]),
                     Child, F_MakeObjA(FO_String,['Lotan is my Lord',80],[FA_String_Reject,{__Num}, FA_String_Format,FV_String_Format_Center, NIL]),
                     Child, F_MakeObjA(FO_String,['Lotan is my Lord',80],[FA_String_Reject,{__Num}, FA_String_Format,FV_String_Format_Right,  NIL]),
                  End,
                End,
*/
->ENDPROC

->PROC Datas
__Text:
   CHAR '<align=center><spacing=2>',
      'Normal text, <pens text=highlight>Highlighted text</pens><br>',
      '<pens style="emboss">Embossed</pens>, <pens style="ghost">Ghost</pens><br>',
      '<pens style="light">Some Light</pens>, <pens style="shadow">Some Shadow</pens><br>',
      '<pens style="glow">Glowing</pens>, <pens up=1 shadow=4>Emboss & Shadow</pens><br>',
      '<i>Italic</i>, <b>Bold</b>, <u>Underlined</u>, Normal, <i><b><u>Mixed</u></b></i><br>',
      '<align=left>A very, very, very long line that will be nicely truncated<br>',
      '<align=center>A very, very, very long line that will be nicely truncated<br>',
      '<align=right>A very, very, very long line that will be nicely truncated',NIL

__CrawText:
   CHAR  '<br><align=center>',
       '<pens shadow=shadow text=shine><font face=helvetica size=15>!! <b>Welcome</b> !!</pens><br><br>',
       '<pens shadow=halfshadow><font size=11>Yes, welcome to a brand new world !',
       '<br>',
       '<hr width=60%>',
       '<br>',
       'Well... This is ',
       '<font size=13 color=#FF0000>F',
       '<font         color=#FFCC00>e</font>',
       '<font         color=#66FF00>e</font>',
       '<font         color=#00FF66>l</font>',
       '<font         color=#00CCFF>i</font>',
       '<font         color=#0000FF>n</font></font>, a new<br>',
       '<b>Object Oriented System</b>.',
       '<br>',
       '<br>',
       '<font size=13 color=#FF0000>F',
       '<font         color=#FFCC00>e</font>',
       '<font         color=#66FF00>e</font>',
       '<font         color=#00FF66>l</font>',
       '<font         color=#00CCFF>i</font>',
       '<font         color=#0000FF>n</font></font> ',
       'has been written from scratch.',
       '<br>',
       'It does not use <b>BOOPSI</b><br>',
       'or <u>any</u> other kind of stuff.<br>',
       '<br>',
       '<font size=13 color=#FF0000>F',
       '<font         color=#FFCC00>e</font>',
       '<font         color=#66FF00>e</font>',
       '<font         color=#00FF66>l</font>',
       '<font         color=#00CCFF>i</font>',
       '<font         color=#0000FF>n</font></font> ',
       'is very fast,<br>',
       'easy to use, and very small<br>',
       '(feelin.library is about 24Ko)<br>',
       '<br>',
       '<pens style=glow><font face=helvetica size=13><u>Main Features</u></font></pens><br>',
       'Own Memory Management\n',
       'Shared Objects\n',
       '<b>Dynamic IDs</b> <font size=9px>(no longer static ones)</font>\n',
       'Easy programmer interface\n',
       'Advanced concepts & OOP\n',
       'Easy debugging\n',
       'Great flexibility\n',
       'Extremely configurable GUI\n',
       '<hr width=60%>',
       '<br>',
       '<font size=13 color=#FF0000>F',
       '<font         color=#FFCC00>e</font>',
       '<font         color=#66FF00>e</font>',
       '<font         color=#00FF66>l</font>',
       '<font         color=#00CCFF>i</font>',
       '<font         color=#0000FF>n</font></font> ',
       'has no past,<br>',
       'only future.<br><br><br>',

       '<i>Well, I hope ;-)</i>...<br><br><br>',

      '<pens shadow=shadow text=shine>E<pens text=halfshine>n</pens><pens text=fill>j</pens><pens text=halfshadow>o</pens><pens text=shadow>y</pens>,<br><br><br>',
      '<font color=#FF00FF>L</font>',
      '<font color=#AA00FF>o</font>',
      '<font color=#5500FF>t</font>',
      '<font color=#0000FF>s</font> ',
      '<font color=#0055FF>o</font>',
      '<font color=#00AAFF>f</font> ',
      '<font color=#00FFFF>l</font>',
      '<font color=#00FFAA>o</font>',
      '<font color=#00FF55>v</font>',
      '<font color=#00FF00>e</font> ',
      '<font color=#55FF00>!</font>',
      '<font color=#AAFF00>!</font>',
      '<font color=#FFFF00>!</font></pens>',
      '<br><br><br><br></font>',NIL

__Hex:   CHAR 'ABCDEFabcdef'
__Num:   CHAR '1234567890',0   
->ENDPROC
