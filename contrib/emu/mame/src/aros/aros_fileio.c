#include "driver.h"
#include "unzip.h"

#include <exec/exec.h>
#include <dos/dos.h>
#include <proto/exec.h>
#include <proto/dos.h>

#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>

#include <zlib.h>

#include "aros_fileio.h"
#include "aros_config.h"
#include "aros_main.h"

/************************************************************************************/

#define MAlloc(a) AllocVec(a,MEMF_PUBLIC|MEMF_CLEAR)
#define Free(a)   FreeVec(a)

/************************************************************************************/

char *nvdir, *hidir, *cfgdir, *inpdir, *stadir, *memcarddir;
char *artworkdir, *screenshotdir, *alternate_name;

/************************************************************************************/

static char ROMZipName[256];
static char SampleZipName[256];

/************************************************************************************/

static struct ArosFile *OpenFile(const char *dir_name, const char *file_name, int mode)
{
    struct ArosFile *file;
    BPTR    lock;
    LONG    i;

    file = MAlloc(sizeof(struct ArosFile));

    if(file)
    {
	i = strlen(dir_name);

	if(i)
	{
	    strcpy(file->Name, dir_name);
	    AddPart(file->Name, (char *)file_name, 255);

	    file->File  = Open(file->Name, mode);
	    file->Type  = FILETYPE_NORMAL;

	    if(!file->File && i && (mode == MODE_OLDFILE))
	    {
        	sprintf(&file->Name[i], ".zip");

        	lock = Lock(file->Name, ACCESS_READ);

        	if(lock)
        	{
        	    file->File  = (BPTR)~0;
        	    file->Type  = FILETYPE_ZIP;
        	}
        	else
        	{
        	    sprintf(&file->Name[i], ".lha");

        	    lock = Lock(file->Name, ACCESS_READ);

        	    if(lock)
        		sprintf(file->Name, "lha <>NIL: e %s.lha %s T:", dir_name, file_name);
        	    else
        	    {
        		sprintf(&file->Name[i], ".lzx");

        		lock = Lock(file->Name, ACCESS_READ);

        		if(lock)
        		    sprintf(file->Name, "lzx <>NIL: x %s.lzx %s T:", dir_name, file_name);
        	    }

        	    if(lock)
        	    {
			/* AROSFIXME */
        		/* System(file->Name, NULL); */

        		sprintf(file->Name,"T:%s", file_name);

        		file->File  = Open(file->Name, mode);
        		file->Type  = FILETYPE_TMP;
        	    }
        	}

        	if(lock)
        	  UnLock(lock);
	    }
	}
	else
	{
	    file->File  = Open((STRPTR) file_name, mode);
	    file->Type  = FILETYPE_NORMAL;
	}

	if(!file->File)
	{
	    Free(file);
	    file = NULL;

	} else {

	    if (file->Type != FILETYPE_ZIP)
	    {
	        UBYTE *mem;
		
		/* AROSFIXME */
		Seek(file->File, 0, OFFSET_END);
		file->Length = Seek(file->File, 0, OFFSET_BEGINNING);
		
		if ((mem = malloc(file->Length)))
		{
		    if (Read(file->File, mem, file->Length) == file->Length)
		    {
		        file->CRC = crc32(0, mem, file->Length);
		    }
		    Seek(file->File, 0, OFFSET_BEGINNING);
		    
		    free(mem);
		}		
	    }
	}
	
    } /* if(file) */

    return(file);
}

/************************************************************************************/

static struct ArosFile *OpenFileType(const char *dir_name, const char *file_name, int mode, int type)
{
    struct ArosFile *file;
    const char  *path;

    char name[256];
    int  path_len;
    int  path_num;
    BPTR lock;

    file = NULL;

