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
#ifndef __AROS__
#include <proto/multiuser.h>
#endif

#define OLD_ACTION_Dummy            20000

#define OLD_ACTION_SET_USER         (OLD_ACTION_Dummy+0)
#define OLD_ACTION_SET_GROUP        (OLD_ACTION_Dummy+1)
#define OLD_ACTION_UID_TO_USERINFO  (OLD_ACTION_Dummy+2)
#define OLD_ACTION_GID_TO_GROUPINFO (OLD_ACTION_Dummy+3)

#define ACTION_UID_TO_USERINFO      1037
#define ACTION_GID_TO_GROUPINFO     1038

static struct Requester dopus_busy_requester;    /* Busy requester */

int doparent(str)
char *str;
{
    int i,b,c;

    c=strlen(str);
    if (c==0) return(0);
    i=c-1;
    if (str[i]==':') {
        char tempbuf[256];

        expand_path(str,tempbuf);
        if ((Stricmp(str,tempbuf))==0 ||
            !(doparent(tempbuf))) return(0);
        strcpy(str,tempbuf);
        return(1);
    }
    if (str[i]=='/') --i;
    for (b=i;b>-1;b--) {
        if (str[b]=='/') { ++b; break; }
        if (str[b]==':') { ++b; break; }
    }
    if (b<0) expand_path("",str);
    else str[b]=0;
    return(1);
}

int doroot(str)
char *str;
{
    int i,b,f=0,c;

    c=strlen(str);
    if (c==0) return(0);
    if (dopus_curwin[data_active_window]->flags & DWF_ARCHIVE)
     {
      BPTR l;
      char *c = NULL;

      while(!(l = Lock(str, ACCESS_READ)))
       {
        c = FilePart(str);
        if (c > str) *(--c) = 0;
       }
      UnLock(l);
      return (c ? 1: doparent(str));
     }
    i=c;
    for (b=0;b<i;b++) if (str[b]==':') { ++b; f=1; break; }
    if (f==1 && b==i) {
        char tempbuf[256];

        expand_path(str,tempbuf);
        if ((Stricmp(str,tempbuf))==0 ||
            !(doroot(tempbuf))) return(0);
        strcpy(str,tempbuf);
        return(1);
    }
    else if (f>0) str[b]=0;
    else expand_path("",str);
    return(1);
}

int fixsortmethod(int win, int type)
 {
    int selsortmethod;

    selsortmethod=config->sortmethod[win];
    if (selsortmethod<0 || selsortmethod>DISPLAY_LAST) return DISPLAY_NAME;

    if (type==ENTRY_DEVICE) return DISPLAY_NAME;
    else if (type>=ENTRY_DIRECTORY) {
        switch (selsortmethod) {
//            case DISPLAY_SIZE:
            case DISPLAY_FILETYPE:
                return DISPLAY_NAME;
                break;
            case DISPLAY_OWNER:
            case DISPLAY_GROUP:
            case DISPLAY_NETPROT:
                if ((!AccountsBase) && (!muBase)) {
                    return DISPLAY_NAME;
                    break;
                }
            default:
                return selsortmethod;
                break;
        }
    }
    else {
        if (!AccountsBase && !muBase &&
            (selsortmethod==DISPLAY_OWNER ||
            selsortmethod==DISPLAY_GROUP ||
            selsortmethod==DISPLAY_NETPROT))
            return DISPLAY_NAME;
        else return selsortmethod;
    }
 }

