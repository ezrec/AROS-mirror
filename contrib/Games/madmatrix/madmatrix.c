#include "madmatrix.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>


#ifdef __AROS__
#include <proto/muimaster.h>
#endif

#include <proto/utility.h>

#include <libraries/mui.h>
#undef SysBase
#undef GfxBase
#undef UtilityBase
#undef IntuitionBase

#include "alloctab.h"

#include <clib/alib_protos.h>

// Custom class MUI

#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif

#define PAS 0.12
#define MADMATRIX_ABS(x) ((x)>0?(x):-(x))

#define MADMATRIX_LIGNE(taille,nombre) (int)((nombre-1)/taille)
#define MADMATRIX_COLONNE(taille,nombre) (int)((nombre-1)%taille)
#define MADMATRIX_NOMBRE(taille,ligne,colonne) ( taille*ligne+colonne+1 )
#define MADMATRIX_VOISINS(taille,nb1, nb2) \
      ((MADMATRIX_ABS(MADMATRIX_LIGNE(taille,nb1)-MADMATRIX_LIGNE(taille,nb2))+\
      MADMATRIX_ABS(MADMATRIX_COLONNE(taille,nb1)-MADMATRIX_COLONNE(taille,nb2)) ) == 1?1:0)

#define MADMATRIX_YOUWIN    "You win"
#define MADMATRIX_GOODLUCK  "Good luck"

void Madmatrix_Rotate(struct IClass *cl,Object *obj, int sens, double pas);
void Madmatrix_Shake(struct IClass *cl,Object *obj, int nb);
void Madmatrix_Init(struct Madmatrix_Data *data);
void Madmatrix_Restart(struct IClass *cl,Object *obj);
void Madmatrix_Tourne(int **tableau,int ligne, int colonne, int sens );
int Madmatrix_Calcul(int **tableau, int taille);


IPTR Madmatrix_Dispose(struct IClass *cl,Object *obj,Msg msg);
IPTR Madmatrix_New(struct IClass *cl,Object *obj,Msg msg);
IPTR Madmatrix_AskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg);
IPTR Madmatrix_Draw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg);
IPTR Madmatrix_Setup(struct IClass *cl,Object *obj,struct MUIP_HandleInput *msg);
IPTR Madmatrix_Cleanup(struct IClass *cl,Object *obj,struct MUIP_HandleInput *msg);
IPTR Madmatrix_HandleInput(struct IClass *cl,Object *obj,struct MUIP_HandleInput *msg);


extern struct GfxBase *GfxBase;
extern struct IntuitionBase *IntuitionBase;
extern struct Library  *MUIMasterBase;



#define _between(a,x,b) ((x)>=(a) && (x)<=(b))
#define _isinobject(x,y) (_between(_mleft(obj),(x),_mright (obj)) && _between(_mtop(obj) ,(y),_mbottom(obj)))

#define MADMATRIX_MAX(a,b) ((a)<(b)?(b):(a))

/// ULONG Madmatrix_New(struct IClass *cl,Object *obj,Msg msg)
IPTR Madmatrix_New(struct IClass *cl,Object *obj,Msg msg)
{
  struct Madmatrix_Data *data;
/*  struct TagItem *tags,*tag;*/

  if (!(obj = (Object *)DoSuperMethodA(cl,obj,msg)))
          return(0);

  data = INST_DATA(cl,obj);

  data->taille = MADMATRIX_TAILLE_MAX;
  data->ntaille = MADMATRIX_TAILLE_MAX;
  data->groupe = NULL;

  data->matrice=(int **)AllocTab(sizeof(int),data->taille,data->taille);

  Madmatrix_Init(data);

  if ( data->matrice == NULL )
    return(FALSE);

  return((IPTR)obj);
}
///

/// ULONG Madmatrix_Dispose(struct IClass *cl,Object *obj,Msg msg)
IPTR Madmatrix_Dispose(struct IClass *cl,Object *obj,Msg msg)
{
  struct Madmatrix_Data *data = (struct Madmatrix_Data *)INST_DATA(cl,obj);

  FreeTab((void **)data->matrice,data->taille);

  return(DoSuperMethodA(cl,obj,msg));
}
///

