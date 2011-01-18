#include "words.h"

struct EasyStruct AboutReq =
       {
        0L,0L,(UBYTE *)"About " LYR_NAME,
              (UBYTE *)LYR_NAME " is Copyright ©1993\nby CEKASOFT\nKarlheinz Klingbeil\nElzstr. 42\n7809 Gutach\nGermany\n\n  ENJOY !!!",
              (UBYTE *)"incredible | silly | great",
       };

struct EasyStruct ExitReq =
       {
        0L,0L,(UBYTE *)"Quit " LYR_NAME,
              (UBYTE *)"Really quit ?",
              (UBYTE *)"Yes (L-AMIGA+V) | No (L-AMIGA + B)",
       };

       
int templatestringClicked( void )
{
 char gadstring[256];
 
 strcpy(gadstring, GetString(Project0Gadgets[GDX_templatestring]));

 /* routine when gadget "" is clicked. */
 if(gadstring[0])
 {
  list_off(LIST_PT);newpattern(gadstring);list_on(LIST_PT);

  GT_SetGadgetAttrs(Project0Gadgets[GDX_templatelist],Project0Wnd,NULL,
                     GTLV_Selected,NodeToNum(&winfo.pattern,(struct Node *)winfo.currentpattern),NULL,TAG_DONE);
 }
 if(edit.pattern)
 {
  edit.pattern = 0;
  GT_SetGadgetAttrs(Project0Gadgets[GDX_templatestring],Project0Wnd,NULL,
                    GA_Disabled,TRUE,TAG_DONE);
  GT_SetGadgetAttrs(Project0Gadgets[GDX_deltemplate],Project0Wnd,NULL,
                    GA_Disabled,TRUE,TAG_DONE);
 }
 else ActivateGadget(Project0Gadgets[GD_templatestring],Project0Wnd,NULL);
 return (TRUE);
}

int wordlistClicked( void )
{
	/* routine when gadget "Words" is clicked. */
 if(winfo.currentclass)
 {
  winfo.currentword = (struct wordnode *)NumToNode(&winfo.currentclass->cl_Words,Project0Msg.Code);
  GT_SetGadgetAttrs(Project0Gadgets[GDX_wordstring],Project0Wnd,NULL,
                    GA_Disabled,FALSE,TAG_DONE);
  GT_SetGadgetAttrs(Project0Gadgets[GDX_delword],Project0Wnd,NULL,
                    GA_Disabled,FALSE,TAG_DONE);
  GT_SetGadgetAttrs(Project0Gadgets[GDX_addword],Project0Wnd,NULL,
                    GA_Disabled,FALSE,TAG_DONE);
  ActivateGadget(Project0Gadgets[GD_wordstring],Project0Wnd,NULL);
  edit.word = 1;
 }
 return (TRUE);
}

int class_stringClicked( void )
{
 char gadstring[256];
 
 strcpy(gadstring, GetString(Project0Gadgets[GDX_class_string]));

	/* routine when gadget "" is clicked. */
 if(gadstring[0])
 {
  list_off(LIST_CL);newclass(gadstring);list_on(LIST_CL);
  GT_SetGadgetAttrs(Project0Gadgets[GDX_classlist],Project0Wnd,NULL,
                     GTLV_Selected,NodeToNum(&winfo.class,(struct Node *)winfo.currentclass),NULL,TAG_DONE);
  list_off(LIST_WD);
  list_on(LIST_WD);
 }
 if(edit.class)
 {
  edit.class = 0;
  GT_SetGadgetAttrs(Project0Gadgets[GDX_class_string],Project0Wnd,NULL,
                    GA_Disabled,TRUE,TAG_DONE);
  GT_SetGadgetAttrs(Project0Gadgets[GDX_delclass],Project0Wnd,NULL,
                    GA_Disabled,TRUE,TAG_DONE);
  GT_SetGadgetAttrs(Project0Gadgets[GDX_addword],Project0Wnd,NULL,
                    GA_Disabled,TRUE,TAG_DONE);
  GT_SetGadgetAttrs(Project0Gadgets[GDX_wordstring],Project0Wnd,NULL,
                    GA_Disabled,TRUE,TAG_DONE);
 }
 else ActivateGadget(Project0Gadgets[GD_class_string],Project0Wnd,NULL);
 return (TRUE);
}

