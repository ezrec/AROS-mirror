#define __chip

#include "words.h"
#define  ID_STRING "Words.File\n"


UWORD __chip icData[] =
{
/* Plane 0 */
    0xFFFF,0xFFFF,0xFFF8,0xFFFF,0xFFFF,0xFFF0,0xFFFF,0xFFFF,
    0xFFE0,0xFFFF,0xFFFF,0xFFC0,0xFF3C,0xFFFF,0x01C0,0xFF99,
    0xFFFF,0xF3C0,0xFF83,0xFFFF,0xE7C0,0xFFC7,0xFFFF,0xCFC0,
    0xFFC3,0xFFFF,0x9FC0,0xFF91,0xFF87,0x3FC0,0xFF38,0xFF33,
    0x01C0,0xFFFF,0xFF33,0xFFC0,0xFFFF,0xFF03,0xFFC0,0xFFFF,
    0xFF33,0xFFC0,0xFFFF,0x3D33,0xFFC0,0xFFFF,0x9933,0xFFC0,
    0xFFFF,0x83FF,0xFFC0,0xFFFF,0xC7FF,0xFFC0,0xFFFF,0xE7FF,
    0xFFC0,0xFFFF,0xE7FF,0xFFC0,0xFFFF,0xE7FF,0xFFC0,0xFFFF,
    0xFFFF,0xFFC0,0xFFFF,0xFFFF,0xFFC0,0xFFFF,0xFFFF,0xFFC0,
    0xE000,0x0000,0x0000,0xC000,0x0000,0x0000,0x8000,0x0000,
    0x0000,0x0000,0x0000,0x0000,
/* Plane 1 */
    0x0000,0x0000,0x0004,0x0000,0x0000,0x000C,0x0000,0x0000,
    0x001C,0x0000,0x0000,0x003C,0x0FFF,0xFFFF,0xFFFC,0x0FFF,
    0xFFFF,0xFFFC,0x0E3D,0xFFFE,0x1BFC,0x0F1B,0xFFFF,0xF7FC,
    0x0FBF,0xFFFF,0xEFFC,0x0FE7,0xFFFF,0xDFFC,0x0FC3,0xFFCF,
    0xFFFC,0x0F33,0xFEEE,0x7FFC,0x0E79,0xFEFE,0x03FC,0x0FFF,
    0xFEEF,0xFFFC,0x0FFF,0xFEEF,0xFFFC,0x0FFF,0xFEEF,0xFFFC,
    0x0FFE,0x3C27,0xFFFC,0x0FFF,0x1BFF,0xFFFC,0x0FFF,0x9FFF,
    0xFFFC,0x0FFF,0xDFFF,0xFFFC,0x0FFF,0xDFFF,0xFFFC,0x0FFF,
    0xCFFF,0xFFFC,0x0FFF,0xCFFF,0xFFFC,0x0FFF,0xFFFF,0xFFFC,
    0x1FFF,0xFFFF,0xFFFC,0x3FFF,0xFFFF,0xFFFC,0x7FFF,0xFFFF,
    0xFFFC,0xFFFF,0xFFFF,0xFFFC,
};

struct Image icI1 =
{
    0, 0,			/* Upper left corner */
    46, 28, 2,			/* Width, Height, Depth */
    icData,		/* Image data */
    0x0003, 0x0000,		/* PlanePick, PlaneOnOff */
    NULL			/* Next image */
};

struct DiskObject icon =
{
    WB_DISKMAGIC,		/* Magic Number */
    WB_DISKVERSION,		/* Version */
    {				/* Embedded Gadget Structure */
	NULL,			/* Next Gadget Pointer */
	0, 0, 46, 29,		/* Left,Top,Width,Height */
	GADGIMAGE | GADGHCOMP,	/* Flags */
	RELVERIFY,		/* Activation Flags */
	BOOLGADGET,		/* Gadget Type */
	(APTR)&icI1,	/* Render Image */
	NULL,			/* Select Image */
	NULL,			/* Gadget Text */
	NULL,			/* Mutual Exclude */
	NULL,			/* Special Info */
	0,			/* Gadget ID */
	NULL,			/* User Data */
    },
    WBPROJECT,			/* Icon Type */
    (STRPTR)"Lyr-O-Mat",  	/* Default Tool */
    NULL,			/* Tool Type Array */
    NO_ICON_POSITION,		/* Current X */
    NO_ICON_POSITION,		/* Current Y */
    NULL,			/* Drawer Structure */
    NULL,			/* Tool Window */
    0				/* Stack Size */
};



