#define chip
#include "shuffle.h"

#define SHUFFLE_HEADER "*Shuffle*"

/* sc:src/shuffle/sh_icon.c --- Data for Shell icon
 *
 */

UWORD chip ShellI1Data[] =
{
/* Plane 0 */
    0x0000,0x0000,0x0000,0x0400,0x0800,0x0000,0x0041,0x0400,
    0x0800,0x0000,0x0041,0x0400,0x7FFF,0xFFFF,0xFFFF,0xFC00,
    0x2000,0x0000,0x0000,0x0400,0x2000,0x0000,0x0000,0x0400,
    0x2330,0x0000,0x0000,0x0400,0x270C,0x0000,0x0000,0x0400,
    0x2303,0x0000,0x0000,0x0400,0x230C,0x0000,0x0000,0x0400,
    0x2330,0x0000,0x0000,0x0400,0x2000,0x0000,0x0000,0x0400,
    0x2000,0x0000,0x0000,0x0400,0x2000,0x0000,0x0000,0x0400,
    0x2000,0x0000,0x0000,0x0400,0x2000,0x0000,0x0000,0x0400,
    0x2000,0x0000,0x0000,0x0400,0x2000,0x0000,0x0000,0x0400,
    0x2000,0x0000,0x0000,0x0400,0x2000,0x0000,0x0000,0x0400,
    0x2000,0x0000,0x0000,0x0400,0x7FFF,0xFFFF,0xFFFF,0xFC00,
/* Plane 1 */
    0xFFFF,0xFFFF,0xFFFF,0xF800,0x8400,0x0000,0x0020,0x8000,
    0x8400,0x0000,0x0020,0x8000,0x8000,0x0000,0x0000,0x0000,
    0x8000,0x0000,0x0000,0x8000,0x8000,0x0000,0x0000,0x8000,
    0x8000,0x0000,0x0000,0x8000,0x8000,0x0000,0x0000,0x8000,
    0x8000,0x0000,0x0000,0x8000,0x8000,0x0000,0x0000,0x8000,
    0x8000,0x0000,0x0000,0x8000,0x8000,0x0000,0x0000,0x8000,
    0x8000,0x0000,0x0000,0x8000,0x8000,0x0000,0x0000,0x8000,
    0x8000,0x0000,0x0000,0x8000,0x8000,0x0000,0x0000,0x8000,
    0x8000,0x0000,0x0000,0x8000,0x8000,0x0000,0x0000,0x8000,
    0x8000,0x0000,0x0000,0x8000,0x8000,0x0000,0x0000,0x8000,
    0x8000,0x0000,0x0000,0x8000,0x8000,0x0000,0x0000,0x0000,
};

struct Image ShellI1 =
{
    0, 0,			/* Upper left corner */
    54, 22, 2,			/* Width, Height, Depth */
    ShellI1Data,		/* Image data */
    0x0003, 0x0000,		/* PlanePick, PlaneOnOff */
    NULL			/* Next image */
};

UWORD chip ShellI2Data[] =
{
/* Plane 0 */
    0x0000,0x0000,0x0000,0x0400,0x7BEF,0x7DEF,0xBDCF,0x7C00,
    0x7BEF,0x7DEF,0xBDCF,0x7C00,0x7BEF,0x7DEF,0xBDEF,0xFC00,
    0x6000,0x0000,0x0000,0x7C00,0x0000,0x0000,0x0000,0x0400,
    0x6320,0x0000,0x0000,0x7C00,0x630C,0x0000,0x0000,0x7C00,
    0x6303,0x0000,0x0000,0x7C00,0x630C,0x0000,0x0000,0x7C00,
    0x6320,0x0000,0x0000,0x7C00,0x4000,0x0000,0x0000,0x0400,
    0x6000,0x0000,0x0000,0x7C00,0x6000,0x0000,0x0000,0x7C00,
    0x6000,0x0000,0x0000,0x7C00,0x6000,0x0000,0x0000,0x7C00,
    0x6000,0x0000,0x0000,0x7C00,0x4000,0x0000,0x0000,0x0C00,
    0x6000,0x0000,0x0000,0x7C00,0x6000,0x0000,0x0000,0x7C00,
    0x6000,0x0000,0x0000,0x7C00,0x7BEF,0x7DEF,0xBDEF,0xFC00,
/* Plane 1 */
    0xFFFF,0xFFFF,0xFFFF,0xF800,0xF7FF,0xFFFF,0xFFBE,0xF800,
    0xF7FF,0xFFFF,0xFFBE,0xF800,0x8410,0x8210,0x4210,0x0000,
    0xC410,0x8210,0x4210,0xF800,0xFFFF,0xFFFF,0xFFFF,0xF800,
    0xC410,0xBE10,0x4210,0xF800,0xC410,0xBE10,0x4210,0xF800,
    0xC410,0xBE10,0x4210,0xF800,0xC410,0xBE10,0x4210,0xF800,
    0xC410,0xBE10,0x4210,0xF800,0xFFFF,0xFFFF,0xFFFF,0xF800,
    0xC410,0x8210,0x4210,0xF800,0xC410,0x8210,0x4210,0xF800,
    0xC410,0x8210,0x4210,0xF800,0xC410,0x8210,0x4210,0xF800,
    0xC410,0x8210,0x4210,0xF800,0xFFFF,0xFFFF,0xFFFF,0xF800,
    0xC410,0x8210,0x4210,0xF800,0xC410,0x8210,0x4210,0xF800,
    0xC410,0x8210,0x4210,0xF800,0x8410,0x8210,0x4210,0x0000,
};

