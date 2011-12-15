/****************************************************************************
*									    *
* Gfx.c 							     970319 *
*									    *
* This is where all Graphics functions belong to			    *
*									    *
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <proto/graphics.h>

#include "MyTypes.h"
#include "Level.h"
#include "MyMath.h"
#include "Init.h"
#include "Gfx.h"
#include "Timer.h"

/*
#define DEBUG
*/

void WaitKey(void);

void SwitchDoubleBuffer(void)
{
    /* Move screen and change MyRastPort */
}

Pol3D ClipPol3D[MaxRecursion];

int32 MyWidth=Width;
int32 MyHeight=Height;
int32 HalfWidth=Width/2;
int32 HalfHeight=Height/2;
int32 QuaterWidth=Width/4;
int32 QuaterHeight=Height/4;
fix   LightLess=DefLightLess;
fix   CameraBehind=DefCameraBehind;
int16 MaxRecLevel=MaxRecursion-2;
int16 Finished = !(0==0);

int16 Frames = 0;
fix   StartTime = 0.0;
fix   TStartTime= 0.0;
fix   EndTime	= 0.0;
fix   RenderTime= 0.0;
fix   WPA8Time	= 0.0;
fix   TotalTime = 0.0;

fix Min4fix(fix a, fix b, fix c, fix d)
{
    if (a<b)
	if (a<c)
	    if (a<d)
		return a;
	    else
		return d;
	else if (c<d)
		return c;
	    else
		return d;
    else if (b<c)
	    if (b<d)
		return b;
	    else
		return d;
	else if (c<d)
		return c;
	    else
		return d;
}

fix Max4fix(fix a, fix b, fix c, fix d)
{
    if (a>b)
	if (a>c)
	    if (a>d)
		return a;
	    else
		return d;
	else if (c>d)
		return c;
	    else
		return d;
    else if (b>c)
	    if (b>d)
		return b;
	    else
		return d;
	else if (c>d)
		return c;
	    else
		return d;
}

void ClipPol3DZ(pPol3D Pol1, pPol3D Pol2, fix Z)
{
    /* Clip Polygon to Z */

    int a;
    fix z;

    for (a=0;a<Pol1->NumPoints;a++)
    {
	if (Pol1->Pnts[a].z>=Z) /* Is Pnts[a] inside? */
	{			/* Add Pnt to Polygon */
	    Pol2->Pnts[Pol2->NumPoints].x=Pol1->Pnts[a].x;
	    Pol2->Pnts[Pol2->NumPoints].y=Pol1->Pnts[a].y;
	    Pol2->Pnts[Pol2->NumPoints].z=Pol1->Pnts[a].z;
	    Pol2->uvl1[Pol2->NumPoints].x=Pol1->uvl1[a].x;
	    Pol2->uvl1[Pol2->NumPoints].y=Pol1->uvl1[a].y;
	    Pol2->uvl1[Pol2->NumPoints].z=Pol1->uvl1[a].z;
	    Pol2->uvl2[Pol2->NumPoints].x=Pol1->uvl2[a].x;
	    Pol2->uvl2[Pol2->NumPoints].y=Pol1->uvl2[a].y;
	    Pol2->uvl2[Pol2->NumPoints].z=Pol1->uvl2[a].z;
	    Pol2->NumPoints++; /* Check if we have 20 points To be considered */

	    /* Check if the next point is outside */

	    if (Pol1->Pnts[a+1].z<Z)
	    {	/* Clip Line */
		z=Pol1->Pnts[a].z - Pol1->Pnts[a+1].z;
		z=(Pol1->Pnts[a].z-Z)/z;  /* z != 0 since points lie on different sides */

		Pol2->Pnts[Pol2->NumPoints].x= Pol1->Pnts[a].x + (Pol1->Pnts[a+1].x - Pol1->Pnts[a].x)*z;
		Pol2->Pnts[Pol2->NumPoints].y= Pol1->Pnts[a].y + (Pol1->Pnts[a+1].y - Pol1->Pnts[a].y)*z;
		Pol2->Pnts[Pol2->NumPoints].z= Z /* Pol1->Pnts[a].z + (Pol1->Pnts[a+1].z - Pol1->Pnts[a].z)*z */;
		Pol2->uvl1[Pol2->NumPoints].x = Pol1->uvl1[a].x  + (Pol1->uvl1[a+1].x  - Pol1->uvl1[a].x )*z;
		Pol2->uvl1[Pol2->NumPoints].y = Pol1->uvl1[a].y  + (Pol1->uvl1[a+1].y  - Pol1->uvl1[a].y )*z;
		Pol2->uvl1[Pol2->NumPoints].z = Pol1->uvl1[a].z  + (Pol1->uvl1[a+1].z  - Pol1->uvl1[a].z )*z;
		Pol2->uvl2[Pol2->NumPoints].x = Pol1->uvl2[a].x  + (Pol1->uvl2[a+1].x  - Pol1->uvl2[a].x )*z;
		Pol2->uvl2[Pol2->NumPoints].y = Pol1->uvl2[a].y  + (Pol1->uvl2[a+1].y  - Pol1->uvl2[a].y )*z;
		Pol2->uvl2[Pol2->NumPoints].z = Pol1->uvl2[a].z  + (Pol1->uvl2[a+1].z  - Pol1->uvl2[a].z )*z;
		Pol2->NumPoints++; /* Check if we have 20 points To be consdered */
	    }
	}
	else if (Pol1->Pnts[a+1].z>=Z) /* Check if next Point is inside */
	{ /* Clip Line */
	    z=Pol1->Pnts[a].z - Pol1->Pnts[a+1].z;
	    z=(Pol1->Pnts[a].z-Z)/z;  /* z != 0 since points lie on different sides */

	    Pol2->Pnts[Pol2->NumPoints].x= Pol1->Pnts[a].x + (Pol1->Pnts[a+1].x - Pol1->Pnts[a].x)*z;
	    Pol2->Pnts[Pol2->NumPoints].y= Pol1->Pnts[a].y + (Pol1->Pnts[a+1].y - Pol1->Pnts[a].y)*z;
	    Pol2->Pnts[Pol2->NumPoints].z= Z /* Pol1->Pnts[a].z + (Pol1->Pnts[a+1].z - Pol1->Pnts[a].z)*z */;
	    Pol2->uvl1[Pol2->NumPoints].x = Pol1->uvl1[a].x  + (Pol1->uvl1[a+1].x  - Pol1->uvl1[a].x )*z;
	    Pol2->uvl1[Pol2->NumPoints].y = Pol1->uvl1[a].y  + (Pol1->uvl1[a+1].y  - Pol1->uvl1[a].y )*z;
	    Pol2->uvl1[Pol2->NumPoints].z = Pol1->uvl1[a].z  + (Pol1->uvl1[a+1].z  - Pol1->uvl1[a].z )*z;
	    Pol2->uvl2[Pol2->NumPoints].x = Pol1->uvl2[a].x  + (Pol1->uvl2[a+1].x  - Pol1->uvl2[a].x )*z;
	    Pol2->uvl2[Pol2->NumPoints].y = Pol1->uvl2[a].y  + (Pol1->uvl2[a+1].y  - Pol1->uvl2[a].y )*z;
	    Pol2->uvl2[Pol2->NumPoints].z = Pol1->uvl2[a].z  + (Pol1->uvl2[a+1].z  - Pol1->uvl2[a].z )*z;
	    Pol2->NumPoints++; /* Check if we have 20 points To be consdered */
	}

    }
    /* Add the first point again to close Polygon */
    Pol2->Pnts[Pol2->NumPoints].x = Pol2->Pnts[0].x;
    Pol2->Pnts[Pol2->NumPoints].y = Pol2->Pnts[0].y;
    Pol2->Pnts[Pol2->NumPoints].z = Pol2->Pnts[0].z;
    Pol2->uvl1[Pol2->NumPoints].x  = Pol2->uvl1[0].x;
    Pol2->uvl1[Pol2->NumPoints].y  = Pol2->uvl1[0].y;
    Pol2->uvl1[Pol2->NumPoints].z  = Pol2->uvl1[0].z;
    Pol2->uvl2[Pol2->NumPoints].x  = Pol2->uvl2[0].x;
    Pol2->uvl2[Pol2->NumPoints].y  = Pol2->uvl2[0].y;
    Pol2->uvl2[Pol2->NumPoints].z  = Pol2->uvl2[0].z;
}

