#ifndef  __BTWINDOW_H_
#define  __BTWINDOW_H_

/*
    header file for X-Mines Best Times window MUI custom class
*/


/********************************************************************

    class attributes

********************************************************************/

#define MUIA_BTWindow_Level         (BTWINDOW_TAG_BASE | 0x0001)
#define MUIA_BTWindow_LevelIdx      (BTWINDOW_TAG_BASE | 0x0002)
#define MUIA_BTWindow_LevelList     (BTWINDOW_TAG_BASE | 0x0003)
#define MUIA_BTWindow_HighlightRank (BTWINDOW_TAG_BASE | 0x0004)


/********************************************************************

    special values for class attributes

********************************************************************/

#define MUIV_BTWindow_Level_Current         -1

#define MUIV_BTWindow_HighlightRank_None    -1


/********************************************************************

    class methods

********************************************************************/

#define MUIM_BTWindow_ResetLevel        (BTWINDOW_TAG_BASE | 0x0801)
#define MUIM_BTWindow_ResetAll          (BTWINDOW_TAG_BASE | 0x0802)


/********************************************************************

    function prototypes

********************************************************************/

struct MUI_CustomClass * CreateBTWindowClass(void);
void DeleteBTWindowClass(struct MUI_CustomClass * mcc);


#endif /* __BTWINDOW_H_ */