    switch(type)
    {
	case OSD_FILETYPE_ROM:
    #ifdef MESS
	case OSD_FILETYPE_ROM_CART:
	case OSD_FILETYPE_IMAGE:
    #endif
	    for(path_num = 0;
        	(path = GetRomPath(Config[CFG_DRIVER], path_num)) != NULL;
        	path_num++)
	    {
        	path_len = strlen(path);

        	if(path_len)
        	{
        	    strcpy(name, path);
		    AddPart(name, (char *)dir_name, 255);

        	    file = OpenFile(name, (char *)file_name, mode);

        	    if(!file)
        	    {
        	        name[path_len] = 0;
        	        file = OpenFile(name, (char *)file_name, mode);
        	    }
		    
		    if (file) break;
        	}
		
	    }

	    if(!file)
	    {
                sprintf(name, "roms/%s", dir_name);
                file = OpenFile(name, file_name, mode);
	    }

	    if(!file)
                file = OpenFile(dir_name, file_name, mode);

	    return(file);

	case OSD_FILETYPE_SAMPLE:
	    for(path_num = 0;
        	(path = GetSamplePath(Config[CFG_DRIVER], path_num)) != NULL;
        	path_num++)
	    {
        	path_len = strlen(path);

        	if(path_len)
        	{
        	    strcpy(name, path);
		    AddPart(name, (char *)dir_name, 255);
		    
        	    file = OpenFile(name, file_name, mode);

        	    if(!file)
        	    {
        	        name[path_len] = 0;
        	        file = OpenFile(name, file_name, mode);
        	    }
		    
		    if (file) break;
        	}
	    }

	    if(!file)
	    {
                sprintf(name, "samples/%s", dir_name);
                file = OpenFile(name, file_name, mode);
	    }

	    for(path_num = 0;
        	(path = GetRomPath(Config[CFG_DRIVER], path_num));
        	path_num++)
	    {
        	path_len = strlen(path);

        	if(path_len)
        	{
        	    strcpy(name, path);
		    AddPart(name, (char *)dir_name, 255);
		    
       	    	    file = OpenFile(name, file_name, mode);

        	    if(!file)
        	    {
        	        name[path_len] = 0;
        	        file = OpenFile(name, file_name, mode);
        	    }
		    
		    if (file) break;
        	}
	    }

	    if(!file)
	    {
                sprintf(name, "roms/%s", dir_name);
                file = OpenFile(name, file_name, mode);
	    }

	    if(!file)
                file = OpenFile(dir_name, file_name, mode);

	    if(!file)
                file = OpenFile("roms", file_name, mode);

	    if(!file)
                file = OpenFile("", file_name, mode);

	    return(file);

	case OSD_FILETYPE_HIGHSCORE:
	    if(mode == MODE_NEWFILE)
	    {
              lock = Lock("hi", ACCESS_READ);

              if(!lock)
        	  lock = CreateDir("hi");

              if(lock)
        	  UnLock(lock);       
	    }
	    sprintf(name, "%s.hi", dir_name);
	    return(OpenFile("hi", name, mode));

	case OSD_FILETYPE_CONFIG:
	    if(mode == MODE_NEWFILE)
	    {
        	lock = Lock("cfg", ACCESS_READ);

        	if(!lock)
        	    lock = CreateDir("cfg");

        	if(lock)
        	    UnLock(lock);
	    }
	    sprintf(name, "%s.cfg", dir_name);
	    return(OpenFile("cfg", name, mode));

	case OSD_FILETYPE_INPUTLOG:
	    sprintf(name, "%s.inp", file_name);
	    return(OpenFile("", name, mode));

	case OSD_FILETYPE_SCREENSHOT:
	    strcpy(name, screenshotdir);
	    AddPart(name, (char *)file_name, 255);
	    return(OpenFile("", name, mode));
	    
	default:
	    return(NULL);
    }
}
/************************************************************************************/

static void CloseFile(struct ArosFile *file)
{
    if(file->Type != FILETYPE_CUSTOM)
    {
	if(file->Type != FILETYPE_ZIP)
	    Close(file->File);

	if(file->Type == FILETYPE_TMP)
	    DeleteFile(file->Name);

	Free(file);
    }
}
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/