void ClipPolygon(pPol3D MyPol1,pPol3D MyPol2, Vct * VctX, Vct * VctY, Vct * Inside)
{
    /* Clip Polygon to the side giben by VctX and VctY */

    Vct   Va,Vb,Vc;
    Vct   UVLa1,UVLa2,UVLb1,UVLb2;
    int16 a,n;
    fix   d,t;
    int16 In,Fail;

    /*
    printf("\nClip Polygon\n");

    printf("%f %f %f , %f %f %f\n",VctX->x,VctX->y,VctX->z,VctY->x,VctY->y,VctY->z);
    */

    MyPol2->NumPoints = 0;
    n=0;

    /* Start Inside */

    Va.x    = Inside->x;
    Va.y    = Inside->y;
    Va.z    = Inside->z;
    UVLa1.x = 0;
    UVLa1.y = 0;
    UVLa1.z = 0;
    UVLa2.x = 0;
    UVLa2.y = 0;
    UVLa2.z = 0;

    In = (0==0);


    d = VctX->x * VctY->y * Inside->z + VctX->y * VctY->z * Inside->x + VctX->z * VctY->x * Inside->y
      - VctX->x * VctY->z * Inside->y - VctX->y * VctY->x * Inside->z - VctX->z * VctY->y * Inside->x;

    if (d==0)
    {
	/*
	printf("Reference Point lies on the Plain!\n");
	*/
	MyPol2->NumPoints=1;
    }
    else
    {
	Fail = !(0==0);

	for (a=0;a <= MyPol1->NumPoints;a++)
	{
	    /* Get next Point */

	    Vb.x = MyPol1->Pnts[a].x;
	    Vb.y = MyPol1->Pnts[a].y;
	    Vb.z = MyPol1->Pnts[a].z;
	    UVLb1.x = MyPol1->uvl1[a].x;
	    UVLb1.y = MyPol1->uvl1[a].y;
	    UVLb1.z = MyPol1->uvl1[a].z;
	    UVLb2.x = MyPol1->uvl2[a].x;
	    UVLb2.y = MyPol1->uvl2[a].y;
	    UVLb2.z = MyPol1->uvl2[a].z;

	    /* Make Vector from Reference to Point */

	    VctSub(&Vb,Inside,&Vc);

	    /* Calculate Intersection with Plane */

	    d = VctX->x * VctY->y * Vc.z + VctX->y * VctY->z * Vc.x + VctX->z * VctY->x * Vc.y
	      - VctX->x * VctY->z * Vc.y - VctX->y * VctY->x * Vc.z - VctX->z * VctY->y * Vc.x;

    #ifdef DEBUG
	    printf("%f %f %f -> %f %f %f : %f %d\n",Inside->x,Inside->y,Inside->z,Vb.x,Vb.y,Vb.z,d,In);
    #endif

	    if (d == 0) /* Line parallel to Side -> Vb is inside */
	    {
		if (!In) /* Are we Outside ? */
		{
		    /* Calculate Intersection */

		    /* Make Vector */

		    VctSub(&Vb,&Va,&Vc);

		    /* Intersection with Plane */

		    d = VctX->x * VctY->y * Vc.z + VctX->y * VctY->z * Vc.x + VctX->z * VctY->x * Vc.y
		      - VctX->x * VctY->z * Vc.y - VctX->y * VctY->x * Vc.z - VctX->z * VctY->y * Vc.x;

    #ifdef DEBUG
		    printf("%f %f %f -> %f %f %f : %f %d\n",Va.x,Va.y,Va.z,Vb.x,Vb.y,Vb.z,d,In);
    #endif

		    if (d==0) /* Also parallel to Plane - rounding error? - just to be save */
		    {
			/* Take point halfway inbetween */
			/* The distance is probably so short that it doesn´t matter anyway */

			VctAdd(&Va,&Vb,&Vc);
			VctMulFix(&Vc,0.5,&Vc);

			MyPol2->Pnts[n].x=Vc.x;
			MyPol2->Pnts[n].y=Vc.y;
			MyPol2->Pnts[n].z=Vc.z;

    #ifdef DEBUG
			printf("Inserted %d %f %f %f\n",n,Vc.x,Vc.y,Vc.z);
    #endif

			VctAdd(&UVLa1,&UVLb1,&Vc);
			VctMulFix(&Vc,0.5,&Vc);
			MyPol2->uvl1[n].x = Vc.x;
			MyPol2->uvl1[n].y = Vc.y;
			MyPol2->uvl1[n].z = Vc.z;

			VctSub(&UVLa2,&UVLb2,&Vc);
			VctMulFix(&Vc,0.5,&Vc);
			MyPol2->uvl2[n].x = Vc.x;
			MyPol2->uvl2[n].y = Vc.y;
			MyPol2->uvl2[n].z = Vc.z;

			if (n>1)
			{
			    VctSub(&MyPol2->Pnts[n],&MyPol2->Pnts[n-1],&Vc);
			    if ((Vc.x < -0.00001) || (Vc.x > 0.00001))
			    {
				n++;
			    }
			    else if ((Vc.y < -0.00001) || (Vc.y > 0.00001))
			    {
				n++;
			    }
			    else if ((Vc.z < -0.00001) || (Vc.z > 0.00001))
			    {
				n++;
			    }
			}
			else
			{
			    n++;
			}
		    }
		    else
		    {
			t = VctX->x * VctY->y * Va.z + VctX->y * VctY->z * Va.x + VctX->z * VctY->x * Va.y
			  - VctX->x * VctY->z * Va.y - VctX->y * VctY->x * Va.z - VctX->z * VctY->y * Va.x;

			t = -t/d;

    #ifdef DEBUG
			printf("%f\n",t);
    #endif

			if (t != 1)
			{
			    VctMulFix(&Vc,t,&Vc);
			    VctAdd(&Va,&Vc,&Vc);

			    MyPol2->Pnts[n].x=Vc.x;
			    MyPol2->Pnts[n].y=Vc.y;
			    MyPol2->Pnts[n].z=Vc.z;

    #ifdef DEBUG
			    printf("Inserted %d %f %f %f\n",n,Vc.x,Vc.y,Vc.z);
    #endif

			    VctSub(&UVLb1,&UVLa1,&Vc); /* Texture Vector */
			    VctMulFix(&Vc,t,&Vc);      /* Texture Vector to intersection */
			    VctAdd(&UVLa1,&Vc,&Vc);    /* Texture at intersection */
			    MyPol2->uvl1[n].x = Vc.x;
			    MyPol2->uvl1[n].y = Vc.y;
			    MyPol2->uvl1[n].z = Vc.z;

			    VctSub(&UVLb2,&UVLa2,&Vc); /* Texture Vector */
			    VctMulFix(&Vc,t,&Vc);      /* Texture Vector to intersection */
			    VctAdd(&UVLa2,&Vc,&Vc);    /* Texture at intersection */
			    MyPol2->uvl2[n].x = Vc.x;
			    MyPol2->uvl2[n].y = Vc.y;
			    MyPol2->uvl2[n].z = Vc.z;

			    if (n>1)
			    {
				VctSub(&MyPol2->Pnts[n],&MyPol2->Pnts[n-1],&Vc);
				if ((Vc.x < -0.00001) || (Vc.x > 0.00001))
				{
				    n++;
				}
				else if ((Vc.y < -0.00001) || (Vc.y > 0.00001))
				{
				    n++;
				}
				else if ((Vc.z < -0.00001) || (Vc.z > 0.00001))
				{
				    n++;
				}
			    }
			    else
			    {
				n++;
			    }
			}
		    }

		    /* We are Inside now */

		    In = (0==0);
		}

		/* Insert Point */

    #ifdef DEBUG
		printf("Inserted %d %f %f %f\n",n,Vb.x,Vb.y,Vb.z);
    #endif

		MyPol2->Pnts[n].x = Vb.x;
		MyPol2->Pnts[n].y = Vb.y;
		MyPol2->Pnts[n].z = Vb.z;
		MyPol2->uvl1[n].x = UVLb1.x;
		MyPol2->uvl1[n].y = UVLb1.y;
		MyPol2->uvl1[n].z = UVLb1.z;
		MyPol2->uvl2[n].x = UVLb2.x;
		MyPol2->uvl2[n].y = UVLb2.y;
		MyPol2->uvl2[n].z = UVLb2.z;

		if (n>1)
		{
		    VctSub(&MyPol2->Pnts[n],&MyPol2->Pnts[n-1],&Vc);
		    if ((Vc.x < -0.00001) || (Vc.x > 0.00001))
		    {
			n++;
		    }
		    else if ((Vc.y < -0.00001) || (Vc.y > 0.00001))
		    {
			n++;
		    }
		    else if ((Vc.z < -0.00001) || (Vc.z > 0.00001))
		    {
			n++;
		    }
		}
		else
		{
		    n++;
		}
	    }
	    else /* Calculate intersection */
	    {
		t = VctX->x * VctY->y * Inside->z + VctX->y * VctY->z * Inside->x + VctX->z * VctY->x * Inside->y
		  - VctX->x * VctY->z * Inside->y - VctX->y * VctY->x * Inside->z - VctX->z * VctY->y * Inside->x;

		t = -t/d;

    #ifdef DEBUG
		printf("%f\n",t);
    #endif

		if (t == 0)
		{
		    /* Point of Reference lies on Plane */

		    printf("Refference Point lies on Plane!");
		    Fail = (0==0);
		}
		else if ((t > 0.99999) && (t < 1.00001))
		{
		    /* Point on Plane */

    #ifdef DEBUG
		    printf("Inserted %d %f %f %f\n",n,Vb.x,Vb.y,Vb.z);
    #endif

		    MyPol2->Pnts[n].x = Vb.x;
		    MyPol2->Pnts[n].y = Vb.y;
		    MyPol2->Pnts[n].z = Vb.z;
		    MyPol2->uvl1[n].x = UVLb1.x;
		    MyPol2->uvl1[n].y = UVLb1.y;
		    MyPol2->uvl1[n].z = UVLb1.z;
		    MyPol2->uvl2[n].x = UVLb2.x;
		    MyPol2->uvl2[n].y = UVLb2.y;
		    MyPol2->uvl2[n].z = UVLb2.z;

		    if (n>1)
		    {
			VctSub(&MyPol2->Pnts[n],&MyPol2->Pnts[n-1],&Vc);
			if ((Vc.x < -0.00001) || (Vc.x > 0.00001))
			{
			    n++;
			}
			else if ((Vc.y < -0.00001) || (Vc.y > 0.00001))
			{
			    n++;
			}
			else if ((Vc.z < -0.00001) || (Vc.z > 0.00001))
			{
			    n++;
			}
		    }
		    else
		    {
			n++;
		    }


		    In = (0==0);
		}
		else if ((t > 0) && (t < 1))
		{
		    /* Vb is Outside */

		    if (In) /* ARe we Inside ? */
		    {
			/* Calculate Intersection */

			/* Make Vector */

			VctSub(&Vb,&Va,&Vc);

			/* Intersection with Plane */

			d = VctX->x * VctY->y * Vc.z + VctX->y * VctY->z * Vc.x + VctX->z * VctY->x * Vc.y
			  - VctX->x * VctY->z * Vc.y - VctX->y * VctY->x * Vc.z - VctX->z * VctY->y * Vc.x;


    #ifdef DEBUG
			printf("%f %f %f -> %f %f %f : %f %d\n",Va.x,Va.y,Va.z,Vb.x,Vb.y,Vb.z,d,In);
    #endif

			if (d==0) /* Parallel to Plane - rounding error? - just to be save */
			{
			    /* Take point halfway inbetween */
			    /* The distance is probably so short that it doesn´t matter anyway */

			    VctAdd(&Va,&Vb,&Vc);
			    VctMulFix(&Vc,0.5,&Vc);

			    MyPol2->Pnts[n].x=Vc.x;
			    MyPol2->Pnts[n].y=Vc.y;
			    MyPol2->Pnts[n].z=Vc.z;

    #ifdef DEBUG
			    printf("Inserted %d %f %f %f\n",n,Vc.x,Vc.y,Vc.z);
    #endif

			    VctAdd(&UVLa1,&UVLb1,&Vc);
			    VctMulFix(&Vc,0.5,&Vc);
			    MyPol2->uvl1[n].x = Vc.x;
			    MyPol2->uvl1[n].y = Vc.y;
			    MyPol2->uvl1[n].z = Vc.z;

			    VctSub(&UVLa2,&UVLb2,&Vc);
			    VctMulFix(&Vc,0.5,&Vc);
			    MyPol2->uvl2[n].x = Vc.x;
			    MyPol2->uvl2[n].y = Vc.y;
			    MyPol2->uvl2[n].z = Vc.z;

			    if (n>1)
			    {
				VctSub(&MyPol2->Pnts[n],&MyPol2->Pnts[n-1],&Vc);
				if ((Vc.x < -0.00001) || (Vc.x > 0.00001))
				{
				    n++;
				}
				else if ((Vc.y < -0.00001) || (Vc.y > 0.00001))
				{
				    n++;
				}
				else if ((Vc.z < -0.00001) || (Vc.z > 0.00001))
				{
				    n++;
				}
			    }
			    else
			    {
				n++;
			    }

			}
			else
			{
			    t = VctX->x * VctY->y * Va.z + VctX->y * VctY->z * Va.x + VctX->z * VctY->x * Va.y
			      - VctX->x * VctY->z * Va.y - VctX->y * VctY->x * Va.z - VctX->z * VctY->y * Va.x;

			    t = -t/d;

    #ifdef DEBUG
			    printf("%f\n",t);
    #endif
			    if (t != 0)
			    {
				VctMulFix(&Vc,t,&Vc);
				VctAdd(&Vc,&Va,&Vc);

				MyPol2->Pnts[n].x=Vc.x;
				MyPol2->Pnts[n].y=Vc.y;
				MyPol2->Pnts[n].z=Vc.z;

    #ifdef DEBUG
				printf("Inserted %d %f %f %f\n",n,Vc.x,Vc.y,Vc.z);
    #endif

				VctSub(&UVLb1,&UVLa1,&Vc); /* Texture Vector */
				VctMulFix(&Vc,t,&Vc);      /* Texture Vector to intersection */
				VctAdd(&UVLa1,&Vc,&Vc);    /* Texture at intersection */
				MyPol2->uvl1[n].x = Vc.x;
				MyPol2->uvl1[n].y = Vc.y;
				MyPol2->uvl1[n].z = Vc.z;

				VctSub(&UVLb2,&UVLa2,&Vc); /* Texture Vector */
				VctMulFix(&Vc,t,&Vc);      /* Texture Vector to intersection */
				VctAdd(&UVLa2,&Vc,&Vc);    /* Texture at intersection */
				MyPol2->uvl2[n].x = Vc.x;
				MyPol2->uvl2[n].y = Vc.y;
				MyPol2->uvl2[n].z = Vc.z;

				if (n>1)
				{
				    VctSub(&MyPol2->Pnts[n],&MyPol2->Pnts[n-1],&Vc);
				    if ((Vc.x < -0.00001) || (Vc.x > 0.00001))
				    {
					n++;
				    }
				    else if ((Vc.y < -0.00001) || (Vc.y > 0.00001))
				    {
					n++;
				    }
				    else if ((Vc.z < -0.00001) || (Vc.z > 0.00001))
				    {
					n++;
				    }
				}
				else
				{
				    n++;
				}
			    }
			}

			/* We are Outside now */

			In = !(0==0);
		    }
		}
		else
		{
		    /* Vb is inside */

		    if (!In) /* Are we Outside */
		    {
			/* Caclulate Intersection */

			/* Make Vector */

			VctSub(&Vb,&Va,&Vc);

			/* Intersection with Plane */

			d = VctX->x * VctY->y * Vc.z + VctX->y * VctY->z * Vc.x + VctX->z * VctY->x * Vc.y
			  - VctX->x * VctY->z * Vc.y - VctX->y * VctY->x * Vc.z - VctX->z * VctY->y * Vc.x;

    #ifdef DEBUG
			printf("%f %f %f -> %f %f %f : %f %d\n",Va.x,Va.y,Va.z,Vb.x,Vb.y,Vb.z,d,In);
    #endif

			if (d==0) /* Parallel to Plane - rounding error? - just to be save */
			{
			    /* Take point halfway inbetween */
			    /* The distance is probably so short that it doesn´t matter anyway */

			    VctAdd(&Va,&Vb,&Vc);
			    VctMulFix(&Vc,0.5,&Vc);

			    MyPol2->Pnts[n].x=Vc.x;
			    MyPol2->Pnts[n].y=Vc.y;
			    MyPol2->Pnts[n].z=Vc.z;

    #ifdef DEBUG
			    printf("Inserted %d %f %f %f\n",n,Vc.x,Vc.y,Vc.z);
    #endif

			    VctAdd(&UVLa1,&UVLb1,&Vc);
			    VctMulFix(&Vc,0.5,&Vc);
			    MyPol2->uvl1[n].x = Vc.x;
			    MyPol2->uvl1[n].y = Vc.y;
			    MyPol2->uvl1[n].z = Vc.z;

			    VctSub(&UVLa2,&UVLb2,&Vc);
			    VctMulFix(&Vc,0.5,&Vc);
			    MyPol2->uvl2[n].x = Vc.x;
			    MyPol2->uvl2[n].y = Vc.y;
			    MyPol2->uvl2[n].z = Vc.z;

			    if (n>1)
			    {
				VctSub(&MyPol2->Pnts[n],&MyPol2->Pnts[n-1],&Vc);
				if ((Vc.x < -0.00001) || (Vc.x > 0.00001))
				{
				    n++;
				}
				else if ((Vc.y < -0.00001) || (Vc.y > 0.00001))
				{
				    n++;
				}
				else if ((Vc.z < -0.00001) || (Vc.z > 0.00001))
				{
				    n++;
				}
			    }
			    else
			    {
				n++;
			    }
			}
			else
			{
			    t = VctX->x * VctY->y * Va.z + VctX->y * VctY->z * Va.x + VctX->z * VctY->x * Va.y
			      - VctX->x * VctY->z * Va.y - VctX->y * VctY->x * Va.z - VctX->z * VctY->y * Va.x;

			    t = -t/d;

    #ifdef DEBUG
			    printf("%f\n",t);
    #endif

			    if (t != 1)
			    {
				VctMulFix(&Vc,t,&Vc);
				VctAdd(&Va,&Vc,&Vc);

				MyPol2->Pnts[n].x=Vc.x;
				MyPol2->Pnts[n].y=Vc.y;
				MyPol2->Pnts[n].z=Vc.z;

    #ifdef DEBUG
				printf("Inserted %d %f %f %f\n",n,Vc.x,Vc.y,Vc.z);
    #endif

				VctSub(&UVLb1,&UVLa1,&Vc); /* Texture Vector */
				VctMulFix(&Vc,t,&Vc);      /* Texture Vector to intersection */
				VctAdd(&UVLa1,&Vc,&Vc);    /* Texture at intersection */
				MyPol2->uvl1[n].x = Vc.x;
				MyPol2->uvl1[n].y = Vc.y;
				MyPol2->uvl1[n].z = Vc.z;

				VctSub(&UVLb2,&UVLa2,&Vc); /* Texture Vector */
				VctMulFix(&Vc,t,&Vc);      /* Texture Vector to intersection */
				VctAdd(&UVLa2,&Vc,&Vc);    /* Texture at intersection */
				MyPol2->uvl2[n].x = Vc.x;
				MyPol2->uvl2[n].y = Vc.y;
				MyPol2->uvl2[n].z = Vc.z;

				if (n>1)
				{
				    VctSub(&MyPol2->Pnts[n],&MyPol2->Pnts[n-1],&Vc);
				    if ((Vc.x < -0.00001) || (Vc.x > 0.00001))
				    {
					n++;
				    }
				    else if ((Vc.y < -0.00001) || (Vc.y > 0.00001))
				    {
					n++;
				    }
				    else if ((Vc.z < -0.00001) || (Vc.z > 0.00001))
				    {
					n++;
				    }
				}
				else
				{
				    n++;
				}
			    }
			}

			/* We are Inside now */

			In = (0==0);
		    }

		    /* insert Point */

    #ifdef DEBUG
		    printf("Inserted %d %f %f %f\n",n,Vb.x,Vb.y,Vb.z);
    #endif

		    MyPol2->Pnts[n].x = Vb.x;
		    MyPol2->Pnts[n].y = Vb.y;
		    MyPol2->Pnts[n].z = Vb.z;
		    MyPol2->uvl1[n].x = UVLb1.x;
		    MyPol2->uvl1[n].y = UVLb1.y;
		    MyPol2->uvl1[n].z = UVLb1.z;
		    MyPol2->uvl2[n].x = UVLb2.x;
		    MyPol2->uvl2[n].y = UVLb2.y;
		    MyPol2->uvl2[n].z = UVLb2.z;

		    if (n>1)
		    {
			VctSub(&MyPol2->Pnts[n],&MyPol2->Pnts[n-1],&Vc);
			if ((Vc.x < -0.00001) || (Vc.x > 0.00001))
			{
			    n++;
			}
			else if ((Vc.y < -0.00001) || (Vc.y > 0.00001))
			{
			    n++;
			}
			else if ((Vc.z < -0.00001) || (Vc.z > 0.00001))
			{
			    n++;
			}
		    }
		    else
		    {
			n++;
		    }
		}
	    }

	    /* Save Point for next turn */

	    Va.x = Vb.x;
	    Va.y = Vb.y;
	    Va.z = Vb.z;
	    UVLa1.x = UVLb1.x;
	    UVLa1.y = UVLb1.y;
	    UVLa1.z = UVLb1.z;
	    UVLa2.x = UVLb2.x;
	    UVLa2.y = UVLb2.y;
	    UVLa2.z = UVLb2.z;
	}

	MyPol2->NumPoints = n-1;

	/* First Point could be wrong, should be equal last Point */

	MyPol2->Pnts[0].x = MyPol2->Pnts[n-1].x;
	MyPol2->Pnts[0].y = MyPol2->Pnts[n-1].y;
	MyPol2->Pnts[0].z = MyPol2->Pnts[n-1].z;
	MyPol2->uvl1[0].x = MyPol2->uvl1[n-1].x;
	MyPol2->uvl1[0].y = MyPol2->uvl1[n-1].y;
	MyPol2->uvl1[0].z = MyPol2->uvl1[n-1].z;
	MyPol2->uvl2[0].x = MyPol2->uvl2[n-1].x;
	MyPol2->uvl2[0].y = MyPol2->uvl2[n-1].y;
	MyPol2->uvl2[0].z = MyPol2->uvl2[n-1].z;

	if (Fail)
	{
	    MyPol2->NumPoints=2;
	}
    }
}

