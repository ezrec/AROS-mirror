/* Running commands on Amiga
Copyright (C) 1995, 1996 Free Software Foundation, Inc.
This file is part of GNU Make.

GNU Make is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Make is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Make; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA.  */

#include "make.h"
#include "variable.h"
#include "amiga.h"
#include <assert.h>
#include <exec/memory.h>
#include <dos/dosextens.h>
#include <libraries/locale.h>
#include <dos/dostags.h>
#include <dos/dosasl.h>

#include <proto/locale.h>
#include <proto/exec.h>
#include <proto/dos.h>

static const char Amiga_version[] = "$VER: Make 3.81a2 () \n"
                    "Amiga Port by A. Digulla (digulla@home.lake.de)\n"
                    "Modified by Andy Broad <andy@broad.ology.org.uk>\n"
                    "incorporating additional code by Olaf Barthel";



        /* Difference between UTC and Amiga time. */

#define UTC_OFFSET 252482400

        /* this macro lets us long-align structures on the stack */

#define D_S(type,name) char a_##name[sizeof(type)+3]; \
                       type *name = (type *)((LONG)(a_##name+3) & ~3);

static LONG
UpdateTime(void)
{
        static LONG GMT_Offset = 0;
#ifndef __AROS__
        if(GMT_Offset == 0)
        {
                struct LocaleBase *LocaleBase;
                struct LocaleIFace *ILocale;

                if((LocaleBase = (struct LocaleBase *)OpenLibrary("locale.library",38)))
                {
                    if((ILocale = (struct LocaleIFace *)GetInterface((struct Library*)LocaleBase,(STRPTR)"main",0,NULL)))
                    {
                        struct Locale *Locale = ILocale->OpenLocale(NULL);

                        GMT_Offset = 60 * Locale->loc_GMTOffset + UTC_OFFSET;
                        ILocale->CloseLocale(Locale);
                        DropInterface((struct Interface *)ILocale);
                    }
                    CloseLibrary((struct Library *)LocaleBase);

                }
                else
                {
#endif
                        GMT_Offset = UTC_OFFSET;

//                }


  //      } 

        return(GMT_Offset);

}
#ifdef foo
int
stat(const char *file,struct stat *st)
{
        BPTR FileLock;
        D_S(struct FileInfoBlock,FileInfo);

                /* Try to get a lock on the file. */

        if(FileLock = Lock((STRPTR)file,ACCESS_READ))
       {
                STATIC char Volume[256],Comment[80];
                D_S(struct InfoData,InfoData);

                        /* Get information on file and filing system. */

                if(Examine(FileLock,FileInfo) && Info(FileLock,InfoData))
                {
                        unsigned short mode = 0;

                                /* Try to get the name of the volume. */

                        if(!NameFromLock(FileLock,Volume,sizeof(Volume)))
                                Volume[0] = 0;
                        else
                        {
                                WORD i;

                                        /* Chop off everything after the colon. */

                                for(i = 0 ; i < sizeof(Volume) ; i++)
                               {
                                        if(Volume[i] == ':')
                                        {
                                                Volume[i + 1] = 0;

                                                break;
                                        }
                                }
                        }

                        UnLock(FileLock);

                                /* Build the protection bits. */

                        if(!(FileInfo->fib_Protection & FIBF_EXECUTE))
                                mode |= S_IEXECUTE;

                        if(!(FileInfo->fib_Protection & FIBF_DELETE))
                                mode |= S_IDELETE;

                        if(!(FileInfo->fib_Protection & FIBF_READ))
                               mode |= S_IREAD;

                        if(!(FileInfo->fib_Protection & FIBF_WRITE))
                                mode |= S_IWRITE;

                        if(!(FileInfo->fib_Protection & FIBF_ARCHIVE))
                                mode |= S_IARCHIVE;

                        if(FileInfo->fib_Protection & FIBF_PURE)
                                mode |= S_IPURE;

                        if(FileInfo->fib_Protection & FIBF_SCRIPT)
                                mode |= S_ISCRIPT;

                                /* Keep the comment. */

                        CopyMem(FileInfo->fib_Comment,Comment,sizeof(Comment)-1);
                        Comment[sizeof(Comment)-1] = 0;

                                /* Fill in the data. */

                        st->st_mode     = mode;
                        st->st_ino      = FileInfo->fib_DiskKey;
                        st->st_dev      = InfoData->id_DiskType;
                        st->st_rdev     = Volume;
                        st->st_nlink    = FileInfo->fib_DirEntryType == ST_SOFTLINK || FileInfo->fib_DirEntryType == ST_LINKDIR || FileInfo->fib_DirEntryType == ST_LINKFILE;
                        st->st_uid      = FileInfo->fib_OwnerUID;
                        st->st_gid      = FileInfo->fib_OwnerGID;
                        st->st_size     = FileInfo->fib_Size;
                        st->st_atime    = UpdateTime() + (FileInfo->fib_Date.ds_Days * 24 * 60 + FileInfo->fib_Date.ds_Minute) * 60 + (FileInfo->fib_Date.ds_Tick / TICKS_PER_SECOND);
                        st->st_mtime    = st->st_atime;
                        st->st_ctime    = st->st_atime;
                        st->st_type     = FileInfo->fib_DirEntryType;
                        st->st_comment  = Comment;

                                /* Success. */

                        return(0);
                }

                UnLock(FileLock);
        }

                /* What else should I choose? */

        errno = _OSERR = EIO;

        return(-1);
}
#endif

int
MyExecute (argv,envp,outfile)
char ** argv;
char ** envp;
BPTR outfile;
{
    char * buffer, * ptr;
    char ** aptr;
    int len,i;
    int status;
    int quote;
    char *string;
    char name_buffer[512];
    char NameHeader[100];
    long NameHeaderLen;

    for (len = 0, aptr=argv; *aptr; aptr++)
    {
        len += strlen (*aptr) + 3;
    }

    buffer = malloc (len);

    if (!buffer)
      fatal (reading_file,"MyExecute: Cannot allocate space for calling a command");

    ptr = buffer;

    for (aptr=argv; *aptr; aptr++)
    {
        string = *aptr;

        for(i = 0, quote = 0 ; i < strlen(string) ; i++)
        {
            if(string[i] == '\"')
                quote ^= 1;

            if(string[i] == ';' && !quote)
                *ptr++ = '\n';
            else
                *ptr++ = string[i];
        }

        *ptr ++ = ' ';
        *ptr = 0;
    }

    ptr[-1] = '\n';

    sprintf(name_buffer,"%u",makelevel + 1);
    SetVar("MAKELEVEL",name_buffer,-1,GVF_LOCAL_ONLY);

    sprintf(NameHeader,"GNUmake_%ld",makelevel + 1);
    NameHeaderLen = strlen(NameHeader);

    if(envp)
    {
       for (aptr=envp; *aptr; aptr++)
       {
          string = *aptr;

          for(i = 0 ; string[i] != '=' ; i++);

          if(i > 0)
          {
             strcpy(name_buffer,NameHeader);
             memcpy(&name_buffer[NameHeaderLen],string,i);
             name_buffer[NameHeaderLen+i] = 0;

             SetVar(name_buffer,&string[i+1],-1,GVF_LOCAL_ONLY);
          }
       }
    }

    status = SystemTags (buffer,
        SYS_UserShell, TRUE,
        outfile ? SYS_Output : TAG_IGNORE,outfile,
    TAG_END);

    sprintf(name_buffer,"%u",makelevel);
    SetVar("MAKELEVEL",name_buffer,-1,GVF_LOCAL_ONLY);

    if(envp)
    {
       for (aptr=envp; *aptr; aptr++)
       {
          string = *aptr;

          for(i = 0 ; string[i] != '=' ; i++);

          if(i > 0)
          {
             strcpy(name_buffer,NameHeader);
             memcpy(&name_buffer[NameHeaderLen],string,i);
             name_buffer[NameHeaderLen+i] = 0;

             DeleteVar(name_buffer,GVF_LOCAL_ONLY);
          }
       }
    }

    free (buffer);

    if (SetSignal(0L,0L) & SIGBREAKF_CTRL_C)
        status = RETURN_FAIL;

    /* Check for BSD style error code. */
    if (status == 1)
        status = RETURN_ERROR;

    /* Warnings don't count */
    if (status <= RETURN_WARN)
        status = 0;

    return status;
}


char *
wildcard_expansion (wc, o)
char * wc, * o;
{
#   define PATH_SIZE	1024
    struct AnchorPath * apath;

    if ( (apath = AllocMem (sizeof (struct AnchorPath) + PATH_SIZE,
	    MEMF_CLEAR))
	)
    {
	apath->ap_Strlen = PATH_SIZE;

	if (MatchFirst (wc, apath) == 0)
	{
	    do
	    {
		o = variable_buffer_output (o, apath->ap_Buf,
			strlen (apath->ap_Buf));
		o = variable_buffer_output (o, " ",1);
	    } while (MatchNext (apath) == 0);
	}

	MatchEnd (apath);
	FreeMem (apath, sizeof (struct AnchorPath) + PATH_SIZE);
    }

    return o;
}


int stricmp (const char* s, const char *d)
{
        while (tolower (*(unsigned char*)s) == tolower(*(unsigned char*)d))
        {
                if(*s == 0) return 0;
                s++;
                d++;
        }
        if(tolower(*(unsigned char*)s) < tolower(*(unsigned char*)d)) return (-1);
        return(1);
}

