/*	trig.c	fast integer trig functions etc. for rubik.c
	martin round	xmas eve 1989
*/

#include <math.h>

short sintab [360];

void init_trig()
	{
	double degtorad;
	int i;

	degtorad = 3.14159/180.0;
	for (i=1; i<90; i++)
		{
		sintab[i] = sintab[180-i] = (short) (0.5 + 256.0 * sin(degtorad * i));
		sintab[i+180] = sintab[360-i] = -sintab[i];
		}
	sintab[0] = 0;
	sintab[90] = 256;
	sintab[180] = 0;
	sintab[270] = -256;

	}

short mysin(int ang)	/*	works with degrees! return is 256*sin(ang)	*/
	{
	
	if (ang < 0)
		return (sintab [ang + 360]);

	if (ang >359)
		return (sintab [ang - 360]);

	return (sintab[ang]);
	}

short mycos(int ang)
	{
	if (ang < -90)
		return (sintab [ang + 450]);
	
	if (ang >269)
		return (sintab [ang - 270]);
	
	return (sintab [ang + 90]);
	}

unsigned short hypoten ( short x, short y)
	{
	register unsigned long square = x * x + y * y;
	register unsigned short root = x<0 ? -x : x;
	register unsigned short correction;
	
	root += y<0 ? -y+1 : y+1;

	do
		{
		root -= (correction = (root * root - square) / (root * 2));
		} while (correction);

	return (--root);
	}

short angle(short x, short y, unsigned short r)	/*	find vector */
	{
	register short ang,s,e,sine;
	short flag;
	
	if ((r * x * y) == 0)
		{
		if (r == 0)
			return (0);
		if (x == 0)
			{
			if (y >=0)
				return (90);
			else
				return (-90);
			}
		if (x >=0)
			return (0);
		else
			return (180);
		}
	
	ang = y<0 ? -y : y;
	
	s = x<0 ? -x : x;
	
	if ((flag = (s < ang)))
		ang = s;
	
	sine  = (ang << 8) / r;

	ang = 21;
	s = 92;		/*	sintab [21]		*/
	e = sine - s;
	
	while ((e < -2) || (e > 2))
		{
		if (e > 4)
			ang += ((46-ang)*e)/(184-s);
		else if (e < -4)
			ang -= ((1-ang)*e)/(s-4);
		else if (e < 0)
			{
			--ang;
			break;
			}
		else
			{
			++ang;
			break;
			}

		e = sine-(s = sintab[ang]);
		}

	if (flag)
		ang = 90 - ang;
	
	if (x < 0)
		ang = 180 - ang;
	
	if (y < 0)
		ang = -ang;
	
	return (ang);
	}
	
void rotate (short *a, short *b, short ang)
	{
	static short olda,oldb,oldang,olda1,oldb1;
	unsigned short r;

	if ((*a == olda) && (*b == oldb) && (ang == oldang))
		{
		*a = olda1;
		*b = oldb1;
		}
	else
		{
		olda = *a;
		oldb = *b;
		oldang = ang;

		ang += angle (*a, *b, (r = hypoten (*a, *b)));

		*a = olda1 = (r * mycos(ang)) >> 8;
		*b = oldb1 = (r * mysin(ang)) >> 8;
		}
	}
