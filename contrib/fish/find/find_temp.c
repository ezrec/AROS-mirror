/* This file contains empty template routines that
 * the IDCMP handler will call uppon. Fill out these
 * routines with your code or use them as a reference
 * to create your program.
 */

#include "find.h"

struct EasyStruct AboutReq =
       {
        0L,0L,(UBYTE *)"About FIND",
              (UBYTE *)"FIND is Copyright ©1993\nby CEKASOFT\nKarlheinz Klingbeil\nElzstr. 42\n7809 Gutach\nGermany\n\n  ENJOY !!!",
              (UBYTE *)"OK",
       };

struct EasyStruct ExitReq =
       {
        0L,0L,(UBYTE *)"Quit FIND",
              (UBYTE *)"Really quit ?",
              (UBYTE *)"Yes (L-AMIGA+V) | No (L-AMIGA + B)",
       };

 
int Gadget00Clicked( void )
{
	/* routine when gadget "Matches found" is clicked. */
 return(TRUE);
}

int patternClicked( void )
{
	/* routine when gadget "_Search" is clicked. */
 if(info.misc.from_rx)
 {
  if(Project0Wnd)GT_SetGadgetAttrs(Project0Gadgets[GDX_pattern],Project0Wnd,NULL,GTST_String,info.strings.search,TAG_DONE);
 }
 else
  strcpy(info.strings.search,GetString(Project0Gadgets[GDX_pattern]));
 return(TRUE);
}

int insertClicked( void )
{
	/* routine when gadget "_Insert" is clicked. */
 long n;
 if(!info.misc.from_rx)n = GetNumber(Project0Gadgets[GDX_insert]);
  else n = info.weight.ins;
 if(n >= 0)info.weight.ins =  n;else putmsg(2);
 if(Project0Wnd)GT_SetGadgetAttrs(Project0Gadgets[GDX_insert],Project0Wnd,NULL,GTIN_Number,info.weight.ins,TAG_DONE);
return(TRUE);
}

int subClicked( void )
{
	/* routine when gadget "S_ubstitute" is clicked. */
 long n;
 if(!info.misc.from_rx)n = GetNumber(Project0Gadgets[GDX_sub]);
  else n = info.weight.sub;
 if(n >= 0)info.weight.sub =  n;else putmsg(3);
 if(Project0Wnd)GT_SetGadgetAttrs(Project0Gadgets[GDX_sub],Project0Wnd,NULL,GTIN_Number,info.weight.sub,TAG_DONE);
 return(TRUE);
}

int delClicked( void )
{
	/* routine when gadget "De_lete" is clicked. */
 long n;
 if(!info.misc.from_rx)n = GetNumber(Project0Gadgets[GDX_del]);
  else n = info.weight.del;
 if(n >= 0)info.weight.del =  n;else putmsg(4);
 if(Project0Wnd)GT_SetGadgetAttrs(Project0Gadgets[GDX_del],Project0Wnd,NULL,GTIN_Number,info.weight.del,TAG_DONE);
 return(TRUE);
}

int filterClicked( void )
{
	/* routine when gadget "Dist_ance filter" is clicked. */
 long n;
 if(!info.misc.from_rx)n = GetNumber(Project0Gadgets[GDX_filter]);
  else n = info.match.filter;
 if((n >= 0) && (n <= 127))info.match.filter =  n;else putmsg(5);
 if(Project0Wnd)GT_SetGadgetAttrs(Project0Gadgets[GDX_filter],Project0Wnd,NULL,GTIN_Number,info.match.filter,TAG_DONE);
 return(TRUE);
}

int outputClicked( void )
{
 if(!info.misc.from_rx)info.misc.out = Project0Msg.Code;
 switch(info.misc.out)
 {
  case 0    : if(Project0Wnd)GT_SetGadgetAttrs(Project0Gadgets[GDX_outfilename],Project0Wnd,NULL,GA_Disabled,TRUE,TAG_DONE);
              break;
  case 1    : if(Project0Wnd)GT_SetGadgetAttrs(Project0Gadgets[GDX_outfilename],Project0Wnd,NULL,GA_Disabled,FALSE,TAG_DONE);
              break;
  case 2    : if(Project0Wnd)
              {
               GT_SetGadgetAttrs(Project0Gadgets[GDX_outfilename],Project0Wnd,NULL,GTST_String,"PRT:",TRUE,TAG_DONE);
               GT_SetGadgetAttrs(Project0Gadgets[GDX_outfilename],Project0Wnd,NULL,GA_Disabled,TRUE,TAG_DONE);
              }
              strcpy(info.strings.outfile,"PRT:");
              break;
 }
 if(Project0Wnd)GT_SetGadgetAttrs(Project0Gadgets[GDX_output],Project0Wnd,NULL,GTCY_Active,info.misc.out,TAG_DONE);
 return(TRUE);
}

