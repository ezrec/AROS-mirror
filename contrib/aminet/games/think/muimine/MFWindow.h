#ifndef  __MFWINDOW_H_
#define  __MFWINDOW_H_
/*
    header file for X-Mines type window MUI custom class
*/


/********************************************************************

    class attributes

********************************************************************/

#define MUIA_MFWindow_Level             (MFWINDOW_TAG_BASE | 0x0001)

#define MUIA_MFWindow_MineFieldImage    (MFWINDOW_TAG_BASE | 0x0010)
#define MUIA_MFWindow_StartButtonImage  (MFWINDOW_TAG_BASE | 0x0011)
#define MUIA_MFWindow_MinesDigitsImage  (MFWINDOW_TAG_BASE | 0x0012)
#define MUIA_MFWindow_TimeDigitsImage   (MFWINDOW_TAG_BASE | 0x0013)

#define MUIA_MFWindow_SafeStart         (MFWINDOW_TAG_BASE | 0x0020)


/********************************************************************

    special values for class attributes

********************************************************************/

#define MUIV_MFWindow_Level(w, h, m)        MAKE_LEVEL(w, h, m)

#define MUIV_MFWindow_Level_Beginner        MUIV_MFWindow_Level( 8,  8, 10)
#define MUIV_MFWindow_Level_Intermediate    MUIV_MFWindow_Level(16, 16, 40)
#define MUIV_MFWindow_Level_Expert          MUIV_MFWindow_Level(30, 16, 99)


/********************************************************************

    class methods

********************************************************************/

#define MUIM_MFWindow_CheckBestTime     (MFWINDOW_TAG_BASE | 0x0801)


/********************************************************************

    function prototypes

********************************************************************/

struct MUI_CustomClass * CreateMFWindowClass(void);
void DeleteMFWindowClass(struct MUI_CustomClass * mfwc);


#endif /* __MFWINDOW_H_ */
