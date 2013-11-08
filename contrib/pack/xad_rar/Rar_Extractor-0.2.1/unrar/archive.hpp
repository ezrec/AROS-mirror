
// Rar_Extractor 0.2.1. See unrar_license.txt.

#ifndef _RAR_ARCHIVE_
#define _RAR_ARCHIVE_

class Pack;

class Archive {
public:

	Archive( File_Reader* );
	const char* IsArchive();
	const char* ReadHeader();
	void SeekToNext();
	int GetHeaderType() {return(CurHeaderType);};

	BaseBlock ShortBlock;
	MainHeader NewMhd;
	FileHeader NewLhd;
	EndArcHeader EndArcHead;
	SubBlockHeader SubBlockHead;
	FileHeader SubHead;
	CommentHeader CommHead;
	ProtectHeader ProtectHead;
	AVHeader AVHead;
	SignHeader SignHead;
	UnixOwnersHeader UOHead;
	MacFInfoHeader MACHead;
	EAHeader EAHead;
	StreamHeader StreamHead;

	Int64 CurBlockPos;
	Int64 NextBlockPos;

	bool OldFormat;
	bool Solid;
	unsigned int SFXSize;

	ushort HeaderCRC;

	bool IsOpened() const { return true; }
	void Close() { }
	
	Int64 FileLength() { return rar_reader->size(); }
	
	void Seek( Int64 pos ) { rar_reader->seek( pos ); }
	Int64 Tell() { return rar_reader->tell(); }
	
	long Read( void* p, long s ) { return rar_reader->read_avail( p, s ); }
	
private:
	File_Reader* rar_reader;
	
	MarkHeader MarkHead;
	OldMainHeader OldMhd;

	int LastReadBlock;
	int CurHeaderType;

	bool IsSignature(byte *D);
	const char* ReadOldHeader();
};

#endif

