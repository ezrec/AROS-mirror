const char *spc_names[] = {
	/*   0 */ "wordwrap",
	/*   1 */ "windowtitles",
	/*   2 */ "windowcycling",
	/*   3 */ "warningsoncmdshell",
	/*   4 */ "viewmode",
	/*   5 */ "version",
	/*   6 */ "tfpen",
	/*   7 */ "tbpen",
	/*   8 */ "tabstop",
	/*   9 */ "sourcebreaks",
	/*  10 */ "simpletabs",
	/*  11 */ "shortlines",
	/*  12 */ "scanf",
	/*  13 */ "savetabs",
	/*  14 */ "saveicons",
	/*  15 */ "rxresult",
	/*  16 */ "rexxport",
	/*  17 */ "restofline",
	/*  18 */ "reqresult",
	/*  19 */ "repstr",
	/*  20 */ "parcol",
	/*  21 */ "modified",
	/*  22 */ "menustrip",
	/*  23 */ "margin",
	/*  24 */ "lineno",
	/*  25 */ "keytable",
	/*  26 */ "itemcheck",
	/*  27 */ "insertmode",
	/*  28 */ "ignorecase",
	/*  29 */ "hgpen",
	/*  30 */ "globalsearch",
	/*  31 */ "fname",
	/*  32 */ "findstr",
	/*  33 */ "filename",
	/*  34 */ "fgpen",
	/*  35 */ "errorsoncmdshell",
	/*  36 */ "dobackup",
	/*  37 */ "debug",
	/*  38 */ "currentword",
	/*  39 */ "currentline",
	/*  40 */ "currentdir",
	/*  41 */ "colno",
	/*  42 */ "cmdshellprompt",
	/*  43 */ "cmdshellname",
	/*  44 */ "cmdshell",
	/*  45 */ "bgpen",
	/*  46 */ "bbpen",
	/*  47 */ "autounblock",
	/*  48 */ "autosplit",
	/*  49 */ "autoindent",
	/*  50 */ "ascii",
	/*  51 */ "appicontitle",
	/*  52 */ "appiconname",
	/*  53 */ "appicondropaction",
	/*  54 */ "appiconclickaction",
	/*  55 */ "appicon",
	/*  56 */ "activetofront",
};

#define spc_num_vars 57

