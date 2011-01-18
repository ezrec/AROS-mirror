#include "find.h"

UWORD __chip I1Data[] =
{
/* Plane 0 */
    0x0000,0x0000,0x7FFF,0xFFFE,0x7FFF,0xFFFC,0x7FFF,0xFF8C,
    0x711F,0x83BC,0x7FFF,0xBFEC,0x7F64,0x77DC,0x7FFF,0xEFDC,
    0x7DF5,0xDFFC,0x7BFD,0xBBDC,0x733B,0xA3FC,0x7FF3,0xFFFC,
    0x7FE7,0xFFFC,0x7FFF,0xFFFC,0x7FFF,0xFFFC,0x4000,0x0000,
/* Plane 1 */
    0x0000,0x0000,0x0000,0x0000,0x3FFF,0xFFFE,0x3FFF,0xFFFE,
    0x3FFF,0xFFC6,0x388F,0xC1D6,0x3DDF,0xDDF6,0x3EB2,0x3BEE,
    0x3F7B,0x77EE,0x3EBA,0xEFFE,0x3DDC,0xDDEE,0x388D,0xC1FE,
    0x3FF9,0xFFFE,0x3FF3,0xFFFE,0x3FFF,0xFFFE,0x3FFF,0xFFFE,
/* Plane 2 */
    0x0000,0x0000,0x0000,0x0000,0x0000,0x0002,0x0000,0x0072,
    0x0EE0,0x7C42,0x0000,0x4012,0x009B,0x8822,0x0000,0x1022,
    0x020A,0x2002,0x0402,0x4422,0x0CC4,0x5C02,0x000C,0x0002,
    0x0018,0x0002,0x0000,0x0002,0x0000,0x0002,0x3FFF,0xFFFE,
/* Plane 3 */
    0x0000,0x0000,0x0000,0x0000,0x0000,0x0002,0x0000,0x0072,
    0x0EE0,0x7C42,0x0000,0x4012,0x009B,0x8822,0x0000,0x1022,
    0x020A,0x2002,0x0402,0x4422,0x0CC4,0x5C02,0x000C,0x0002,
    0x0018,0x0002,0x0000,0x0002,0x0000,0x0002,0x3FFF,0xFFFE,
/* Plane 4 */
    0x0000,0x0000,0x0000,0x0000,0x0000,0x0002,0x0000,0x0072,
    0x0EE0,0x7C42,0x0000,0x4012,0x009B,0x8822,0x0000,0x1022,
    0x020A,0x2002,0x0402,0x4422,0x0CC4,0x5C02,0x000C,0x0002,
    0x0018,0x0002,0x0000,0x0002,0x0000,0x0002,0x3FFF,0xFFFE,
/* Plane 5 */
    0x0000,0x0000,0x0000,0x0000,0x0000,0x0002,0x0000,0x0072,
    0x0EE0,0x7C42,0x0000,0x4012,0x009B,0x8822,0x0000,0x1022,
    0x020A,0x2002,0x0402,0x4422,0x0CC4,0x5C02,0x000C,0x0002,
    0x0018,0x0002,0x0000,0x0002,0x0000,0x0002,0x3FFF,0xFFFE,
/* Plane 6 */
    0x0000,0x0000,0x0000,0x0000,0x0000,0x0002,0x0000,0x0072,
    0x0EE0,0x7C42,0x0000,0x4012,0x009B,0x8822,0x0000,0x1022,
    0x020A,0x2002,0x0402,0x4422,0x0CC4,0x5C02,0x000C,0x0002,
    0x0018,0x0002,0x0000,0x0002,0x0000,0x0002,0x3FFF,0xFFFE,
/* Plane 7 */
    0x0000,0x0000,0x0000,0x0000,0x0000,0x0002,0x0000,0x0072,
    0x0EE0,0x7C42,0x0000,0x4012,0x009B,0x8822,0x0000,0x1022,
    0x020A,0x2002,0x0402,0x4422,0x0CC4,0x5C02,0x000C,0x0002,
    0x0018,0x0002,0x0000,0x0002,0x0000,0x0002,0x3FFF,0xFFFE,
};

struct Image I1 =
{
    0, 0,			/* Upper left corner */
    31, 16, 8,			/* Width, Height, Depth */
    I1Data,		/* Image data */
    0x00FF, 0x0000,		/* PlanePick, PlaneOnOff */
    NULL			/* Next image */
};

