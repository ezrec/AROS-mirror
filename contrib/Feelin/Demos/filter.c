
#include <libraries/feelin.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/feelin.h>

struct FeelinBase *FeelinBase;

#define MARKUP "<image type='picture' "
#define SOURCE "src='feelin:resources/system/logo.png' "
#define MODE   "mode='scale' "
#define MARKUP_END "/>"

void main(void)
{
    FObject app,win;

    if (F_FEELIN_OPEN)
    {
        app = AppObject,
            Child, win = WindowObject,
                FA_Window_Title,  "Feelin : Filters",
                FA_Window_Open,   TRUE,
                
                FA_Width, "200",
                FA_Height, "200",

                FA_Frame, "<frame id='0' padding='10' />",
                FA_Back, "<image type='gradient' start='#B8F0F8' middle='#000000' end='#002080' />",
                
                Child, RowGroup(4),
                    Child, ImageObject, "FA_Image_Spec", MARKUP SOURCE MARKUP_END, End,
                    Child, ImageObject, "FA_Image_Spec", MARKUP SOURCE "filter='nearest'" MODE MARKUP_END, End,
                    Child, VLabel("<pens text='shine'>Source"),
                    Child, VLabel("<pens text='shine'>Nearest"),
                    Child, ImageObject, "FA_Image_Spec", MARKUP SOURCE "filter='average'" MODE MARKUP_END, End,
                    Child, ImageObject, "FA_Image_Spec", MARKUP SOURCE "filter='bilinear'" MODE MARKUP_END, End,
                    Child, VLabel("<pens text='shine'>Average"),
                    Child, VLabel("<pens text='shine'>Bilinear"),
                End,
            End,
        End;

        if (app)
        {
            F_Do(win,FM_Notify,FA_Window_CloseRequest,TRUE, app,FM_Application_Shutdown,0);
            F_Do(app,FM_Application_Run);
            F_DisposeObj(app);
        }
        else
        {
            Printf("Unable to create application\n");
        }

        F_FEELIN_CLOSE;
    }
}

