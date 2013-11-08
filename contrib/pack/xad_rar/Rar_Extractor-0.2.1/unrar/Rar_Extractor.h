
// RAR archive extractor

// Rar_Extractor 0.2.1. See unrar_license.txt.

#ifndef RAR_EXTRACTOR_H
#define RAR_EXTRACTOR_H

#include "abstract_file.h"

#ifndef __amigaos4__
typedef long long int64;
#endif

// Must be defined by user
extern "C" void rar_out_of_memory();

extern const char not_rar_file [];
extern const char end_of_rar [];

class Rar_Extractor {
public:
	Rar_Extractor();
	~Rar_Extractor();
	
	// NULL on success, otherwise error string (or other special status)
	typedef const char* error_t;
	
	// Close current file and open new RAR file. NULL if valid RAR archive,
	// not_rar_file if file is not an RAR archive, otherwise error string.
	// Keeps pointer to reader until close().
	error_t open( File_Reader* );
	
	// Enable faster scanning of solid archives, with the limitation
	// that no further extraction can be performed until rewind() is
	// called.
	void scan_only();
	
	// Access next item in archive. NULL on success, end_of_rar if no more items,
	// otherwise error string. After opening or rewinding archive, this must be
	// called to get the first item.
	error_t next();
	
	// Information about current item
	struct info_t {
		const char* name;
		unsigned long date; // DOS format, 0 if unavailable
		bool is_file;
		long size;
		int64 packsize;
		unsigned long attrs;
	};
	const info_t& info() const;
	
	// Extract current file
	error_t extract( Data_Writer& );
	
	// Go back to beginning of archive and re-allow extraction
	error_t rewind();
	
	// Close archive
	void close();
	
	// End of public interface
private:
	struct Impl; // prevents RAR internals from having to be #included here
	Impl* impl;
	File_Reader* reader;
	enum action_t { no_action, seek_next, skip_extraction };
	action_t action;
	bool allow_extraction;
	info_t info_;
};

inline void Rar_Extractor::scan_only() { allow_extraction = false; }

inline const Rar_Extractor::info_t& Rar_Extractor::info() const { return info_; }

#endif