/// ULONG Madmatrix_AskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg)
IPTR Madmatrix_AskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg)
{
  struct Madmatrix_Data *data = INST_DATA(cl,obj);
  int largeur;

  data->font_sx = _font(obj)->tf_XSize;
  data->font_sy = _font(obj)->tf_YSize;

  /*printf("Ask %p %d %d\n",_font(obj),_font(obj)->tf_XSize,_font(obj)->tf_YSize);*/

  DoSuperMethodA(cl,obj,(Msg)msg);

  largeur = (3*data->taille*data->font_sx+1);

/*
  printf("%p\n",_rp(obj));

  printf("Largeur1 = %d\n",largeur);
  largeur = MADMATRIX_MAX(largeur,TextLength(_rp(obj),MADMATRIX_YOUWIN,strlen(MADMATRIX_YOUWIN)));
  printf("Largeur2 = %d\n",largeur);
  largeur = MADMATRIX_MAX(largeur,TextLength(_rp(obj),MADMATRIX_GOODLUCK,strlen(MADMATRIX_GOODLUCK)));
  printf("Largeur3 = %d\n",largeur);
*/
  largeur = MADMATRIX_MAX(largeur,data->font_sx*strlen(MADMATRIX_YOUWIN));
  largeur = MADMATRIX_MAX(largeur,data->font_sx*strlen(MADMATRIX_GOODLUCK));

  msg->MinMaxInfo->MinWidth += largeur;
  msg->MinMaxInfo->DefWidth += largeur;
  msg->MinMaxInfo->MaxWidth += largeur;

  msg->MinMaxInfo->MinHeight += (2*(data->taille+2)*data->font_sy); /**/
  msg->MinMaxInfo->DefHeight += (2*(data->taille+2)*data->font_sy);
  msg->MinMaxInfo->MaxHeight += (2*(data->taille+2)*data->font_sy);


  return(0);
}
///