int entryorder(int sortmethod, int reverse, struct Directory *entry1, struct Directory *entry2)
 {
  int a,b;

  switch (sortmethod)
   {
    case DISPLAY_NAME:
sortname:
        a = namesort(entry2->name,entry1->name);
        if ((reverse && (a>0)) || (!reverse && (a<0))) return 1;
        break;

    case DISPLAY_SIZE:
        if (entry2->size==entry1->size) goto sortname;
        if ((reverse && (entry2->size>entry1->size)) || ((!reverse) && (entry2->size<entry1->size)))
          return 1;
        break;

    case DISPLAY_DATE:
        a=CompareDate(&entry2->date,&entry1->date);
        if (a==0) goto sortname;
//D(bug("addfile: sortdate: a=%ld\n",a));
        if ((reverse && a>0) || (!reverse && a<0)) return 1;
        break;

    case DISPLAY_FILETYPE:
        if (entry2->description && entry1->description) a=Stricmp(entry2->description,entry1->description);
        else if (entry2->description) a=1;
        else if (entry1->description) a=-1;
        else a=0;
        if (a==0) goto sortname;
        if ((reverse && a>0) || (!reverse && a<0)) return 1;
        break;

    case DISPLAY_COMMENT:
        if (entry2->comment && entry1->comment) a=Stricmp(entry2->comment,entry1->comment);
        else if (entry2->comment) a=1;
        else if (entry1->comment) a=-1;
        else a=0;
        if (a==0) goto sortname;
        if ((reverse && a>0) || (!reverse && a<0)) return 1;
        break;

    case DISPLAY_EXT:
        if (entry2->extension && entry1->extension) a=Stricmp(entry2->extension,entry1->extension);
        else if (entry2->extension) a=1;
        else if (entry1->extension) a=-1;
        else a=0;
        if (a==0) goto sortname;
        if ((reverse && a>0) || (!reverse && a<0)) return 1;
        break;

    case DISPLAY_PROTECT:
        a=(entry1->protection&255)^15;
        b=(entry2->protection&255)^15;
        if (a==b) goto sortname;
        if ((reverse && b>a) || (!reverse && b<a)) return 1;
        break;

    case DISPLAY_OWNER:
        if (entry2->network && entry2->network->owner && entry1->network && entry1->network->owner)
           a=Stricmp(entry2->network->owner,entry1->network->owner);
        else if (entry2->network && entry2->network->owner) a=1;
        else if (entry1->network && entry1->network->owner) a=-1;
        else a=0;
        if (a==0) goto sortname;
        if ((reverse && a>0) || (!reverse && a<0)) return 1;
        break;

    case DISPLAY_GROUP:
        if (entry2->network && entry2->network->group && entry1->network && entry1->network->group)
           a=Stricmp(entry2->network->group,entry1->network->group);
        else if (entry2->network && entry2->network->group) a=1;
        else if (entry1->network && entry1->network->group) a=-1;
        else a=0;
        if (a==0) goto sortname;
        if ((reverse && a>0) || (!reverse && a<0)) return 1;
        break;

    case DISPLAY_NETPROT:
        a=entry1->protection&(~255);
        b=entry2->protection&(~255);
        if (a==b) goto sortname;
        if ((reverse && a>0) || (!reverse && a<0)) return 1;
        break;

    default: /* dead code? */
D(bug("main3.c: unknown sort mode!\n"));
/*
        if (!entry->next) {
            addposition=entry;
            endwhile=2;
        }
*/
        break;
   }
  return 0;
 }