int16 ClipSide(pBot MyPlayer,pSide MySide,int16 RecLevel,fix Light)
{
    Pol3D MyPol3D1,MyPol3D2;
    int16 a,b;
    Vct   Inside;

    /*
    printf("ClipSide\n");
    */

    /* Init Pol3D */

    Light*=MySide->Light;

    MyPol3D1.NumPoints=4;

    MyPol3D1.Pnts[0].x=MySide->Pnts[0]->x;
    MyPol3D1.Pnts[0].y=MySide->Pnts[0]->y;
    MyPol3D1.Pnts[0].z=MySide->Pnts[0]->z;
    MyPol3D1.uvl1[0].x=MySide->BackPnts[0].x;
    MyPol3D1.uvl1[0].y=MySide->BackPnts[0].y;
    MyPol3D1.uvl1[0].z=MySide->BackPnts[0].z*Light;
    MyPol3D1.uvl2[0].x=MySide->FrontPnts[0].x;
    MyPol3D1.uvl2[0].y=MySide->FrontPnts[0].y;
    MyPol3D1.uvl2[0].z=MySide->FrontPnts[0].z*Light;

    MyPol3D1.Pnts[1].x=MySide->Pnts[1]->x;
    MyPol3D1.Pnts[1].y=MySide->Pnts[1]->y;
    MyPol3D1.Pnts[1].z=MySide->Pnts[1]->z;
    MyPol3D1.uvl1[1].x=MySide->BackPnts[1].x;
    MyPol3D1.uvl1[1].y=MySide->BackPnts[1].y;
    MyPol3D1.uvl1[1].z=MySide->BackPnts[1].z*Light;
    MyPol3D1.uvl2[1].x=MySide->FrontPnts[1].x;
    MyPol3D1.uvl2[1].y=MySide->FrontPnts[1].y;
    MyPol3D1.uvl2[1].z=MySide->FrontPnts[1].z*Light;

    MyPol3D1.Pnts[2].x=MySide->Pnts[2]->x;
    MyPol3D1.Pnts[2].y=MySide->Pnts[2]->y;
    MyPol3D1.Pnts[2].z=MySide->Pnts[2]->z;
    MyPol3D1.uvl1[2].x=MySide->BackPnts[2].x;
    MyPol3D1.uvl1[2].y=MySide->BackPnts[2].y;
    MyPol3D1.uvl1[2].z=MySide->BackPnts[2].z*Light;
    MyPol3D1.uvl2[2].x=MySide->FrontPnts[2].x;
    MyPol3D1.uvl2[2].y=MySide->FrontPnts[2].y;
    MyPol3D1.uvl2[2].z=MySide->FrontPnts[2].z*Light;

    MyPol3D1.Pnts[3].x=MySide->Pnts[3]->x;
    MyPol3D1.Pnts[3].y=MySide->Pnts[3]->y;
    MyPol3D1.Pnts[3].z=MySide->Pnts[3]->z;
    MyPol3D1.uvl1[3].x=MySide->BackPnts[3].x;
    MyPol3D1.uvl1[3].y=MySide->BackPnts[3].y;
    MyPol3D1.uvl1[3].z=MySide->BackPnts[3].z*Light;
    MyPol3D1.uvl2[3].x=MySide->FrontPnts[3].x;
    MyPol3D1.uvl2[3].y=MySide->FrontPnts[3].y;
    MyPol3D1.uvl2[3].z=MySide->FrontPnts[3].z*Light;

/*
    if (MySide->Back != 0)
    {
	printf("\nOriginal Coords\n");
	for (a=0;a<MyPol3D1.NumPoints;a++)
	{
	    printf("%f %f %f (%f %f %f) (%f %f %f)\n",MyPol3D1.Pnts[a].x,MyPol3D1.Pnts[a].y,MyPol3D1.Pnts[a].z,
						      MyPol3D1.uvl1[a].x,MyPol3D1.uvl1[a].y,MyPol3D1.uvl1[a].z,
						      MyPol3D1.uvl2[a].x,MyPol3D1.uvl2[a].y,MyPol3D1.uvl2[a].z);
	}
    }
*/
    /* Transform to ships view */

    for (a=0;a<4;a++)
    {
	VctSub(&MyPol3D1.Pnts[a],&MyPlayer->Pos,&MyPol3D2.Pnts[a]);
	VctMulMat(&MyPol3D2.Pnts[a],&MyPlayer->Direction,&MyPol3D1.Pnts[a]);
	MyPol3D1.Pnts[a].z+=CameraBehind;
	/*
	printf("%f %f %f\n",MyPol3D1.Pnts[a].x,MyPol3D1.Pnts[a].y,MyPol3D1.Pnts[a].z);
	*/
    }
/*
    if (MySide->Back != 0)
    {
	printf("\nPolygon in ships Coords\n");
	for (a=0;a<MyPol3D1.NumPoints;a++)
	{
	     printf("%f %f %f (%f %f %f) (%f %f %f)\n",MyPol3D1.Pnts[a].x,MyPol3D1.Pnts[a].y,MyPol3D1.Pnts[a].z,
						       MyPol3D1.uvl1[a].x,MyPol3D1.uvl1[a].y,MyPol3D1.uvl1[a].z,
						       MyPol3D1.uvl2[a].x,MyPol3D1.uvl2[a].y,MyPol3D1.uvl2[a].z);
	}
    }
*/
    MyPol3D1.Pnts[4].x=MyPol3D1.Pnts[0].x; /* Last line should close the polygon */
    MyPol3D1.Pnts[4].y=MyPol3D1.Pnts[0].y;
    MyPol3D1.Pnts[4].z=MyPol3D1.Pnts[0].z;
    MyPol3D1.uvl1[4].x=MyPol3D1.uvl1[0].x;
    MyPol3D1.uvl1[4].y=MyPol3D1.uvl1[0].y;
    MyPol3D1.uvl1[4].z=MyPol3D1.uvl1[0].z;
    MyPol3D1.uvl2[4].x=MyPol3D1.uvl2[0].x;
    MyPol3D1.uvl2[4].y=MyPol3D1.uvl2[0].y;
    MyPol3D1.uvl2[4].z=MyPol3D1.uvl2[0].z;

    if (  Max4fix(MyPol3D1.Pnts[0].z,MyPol3D1.Pnts[1].z,MyPol3D1.Pnts[2].z,MyPol3D1.Pnts[3].z)
	< ClipPol3D[RecLevel].MinZ)
    {
	/*
	printf("ClipSide MaxFailed\n");
	*/
	return !(0==0);
    }
    else
    {

	/* Clip Polygon to ClipPol3D.MinZ */

	MyPol3D2.NumPoints=0;

	ClipPol3DZ( &MyPol3D1, &MyPol3D2, ClipPol3D[RecLevel].MinZ);

#ifdef DEBUG
	/* Print out Cliped Polygon */

	printf("\nClipped to Min Z = %f\n",ClipPol3D[RecLevel].MinZ);
	for (a=0;a<MyPol3D2.NumPoints;a++)
	{
	    printf("%f %f %f (%f %f %f) (%f %f %f)\n",MyPol3D2.Pnts[a].x,MyPol3D2.Pnts[a].y,MyPol3D2.Pnts[a].z,
						      MyPol3D2.uvl1[a].x,MyPol3D2.uvl1[a].y,MyPol3D2.uvl1[a].z,
						      MyPol3D2.uvl2[a].x,MyPol3D2.uvl2[a].y,MyPol3D2.uvl2[a].z);
	}
#endif

	if (MyPol3D2.NumPoints >2)  /* 3 Points needed for an Area */
	{

	    /* Insert Points to close Polygon */

	    ClipPol3D[RecLevel].Pnts[ClipPol3D[RecLevel].NumPoints].x = ClipPol3D[RecLevel].Pnts[0].x;
	    ClipPol3D[RecLevel].Pnts[ClipPol3D[RecLevel].NumPoints].y = ClipPol3D[RecLevel].Pnts[0].y;
	    ClipPol3D[RecLevel].Pnts[ClipPol3D[RecLevel].NumPoints].z = ClipPol3D[RecLevel].Pnts[0].z;
	    ClipPol3D[RecLevel].uvl1[ClipPol3D[RecLevel].NumPoints].x = ClipPol3D[RecLevel].uvl1[0].x;
	    ClipPol3D[RecLevel].uvl1[ClipPol3D[RecLevel].NumPoints].y = ClipPol3D[RecLevel].uvl1[0].y;
	    ClipPol3D[RecLevel].uvl1[ClipPol3D[RecLevel].NumPoints].z = ClipPol3D[RecLevel].uvl1[0].z;
	    ClipPol3D[RecLevel].uvl2[ClipPol3D[RecLevel].NumPoints].x = ClipPol3D[RecLevel].uvl2[0].x;
	    ClipPol3D[RecLevel].uvl2[ClipPol3D[RecLevel].NumPoints].y = ClipPol3D[RecLevel].uvl2[0].y;
	    ClipPol3D[RecLevel].uvl2[ClipPol3D[RecLevel].NumPoints].z = ClipPol3D[RecLevel].uvl2[0].z;

	    /* Calculate Mid Point */

	    Inside.x=0;
	    Inside.y=0;
	    Inside.z=0;

	    for (a=0;a<ClipPol3D[RecLevel].NumPoints;a++)
	    {
		VctAdd(&Inside,&ClipPol3D[RecLevel].Pnts[a],&Inside);
	    }

	    VctMulFix(&Inside,1/(fix)ClipPol3D[RecLevel].NumPoints,&Inside);

#ifdef DEBUG
	    /* Print clipping Polygone */

	    printf("Clipping to\n");

	    for (a=0;a<ClipPol3D[RecLevel].NumPoints+2;a++)
	    {
		printf("Clip %d %f %f %f\n",a,ClipPol3D[RecLevel].Pnts[a].x,ClipPol3D[RecLevel].Pnts[a].y,ClipPol3D[RecLevel].Pnts[a].z);
	    }
#endif

	    /* Clip Polygon */

	    for (a=0;a<ClipPol3D[RecLevel].NumPoints;a++)
	    {
		ClipPolygon(&MyPol3D2,&MyPol3D1,&ClipPol3D[RecLevel].Pnts[a],&ClipPol3D[RecLevel].Pnts[a+1],&Inside);

		if (MyPol3D1.NumPoints<3) /* Do we still have an area */
		{
		    a=ClipPol3D[RecLevel].NumPoints;
		}
		else
		{
#ifdef DEBUG
		    printf("%d\n",a);
#endif
		    for (b=0;b<=MyPol3D1.NumPoints;b++)
		    {
#ifdef DEBUG
			printf("Point %d : %f %f %f",b,MyPol3D2.Pnts[b].x,MyPol3D2.Pnts[b].y,MyPol3D2.Pnts[b].z);
			printf("--> %f %f %f\n",       MyPol3D1.Pnts[b].x,MyPol3D1.Pnts[b].y,MyPol3D1.Pnts[b].z);
#endif

			MyPol3D2.Pnts[b].x = MyPol3D1.Pnts[b].x;
			MyPol3D2.Pnts[b].y = MyPol3D1.Pnts[b].y;
			MyPol3D2.Pnts[b].z = MyPol3D1.Pnts[b].z;
			MyPol3D2.uvl1[b].x = MyPol3D1.uvl1[b].x;
			MyPol3D2.uvl1[b].y = MyPol3D1.uvl1[b].y;
			MyPol3D2.uvl1[b].z = MyPol3D1.uvl1[b].z;
			MyPol3D2.uvl2[b].x = MyPol3D1.uvl2[b].x;
			MyPol3D2.uvl2[b].y = MyPol3D1.uvl2[b].y;
			MyPol3D2.uvl2[b].z = MyPol3D1.uvl2[b].z;
		    }
		    MyPol3D2.NumPoints = MyPol3D1.NumPoints;
		}
	    }

	    if (MyPol3D1.NumPoints>2) /* Do we have ab Area */
	    {
		/* Save cliped Polygon and set ClipPol3D.MinZ */

		ClipPol3D[RecLevel+1].MinZ=MyPol3D2.Pnts[0].z;
		ClipPol3D[RecLevel+1].NumPoints = MyPol3D2.NumPoints;

		for (a=0;a<MyPol3D2.NumPoints;a++)
		{
		    ClipPol3D[RecLevel+1].Pnts[a].x = MyPol3D2.Pnts[a].x;
		    ClipPol3D[RecLevel+1].Pnts[a].y = MyPol3D2.Pnts[a].y;
		    ClipPol3D[RecLevel+1].Pnts[a].z = MyPol3D2.Pnts[a].z;
		    ClipPol3D[RecLevel+1].uvl1[a].x = MyPol3D2.uvl1[a].x;
		    ClipPol3D[RecLevel+1].uvl1[a].y = MyPol3D2.uvl1[a].y;
		    ClipPol3D[RecLevel+1].uvl1[a].z = MyPol3D2.uvl1[a].z;
		    ClipPol3D[RecLevel+1].uvl2[a].x = MyPol3D2.uvl2[a].x;
		    ClipPol3D[RecLevel+1].uvl2[a].y = MyPol3D2.uvl2[a].y;
		    ClipPol3D[RecLevel+1].uvl2[a].z = MyPol3D2.uvl2[a].z;

		    if (ClipPol3D[RecLevel+1].MinZ > MyPol3D2.Pnts[a].z)
		    {
			ClipPol3D[RecLevel+1].MinZ = MyPol3D2.Pnts[a].z;
		    }
		}

		return (0==0);
	    }
	    else /* Only <3 Points after Clipping */
	    {
		return !(0==0);
	    }
	}
	else /* Only <3 Points after ClipMinZ */
	{
	    return !(0==0);
	}
    }
}

