/*	rotate.c	for rubik.c	M.J.R.	Boxing Day 1989	*/

#include "header.c"

extern struct points point;
extern struct one_square square[56];
extern int twistlist[21];
int biglist [126] =
	{
	 0, 1, 2, 3, 4, 5, 6, 7, 8,18,19,20,27,28,29,36,39,42,45,48,51,
	 9,10,11,12,13,14,15,16,17,24,25,26,33,34,35,38,41,44,47,50,53,
	 0, 1, 2, 9,10,11,18,19,20,21,22,23,24,25,26,36,37,38,45,46,47,
	 6, 7, 8,15,16,17,27,28,29,30,31,32,33,34,35,42,43,44,51,52,53,
	 0, 3, 6, 9,12,15,18,21,24,27,30,33,36,37,38,39,40,41,42,43,44,
	 2, 5, 8,11,14,17,20,23,26,29,32,35,45,46,47,48,49,50,51,52,53
	};

extern short mysin(int);
extern short mycos(int);
extern short hypoten(short, short);
extern short angle(short, short, short);
extern unsigned short size;
extern void rotate (short *a, short *b, short ang);

void fb (short layer, short ang)	/* twist front or back layer	*/
	{
	/*	layer = 0 for front, 1 for back	*/

	int i,j,k,*p,*q;

	/*	define extra 2 squares	*/
	if (layer)
		{
		i = 55;
		j = 54;
		}
	else
		{
		i = 54;
		j = 55;
		}

	(square[i]).corner[0] = &(point.xyz  [0] [0] [k = layer + layer + 1]);
	(square[i]).corner[1] = &(point.xyz  [5] [0] [k]);
	(square[i]).corner[2] = &(point.xyz  [5] [5] [k]);
	(square[i]).corner[3] = &(point.xyz  [0] [5] [k]);
	(square[j]).corner[0] = &(point.xyz  [0] [0] [++k]);
	(square[j]).corner[1] = &(point.xyz  [0] [5] [k]);
	(square[j]).corner[2] = &(point.xyz  [5] [5] [k]);
	(square[j]).corner[3] = &(point.xyz  [5] [0] [k]);

	for (k=4*layer; k < (4*layer+2); k++)
		for (i=0; i<6; i++)
			for (j=0; j<6; j++)
				if (i==0 || i==5 || j==0 || j==5 || k==0 || k==5)
					rotate
						(
						&((point.xyz [i] [j] [k]).x),
						&((point.xyz [i] [j] [k]).y),
						ang
						);

	p = &(twistlist[0]);
	q = &(biglist[layer * 21]);
	
	for (i=0; i<21; i++)
		*p++ = *q++;
	}

void bt (short layer, short ang)	/* twist bottom or top layer	*/
	{
	/*	layer = 0 for bottom, 1 for top	*/

	int i,j,k,*p,*q;

	/*	define extra 2 squares	*/
	if (layer)
		{
		i = 55;
		j = 54;
		}
	else
		{
		i = 54;
		j = 55;
		}

	(square[i]).corner[0] = &(point.xyz  [0] [k = layer + layer + 1] [0]);
	(square[i]).corner[1] = &(point.xyz  [0] [k] [5]);
	(square[i]).corner[2] = &(point.xyz  [5] [k] [5]);
	(square[i]).corner[3] = &(point.xyz  [5] [k] [0]);
	(square[j]).corner[0] = &(point.xyz  [0] [++k] [0]);
	(square[j]).corner[1] = &(point.xyz  [5] [k] [0]);
	(square[j]).corner[2] = &(point.xyz  [5] [k] [5]);
	(square[j]).corner[3] = &(point.xyz  [0] [k] [5]);

	for (k=4*layer; k < (4*layer+2); k++)
		for (i=0; i<6; i++)
			for (j=0; j<6; j++)
				if (i==0 || i==5 || j==0 || j==5 || k==0 || k==5)
					rotate
						(
						&((point.xyz [i] [k] [j]).x),
						&((point.xyz [i] [k] [j]).z),
						ang
						);

	p = &(twistlist[0]);
	q = &(biglist[42 + layer * 21]);
	
	for (i=0; i<21; i++)
		*p++ = *q++;
	}

void lr (short layer, short ang)	/* twist left or right layer	*/
	{
	/*	layer = 0 for left, 1 for right	*/

	int i,j,k,*p,*q;

	/*	define extra 2 squares	*/
	if (layer)
		{
		i = 55;
		j = 54;
		}
	else
		{
		i = 54;
		j = 55;
		}

	(square[i]).corner[0] = &(point.xyz [k = layer + layer + 1] [0] [0]);
	(square[i]).corner[1] = &(point.xyz [k] [5] [0]);
	(square[i]).corner[2] = &(point.xyz [k] [5] [5]);
	(square[i]).corner[3] = &(point.xyz [k] [0] [5]);
	(square[j]).corner[0] = &(point.xyz [++k] [0] [0]);
	(square[j]).corner[1] = &(point.xyz [k] [0] [5]);
	(square[j]).corner[2] = &(point.xyz [k] [5] [5]);
	(square[j]).corner[3] = &(point.xyz [k] [5] [0]);

	for (k=4*layer; k < (4*layer+2); k++)
		for (i=0; i<6; i++)
			for (j=0; j<6; j++)
				if (i==0 || i==5 || j==0 || j==5 || k==0 || k==5)
					rotate
						(
						&((point.xyz [k] [j] [i]).z),
						&((point.xyz [k] [j] [i]).y),
						ang
						);


	p = &(twistlist[0]);
	q = &(biglist[84 + layer * 21]);
	
	for (i=0; i<21; i++)
		*p++ = *q++;
	}

void roll (short ang)	/*	roll whole cube through (ang)	*/
	{
	int i,j,k;
	
	for (i=0; i<6; i++)
		for (j=0; j<6; j++)
			for (k=0; k<6; k++)
				if (i==0 || i==5 || j==0 || j==5 || k==0 || k==5)
					rotate
						(
						&((point.xyz [i] [j] [k]).x),
						&((point.xyz [i] [j] [k]).y),
						ang
						);
	}

void yaw (short ang)	/*	yaw whole cube through (ang)	*/
	{
	int i,j,k;
	
	for (i=0; i<6; i++)
		for (j=0; j<6; j++)
			for (k=0; k<6; k++)
				if (i==0 || i==5 || j==0 || j==5 || k==0 || k==5)
					rotate
						(
						&((point.xyz [i] [j] [k]).x),
						&((point.xyz [i] [j] [k]).z),
						ang
						);
	}

void pitch (short ang)	/*	pitch whole cube through (ang)	*/
	{
	int i,j,k;
	
	for (i=0; i<6; i++)
		for (j=0; j<6; j++)
			for (k=0; k<6; k++)
				if (i==0 || i==5 || j==0 || j==5 || k==0 || k==5)
					rotate
						(
						&((point.xyz [i] [j] [k]).y),
						&((point.xyz [i] [j] [k]).z),
						ang
						);
	}
