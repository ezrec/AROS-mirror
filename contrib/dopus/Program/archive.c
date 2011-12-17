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
#ifdef USE_XADMASTER
#include <proto/xadmaster.h>
#endif

#define EXALL_NUM 2

int readarchive(struct DirectoryWindow *dir,int win)
{
#ifdef USE_XADMASTER
   BPTR lock;
   long len, i;
   struct xadFileInfo *xfi;
   struct DateStamp ds;
   unsigned char arcname[256], arcdir[256], buf[FILEBUF_SIZE], *c;
   int type;

   strcpy(arcdir,"");
D(bug("readarchive: %s\n",dir->directory)); //Delay(100);
   strcpy(arcname, dir->directory);
   if ((len = strlen(arcname))) if (arcname[len-1] == '/') arcname[len-1] = 0;
//D(bug("arcname: %s\n",arcname));
   while(!(lock = Lock(arcname, ACCESS_READ)))
    {
     c = FilePart(arcname);
     if (c == arcname) return 0;
     if (c > arcname) *(c-1) = 0;
//D(bug("arcname2: %s\n",arcname));
     strcpy(arcdir,dir->directory+(IPTR)c-(IPTR)arcname);
    }
   len = strlen(arcdir);
//D(bug("arcdir: %s\n",arcdir));
   UnLock(lock);
   if (! dir->xai)
    {
     dostatustext(globstring[STR_OPENING_ARCHIVE]);//HUX
     if ((dir->arcname = AllocVec(strlen(arcname)+1, MEMF_ANY)))
      {
       strcpy(dir->arcname,arcname);
       if ((dir->xai = xadAllocObjectA(XADOBJ_ARCHIVEINFO, NULL)))
        {
         struct TagItem ti[2];

         ti[0].ti_Tag = XAD_INFILENAME;
         ti[0].ti_Data = (Tag)arcname;
         ti[1].ti_Tag = TAG_END;

D(bug("Allocated dir->xai: %lx\nOpening the archive...\n",dir->xai));
         if(!(xadGetInfoA(dir->xai, ti)))
          {
D(bug("done\n"));
           if (dir->xai->xai_DiskInfo)
            {
             struct xadArchiveInfo *xai2;
             int err;

             if ((xai2 = xadAllocObjectA(XADOBJ_ARCHIVEINFO, NULL)))
              {
               struct TagItem ti2[2];

               ti2[0].ti_Tag = XAD_ENTRYNUMBER;
               ti2[0].ti_Data = dir->xai->xai_DiskInfo->xdi_EntryNumber;
               ti2[1].ti_Tag = TAG_MORE;
               ti2[1].ti_Data = (IPTR)ti;

D(bug("Allocated dir->xai2: %lx\n",xai2));
               if(!(err=xadGetDiskInfo(xai2, XAD_NOEMPTYERROR, 1, XAD_INDISKARCHIVE, (IPTR)ti2, TAG_DONE)))
                {
D(bug("xai2 initialized\n"));
                 if (xai2->xai_FileInfo)
                  {
                   xadFreeInfo(dir->xai);
                   xadFreeObjectA(dir->xai, NULL);
                   dir->xai = xai2;
                  }
                 else xadFreeInfo(xai2);
                }
D(bug("err = %lx\n",err));
               xadFreeObjectA(xai2, NULL);
              }
            }
           dir->flags |= DWF_ARCHIVE;
          }
         else
          {
D(bug("failed\n"));
           xadFreeObjectA(dir->xai, NULL);
           dir->xai = NULL;
           FreeVec(dir->arcname);
           dir->arcname = NULL;
           return(0);
          }
        }
       else
        {
         FreeVec(dir->arcname);
         dir->arcname = NULL;
         return(0);
        }
      }
     else return(0);
    }
   else D(bug("dir->xai present: %lx\n",dir->xai));
D(bug("archive: %s\tarcdir: %s\n",arcname,arcdir));
   if ((xfi = dir->xai->xai_FileInfo))
    {
//D(bug("xfi = %lx\n",xfi));
     while (xfi)
      {
//D(bug("Object: %s\n",xfi->xfi_FileName));
//           if ((len == 0) || (strncmp(arcdir,xfi->xfi_FileName,len) != 0))
        {
         UBYTE c1,c2;
         c = xfi->xfi_FileName;
//D(bug("arcdir: %s (%ld)\n",arcdir,len));
//         for (i = 0; ToLower(arcdir[i]) == ToLower(xfi->xfi_FileName[i]); i++) c++;
         i=0;
         do
          {
           c1 = ToLower(arcdir[i]);
           c2 = ToLower(xfi->xfi_FileName[i]);
           if (c1 == c2)
            {
             c++;
             i++;
            }
          }
         while (c1==c2);

         if (i >= len)
          {
           for (i = 0; *c && (*c != '/') && (i < (FILEBUF_SIZE-1)); i++, c++) buf[i] = *c;
           buf[i] = 0;

//D(bug("dir cut: %s\n",buf));
           if(!(findfile(dir,buf,NULL)))
            {
D(bug("readarchive/add: %s\n",buf));
             if (*c == '/') type = ST_USERDIR;
             else type = ST_FILE;

             if ((xfi->xfi_Flags & XADFIF_DIRECTORY)==XADFIF_DIRECTORY) type = ST_USERDIR;

             xadConvertDates(XAD_DATEXADDATE, (Tag)&xfi->xfi_Date, XAD_GETDATEDATESTAMP, (Tag)&ds, TAG_END);

             addfile(dir, win, buf,
                     (type > 0) ? -1LL : xfi->xfi_Size,
                     type,
                     &ds, xfi->xfi_Comment, xfi->xfi_Protection, 0,
                     FALSE, NULL, NULL, xfi->xfi_OwnerUID, xfi->xfi_OwnerGID);
            }
          }
        }
       xfi = xfi -> xfi_Next;
      }
     dostatustext(globstring[STR_OKAY_TITLE]); //HUX
//     dostatustext("OK");
     return(1);
    }
#endif /* USE_XADMASTER */
   return(0);
}

