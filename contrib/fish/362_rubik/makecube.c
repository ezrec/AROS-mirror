/*	makecube.c  ...for rubik.c	*/

/*	started 23-Dec-89	*/

#include "header.c"

extern struct points point;
extern struct points start;
extern unsigned short size;

extern struct one_square square[56];	/*	54 on cube + 2 inter-planes  */

void init_squares()		/*	this defines the cubes geometry called ONCE	*/
	{
	int face,i;
	
	for (face=0; face<2; face++)	/*	small squares in xy plane	*/
		{
		(square[9*face+0]).corner[0] = &(point.xyz [0] [0] [face*5]);
		(square[9*face+0]).corner[1] = &(point.xyz [1] [0] [face*5]);
		(square[9*face+0]).corner[2] = &(point.xyz [1] [1] [face*5]);
		(square[9*face+0]).corner[3] = &(point.xyz [0] [1] [face*5]);

		(square[9*face+1]).corner[0] = &(point.xyz [2] [0] [face*5]);
		(square[9*face+1]).corner[1] = &(point.xyz [3] [0] [face*5]);
		(square[9*face+1]).corner[2] = &(point.xyz [3] [1] [face*5]);
		(square[9*face+1]).corner[3] = &(point.xyz [2] [1] [face*5]);

		(square[9*face+2]).corner[0] = &(point.xyz [4] [0] [face*5]);
		(square[9*face+2]).corner[1] = &(point.xyz [5] [0] [face*5]);
		(square[9*face+2]).corner[2] = &(point.xyz [5] [1] [face*5]);
		(square[9*face+2]).corner[3] = &(point.xyz [4] [1] [face*5]);

		(square[9*face+3]).corner[0] = &(point.xyz [0] [2] [face*5]);
		(square[9*face+3]).corner[1] = &(point.xyz [1] [2] [face*5]);
		(square[9*face+3]).corner[2] = &(point.xyz [1] [3] [face*5]);
		(square[9*face+3]).corner[3] = &(point.xyz [0] [3] [face*5]);

		(square[9*face+4]).corner[0] = &(point.xyz [2] [2] [face*5]);
		(square[9*face+4]).corner[1] = &(point.xyz [3] [2] [face*5]);
		(square[9*face+4]).corner[2] = &(point.xyz [3] [3] [face*5]);
		(square[9*face+4]).corner[3] = &(point.xyz [2] [3] [face*5]);

		(square[9*face+5]).corner[0] = &(point.xyz [4] [2] [face*5]);
		(square[9*face+5]).corner[1] = &(point.xyz [5] [2] [face*5]);
		(square[9*face+5]).corner[2] = &(point.xyz [5] [3] [face*5]);
		(square[9*face+5]).corner[3] = &(point.xyz [4] [3] [face*5]);

		(square[9*face+6]).corner[0] = &(point.xyz [0] [4] [face*5]);
		(square[9*face+6]).corner[1] = &(point.xyz [1] [4] [face*5]);
		(square[9*face+6]).corner[2] = &(point.xyz [1] [5] [face*5]);
		(square[9*face+6]).corner[3] = &(point.xyz [0] [5] [face*5]);

		(square[9*face+7]).corner[0] = &(point.xyz [2] [4] [face*5]);
		(square[9*face+7]).corner[1] = &(point.xyz [3] [4] [face*5]);
		(square[9*face+7]).corner[2] = &(point.xyz [3] [5] [face*5]);
		(square[9*face+7]).corner[3] = &(point.xyz [2] [5] [face*5]);

		(square[9*face+8]).corner[0] = &(point.xyz [4] [4] [face*5]);
		(square[9*face+8]).corner[1] = &(point.xyz [5] [4] [face*5]);
		(square[9*face+8]).corner[2] = &(point.xyz [5] [5] [face*5]);
		(square[9*face+8]).corner[3] = &(point.xyz [4] [5] [face*5]);
		}

	for (face=2; face<4; face++)	/*	small squares in xz plane	*/
		{
		(square[9*face+0]).corner[0] = &(point.xyz [0] [(face-2)*5] [0]);
		(square[9*face+0]).corner[1] = &(point.xyz [1] [(face-2)*5] [0]);
		(square[9*face+0]).corner[2] = &(point.xyz [1] [(face-2)*5] [1]);
		(square[9*face+0]).corner[3] = &(point.xyz [0] [(face-2)*5] [1]);

		(square[9*face+1]).corner[0] = &(point.xyz [2] [(face-2)*5] [0]);
		(square[9*face+1]).corner[1] = &(point.xyz [3] [(face-2)*5] [0]);
		(square[9*face+1]).corner[2] = &(point.xyz [3] [(face-2)*5] [1]);
		(square[9*face+1]).corner[3] = &(point.xyz [2] [(face-2)*5] [1]);

		(square[9*face+2]).corner[0] = &(point.xyz [4] [(face-2)*5] [0]);
		(square[9*face+2]).corner[1] = &(point.xyz [5] [(face-2)*5] [0]);
		(square[9*face+2]).corner[2] = &(point.xyz [5] [(face-2)*5] [1]);
		(square[9*face+2]).corner[3] = &(point.xyz [4] [(face-2)*5] [1]);

		(square[9*face+3]).corner[0] = &(point.xyz [0] [(face-2)*5] [2]);
		(square[9*face+3]).corner[1] = &(point.xyz [1] [(face-2)*5] [2]);
		(square[9*face+3]).corner[2] = &(point.xyz [1] [(face-2)*5] [3]);
		(square[9*face+3]).corner[3] = &(point.xyz [0] [(face-2)*5] [3]);

		(square[9*face+4]).corner[0] = &(point.xyz [2] [(face-2)*5] [2]);
		(square[9*face+4]).corner[1] = &(point.xyz [3] [(face-2)*5] [2]);
		(square[9*face+4]).corner[2] = &(point.xyz [3] [(face-2)*5] [3]);
		(square[9*face+4]).corner[3] = &(point.xyz [2] [(face-2)*5] [3]);

		(square[9*face+5]).corner[0] = &(point.xyz [4] [(face-2)*5] [2]);
		(square[9*face+5]).corner[1] = &(point.xyz [5] [(face-2)*5] [2]);
		(square[9*face+5]).corner[2] = &(point.xyz [5] [(face-2)*5] [3]);
		(square[9*face+5]).corner[3] = &(point.xyz [4] [(face-2)*5] [3]);

		(square[9*face+6]).corner[0] = &(point.xyz [0] [(face-2)*5] [4]);
		(square[9*face+6]).corner[1] = &(point.xyz [1] [(face-2)*5] [4]);
		(square[9*face+6]).corner[2] = &(point.xyz [1] [(face-2)*5] [5]);
		(square[9*face+6]).corner[3] = &(point.xyz [0] [(face-2)*5] [5]);

		(square[9*face+7]).corner[0] = &(point.xyz [2] [(face-2)*5] [4]);
		(square[9*face+7]).corner[1] = &(point.xyz [3] [(face-2)*5] [4]);
		(square[9*face+7]).corner[2] = &(point.xyz [3] [(face-2)*5] [5]);
		(square[9*face+7]).corner[3] = &(point.xyz [2] [(face-2)*5] [5]);

		(square[9*face+8]).corner[0] = &(point.xyz [4] [(face-2)*5] [4]);
		(square[9*face+8]).corner[1] = &(point.xyz [5] [(face-2)*5] [4]);
		(square[9*face+8]).corner[2] = &(point.xyz [5] [(face-2)*5] [5]);
		(square[9*face+8]).corner[3] = &(point.xyz [4] [(face-2)*5] [5]);
		}

	for (face=4; face<6; face++)	/*	small squares in yz plane	*/
		{
		(square[9*face+0]).corner[0] = &(point.xyz [(face-4)*5] [0] [0]);
		(square[9*face+0]).corner[1] = &(point.xyz [(face-4)*5] [0] [1]);
		(square[9*face+0]).corner[2] = &(point.xyz [(face-4)*5] [1] [1]);
		(square[9*face+0]).corner[3] = &(point.xyz [(face-4)*5] [1] [0]);
                                                             
		(square[9*face+1]).corner[0] = &(point.xyz [(face-4)*5] [0] [2]);
		(square[9*face+1]).corner[1] = &(point.xyz [(face-4)*5] [0] [3]);
		(square[9*face+1]).corner[2] = &(point.xyz [(face-4)*5] [1] [3]);
		(square[9*face+1]).corner[3] = &(point.xyz [(face-4)*5] [1] [2]);
                                                             
		(square[9*face+2]).corner[0] = &(point.xyz [(face-4)*5] [0] [4]);
		(square[9*face+2]).corner[1] = &(point.xyz [(face-4)*5] [0] [5]);
		(square[9*face+2]).corner[2] = &(point.xyz [(face-4)*5] [1] [5]);
		(square[9*face+2]).corner[3] = &(point.xyz [(face-4)*5] [1] [4]);
                                                             
		(square[9*face+3]).corner[0] = &(point.xyz [(face-4)*5] [2] [0]);
		(square[9*face+3]).corner[1] = &(point.xyz [(face-4)*5] [2] [1]);
		(square[9*face+3]).corner[2] = &(point.xyz [(face-4)*5] [3] [1]);
		(square[9*face+3]).corner[3] = &(point.xyz [(face-4)*5] [3] [0]);
                                                             
		(square[9*face+4]).corner[0] = &(point.xyz [(face-4)*5] [2] [2]);
		(square[9*face+4]).corner[1] = &(point.xyz [(face-4)*5] [2] [3]);
		(square[9*face+4]).corner[2] = &(point.xyz [(face-4)*5] [3] [3]);
		(square[9*face+4]).corner[3] = &(point.xyz [(face-4)*5] [3] [2]);
                                                             
		(square[9*face+5]).corner[0] = &(point.xyz [(face-4)*5] [2] [4]);
		(square[9*face+5]).corner[1] = &(point.xyz [(face-4)*5] [2] [5]);
		(square[9*face+5]).corner[2] = &(point.xyz [(face-4)*5] [3] [5]);
		(square[9*face+5]).corner[3] = &(point.xyz [(face-4)*5] [3] [4]);
                                                             
		(square[9*face+6]).corner[0] = &(point.xyz [(face-4)*5] [4] [0]);
		(square[9*face+6]).corner[1] = &(point.xyz [(face-4)*5] [4] [1]);
		(square[9*face+6]).corner[2] = &(point.xyz [(face-4)*5] [5] [1]);
		(square[9*face+6]).corner[3] = &(point.xyz [(face-4)*5] [5] [0]);
                                                             
		(square[9*face+7]).corner[0] = &(point.xyz [(face-4)*5] [4] [2]);
		(square[9*face+7]).corner[1] = &(point.xyz [(face-4)*5] [4] [3]);
		(square[9*face+7]).corner[2] = &(point.xyz [(face-4)*5] [5] [3]);
		(square[9*face+7]).corner[3] = &(point.xyz [(face-4)*5] [5] [2]);
                                                             
		(square[9*face+8]).corner[0] = &(point.xyz [(face-4)*5] [4] [4]);
		(square[9*face+8]).corner[1] = &(point.xyz [(face-4)*5] [4] [5]);
		(square[9*face+8]).corner[2] = &(point.xyz [(face-4)*5] [5] [5]);
		(square[9*face+8]).corner[3] = &(point.xyz [(face-4)*5] [5] [4]);
		}

	for (i=0; i<54; i++)
		(square[i]).colour = 1+ i / 9;

	for (i=54; i<56; i++)
		(square[i]).colour = 0;
	}                                             