struct Directory *addfile(dir,win,name,size,type,date,comment,protection,subtype,show,dispstr,addafter,ownerid,groupid)
struct DirectoryWindow *dir;
int win;
char *name;
long long size;
int type;
struct DateStamp *date;
char *comment;
int protection,subtype,show;
char *dispstr;
struct Directory *addafter;
UWORD ownerid,groupid;
{
    struct Directory *addposition=NULL,*entry,*workfirst,*newentry;
    char *description=NULL,*owner=NULL,*group=NULL;

    if (status_iconified && status_flags&STATUS_ISINBUTTONS) return((struct Directory *)1);

    if (win<0 || !dir || (type!=ENTRY_CUSTOM && protection&128 && config->hiddenbit))
        return((struct Directory *)-1);

    if (type<=ENTRY_FILE) {
        if (config->showpat[0] && !(LMatchPatternI(config->showpatparsed,name)))
            return((struct Directory *)-1);
        if (config->hidepat[0] && (LMatchPatternI(config->hidepatparsed,name)))
            return((struct Directory *)-1);
    }

    if (!(newentry=LibAllocPooled(dir_memory_pool,sizeof(struct Directory))))
        return(0);

//D(bug("addfile: name:%s\tsize = %lx%lx\ttype = %ld\n",name,a=size>>32,b=size,type));
//D(bug("addfile: name:%s\tuid = %lx\tgid = %lx\n",name,ownerid,groupid));
//D(bug("addfile: name:%s\tprotection = %lx\n",name,protection));

    /* Fill the entry with data */

    newentry->last=NULL;
    newentry->next=NULL;

    newentry->comment=NULL;
    newentry->dispstr=NULL;
    newentry->description=NULL;
    newentry->network=NULL;

    newentry->size=size;
    newentry->type=type;
    newentry->subtype=subtype;
    newentry->protection=protection;
    newentry->owner_id=ownerid;
    newentry->group_id=groupid;

    newentry->selected=FALSE;

    if (type==ENTRY_CUSTOM) {
        if (dispstr && dispstr[0]) {
            if ((newentry->dispstr=LibAllocPooled(dir_memory_pool,strlen(dispstr)+1)))
              strcpy(newentry->dispstr,dispstr);
//            else return(0);
        }
        lsprintf(newentry->name,"%ld",(long int)dir->total);
    }
    else {
        LStrnCpy(newentry->name,name,FILEBUF_SIZE-2);
        if (type!=ENTRY_DEVICE)
         {
          /* get missing data for the new entry */

          int a;

          if (type <= ENTRY_FILE)
           {
            char *c,*d=newentry->name;

            while (c=strchr(d,'.')) d=c+1;
            if ((d == newentry->name) || (d == (newentry->name+1))) c = NULL;
            else c=d-1;
            if (c)
             {
              if (c == name) c = NULL;
              else c++;
              newentry->extension = c;
             }
            else newentry->extension = NULL;
//D(bug("file %s extension: %s\n",newentry->name,newentry->extension?newentry->extension:"<none>"));
           }
          else newentry->extension = NULL;

          for (a=0;a<DISPLAY_LAST+1;a++)
           {
            switch (config->displaypos[win][a])
             {
              case DISPLAY_FILETYPE:
                  if (!description) description=getfiledescription(name,win);
                  break;
#ifndef __AROS__
              case DISPLAY_OWNER:
                  if (!owner && ownerid)
                   {
                    if (dir->last_owner==ownerid) owner=dir->owner_name;
                    else if (AccountsBase && user_info &&
                            (!(EnvoyPacket(dir->realdevice,ACTION_UID_TO_USERINFO,OLD_ACTION_UID_TO_USERINFO,ownerid,user_info))))
                     {
                      owner=user_info->ui_UserName;
                      dir->last_owner=ownerid;
                      strcpy(dir->owner_name,owner);
                     }
                    else if (muBase && mu_userinfo)
                     {
                      mu_userinfo->uid = ownerid;
                      if (!(muGetUserInfo(mu_userinfo,muKeyType_uid))) strcpy(mu_userinfo->UserID,"???");
                      owner = mu_userinfo->UserID;
                      dir->last_owner=ownerid;
                      strcpy(dir->owner_name,owner);
    //D(bug("file %s owner: %s\n",name,owner));
                     }
                   }
                  break;
              case DISPLAY_GROUP:
                  if (!group && groupid)
                   {
                    if (dir->last_group==groupid) group=dir->group_name;
                    else if (AccountsBase && group_info &&
                            (!(EnvoyPacket(dir->realdevice,ACTION_GID_TO_GROUPINFO,OLD_ACTION_GID_TO_GROUPINFO,groupid,group_info))))
                     {
                      group=group_info->gi_GroupName;
                      dir->last_group=groupid;
                      strcpy(dir->group_name,group);
                     }
                    else if (muBase && mu_groupinfo)
                     {
                      mu_groupinfo->gid = groupid;
                      if (!(muGetGroupInfo(mu_groupinfo,muKeyType_gid))) strcpy(mu_groupinfo->GroupID,"???");
                      group = mu_groupinfo->GroupID;
                      dir->last_group=groupid;
                      strcpy(dir->group_name,group);
    //D(bug("file %s group: %s\n",name,group));
                     }
                   }
                  break;
#endif
             }
           }
         }
        if (type!=ENTRY_DEVICE && description &&
            (newentry->description=LibAllocPooled(dir_memory_pool,strlen(description)+1))) {
            strcpy(newentry->description,description);
        }
        if ((AccountsBase || muBase) &&
            (newentry->network=LibAllocPooled(dir_memory_pool,sizeof(struct NetworkStuff)))) {
            int a,b;
            char *ptr;

            if (owner && (newentry->network->owner=LibAllocPooled(dir_memory_pool,strlen(owner)+1)))
                strcpy(newentry->network->owner,owner);

            if (group && (newentry->network->group=LibAllocPooled(dir_memory_pool,strlen(group)+1)))
                strcpy(newentry->network->group,group);

            ptr=newentry->network->net_protbuf;
            for (a=FIBB_GRP_READ,b=4;a>=FIBB_GRP_DELETE;a--,b++) {
                if (!(protection&(1<<a))) *ptr++='-';
                else *ptr++=str_protbits[b];
            }
            *ptr++=' ';
            for (a=FIBB_OTR_READ,b=4;a>=FIBB_OTR_DELETE;a--,b++) {
                if (!(protection&(1<<a))) *ptr++='-';
                else *ptr++=str_protbits[b];
            }
        }
        if (type==ENTRY_DEVICE) {
            switch (size) {
                case DLT_DEVICE: comment="<DEV>"; break;
                case DLT_DIRECTORY: comment="<ASN>"; break;
                case DLT_VOLUME: comment="<VOL>"; break;
                case DLT_LATE: comment="<DFR>"; break;
                case DLT_NONBINDING: comment="<NBD>"; break;
            }
        }
        getprot(protection,newentry->protbuf);
    }

    if (date) {
        newentry->date.ds_Days=date->ds_Days;
        newentry->date.ds_Minute=date->ds_Minute;
        newentry->date.ds_Tick=date->ds_Tick;

        seedate(date,newentry->datebuf,1);
//D(bug("addfile: datebuf = %s.\n",newentry->datebuf));
    }

    if (comment && comment[0]) {
        if ((newentry->comment=LibAllocPooled(dir_memory_pool,strlen(comment)+1)))
            strcpy(newentry->comment,comment);
//        else if (type==ENTRY_CUSTOM) return(0);
    }

    /* locate insert position */

    if (type==ENTRY_CUSTOM)
     {
      workfirst=dir->firstentry;
      if (addafter) addposition=addafter;
      else
       {
        switch (subtype)
         {
          case CUSTOMENTRY_BUFFERLIST:
            entry=dir->firstentry;
            while (entry)
             {
              if (Stricmp(comment,entry->comment)<=0)
               {
                if (entry->last) addposition=entry->last;
                else addposition=(struct Directory *)-1;
                break;
               }
              entry=entry->next;
             }
            break;
         }
       }
     }
    else
     {
      int /*a,b,*/reverse=0,check=0,sortmethod,endwhile;


      workfirst=addposition=NULL;

      if (addafter) {
          if (!(workfirst=addafter->next)) workfirst=addposition=addafter;
      }
      else {
          switch (config->separatemethod[win]) {
              case SEPARATE_DIRSFIRST:
                  if (dir->firstfile || dir->firstentry) {
                      if (type<=ENTRY_FILE) {
                          workfirst=dir->firstfile;
                          if (!workfirst) {
                              workfirst=dir->firstentry;
                              while (workfirst->next) workfirst=workfirst->next;
                              addposition=workfirst;
                          }
                      }
                      else workfirst=dir->firstentry;
                  }
                  check=1;
                  break;
              case SEPARATE_FILESFIRST:
                  if (dir->firstdir || dir->firstentry) {
                      if (type>=ENTRY_DEVICE) {
                          workfirst=dir->firstdir;
                          if (!workfirst) {
                              workfirst=dir->firstentry;
                              while (workfirst->next) workfirst=workfirst->next;
                              addposition=workfirst;
                          }
                      }
                      else workfirst=dir->firstentry;
                  }
                  check=1;
                  break;
              default:
                  workfirst=dir->firstentry;
                  break;
          }
      }

      if (workfirst && !addposition)
       {
          if (config->sortflags&(1<<win)) reverse=1;

          sortmethod = fixsortmethod(win,type);
          entry=workfirst;
          endwhile=0;

          while (entry) {
              if (check) {
                  switch (config->separatemethod[win]) {
                      case SEPARATE_DIRSFIRST:
                          if (entry->type<=ENTRY_FILE && type>=ENTRY_DEVICE) {
                              if (entry->last) addposition=entry->last;
                              else addposition=(struct Directory *)-1;
                              endwhile=1;
                          }
                          break;
                      case SEPARATE_FILESFIRST:
                          if (type<=ENTRY_FILE && entry->type>=ENTRY_DEVICE) {
                              if (entry->last) addposition=entry->last;
                              else addposition=(struct Directory *)-1;
                              endwhile=1;
                          }
                          break;
                  }
              }
              if (!endwhile) endwhile = !entryorder(sortmethod,reverse,newentry,entry);

              if (endwhile) {
//                  if (endwhile==1)
                   {
                      if (entry->last) addposition=entry->last;
                      else addposition=(struct Directory *)-1;
                   }
                  break;
              }
              entry=entry->next;
          }
       }
      else workfirst=NULL;
     }

    /* Link the new entry */

    switch ((int)addposition)
     {
      case -1:   /* Add at head of list */
        newentry->next=workfirst;
        if (workfirst) workfirst->last=newentry;
        switch (config->separatemethod[win])
         {
            case SEPARATE_DIRSFIRST:
                if (type<=ENTRY_FILE) dir->firstfile=newentry;
                break;
            case SEPARATE_FILESFIRST:
                if (type>=ENTRY_DEVICE) dir->firstdir=newentry;
                break;
         }
        dir->firstentry=newentry;
        break;

      case 0:
        if (workfirst) {    /* Add at end of list */
            while (workfirst->next) workfirst=workfirst->next;
            workfirst->next=newentry;
            newentry->last=workfirst;
        }
        else {  /* Start a new list */
            dir->firstentry=newentry;
            if (type<=ENTRY_FILE) dir->firstfile=newentry;
            else dir->firstdir=newentry;
        }
        break;

      default: /* Add after this entry */
        if (addposition->next) {
            newentry->next=addposition->next;
            addposition->next->last=newentry;
        }
        newentry->last=addposition;
        addposition->next=newentry;
        if (addposition->type>=ENTRY_DEVICE && type<=ENTRY_FILE)
            dir->firstfile=newentry;
        else if (addposition->type<=ENTRY_FILE && type>=ENTRY_DEVICE)
            dir->firstdir=newentry;
        break;
     }

    if (type==ENTRY_CUSTOM) if (!newentry->comment || !newentry->dispstr) return(0);

    /* update the Directory structure */

    switch (ENTRYTYPE(type)) {
        case ENTRY_FILE:
            ++dir->filetot;
            dir->bytestot+=size;
            break;
        case ENTRY_DIRECTORY:
            if (size>-1) dir->bytestot+=size;
        case ENTRY_DEVICE:
            ++dir->dirtot;
            break;
    }
    ++dir->total;

    /* remove duplicates */

    if (newentry->next) {
//D(bug("addfile: newentry(%s), nextentry(%s)\n",newentry->name,newentry->next->name));
        if (newentry->name[0] && (Stricmp(newentry->name,newentry->next->name))==0)
            removefile(newentry->next,dir,win,0);
        else if (type==ENTRY_CUSTOM && subtype==CUSTOMENTRY_BUFFERLIST &&
            (Stricmp(newentry->comment,newentry->next->comment))==0)
            removefile(newentry->next,dir,win,0);
    }

    /* refresh lister */

    if (show && type!=ENTRY_DEVICE) {
        refreshwindow(win,1);
        doposprop(win);
    }

    return(newentry);
}