/// ULONG Madmatrix_Draw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg)
IPTR Madmatrix_Draw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg)
{
  struct Madmatrix_Data *data = INST_DATA(cl,obj);
  int ligne,colonne;
  char temp[1000];
  int tf_x=_rp(obj)->Font->tf_XSize;
  int tf_y=_rp(obj)->Font->tf_YSize;
  int offset_x, offset_y, x,y;


  /* clear message zone */

  SetAPen(_rp(obj),0);
  RectFill(_rp(obj),_mleft(obj),_mtop(obj)+(2*tf_y)*(data->taille)+1,_mright(obj),_mbottom(obj));

  if ( data->message )
  {
    SetAPen(_rp(obj),1);
    x = _mleft(obj)+1;
    y = _mtop(obj)+(2*tf_y)*(data->taille+1);

    Move(_rp(obj),x,y);
    Text(_rp(obj),data->message,strlen(data->message));

    data->message=NULL;
  }

  if ( data->box == -1 )
  {
    data->box=0;
    SetAPen(_rp(obj),0);
    Move(_rp(obj),_mleft(obj)+(3*tf_x)*data->colonne,_mtop(obj)+(2*tf_y)*(data->ligne+1)-3*tf_x/2);
    Draw(_rp(obj),_mleft(obj)+(3*tf_x)*(data->colonne+2),_mtop(obj)+(2*tf_y)*(data->ligne+1)-3*tf_x/2);
    Draw(_rp(obj),_mleft(obj)+(3*tf_x)*(data->colonne+2),_mtop(obj)+(2*tf_y)*(data->ligne+2)+tf_x/2);
    Draw(_rp(obj),_mleft(obj)+(3*tf_x)*data->colonne,_mtop(obj)+(2*tf_y)*(data->ligne+2)+tf_x/2);
    Draw(_rp(obj),_mleft(obj)+(3*tf_x)*data->colonne,_mtop(obj)+(2*tf_y)*(data->ligne+1)-3*tf_x/2);
    return(TRUE);
  }

  if ( data->box == 1 )
  {
    data->box=0;
    SetAPen(_rp(obj),1);
    Move(_rp(obj),_mleft(obj)+(3*tf_x)*data->colonne,_mtop(obj)+(2*tf_y)*(data->ligne+1)-3*tf_x/2);
    Draw(_rp(obj),_mleft(obj)+(3*tf_x)*(data->colonne+2),_mtop(obj)+(2*tf_y)*(data->ligne+1)-3*tf_x/2);
    Draw(_rp(obj),_mleft(obj)+(3*tf_x)*(data->colonne+2),_mtop(obj)+(2*tf_y)*(data->ligne+2)+tf_x/2);
    Draw(_rp(obj),_mleft(obj)+(3*tf_x)*data->colonne,_mtop(obj)+(2*tf_y)*(data->ligne+2)+tf_x/2);
    Draw(_rp(obj),_mleft(obj)+(3*tf_x)*data->colonne,_mtop(obj)+(2*tf_y)*(data->ligne+1)-3*tf_x/2);
    return(TRUE);
  }


  DoSuperMethodA(cl,obj,(Msg)msg);

  if (data->matrice == NULL)
  {
    return FALSE;
  }

  SetAPen(_rp(obj),0);
  if ( data->mouvement == 0 )
    RectFill(_rp(obj),_mleft(obj),_mtop(obj),_mright(obj),_mbottom(obj));
  else
    RectFill(_rp(obj),
              _mleft(obj)+(3*tf_x)*data->colonne+1,_mtop(obj)+(2*tf_y)*(data->ligne+1)-3*tf_x/2+1,
              _mleft(obj)+(3*tf_x)*(data->colonne+2)-1,_mtop(obj)+(2*tf_y)*(data->ligne+2)+tf_x/2-1
            );


  SetAPen(_rp(obj),1);


  offset_x = (3*tf_x)*data->pas;
  offset_y = (2*tf_y)*data->pas;

  switch ( data->mouvement )
  {
    case 1:

      ligne = data->ligne; colonne = data->colonne;
      x = _mleft(obj)+(3*tf_x)*colonne;  y = _mtop(obj)+(2*tf_y)*(ligne+1);
      sprintf(temp,"%3d",data->matrice[ligne][colonne]);

      Move(_rp(obj),x+offset_x,y); Text(_rp(obj),temp,strlen(temp));

      colonne++;
      x = _mleft(obj)+(3*tf_x)*colonne;  y = _mtop(obj)+(2*tf_y)*(ligne+1);
      sprintf(temp,"%3d",data->matrice[ligne][colonne]);

      Move(_rp(obj),x,y+offset_y); Text(_rp(obj),temp,strlen(temp));

      ligne++;
      x = _mleft(obj)+(3*tf_x)*colonne;  y = _mtop(obj)+(2*tf_y)*(ligne+1);
      sprintf(temp,"%3d",data->matrice[ligne][colonne]);

      Move(_rp(obj),x-offset_x,y); Text(_rp(obj),temp,strlen(temp));

      colonne--;
      x = _mleft(obj)+(3*tf_x)*colonne;  y = _mtop(obj)+(2*tf_y)*(ligne+1);
      sprintf(temp,"%3d",data->matrice[ligne][colonne]);

      Move(_rp(obj),x,y-offset_y); Text(_rp(obj),temp,strlen(temp));

      break;

    case -1:

      ligne = data->ligne; colonne = data->colonne;
      x = _mleft(obj)+(3*tf_x)*colonne;  y = _mtop(obj)+(2*tf_y)*(ligne+1);
      sprintf(temp,"%3d",data->matrice[ligne][colonne]);
      Move(_rp(obj),x,y+offset_y); Text(_rp(obj),temp,strlen(temp));


      colonne++;
      x = _mleft(obj)+(3*tf_x)*colonne;  y = _mtop(obj)+(2*tf_y)*(ligne+1);
      sprintf(temp,"%3d",data->matrice[ligne][colonne]);

      Move(_rp(obj),x-offset_x,y); Text(_rp(obj),temp,strlen(temp));

      ligne++;
      x = _mleft(obj)+(3*tf_x)*colonne;  y = _mtop(obj)+(2*tf_y)*(ligne+1);
      sprintf(temp,"%3d",data->matrice[ligne][colonne]);

      Move(_rp(obj),x,y-offset_y); Text(_rp(obj),temp,strlen(temp));

      colonne--;
      x = _mleft(obj)+(3*tf_x)*colonne;  y = _mtop(obj)+(2*tf_y)*(ligne+1);
      sprintf(temp,"%3d",data->matrice[ligne][colonne]);

      Move(_rp(obj),x+offset_x,y); Text(_rp(obj),temp,strlen(temp));

      break;

    default :
      for ( ligne=0;ligne<data->taille; ligne++)
      {
        for ( colonne=0;colonne<data->taille; colonne++ )
        {
          x = _mleft(obj)+(3*tf_x)*colonne;
          y = _mtop(obj)+(2*tf_y)*(ligne+1);
          sprintf(temp,"%3d",data->matrice[ligne][colonne]);

          Move(_rp(obj),x,y);
          Text(_rp(obj),temp,strlen(temp));

        }
      }
      break;
  }




  if ( ! data->shaking )
  {
    Move(_rp(obj),_mleft(obj)+(3*tf_x)*data->colonne,_mtop(obj)+(2*tf_y)*(data->ligne+1)-3*tf_x/2);
    Draw(_rp(obj),_mleft(obj)+(3*tf_x)*(data->colonne+2),_mtop(obj)+(2*tf_y)*(data->ligne+1)-3*tf_x/2);
    Draw(_rp(obj),_mleft(obj)+(3*tf_x)*(data->colonne+2),_mtop(obj)+(2*tf_y)*(data->ligne+2)+tf_x/2);
    Draw(_rp(obj),_mleft(obj)+(3*tf_x)*data->colonne,_mtop(obj)+(2*tf_y)*(data->ligne+2)+tf_x/2);
    Draw(_rp(obj),_mleft(obj)+(3*tf_x)*data->colonne,_mtop(obj)+(2*tf_y)*(data->ligne+1)-3*tf_x/2);
  }


  return(TRUE);

}
///

