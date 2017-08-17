#include "async.h"


_LIBCALL LONG
ReadLineAsync( _REG( a0 ) AsyncFile *file, _REG( a1 ) APTR buffer, _REG( d0 ) LONG bufSize )
{
	LONG	len;

	/* First read any data up to the LF or the buffer is full */
	if( FGetsLenAsync( file, buffer, bufSize, &len ) )
	{
		UBYTE	*end;

		end = ( ( UBYTE * ) buffer ) + len - 1;

		if( *end != '\n' )
		{
			UBYTE	ch = 0;

			/* We didn't reach EOF yet */
			while( TRUE )
			{
				UBYTE	*ptr;
				LONG	i, count;

				ptr = ( UBYTE * ) file->af_Offset;

				if( count = file->af_BytesLeft )
				{
					/* Scan for LF char in buffer */
					for( i = 0; ( i < count ) && ( *ptr != '\n' ); ++i, ++ptr )
					{
					}

					/* If i < count, then the loop above aborted
					 * due to LF char.
					 */
					if( i < count )
					{
						ch = '\n';
						++i;
					}

					file->af_BytesLeft -= i;
					file->af_Offset    += i;

					if( i < count )
					{
						/* All done */
						break;
					}
				}

				if( ReadAsync( file, &ch, 1 ) < 1 )
				{
					break;
				}

				if( ch == '\n' )
				{
					/* All done */
					break;
				}
			}

			if( ch == '\n' )
			{
				/* Overwrite last char with LF */
				*end++ = '\n';
				*end = '\0';
			}
		}
	}

	return( len );
}
