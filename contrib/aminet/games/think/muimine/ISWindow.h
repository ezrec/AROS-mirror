#ifndef  __ISWINDOW_H_
#define  __ISWINDOW_H_

/*
    header file for X-Mines Image Select window MUI custom class
*/


/********************************************************************

    class attributes

********************************************************************/

/*
    these attributes are [I.G], they specify the image files that
    are selected
*/
#define MUIA_ISWindow_MineFieldImage    (ISWINDOW_TAG_BASE | 0x0001)
#define MUIA_ISWindow_StartButtonImage  (ISWINDOW_TAG_BASE | 0x0002)
#define MUIA_ISWindow_MinesDigitsImage  (ISWINDOW_TAG_BASE | 0x0003)
#define MUIA_ISWindow_TimeDigitsImage   (ISWINDOW_TAG_BASE | 0x0004)

/*
    this attribute is [.SG], it specifies whether the window was
    closed by the 'OK' or 'Cancel' buttons
*/
#define MUIA_ISWindow_ExitCode          (ISWINDOW_TAG_BASE | 0x0008)


/********************************************************************

    special values for class attributes

********************************************************************/

#define MUIV_ISWindow_ExitCode_Cancel   0
#define MUIV_ISWindow_ExitCode_OK       1



/********************************************************************

    class methods

********************************************************************/

#define MUIM_ISWindow_ExitCheck         (ISWINDOW_TAG_BASE | 0x0800)

struct MUIP_ISWindow_ExitCheck  { STACKED ULONG MethodId; STACKED ULONG ExitCode; };


/********************************************************************

    function prototypes

********************************************************************/

struct MUI_CustomClass * CreateISWindowClass(void);
void DeleteISWindowClass(struct MUI_CustomClass * mcc);


#endif /* __ISWINDOW_H_ */

