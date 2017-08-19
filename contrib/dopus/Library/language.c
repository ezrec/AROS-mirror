/****************************************************************

   This file was created automatically by `FlexCat 2.5'
   from "../catalogs/dopus4_lib.cd".

   Do NOT edit by hand!

****************************************************************/

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

#include "dopuslib.h"
#ifndef __AROS__
#include <proto/powerpacker.h>
#endif

extern struct IntuiText defaulttext[];
extern char *devtypes[];

typedef struct {
    ULONG ckID;
    ULONG ckSize;
} ChunkHeader;

char **string_table;

const struct DefaultString default_strings[] =
{
    { STR_CONFIG_TOO_OLD,"Pre-3.32 configuration files not recognised!" },
    { STR_CONTINUE,"Continue" },
    { STR_REQUEST,"Request" },
    { STR_TRY_AGAIN,"Try Again" },
    { STR_CANCEL,"Cancel" },
    { STR_CANNOT_FIND_FILE_REQ,"Unable to find the file \"%s\"\n"\
	"in either the C: directory or the\n"\
	"Modules/ or the DOPUS:Modules/ directory.\n"\
	"Insert DOpus disk if necessary and select\n"\
	"%s, or %s to abort the operation" },
    { STR_FILEREQ_FILE,"File" },
    { STR_FILEREQ_DRAWER,"Drawer" },
    { STR_FILEREQ_ACCEPT,"Accept" },
    { STR_FILEREQ_PARENT,"Parent" },
    { STR_FILEREQ_DRIVES,"Drives" },
    { STR_FILEREQ_DIR,"    Dir" },
    { STR_FILEREQ_NAME,"File request" },
    { STR_DEV_DEV,"  <DEV>" },
    { STR_DEV_ASN,"  <ASN>" },
    { STR_DEV_VOL,"  <VOL>" },
    { STR_DEV_DFR,"  <DFR>" },
    { STR_DEV_NBD,"  <NBD>" },
    { STR_SELECT_DIR,"Select directory" },
    { STR_SELECT_FILE,"Select file" },
    { STR_STRING_COUNT, NULL }
};

void initstrings(void)
{
    defaulttext[0].IText=string_table[STR_FILEREQ_FILE];
    defaulttext[1].IText=string_table[STR_FILEREQ_DRAWER];
    defaulttext[2].IText=string_table[STR_FILEREQ_ACCEPT];
    defaulttext[3].IText=string_table[STR_CANCEL];
    defaulttext[4].IText=string_table[STR_FILEREQ_PARENT];
    defaulttext[5].IText=string_table[STR_FILEREQ_DRIVES];

    devtypes[0] = string_table[STR_DEV_DEV];
    devtypes[1] = string_table[STR_DEV_ASN];
    devtypes[2] = string_table[STR_DEV_VOL];
    devtypes[3] = string_table[STR_DEV_DFR];
    devtypes[4] = string_table[STR_DEV_NBD];
}

