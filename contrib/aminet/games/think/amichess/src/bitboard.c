#include "common.h"
#include <stdio.h>

BitBoard::BitBoard(unsigned long long i)
{
long *pi=(long *)&i;
hi=pi[0];
lo=pi[1];
}

BitBoard BitBoard::operator<<(int s) const
{
unsigned int rlo=lo;
unsigned int rhi=hi;
while(s)
	{
	if(rlo&0x80000000)
		{
		rlo<<=1;
		rhi<<=1;
		rhi|=1;
		}
	else
		{
		rlo<<=1;
		rhi<<=1;
		}
	s--;
	}
return BitBoard(rhi,rlo);
}

BitBoard BitBoard::operator>>(int s) const
{
unsigned int rlo=lo;
unsigned int rhi=hi;
while(s)
	{
	if(rhi&1)
		{
		rlo>>=1;
		rhi>>=1;
		rlo|=0x80000000;
		}
	else
		{
		rlo>>=1;
		rhi>>=1;
		}
	s--;
	}
return BitBoard(rhi,rlo);
}

BitBoard &BitBoard::operator<<=(int s)
{
while(s)
	{
	if(lo&0x80000000)
		{
		lo<<=1;
		hi<<=1;
		hi|=1;
		}
	else
		{
		lo<<=1;
		hi<<=1;
		}
	s--;
	}
return *this;
}