struct DiskObject icon =
{
    WB_DISKMAGIC,		/* Magic Number */
    WB_DISKVERSION,		/* Version */
    {				/* Embedded Gadget Structure */
	NULL,			/* Next Gadget Pointer */
	0, 0, 31, 17,		/* Left,Top,Width,Height */
	GADGIMAGE | GADGHCOMP,	/* Flags */
	RELVERIFY,		/* Activation Flags */
	BOOLGADGET,		/* Gadget Type */
	(APTR)&I1,	/* Render Image */
	NULL,			/* Select Image */
	NULL,			/* Gadget Text */
	NULL,			/* Mutual Exclude */
	NULL,			/* Special Info */
	0,			/* Gadget ID */
	NULL,			/* User Data */
    },
    WBPROJECT,			/* Icon Type */
    "FIND",			/* Default Tool */
    NULL,			/* Tool Type Array */
    NO_ICON_POSITION,		/* Current X */
    NO_ICON_POSITION,		/* Current Y */
    NULL,			/* Drawer Structure */
    NULL,			/* Tool Window */
    0				/* Stack Size */
};



#define FIND_HEADER     "*FIND*"
#define NO_N            *(stpchr(fname,'\n')) = '\0'

struct TagItem       FRTags[] =
                     {
                      ASL_Hail,NULL,
                      ASL_FuncFlags,0L,
                      ASL_LeftEdge,0L,
                      ASL_TopEdge,0L,
                      ASL_Dir,0L,
                      ASL_Pattern,(ULONG)"~(#?.info)",
                      ASL_File,0L,
                      TAG_DONE,0L
                     };

struct EasyStruct ExistsReq =
       {
        0L,0L,(UBYTE *)"FIND... Error",
              (UBYTE *)"The File\n%s\nalready exists !",
              (UBYTE *)" Overwrite | CANCEL",
       };


int ReqFile(ULONG mode)
{
 int rc = FALSE;
 struct FileRequester *FileReq;
 int request = FALSE;
 UBYTE *f,*d,*t = NULL;
 switch(mode)
 {
  case MODE_INSERTNAME : request = TRUE;
                         FRTags[0].ti_Data = (ULONG)"Find: Choose a File...";
                         FRTags[1].ti_Data = 0L;
                         FRTags[4].ti_Data = (ULONG)info.strings.load_dir;
                         FRTags[6].ti_Data = (ULONG)info.strings.load_file;
                         d = info.strings.load_dir;
                         f = info.strings.load_file;
                         t = info.strings.filename;
                         break;
  case MODE_LOADCONFIG : strcpy(info.strings.load_dir, "env:");
                         strcpy(info.strings.load_file, "find.config");
                         break;
  case MODE_LOADREQUEST: FRTags[0].ti_Data = (ULONG)"Find: Choose a Configuration File...";
                         FRTags[1].ti_Data = 0L;
                         FRTags[4].ti_Data = (ULONG)info.strings.load_dir;
                         FRTags[6].ti_Data = (ULONG)info.strings.load_file;
                         d = info.strings.load_dir;
                         f = info.strings.load_file;
                         t = info.strings.filename;
                         request = TRUE;
  case MODE_SAVECONFIG_ENV:
                         strcpy(info.strings.save_dir, "ENV:");
                         strcpy(info.strings.save_file, "find.config");
                         break;
  case MODE_SAVECONFIG_ARC:
                         strcpy(info.strings.save_dir, "ENVARC:");
                         strcpy(info.strings.save_file, "find.config");
                         break;
  case MODE_SAVEOLD    : break;                       
  case MODE_SAVEREQUEST: request = TRUE;
                         FRTags[0].ti_Data = (ULONG)"Find: Save configuration...";
                         FRTags[1].ti_Data = FILF_SAVE;
                         FRTags[4].ti_Data = (ULONG)info.strings.save_dir;
                         FRTags[6].ti_Data = (ULONG)info.strings.save_file;
                         d = info.strings.save_dir;
                         f = info.strings.save_file;
                         break;
 }
 if(request)
 {
  if(FileReq = (struct FileRequester *)AllocAslRequest(ASL_FileRequest,FRTags))
  {
   if(RequestFile(FileReq))
   {
    strcpy(d,FileReq->rf_Dir);
    strcpy(f,FileReq->rf_File);
    if(t)
    {
     strcpy(t,FileReq->rf_Dir);
     AddPart(t,FileReq->rf_File,STRINGSIZE);
    }
    rc = CheckExist(mode);
   }
   FreeAslRequest((APTR)FileReq);
  }
 }
 return rc;
}
 
int CheckExist(ULONG mode)
{
 int rc = TRUE;
 FILE *f;
 UBYTE fname[STRINGSIZE];
 switch(mode)
 {
  case MODE_SAVEOLD     :
  case MODE_SAVEREQUEST : strcpy(fname,info.strings.save_dir);
                          AddPart(fname,info.strings.save_file,STRINGSIZE);
                          if(f = fopen(fname,"r"))
                          {
                           fclose(f);     
                           if(EasyRequest(Project0Wnd,&ExistsReq,NULL,fname) != 1)
                            rc = FALSE;
                          }
                          break;
 }
 return rc;
}
 
