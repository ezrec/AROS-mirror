/*********************************************************************
----------------------------------------------------------------------

	guigfx_convolve

----------------------------------------------------------------------
*********************************************************************/

#include "guigfx_global.h"


#define BORDERMODE_WRAP			0
#define	BORDERMODE_CONTINOUS	1
#define	BORDERMODE_FIXEDCOLOR	2



typedef struct Kernel
{
	float	colorkernel[3*4];

	int		*spatkernel;

	int		spatwidth, spatheight;
	
	int		bordermode;
	
	ULONG	fixedcolor;
	
	int		divisor;
	int		increment;

} KERNEL;


extern ULONG PIC_Convolve(PIC *pic, KERNEL *kernel, TAGLIST taglist);
