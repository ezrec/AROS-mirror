#ifndef  __LEWINDOW_H_
#define  __LEWINDOW_H_

/*
    header file for X-Mines Level Editor window MUI custom class
*/


/********************************************************************

    class attributes

********************************************************************/

/*
    both of these tags MUST be set on initialization, the data is
    a pointer to a struct LevelDataList

    MUIA_LEWindow_GivenLevels  - this attribute is the oigional data
                                 to be edited, it is not modified

    MUIA_LEWindow_ReturnLevels - this attribute is an empty list that
                                 is filled with the new list if the
                                 levels were edited and editing was
                                 accepted

    when the window closes the return list has to be checked to
    determine if editing took place and was accepted

    it is the responsibility of the caller to dispose of any level
    data both given and return
*/
#define MUIA_LEWindow_GivenLevels   (LEWINDOW_TAG_BASE | 0x0001)
#define MUIA_LEWindow_ReturnLevels  (LEWINDOW_TAG_BASE | 0x0002)


/********************************************************************

    special values for class attributes

********************************************************************/



/********************************************************************

    class methods

********************************************************************/



/********************************************************************

    function prototypes

********************************************************************/

struct MUI_CustomClass * CreateLEWindowClass(void);
void DeleteLEWindowClass(struct MUI_CustomClass * mcc);


#endif /* __LEWINDOW_H_ */