int classlistClicked( void )
{
	/* routine when gadget "Classes" is clicked. */
 winfo.currentclass = (struct classnode *)NumToNode(&winfo.class,Project0Msg.Code);
 winfo.currentword  = (struct wordnode  *)winfo.currentclass->cl_Words.lh_TailPred;
 list_off(LIST_WD);
 GT_SetGadgetAttrs(Project0Gadgets[GDX_class_string],Project0Wnd,NULL,
                    GA_Disabled,FALSE,TAG_DONE);
 GT_SetGadgetAttrs(Project0Gadgets[GDX_delclass],Project0Wnd,NULL,
                    GA_Disabled,FALSE,TAG_DONE);
 GT_SetGadgetAttrs(Project0Gadgets[GDX_addword],Project0Wnd,NULL,
                    GA_Disabled,FALSE,TAG_DONE);
 ActivateGadget(Project0Gadgets[GD_class_string],Project0Wnd,NULL);
 list_on(LIST_CL);
 edit.class = 1;
 return (TRUE);
}

int wordstringClicked( void )
{
 char gadstring[256];
 
 strcpy(gadstring, GetString(Project0Gadgets[GDX_wordstring]));
	/* routine when gadget "" is clicked. */
 if(gadstring[0])
 {
  list_off(LIST_WD);newword(gadstring);list_on(LIST_WD);
  if(winfo.currentclass)
  {
   GT_SetGadgetAttrs(Project0Gadgets[GDX_wordlist],Project0Wnd,NULL,
                     GTLV_Selected,NodeToNum(&winfo.currentclass->cl_Words,(struct Node *)winfo.currentword),NULL,TAG_DONE);
  } 
 }
 if(edit.word)
 {
  edit.word = 0;
  GT_SetGadgetAttrs(Project0Gadgets[GDX_wordstring],Project0Wnd,NULL,
                    GA_Disabled,TRUE,TAG_DONE);
  GT_SetGadgetAttrs(Project0Gadgets[GDX_delword],Project0Wnd,NULL,
                    GA_Disabled,TRUE,TAG_DONE);
 }
 else  ActivateGadget(Project0Gadgets[GD_wordstring],Project0Wnd,NULL);
 return (TRUE);
}

int templatelistClicked( void )
{
 /* routine when gadget "Pattern" is clicked. */
 winfo.currentpattern = (struct patternnode *)NumToNode(&winfo.pattern,Project0Msg.Code);
 GT_SetGadgetAttrs(Project0Gadgets[GDX_templatestring],Project0Wnd,NULL,
                    GA_Disabled,FALSE,TAG_DONE);
 GT_SetGadgetAttrs(Project0Gadgets[GDX_deltemplate],Project0Wnd,NULL,
                    GA_Disabled,FALSE,TAG_DONE);
 ActivateGadget(Project0Gadgets[GD_templatestring],Project0Wnd,NULL);
 edit.pattern = 1;
 return (TRUE);
}

int numpatClicked( void )
{
 long num = GetNumber(Project0Gadgets[GDX_numpat]);
 if(num > 0) NumGen = num;
 else
 {
  GT_SetGadgetAttrs(Project0Gadgets[GDX_numpat],Project0Wnd,NULL,
                    GTIN_Number,1,TAG_DONE);
  NumGen = 1; 
 }
 return (TRUE);
}

int generateClicked( void )
{
	/* routine when gadget "Generate" is clicked. */
 int i;
 int pattern;
 struct Node *n;

 if(edit.pmode)
 {
  if(winfo.numpattern)
  {
   for(i = 0;i < NumGen;i++)
   {
    pattern = (ULONG)RangeRand(winfo.numpattern);
    n = NumToNode(&winfo.pattern,pattern);
    winfo.nach = strdup(n->ln_Name);
    ReplaceLine();
    line_out(winfo.nach);
    free(winfo.nach);
    winfo.nach=NULL;
   }
  }
 }
 else
 {
  for(i = 0;i < NumGen;i++)
  {
   for(n = winfo.pattern.lh_Head;n->ln_Succ;n = n->ln_Succ)
   {
    winfo.nach = strdup(n->ln_Name);
    ReplaceLine();
    line_out(winfo.nach);
    free(winfo.nach);
    winfo.nach=NULL;
   }
  }
 }
 return (TRUE);
}

int addtemplateClicked( void )
{
	/* routine when gadget "Add" is clicked. */
 GT_SetGadgetAttrs(Project0Gadgets[GDX_templatestring],Project0Wnd,NULL,
                   GA_Disabled,FALSE,TAG_DONE);
 GT_SetGadgetAttrs(Project0Gadgets[GDX_templatestring],Project0Wnd,NULL,
                   GTST_String,(ULONG)"",TAG_DONE);
 ActivateGadget(Project0Gadgets[GD_templatestring],Project0Wnd,NULL);
 edit.pattern = 0;
 return (TRUE);
}