int namesort(str1,str2)
char *str1,*str2;
{
    int n1,n2;

    if (SORT_ISALPHA(config->sortflags)) return Stricmp(str1,str2);

    if (((SORT_ISDEC(config->sortflags)) && _isdigit(str1[0]) && _isdigit(str2[0])) ||
        ((SORT_ISHEX(config->sortflags)) && _isxdigit(str1[0]) && _isxdigit(str2[0]))) {
/*        if ((str1[0] == '0') || (str2[0] == '0'))
         {
*/
          for(n1 = 0; str1[n1] && (str1[n1] == '0'); n1++);
          for(n2 = 0; str2[n2] && (str2[n2] == '0'); n2++);
          if (n1 != n2) return (n2-n1);
//         }
        if (SORT_ISDEC(config->sortflags))
         {
          n1=atoi(str1); n2=atoi(str2);
         }
        else
         {
          n1=strtol(str1,NULL,16); n2=strtol(str2,NULL,16);
         }
        if (n1!=n2) return((n1-n2));
        str1=getstrafternum(str1);
        str2=getstrafternum(str2);
    }
    return(Stricmp(str1,str2));
}
/*
inter_sort(str1,str2)
char *str1,*str2;
{
    if (UtilityBase) return(Stricmp(str1,str2));
    return(LStrCmpI(str1,str2));
}
*/
char *getstrafternum(str)
char *str;
{
    char *ptr=str;

    while (*ptr) {
        if (!_isdigit(*ptr)) break;
        ++ptr;
    }
    return(ptr);
}