void DrawLine(pPol2D MyPol2D, int16 n, pClip2D MyClip2D)
{
    /* Draw line n of Polygon into MyClip2D */

    fix   x,y,dx,dy,d;
    int16 t;
    int32 u1,v1,l1,u2,v2,l2,du1,dv1,dl1,du2,dv2,dl2;
    fix   D;

#ifdef DEBUG
    Move(MyRastPort,(int)MyPol2D->Pnts[n].x  ,(int)MyPol2D->Pnts[n].y);
    Draw(MyRastPort,(int)MyPol2D->Pnts[n+1].x,(int)MyPol2D->Pnts[n+1].y);
#endif

    x = MyPol2D->Pnts[n].x;
    y = MyPol2D->Pnts[n].y;
    dx = MyPol2D->Pnts[n+1].x - MyPol2D->Pnts[n].x;
    dy = MyPol2D->Pnts[n+1].y - MyPol2D->Pnts[n].y;

    d = (dx>-dx?dx:-dx) + (dy>-dy?dy:-dy) + 5;

    D = 1 / (fix)d;

    dx *= D;
    dy *= D;

    u1	=  MyPol2D->uvl1[n].x * DefTextureU1;
    v1	=  MyPol2D->uvl1[n].y * DefTextureV1;
    l1	=  MyPol2D->uvl1[n].z * DefTextureL1;
    u2	=  MyPol2D->uvl2[n].x * DefTextureU1;
    v2	=  MyPol2D->uvl2[n].y * DefTextureV1;
    l2	=  MyPol2D->uvl2[n].z * DefTextureL1;
    du1 = (MyPol2D->uvl1[n+1].x - MyPol2D->uvl1[n].x) * DefTextureU1 * D;
    dv1 = (MyPol2D->uvl1[n+1].y - MyPol2D->uvl1[n].y) * DefTextureV1 * D;
    dl1 = (MyPol2D->uvl1[n+1].z - MyPol2D->uvl1[n].z) * DefTextureL1 * D;
    du2 = (MyPol2D->uvl2[n+1].x - MyPol2D->uvl2[n].x) * DefTextureU1 * D;
    dv2 = (MyPol2D->uvl2[n+1].y - MyPol2D->uvl2[n].y) * DefTextureV1 * D;
    dl2 = (MyPol2D->uvl2[n+1].z - MyPol2D->uvl2[n].z) * DefTextureL1 * D;

    for (t=0;t<=d;t++)
    {
	if (y>=0)
	if (y<MyHeight)
	{
	    if (MyClip2D->MinX[(int16)y]>x)
	    {
		MyClip2D->MinX[(int16)y]=x;
		MyClip2D->MinUVL1[(int16)y].x=u1;
		MyClip2D->MinUVL1[(int16)y].y=v1;
		MyClip2D->MinUVL1[(int16)y].z=l1;
		MyClip2D->MinUVL2[(int16)y].x=u2;
		MyClip2D->MinUVL2[(int16)y].y=v2;
		MyClip2D->MinUVL2[(int16)y].z=l2;
	    }
	    if (MyClip2D->MaxX[(int16)y]<x)
	    {
		MyClip2D->MaxX[(int16)y]=x;
		MyClip2D->MaxUVL1[(int16)y].x=u1;
		MyClip2D->MaxUVL1[(int16)y].y=v1;
		MyClip2D->MaxUVL1[(int16)y].z=l1;
		MyClip2D->MaxUVL2[(int16)y].x=u2;
		MyClip2D->MaxUVL2[(int16)y].y=v2;
		MyClip2D->MaxUVL2[(int16)y].z=l2;
	    }
	}
	x+=dx;
	y+=dy;
	u1+=du1;
	v1+=dv1;
	l1+=dl1;
	u2+=du2;
	v2+=dv2;
	l2+=dl2;
    }
}

