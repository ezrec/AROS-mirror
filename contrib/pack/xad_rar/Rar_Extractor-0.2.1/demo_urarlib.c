
/* Shows how to use C interface to Rar_Extractor */

#include "unrarlib/unrarlib.h"

#include <stdlib.h>
#include <stdio.h>

int main( int argc, char** argv )
{
	/* Path of RAR archive */
	char* path = (argc > 1 ? argv [1] : "test.rar");
	
	/* Get list of files in archive */
	ArchiveList_struct* list = NULL;
	int count = urarlib_list( path, &list );
	ArchiveList_struct* node;
	
	printf( "File count: %d\n", count );
	
	/* Iterate through each file */
	for ( node = list; node; node = node->next )
	{
		/* Extract current file */
		char* data = NULL;
		unsigned long size = 0;
		int success = urarlib_get( &data, &size, node->item.Name, path, NULL );
		if ( !success )
		{
			printf( "Error extracting %s\n", node->item.Name );
		}
		else
		{
			/* Print first 20 bytes of file */
			printf( "File: %s (%d bytes). First 20 bytes: %.20s\n",
					node->item.Name, size, data );
			
			/* Caller must free data */
			free( data );
		}
	}
	
	/* Free memory used by file list */
	urarlib_freelist( list );
	
	return 0;
}