void removefile(file,dir,win,show)
struct Directory *file;
struct DirectoryWindow *dir;
int win,show;
{
    int ty=file->type,se=file->selected;
    long long sz=file->size;

    if (win<0 || !dir || (status_iconified && status_flags&STATUS_ISINBUTTONS)) return;

    if (file==last_selected_entry) last_selected_entry=NULL;

    if (file==dir->firstfile) {
        if (file->next) {
            dir->firstfile=file->next;
            if (file->last) dir->firstfile->last=file->last;
            else dir->firstfile->last=NULL;
        }
        else dir->firstfile=NULL;
    }
    else if (file==dir->firstdir) {
        if (file->next) {
            dir->firstdir=file->next;
            if (file->last) dir->firstdir->last=file->last;
            else dir->firstdir->last=NULL;
        }
        else dir->firstdir=NULL;
    }
    if (file==dir->firstentry) {
        if (file->next) {
            dir->firstentry=file->next;
            if (dir->firstentry->next) dir->firstentry->next->last=dir->firstentry;
            dir->firstentry->last=NULL;
        }
        else dir->firstentry=NULL;
    }
    else {
        if (file->next) {
            file->last->next=file->next;
            file->next->last=file->last;
        }
        else file->last->next=NULL;
    }
    if (func_single_entry && func_single_entry==file) func_entry_deleted=1;
    free_file_memory(file);
    --dir->total;
    if (ty<0) {
        --dir->filetot; dir->bytestot-=sz;
        if (se) {
            --dir->filesel;
            dir->bytessel-=sz;
        }
    }
    else {
        --dir->dirtot;
        if (ty>0 && sz>0) dir->bytestot-=sz;
        if (se) {
            --dir->dirsel;
            if (ty>0 && sz>0) dir->bytessel-=sz;
        }
    }
    if (dir->offset>(dir->total-scrdata_dispwin_lines))
        dir->offset=dir->total-scrdata_dispwin_lines;
    if (dir->offset<0) dir->offset=0;
    if (show) refreshwindow(win,1);
}

