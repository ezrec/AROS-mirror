#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>

#ifdef __AROS__
#include <aros/asmcall.h>
#endif

#define MADMATRIX_TAILLE_MAX 6

/* I know this is ugly ( random values...) */
#define MADMATRIX_TAILLE 566774
#define MADMATRIX_GROUPE 566775

#define MADMATRIX_SHAKE 257180
#define MADMATRIX_RESTART 257181

struct Madmatrix_Data
{
  int taille;
  int ntaille;

  int **matrice;
  int ligne,colonne;

  int font_sx, font_sy;

  Object *groupe;

  int shaking; /* 0 ou 1 */
  int shaked; /* 0 ou 1 */

  int mouvement; /* -1,0 ou 1 */
  double pas;

  int box; /* -1 0 ou 1 */

  char *message;

};

#ifndef __AROS__
__asm ULONG Madmatrix_Dispatcher(register __a0 struct IClass *cl, register __a2 Object *obj, register __a1 Msg msg);
#else
AROS_UFH3(IPTR, Madmatrix_Dispatcher,
	AROS_UFHA(Class  *, cl,  A0),
	AROS_UFHA(Object *, obj, A2),
	AROS_UFHA(Msg     , msg, A1));
#endif

