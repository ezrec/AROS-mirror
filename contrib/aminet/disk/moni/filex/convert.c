#ifdef USE_PROTO_INCLUDES
#include <exec/types.h>
#include <dos/dos.h>
#include <intuition/intuition.h>

#include <proto/utility.h>
#include <ctype.h>

#endif

#include "filexstructs.h"
#include "allprotos.h"

/*
 * LONG ConvertHexstringToHexnumber(UBYTE *s,UBYTE *n)
 *
 * Wandelt die Zeichenkette s in Bytes n um
 * und gibt die Anzahl der Bytes (nachher) zurück
 */

LONG ConvertHexstringToHexnumber(UBYTE *s,UBYTE *n)
{
	UBYTE wert=0;
	BOOL up=TRUE;
	LONG anz=0;

	while(*s!=0)
	{
		if(isxdigit(*s))
		{
			if(isdigit(*s))
			{
				if(up)
				{
					wert=(*s-'0')*0x10;
					up=FALSE;
				}
				else
				{
					wert+=*s-'0';
					*n++=wert;
					up=TRUE;
					anz++;
				}
			}
			else
			{
				if(up)
				{
					wert=(ToUpper(*s)-'A'+0xa)*0x10;
					up=FALSE;
				}
				else
				{
					wert+=ToUpper(*s)-'A'+0xa;
					*n++=wert;
					up=TRUE;
					anz++;
				}
			}
		}
		s++;
	}
	return(anz);
}

int My_stcb_l( char *String, long *Zahl )
{
	int Zeichen = 0;

	*Zahl = 0;

	while(( *String == '0' ) || ( *String == '1' ) || ( *String == ' ' ))
	{
		if( *String != ' ' )
		{
			*Zahl = ( *Zahl << 1L ) + *String - '0';
			Zeichen++;
		}

		String++;
	}

	return( Zeichen );	
}

int My_stcl_b( char *String, long Zahl )
{
	int k;
	int Zeichen = 0;

	if( Zahl == 0 )
	{
		*String++ = '0';
		*String = 0;
		return( 1 );
	}

	for( k = 31; k >= 0; k-- )
	if( Zahl & ( 1L << k))
		break;

	if( k >= 0 )
	{
		*String++ = '1';
		Zeichen++;
		k--;

		for(; k >= 0; k-- )
		{
			if( Zahl & ( 1L << k))
				*String++ = '1';
			else
				*String++ = '0';

			Zeichen++;
		}

		*String = 0;
	}

	return( Zeichen );	
}