/// ULONG Madmatrix_Setup(struct IClass *cl,Object *obj,struct MUIP_HandleInput *msg)
/* init data & allocate pens */
IPTR Madmatrix_Setup(struct IClass *cl,Object *obj,struct MUIP_HandleInput *msg)
{
  if (!(DoSuperMethodA(cl,obj,(Msg)msg)))
    return(FALSE);

  MUI_RequestIDCMP(obj,IDCMP_RAWKEY);

  srand(time(NULL));

  return(TRUE);
}
///


/// ULONG Madmatrix_Set(struct IClass *cl,Object *obj,Msg msg)
IPTR Madmatrix_Set(struct IClass *cl,Object *obj,struct opSet *msg)
{
  struct Madmatrix_Data *data = INST_DATA(cl,obj);
  struct TagItem *tags,*tag;

  for (tags=msg->ops_AttrList;(tag=NextTagItem(&tags));)
  {

    switch (tag->ti_Tag)
    {
      case MADMATRIX_TAILLE:
        data->ntaille=tag->ti_Data;
        break;
      case MADMATRIX_GROUPE:
        data->groupe=(Object *)tag->ti_Data;
        break;

    }
  }
  return(DoSuperMethodA(cl,obj,msg));
}
///

/// ULONG Madmatrix_Get(struct IClass *cl,Object *obj,Msg msg)
IPTR Madmatrix_Get(struct IClass *cl,Object *obj,struct opGet *msg)
{
  struct Madmatrix_Data *data = INST_DATA(cl,obj);
  IPTR *store = msg->opg_Storage;

  switch (msg->opg_AttrID)
  {
    case MADMATRIX_TAILLE:
      *store = data->taille;
      return(TRUE);
  }

  return(DoSuperMethodA(cl,obj,msg));
}
///

/// ULONG Madmatrix_HandleInput(struct IClass *cl,Object *obj,struct MUIP_HandleInput *msg)
IPTR Madmatrix_HandleInput(struct IClass *cl,Object *obj,struct MUIP_HandleInput *msg)
{
  struct Madmatrix_Data *data = INST_DATA(cl,obj);

  if (msg->muikey!=MUIKEY_NONE)
  {
    switch (msg->muikey)
    {
      case MUIKEY_UP :
        data->box=-1;
        MUI_Redraw(obj,MADF_DRAWUPDATE);
        if ( data->ligne>0 )
          data->ligne--;
        data->box=1;
        MUI_Redraw(obj,MADF_DRAWUPDATE);
        break;

      case MUIKEY_DOWN :
        data->box=-1;
        MUI_Redraw(obj,MADF_DRAWUPDATE);
        if ( data->ligne<data->taille-2 )
          data->ligne++;
        data->box=1;
        MUI_Redraw(obj,MADF_DRAWUPDATE);
        break;

      case MUIKEY_RIGHT :
        data->box=-1;
        MUI_Redraw(obj,MADF_DRAWUPDATE);
        if ( data->colonne<data->taille-2 )
          data->colonne++;
        data->box=1;
        MUI_Redraw(obj,MADF_DRAWUPDATE);
        break;

      case MUIKEY_LEFT :
        data->box=-1;
        MUI_Redraw(obj,MADF_DRAWUPDATE);
        if ( data->colonne>0 )
          data->colonne--;
        data->box=1;
        MUI_Redraw(obj,MADF_DRAWUPDATE);
        break;

      case MUIKEY_PRESS :
        Madmatrix_Rotate(cl,obj, -1,PAS);
        /*MUI_Redraw(obj,MADF_DRAWUPDATE);*/
        break;
      case MUIKEY_TOGGLE :
        Madmatrix_Rotate(cl,obj, 1, PAS);
        /*MUI_Redraw(obj,MADF_DRAWUPDATE);*/
        break;
    }
  }

  if ( msg->imsg &&  msg->imsg->Class == IDCMP_RAWKEY  )
  {
    if ( msg->imsg->Code ==  24 ) /* 'o' key */
        Madmatrix_Rotate(cl,obj, -1,PAS);

    if ( msg->imsg->Code ==  25 ) /* 'p' key */
        Madmatrix_Rotate(cl,obj, 1,PAS);
  }
  return(DoSuperMethodA(cl,obj,(Msg)msg));
}
///

