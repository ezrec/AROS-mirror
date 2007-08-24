OPT PREPROCESS

MODULE 'feelin','libraries/feelin'

#define CrawlerObject F_NewObjA('Crawler',[TAG_IGNORE,NIL

PROC main()
    DEF app,win

    IF feelinbase := OpenLibrary('feelin.library',FV_FEELIN_VERSION)
      app := AppObject,
         Child, win := WindowObject,
            FA_Window_Title, 'Feelin : Crawling',
            FA_Window_Open,   TRUE,

            Child, HGroup, FA_MinWidth,100,
                Child, CrawlerObject, GaugeFrame, 'FA_Crawler_Delay',20, FA_MinHeight,40, FA_MinWidth, 40,
                  Child, VGroup, FA_ColorScheme,'<scheme shine="s:2" fill="s:4" />', FA_Back,FI_Fill, FA_Frame,'<frame padding="10" />',
                     Child, TextObject, FA_SetMax,FV_SetHeight,FA_ChainToCycle,FALSE, FA_Text, {__Text}, End,

                     Child, CrawlerObject, GaugeFrame, 'FA_Crawler_Delay',40, FA_MinHeight,8*12, FA_MinWidth, 40, 
                        Child, VGroup, FA_ColorScheme,'<scheme shine="s:2" fill="s:4" />', FA_Back,FI_Fill, FA_SetMax,FV_SetHeight, FA_Frame,'<frame padding="10" />',
                            Child, TextObject, FA_ChainToCycle,FALSE, FA_Text, {__Text}, End,

                            Child, CrawlerObject, GaugeFrame, 'FA_Crawler_Delay',60, FA_MinHeight,8*12,
                              Child, VGroup, FA_ColorScheme,'<scheme shine="s:2" fill="s:4" />', FA_Back,FI_Fill, FA_SetMax,FV_SetHeight, FA_Frame,'<frame padding="10" />',
                                 Child, TextObject, FA_ChainToCycle,FALSE, FA_Text, {__Text}, End,
                              End,
                            End,
                        End,
                     End,
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
      WriteF('Unable to open feelin.library %ld\n',FV_FEELIN_VERSION)
    ENDIF
ENDPROC

__Text:
    CHAR '<align=center><pens style=shadow>',
        'We live on a <b>montain</b>.\n\n',

        'Right at the top.\n\n',

        'There is\n',
        'a beautiful view\n',
        'from the top\n',
        'of the montain.\n\n',

        'Every morning\n',
        'I walk toward the edge\n',
        'and throw little things off',0
