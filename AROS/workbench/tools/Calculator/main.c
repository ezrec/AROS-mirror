/*
    Copyright © 2012, The AROS Development Team. All rights reserved.
    $Id$
*/

#define INTUITION_NO_INLINE_STDARG

#define DEBUG 0
#include <aros/debug.h>

#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/graphics.h>
#include <proto/utility.h>

#include <libraries/mui.h>

#include "display.h"

extern IPTR CalcDisplay_Dispatcher();
struct MUI_CustomClass *CalcDisplay_CLASS;

int main(int argc, char *argv[])
{
    Object *CalcAppObj, *CalcWindObj, *CalcWContentsObj, *CalcDisplayObj;
    Object *ButAObj, *ButTmp1Obj, *ButTmp2Obj, *ButCAObj, *ButCEObj;
    Object *ButBObj, *ButTmp5Obj, *ButTmp6Obj, *ButTmp7Obj, *ButTmp8Obj;
    Object *ButCObj, *But7Obj, *But8Obj, *But9Obj, *ButDIVObj;
    Object *ButDObj, *But4Obj, *But5Obj, *But6Obj, *ButMULTObj;
    Object *ButEObj, *But1Obj, *But2Obj, *But3Obj, *ButMINUSObj;
    Object *ButFObj, *But0Obj, *ButPeriodObj, *ButPLUSObj, *ButEQUALObj;

    CalcDisplay_CLASS = MUI_CreateCustomClass(NULL, MUIC_Area, NULL, sizeof(struct CalcDisplay_DATA), CalcDisplay_Dispatcher);

    CalcAppObj = ApplicationObject,
        MUIA_Application_Title,       (IPTR) "AROS Calculator",
        MUIA_Application_Version,     (IPTR) "$VER: Calc 1.0 (04.05.2012)",
        MUIA_Application_Copyright,   (IPTR) "Copyright © 2012, The AROS Development Team. All rights reserved.",
        MUIA_Application_Author,      (IPTR) "Nick Andrews",
        MUIA_Application_Description, (IPTR) "Calculates stuff(tm).",
        MUIA_Application_Base,        (IPTR) "CALCULATOR",

        SubWindow, (IPTR) (CalcWindObj = WindowObject,
            MUIA_Window_Title, (IPTR) "Calculator",
            MUIA_Window_ID, MAKE_ID('C','A','L','C'),
            MUIA_Window_SizeGadget, TRUE,
            WindowContents, (IPTR) (CalcWContentsObj = VGroup,

                Child, (IPTR)(CalcDisplayObj = NewObject(CalcDisplay_CLASS->mcc_Class, NULL,
                TAG_DONE)),
                Child, (IPTR)VGroup,
                    Child, (IPTR) ColGroup(5),
                        Child, ButAObj = SimpleButton("_A"),
                        Child, ButTmp1Obj = SimpleButton(" "),
                        Child, ButTmp2Obj = SimpleButton(" "),
                        Child, ButCAObj = SimpleButton("CA"),
                        Child, ButCEObj = SimpleButton("CE"),
                        Child, ButBObj = SimpleButton("_B"),
                        Child, ButTmp5Obj = SimpleButton(" "),
                        Child, ButTmp6Obj = SimpleButton(" "),
                        Child, ButTmp7Obj = SimpleButton(" "),
                        Child, ButTmp8Obj = SimpleButton(" "),
                        Child, ButCObj = SimpleButton("_C"),
                        Child, But7Obj = SimpleButton("_7"),
                        Child, But8Obj = SimpleButton("_8"),
                        Child, But9Obj = SimpleButton("_9"),
                        Child, ButDIVObj = SimpleButton("_/"),
                        Child, ButDObj = SimpleButton("_D"),
                        Child, But4Obj = SimpleButton("_4"),
                        Child, But5Obj = SimpleButton("_5"),
                        Child, But6Obj = SimpleButton("_6"),
                        Child, ButMULTObj = SimpleButton("_*"),
                        Child, ButEObj = SimpleButton("_E"),
                        Child, But1Obj = SimpleButton("_1"),
                        Child, But2Obj = SimpleButton("_2"),
                        Child, But3Obj = SimpleButton("_3"),
                        Child, ButMINUSObj = SimpleButton("_-"),
                        Child, ButFObj = SimpleButton("_F"),
                        Child, But0Obj = SimpleButton("_0"),
                        Child, ButPeriodObj = SimpleButton("_."),
                        Child, ButPLUSObj = SimpleButton("_+"),
                        Child, ButEQUALObj = SimpleButton("_="),
                    End,
                End,
            End),
        End),

    End;

    if (!CalcAppObj)
    {
        D(bug("[CALC] Failed to create Calculator GUI\n"));
        return 5;
    }

    SET(ButAObj, MUIA_Disabled, TRUE);
    SET(ButBObj, MUIA_Disabled, TRUE);
    SET(ButCObj, MUIA_Disabled, TRUE);
    SET(ButDObj, MUIA_Disabled, TRUE);
    SET(ButEObj, MUIA_Disabled, TRUE);
    SET(ButFObj, MUIA_Disabled, TRUE);
    SET(ButTmp1Obj, MUIA_Disabled, TRUE);
    SET(ButTmp2Obj, MUIA_Disabled, TRUE);
    SET(ButCAObj, MUIA_Disabled, TRUE);
    SET(ButCEObj, MUIA_Disabled, TRUE);
    SET(ButTmp5Obj, MUIA_Disabled, TRUE);
    SET(ButTmp6Obj, MUIA_Disabled, TRUE);
    SET(ButTmp7Obj, MUIA_Disabled, TRUE);
    SET(ButTmp8Obj, MUIA_Disabled, TRUE);

    DoMethod(CalcWindObj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, CalcAppObj, 2,
        MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

    DoMethod(ButCAObj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)-1); // CA
    DoMethod(ButCEObj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)-2); // CE

    DoMethod(CalcDisplayObj, MUIM_Notify, MUIA_CalcDisplay_Calculated, MUIV_EveryTime, ButCAObj, 3,
        MUIM_Set, MUIA_Disabled, MUIV_TriggerValue);
    DoMethod(CalcDisplayObj, MUIM_Notify, MUIA_CalcDisplay_Calculated, MUIV_EveryTime, ButCEObj, 3,
        MUIM_Set, MUIA_Disabled, MUIV_TriggerValue);

    DoMethod(ButAObj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'A');
    DoMethod(ButBObj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'B');
    DoMethod(ButCObj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'C');
    DoMethod(ButDObj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'D');
    DoMethod(ButEObj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'E');
    DoMethod(ButFObj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'F');

    DoMethod(But1Obj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'1');
    DoMethod(But2Obj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'2');
    DoMethod(But3Obj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'3');
    DoMethod(But4Obj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'4');
    DoMethod(But5Obj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'5');
    DoMethod(But6Obj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'6');
    DoMethod(But7Obj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'7');
    DoMethod(But8Obj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'8');
    DoMethod(But9Obj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'9');
    DoMethod(But0Obj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'0');

    DoMethod(ButPeriodObj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'.');

    DoMethod(ButDIVObj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'/');
    DoMethod(ButMULTObj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'*');
    DoMethod(ButMINUSObj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'-');
    DoMethod(ButPLUSObj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'+');
    DoMethod(ButEQUALObj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'=');

    SET(CalcWindObj, MUIA_Window_Open, TRUE);
    {
        ULONG sigs = 0;

        while (DoMethod(CalcAppObj, MUIM_Application_NewInput,
                &sigs) != MUIV_Application_ReturnID_Quit)
        {
            if (sigs)
            {
                sigs = Wait(sigs | SIGBREAKF_CTRL_C);
                if (sigs & SIGBREAKF_CTRL_C)
                    break;
            }
        }
    }

    SET(CalcWindObj, MUIA_Window_Open, FALSE);

    MUI_DisposeObject(CalcAppObj);

    MUI_DeleteCustomClass(CalcDisplay_CLASS);

main_error:
    return 0;
}