struct Image ShellI2 =
{
    0, 0,			/* Upper left corner */
    54, 22, 2,			/* Width, Height, Depth */
    ShellI2Data,		/* Image data */
    0x0003, 0x0000,		/* PlanePick, PlaneOnOff */
    NULL			/* Next image */
};

struct DiskObject Shell =
{
    WB_DISKMAGIC,		/* Magic Number */
    WB_DISKVERSION,		/* Version */
    {				/* Embedded Gadget Structure */
	NULL,			/* Next Gadget Pointer */
	0, 0, 54, 23,		/* Left,Top,Width,Height */
	GADGIMAGE | GADGHIMAGE,	/* Flags */
	RELVERIFY,		/* Activation Flags */
	BOOLGADGET,		/* Gadget Type */
	(APTR)&ShellI1,	/* Render Image */
	(APTR)&ShellI2,	/* Select Image */
	NULL,			/* Gadget Text */
	NULL,			/* Mutual Exclude */
	NULL,			/* Special Info */
	100,			/* Gadget ID */
	(APTR) 0x0001,		/* User Data (Revision) */
    },
    WBPROJECT,			/* Icon Type */
    (char *)"Shuffle",		/* Default Tool */
    NULL,			/* Tool Type Array */
    NO_ICON_POSITION,		/* Current X */
    NO_ICON_POSITION,		/* Current Y */
    NULL,			/* Drawer Structure */
    NULL,			/* Tool Window */
};



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
        0L,0L,(UBYTE *)"Shuffle... Error",
              (UBYTE *)"The File\n%s\nalready exists !",
              (UBYTE *)" Overwrite | CANCEL",
       };



void save_config(ULONG mode)
{
 struct FileRequester *FileReq;
 UBYTE fname[256];
 int save = FALSE;
 FILE *f;
 switch(mode)
 {
  case SAVE_NAME   : strcpy(fname,info.f_dir);
                     AddPart(fname,info.f_file,256);
                     save = TRUE;
                     break;
  case SAVE_AS     : FRTags[0].ti_Data = (ULONG)"Save Shuffle Configuration";
                     FRTags[1].ti_Data = FILF_SAVE;
                     FRTags[4].ti_Data = (ULONG)info.f_dir;
                     FRTags[6].ti_Data = (ULONG)info.f_file;
                     if(FileReq = (struct FileRequester *)AllocAslRequest(ASL_FileRequest,FRTags))
                     {
                      if(RequestFile(FileReq))
                      { 
                       strcpy(info.f_dir,FileReq->rf_Dir);
                       strcpy(info.f_file,FileReq->rf_File);
                       strcpy(fname,FileReq->rf_Dir);
                       AddPart(fname,FileReq->rf_File,256);
                       save = TRUE;
                      }
                      FreeAslRequest((APTR)FileReq);
                     }
                     break;
  case SAVE_DEF_ENVARC :
                     strcpy(fname,"ENVARC:");
                     AddPart(fname,"shuffle.config",256);
                     save = TRUE;
                     break;
 case SAVE_DEF_ENV     :
                     strcpy(fname,"ENV:");
                     AddPart(fname,"shuffle.config",256);
                     save = TRUE;
                     break;

 }
 if(save)
 {
  if(f = fopen(fname,"w"))
  {
   fprintf(f,"%s\n",SHUFFLE_HEADER);
   fprintf(f,"%ld\n",info.res);
   fprintf(f,"%ld\n",info.numbers);
   fprintf(f,"&ld\n",info.mode);
   fprintf(f,"%ld\n",info.m_shuffles);
   fclose(f);
   if(info.writeicon)
   {
    PutDiskObject(fname,&Shell);
   }
  }
 }
}

void load_config(UBYTE *name)
{
 struct FileRequester *FileReq;
 UBYTE fname[256];
 int load = FALSE;
 FILE *f;
 if(name != NULL)
 {
  strcpy(fname,name);
  load = TRUE;
 }
 else
 {
  FRTags[0].ti_Data = (ULONG)"Load Shuffle Configuration";
  FRTags[1].ti_Data = 0L;
  if(FileReq = (struct FileRequester *)AllocAslRequest(ASL_FileRequest,FRTags))
  {
   if(RequestFile(FileReq))
   {
    strcpy(fname,FileReq->rf_Dir);
    AddPart(fname,FileReq->rf_File,256);
    load = TRUE;
   }
   FreeAslRequest((APTR)FileReq);
  }
 }
 if(load)
  {
   if(f = fopen(fname,"r"))
   {
    fgets(fname,256,f);
    if(!strncmp(fname,SHUFFLE_HEADER,strlen(SHUFFLE_HEADER)))
    {
     fgets(fname,256,f);info.res        = atol(fname); 
     GT_SetGadgetAttrs(Project0Gadgets[GDX_tiles],Project0Wnd,NULL,
                       GTCY_Active,info.res,TAG_DONE);
     fgets(fname,256,f);info.numbers    = atol(fname); 
     GT_SetGadgetAttrs(Project0Gadgets[GDX_numbers],Project0Wnd,NULL,
                       GTCB_Checked,info.numbers,TAG_DONE);
     fgets(fname,256,f);info.mode       = atol(fname);   
     GT_SetGadgetAttrs(Project0Gadgets[GDX_mode],Project0Wnd,NULL,
                       GTCY_Active,info.mode,TAG_DONE);
     fgets(fname,256,f);info.m_shuffles = atol(fname); 
     info.shuffle = atol(shuffle0Labels[info.m_shuffles]); 
     GT_SetGadgetAttrs(Project0Gadgets[GDX_shuffle],Project0Wnd,NULL,
                       GTCY_Active,info.m_shuffles,TAG_DONE);
    }
    fclose(f);
   }
  }

}