void DrawSide(pSide MySide,int16 RecLevel)
{
    /* Draw Side as Clipped to in ClipPol3D */

    int16  a,b,c;
    int16  MinY,MaxY;
    Clip2D MyClip2D;
    Pol2D  MyPol2D;
    fix    z;
    int32 d,u1,v1,l1,u2,v2,l2,du1,dv1,dl1,du2,dv2,dl2;
    int16  l;

    /* Transform to 2D and find MinY and MaxY */

    MinY=MyHeight;
    MaxY=0;

    MyPol2D.NumPoints = ClipPol3D[RecLevel].NumPoints;

    for (a=0;a<MyPol2D.NumPoints;a++)
    {
	z = 1.0 / ClipPol3D[RecLevel].Pnts[a].z;

	MyPol2D.Pnts[a].x = ClipPol3D[RecLevel].Pnts[a].x * z * HalfWidth + HalfWidth;
	MyPol2D.Pnts[a].y =-ClipPol3D[RecLevel].Pnts[a].y * z * HalfHeight+HalfHeight;

	/*
	printf("2DPoint %f %f %f --> %f %f\n",ClipPol3D[RecLevel].Pnts[a].x,ClipPol3D[RecLevel].Pnts[a].y,ClipPol3D[RecLevel].Pnts[a].z,
					      MyPol2D.Pnts[a].x,MyPol2D.Pnts[a].y);
	*/

	MyPol2D.uvl1[a].x = ClipPol3D[RecLevel].uvl1[a].x;
	MyPol2D.uvl1[a].y = ClipPol3D[RecLevel].uvl1[a].y;
	MyPol2D.uvl1[a].z = ClipPol3D[RecLevel].uvl1[a].z*pow(LightLess,1/z);
	MyPol2D.uvl2[a].x = ClipPol3D[RecLevel].uvl2[a].x;
	MyPol2D.uvl2[a].y = ClipPol3D[RecLevel].uvl2[a].y;
	MyPol2D.uvl2[a].z = ClipPol3D[RecLevel].uvl2[a].z*pow(LightLess,1/z);

	if (MyPol2D.Pnts[a].y < MinY)
	{
	    MinY = MyPol2D.Pnts[a].y;
	}
	if (MyPol2D.Pnts[a].y > MaxY)
	{
	    MaxY = MyPol2D.Pnts[a].y;
	}
    }

    if (MinY < 0) MinY = 0;
    if (MaxY >= Height) MaxY = Height -1;

    /* Init MinX and MaxX */

    for (a=MinY;a<=MaxY;a++)
    {
	MyClip2D.MinX[a]=MyWidth-1;
	MyClip2D.MaxX[a]=0;
    }

    /* Add Point to close Polygon */

    MyPol2D.Pnts[MyPol2D.NumPoints].x = MyPol2D.Pnts[0].x;
    MyPol2D.Pnts[MyPol2D.NumPoints].y = MyPol2D.Pnts[0].y;
    MyPol2D.Pnts[MyPol2D.NumPoints].z = MyPol2D.Pnts[0].z;
    MyPol2D.uvl1[MyPol2D.NumPoints].x = MyPol2D.uvl1[0].x;
    MyPol2D.uvl1[MyPol2D.NumPoints].y = MyPol2D.uvl1[0].y;
    MyPol2D.uvl1[MyPol2D.NumPoints].z = MyPol2D.uvl1[0].z;
    MyPol2D.uvl2[MyPol2D.NumPoints].x = MyPol2D.uvl2[0].x;
    MyPol2D.uvl2[MyPol2D.NumPoints].y = MyPol2D.uvl2[0].y;
    MyPol2D.uvl2[MyPol2D.NumPoints].z = MyPol2D.uvl2[0].z;

    /* Draw Lines */

    for (a=0;a<MyPol2D.NumPoints;a++)
    {
	DrawLine(&MyPol2D,a,&MyClip2D);
    }

    /* Fill Area */

    for (a=MinY;a<=MaxY;a++)
    {
	d = (MyClip2D.MaxX[a] - MyClip2D.MinX[a]);
	d += d==0;

	u1  =  MyClip2D.MinUVL1[a].x;
	v1  =  MyClip2D.MinUVL1[a].y;
	l1  =  MyClip2D.MinUVL1[a].z;
	u2  =  MyClip2D.MinUVL2[a].x;
	v2  =  MyClip2D.MinUVL2[a].y;
	l2  =  MyClip2D.MinUVL2[a].z;
	du1 = (MyClip2D.MaxUVL1[a].x - u1) / d;
	dv1 = (MyClip2D.MaxUVL1[a].y - v1) / d;
	dl1 = (MyClip2D.MaxUVL1[a].z - l1) / d;
	du2 = (MyClip2D.MaxUVL2[a].x - u2) / d;
	dv2 = (MyClip2D.MaxUVL2[a].y - v2) / d;
	dl2 = (MyClip2D.MaxUVL2[a].z - l2) / d;
	/*
	printf("\n%ld %ld %ld   %ld %ld %ld\n",MyClip2D.MinUVL1[a].x>>8,MyClip2D.MinUVL1[a].y>>8,MyClip2D.MinUVL2[a].z>>8,MyClip2D.MaxUVL1[a].x>>8,MyClip2D.MaxUVL1[a].y>>8,MyClip2D.MaxUVL1[a].z)>>8;
	printf(  "%ld %ld %ld   %ld %ld %ld\n",MyClip2D.MinUVL2[a].x>>8,MyClip2D.MinUVL2[a].y>>8,MyClip2D.MinUVL2[a].z>>8,MyClip2D.MaxUVL2[a].x>>8,MyClip2D.MaxUVL2[a].y>>8,MyClip2D.MaxUVL2[a].z)>>8;
	printf(  "%ld %ld %ld   %ld %ld %ld\n",u1,v1,l1,du1,dv1,dl1);
	printf(  "%ld %ld %ld   %ld %ld %ld\n",u2,v2,l2,du2,dv2,dl2);
	*/
	/* draw horizontal line */

	for (b=MyClip2D.MinX[a];b<=MyClip2D.MaxX[a];b++)
	{
	    if (b>=0)
	    if (b<MyWidth)
	    {
		c=0;l=0;

		if (MySide->Front != 0)
		{
		    c=MySide->Front->ColMap[ ((u2>>DefTextureShiftX) & DefTextureMaskX)
					    +(v2 & DefTextureMaskY)];
		    l=(l2>>8)&255;
		}
		if (c != 0)
		{
		    ColMap[a*MyWidth+b]=MyFade.Col[l][c];
		}
		else
		{
		    if (MySide->Back != 0)
		    {
			c=MySide->Back->ColMap[ ((u1>>DefTextureShiftX) & DefTextureMaskX)
					       +(v1 & DefTextureMaskY)];
			l=(l1>>8)&255;
			ColMap[a*MyWidth+b]=MyFade.Col[l][c];
		    }
		}
		/*
		SetAPen(MyRastPort,c+ (c==0));
		WritePixel(MyRastPort,b,a);
		*/
	    }
	    u1+=du1;
	    v1+=dv1;
	    l1+=dl1;
	    u2+=du2;
	    v2+=dv2;
	    l2+=dl2;
	}
    }
}

