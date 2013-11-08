
// Rar_Extractor 0.2.1

#include "unrarlib.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <setjmp.h>
#include <proto/exec.h>

#include "unrar/Rar_Extractor.h"

#ifndef MEMF_PRIVATE
#define MEMF_PRIVATE 0
#endif

// Copyright (C) 2005 by Shay Green. MIT license.

// Handling deeply nested out-of-memory without exceptions is
// kind of hacky, and the global jmp_buf makes it thread-UNsafe.
// If C++ exception use is acceptable, rar_out_of_memory() could
// throw an exception that is caught in the wrapper functions.

static jmp_buf jmp_env;

void rar_out_of_memory() // called by Rar_Extractor
{
	longjmp( jmp_env, 1 );
}

// log error message to stderr when debugging
inline const char* log_error( const char* str )
{
	#ifndef NDEBUG
	#ifdef __amigaos4__
	IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
		if ( str && str != end_of_rar )
			IExec->DebugPrintF( "RAR Error: %s\n", str );
	#endif
	#endif
	return str;
}

int urarlib_get( unsigned long* size, char* filename,
		void *xadLib, struct xadArchiveInfo *ai )
{
	*size = 0;
	
	// catch failed construction of rar extractor
	if ( setjmp( jmp_env ) )
		return false;
	
	Rar_Extractor rar;
	XAD_File_Reader file(xadLib, ai);
	if ( setjmp( jmp_env ) )
		goto error;

	if ( log_error( rar.open( &file ) ) )
		goto error;
	
	while ( !log_error( rar.next() ) )
	{
		if ( rar.info().is_file && 0 == strcmp( rar.info().name, filename ) )
		{
			XAD_Writer out( xadLib, ai );
			if ( log_error( rar.extract( out ) ) )
				goto error;
			
			// success
			*size = rar.info().size;
			return true;
		}
	}
	
	 // not found
	 
error:
	return false;
}

int urarlib_list( ArchiveList_struct** list, void *xadLib, struct xadArchiveInfo *ai )
{
	int count = 0;
	ArchiveList_struct* volatile item = NULL;
	*list = NULL;
	#ifdef __amigaos4__
	IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
	#endif
	
	// catch failed construction of rar extractor
	if ( setjmp( jmp_env ) )
		return 0;
	
	Rar_Extractor rar;
	XAD_File_Reader file(xadLib, ai);
	if ( setjmp( jmp_env ) )
		goto error;
	
	if ( log_error( rar.open( &file ) ) )
		goto error;
	
	// significantly faster for solid archives
	rar.scan_only();
	
	while ( true )
	{
		const char* result = rar.next();
		if ( result == end_of_rar )
			return count; // success
		
		if ( log_error( result ) )
			goto error;
		
		if ( rar.info().is_file )
		{
			item = (ArchiveList_struct*) AllocVec( sizeof (ArchiveList_struct), MEMF_PRIVATE | MEMF_CLEAR );
			if ( !item )
				goto error;
			
			item->item.NameSize = strlen( rar.info().name );
			item->item.Name = (char*) AllocVec( item->item.NameSize + 1 ,MEMF_PRIVATE | MEMF_CLEAR);
			if ( !item->item.Name )
				goto error;
			
			strcpy( item->item.Name, rar.info().name );
			item->item.UnpSize = rar.info().size;
	
			item->item.FileTime = rar.info().date;
			item->item.PackSize = (long) rar.info().packsize;
			item->item.FileAttr = rar.info().attrs;

			// insert at end
			*list = item;
			item->next = NULL;
			list = &item->next;
			count++;
		}
	}
	
error:
	FreeVec( item );
	return 0;
}

void urarlib_freelist( ArchiveList_struct* node )
{
	while ( node )
	{
		ArchiveList_struct* next = node->next;
		FreeVec( node->item.Name );
		FreeVec( node );
		node = next;
	}
}