int deltemplateClicked( void )
{
	/* routine when gadget "Delete" is clicked. */
 delpattern();
 GT_SetGadgetAttrs(Project0Gadgets[GDX_templatestring],Project0Wnd,NULL,
                   GA_Disabled,TRUE,TAG_DONE);
 GT_SetGadgetAttrs(Project0Gadgets[GDX_deltemplate],Project0Wnd,NULL,
                   GA_Disabled,TRUE,TAG_DONE);

 return (TRUE);
}

int addclassClicked( void )
{
	/* routine when gadget "Add" is clicked. */
 GT_SetGadgetAttrs(Project0Gadgets[GDX_class_string],Project0Wnd,NULL,
                   GA_Disabled,FALSE,TAG_DONE);
 GT_SetGadgetAttrs(Project0Gadgets[GDX_addword],Project0Wnd,NULL,
                   GA_Disabled,FALSE,TAG_DONE);
 GT_SetGadgetAttrs(Project0Gadgets[GDX_class_string],Project0Wnd,NULL,
                   GTST_String,(ULONG)"",TAG_DONE);
 ActivateGadget(Project0Gadgets[GD_class_string],Project0Wnd,NULL);
 edit.class = 0;
 return (TRUE);
}

int delclassClicked( void )
{
	/* routine when gadget "Delete" is clicked. */
 delclass();
 GT_SetGadgetAttrs(Project0Gadgets[GDX_class_string],Project0Wnd,NULL,
                   GA_Disabled,TRUE,TAG_DONE);
 GT_SetGadgetAttrs(Project0Gadgets[GDX_delclass],Project0Wnd,NULL,
                   GA_Disabled,TRUE,TAG_DONE);
 GT_SetGadgetAttrs(Project0Gadgets[GDX_wordstring],Project0Wnd,NULL,
                   GA_Disabled,TRUE,TAG_DONE);
 GT_SetGadgetAttrs(Project0Gadgets[GDX_delword],Project0Wnd,NULL,
                   GA_Disabled,TRUE,TAG_DONE);
 GT_SetGadgetAttrs(Project0Gadgets[GDX_addword],Project0Wnd,NULL,
                   GA_Disabled,TRUE,TAG_DONE);
 return (TRUE);
}

int addwordClicked( void )
{
	/* routine when gadget "Add" is clicked. */
 GT_SetGadgetAttrs(Project0Gadgets[GDX_wordstring],Project0Wnd,NULL,
                   GA_Disabled,FALSE,TAG_DONE);
 GT_SetGadgetAttrs(Project0Gadgets[GDX_wordstring],Project0Wnd,NULL,
                   GTST_String,(ULONG)"",TAG_DONE);
 ActivateGadget(Project0Gadgets[GD_wordstring],Project0Wnd,NULL);
 edit.word = 0;
 return (TRUE);
}

int delwordClicked( void )
{
	/* routine when gadget "Delete" is clicked. */
 delword();
 return (TRUE);
}

int modeClicked( void )
{
 edit.pmode = (edit.pmode) ? 0 : 1; 
 return (TRUE);
}
int Project0Loadpattern( void )
{
 edit.l_p = 1;
 edit.l_c = 0;
 loadAll(MODE_LOADFILE,"Loading Patterns...");
 return TRUE;
}
int Project0Loadclasses( void )
{
 edit.l_p = 0;
 edit.l_c = 1;
 loadAll(MODE_LOADFILE,"Loading Classes...");
 return TRUE;
}
int Project0appendpattern( void )
{
 edit.l_p = 1;
 edit.l_c = 0;
 loadAll(MODE_APPENDFILE,"Appending Patterns...");
 return TRUE;
}
int Project0appendclasses( void )
{
 edit.l_p = 0;
 edit.l_c = 1;
 loadAll(MODE_APPENDFILE,"Appending Classes...");
 return TRUE;
}
int Project0savepattern( void )
{
 edit.s_p = 1;
 edit.s_c = 0;
 saveAll(MODE_SAVEFILE,"Lyr-O-Mat... Save Pattern only");
 return TRUE;
}
int Project0saveclasses( void )
{
 edit.s_p = 0;
 edit.s_c = 1;
 saveAll(MODE_SAVEFILE,"Lyr-O-Mat... Save classes only");
 return TRUE;
}
int Project0about( void )
{
 EasyRequestArgs(Project0Wnd,&AboutReq,NULL,NULL);
 return TRUE;
}

int Project0Loadall( void )
{
	/* routine when (sub)item "Load words..." is selected. */
 edit.l_p = 1;
 edit.l_c = 1;
 loadAll(MODE_LOADFILE,"Loading Patterns & Classes");
 return (TRUE);
}