void FillClip(int16 RecLevel)
{
    /* Make Side Empty */
    /* Draw Side */

}

int16 ClipPolygonMesh(pBot MyPlayer,pMesh MyMesh,pPolygon MyPol3D,Vct * Base,int16 RecLevel)
{
    Pol3D MyPol3D1,MyPol3D2;
    int16 a,b;
    Vct   Inside;

    /*
    printf("ClipMesh\n");
    */

    /* Init MyPol3D1 */

    MyPol3D1.NumPoints = MyPol3D->NumPoints;

    for (a=0;a<MyPol3D->NumPoints;a++)
    {
	VctAdd(&MyPol3D->Pnts[a],Base,&MyPol3D1.Pnts[a]);

	MyPol3D1.uvl1[a].x = MyPol3D->BackPnts[a].x;
	MyPol3D1.uvl1[a].y = MyPol3D->BackPnts[a].y;
	MyPol3D1.uvl1[a].z = MyPol3D->BackPnts[a].z;

	MyPol3D1.uvl2[a].x = MyPol3D->FrontPnts[a].x;
	MyPol3D1.uvl2[a].y = MyPol3D->FrontPnts[a].y;
	MyPol3D1.uvl2[a].z = MyPol3D->FrontPnts[a].z;
    }

/*
    printf("\nOriginal Coords\n");
    for (a=0;a<MyPol3D1.NumPoints;a++)
    {
	printf("%f %f %f (%f %f %f) (%f %f %f)\n",MyPol3D1.Pnts[a].x,MyPol3D1.Pnts[a].y,MyPol3D1.Pnts[a].z,
						  MyPol3D1.uvl1[a].x,MyPol3D1.uvl1[a].y,MyPol3D1.uvl1[a].z,
						  MyPol3D1.uvl2[a].x,MyPol3D1.uvl2[a].y,MyPol3D1.uvl2[a].z);
    }
*/
    /* Transform to ships view */

    for (a=0;a<4;a++)
    {
	VctSub(&MyPol3D1.Pnts[a],&MyPlayer->Pos,&MyPol3D2.Pnts[a]);
	VctMulMat(&MyPol3D2.Pnts[a],&MyPlayer->Direction,&MyPol3D1.Pnts[a]);
	MyPol3D1.Pnts[a].z+=CameraBehind;
	/*
	printf("%f %f %f\n",MyPol3D1.Pnts[a].x,MyPol3D1.Pnts[a].y,MyPol3D1.Pnts[a].z);
	*/
    }
/*
    if (MySide->Back != 0)
    {
	printf("\nPolygon in ships Coords\n");
	for (a=0;a<MyPol3D1.NumPoints;a++)
	{
	     printf("%f %f %f (%f %f %f) (%f %f %f)\n",MyPol3D1.Pnts[a].x,MyPol3D1.Pnts[a].y,MyPol3D1.Pnts[a].z,
						       MyPol3D1.uvl1[a].x,MyPol3D1.uvl1[a].y,MyPol3D1.uvl1[a].z,
						       MyPol3D1.uvl2[a].x,MyPol3D1.uvl2[a].y,MyPol3D1.uvl2[a].z);
	}
    }
*/
    MyPol3D1.Pnts[4].x=MyPol3D1.Pnts[0].x; /* Last line should close the polygon */
    MyPol3D1.Pnts[4].y=MyPol3D1.Pnts[0].y;
    MyPol3D1.Pnts[4].z=MyPol3D1.Pnts[0].z;
    MyPol3D1.uvl1[4].x=MyPol3D1.uvl1[0].x;
    MyPol3D1.uvl1[4].y=MyPol3D1.uvl1[0].y;
    MyPol3D1.uvl1[4].z=MyPol3D1.uvl1[0].z;
    MyPol3D1.uvl2[4].x=MyPol3D1.uvl2[0].x;
    MyPol3D1.uvl2[4].y=MyPol3D1.uvl2[0].y;
    MyPol3D1.uvl2[4].z=MyPol3D1.uvl2[0].z;

    if (  Max4fix(MyPol3D1.Pnts[0].z,MyPol3D1.Pnts[1].z,MyPol3D1.Pnts[2].z,MyPol3D1.Pnts[3].z)
	< ClipPol3D[RecLevel].MinZ + CameraBehind)
    {
	/*
	printf("ClipSide MaxFailed\n");
	*/
	return !(0==0);
    }
    else
    {

	/* Clip Polygon to ClipPol3D.MinZ */

	MyPol3D2.NumPoints=0;

	ClipPol3DZ( &MyPol3D1, &MyPol3D2, ClipPol3D[RecLevel].MinZ);

#ifdef DEBUG
	/* Print out Cliped Polygon */

	printf("\nClipped to Min Z = %f\n",ClipPol3D[RecLevel].MinZ);
	for (a=0;a<MyPol3D2.NumPoints;a++)
	{
	    printf("%f %f %f (%f %f %f) (%f %f %f)\n",MyPol3D2.Pnts[a].x,MyPol3D2.Pnts[a].y,MyPol3D2.Pnts[a].z,
						      MyPol3D2.uvl1[a].x,MyPol3D2.uvl1[a].y,MyPol3D2.uvl1[a].z,
						      MyPol3D2.uvl2[a].x,MyPol3D2.uvl2[a].y,MyPol3D2.uvl2[a].z);
	}
#endif

	if (MyPol3D2.NumPoints >2)  /* 3 Points needed for an Area */
	{

	    /* Insert Points to close Polygon */

	    ClipPol3D[RecLevel].Pnts[ClipPol3D[RecLevel].NumPoints].x = ClipPol3D[RecLevel].Pnts[0].x;
	    ClipPol3D[RecLevel].Pnts[ClipPol3D[RecLevel].NumPoints].y = ClipPol3D[RecLevel].Pnts[0].y;
	    ClipPol3D[RecLevel].Pnts[ClipPol3D[RecLevel].NumPoints].z = ClipPol3D[RecLevel].Pnts[0].z;
	    ClipPol3D[RecLevel].uvl1[ClipPol3D[RecLevel].NumPoints].x = ClipPol3D[RecLevel].uvl1[0].x;
	    ClipPol3D[RecLevel].uvl1[ClipPol3D[RecLevel].NumPoints].y = ClipPol3D[RecLevel].uvl1[0].y;
	    ClipPol3D[RecLevel].uvl1[ClipPol3D[RecLevel].NumPoints].z = ClipPol3D[RecLevel].uvl1[0].z;
	    ClipPol3D[RecLevel].uvl2[ClipPol3D[RecLevel].NumPoints].x = ClipPol3D[RecLevel].uvl2[0].x;
	    ClipPol3D[RecLevel].uvl2[ClipPol3D[RecLevel].NumPoints].y = ClipPol3D[RecLevel].uvl2[0].y;
	    ClipPol3D[RecLevel].uvl2[ClipPol3D[RecLevel].NumPoints].z = ClipPol3D[RecLevel].uvl2[0].z;

	    /* Calculate Mid Point */

	    Inside.x=0;
	    Inside.y=0;
	    Inside.z=0;

	    for (a=0;a<ClipPol3D[RecLevel].NumPoints;a++)
	    {
		VctAdd(&Inside,&ClipPol3D[RecLevel].Pnts[a],&Inside);
	    }

	    VctMulFix(&Inside,1/(fix)ClipPol3D[RecLevel].NumPoints,&Inside);

#ifdef DEBUG
	    /* Print clipping Polygone */

	    printf("Clipping to\n");

	    for (a=0;a<ClipPol3D[RecLevel].NumPoints+2;a++)
	    {
		printf("Clip %d %f %f %f\n",a,ClipPol3D[RecLevel].Pnts[a].x,ClipPol3D[RecLevel].Pnts[a].y,ClipPol3D[RecLevel].Pnts[a].z);
	    }
#endif

	    /* Clip Polygon */

	    for (a=0;a<ClipPol3D[RecLevel].NumPoints;a++)
	    {
		ClipPolygon(&MyPol3D2,&MyPol3D1,&ClipPol3D[RecLevel].Pnts[a],&ClipPol3D[RecLevel].Pnts[a+1],&Inside);

		if (MyPol3D1.NumPoints<3) /* Do we still have an area */
		{
		    a=ClipPol3D[RecLevel].NumPoints;
		}
		else
		{
#ifdef DEBUG
		    printf("%d\n",a);
#endif
		    for (b=0;b<=MyPol3D1.NumPoints;b++)
		    {
#ifdef DEBUG
			printf("Point %d : %f %f %f",b,MyPol3D2.Pnts[b].x,MyPol3D2.Pnts[b].y,MyPol3D2.Pnts[b].z);
			printf("--> %f %f %f\n",       MyPol3D1.Pnts[b].x,MyPol3D1.Pnts[b].y,MyPol3D1.Pnts[b].z);
#endif

			MyPol3D2.Pnts[b].x = MyPol3D1.Pnts[b].x;
			MyPol3D2.Pnts[b].y = MyPol3D1.Pnts[b].y;
			MyPol3D2.Pnts[b].z = MyPol3D1.Pnts[b].z;
			MyPol3D2.uvl1[b].x = MyPol3D1.uvl1[b].x;
			MyPol3D2.uvl1[b].y = MyPol3D1.uvl1[b].y;
			MyPol3D2.uvl1[b].z = MyPol3D1.uvl1[b].z;
			MyPol3D2.uvl2[b].x = MyPol3D1.uvl2[b].x;
			MyPol3D2.uvl2[b].y = MyPol3D1.uvl2[b].y;
			MyPol3D2.uvl2[b].z = MyPol3D1.uvl2[b].z;
		    }
		    MyPol3D2.NumPoints = MyPol3D1.NumPoints;
		}
	    }

	    if (MyPol3D1.NumPoints>2) /* Do we have ab Area */
	    {
		/* Save cliped Polygon and set ClipPol3D.MinZ */

		ClipPol3D[RecLevel+1].MinZ=MyPol3D2.Pnts[0].z;
		ClipPol3D[RecLevel+1].NumPoints = MyPol3D2.NumPoints;

		for (a=0;a<MyPol3D2.NumPoints;a++)
		{
		    ClipPol3D[RecLevel+1].Pnts[a].x = MyPol3D2.Pnts[a].x;
		    ClipPol3D[RecLevel+1].Pnts[a].y = MyPol3D2.Pnts[a].y;
		    ClipPol3D[RecLevel+1].Pnts[a].z = MyPol3D2.Pnts[a].z;
		    ClipPol3D[RecLevel+1].uvl1[a].x = MyPol3D2.uvl1[a].x;
		    ClipPol3D[RecLevel+1].uvl1[a].y = MyPol3D2.uvl1[a].y;
		    ClipPol3D[RecLevel+1].uvl1[a].z = MyPol3D2.uvl1[a].z;
		    ClipPol3D[RecLevel+1].uvl2[a].x = MyPol3D2.uvl2[a].x;
		    ClipPol3D[RecLevel+1].uvl2[a].y = MyPol3D2.uvl2[a].y;
		    ClipPol3D[RecLevel+1].uvl2[a].z = MyPol3D2.uvl2[a].z;

		    if (ClipPol3D[RecLevel+1].MinZ > MyPol3D2.Pnts[a].z)
		    {
			ClipPol3D[RecLevel+1].MinZ = MyPol3D2.Pnts[a].z;
		    }
		}

		return (0==0);
	    }
	    else /* Only <3 Points after Clipping */
	    {
		return !(0==0);
	    }
	}
	else /* Only <3 Points after ClipMinZ */
	{
	    return !(0==0);
	}
    }
}

