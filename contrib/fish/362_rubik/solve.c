/*	I couldn't get an elegant tree searcher to work.  Hence this kludge. */


#include <aros/oldprograms.h>
#include "header.c"
#include "sequences.c"

short failed;
short waiting_output;
unsigned char output_buffer[20];

extern char tr [120];
extern struct one_square square[56];

extern void showtwist (short, short);
extern struct IntuiMessage *msg, *getmsg();
extern struct Window *wnd1,*wnd2;



struct IntuiMessage *flush()
	{
	unsigned char move;
	short layer,direc;
	short i;
	
	msg = NULL;
	
	while (waiting_output && ((msg = getmsg()) == NULL))
		{
		move = output_buffer[0];
	
		layer = --move/3;
		if ((direc = (move % 3)) == 0)
			direc = -1;

		showtwist (layer,direc);
		
		for (i=0; i < waiting_output; i++)
			output_buffer[i] = output_buffer[i+1];
		
		--waiting_output;
		}
	
	return (msg);
	}

void output (unsigned char move)
	{
	output_buffer [waiting_output++] = move;
	}

void cornerseq (short seq)
	{
	unsigned short i;
	
	for (i=0; i<6; i++)
		output(corner5[seq][i]);
	}

short checkcorner
	(
	short offset,
	short a, short b, short c,
	short x, short y, short z
	)
	{
	short corner = 0;
	
	if (square[x].colour == a)
		{
		if
			(
			((square[y].colour == b) && (square[z].colour == c)) ||
			((square[y].colour == c) && (square[z].colour == b))
			)
			corner = 1 + offset;
		}
	else if (square[y].colour == a)
		{
		if
			(
			((square[x].colour == b) && (square[z].colour == c)) ||
			((square[x].colour == c) && (square[z].colour == b))
			)
			corner = 2 + offset;
		}
	else if (square[z].colour == a)
		{
		if
			(
			((square[x].colour == b) && (square[y].colour == c)) ||
			((square[x].colour == c) && (square[y].colour == b))
			)
			corner = 3 + offset;
		}
	
	return (corner);
	}

short findcorner (short a, short b, short c)
	{
	short corner;

	a = square[a].colour;
	b = square[b].colour;
	c = square[c].colour;
	
	if
		(
		((corner = checkcorner(0,a,b,c,0,18,36)) == 0) &&
		((corner = checkcorner(3,a,b,c,2,45,20)) == 0) &&
		((corner = checkcorner(6,a,b,c,6,42,27)) == 0) &&
		((corner = checkcorner(9,a,b,c,8,29,51)) == 0) &&
		((corner = checkcorner(12,a,b,c,9,38,24)) == 0) &&
		((corner = checkcorner(15,a,b,c,11,26,47)) == 0) &&
		((corner = checkcorner(18,a,b,c,15,33,44)) == 0)
		)
		corner = checkcorner(21,a,b,c,17,53,35);
	
	return (corner);
	}

void edgeseq (short seq)
	{
	unsigned short i;
	
	for (i=0; i<6; i++)
		output(edge4[seq][i]);

	}

void edgeseq2 (short seq)
	{
	unsigned short i;

	for (i=0; i<5; i++)
		output(edge5[seq][i]);
	}

short checkedge
	(
	short offset,
	short a, short b,
	short x, short y
	)
	{
	short edge = 0;
	
	if ((square[x].colour == a) && (square[y].colour == b))
		edge = 1 + offset;
	else if ((square[x].colour == b) && (square[y].colour == a))
		edge = 2 + offset;

	return (edge);
	}

short findedge (short a, short b)
	{
	short edge;

	a = square[a].colour;
	b = square[b].colour;

	if	(
		((edge = checkedge(0,a,b,1,19)) == 0) &&
		((edge = checkedge(2,a,b,3,39)) == 0) &&
		((edge = checkedge(4,a,b,5,48)) == 0) &&
		((edge = checkedge(6,a,b,7,28)) == 0) &&
		((edge = checkedge(8,a,b,10,25)) == 0) &&
		((edge = checkedge(10,a,b,12,41)) == 0) &&
		((edge = checkedge(12,a,b,14,50)) == 0) &&
		((edge = checkedge(14,a,b,16,34)) == 0) &&
		((edge = checkedge(16,a,b,21,37)) == 0) &&
		((edge = checkedge(18,a,b,23,46)) == 0) &&
		((edge = checkedge(20,a,b,30,43)) == 0)
		)
		edge = checkedge(22,a,b,32,52);

	return (edge);
	}