char *spc_get(long lock) {
    long array[6];
    char *value   = NULL;
    char *av0_bak = av[0];
    char *av1_bak = av[1];
    char *av2_bak = av[2];
    switch (lock) {
    case ~0: /* wordwrap */
value = GETF_WORDWRAP(Ep) ?"1":"0"; 
    break;
    case ~1: /* windowtitles */
value = GETF_WINDOWTITLES(Ep) ?"1":"0"; 
    break;
    case ~2: /* windowcycling */
value = GETF_WINDOWCYCLING(Ep) ?"1":"0"; 
    break;
    case ~3: /* warningsoncmdshell */
value = CMDSH_WarningsOut ?"1":"0"; 
    break;
    case ~4: /* viewmode */
value = GETF_VIEWMODE(Ep) ?"1":"0"; 
    break;
    case ~5: /* version */
value = version; 
    break;
    case ~6: /* tfpen */
value = ltostr((long)TITLE_FPEN(Ep)); 
    break;
    case ~7: /* tbpen */
value = ltostr((long)TITLE_BPEN(Ep)); 
    break;
    case ~8: /* tabstop */
value = ltostr((long)Ep->config.tabstop); 
    break;
    case ~9: /* sourcebreaks */
value = GETF_SOURCEBREAKS(Ep) ?"1":"0"; 
    break;
    case ~10: /* simpletabs */
value = GETF_SIMPLETABS(Ep) ?"1":"0"; 
    break;
    case ~11: /* shortlines */
value = GETF_SLINE(Ep) ?"1":"0"; 
    break;
    case ~12: /* scanf */
value = String; 
    break;
    case ~13: /* savetabs */
value = GETF_SAVETABS(Ep) ?"1":"0"; 
    break;
    case ~14: /* saveicons */
value = GETF_SAVEICONS(Ep) ?"1":"0"; 
    break;
    case ~15: /* rxresult */
value = get_rexx_result(); 
    break;
    case ~16: /* rexxport */
value = RexxPortName; 
    break;
    case ~17: /* restofline */
value = Current + Ep->column; 
    break;
    case ~18: /* reqresult */
value = ReqString; 
    break;
    case ~19: /* repstr */
value = Rstr; 
    break;
    case ~20: /* parcol */
value = ltostr((long)Ep->config.wwcol); 
    break;
    case ~21: /* modified */
value = GETF_MODIFIED(Ep) ?"1":"0"; 
    break;
    case ~22: /* menustrip */

	if ((value = (void *)currentmenu()))
	    value = ((struct Node *)value)->ln_Name;
    
    break;
    case ~23: /* margin */
value = ltostr((long)Ep->config.margin); 
    break;
    case ~24: /* lineno */
value = ltostr((long)Ep->line + 1); 
    break;
    case ~25: /* keytable */

	if ((value = (void*)currenthash()))
	    value = ((struct Node *)value)->ln_Name;
    
    break;
    case ~26: /* itemcheck */

      if (active_menu)
	  value = (((struct MenuItem *)active_menu)->Flags & CHECKED) ? "1": "0";
    
    break;
    case ~27: /* insertmode */
value = GETF_INSERTMODE(Ep) ?"1":"0"; 
    break;
    case ~28: /* ignorecase */
value = GETF_IGNORECASE(Ep) ?"1":"0"; 
    break;
    case ~29: /* hgpen */
value = ltostr((long)BLOCK_FPEN(Ep)); 
    break;
    case ~30: /* globalsearch */
value = GETF_GLOBALSEARCH(Ep) ?"1":"0"; 
    break;
    case ~31: /* fname */
value = Ep->name; 
    break;
    case ~32: /* findstr */
value = Fstr; 
    break;
    case ~33: /* filename */
if (getpathto(Ep->dirlock, Ep->name, tmp_buffer))
	    value = tmp_buffer; 
    break;
    case ~34: /* fgpen */
value = ltostr((long)TEXT_FPEN(Ep)); 
    break;
    case ~35: /* errorsoncmdshell */
value = CMDSH_ErrorsOut ?"1":"0"; 
    break;
    case ~36: /* dobackup */
value = GETF_DOBACK(Ep) ?"1":"0"; 
    break;
    case ~37: /* debug */
value = GETF_DEBUG(Ep) ?"1":"0"; 
    break;
    case ~38: /* currentword */
value = current_word(); 
    break;
    case ~39: /* currentline */
value = Current; 
    break;
    case ~40: /* currentdir */
if (NameFromLock(Ep->dirlock, tmp_buffer, sizeof(tmp_buffer)))
	    value = tmp_buffer; 
    break;
    case ~41: /* colno */
value = ltostr((long)Ep->column + 1); 
    break;
    case ~42: /* cmdshellprompt */
value = CMDSH_Prompt; 
    break;
    case ~43: /* cmdshellname */
value = CMDSH_FileName; 
    break;
    case ~44: /* cmdshell */
value = CMDSH_Active ?"1":"0"; 
    break;
    case ~45: /* bgpen */
value = ltostr((long)TEXT_BPEN(Ep)); 
    break;
    case ~46: /* bbpen */
value = ltostr((long)BLOCK_BPEN(Ep)); 
    break;
    case ~47: /* autounblock */
value = GETF_AUTOUNBLOCK(Ep) ?"1":"0"; 
    break;
    case ~48: /* autosplit */
value = GETF_AUTOSPLIT(Ep) ?"1":"0"; 
    break;
    case ~49: /* autoindent */
value = GETF_AUTOINDENT(Ep) ?"1":"0"; 
    break;
    case ~50: /* ascii */
value = ltostr((long)Current[Ep->column]); 
    break;
    case ~51: /* appicontitle */
value = APIC_IconTitle; 
    break;
    case ~52: /* appiconname */
value = APIC_IconFile; 
    break;
    case ~53: /* appicondropaction */
value = APIC_ActionDrop; 
    break;
    case ~54: /* appiconclickaction */
value = APIC_ActionDblClick; 
    break;
    case ~55: /* appicon */
value = APIC_Active ?"1":"0"; 
    break;
    case ~56: /* activetofront */
value = GETF_ACTIVATETOFRONT(Ep) ?"1":"0"; 
    break;
    default:
        return NULL;
    }
    return value;
} /* spc_get */