void outputlist(void)
{
 struct Node *n;
 switch(info.misc.out)
 {
  case 0  : break;
  case 1  : putmsg(13);
            if(info.misc.outp = fopen(info.strings.outfile,"w"))
            {
             printheader(info.misc.outp);
             if(IsListEmpty(&info.match.list))
              fprintf(info.misc.outp,"NO MATCH FOUND\n");
             else
             {
              for(n = info.match.list.lh_Head;n->ln_Succ;n = n->ln_Succ)
              {
               fprintf(info.misc.outp,"%s\n",n->ln_Name);
              }
             }
             fclose(info.misc.outp);
             putmsg(16);
            }
            else putmsg(14);
            break;
 case 2  : putmsg(12);
            if(info.misc.outp = fopen("prt:","w"))
            {
             printheader(info.misc.outp);
             if(IsListEmpty(&info.match.list))
              fprintf(info.misc.outp,"NO MATCH FOUND\n");
             else
             {
              for(n = info.match.list.lh_Head;n->ln_Succ;n = n->ln_Succ)
              {
               fprintf(info.misc.outp,"MATCH=%s\n",n->ln_Name);
              }
             }
             fclose(info.misc.outp);
             putmsg(17);
            }
            else putmsg(15);
            break; 
  default : break;        
 }
}

void loadconfig()
{
 FILE *f;
 ULONG n;
 UBYTE fname[STRINGSIZE];
 putmsg(18);
 strcpy(fname,info.strings.load_dir);
 AddPart(fname,info.strings.load_file,STRINGSIZE);
 if(f = fopen(fname,"r"))
 {
  fgets(fname,STRINGSIZE,f);
  if(!strncmp(fname,FIND_HEADER,strlen(FIND_HEADER)))
  {
   info.misc.from_rx = TRUE;
   fread(&info.weight,sizeof(info.weight),1,f);
   fread(&info.flags,sizeof(info.flags),1,f);
   fread(&info.strings,sizeof(info.strings),1,f);
   fread(&info.match.filter,sizeof(info.match.filter),1,f);
   fread(&info.misc.out,sizeof(info.misc.out),1,f);
   fread(&info.misc.fromline,sizeof(info.misc.fromline),1,f);
   fread(&info.misc.toline,sizeof(info.misc.toline),1,f);
   fscanf(f,"%ld\n",&n);
   if(n >= 20)info.data.size = n;
   sizeClicked();
   info.misc.from_rx = FALSE;
   if(Project0Wnd)Project0Render();
  }
  else putmsg(20);
  fclose(f); 
 }
 else putmsg(21);
 putmsg(0); 
}

void saveconfig()
{
 FILE *f;
 UBYTE fname[STRINGSIZE];
 putmsg(19);
 strcpy(fname,info.strings.save_dir);
 AddPart(fname,info.strings.save_file,STRINGSIZE);
 if(f = fopen(fname,"w"))
 {
  fprintf(f,"%s\n",FIND_HEADER);
  fwrite(&info.weight,sizeof(info.weight),1,f);
  fwrite(&info.flags,sizeof(info.flags),1,f);
  fwrite(&info.strings,sizeof(info.strings),1,f);
  fwrite(&info.match.filter,sizeof(info.match.filter),1,f);
  fwrite(&info.misc.out,sizeof(info.misc.out),1,f);
  fwrite(&info.misc.fromline,sizeof(info.misc.fromline),1,f);
  fwrite(&info.misc.toline,sizeof(info.misc.toline),1,f);
  fprintf(f,"%ld\n",info.data.size);
  fclose(f);
  if(info.misc.writeicon)
   {
    PutDiskObject(fname,&icon);
   }
 }
 else putmsg(22);   
 putmsg(0);
}

void printheader(FILE *fp)
{
 time_t t;
 struct tm *date;
 time(&t);
 date = localtime(&t);
 fprintf(fp,"************************************************************\n");
 fprintf(fp,"*                     Find V1.0                            *\n");
 fprintf(fp,"*           Copyright (©) 1993 CEKASOFT                    *\n");
 fprintf(fp,"************************************************************\n");
 fprintf(fp,"DATE=%s",asctime(date));
 fprintf(fp,"FILE=%s\n",info.strings.filename);
 fprintf(fp,"FROM=%ld\n",info.misc.fromline);
 fprintf(fp,"TO=%ld\n",info.misc.toline);
 fprintf(fp,"PATTERN=%s\n",info.strings.search);
 fprintf(fp,"FILTER=%ld\n",info.match.filter);
 fprintf(fp,"INSERT=%ld\n",info.weight.ins);
 fprintf(fp,"SUBSTITUTE=%ld\n",info.weight.sub);
 fprintf(fp,"DELETE=%ld\n",info.weight.del);
 fprintf(fp,"SIZE=%ld\n",info.data.size);
 fprintf(fp,"************************************************************\n");
 fprintf(fp,"*                   Matches found                          *\n");
 fprintf(fp,"************************************************************\n"); 
}