void *osd_fopen(const char *gamename,const char *filename,int filetype,int write)
{
    struct ArosFile *file;
    char    *zip_name;

    file = NULL;

    if(!write)
    {
	if(filetype == OSD_FILETYPE_ROM)
	    zip_name = ROMZipName;
	else if(filetype == OSD_FILETYPE_SAMPLE)
	    zip_name = SampleZipName;
	else
	    zip_name = NULL;

	if(zip_name && zip_name[0])
	{
	    file = calloc(sizeof(struct ArosFile), 1);

	    if(file)
	    {
        	if(!load_zipped_file(zip_name, filename, &file->Data, &file->Length))
        	{
        	    file->Type  = FILETYPE_CUSTOM;
        	    file->CRC = crc32(0, file->Data, file->Length);
        	}
        	else
        	{
        	    free(file);
        	    file = NULL;
        	}
	    }
	}
    }

    if(!file)
    {
	file = OpenFileType(gamename, filename, write ? MODE_NEWFILE : MODE_OLDFILE, filetype);

	if(file && (file->Type == FILETYPE_ZIP))
	{
	    if(filetype == OSD_FILETYPE_ROM)
                zip_name = ROMZipName;
	    else if(filetype == OSD_FILETYPE_SAMPLE)
                zip_name = SampleZipName;
	    else
                zip_name = NULL;

	    /* Cache the zip filename. */

	    if(zip_name)
                strcpy(zip_name, file->Name);

	    if(load_zipped_file(zip_name, filename, &file->Data, &file->Length))
	    {
        	file->Data = NULL;
        	osd_fclose(file);

        	return(NULL);
	    }

	    file->CRC = crc32(0, file->Data, file->Length);
	}
	
	
    }

    return((void *) file);
}

/************************************************************************************/

void osd_fclose(void *file)
{
    struct ArosFile *afile = (struct ArosFile *)file;

    if(((afile->Type == FILETYPE_ZIP) || (afile->Type == FILETYPE_CUSTOM)) && afile->Data)
	free(afile->Data);

    if(afile->Type == FILETYPE_CUSTOM)
	free(file);
    else
	CloseFile(afile);
}

/************************************************************************************/

int osd_fread(void *file_handle, void *buffer, int length)
{
    struct ArosFile *file;
    LONG len;

    file = (struct ArosFile *) file_handle;

    switch(file->Type)
    {
	case FILETYPE_ZIP:
	case FILETYPE_CUSTOM:
	    if(file->Data)
	    {
        	len = file->Length - file->Offset;

        	if(len > length)
        	    len = length;

        	memcpy(buffer, &file->Data[file->Offset], len);

        	file->Offset += len;

        	return(len);
	    }

	    break;

	case FILETYPE_NORMAL:
	case FILETYPE_TMP:
	    len = ReadFile(file->File, buffer, length);

	    return(len);
    }

    return(0);
}

/************************************************************************************/

int osd_fread_scatter(void *void_file_p, void *buffer_p, int length, int increment)
{
    struct ArosFile *file_p;
    UBYTE buf[4096];
    UBYTE *dst_p;
    UBYTE *src_p;
    int   remaining_len;
    int   len;

    file_p = (struct ArosFile *) void_file_p;
    dst_p  = buffer_p;

    switch(file_p->Type)
    {
	case FILETYPE_ZIP:
	case FILETYPE_CUSTOM:
	    if(file_p->Data)
	    {
        	len = file_p->Length - file_p->Offset;

        	if(len > length)
        	    len = length;

        	length = len;

        	src_p = &file_p->Data[file_p->Offset];

        	while(len--)
        	{
        	    *dst_p = *src_p++;

        	    dst_p += increment;
        	}

        	file_p->Offset += length;

        	return(length);
	    }

	    break;

	case FILETYPE_NORMAL:
	case FILETYPE_TMP:
	    remaining_len = length;

	    while(remaining_len)
	    {      
        	if(remaining_len < sizeof(buf))
        	    len = remaining_len;
        	else
        	    len = sizeof(buf);

        	len = ReadFile(file_p->File, buf, len);

        	if(len == 0)
        	    break;

        	remaining_len -= len;

        	src_p = buf;

        	while(len--)
        	{
        	    *dst_p = *src_p++;

        	    dst_p += increment;
        	}
	    }

	    length = length - remaining_len;

	    return(length);

	    break;
    }

    return(0);
}

/************************************************************************************/

