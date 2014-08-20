
#ifndef	FILE_H
#define	FILE_H

#include <exec/types.h>
#include <dos/dos.h>
#include <intuition/classusr.h>

#include "chunky.h"

#ifdef __cplusplus
extern "C" {
#endif

void *fileLoad(char *name);
void fileFree(void *mem);
char *fileGetFilePart(char	*fname);
unsigned int fileLength(char *fname);
#define FileLen fileLength

#define	loadfile(name) loadfileN(name)

Texture	*AllocTextureStruct(void);
void FreeTexture(Texture *txt);
#define	txtFree	FreeTexture

Texture	*LoadChunkyPicture(char	*name,int width,int	height);
Texture	*LoadPicture(char *fname);
Texture	*LoadPictureHI16(char *fname);
Texture	*LoadPictureTRUE24(char *fname);
Texture	*LoadPictureTRUE32(char *fname);
Texture	*LoadPictureGRAY8(char *fname);
Texture	*LoadPictureARGB32(char *fname);
Texture	*LoadPictureARGB16(char *fname);


Texture	*txt_CreateBlank(int width,int height);
Texture	*txt_CreateBlank16(int width,int height);
Texture	*txt_CreateBlank24(int width,int height);
Texture	*txt_CreateBlank32(int width,int height);

Texture	*txtCreate(int width, int height, int pixfmt);
Texture	*txtCreateFromImage(ChkImage *image);
Texture	*txtCreateBlank(int width, int height, int format);

int savefile(char *name,void *mem,int len);

int jpegGetDimmensions(char *fname, int *width, int *height);
struct texture *LoadPictureJPEG  (char *fname);
struct texture *LoadPictureJPEG24(char *fname);
struct texture *LoadPictureJPEG24Scaled(char   *fname, int minwidth, int minheight, int *realwidth, int *realheight);
struct texture *LoadPictureJPEG8 (char *fname);
struct texture *LoadPictureJPEG32(char *fname);
struct texture *LoadPictureJPEGThumbnail(char *fname, int *realwidth, int *realheight, int minwidth, int minheight);

#define	LoadPictureJPEG16(fname) LoadPictureJPEG(fname)

void txtReloadImage(Texture	*txt, char *name);

void SetDebugMode(int mode);

int	isDTPicture(char *fname);
int	checkExtension(char *fname, char  *extension);

char *getLastFileType(void);
void fileGetType(char *fname, char *type);

typedef	struct FileInfo_s
{
	unsigned int gid; /* from datatypes GID_xx */
	char *type;	/* */
	struct DateStamp datestamp;
	unsigned int filesize;
	int width; /* != if available */
	int height;	/* */
} FileInfo;

#define FINFO_DATATYPE 1
#define FINFO_DIMMENSIONS 2

FileInfo *fileGetInfo(char *fname, int flags);
void fileFreeInfo(FileInfo *finfo);

void dirDelete(char *path, int recursive);
int fileCopyTo(char *src, char *dst);
int pathEquals(char *p1, char *p2);
int fileDelete(char *src);
int filesCopy(Object *app, char *reqtitle, char **list, char *dest, int move);

char *CapacityFormat(char *s, unsigned int size, unsigned long long n);

#ifdef __cplusplus
}
#endif

#endif