void swapdirentries(struct DirectoryWindow *dir, struct Directory *e1, struct Directory *e2)
 {
  struct Directory *t;

  if (!e1 || !e2) return;

//D(bug("swap!\n"));
  t = e2->next;
  e2->next = e1;
  e1->next = t;
  if (t) t->last = e1;

  t = e1->last;
  e1->last = e2;
  e2->last = t;
  if (t) t->next = e2;

  if (e1 == dir->firstentry) dir->firstentry = e2;
  if (e1->type == e2->type)
   {
    if (e1 == dir->firstfile) dir->firstfile = e2;
    else if (e1 == dir->firstdir) dir->firstdir = e2;
   }
 }

void sortdir(struct DirectoryWindow *dir, int win)
 {
  struct Directory *entry1=NULL,*entry2;
  int swap,rerun=1;

  if (!dir->firstentry) return;

  busy();
  if (dir->firstentry->type==ENTRY_CUSTOM)
   {
    switch (dir->firstentry->subtype)
     {
      case CUSTOMENTRY_BUFFERLIST:
        for(; 1; entry1 = entry2)
         {
          if (!(entry2 = entry1 ? entry1->next : NULL))
           {
            if (rerun)
             {
              entry1 = dir->firstentry;
              if (!(entry2 = entry1 ? entry1->next : NULL)) break;
              rerun=0;
             }
            else break;
           }
          swap = 0;

          if (entry1->comment && entry2->comment)
            if (Stricmp(entry1->comment,entry2->comment)>0)
             {
              swapdirentries(dir,entry1,entry2);
              rerun = 1;
             }
         }
        break;
     }
   }
  else
   {
    int /*a,b,*/reverse,check/*,sortmethod*/;

    switch (config->separatemethod[win])
     {
      case SEPARATE_DIRSFIRST:
      case SEPARATE_FILESFIRST:
        check=1;
        break;
      default:
        check=0;
        break;
     }
    reverse = (config->sortflags&(1<<win));

#if defined(DEBUG) && !defined(__AROS__)
{
long long time1,time2;
kinittimer();
kgettime(&time1);
#endif
    for(; 1; entry1 = entry2)
     {
      if (!(entry2 = entry1 ? entry1->next : NULL))
       {
        if (rerun)
         {
          entry1 = dir->firstentry;
          if (!(entry2 = entry1 ? entry1->next : NULL)) break;
          rerun=0;
         }
        else break;
       }
      swap=0;
//D(bug("sortdir: entry1=%s, entry2=%s\n",entry1->name,entry2->name));
      if (check)
       {
        if (entry1->type != entry2->type)
         {
          switch (config->separatemethod[win])
           {
            case SEPARATE_DIRSFIRST:
                if ((entry1->type<=ENTRY_FILE) && (entry2->type>=ENTRY_DEVICE)) swap=1;
                else swap=-1;
                break;
            case SEPARATE_FILESFIRST:
                if ((entry2->type<=ENTRY_FILE) && (entry1->type>=ENTRY_DEVICE)) swap=1;
                else swap=-1;
                break;
           }
         }
       }
      if (!swap) swap = entryorder(fixsortmethod(win,(entry1->type!=entry2->type)?ENTRY_DIRECTORY:entry1->type),reverse,entry1,entry2);
      if (swap==1)
       {
        swapdirentries(dir,entry1,entry2);
        rerun=1;
       }
     }
#if defined(DEBUG) && !defined(__AROS__)
kgettime(&time2);
kremovetimer();
bug("sortdir() took %ld ticks\n",(ULONG)(time2-time1));
}
#endif
//D(bug("sortdir: %ld %s\n",sortmethod,reverse?"reverse":""));
   }
  unbusy();
 }

