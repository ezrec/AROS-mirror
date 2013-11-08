
// Rar_Extractor 0.2.1. See unrar_license.txt.

#include "rar.hpp"

#include <new>
#include <assert.h>
#include <string.h>

const char not_rar_file [] = "Not an RAR archive";
const char end_of_rar [] = "End of RAR reached";

struct Rar_Extractor::Impl
{
	Archive arc;
	ComprDataIO DataIO;
	bool first_file;
	Unpack Unp;
	
	const char* extract( Data_Writer&, bool check_crc = true );
	
	Impl( File_Reader* in ) :
		arc( in ),
		Unp( &DataIO )
	{
		first_file = true;
	}
};

Rar_Extractor::Rar_Extractor() : impl( NULL )
{
}

const char* Rar_Extractor::rewind()
{
	assert( impl );
	File_Reader* saved_reader = reader;
	close();
	return open( saved_reader );
}

const char* Rar_Extractor::open( File_Reader* in )
{
	close();
	
	reader = in;
	impl = new (std::nothrow) Impl( in );
	if ( !impl ) {
		rar_out_of_memory();
		return "Out of memory";
	}
	
	impl->Unp.Init( NULL );
	
	error_t error = impl->arc.IsArchive();
	if ( error ) {
		close();
		return error;
	}
	
	action = no_action;
	allow_extraction = true;
	
	return NULL;
}

Rar_Extractor::~Rar_Extractor()
{
	close();
}

const char* Rar_Extractor::next()
{
	switch ( action )
	{
		case skip_extraction:
			if ( allow_extraction && info_.is_file && impl->arc.Solid )
			{
				// if solid archive, *must* extract data since it accumulates internally
				Null_Writer out;
				const char* error = impl->extract( out, false );
				if ( error )
					return error;
			}
			// fall through
		case seek_next:
			impl->arc.SeekToNext();
			break;
	}
	
	action = skip_extraction;
	
	for ( error_t error; (error = impl->arc.ReadHeader()) != end_of_rar;
			impl->arc.SeekToNext() )
	{
		if ( error )
			return error;
		
		HEADER_TYPE type = static_cast<HEADER_TYPE> (impl->arc.GetHeaderType());
		
		if ( type == ENDARC_HEAD )
			break;
		
		// to do: log untested block types
		
		// skip non-files
		if ( type != FILE_HEAD )
			continue;
		
		// skip label
		if ( impl->arc.NewLhd.HostOS <= HOST_WIN32 && (impl->arc.NewLhd.FileAttr & 8) )
			continue;
		
		// skip links
		if ( (impl->arc.NewLhd.FileAttr & 0xF000) == 0xA000 )
			continue;
		
		info_.is_file = (impl->arc.NewLhd.Flags & LHD_WINDOWMASK) != LHD_DIRECTORY;
		info_.name = impl->arc.NewLhd.FileName;
		info_.size = (info_.is_file ? impl->arc.NewLhd.UnpSize : 0);
		info_.date = impl->arc.NewLhd.FileTime;
		info_.packsize = impl->arc.NewLhd.FullPackSize;
		info_.attrs = impl->arc.NewLhd.FileAttr;
		return NULL; // success
	}
	
	return end_of_rar;
}
	
const char* Rar_Extractor::extract( Data_Writer& out )
{
	assert( info_.is_file );
	action = seek_next;
	return impl->extract( out );
}
	
static void UnstoreFile(ComprDataIO &DataIO,Int64 DestUnpSize)
{
	Rar_Array<byte> Buffer( 0x10000 );
	while ( true )
	{
		unsigned int result = DataIO.UnpRead( &Buffer[0], Buffer.Size() );
		if ( result == 0 || (int) result == -1 )
			break;
		
		result = (result < DestUnpSize ? result : int64to32( DestUnpSize ));
		DataIO.UnpWrite( &Buffer[0], result );
		if ( DestUnpSize >= 0 )
			DestUnpSize -= result;
	}
}

const char* Rar_Extractor::Impl::extract( Data_Writer& rar_writer, bool check_crc )
{
	assert( arc.GetHeaderType() == FILE_HEAD );
	
	if ( arc.NewLhd.Flags & (LHD_SPLIT_AFTER | LHD_SPLIT_BEFORE) )
		return "Segmented RAR not supported";
	
	if ( arc.NewLhd.Flags & LHD_PASSWORD )
		return "Encrypted RAR archive not supported";
	
	if ( arc.NewLhd.UnpVer < 13 || arc.NewLhd.UnpVer > UNP_VER )
		return "RAR file uses an unsupported format (too old or too recent)";
	
	arc.Seek( arc.NextBlockPos - arc.NewLhd.FullPackSize );
	
	DataIO.CurUnpRead=0;
	DataIO.CurUnpWrite=0;
	DataIO.UnpFileCRC = arc.OldFormat ? 0 : 0xffffffff;
	DataIO.PackedCRC = 0xffffffff;
	DataIO.SetPackedSizeToRead( arc.NewLhd.FullPackSize );
	DataIO.SetFiles( &arc, &rar_writer );
	DataIO.SetSkipUnpCRC( !check_crc );

	if ( arc.NewLhd.Method == 0x30 ) {
		UnstoreFile( DataIO, arc.NewLhd.FullUnpSize );
	}
	else {
		Unp.SetDestSize( arc.NewLhd.FullUnpSize );
		if ( arc.NewLhd.UnpVer <= 15 )
			Unp.DoUnpack( 15, arc.Solid && !first_file );
		else
			Unp.DoUnpack( arc.NewLhd.UnpVer, arc.NewLhd.Flags & LHD_SOLID);
	}
	
	first_file = false;
	
	arc.SeekToNext();
	
	if ( check_crc )
	{
		unsigned long correct = arc.NewLhd.FileCRC;
		if ( !arc.OldFormat )
			correct = ~correct;
		
		if ( UINT32( DataIO.UnpFileCRC ) != UINT32( correct ) )
			return "CRC error";
	}
	
	return NULL;
}

void Rar_Extractor::close()
{
	if ( impl ) {
		impl->arc.Close();
		delete impl;
		impl = NULL;
	}
	
	reader = NULL;
	
	memset( &info_, 0, sizeof info_ );
}