void DrawBot(pBot MyPlayer,pBot MyBot,int16 RecLevel,fix Light)
{
    int16 a;
    Mesh  MyMesh;
    Side  MySide;
    fix n;

    /*
    printf("DrawBot\n");
    */

    /* Init Mesh at Bots Position */

    MyMesh.NumPolygons = MyBot->Mesh->NumPolygons;

    for (a=0;a<MyMesh.NumPolygons;a++)
    {
	MyMesh.Polygons[a] = MyBot->Mesh->Polygons[a];
	VctAdd(&MyBot->Pos,&MyBot->Mesh->Base[a],&MyMesh.Base[a]);
    }

    MyMesh.Light = Light * MyBot->Light;
    MyMesh.Direction.a.x = MyBot->Direction.a.x;
    MyMesh.Direction.a.y = MyBot->Direction.a.y;
    MyMesh.Direction.a.z = MyBot->Direction.a.z;
    MyMesh.Direction.b.x = MyBot->Direction.b.x;
    MyMesh.Direction.b.y = MyBot->Direction.b.y;
    MyMesh.Direction.b.z = MyBot->Direction.b.z;
    MyMesh.Direction.c.x = MyBot->Direction.c.x;
    MyMesh.Direction.c.y = MyBot->Direction.c.y;
    MyMesh.Direction.c.z = MyBot->Direction.c.z;

    /* Clip Polygons */

    for (a=0;a<MyMesh.NumPolygons;a++)
    {
	n=VctDot(&MyPlayer->Direction.c,&MyMesh.Polygons[a]->Normal);

	/*
	printf("%d %f\n",a,n);
	*/

	if (n<-0.4)
	{
	    if (ClipPolygonMesh(MyPlayer,&MyMesh,MyMesh.Polygons[a],&MyMesh.Base[a],RecLevel))
	    {
		/* Draw Polygon */

		MySide.Back  = MyMesh.Polygons[a]->Back;
		MySide.Front = MyMesh.Polygons[a]->Front;

		DrawSide(&MySide,RecLevel+1);
	    }
	}
    }
}

