#include <stdio.h>
#include <stdlib.h>

#include <clib/alib_protos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>

#ifdef __AROS__
#include <proto/muimaster.h>
extern struct Library *MUIMasterBase;
#endif


#include <mui.h>
#undef SysBase /* the definition should be not done in mui.h */
#undef IntuitionBase /* the definition should be not done in mui.h */
#undef DOSBase /* the definition should be not done in mui.h */

#include "libs.h"
#include "madmatrix.h"

/*#define SOLVE*/

#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))

int main(int argc, char **argv)
{
  ULONG ok;
  Object *app,*win_main, *madmatrix, *restart, *shake, *taille, *groupe;
#ifdef SOLVE
  Object *resoud
#endif


  struct MUI_CustomClass *mcc;

  if ( ! Ouvrir_Libs() )
  {
    printf("Ouverture des librairies impossible\n");
    return(0);
  }



  if (!(mcc = MUI_CreateCustomClass(NULL,MUIC_Area,NULL,sizeof(struct Madmatrix_Data),Madmatrix_Dispatcher)))
  {
    printf("Cannot create custom class.\n");
    return(0);
  }



  app=ApplicationObject,
    MUIA_Application_Title,     "MadMatrix",
    MUIA_Application_Version,    "$VER: MadMatrix v1.2",
    MUIA_Application_Copyright,   "© Olivier Croquette",
    MUIA_Application_Author,    "Olivier Croquette",
    MUIA_Application_Description,  "'Rotation in a matrix' Game",

    SubWindow,win_main=WindowObject,
      MUIA_Window_Title,"MadMatrix",
      MUIA_Window_ID , MAKE_ID('G','N','R','C'),
      MUIA_CycleChain, TRUE,
      WindowContents,
        groupe=VGroup,
          Child, TextObject,
              TextFrame,
              MUIA_Background, MUII_TextBack,
              MUIA_Text_Contents, "I'll make you mad !",
          End,
          Child,
            HGroup,
              Child, Label("Size : "),
              Child, taille=MUI_NewObject(MUIC_Slider,
                                                MUIA_Numeric_Min,3,
                                                MUIA_Numeric_Max,MADMATRIX_TAILLE_MAX,
                                                MUIA_Numeric_Format,"%1ld",
                                                MUIA_Slider_Horiz,TRUE,
                                                MUIA_Numeric_Reverse,FALSE,
                                                TAG_DONE),
            End,
          Child,
            HGroup,
              Child,restart =SimpleButton("Restart"),
              Child,shake   =SimpleButton("Shake"),
#ifdef SOLVE
              Child,resoud   =SimpleButton("Solve"),
#endif
            End,

          Child, madmatrix=(Object *)NewObject(mcc->mcc_Class,NULL,TextFrame,TAG_DONE),
        End,

      End,
  End;


  if(!app)
  {
    printf("Unable to create application.\n");
    Fermer_Libs();
    return(0);
  }



  DoMethod(win_main,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
            app,2,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);

  DoMethod(restart,MUIM_Notify,MUIA_Pressed,FALSE,madmatrix,1,MADMATRIX_RESTART);
  DoMethod(shake,MUIM_Notify,MUIA_Pressed,FALSE,madmatrix,1,MADMATRIX_SHAKE);
#ifdef SOLVE
  DoMethod(resoud,MUIM_Notify,MUIA_Pressed,FALSE,madmatrix,1,MADMATRIX_RESOUD);
#endif

  DoMethod(taille,MUIM_Notify,MUIA_Numeric_Value,MUIV_EveryTime,madmatrix,3,MUIM_Set,MADMATRIX_TAILLE,MUIV_TriggerValue);

  set(win_main,MUIA_Window_DefaultObject, (LONG)madmatrix);
  set(win_main,MUIA_Window_Open,TRUE);

  get(win_main,MUIA_Window_Open,&ok);

  set(madmatrix,MADMATRIX_GROUPE,(ULONG)groupe);

  if ( argc > 1 )
  {
    set(taille,MUIA_Numeric_Value,atoi(argv[1]));
    set(madmatrix,MADMATRIX_TAILLE,atoi(argv[1]));
    DoMethod(madmatrix,MADMATRIX_RESTART);
  }
  else
    set(taille,MUIA_Numeric_Value,MADMATRIX_TAILLE_MAX);


  if(ok)
  {

    LONG sigs=0;

    while(DoMethod(app,MUIM_Application_NewInput,&sigs)!= (unsigned int)MUIV_Application_ReturnID_Quit)
    {
      if(sigs) sigs = Wait(sigs);
    }
  }

  MUI_DisposeObject(app);
  MUI_DeleteCustomClass(mcc);


  Fermer_Libs();

  return(0);
}

