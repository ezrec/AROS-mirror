/*
**	GadTools layout toolkit
**
**	Copyright © 1993-1998 by Olaf `Olsen' Barthel
**		Freely distributable.
**
**	:ts=4
*/

#ifndef _GTLAYOUT_GLOBAL_H
#include "gtlayout_global.h"
#endif

#include "Assert.h"

#ifdef DO_HEXHOOK	/* Support code */

	// Check whether the buffer only contains a valid hex/binary/octal number...

BOOL
LTP_ConvertNum(BOOL negAllowed,STRPTR buffer,LONG *value)
{
	ULONG	num;
	LONG	neg;
	LONG	ch;

	num = 0;

	if((buffer[0] == '-') && negAllowed)
	{
		neg = -1;

		buffer++;
	}
	else
		neg = 1;

	if(((buffer[0] == '0') && ToUpper(buffer[1] == 'X')) || (buffer[0] == '$'))
	{
		if(*buffer++ != '$')
			buffer++;

		while((ch = ToUpper(*buffer++)))
		{
			num = num * 16;

			if((ch >= 'A') && (ch <= 'F'))
				num += (ch - 'A') + 10;
			else
			{
				if ((ch >= '0') && (ch <= '9'))
					num += ch - '0';
				else
					return(FALSE);
			}
		}
	}
	else
	{
		if(*buffer == '%')
		{
			buffer++;

			while((ch = *buffer++))
			{
				if((ch < '0') || (ch > '1'))
					return(FALSE);
				else
					num = (num * 2) + (ch - '0');
			}
		}
		else
		{
			if(*buffer == '&')
			{
				buffer++;

				while((ch = *buffer++))
				{
					if((ch < '0') || (ch > '7'))
						return(FALSE);
					else
						num = (num * 8) + (ch - '0');
				}
			}
			else
			{
				while((ch = *buffer++))
				{
					if((ch < '0') || (ch > '9'))
						return(FALSE);
					else
						num = (num * 10) + (ch - '0');
				}
			}
		}
	}

	*value = num * neg;

	return(TRUE);
}

#endif	/* DO_HEXHOOK */