int outfilenameClicked( void )
{
	/* routine when gadget "" is clicked. */
 if(info.misc.from_rx)
 {
  if(Project0Wnd)GT_SetGadgetAttrs(Project0Gadgets[GDX_outfilename],Project0Wnd,NULL,GTST_String,info.strings.outfile,TAG_DONE);
 }
 else
  strcpy(info.strings.outfile,GetString(Project0Gadgets[GDX_outfilename]));
 return(TRUE);
}

int flag_wordClicked( void )
{
	/* routine when gadget "_Word" is clicked. */
 info.flags.word = (info.flags.word) ? 0 : 1;
 if(info.misc.from_rx && Project0Wnd)GT_SetGadgetAttrs(Project0Gadgets[GDX_flag_word],Project0Wnd,NULL,GTCB_Checked,(ULONG)info.flags.word,TAG_DONE);
 return(TRUE);
}

int flag_linenumberClicked( void )
{
	/* routine when gadget "Linen_umber" is clicked. */
 info.flags.linenum = (info.flags.linenum) ? 0 : 1;
 if(info.misc.from_rx && Project0Wnd)GT_SetGadgetAttrs(Project0Gadgets[GDX_flag_linenumber],Project0Wnd,NULL,GTCB_Checked,(ULONG)info.flags.linenum,TAG_DONE);
 return(TRUE);
}

int flag_distClicked( void )
{
	/* routine when gadget "Li_ne" is clicked. */
 info.flags.dist = (info.flags.dist) ? 0 : 1;
 if(info.misc.from_rx && Project0Wnd)GT_SetGadgetAttrs(Project0Gadgets[GDX_flag_distance],Project0Wnd,NULL,GTCB_Checked,(ULONG)info.flags.dist,TAG_DONE);
 return(TRUE);
}

int flag_wnumClicked( void )
{
	/* routine when gadget "Di_stance" is clicked. */
 info.flags.wnum = (info.flags.wnum) ? 0 : 1;
 if(info.misc.from_rx && Project0Wnd)GT_SetGadgetAttrs(Project0Gadgets[GDX_flag_wnum],Project0Wnd,NULL,GTCB_Checked,(ULONG)info.flags.wnum,TAG_DONE);
 return(TRUE);
}

int startClicked( void )
{
	/* routine when gadget "Star_t" is clicked. */
 DisableAll();
 WLDMatch();
 return(TRUE);
}

int sizeClicked( void )
{
	/* routine when gadget "Si_ze" is clicked. */
 long n;
 if(!info.misc.from_rx)n = GetNumber(Project0Gadgets[GDX_size]);
  else n = info.data.size;
 if(n >= 20) 
 {
  if(n != (info.data.size + 1))
  {
   ULONG *data;
   n +=1;
   if(data = (ULONG *)AllocVec((n * n * sizeof(ULONG)),MEMF_ANY | MEMF_CLEAR))
   {
    if(info.data.data)FreeVec(info.data.data);
    info.data.data  = data;
    info.data.size  = n - 1;
   }
   else putmsg(1);
  }
 }
 else putmsg(6);
 if(Project0Wnd)GT_SetGadgetAttrs(Project0Gadgets[GDX_size],Project0Wnd,NULL,GTIN_Number,info.data.size,TAG_DONE);
 return(TRUE);
}

int stopClicked( void )
{
	/* routine when gadget "Sto_p" is clicked. */
 return(TRUE);
}

int filestringClicked( void )
{
	/* routine when gadget "File" is clicked. */
 if(info.misc.from_rx)
 {
  if(Project0Wnd)GT_SetGadgetAttrs(Project0Gadgets[GDX_filestring],Project0Wnd,NULL,GTST_String,info.strings.filename,TAG_DONE);
 }
 else
  strcpy(info.strings.filename,GetString(Project0Gadgets[GDX_filestring]));
 return(TRUE);
}

int requestfileClicked( void )
{
	/* routine when gadget "_?" is clicked. */
 ReqFile(MODE_INSERTNAME);
 info.misc.from_rx = TRUE;
 filestringClicked();
 info.misc.from_rx = FALSE;
 return(TRUE);
}

int delimClicked( void )
{
	/* routine when gadget "Deli_miter" is clicked. */
 if(info.misc.from_rx)
 {
  if(Project0Wnd)GT_SetGadgetAttrs(Project0Gadgets[GDX_delim],Project0Wnd,NULL,GTST_String,info.strings.delim,TAG_DONE);
 }
 else
  strcpy(info.strings.delim,GetString(Project0Gadgets[GDX_delim]));
 return(TRUE);
}

int fromlineClicked()
{
 long n;
 if(!info.misc.from_rx)n = GetNumber(Project0Gadgets[GDX_fromline]);
  else n = info.misc.fromline;
 if((n > 0) && (n < info.misc.toline))info.misc.fromline = n;
 if(Project0Wnd)GT_SetGadgetAttrs(Project0Gadgets[GDX_fromline],Project0Wnd,NULL,GTIN_Number,info.misc.fromline,TAG_DONE);
 return (TRUE);
}