void freearchive(struct DirectoryWindow *dir)
{
#ifdef USE_XADMASTER
 if (dir->xai)
  {
D(bug("Freeing dir->xai: %lx\n",dir->xai));
   xadFreeInfo(dir->xai);
   if (dir->arcname) FreeVec(dir->arcname);
   dir->arcname = NULL;
   xadFreeObject(dir->xai, TAG_END);
   dir->xai = NULL;
   dir->arcpassword[0]=0;
  }
#endif
}

BOOL getsourcefromarc(struct DirectoryWindow *dir, char *buf, char *file)
 {
#ifdef USE_XADMASTER
D(bug("getsourcefromarc(%s,%s)\n",buf?buf:"<NULL>",file?file:"<NULL>"));
  if (dir && (dir->flags & DWF_ARCHIVE))
   {
    char srcdir[256], tempname[FILEBUF_SIZE];

    strcpy(srcdir,"T:");
    if (unarcfiledir(dir,srcdir,tempname,file))
     {
      AddPart(srcdir,tempname,256);
      strcpy(buf,srcdir);
      return TRUE;
     }
   }
#endif
  return FALSE;
 }

BOOL unarcfiledir(const struct DirectoryWindow *dir, const char *path, char *namebuf, const char *file)
{
#ifdef USE_XADMASTER
 if (dir->xai)
  {
   if (dir->arcname)
    {
     struct xadFileInfo *xfi;
     char arcname[256], arcdir[256], *c;
     int err=XADERR_UNKNOWN;

     if (dir->xai->xai_Flags & XADAIF_CRYPTED)
      {
D(bug("Encrypted archive!\n"));
        if (dir->arcpassword[0] == 0) whatsit(globstring[STR_ENTER_PASSWORD],32,dir->arcpassword,NULL);
      }

     strcpy(arcname,dir->arcname);
D(bug("unarcfiledir: arcname = %s\n",arcname));
      {
       c = strstr(dir->directory,FilePart(arcname));
       if (c) for (; c && (*c != '/'); c++);
       if (c) c++;
       strcpy(arcdir,c?c:"");
      }
D(bug("unarcfiledir: arcdir = %s\n",arcdir));
/*
     else
      {
       BPTR lock;

       strcpy(arcdir,"");
    //D(bug("Get dir: %s\n",dir->directory));
       strcpy(arcname, dir->directory);
       if (i = strlen(arcname)) if (arcname[i-1] == '/') arcname[i-1] = 0;
       while(!(lock = Lock(arcname, ACCESS_READ)))
        {
         c = FilePart(arcname);
         if (c > arcname) *(c-1) = 0;
         strcpy(arcdir,dir->directory+(int)c-(int)arcname);
        }
       UnLock(lock);
      }
*/
     AddPart(arcdir,file,256);
     strcpy(namebuf,"dopustmp");
     lsprintf(arcname,"%04lx",GetUniqueID());
     strcat(namebuf,arcname);
     c = strchr(file,'.');
     if (c) strcat(namebuf,c);
     strcpy(arcname,path);
     strcat(arcname,namebuf);
     for (xfi = dir->xai->xai_FileInfo; xfi; xfi = xfi->xfi_Next)
       if (LStrCmpI(xfi->xfi_FileName,arcdir) == 0)
         break;

     if (xfi) while(err != XADERR_OK)
      {
       err = xadFileUnArc(dir->xai,XAD_ENTRYNUMBER, xfi->xfi_EntryNumber,
                                   XAD_OUTFILENAME, (IPTR)arcname,
                                   dir->arcpassword[0]?XAD_PASSWORD:TAG_IGNORE, dir->arcpassword,
                                   TAG_END);
       switch (err)
        {
         case XADERR_OK:
          {
           struct DateStamp ds;

           xadConvertDates(XAD_DATEXADDATE, (Tag)&xfi->xfi_Date, XAD_GETDATEDATESTAMP, (Tag)&ds, TAG_END);
           SetFileDate(arcname,&ds);
           SetProtection(arcname,xfi->xfi_Protection);
           if (xfi->xfi_Comment) SetComment(arcname,xfi->xfi_Comment);

           strcpy(str_arcorgname,file);
D(bug("str_arcorgname set\n"));
           return TRUE;
          }
         case XADERR_PASSWORD:
           if (!(whatsit(globstring[STR_ENTER_PASSWORD],32,dir->arcpassword,NULL)))
             return FALSE;
           break;
         default:
D(bug("XADERR: %ld\n",err);)
           err = XADERR_OK;
        }
      }
    }
  }
#endif
 return FALSE;
}

void removetemparcfile(const char *name)
 {
  if (str_arcorgname[0])
   {
D(bug("removetemparcfile(%s)\n",name));
    DeleteFile(name);
    str_arcorgname[0]=0;
D(bug("str_arcorgname cleared\n"));
   }
 }

void arcfillfib(struct FileInfoBlock *fib, struct Directory *entry)
{
  if (entry == NULL) return;
D(bug("arcfillfib: %s (%s)\n",entry->name?entry->name:"<NULL>",entry->comment?entry->comment:"<NULL>"));
  fib->fib_DirEntryType = entry->/*sub*/type;
  strcpy(fib->fib_FileName,entry->name);
  fib->fib_Protection = entry->protection;
  fib->fib_Size = (int)entry->size;
  fib->fib_Date = entry->date;
  if (entry->comment) strcpy(fib->fib_Comment,entry->comment);
  else fib->fib_Comment[0] = 0;
  fib->fib_OwnerUID = entry->owner_id;
  fib->fib_OwnerGID = entry->group_id;
}