void DrawCube(pBot MyPlayer,pCube MyCube,int16 RecLevel,fix Light)
{
    int16 a;

    pBot  MyBot;

    /*
    printf("DrawCube %ld at %d\n",MyCube,RecLevel);
    */

    if (RecLevel == MaxRecLevel)
    {
	/* Fill everything black */

	FillClip(RecLevel);
    }
    else
    {
	ClipPol3D[RecLevel+1].Cube=MyCube; /* Save Cube to protect against loops */

	for (a=0;a<6;a++)
	{
#ifdef DEBUG
	    printf("Cube %d Side %d\n",((long)MyCube-(long)&MyLevelData.Cubes[0])/sizeof(Cube)+1,a);
#endif
	    if (MyCube->Cubes[a] != ClipPol3D[RecLevel].Cube) /* Do not draw the side we look through */
	    {
		/* Draw Side and Cube */

		/* check if anything is to be drawn at all */

		if (ClipSide(MyPlayer,&MyCube->Sides[a],RecLevel,Light))
		{
		/*
		    printf("Cube %ld Side %d %ld MinZ %f %f\n",MyCube,a,MyCube->Cubes[a],ClipMinZ[RecLevel],ClipMinZ[RecLevel+1]);
		*/

		    /* Is there a background Texture ? */

		    if (MyCube->Sides[a].Back != 0)
		    {
			/* Draw Textures */
		    /*
		       SetAPen(MyRastPort,45);
		    */
			DrawSide(&MyCube->Sides[a],RecLevel+1);
		    /*
		       SetAPen(MyRastPort,32);
		    */
		    }
		    else if (MyCube->Cubes[a] != 0)
		    {
			/* Draw connected cube */

			DrawCube(MyPlayer, MyCube->Cubes[a], RecLevel+1, Light * LightLess );

			if (MyCube->Sides[a].Front != 0)
			{
			    /* Draw Foreground Texture */

			    DrawSide(&MyCube->Sides[a],RecLevel+1);
			}
		    }

		}
		/*
		else
		{
		    printf("Nothing to draw\n");
		}
		*/
	    }
	}
	/* Draw Bots if there are any */

	MyBot = MyCube->FirstBot;

	while (MyBot != 0)
	{
	    if ((MyBot->Type & BotDraw) != 0)
	    {
		DrawBot(MyPlayer,MyBot,RecLevel,Light);
	    }

	    MyBot = MyBot->Next;
	}
    }
}

void MainLoop(void)
{
    int16 x,y;
    pBot  MyPlayer;

    /* Init Clip Polygon */

    ClipPol3D[0].NumPoints=4;
    ClipPol3D[0].MinZ=+0.001;
    ClipPol3D[0].Cube=(pCube)1;

    ClipPol3D[0].Pnts[0].x=-1.0;
    ClipPol3D[0].Pnts[0].y=-1.0;
    ClipPol3D[0].Pnts[0].z=+1.0;

    ClipPol3D[0].Pnts[1].x=+1.0;
    ClipPol3D[0].Pnts[1].y=-1.0;
    ClipPol3D[0].Pnts[1].z=+1.0;

    ClipPol3D[0].Pnts[2].x=+1.0;
    ClipPol3D[0].Pnts[2].y=+1.0;
    ClipPol3D[0].Pnts[2].z=+1.0;

    ClipPol3D[0].Pnts[3].x=-1.0;
    ClipPol3D[0].Pnts[3].y=+1.0;
    ClipPol3D[0].Pnts[3].z=+1.0;

    ClipPol3D[0].Pnts[4].x=-1.0;
    ClipPol3D[0].Pnts[4].y=-1.0;
    ClipPol3D[0].Pnts[4].z=+1.0;

    /* Draw something into the info window */

    for (y=0;y<64;y++)
    {
	for (x=0;x<((int)((MyWidth+1)/MyLevelData.Textures[0].TextureWidth))*MyLevelData.Textures[0].TextureWidth;x++)
	{
	   SetAPen(InfoRastPort,MyLevelData.Textures[0].ColMap[x%MyLevelData.Textures[0].TextureWidth+y*MyLevelData.Textures[0].TextureWidth]);
	    WritePixel(InfoRastPort,x+MyWidth%MyLevelData.Textures[0].TextureWidth/2,y);
	}
    }

    MyPlayer = &MyLevelData.Bots[0];

    printf("%f\n",ColLight);

    while (!Finished)
    {
	/*
	for (x=0;x<MyWidth*MyHeight;x++)
	{
	    ColMap[x]=0;
	}
	*/

	TStartTime  = CurrentTime();

	MovePlayer(MyPlayer);

	MoveBots();

	EndTime = CurrentTime();


	if (MyPlayer->NumRings >= MyLevelData.NumRings)
	{
	    ColLight= 0.95 * ColLight;
	    if (ColLight<0.2)
	    {
		Finished = !Finished;
	    }
	}
	/*
	printf("\nPlayer : %f %f %f\n",MyPlayer->Pos.x,MyPlayer->Pos.y,MyPlayer->Pos.z);
	printf(  "         %f %f %f\n",MyPlayer->Direction.a.x,MyPlayer->Direction.a.y,MyPlayer->Direction.a.z);
	printf(  "         %f %f %f\n",MyPlayer->Direction.b.x,MyPlayer->Direction.b.y,MyPlayer->Direction.b.z);
	printf(  "         %f %f %f\n",MyPlayer->Direction.c.x,MyPlayer->Direction.c.y,MyPlayer->Direction.c.z);
	*/

	StartTime  = CurrentTime();

	DrawCube(MyPlayer,MyPlayer->Cube,0,ColLight);

	EndTime  = CurrentTime();
	RenderTime += EndTime - StartTime;
	StartTime = EndTime;

	WritePixelArray8(MyRastPort,0,0,MyWidth-1,MyHeight-1,ColMap,MyTmpRast);

	Frames++;

	EndTime  = CurrentTime();
	WPA8Time += EndTime - StartTime;

	TotalTime += EndTime - TStartTime;

	if (Frames == 100)
	{
	    ULONG fps, rend, wpa;

	    fps = (Frames/TotalTime) * 100.0;
	    rend = 100.0*RenderTime/TotalTime;
	    wpa = 100.0*WPA8Time/TotalTime;

	    printf("FPS = %3ld.%02ld  Rendern %2ld.%02ld   WPA8 %2ld.%02ld\n"
		,(long)fps/100,(long)fps%100
		,(long)rend/100,(long)rend%100
		,(long)wpa/100,(long)wpa%100
	    );

	    Frames     = 0;
	    TotalTime  = 0.0;
	    RenderTime = 0.0;
	    WPA8Time   = 0.0;
	}
    }
}

void WaitKey(void)
{
    char a[80];

    printf("Enter something to continue!\n");
    scanf("%s",a);
}
