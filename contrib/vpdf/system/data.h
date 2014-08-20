#ifndef	DATA_H
#define	DATA_H

#include <stdio.h>

typedef	struct filenode_t
{

	struct filenode_t *next;   /* pointer to the next node */
	struct datahandle_t *data; /* a datafile this file belongs to */
	char *name;			  	   /* comple path */
	int	size;			   	   /* */
	int	offset;		   		   /* offset in the disk file when the logical file starts */

} FileNode;


typedef	struct datahandle_t
{
	struct datahandle_t	*next;
	FILE *sourceFile;	      /* disk opened file */
	FileNode *files; 		  /* list of file headers */

} DataHandle;

/* type of file the filehandle describes */

enum
{
	FH_ARCHIVE,
	FH_DISK
};

typedef	struct filehandle_t
{
	int	type;	        		/* FX_xxx */
	/* for FH_ARCHIVE */
	DataHandle *data;	 		/* data where the file resists */
	FileNode *file;	   			/* node describing the file attributes */

	/* for FH_NORMAL */

	FILE *diskFile;

	/* buffering veriables (used mainly with FH_ARCHIVE */

	int	position;	       	   /* current file position */
	char *buffer;	      	   /* buffer used for faster reading */
	int	bufferPosition;        /* position in the buffer */
	int	bufferSize;
	int	bufferBytes;       	   /* number of bytes in the buffer */

} FileHandle;

DataHandle *dataOpen(char *fname);
void dataClose(DataHandle *data);
void dataCloseAll(void);
void *dataLoadFile(char	*fname);
unsigned int dataFileLength(char *fname);
FileHandle *dataFileOpen(char *fname);
void dataFileClose(FileHandle *handle);
void *dataFileGets(FileHandle *handle, void *buf, int len);
int	dataFileRead(FileHandle	*handle, void *buf, int len);
void dataExportRequestedFiles(char *fname);
void dataStoreRequestedFiles(int enable);

#endif
