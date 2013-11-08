
// Rar_Extractor 0.2.1. See unrar_license.txt.

#include "rar.hpp"

Archive::Archive( Rar_Reader* in ) : rar_reader( in )
{
	OldFormat=false;
	Solid=false;
	SFXSize=0;
	LastReadBlock=0;

	CurBlockPos=0;
	NextBlockPos=0;

	memset(&NewMhd,0,sizeof(NewMhd));
	NewMhd.HeadType=MAIN_HEAD;
	NewMhd.HeadSize=SIZEOF_NEWMHD;
	HeaderCRC=0;
}

bool Archive::IsSignature(byte *D)
{
	OldFormat = false;
	if ( D[0] != 0x52 )
		return false;
	
	if (D[1]==0x45 && D[2]==0x7e && D[3]==0x5e) {
		OldFormat = true;
		return true;
	}
	
	if (D[1]==0x61 && D[2]==0x72 && D[3]==0x21 && D[4]==0x1a && D[5]==0x07 && D[6]==0x00)
		return true;
	
	return false;
}

const char* Archive::IsArchive()
{
	if ( Read( MarkHead.Mark,SIZEOF_MARKHEAD ) != SIZEOF_MARKHEAD )
		return not_rar_file;
	
	SFXSize=0;
	if (IsSignature(MarkHead.Mark))
	{
		if (OldFormat)
			Seek(0);
	}
	else
	{
		return not_rar_file;
		
		// to do: Remove? Scans first 256K of file which is somewhat likely
		// to see a phantom rar header in compressed data. Meant for
		// self-extracting archives where executable content is at beginning.
		Rar_Array<char> Buffer(0x40000);
		long CurPos=int64to32(Tell());
		int ReadSize=Read(&Buffer[0],Buffer.Size()-16);
		for (int I=0;I<ReadSize;I++)
			if (Buffer[I]==0x52 && IsSignature((byte *)&Buffer[I]))
			{
				if (OldFormat && I>0 && CurPos<28 && ReadSize>31)
				{
					char *D=&Buffer[28-CurPos];
					if (D[0]!=0x52 || D[1]!=0x53 || D[2]!=0x46 || D[3]!=0x58)
						continue;
				}
				SFXSize=CurPos+I;
				Seek(SFXSize);
				if (!OldFormat)
					Read(MarkHead.Mark,SIZEOF_MARKHEAD);
				break;
			}
		if (SFXSize==0)
			return not_rar_file;
	}
	
	const char* error = ReadHeader();
	if ( error )
		return error;
	
	SeekToNext();
	if (OldFormat) {
		NewMhd.Flags=OldMhd.Flags & 0x3f;
		NewMhd.HeadSize=OldMhd.HeadSize;
	}
	else if (HeaderCRC!=NewMhd.HeadCRC) {
		return "RAR header CRC is wrong";
	}
	
	if ( NewMhd.Flags & MHD_VOLUME )
		return "Split RAR files are not supported";
	
	if ( NewMhd.Flags & MHD_PASSWORD )
		return "Encrypted RAR files are not supported";
	
	Solid = (NewMhd.Flags & MHD_SOLID) != 0;
	
	return NULL;
}

void Archive::SeekToNext()
{
	Seek(NextBlockPos);
}