struct TagItem       FRTags[] =
                     {
                      ASL_Hail,NULL,
                      ASL_FuncFlags,0L,
                      ASL_LeftEdge,0L,
                      ASL_TopEdge,0L,
                      ASL_Dir,0L,
                      ASL_Pattern,(IPTR)"~(#?.info)",
                      ASL_File,0L,
                      TAG_DONE,0L
                     };


struct EasyStruct ExistsReq =
       {
        0L,0L,(UBYTE *)"Lyr-O-Mat... Error",
              (UBYTE *)"The File\n%s\nalready exists !",
              (UBYTE *)" Overwrite | CANCEL",
       };

struct EasyStruct wrongfileReq =
       {
        0L,0L,(UBYTE *)"Lyr-O-Mat... Error",
              (UBYTE *)"The file\n%s\n does not exist or\nis not a .wds file",
              (UBYTE *)"CANCEL",
       };


void saveAll(ULONG mode,UBYTE *title)
{
 struct Node *p,*c,*w;
 UBYTE fname[256];
 int write_it = FALSE;
 struct  FileRequester *FileReq;
 FILE *file;
 FRTags[0].ti_Data = (ULONG)title;
 FRTags[1].ti_Data = FILF_SAVE;
 FRTags[4].ti_Data = (ULONG)winfo.l_dir;
 FRTags[6].ti_Data = (ULONG)winfo.l_name;
 if(mode != MODE_SAVECONFIG)
 {
  if(FileReq = (struct FileRequester *)AllocAslRequest(ASL_FileRequest,FRTags))
  {
   if(RequestFile(FileReq))
   {
    strcpy(winfo.s_name,FileReq->rf_File);
    strcpy(winfo.s_dir,FileReq->rf_Dir);
    strcpy(fname,winfo.s_dir);
    AddPart(fname,winfo.s_name,256);
   }
   FreeAslRequest((APTR)FileReq);
  }
 }
 else
 {
  strcpy(fname,"s:");
  AddPart(fname,"Lyr-O-Mat_config.wds",256);
 }
 if( checkfile(fname,MODE_CHECKEXIST))
 {
  if(EasyRequest(Project0Wnd,&ExistsReq,NULL,fname) == 1) write_it = TRUE;
 }
 else write_it = TRUE;
 if(write_it)
 {
  if(file = fopen(fname,"w"))
  {
   fprintf(file,"%s",ID_STRING);
   if(edit.s_p == 1)
   {
    for(p = winfo.pattern.lh_Head;p->ln_Succ;p = p->ln_Succ)
    {
     fprintf(file,"%s\n",p->ln_Name);   
    }
    if(edit.pmode)
      fprintf(file,"_pmode\n");
    else
      fprintf(file,"_tmode\n");
   }
   if(edit.s_c == 1)
   {  
    for(c = winfo.class.lh_Head;c->ln_Succ;c = c->ln_Succ)
    {
     fprintf(file,"#%s\n",c->ln_Name);
     for(w = ((struct classnode *)c)->cl_Words.lh_Head;w->ln_Succ;w = w->ln_Succ)
     {
      fprintf(file,"?%s\n",w->ln_Name);
     }
    }
   }
   if(edit.writeicon)
   {
    BPTR lock;
    strcpy(winfo.icon_name,fname);
    strcat(winfo.icon_name,".info");
    if(lock = Lock(winfo.icon_name,SHARED_LOCK))UnLock(lock);
     else
      PutDiskObject(fname,&icon);
   }
   fclose(file);
  }
 }
}
void loadAll(ULONG mode,UBYTE *title)
{
 UBYTE fname[512];
 UBYTE id[256];
 struct  FileRequester *FileReq;
 FILE  *file;
 UBYTE *s;
 int attempt_load = FALSE;
 FRTags[0].ti_Data = (ULONG)title;
 FRTags[1].ti_Data = 0L;
 FRTags[4].ti_Data = (ULONG)winfo.l_dir;
 FRTags[6].ti_Data = (ULONG)winfo.l_name;
 if(mode != MODE_LOADCONFIG)
 {
  if(FileReq = (struct FileRequester *)AllocAslRequest(ASL_FileRequest,FRTags))
  {
   if(RequestFile(FileReq))
   {
    strcpy(winfo.l_name,FileReq->rf_File);
    strcpy(winfo.l_dir,FileReq->rf_Dir);
    strcpy(fname,winfo.l_dir);
    AddPart(fname,winfo.l_name,512);
    attempt_load = TRUE;
   }
   FreeAslRequest((APTR)FileReq);
  }
 }
 else
 {
  strcpy(fname,winfo.d_dir);
  AddPart(fname,winfo.d_name,256);
  if(file = fopen(fname,"r"))
  {
   fclose(file);
   attempt_load = TRUE;
  }
 }
 if(!attempt_load)return;
 if((mode != MODE_LOADCONFIG ) && (!checkfile(fname,MODE_CHECKFILE)))
 {
  EasyRequest(Project0Wnd,&wrongfileReq,NULL,fname);
 }
 else
 {
  list_off(LIST_PT);list_off(LIST_CL);list_off(LIST_WD);
  GT_SetGadgetAttrs(Project0Gadgets[GDX_addtemplate],Project0Wnd,NULL,
                   GA_Disabled,TRUE,TAG_DONE);
  GT_SetGadgetAttrs(Project0Gadgets[GDX_addclass],Project0Wnd,NULL,
                   GA_Disabled,TRUE,TAG_DONE);
  GT_SetGadgetAttrs(Project0Gadgets[GDX_addword],Project0Wnd,NULL,
                   GA_Disabled,TRUE,TAG_DONE);
  SetWindowTitles(Project0Wnd,title,(UBYTE *)-1L);
  edit.pmode = 1;
  GT_SetGadgetAttrs(Project0Gadgets[GDX_mode],Project0Wnd,NULL,
                    GTCY_Active,0,TAG_DONE);
  if(file = fopen(fname,"r"))
  {
   if(mode == MODE_LOADFILE)delAll();
   fgets(id,256,file);
   while(fgets(id,256,file))
   {
    s = id + strlen(id) - 1;
    while((*s == ' ') || (*s == '\n'))*s-- = '\0';
    switch(*id)
    {
     case     '_' : if(!stricmp(id,"_pmode"))
                    {
                     edit.pmode = 1;
                     GT_SetGadgetAttrs(Project0Gadgets[GDX_mode],Project0Wnd,NULL,
                     GTCY_Active,0,TAG_DONE);
                    }
                    else
                    if(!stricmp(id,"_tmode"))
                    {
                     edit.pmode = 0;
                     GT_SetGadgetAttrs(Project0Gadgets[GDX_mode],Project0Wnd,NULL,
                     GTCY_Active,1,TAG_DONE);   
                    }
                    break;
     case     '#' : if( (edit.l_c == 1) && 
                        (*(id + 1)    ) &&
                        (!(FindName(&winfo.class,id + 1)))
                      )
                      newclass(id + 1);
                      winfo.currentclass = (struct classnode *)FindName(&winfo.class,id + 1);
                    break;
     case     '?' : if((edit.l_c == 1) && 
                       (*(id + 1)    ) &&
                       (!(FindName(&winfo.currentclass->cl_Words,id + 1)))
                      )
                      newword(id + 1);
                    break;
     case     ';' : break;
     case     '+' :  
     default      : if(
                       (edit.l_p == 1) && 
                       (*id          ) &&
                       (!(FindName(&winfo.pattern,id)))
                      )
                      newpattern(id);
                    break;
    }
   }
   fclose(file);
  }
  GT_SetGadgetAttrs(Project0Gadgets[GDX_addtemplate],Project0Wnd,NULL,
                   GA_Disabled,FALSE,TAG_DONE);
  GT_SetGadgetAttrs(Project0Gadgets[GDX_addclass],Project0Wnd,NULL,
                   GA_Disabled,FALSE,TAG_DONE);
  GT_SetGadgetAttrs(Project0Gadgets[GDX_addword],Project0Wnd,NULL,
                   GA_Disabled,FALSE,TAG_DONE);
  SetWindowTitles(Project0Wnd,Project0Wdt,(UBYTE *)-1L);
  list_on(LIST_PT);list_on(LIST_CL);list_on(LIST_WD);
 }
}

void RequestFName()
{
}

int checkfile(UBYTE *fname,ULONG mode)
{
 int   rc = FALSE;
 FILE *file;
 UBYTE check[20];
 switch(mode)
 {
  case MODE_CHECKEXIST : if(file = fopen(fname,"r"))
                          {
                           rc = TRUE;
                           fclose(file);
                          }
                          break;
  case MODE_CHECKFILE   : if(file = fopen(fname,"r"))
                          {
                           fgets(check,20,file);
                           if(!strcmp(check,ID_STRING))rc = TRUE;
                           fclose(file);
                          }
                          break;
 }                   
 return rc;
}

struct classnode *LookForClass(UBYTE *cls)
{
 struct Node *c;
 for(c = winfo.class.lh_Head;c->ln_Succ;c = c->ln_Succ)
 {
  if(!strncmp(c->ln_Name,cls,strlen(cls)))return (struct classnode *)c;
 }  
 return NULL;
}
