/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <string.h>
#include <stdlib.h>

#include <proto/feelin.h>

#include <libraries/feelin.h>
#include <feelin/htmldocument.h>

/****************************************************************************
*** object ******************************************************************
****************************************************************************/

enum    {

        FM_Adjust_Query,
        FM_Adjust_ToString,
        FM_Adjust_ToObject
        
        };
         
struct LocalObjectData
{
    FObject                         XMLObject;
    FObject                         HTMLParser;
    
    FObject                         style_bold;
    FObject                         style_italic;
    FObject                         style_underlined;
    FObject                         align;
/*
    FObject                         font_face;
    FObject                         font_color;
*/
    FObject                         pens_up;
    FObject                         pens_down;
    FObject                         pens_light;
    FObject                         pens_shadow;
    FObject                         pens_text;
    FObject                         pens_style;
    FObject                         test;
};