int dorun(name,state,workbench)
char *name;
int state,workbench;
{
    char argbuf[256],louise[80],buf[256];
    int rec,len;
    struct dopusfuncpar par;

    argbuf[0]=0;
    if (workbench) {
        buf[0]=FC_WORKBENCH;
        buf[1]=0;
    }
    else buf[0]=0;
    StrConcat(buf,"\"",256);
    StrConcat(buf,name,256);
    StrConcat(buf,"\"",256);

    len=256-strlen(buf);
    if (state) {
        lsprintf(louise,globstring[STR_ENTER_ARGUMENTS_FOR],BaseName(name));
        if (!(rec=whatsit(louise,len,argbuf,globstring[STR_SKIP]))) {
            myabort();
            return(-1);
        }
        if (rec==2) return(-2);
    }
    else rec=1;
    if (!rec) {
        myabort();
        return(0);
    }
    if (argbuf[0]) {
        StrConcat(buf," ",256);
        StrConcat(buf,argbuf,256);
    }
bug("R10\n");
    defaultpar(&par);
    dofunctionstring(buf,NULL,BaseName(name),&par);
    return(1);
}

void busy()
{
    if (!(status_flags&STATUS_BUSY)) {
        size_gadgets[0].GadgetType=GTYP_BOOLGADGET;
        size_gadgets[1].GadgetType=GTYP_BOOLGADGET;
        if (!Window->FirstRequest) {
            InitRequester(&dopus_busy_requester);
            dopus_busy_requester.Flags=NOISYREQ;
            Request(&dopus_busy_requester,Window);
        }
        endnotifies();
        status_flags|=STATUS_BUSY;
    }
    SetBusyPointer(Window);
}

void unbusy()
{
    if (status_flags&STATUS_BUSY) {
        struct Message *msg;

        if (Window->FirstRequest) EndRequest(&dopus_busy_requester,Window);
        flushidcmp();
        while ((msg=GetMsg(count_port))) ReplyMsg(msg);
        startnotifies();
        size_gadgets[0].GadgetType=GTYP_SIZING;
        size_gadgets[1].GadgetType=GTYP_SIZING;
        status_flags&=~STATUS_BUSY;
    }
    ClearPointer(Window);
}

void setnullpointer(wind)
struct Window *wind;
{
D(bug("null_pointer at %lx\n",null_pointer));
    SetPointer(wind,null_pointer,1,16,0,0);
}

void free_file_memory(file)
struct Directory *file;
{
    if (file) {
        if (file->comment)
            LibFreePooled(dir_memory_pool,file->comment,strlen(file->comment)+1);

        if (file->type==ENTRY_CUSTOM && file->dispstr)
            LibFreePooled(dir_memory_pool,file->dispstr,strlen(file->dispstr)+1);

        if (file->description)
            LibFreePooled(dir_memory_pool,file->description,strlen(file->description)+1);

        if (file->network) {
            if (file->network->owner)
                LibFreePooled(dir_memory_pool,file->network->owner,strlen(file->network->owner)+1);

            if (file->network->group)
                LibFreePooled(dir_memory_pool,file->network->group,strlen(file->network->group)+1);

            LibFreePooled(dir_memory_pool,file->network,sizeof(struct NetworkStuff));
        }

        LibFreePooled(dir_memory_pool,file,sizeof(struct Directory));
    }
}
