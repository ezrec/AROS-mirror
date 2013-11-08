
// Rar_Extractor 0.2.1. See unrar_license.txt.

#ifndef _RAR_DATAIO_
#define _RAR_DATAIO_

class Archive;

class Unpack;

class ComprDataIO {
public:
	
	ComprDataIO();
	void Init();
	int UnpRead(byte *Addr,uint Count);
	void UnpWrite(byte *Addr,uint Count);
	void GetUnpackedData(byte **Data,uint *Size);
	void SetPackedSizeToRead(Int64 Size) {UnpPackedSize=Size;}
	void SetSkipUnpCRC(bool Skip) {SkipUnpCRC=Skip;}
	
	void SetFiles( Archive*, Rar_Writer* );
	
	void SetSubHeader(FileHeader *hd,Int64 *Pos) {SubHead=hd;SubHeadPos=Pos;}
	void SetUnpackToMemory(byte *Addr,uint Size);

	bool UnpVolume;
	Int64 CurUnpRead,CurUnpWrite;

	uint PackFileCRC,UnpFileCRC,PackedCRC;

private:
	Int64 UnpPackedSize;

	bool SkipUnpCRC;
	
	Archive* archive;
	Rar_Writer* rar_writer;

	FileHeader *SubHead;
	Int64 *SubHeadPos;
};

#endif
