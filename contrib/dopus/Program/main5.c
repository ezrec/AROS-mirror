/*

Directory Opus 4
Original GPL release version 4.12
Copyright 1993-2000 Jonathan Potter

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

All users of Directory Opus 4 (including versions distributed
under the GPL) are entitled to upgrade to the latest version of
Directory Opus version 5 at a reduced price. Please see
http://www.gpsoft.com.au for more information.

The release of Directory Opus 4 under the GPL in NO WAY affects
the existing commercial status of Directory Opus 5.

*/

#include "dopus.h"
#include "asyncio.h"

int copyfile(src,dst,err,size,password,encryptstate)
char *src,*dst;
int *err,size;
char *password;
int encryptstate;
{
	char *buffer;
	
	/* AROS: We haveno __aligned, so we use AllocDosObject()
	struct FileInfoBlock __aligned cfinfo;
	*/
	struct FileInfoBlock *cfinfo;
	
	BPTR out;
	
	int length,suc,readsize,size_read,size_write,size_total,ret=0,prog=0,buffer_size;
	BPTR inhandle;
	BPTR outhandle;
	struct AsyncFile *infile;
/*
	struct AsyncFile *outfile;
*/
	struct DateStamp ds,*dsp;
	ULONG owner_info;
	
	cfinfo = AllocDosObject(DOS_FIB, NULL);
	if (NULL == cfinfo)
		return -1;

	buffer=NULL;
	infile=NULL;
/*
	outfile=NULL;
*/
	inhandle=0;
	outhandle=0;

	if (password) {
		int a,encrypt=1;

		for (a=0;password[a];a++) encrypt*=password[a];
		Seed(encrypt);
	}

	if (config->dynamicflags&UPDATE_PROGRESSIND_COPY) prog=1;

	suc=lockandexamine(src,cfinfo);

	if (size==0) {
		if (suc) {
			if (cfinfo->fib_Size==0) {
				if ((out=Open(dst,MODE_NEWFILE))) Close(out);
				else goto failed;
			}
			if (config->copyflags&COPY_DATE) setdate(dst,&(cfinfo->fib_Date));
			if (config->copyflags&COPY_PROT) SetProtection(dst,cfinfo->fib_Protection);
			if (config->copyflags&COPY_NOTE) SetComment(dst,cfinfo->fib_Comment);
			if (system_version2) {
				owner_info=(cfinfo->fib_OwnerUID<<16)|cfinfo->fib_OwnerGID;
				if (owner_info) SetOwner(dst,owner_info);
			}
		}
		filloutcopydatafile(dst);
		if (suc) { FreeDosObject(DOS_FIB, cfinfo); return(1); }
	}

	if (!suc) {
		*err=IoErr();
		FreeDosObject(DOS_FIB, cfinfo);
		return(0);
	}

	if (!(size=cfinfo->fib_Size)) {
		if (!(out=Open(dst,MODE_NEWFILE))) goto failed;
		Close(out);
		FreeDosObject(DOS_FIB, cfinfo);
		return(1);
	}

	if (size<=COPY_BUF_SIZE) {
		dotaskmsg(hotkeymsg_port,PROGRESS_UPDATE,100,100,NULL,1);
		prog=0;
	}
	else dotaskmsg(hotkeymsg_port,PROGRESS_UPDATE,0,100,NULL,1);

/*
	if (prog) {
		buffer_size=COPY_BUF_SIZE;
		buffer=AllocMem(COPY_BUF_SIZE,0);
		if (infile=OpenAsync(src,MODE_READ,ASYNC_READ_SIZE))
			outfile=OpenAsync(dst,MODE_WRITE,ASYNC_WRITE_SIZE);
		if (!outfile) goto failed;
	}
	else {
		if (size>65536) buffer_size=size/2;
		else buffer_size=size;
		if (buffer_size>122880) buffer_size=122880;

		while (buffer_size>0) {
			if (buffer=AllocMem(buffer_size,0)) break;
			buffer_size/=2;
		}
		if (inhandle=Open(src,MODE_OLDFILE))
			outhandle=Open(dst,MODE_NEWFILE);
		if (!outhandle) goto failed;
	}

	if (!buffer) goto failed;
*/

	if (prog) {
		buffer_size=2048;
		infile=OpenAsync(src,MODE_READ,ASYNC_READ_SIZE);
	}
	else {
		if (size>65536) buffer_size=size/2;
		else buffer_size=size;
		if (buffer_size>122880) buffer_size=122880;
	}

	while (buffer_size>0) {
		if ((buffer=AllocMem(buffer_size,0))) break;
		buffer_size/=2;
	}

	if (!buffer) goto failed;

	if (!infile && (!(inhandle=Open(src,MODE_OLDFILE)))) goto failed;
	if (!(outhandle=Open(dst,MODE_NEWFILE))) goto failed;

	DateStamp(&ds);

	size_read=size_write=0;
	size_total=size*2;

	while (size>0) {
		readsize=(size>buffer_size)?buffer_size:size;
		if (infile) length=ReadAsync(infile,buffer,readsize);
		else length=Read(inhandle,buffer,readsize);

		size-=readsize;
		size_read+=length;

		if (prog)
			dotaskmsg(hotkeymsg_port,PROGRESS_UPDATE,size_read+size_write,size_total,NULL,1);

		if (status_haveaborted) {
			ret=-1;
			goto failed;
		}

		if (password) {
			int a;
			char enbyte;

			if (encryptstate) {
				for (a=0;a<length;a++) {
					enbyte=Random(9999);
					buffer[a]+=enbyte;
				}
			}
			else {
				for (a=0;a<length;a++) {
					enbyte=Random(9999);
					buffer[a]-=enbyte;
				}
			}
		}

		if (length>0) {
/*
			if (outfile) {
				if ((WriteAsync(outfile,buffer,length))==-1) goto failed;
			}
			else
*/
			if ((Write(outhandle,buffer,length))==-1) goto failed;
		}
		size_write+=length;
	
		if (prog)
			dotaskmsg(hotkeymsg_port,PROGRESS_UPDATE,size_read+size_write,size_total,NULL,1);

		if (status_haveaborted) {
			ret=-1;
			goto failed;
		}
	}

	if (infile) CloseAsync(infile);
/*
	if (outfile) CloseAsync(outfile);
*/
	if (inhandle) Close(inhandle);
	if (outhandle) Close(outhandle);

	FreeMem(buffer,buffer_size);

	if (config->copyflags&COPY_DATE) {
		setdate(dst,&(cfinfo->fib_Date));
		dsp=&cfinfo->fib_Date;
	}
	else dsp=&ds;
	copy_datestamp(dsp,&dos_copy_date);
	if (config->copyflags&COPY_PROT) SetProtection(dst,cfinfo->fib_Protection);
	dos_copy_protection=cfinfo->fib_Protection;
	if (config->copyflags&COPY_NOTE) {
		SetComment(dst,cfinfo->fib_Comment);
		strcpy(dos_copy_comment,cfinfo->fib_Comment);
	}
	else dos_copy_comment[0]=0;
	FreeDosObject(DOS_FIB, cfinfo);
	return(1);

failed:
	*err=IoErr();
	if (buffer) FreeMem(buffer,buffer_size);
	if (infile) CloseAsync(infile);
	if (inhandle) Close(inhandle);
	if (/* outfile || */ outhandle) {
/*
		if (outfile) CloseAsync(outfile);
		else
*/
		Close(outhandle);
		DeleteFile(dst);
	}
	FreeDosObject(DOS_FIB, cfinfo);
	return(ret);
}