int spc_set(long lock, char *value) {
    long array[6];
    char *av0_bak = av[0];
    char *av1_bak = av[1];
    char *av2_bak = av[2];
    switch (lock) {
    case ~0: /* wordwrap */
SETF_WORDWRAP (Ep, test_arg(value, GETF_WORDWRAP(Ep))); 
    break;
    case ~1: /* windowtitles */
SETF_WINDOWTITLES (Ep, test_arg(value, GETF_WINDOWTITLES(Ep))); 
    break;
    case ~2: /* windowcycling */
SETF_WINDOWCYCLING (Ep, test_arg(value, GETF_WINDOWCYCLING(Ep))); 
    break;
    case ~3: /* warningsoncmdshell */
CMDSH_WarningsOut = test_arg (value, CMDSH_WarningsOut); 
    break;
    case ~4: /* viewmode */
SETF_VIEWMODE (Ep, test_arg(value, GETF_VIEWMODE(Ep))); 
    break;
    case ~6: /* tfpen */
TITLE_FPEN(Ep) = atol(value); 
    break;
    case ~7: /* tbpen */
TITLE_BPEN(Ep) = atol(value); 
    break;
    case ~8: /* tabstop */
Ep->config.tabstop = atol(value); 
    break;
    case ~9: /* sourcebreaks */
SETF_SOURCEBREAKS (Ep, test_arg(value, GETF_SOURCEBREAKS(Ep))); 
    break;
    case ~10: /* simpletabs */
SETF_SIMPLETABS (Ep, test_arg(value, GETF_SIMPLETABS(Ep))); 
    break;
    case ~11: /* shortlines */
SETF_SLINE (Ep, test_arg(value, GETF_SLINE(Ep))); 
    break;
    case ~12: /* scanf */
String = strrep (String, value); 
    break;
    case ~13: /* savetabs */
SETF_SAVETABS (Ep, test_arg(value, GETF_SAVETABS(Ep))); 
    break;
    case ~14: /* saveicons */
SETF_SAVEICONS (Ep, test_arg(value, GETF_SAVEICONS(Ep))); 
    break;
    case ~15: /* rxresult */
{
	av[1] = value;
	put_rexx_result();
	av[1] = av1_bak;
    }
    break;
    case ~19: /* repstr */
strcpy(Rstr, value); 
    break;
    case ~20: /* parcol */
Ep->config.wwcol = atol(value); 
    break;
    case ~21: /* modified */
SETF_MODIFIED (Ep, test_arg(value, GETF_MODIFIED(Ep))); 
    break;
    case ~23: /* margin */
Ep->config.margin = atol(value); 
    break;
    case ~27: /* insertmode */
SETF_INSERTMODE (Ep, test_arg(value, GETF_INSERTMODE(Ep))); 
    break;
    case ~28: /* ignorecase */
SETF_IGNORECASE (Ep, test_arg(value, GETF_IGNORECASE(Ep))); 
    break;
    case ~29: /* hgpen */
BLOCK_FPEN(Ep) = atol(value); 
    break;
    case ~30: /* globalsearch */
SETF_GLOBALSEARCH (Ep, test_arg(value, GETF_GLOBALSEARCH(Ep))); 
    break;
    case ~31: /* fname */
strcpy(Ep->name, value); 
    break;
    case ~32: /* findstr */
strcpy(Fstr, value); 
    break;
    case ~34: /* fgpen */
TEXT_FPEN(Ep) = atol(value); 
    break;
    case ~35: /* errorsoncmdshell */
CMDSH_ErrorsOut = test_arg (value, CMDSH_ErrorsOut); 
    break;
    case ~36: /* dobackup */
SETF_DOBACK (Ep, test_arg(value, GETF_DOBACK(Ep))); 
    break;
    case ~37: /* debug */
SETF_DEBUG (Ep, test_arg(value, GETF_DEBUG(Ep))); 
    break;
    case ~39: /* currentline */
strcpy (Current, value); Clen = strlen(Current); while (Clen < Ep->column) Current[Clen++] = ' '; Current[Clen] = '\0'; 
    break;
    case ~42: /* cmdshellprompt */
CMDSH_Prompt = strrep (CMDSH_Prompt, value); 
    break;
    case ~43: /* cmdshellname */
CMDSH_FileName = strrep (CMDSH_FileName, value); 
    break;
    case ~44: /* cmdshell */
{ char inter; inter = test_arg(value, CMDSH_Active); if (inter != CMDSH_Active) if (inter) do_opencmdshell(); else do_closecmdshell(); } 
    break;
    case ~45: /* bgpen */
TEXT_BPEN(Ep) = atol(value); 
    break;
    case ~46: /* bbpen */
BLOCK_BPEN(Ep) = atol(value); 
    break;
    case ~47: /* autounblock */
SETF_AUTOUNBLOCK (Ep, test_arg(value, GETF_AUTOUNBLOCK(Ep))); 
    break;
    case ~48: /* autosplit */
SETF_AUTOSPLIT (Ep, test_arg(value, GETF_AUTOSPLIT(Ep))); 
    break;
    case ~49: /* autoindent */
SETF_AUTOINDENT (Ep, test_arg(value, GETF_AUTOINDENT(Ep))); 
    break;
    case ~51: /* appicontitle */
APIC_IconTitle = strrep (APIC_IconTitle, value); 
    break;
    case ~52: /* appiconname */
APIC_IconFile = strrep (APIC_IconFile, value); 
    break;
    case ~53: /* appicondropaction */
APIC_ActionDrop = strrep (APIC_ActionDrop, value); 
    break;
    case ~54: /* appiconclickaction */
APIC_ActionDblClick = strrep (APIC_ActionDblClick, value); 
    break;
    case ~55: /* appicon */
{ char inter; inter = test_arg(value, APIC_Active); if (inter != APIC_Active) if (inter) do_apicshow(); else do_apichide(); } 
    break;
    case ~56: /* activetofront */
SETF_ACTIVATETOFRONT (Ep, test_arg(value, GETF_ACTIVATETOFRONT(Ep))); 
    break;
    default:
        return 0;
    }
    return 1;
} /* spc_set */