void edgesequence (int seq)
	{
	short i,j;
	unsigned char u;
	
	if (seq < 24)
		for (i = 1; i <= edge1[seq][0]; i++)
			output (edge1[seq][i]);
	else
		for (i = 1; i <= edge2[seq-24][0]; i++)
			{
			if ((u = edge2[seq-24][i]) < 19)
				output (u);
			else
				for (j = 1; j <= edge3[u-19][0]; j++)
					output (edge3[u-19][j]);
			}
	}

struct IntuiMessage *edgefix(short up, short right, short down)
	{
	short i;

	if ((findedge(right,up) == 1) && (findedge(right,down) == 7))
		return (NULL);
	
	if ((i = findedge(down,right)) == 0)
		{
		failed = 1;
		return (NULL);
		}
	
	if (i > 1)
		edgesequence(i-2);
	
	if (NULL != (msg = flush()))
		return (msg);

	if ((i = findedge(right,up)) < 3)
		{
		failed = 1;
		return (0);
		}
	
	edgesequence(i + 21);
	
	return (flush());
	}

struct IntuiMessage *solve()
	{
	short i,j;
	
	if (NULL != (msg = flush()))
		return (msg);

/*	get right corners correct	*/

	if ((i = findcorner(4,22,40)-2) < -1)
		return (NULL);
	
	if (i >= 0)
		for (j=1; j<=corner1[i][0]; j++)
			output (corner1[i][j]);

	if (NULL != (msg = flush()))
		return (msg);
		
	if ((i = findcorner(4,49,22)-5) < -1)
		return (NULL);
	
	if (i >= 0)
		for (j=1; j<=corner2[i][0]; j++)
			output (corner2[i][j]);

	if (NULL != (msg = flush()))
		return(msg);

	if ((i = findcorner(4,40,31)-8) < -1)
		return (NULL);
	
	if (i >= 0)
		for (j=1; j<=corner3[i][0]; j++)
			output (corner3[i][j]);

	if ((msg = flush()))
		return(msg);

	if ((i = findcorner(4,31,49)-11) < -1)
		return (NULL);
	
	if (i >= 0)
		for (j=1; j<=corner4[i][0]; j++)
			output (corner4[i][j]);

	if (NULL != (msg = flush()))
		return(msg);

/*	get left corners in position	*/

	if ((i = findcorner(13,40,22)-13) < 0)
		return (NULL);
	
	switch (i/3)
		{
		case 0:
			break;
		case 1:
			output (5);
			break;
		case 2:
			output (4);
			break;
		case 3:
			output (6);
			break;
		default:
			return (NULL);
		}
	
	if (NULL != (msg = flush()))
		return(msg);

	if ((i = findcorner(13,22,49)-16) < 0)
		return (NULL);
	
	switch (i/3)
		{
		case 0:
			break;
		case 1:
			output (5);
			output (8);
			output (4);
			output (11);
			output (5);
			output (7);
			output (4);
			output (10);
			break;
		case 2:
			output (11);
			output (5);
			output (8);
			output (4);
			output (10);
			output (5);
			output (7);
			output (4);
			break;
		default:
			return (NULL);
		}

	if (NULL != (msg = flush()))
		return(msg);

	if ((i = findcorner(13,31,40)-19) < 0)
		return (NULL);
	
	switch (i/3)
		{
		case 0:
			break;
		case 1:
			output (4);
			output (17);
			output (5);
			output (14);
			output (4);
			output (16);
			output (5);
			output (13);
			output (4);
			break;
		default:
			return (NULL);
		}

	if (NULL != (msg = flush()))
		return(msg);

/*	get left corners oriented	*/

	if ((i = findcorner(13,40,22)-13) < 0)
		return (NULL);
	
	switch (i)
		{
		case 0:
			break;
		case 1:
			cornerseq (1);
			
			if ((msg = flush()))
				return (msg);
			
			if (findcorner(13,22,49)==18)
				{
				output (5);
				cornerseq (0);
				output (4);
				}
			else if (findcorner(13,31,40)==21)
				{
				output (4);
				cornerseq (0);
				output (5);
				}
			else
				{
				output (6);
				cornerseq (0);
				output (6);
				}
			
			break;
		case 2:
			cornerseq (0);
			
			if (NULL != (msg = flush()))
				return (msg);

			if (findcorner(13,22,49)==17)
				{
				output (5);
				cornerseq (1);
				output (4);
				}
			else if (findcorner(13,31,40)==20)
				{
				output (4);
				cornerseq (1);
				output (5);
				}
			else
				{
				output (6);
				cornerseq (1);
				output (6);
				}
			
			break;
		default:
			return (NULL);
		}

	if (NULL != (msg = flush()))
		return(msg);

	if ((i = findcorner(13,22,49)-16) < 0)
		return (NULL);
	
	switch (i)
		{
		case 0:
			break;
		case 1:
			cornerseq (3);
			
			if (NULL != (msg = flush()))
				return (msg);

			if (findcorner(13,31,40)==21)
				{
				output (6);
				cornerseq (2);
				output (6);
				}
			else
				{
				output (5);
				cornerseq (2);
				output (4);
				}
			
			break;
		case 2:
			cornerseq (2);
			
			if (NULL != (msg = flush()))
				return (msg);

			if (findcorner(13,31,40)==20)
				{
				output (6);
				cornerseq (3);
				output (6);
				}
			else
				{
				output (5);
				cornerseq (3);
				output (4);
				}
			
			break;
		default:
			return (NULL);
		}

	if (NULL !=( msg = flush()))
		return(msg);

	if ((i = findcorner(13,31,40)-19) < 0)
		return (NULL);
	
	switch (i)
		{
		case 0:
			break;
		case 1:
			cornerseq (5);
			output (4);
			cornerseq (4);
			output (5);
			break;
		case 2:
			cornerseq (4);
			output (4);
			cornerseq (5);
			output (5);
			break;
		default:
			return (NULL);
		}

	if ((msg = flush()))
		return (msg);

	if ((i = findcorner(13,49,31)) != 22)
		return (NULL);

/*	make top and bottom edges correct	*/
	
	failed = 0;
	
	if ((msg = edgefix(22,4,31)))
		return (msg);
	
	if (failed)
		return (NULL);
	
	output (7);
	output (10);
	
	if ((msg = flush()))
		return(msg);

	if ((msg = edgefix(22,40,31)))
		return (msg);
	
	if (failed)
		return (NULL);

	output (7);
	output (10);
	
	if ((msg = flush()))
		return (msg);

	if ((msg = edgefix(22,13,31)))
		return (msg);
	
	if (failed)
		return (NULL);

	output (7);
	output (10);
	
	if ((msg = flush()))
		return(msg);

	if ((msg = edgefix(22,49,31)))
		return (msg);
	
	if (failed)
		return (NULL);

	output (7);
	output (10);
	
	if ((msg = flush()))
		return(msg);

/*	get middle slice edges into position	*/

	switch (findedge(4,40))
		{
		case 3:
		case 4:
			break;
		case 5:
		case 6:
			edgeseq (0);
			break;
		case 11:
		case 12:
			edgeseq (1);
			break;
		case 13:
		case 14:
			edgeseq (2);
			break;
		default:
			return (NULL);
		}

	if ((msg = flush()))
		return(msg);

	switch (findedge(13,49))
		{
		case 5:
		case 6:
			edgeseq (3);
			break;
		case 11:
		case 12:
			edgeseq (4);
			break;
		case 13:
		case 14:
			break;
		default:
			return (NULL);
		}
	
	if ((msg = flush()))
		return(msg);

/*	get middle edges oriented	*/

	if (findedge(4,40) == 4)
		{
		edgeseq2 (0);
		
		if ((msg = flush()))
			return(msg);

		if (findedge(4,49) == 6)
			{
			output (7);
			output (10);
			edgeseq2 (3);
			output (8);
			output (11);
			}
		else if (findedge(13,40) == 12)
			{
			output (8);
			output (11);
			edgeseq2 (4);
			output (7);
			output (10);
			}
		else
			{
			output (9);
			output (12);
			edgeseq2 (5);
			output (9);
			output (12);
			}
		}

	if ((msg = flush()))
		return(msg);

	if (findedge(4,49) == 6)
		{
		edgeseq2 (1);
		
		if ((msg = flush()))
			return(msg);

		if (findedge(13,40) == 12)
			{
			output (9);
			output (12);
			edgeseq2 (4);
			output (9);
			output (12);
			}
		else
			{
			output (7);
			output (10);
			edgeseq2 (5);
			output (8);
			output (11);
			}
		}

	if ((msg = flush()))
		return(msg);

	if (findedge(13,40) == 12)
		{
		edgeseq2 (2);
		output (8);
		output (11);
		edgeseq2 (5);
		output (7);
		output (10);
		}

	if ((msg = flush()))
		return(msg);

/*	if (findedge(13,49) != 13)
		return (NULL);
*/
	return (NULL);
	}