struct Directory *checktot(dir)
struct DirectoryWindow *dir;
{
	struct Directory *first;

	if (!dir) return(NULL);
	first=dir->firstentry;
	while (first) {
		if (first->selected && first->type<=ENTRY_FILE) break;
		first=first->next;
	}
	return(first);
}

struct Directory *checkdirtot(dir)
struct DirectoryWindow *dir;
{
	struct Directory *first;

	if (!dir) return(NULL);
	first=dir->firstentry;
	while (first) {
		if (first->selected && ENTRYTYPE(first->type)==ENTRY_DIRECTORY) break;
		first=first->next;
	}
	return(first);
}

struct Directory *checkdevtot(dir)
struct DirectoryWindow *dir;
{
	struct Directory *first;

	if (!dir) return(NULL);
	first=dir->firstentry;
	while (first) {
		if (first->selected && first->type==ENTRY_DEVICE) break;
		first=first->next;
	}
	return(first);
}

struct Directory *checkalltot(dir)
struct DirectoryWindow *dir;
{
	struct Directory *first;

	if (!dir) return(NULL);
	first=dir->firstentry;
	while (first) {
		if (first->selected && first->type!=ENTRY_DEVICE && first->type!=ENTRY_CUSTOM) break;
		first=first->next;
	}
	return(first);
}

struct Directory *findfile(dir,name,count)
struct DirectoryWindow *dir;
char *name;
int *count;
{
	struct Directory *find;
/*
	char parsebuf[100];

	LParsePatternI(name,parsebuf);
*/

	if (dir) {
		find=dir->firstentry;
		if (count) *count=0;
		while (find) {
/*
			if (find->name && (LMatchPatternI(parsebuf,find->name))) return(find);
*/
			if (find->name && !(LStrCmpI(name,find->name))) return(find);

			find=find->next;
			if (count) ++(*count);
		}
	}
	return(NULL);
}