void init_points()	/*	generate cube centered on (0,0,0)	*/
	{
	int i,j,k;
	
	for (i=0; i<6; i++)
		for (j=0; j<6; j++)
			for (k=0; k<6; k++)
				if (i==0 || i==5 || j==0 || j==5 || k==0 || k==5)
					{
					switch (i)
						{
						case 0:
							(start.xyz [i] [j] [k]).x = size * -3;
							break;
						case 1:
						case 2:
							(start.xyz [i] [j] [k]).x = -size;
							break;
						case 3:
						case 4:
							(start.xyz [i] [j] [k]).x = size;
							break;
						case 5:
							(start.xyz [i] [j] [k]).x = size * 3;
							break;
						}
					switch (j)
						{
						case 0:
							(start.xyz [i] [j] [k]).y = size * -3;
							break;
						case 1:
						case 2:
							(start.xyz [i] [j] [k]).y = -size;
							break;
						case 3:
						case 4:
							(start.xyz [i] [j] [k]).y = size;
							break;
						case 5:
							(start.xyz [i] [j] [k]).y = size * 3;
							break;
						}
					switch (k)
						{
						case 0:
							(start.xyz [i] [j] [k]).z = size * -3;
							break;
						case 1:
						case 2:
							(start.xyz [i] [j] [k]).z = -size;
							break;
						case 3:
						case 4:
							(start.xyz [i] [j] [k]).z = size;
							break;
						case 5:
							(start.xyz [i] [j] [k]).z = size * 3;
							break;
						}
					}
	}