/// ULONG Madmatrix_Cleanup(struct IClass *cl,Object *obj,struct MUIP_HandleInput *msg)
IPTR Madmatrix_Cleanup(struct IClass *cl,Object *obj,struct MUIP_HandleInput *msg)
{
  MUI_RejectIDCMP(obj,IDCMP_RAWKEY);

  return(DoSuperMethodA(cl,obj,(Msg)msg));
}
///

/// ULONG __attribute__((regparm(3))) Madmatrix_Dispatcher(struct IClass *cl , void *msg,Object *obj  )

#ifndef __AROS__
__asm ULONG Madmatrix_Dispatcher(register __a0 struct IClass *cl, register __a2 Object *obj, register __a1 Msg msg)
#else
BOOPSI_DISPATCHER(IPTR, Madmatrix_Dispatcher, cl, obj, msg)
#endif
{
  struct Madmatrix_Data *data;

  data = INST_DATA(cl,obj);

  switch (msg->MethodID)
  {
    case OM_NEW           : return(Madmatrix_New(cl,obj,(APTR)msg));
    case OM_DISPOSE       : return(Madmatrix_Dispose(cl,obj,(APTR)msg));
    case OM_SET           : return(Madmatrix_Set(cl,obj,(APTR)msg));
    case OM_GET           : return(Madmatrix_Get(cl,obj,(APTR)msg));
    case MUIM_AskMinMax   : return(Madmatrix_AskMinMax (cl,obj,(struct MUIP_AskMinMax *)msg));
    case MUIM_Draw        : return(Madmatrix_Draw  (cl,obj,(struct MUIP_Draw *)msg));
    case MUIM_HandleInput : return(Madmatrix_HandleInput(cl,obj,(struct MUIP_HandleInput *)msg));
    case MUIM_Setup       : return(Madmatrix_Setup  (cl,obj,(struct MUIP_HandleInput *)msg));
    case MUIM_Cleanup     : return(Madmatrix_Cleanup (cl,obj,(struct MUIP_HandleInput *)msg));
    case MADMATRIX_SHAKE         :  Madmatrix_Shake(cl,obj,data->taille*data->taille-data->taille+1); return(TRUE);
    case MADMATRIX_RESTART       :  Madmatrix_Restart(cl,obj); return(TRUE);
  }



  return(DoSuperMethodA(cl,obj,(Msg)msg));
}
#ifdef __AROS__
BOOPSI_DISPATCHER_END
#endif
///





/// void Madmatrix_Rotate(struct IClass *cl,Object *obj, int sens, double pas)
/* init data & allocate pens */
void Madmatrix_Rotate(struct IClass *cl,Object *obj, int sens, double pas)
{
  struct Madmatrix_Data *data = INST_DATA(cl,obj);

  data->mouvement=sens;
  for ( data->pas=0; data->pas <= 1; data->pas+=pas )
  {
    WaitTOF();
    MUI_Redraw(obj,MADF_DRAWUPDATE);
  }

  Madmatrix_Tourne(data->matrice, data->ligne, data->colonne, sens);
  if ( data->shaked )
    if ( Madmatrix_Calcul(data->matrice,data->taille) == data->taille*data->taille )
      data->message="You win !";
  Madmatrix_Tourne(data->matrice, data->ligne, data->colonne, -sens);

  data->pas=1;
  MUI_Redraw(obj,MADF_DRAWUPDATE);
  data->mouvement=0;

  Madmatrix_Tourne(data->matrice, data->ligne, data->colonne, sens);
}
///

