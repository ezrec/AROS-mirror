#ifndef  __FACEBUTTON_H_
#define  __FACEBUTTON_H_
/*
    header file for X-Mines face button MUI custom class
*/


/********************************************************************

    class attributes

********************************************************************/

/*
    this attributes is set at initialization only
*/
#define MUIA_FaceButton_ImageFile   (FACEBUTTON_TAG_BASE | 0x0001)

/*
    this attribute is setable only, it defines which image to show
*/
#define MUIA_FaceButton_ImageIdx    (FACEBUTTON_TAG_BASE | 0x0002)

#define MUIV_FaceButton_ImageIdx_Normal     0
#define MUIV_FaceButton_ImageIdx_Selected   1
#define MUIV_FaceButton_ImageIdx_Oh         2
#define MUIV_FaceButton_ImageIdx_Good       3
#define MUIV_FaceButton_ImageIdx_Bad        4


/********************************************************************

    class methods

********************************************************************/



/********************************************************************

    function prototypes

********************************************************************/

struct MUI_CustomClass * CreateFaceButtonClass(void);
void DeleteFaceButtonClass(struct MUI_CustomClass * mcc);


#endif /* __FACEBUTTON_H_ */