int delfile(name,nam,errs,unprotect,errcheck)
char *name,*nam,*errs;
int unprotect,errcheck;
{
	int suc,a,err,try=0,recplus=0;
	char buf[300],buf2[100];

loop:
	suc=DeleteFile(name);
	if (!suc) {
		if ((err=IoErr())==ERROR_OBJECT_NOT_FOUND) suc=1;
		else {
			if (err==ERROR_DIRECTORY_NOT_EMPTY) return(-2);
			else if (err==ERROR_DELETE_PROTECTED && try==0) {
				if (!(config->deleteflags&DELETE_SET)) {
					if (!unprotect) {
						doerror(ERROR_DELETE_PROTECTED);
						geterrorstring(buf2,ERROR_DELETE_PROTECTED);
						lsprintf(buf,globstring[STR_ERROR_OCCURED],globstring[STR_DELETING],nam,buf2);
						strcat(buf,globstring[STR_SELECT_UNPROTECT]);
						if (!(a=simplerequest(buf,globstring[STR_UNPROTECT],
							globstring[STR_ABORT],globstring[STR_UNPROTECT_ALL],globstring[STR_SKIP],NULL)))
							return(-1);
						if (a==3) return(0);
						if (a==2) recplus=1;
					}
				}
				try=1;
				SetProtection(name,0);
				goto loop;
			}
			else if (!errcheck) return(-2);
			doerror(err);
			if ((a=checkerror(errs,nam,err))==3) return(-1);
			if (a==1) goto loop;
		}
	}
	else {
		if (recplus) suc=2;
		else suc=1;
	}
	return(suc);
}

int getwildrename(sname,dname,name,newn)
char *sname,*dname,*name,*newn;
{
	char sfirst[40],slast[40],dfirst[40],dlast[40],foon[40];
	int a,b,c,flen,llen,d;

	b=strlen(sname); sfirst[0]=slast[0]=0;
	for (a=0;a<b;a++)
		if (sname[a]=='*') {
			strcpy(sfirst,sname); sfirst[(flen=a)]=0;
			strcpy(slast,(char *)&sname[a+1]);
			llen=b-a;
			break;
		}
	b=strlen(dname);
	for (a=0;a<b;a++)
		if (dname[a]=='*') {
			strcpy(dfirst,dname); dfirst[a]=0;
			strcpy(dlast,(char *)&dname[a+1]);
			break;
		}
	a=strlen(sfirst); b=strlen(slast);
	if ((!a || (LStrnCmpI(name,sfirst,flen))==0) &&
		(!b || ((d=strlen(name))>=llen && (LStrCmpI(&name[(d-llen)+1],slast))==0))) {
		c=strlen(name)-a-b;
		CopyMem((char *)&name[a],foon,c);
		foon[c]=0;
		strcpy(newn,dfirst); strcat(newn,foon); strcat(newn,dlast);
		if (newn[0]!=0) return(1);
	}
	return(0);
}

void filloutcopydata(dir)
struct Directory *dir;
{
	dos_copy_date.ds_Days=dir->date.ds_Days;
	dos_copy_date.ds_Minute=dir->date.ds_Minute;
	dos_copy_date.ds_Tick=dir->date.ds_Tick;
	dos_copy_protection=dir->protection;
	if (dir->comment) strcpy(dos_copy_comment,dir->comment);
	else dos_copy_comment[0]=0;
}

void filloutcopydatafile(fil)
char *fil;
{
	/* AROS: Use AllocDosObject
	struct FileInfoBlock __aligned fileinfo;
	*/
	struct FileInfoBlock *fileinfo;
	
	fileinfo = AllocDosObject(DOS_FIB, NULL);
	assert(NULL != fileinfo);

	if (lockandexamine(fil,fileinfo)) {
		dos_copy_date.ds_Days=fileinfo->fib_Date.ds_Days;
		dos_copy_date.ds_Minute=fileinfo->fib_Date.ds_Minute;
		dos_copy_date.ds_Tick=fileinfo->fib_Date.ds_Tick;
		dos_copy_protection=fileinfo->fib_Protection;
		strcpy(dos_copy_comment,fileinfo->fib_Comment);
	}
	
	FreeDosObject(DOS_FIB, fileinfo);
}

void update_buffer_stamp(win,true)
int win,true;
{
	/* AROS: Use AllocDosObject
	struct FileInfoBlock __aligned fib;
	*/
	struct FileInfoBlock *fib;
	
	char dirbuf[256];
	struct DirectoryWindow *dirwin;

	if (win==-1 || !(config->dirflags&DIRFLAGS_REREADOLD)) return;
	
	fib = AllocDosObject(DOS_FIB, NULL);
	assert(NULL != fib);

	dirwin=dopus_curwin[win];
	strcpy(dirbuf,str_pathbuffer[win]);
	FOREVER {
		if (true) {
			if (lockandexamine(dirbuf,fib))
				copy_datestamp(&fib->fib_Date,&dirwin->dirstamp);
			if (!(doparent(dirbuf)) ||
				!(dirwin=findbuffer(dirbuf,win,0,1))) break;
		}
		else {
			if (!(dirwin=dirwin->next) ||
				dirwin==dopus_curwin[win]) break;
			if (LStrnCmpI(dirwin->directory,dirbuf,strlen(dirbuf))==0) {
				dirwin->dirstamp.ds_Days=0;
				dirwin->dirstamp.ds_Minute=0;
				dirwin->dirstamp.ds_Tick=0;
			}
		}
	}
	
	FreeDosObject(DOS_FIB, fib);
}

int check_key_press(func,code,qual)
struct dopusfunction *func;
USHORT code,qual;
{
	if (!func->function ||
		!func->function[0] ||
		(func->key==0xff && func->qual==0) ||
		func->key==0 ||
		func->qual!=qual) return(0);
	if (func->key==0xff || func->key==code) return(1);
	return(0);
}
