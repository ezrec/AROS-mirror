#ifndef  __DIGITS_H_
#define  __DIGITS_H_
/*
    header file for X-Mines 3 digit display MUI custom class
*/


/********************************************************************

    class attributes

********************************************************************/

/*
    this attributes is set at initialization only
*/
#define MUIA_Digits_ImageFile   (DIGITS_TAG_BASE | 0x0001)

/*
    this attribute is setable only, it is the number the counter displays
*/
#define MUIA_Digits_Number      (DIGITS_TAG_BASE | 0x0002)

#define MUIV_Digits_Number_Blanked    ((int)0x7FFF)
#define MUIV_Digits_Number_Dashed     ((int)0x7FFE)


/********************************************************************

    class methods

********************************************************************/



/********************************************************************

    function prototypes

********************************************************************/

struct MUI_CustomClass * CreateDigitsClass(void);
void DeleteDigitsClass(struct MUI_CustomClass * mcc);


#endif /* __DIGITS_H_ */