int __saveds DoReadStringFile(register struct StringData *stringdata __asm("a0"), register char *filename __asm("a1"))
{
    int a;
    struct DefaultString *defstr;
//    BPTR file;

//D(bug("ReadStringFile(%lx,%s)\n",stringdata,filename);Delay(50));
    if (!stringdata) return(0);
    if (!stringdata->string_table) {
        if (!(stringdata->string_table=AllocMem(stringdata->string_count*sizeof(APTR),MEMF_CLEAR)))
            return(0);
    }
//D(bug("ReadStringFile(%s)\n",filename));

    if (stringdata->string_buffer) {
        FreeMem(stringdata->string_buffer,stringdata->string_size);
        stringdata->string_buffer=NULL;
    }
    defstr=stringdata->default_table;

    for (a=0;a<stringdata->string_count;a++) {
        if (!defstr[a].string) break;
        stringdata->string_table[defstr[a].string_id]=defstr[a].string;
//D(bug("%ld\t%s\n",defstr[a].string_id,defstr[a].string));
    }
/*
    if (filename && (file=Open(filename,MODE_OLDFILE))) {
        struct Library *PPBase;
        struct Process *myproc;
        APTR wsave;
        ULONG test[3];

        Read(file,(char *)test,12);
        if (test[0]=='PX20' || test[0]=='PP11' || test[0]=='PP20') {
            Close(file);
            if ((PPBase=OpenLibrary("powerpacker.library",0))) {
                myproc=(struct Process *)FindTask(NULL);
                wsave=myproc->pr_WindowPtr;
                myproc->pr_WindowPtr=(APTR)-1;

                ppLoadData(filename,
                    DECR_NONE,
                    MEMF_CLEAR,
                    &stringdata->string_buffer,
                    &stringdata->string_size,
                    NULL);

                myproc->pr_WindowPtr=wsave;
                CloseLibrary(PPBase);
            }
        }
        else {
            if (test[0]==ID_FORM && test[2]==ID_OSTR) {
                Seek(file,0,OFFSET_END);
                if ((stringdata->string_size=Seek(file,0,OFFSET_BEGINNING))>0 &&
                    (stringdata->string_buffer=AllocMem(stringdata->string_size,0)))
                    Read(file,stringdata->string_buffer,stringdata->string_size);
            }
            Close(file);
        }
    }
    if (stringdata->string_buffer) {
        int size,*ptr;
        StringHeader *header;
        String *string;
        char *cptr;

        ptr=(int *)stringdata->string_buffer;
        header=(StringHeader *)&stringdata->string_buffer[12];

        if (ptr[0]!=ID_FORM || ptr[2]!=ID_OSTR ||
            header->header_id!=ID_STHD ||
            header->version<stringdata->min_version) {
            FreeMem(stringdata->string_buffer,stringdata->string_size);
            stringdata->string_buffer=NULL;
        }
        else {
            cptr=(char *)header+sizeof(StringHeader);

            for (a=0;a<header->stringcount;a++) {
                string=(String *)cptr;
                if (string->chunk_id==ID_STRN) {
                    if (string->string_id>=0 &&
                        string->string_id<stringdata->string_count)
                        stringdata->string_table[string->string_id]=cptr+sizeof(String);
                }
//D(bug("%ld\t%s\n",string->string_id,cptr+sizeof(String)));
                size=string->chunk_size+sizeof(ChunkHeader);
                if (size%2) ++size;
                cptr+=size;
                if (cptr>=(stringdata->string_buffer+stringdata->string_size)) break;
            }
        }
    }
*/
    if (filename) if (filename[0]) if (stringdata->LocaleBase = OpenLibrary("locale.library",38))
     {
      struct Library *LocaleBase = stringdata->LocaleBase;
/*
      char catname[FILEBUF_SIZE], lang[32], *s, *p;

      s = catname;
      p = FilePart(filename);
      while(strchr(p,'_'))
       {
        for (; *p && (*p != '_'); p++, s++) *s = *p;
        if(*p) p++;
        else return(1);
       }
      *s = 0;
      strcat(catname,".catalog");
      for (s = lang; *p && (*p != '.'); p++, s++) *s = ToLower(*p);
      *s = 0;

D(bug("catalog: %s\tlanguage: %s\n",catname,lang));
      if (stringdata->catalog = OpenCatalog(NULL,catname,OC_Language,*p?lang:NULL,TAG_END))
*/
      if (stringdata->catalog = OpenCatalogA(NULL,filename,NULL/*OC_Language,NULL,TAG_END*/))
       {
//D(bug("Loading .catalog strings\n"));
        for (a = 0; a < stringdata->string_count; a++)
         {
          if (!defstr[a].string) break;
          stringdata->string_table[defstr[a].string_id]=GetCatalogStr(stringdata->catalog,defstr[a].string_id,defstr[a].string);
//D(bug("%ld\t%s\n",defstr[a].string_id,GetCatalogStr(stringdata->catalog,defstr[a].string_id,defstr[a].string)));
         }
       }
     }
//D(KDump(stringdata,sizeof(struct StringData)));
    return(1);
}

void __saveds DoFreeStringFile(register struct StringData *stringdata __asm("a0"))
{
//D(bug("FreeStringFile(%lx)\n",stringdata);Delay(50));
//D(KDump(stringdata,sizeof(struct StringData)));
    if (stringdata) {
        if (stringdata->string_buffer) {
//D(bug("FreeStringFile:free buffer\n");Delay(50));
            FreeMem(stringdata->string_buffer,stringdata->string_size);
            stringdata->string_buffer=NULL;
        }
        if (stringdata->string_table) {
//D(bug("FreeStringFile:free table\n");Delay(50));
            FreeMem(stringdata->string_table,stringdata->string_count*sizeof(APTR));
            stringdata->string_table=NULL;
        }
// JRZ: begin
        if (stringdata->LocaleBase)
         {
          struct Library *LocaleBase = stringdata->LocaleBase;

//D(bug("FreeStringFile:closing catalog\n",stringdata->catalog);Delay(50));
          /*if (stringdata->catalog)*/ CloseCatalog(stringdata->catalog);
          stringdata->catalog = NULL;
          stringdata->LocaleBase = NULL;
//D(bug("FreeStringFile:closing library\n");Delay(50));
          CloseLibrary(LocaleBase);
         }
// JRZ: end
    }
//D(bug("FreeStringFile:leaving\n");Delay(50));
}
