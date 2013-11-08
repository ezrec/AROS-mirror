
/* C interface to Rar_Extractor, using unrarlib 0.4.0 (UniquE RAR File Library) interface */

#ifndef URARLIB_H
#define URARLIB_H

#ifdef __cplusplus
	extern "C" {
#endif

struct xadArchiveInfo;

struct RAR20_archive_entry
{
	char* Name;
	unsigned short NameSize;
	unsigned long PackSize;
	unsigned long UnpSize;
	unsigned char HostOS;
	unsigned long FileCRC;
	unsigned long FileTime;
	unsigned char UnpVer;
	unsigned char Method;
	unsigned long FileAttr;
};

typedef struct ArchiveList_struct
{
	struct RAR20_archive_entry item;
	struct ArchiveList_struct* next; // next entry in list, or NULL if end of list
	
} ArchiveList_struct;

/* Allocate and set *list_out to first node of linked list of files from RAR archive at
   'rar_path' and return number of items in list. Use urarlib_freelist() to free memory
   used by the list.
*/
int urarlib_list( ArchiveList_struct** list_out, void *xadLib, struct xadArchiveInfo *ai );

/* Extract file 'filename' from an RAR archive at 'rar_path' into internal buffer.
   Sets *buffer_out to beginning of buffer and *size_out to number of bytes extracted.
   Returns true if successful otherwise false. Password is not supported (must be NULL).
   Caller must free returned buffer when done with it.

   Note: output is treated as void**, but is void* here to maintain urarlib.h compatibility.
*/
int urarlib_get( unsigned long* size_out, char* filename,
		void *, struct xadArchiveInfo * );

/* Free memory used by list of nodes returned by urarlib_list(). Pointer must be that
   returned by urarlib_list() (that is, the first node in the list).
*/
void urarlib_freelist( ArchiveList_struct* list );

#ifdef __cplusplus
	}
#endif

#endif

