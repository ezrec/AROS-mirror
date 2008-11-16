
extern const struct Hook DisplayLI_TextHook;
extern const struct Hook CompareLI_TextHook;
extern const struct Hook ConstructLI_TextHook;
extern const struct Hook DestructLI_TextHook;






/* MUI STUFF */

#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif

/* *********************************************** */

#define SimpleButtonCycle(name) \
  TextObject, \
    ButtonFrame, \
    MUIA_CycleChain, 1, \
    MUIA_Font, MUIV_Font_Button, \
    MUIA_Text_Contents, name, \
    MUIA_Text_PreParse, "\33c", \
    MUIA_InputMode    , MUIV_InputMode_RelVerify, \
    MUIA_Background   , MUII_ButtonBack, \
  End

/* *********************************************** */

#define SimpleButtonTiny(name) \
  TextObject, \
    ButtonFrame, \
    MUIA_Font, MUIV_Font_Tiny, \
    MUIA_Text_Contents, name, \
    MUIA_Text_PreParse, "\33c", \
    MUIA_InputMode    , MUIV_InputMode_RelVerify, \
    MUIA_Background   , MUII_ButtonBack, \
  End

/* *********************************************** */

#define NFloattext(ftxt) \
    NListviewObject, \
      MUIA_Weight, 50, \
      MUIA_CycleChain, 1, \
      MUIA_NListview_Horiz_ScrollBar, MUIV_NListview_HSB_None, \
      MUIA_NListview_Vert_ScrollBar, MUIV_NListview_VSB_Always, \
      MUIA_NListview_NList,NFloattextObject, \
        MUIA_NList_DefaultObjectOnClick, TRUE, \
        MUIA_NFloattext_Text, ftxt, \
        MUIA_NFloattext_TabSize, 4, \
        MUIA_NFloattext_Justify, TRUE, \
      End, \
    End

/* *********************************************** */

