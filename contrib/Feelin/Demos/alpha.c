
#include <libraries/feelin.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/feelin.h>

struct FeelinBase *FeelinBase;

#define BACK                                    "<image type='gradient' start='#B8F0F8' middle='#000000' end='#002080' />"

int32 main(void)
{
    FObject app,win,img,txt;

    if (F_FEELIN_OPEN)
    {
        app = AppObject,
            Child, win = WindowObject,
                FA_Window_Title,  "Feelin : Alpha",
                FA_Window_Open,   TRUE,
                
                FA_Width,   "200",
                FA_Height,  "200",

                Child, VGroup,
                    Child, txt = TextObject,
                        FA_Frame,         "<frame id='18' padding='10' />",
                        FA_Back,          BACK,
                        FA_ChainToCycle, FALSE,
                        FA_Text_VCenter, TRUE,
                        FA_Text_PreParse, "<align=center>",
                        FA_Text,
#if !defined(__AROS__)
						"<image type='picture' src='site:images/feelin.png'><br>"
#else
						"<image type='picture' src='Feelin:resources/images/feelin.png'><br>"
#endif
                        "<font face='Helvetica' size='9' color='#1040A0'>Copyright © 2001-2005</font><br>"
                        "<font face='Helvetica' size='15' color='#FFFFFF'><b>Olivier LAVIALE</b></font><br>"
                        "<pens text='#185CB8'>www.gofromiel.com/feelin/</pens><br>"
                        "<br>"
                        "<font face='Helvetica' size='11' color='#FFFF00'>- Contributors -</font>"
                        "<br>"
                        "<font face='Helvetica' size='13' color='#FFFFFF'><b>Guillaume ROGUEZ</b></font><font color='#B8F0F8'> : MorphOS port</font><br>"
                        "<br>"
                        "<font face='Helvetica' size='9' color='#1040A0'><pens text='shadow'>(2005/08/20)</pens></font><br>",
                    End,
                
                    Child, HGroup, GroupFrame, FA_Frame_Title,"Backdrop",
                        Child, img = PopImageObject, "Spec", BACK, End,
                    End,
                End,
            End,
        End;

        if (app)
        {
            F_Do(win,FM_Notify,FA_Window_CloseRequest,TRUE, app,FM_Application_Shutdown,0);

            F_Do(img,FM_Notify, "FA_Preview_Spec",FV_Notify_Always, txt,FM_Set,2, FA_Back,FV_Notify_Value);
 
            F_Do(app,FM_Application_Run);
            F_DisposeObj(app);
        }
        else
        {
            Printf("Unable to create application\n");
        }

        F_FEELIN_CLOSE;
    }

    return 0;
}