/// void Madmatrix_Shake(struct IClass *cl,Object *obj, int nb)
void Madmatrix_Shake(struct IClass *cl,Object *obj, int nb)
{
  struct Madmatrix_Data *data = INST_DATA(cl,obj);
  int cpt, sens=-1;

  data->box = -1;
  MUI_Redraw(obj,MADF_DRAWUPDATE);

  data->shaking=1;

  for ( cpt=0; cpt<nb; cpt++ )
  {
    data->ligne   = rand()%(data->taille-1);
    data->colonne = rand()%(data->taille-1);
    sens=-sens;

    Madmatrix_Rotate(cl,obj,sens,3*PAS);

  }
  data->shaking=0;
  data->shaked=1;
  data->ligne=0;
  data->colonne=0;

  data->box = 1;
  data->message=MADMATRIX_GOODLUCK;
  MUI_Redraw(obj,MADF_DRAWUPDATE);

}
///

/// void Madmatrix_Init(struct Madmatrix_Data *data)
void Madmatrix_Init(struct Madmatrix_Data *data)
{
  int ligne, colonne,nb=1;

  for ( ligne=0; ligne<data->taille; ligne++ )
    for ( colonne=0; colonne<data->taille; colonne++ )
      data->matrice[ligne][colonne]=nb++;

  data->ligne   = 0;
  data->colonne = 0;

  data->mouvement=0;
  data->shaking=0;

}
///

/// void Madmatrix_Restart(struct IClass *cl,Object *obj)
void Madmatrix_Restart(struct IClass *cl,Object *obj)
{
  struct Madmatrix_Data *data = INST_DATA(cl,obj);


  if ( data->ntaille > MADMATRIX_TAILLE_MAX )
    data->ntaille = MADMATRIX_TAILLE_MAX;

  if ( data->ntaille < 3 )
    data->ntaille=3;

  if ( data->taille != data->ntaille )
  {
    if ( data->groupe )
    {
      if (DoMethod(data->groupe,MUIM_Group_InitChange))
      {
        FreeTab((void **)data->matrice,data->taille);
        data->matrice = NULL;

        DoMethod(data->groupe,OM_REMMEMBER,obj);
        data->taille=data->ntaille;
        DoMethod(data->groupe,OM_ADDMEMBER,obj);

        DoMethod(data->groupe,MUIM_Group_ExitChange);
      }
    }
    else
    {
      FreeTab((void **)data->matrice,data->taille);
      data->matrice = NULL;
      data->taille=data->ntaille;
    }


    data->matrice=(int **)AllocTab(sizeof(int),data->taille,data->taille);
  }

  Madmatrix_Init(data);

  data->shaked = 0;

  MUI_Redraw(obj,MADF_DRAWOBJECT);
}
///


/// void Madmatrix_Tourne(int **tableau,int ligne, int colonne, int sens )
void Madmatrix_Tourne(int **tableau,int ligne, int colonne, int sens )
{
  int temp=tableau[ligne   ][colonne   ];

  if ( sens>=0 )
  {
    tableau[ligne   ][colonne   ]   = tableau[ligne+1 ][colonne   ];
    tableau[ligne+1 ][colonne   ]   = tableau[ligne+1 ][colonne+1 ];
    tableau[ligne+1 ][colonne+1 ]   = tableau[ligne   ][colonne+1 ];
    tableau[ligne   ][colonne+1 ]   = temp;
  }
  else
  {
    tableau[ligne   ][colonne   ]   = tableau[ligne   ][colonne+1 ];
    tableau[ligne   ][colonne+1 ]   = tableau[ligne+1 ][colonne+1 ];
    tableau[ligne+1 ][colonne+1 ]   = tableau[ligne+1 ][colonne   ];
    tableau[ligne+1 ][colonne   ]   = temp;
  }


}
///


/// int Madmatrix_Calcul(int **tableau, int taille)
int Madmatrix_Calcul(int **tableau, int taille)
{
  int ligne, colonne;
  int nb=0;


  for ( ligne=0; ligne<taille; ligne++ )
    for ( colonne=0; colonne<taille; colonne++ )
    {
      if ( tableau[ligne][colonne] == MADMATRIX_NOMBRE(taille,ligne,colonne) )
        nb++;
    }

  return(nb);
}
///