int osd_fread_swap(void *file_handle, void *buffer, int length)
{
    int i;
    unsigned char *buf;
    unsigned char temp;
    int res;


    res = osd_fread(file_handle,buffer,length);

    buf = buffer;
    for (i = 0;i < length;i+=2)
    {
	temp = buf[i];
	buf[i] = buf[i+1];
	buf[i+1] = temp;
    }

    return res;
}

/************************************************************************************/

int osd_fwrite(void *void_file_p, const void *buffer_p, int length)
{
    struct ArosFile *file_p;
    LONG rc;

    file_p = (struct ArosFile *) void_file_p;

    switch(file_p->Type)
    {
	case FILETYPE_ZIP:
	case FILETYPE_CUSTOM:
	    return(-1);

	case FILETYPE_NORMAL:
	case FILETYPE_TMP:
	    rc = WriteFile(file_p->File, (void *) buffer_p, length);

	    if(rc > 0)
              return(rc);
    }

    return(0);
}

/************************************************************************************/

int osd_fwrite_swap(void *file,const void *buffer,int length)
{
    int i;
    unsigned char *buf;
    unsigned char temp;
    int res;


    buf = (unsigned char *)buffer;
    for (i = 0;i < length;i+=2)
    {
	temp = buf[i];
	buf[i] = buf[i+1];
	buf[i+1] = temp;
    }

    res = osd_fwrite(file,buffer,length);

    for (i = 0;i < length;i+=2)
    {
	temp = buf[i];
	buf[i] = buf[i+1];
	buf[i+1] = temp;
    }

    return res;
}

/************************************************************************************/

int osd_fseek(void *file, int position, int mode)
{
    struct ArosFile *afile = (struct ArosFile *)file;

    LONG  rc = 0;

    if((afile->Type == FILETYPE_ZIP) || (afile->Type == FILETYPE_CUSTOM))
    {
	switch(mode)
	{
	    case SEEK_SET:
        	afile->Offset = position;
        	break;
		
	    case SEEK_CUR:
        	afile->Offset += position;
        	break;
		
	    case SEEK_END:
        	afile->Offset = afile->Length + position;
        	break;
		
	    default:
        	rc = -1;
		break;
	}
    }
    else
    {
	switch(mode)
	{
	    case SEEK_SET:
        	rc = SeekFile(afile->File, position, OFFSET_BEGINNING);
        	break;
		
	    case SEEK_CUR:
        	rc = SeekFile(afile->File, position, OFFSET_CURRENT);
        	break;
		
	    case SEEK_END:
        	rc = SeekFile(afile->File, position, OFFSET_END);
        	break;
		
	    default:
        	rc = -1;
		break;
	}

	if(rc != -1) rc = 0;
    }

    return rc;
}

/************************************************************************************/

int osd_fsize(void *file)
{
    struct ArosFile *afile = (struct ArosFile *)file;

    if((afile->Type == FILETYPE_ZIP) || (afile->Type == FILETYPE_CUSTOM))
        return(afile->Length);

    /* AROSFIXME? */
    return afile->Length;
}

/************************************************************************************/

unsigned int osd_fcrc(void *file)
{
    struct ArosFile *afile = (struct ArosFile *)file;
    
    return afile->CRC;
}

/************************************************************************************/

int osd_faccess (const char *newfilename, int filetype)
{
    FILE *f;
    char name[257];
    
    int result = 1;
    
    switch(filetype)
    {
        case OSD_FILETYPE_SCREENSHOT:
	    strcpy(name, screenshotdir);
	    AddPart(name, (char *)newfilename, 256);
	    f = fopen(name, "rb");
	    if (!f)
	    {
	        result = 0;
	    } else {
	        fclose(f);
	    }
	    break;
	    
    }
    
    return result;
}

/************************************************************************************/

int osd_fchecksum (const char *game, const char *filename, unsigned int *length, unsigned int *sum)
{
  return(0);
}

/************************************************************************************/

int osd_display_loading_rom_message (const char *name, int current, int total)
{
	if( name )
		fprintf (stdout, "loading %-12s\r", name);
	else
		fprintf (stdout, "                    \r");
	fflush (stdout);

	if (CheckSignal(SIGBREAKF_CTRL_C)) return 1;
	
	return 0;
}

/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