int tolineClicked()
{
 long n;
 if(!info.misc.from_rx)n = GetNumber(Project0Gadgets[GDX_toline]);
  else n = info.misc.toline;
 if((n > 0) && (n < 999999) && (n > info.misc.fromline))info.misc.toline = n;
 if(Project0Wnd)GT_SetGadgetAttrs(Project0Gadgets[GDX_toline],Project0Wnd,NULL,GTIN_Number,info.misc.toline,TAG_DONE);
 return (TRUE);
}

int Project0loadconfig( void )
{
	/* routine when (sub)item "Load..." is selected. */
 if(ReqFile(MODE_LOADREQUEST))
  loadconfig();
 return(TRUE);
}

int Project0saveconfig( void )
{
	/* routine when (sub)item "Save..." is selected. */
 if(ReqFile(MODE_SAVEOLD))
  saveconfig();
 return(TRUE);
}

int Project0saveasconfig( void )
 {
	/* routine when (sub)item "Save as..." is selected. */
 if(ReqFile(MODE_SAVEREQUEST))
  saveconfig();
 return(TRUE);
}

int Project0savedefault( void )
{
	/* routine when (sub)item "Save as Default" is selected. */
 ReqFile(MODE_SAVECONFIG_ENV); saveconfig();
 ReqFile(MODE_SAVECONFIG_ARC); saveconfig();
 return(TRUE);
}

int Project0about( void )
{
	/* routine when (sub)item "About" is selected. */
 EasyRequestArgs(Project0Wnd,&AboutReq,NULL,NULL);
 return(TRUE);
}

int Project0quit( void )
{
	/* routine when (sub)item "Quit" is selected. */
 if( EasyRequestArgs(Project0Wnd,&ExitReq,NULL,NULL) == 1)
   return FALSE;
  else
   return TRUE;
 return (FALSE);
}

int Project0resetdef(void)
{
 strcpy(info.strings.load_dir,"env:");
 strcpy(info.strings.load_file,"find.config");
 ReqFile(MODE_LOADCONFIG);
 loadconfig();
 return (TRUE);
}

int Project0reset( void )
{
 SetDefaults();
 return(TRUE); 
}

int Project0writeicons( void )
{
	/* routine when (sub)item "Write Icons" is selected. */
 info.misc.writeicon = (info.misc.writeicon) ? FALSE : TRUE;
 return(TRUE);
}

int Project0CloseWindow( void )
{
	/* routine for "IDCMP_CLOSEWINDOW". */
 CloseProject0Window();
 return(TRUE);
}

int Project0VanillaKey( void )
{
	/* routine for "IDCMP_VANILLAKEY". */
 info.misc.from_rx = TRUE;
 switch(Project0Msg.Code)
 {
  case 'e':
  case 'E':ActivateGadget(Project0Gadgets[GD_pattern],Project0Wnd,NULL);
           break;
  case 'i':
  case 'I':ActivateGadget(Project0Gadgets[GD_insert],Project0Wnd,NULL);
           break;
  case 'u':
  case 'U':ActivateGadget(Project0Gadgets[GD_sub],Project0Wnd,NULL);
           break;
  case 'l':
  case 'L':ActivateGadget(Project0Gadgets[GD_del],Project0Wnd,NULL);
           break;
  case 'a':
  case 'A':ActivateGadget(Project0Gadgets[GD_filter],Project0Wnd,NULL);
           break;
  case 'p':
  case 'P':if(info.misc.out < 2)info.misc.out++;
           else info.misc.out = 0;
           outputClicked();
           break;
  case 'w':
  case 'W':flag_wnumClicked();
           break;
  case 'm':
  case 'M':flag_linenumberClicked();
           break;
  case 'c':
  case 'C':flag_distClicked();
           break;
  case 'r':
  case 'R':flag_wordClicked();
           break;
  case 's':
  case 'S':startClicked();
           break;
  case 'z':
  case 'Z':ActivateGadget(Project0Gadgets[GD_size],Project0Wnd,NULL);
           break;
  case 'f':
  case 'F':ActivateGadget(Project0Gadgets[GD_filestring],Project0Wnd,NULL);
           break;
  case 'd':
  case 'D':ActivateGadget(Project0Gadgets[GD_delim],Project0Wnd,NULL);
           break;
  case 'o':
  case 'O':ActivateGadget(Project0Gadgets[GD_fromline],Project0Wnd,NULL);
           break;
  case 't':
  case 'T':ActivateGadget(Project0Gadgets[GD_toline],Project0Wnd,NULL);
           break;
  case '?': requestfileClicked();
            break;
  default : break;
 }
 info.misc.from_rx = FALSE;
 return(TRUE);
}

