#ifndef FEELIN_TEXTDISPLAY_PARSE_H
#define FEELIN_TEXTDISPLAY_PARSE_H

/*

    TextDisplay/parse.h

*/

#define FV_TD_MAX_ATTRIBUTES                    16

struct in_TD_ParseResult
{
    uint32                          ID;
    uint8                           Types[FV_TD_MAX_ATTRIBUTES];
    uint32                          Attributes[FV_TD_MAX_ATTRIBUTES];
};

typedef struct in_TD_ParseResult                FTDParseResult;

#define _td_result_type(id)                     Context -> result.Types[id]
#define _td_result_attr(id)                     Context -> result.Attributes[id]

enum    {

        FV_TD_RESULT_NONE,
        FV_TD_RESULT_TYPE_HEX,                        // "#33"
        FV_TD_RESULT_TYPE_DEC,                        // "33" "+33" "-33"
        FV_TD_RESULT_TYPE_PIXEL,                      // "33px"
        FV_TD_RESULT_TYPE_PERCENT,                    // "33%"
        FV_TD_RESULT_TYPE_STRING,
        FV_TD_RESULT_TYPE_RESOLVED,                   // "shadow" -> FV_Pen_Shadow
        FV_TD_RESULT_TYPE_BOOLEAN

        };

/* The FTDParseResult structure  holds  results  of  the  translation.  The
command  is  translated  to an ID e.g. <i> becomes FV_TD_ITALIC_ON, and its
attributes are organized (if needed) and translated (strings are duplicated
and nulled, decimal and hexadecimal strings are converted to numeric values
*/

STRPTR td_parse_markup(STRPTR s,FTDParseResult *result);

enum    {

        FV_TD_NONE,
        FV_TD_ALIGN,                            // <align...>
        FV_TD_BOLD_ON,                          // <b>
        FV_TD_BOLD_OFF,                         //              </b>
        FV_TD_BR,                               // <br>
        FV_TD_FONT_ON,                          // <font>
        FV_TD_FONT_OFF,                         //              </font>
        FV_TD_HR,                               // <hr>
        FV_TD_ITALIC_ON,                        // <i>
        FV_TD_ITALIC_OFF,                       //              </i>
        FV_TD_IMAGE,                            // <image ... />
        FV_TD_PENS_ON,                          // <pens ...>
        FV_TD_PENS_OFF,                         //              </pens>
        FV_TD_SPACING,                          // <spacing>
        FV_TD_STOP_ON,                          // <stop>
        FV_TD_STOP_OFF,                         //              </stop>
        FV_TD_UNDERLINED_ON,                    // <u>
        FV_TD_UNDERLINED_OFF                    //              </u>

        };

enum    {

        FV_TD_RESULT_ALIGN

        };

enum    {

        FV_TD_RESULT_FONT_COLOR,
        FV_TD_RESULT_FONT_FACE,
        FV_TD_RESULT_FONT_SIZE

        };

enum    {

        FV_TD_RESULT_HR_ALIGN,
        FV_TD_RESULT_HR_COMPACT,
        FV_TD_RESULT_HR_NOSHADE,
        FV_TD_RESULT_HR_SHADOW,
        FV_TD_RESULT_HR_SHINE,
        FV_TD_RESULT_HR_SIZE,
        FV_TD_RESULT_HR_WIDTH

        };

enum    {

        FV_TD_RESULT_IMAGE_SPEC

        };

enum    {

        FV_TD_RESULT_SPACING

        };

enum    {

        FV_TD_RESULT_PENS_DOWN,
        FV_TD_RESULT_PENS_LIGHT,
        FV_TD_RESULT_PENS_SHADOW,
        FV_TD_RESULT_PENS_STYLE,
        FV_TD_RESULT_PENS_TEXT,
        FV_TD_RESULT_PENS_UP

        };

/**/


enum    {

        FV_TD_PENS_STYLE_NOT_DEFINED,
        FV_TD_PENS_STYLE_EMBOSS,
        FV_TD_PENS_STYLE_GHOST,
        FV_TD_PENS_STYLE_GLOW,
        FV_TD_PENS_STYLE_LIGHT,
        FV_TD_PENS_STYLE_SHADOW

        };

#endif