int Project0appendall( void )
{
	/* routine when (sub)item "Load words..." is selected. */
 edit.l_p = 1;
 edit.l_c = 1;
 loadAll(MODE_APPENDFILE,"Appending Patterns & Classes...");
 return (TRUE);
}


int Project0saveall( void )
{
	/* routine when (sub)item "Save words..." is selected. */
 edit.s_p = 1;
 edit.s_c = 1;
 saveAll(MODE_SAVEFILE,"Lyr-O-Mat... Save all Data");
 return (TRUE);
}

int Project0savedefault( void )
{
	/* routine when (sub)item "Save as Default..." is selected. */
 edit.s_p = 1;
 edit.s_c = 1;
 saveAll(MODE_SAVECONFIG,"Lyr-O-Mat... Save configuration");
 return (TRUE);
}

int Project0ShowInfo( void )
{
 Project0ShowPInfo(); 
 Project0ShowCInfo();
 return TRUE;
}


int Project0Item0( void )
{
	/* routine when (sub)item "Print" is selected. */
 if(winfo.printfile)
 {
  fclose(winfo.printfile);
  winfo.printfile = NULL;
 }
 else
 {
  winfo.printfile = fopen("prt:","w");
 }
 return (TRUE);
}

int Project0ConWd( void )
{
 if(winfo.outfile)
 {
  fclose(winfo.outfile);
  winfo.outfile = NULL;
 }
 else
 {
  winfo.outfile = fopen(winfo.outfilename,"w");
 }
 return (TRUE);
}

int Project0quit( void )
{
	/* routine when (sub)item "Quit..." is selected. */
 if( EasyRequestArgs(Project0Wnd,&ExitReq,NULL,NULL) == 1)
   return FALSE;
  else
   return TRUE;
}

int Project0CloseWindow( void )
{
	/* routine for "IDCMP_CLOSEWINDOW". */
 return Project0quit();
}

void line_out(UBYTE *line)
{
 if(winfo.outfile)
 {
    fprintf(winfo.outfile,"%s\n",line);
    fflush(winfo.outfile);
 }
 if(winfo.printfile) fprintf(winfo.printfile,"%s\n",line);
}

int Project0VanillaKey( void)
{
 int rc = TRUE;
 switch(Project0Msg.Code)
 {
  case '\n'  :
  case ' '  :
  case 'g'  :
  case 'G'  : generateClicked();
              break;
  case 'q'  :
  case 'Q'  : rc = Project0quit();
  default   : break;
 }
 return rc;
}

int Project0ShowPInfo( void )
{
 UBYTE line[256];
 struct Node *p;
 line_out("***************************************");
 line_out("*      Lyr-O-Mat Pattern info         *");
 line_out("***************************************");
 if(winfo.numpattern)
 {
  sprintf(line,"* %6ld Patterns",winfo.numpattern);
  line_out(line);
  for(p = winfo.pattern.lh_Head;p->ln_Succ;p=p->ln_Succ)
  {
   sprintf(line,"** %s",p->ln_Name);
   line_out(line);   
  }
 }
 else
 line_out("* No Patterns");
 line_out("***************************************");  
 return(TRUE);
}

int Project0ShowCInfo( void )
{
 UBYTE line[256];
 struct Node *p;
 line_out("***************************************");
 line_out("*       Lyr-O-Mat Class info          *");
 line_out("***************************************");
 if(winfo.numclass)
 {
  sprintf(line,"\n* %6ld Classes",winfo.numclass);
  line_out(line);
  for(p = winfo.class.lh_Head;p->ln_Succ;p=p->ln_Succ)
  {
   struct classnode *c;
   c = (struct classnode *)p;   
   if(c->cl_Words.lh_Head->ln_Succ)
   {
    if(c->cl_NumWords > 1)
      sprintf(line,"** Class : %s :  %ld words ** %s, %s...",p->ln_Name,c->cl_NumWords,c->cl_Words.lh_Head->ln_Name,c->cl_Words.lh_Head->ln_Succ->ln_Name);
    else
      sprintf(line,"** Class : %s :  %ld words ** %s",p->ln_Name,c->cl_NumWords,c->cl_Words.lh_Head->ln_Name);
    line_out(line);
   }
   else line_out("*** No words in this class !!");
  }
 }
 else
 line_out("* No Classes");
 line_out("***************************************");  
 return (TRUE);
}

int Project0WriteIcon(void)
{
 if(edit.writeicon)
  edit.writeicon = 0;
 else
  edit.writeicon = 1;
 return TRUE;
}
