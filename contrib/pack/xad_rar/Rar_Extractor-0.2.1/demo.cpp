
// Use Rar_Extractor to open "test.rar" and list it and possibly extract files.

const int extract = 0; // 0: just list files, 1: extract

#include "unrar/Rar_Extractor.h"

#include <stdlib.h>
#include <stdio.h>

static void check_error( const char* str )
{
	if ( str )
	{
		fprintf( stderr, "Error: %s\n", str );
		exit( EXIT_FAILURE );
	}
}

int main()
{
	// Open file
	Std_File_Reader file;
	check_error( file.open( "test.rar" ) );
	
	// Open archive
	Rar_Extractor rar;
	check_error( rar.open( &file ) );
	
	// If not extracting, use faster scan-only mode
	if ( !extract )
		rar.scan_only();
	
	// Iterate over each item in archive until end is reached
	const char* error;
	while ( (error = rar.next()) != end_of_rar )
	{
		check_error( error );
		
		if ( !rar.info().is_file )
		{
			printf( "Directory %s\n", rar.info().name );
		}
		else
		{
			printf( "File %s\n", rar.info().name );
			
			if ( extract )
			{
				// Create output file
				Std_File_Writer out;
				check_error( out.open( rar.info().name ) );
				
				// Extract to it
				error = rar.extract( out );
				if ( error )
					fprintf( stderr, "Error: %s\n", error );
			}
		}
	}
	
	return 0;
}

