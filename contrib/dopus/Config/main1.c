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

#include "config.h"

int dooperationconfig()
{
	ULONG class;
	USHORT code,gadgetid,qual;
	char *flag=NULL,*flag2=NULL;
	struct ConfigUndo *undo;
	struct Gadget *gad;
	int lastsel=-1,a,disppos=0,b,formatwin=0,x,y;
	struct DOpusListView *view;
	struct DOpusRemember *borderkey=NULL;
	
kprintf("dooperationconfig\n");

	showconfigscreen(CFG_OPERATION);
	undo=makeundo(UNDO_MAIN);

	FOREVER {
		Wait(1<<Window->UserPort->mp_SigBit);
		while ((IMsg=getintuimsg())) {
			if (lastsel!=OP_FORMAT ||
				((view=ListViewIDCMP(listformatlists,IMsg))==(struct DOpusListView *)-1)) {
				class=IMsg->Class; code=IMsg->Code; qual=IMsg->Qualifier;
				x=IMsg->MouseX; y=IMsg->MouseY;
				if (class==GADGETUP || class==GADGETDOWN) {
					gad=(struct Gadget *)IMsg->IAddress;
					gadgetid=gad->GadgetID;
				}
				ReplyMsg((struct Message *)IMsg);
				switch (class) {
					case IDCMP_MENUPICK:
						if (code==MENUNULL) break;
						if (lastsel>-1) {
							maingad[lastsel].Flags&=~SELECTED;
							select_gadget(&maingad[lastsel],0);
							if (lastsel==OP_FORMAT) {
								RemoveGList(Window,formatgads,(network)?10:8);
								RemoveListView(listformatlists,2);
							}
							removetickgads();
							lastsel=-1;
						}
						switch ((a=ITEMNUM(code))) {
							case 0:
							case 1:
								doload(CFG_OPERATION,a);
								break;
						}
						break;

					case IDCMP_MOUSEBUTTONS:
						if (code==MENUDOWN && lastsel==OP_FORMAT) {
							fixrmbimage(&formatclearrmb,NULL,0,x,y);
							a=DoRMBGadget(&formatclearrmb,Window);
							fixrmbimage(&formatclearrmb,NULL,1,-1,-1);
							if (a==0) goto clearlast;
						}
						break;

					case IDCMP_GADGETUP:
					case IDCMP_GADGETDOWN:
						if (gadgetid>=GAD_BASE && flag) {
							if (gadgetid<5000)
								*flag=processtickgad(operationgadgets[lastsel],(int)*flag,gadgetid-GAD_BASE,0);
							else *flag2=processtickgad(operationgadgets[lastsel],(int)*flag2,gadgetid-5000,1);
						}
						else switch (gadgetid) {
							case OP_CANCEL:
								doundo(undo,UNDO_MAIN);
							case OP_OKAY:
								doundo(undo,0);
								if (lastsel==OP_FORMAT) {
									RemoveGList(Window,formatgads,(network)?10:8);
									RemoveListView(listformatlists,2);
									LFreeRemember(&borderkey);
								}
								return((gadgetid==OP_OKAY));

							case FORMAT_CLEAR:
								if (qual&(IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT)) {
clearlast:
									for (a=15;a>=0;a--) {
										if (config->displaypos[formatwin][a]!=-1) {
											if (config->sortmethod[formatwin]==config->displaypos[formatwin][a]) {
												config->sortmethod[formatwin]=-1;
												listformatlists[1].itemselected=-1;
											}
											config->displaypos[formatwin][a]=-1;
											break;
										}
									}
									disppos=fixformlist(formatwin);
									break;
								}
							case FORMAT_RESET:
								for (a=0;a<16;a++) config->displaypos[formatwin][a]=-1;
								if (gadgetid==FORMAT_RESET) {
									for (a=0;a<5;a++) config->displaypos[formatwin][a]=a;
									config->sortmethod[formatwin]=0;
								}
								else config->sortmethod[formatwin]=-1;
								listformatlists[1].itemselected=config->sortmethod[formatwin];
								disppos=fixformlist(formatwin);
								break;

							case FORMAT_LENGTH+0:
							case FORMAT_LENGTH+1:
							case FORMAT_LENGTH+2:
							case FORMAT_LENGTH+3:
							case FORMAT_LENGTH+4:
								b=gadgetid-FORMAT_LENGTH;
								a=atoi(formatlen_buf[b]);
								if (a<5) a=5; else if (a>maxlength[b]) a=maxlength[b];
								lsprintf(formatlen_buf[b],"%ld",a);
								RefreshStrGad(gad,Window);
								config->displaylength[formatwin][b]=a;
								if (code!=0x9) getnextgadget(gad);
								break;

							case FORMAT_WINDOW:
								formatwin=1-formatwin;
								DoCycleGadget(&formatgads[0],Window,windownames,formatwin);
								DoCycleGadget(&formatgads[3],Window,separatenames,config->separatemethod[formatwin]);
								if (config->sortflags&(1<<formatwin)) formatgads[4].Flags|=GFLG_SELECTED;
								else formatgads[4].Flags&=~GFLG_SELECTED;
								RefreshGList(&formatgads[4],Window,NULL,1);
								listformatlists[1].itemselected=-1;
								for (a=0;a<16;a++) {
									if (config->displaypos[formatwin][a]==-1) break;
									if (config->displaypos[formatwin][a]==config->sortmethod[formatwin]) {
										listformatlists[1].itemselected=a;
										break;
									}
								}
								disppos=fixformlist(formatwin);
								fixdisplaylengths(formatwin);
								break;

							case FORMAT_SEPARATE:
								if (qual&(IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT)) {
									if ((--config->separatemethod[formatwin])<0)
										config->separatemethod[formatwin]=2;
								}
								else if ((++config->separatemethod[formatwin])>2)
									config->separatemethod[formatwin]=0;
								DoCycleGadget(&formatgads[3],Window,separatenames,config->separatemethod[formatwin]);
								break;

							case FORMAT_REVERSESORT:
								config->sortflags^=1<<formatwin;
								break;

							default:
								flag=NULL;
								if (lastsel>-1 && lastsel!=gadgetid) {
									maingad[lastsel].Flags&=~SELECTED;
									select_gadget(&maingad[lastsel],0);
									makehelpname((char *)-1);
									doscreentitle(cfg_string[STR_OPERATION_SCREEN]);
								}
								if (lastsel==OP_FORMAT) {
									RemoveGList(Window,formatgads,(network)?10:8);
									RemoveListView(listformatlists,2);
									LFreeRemember(&borderkey);
								}
								if (lastsel==gadgetid) {
									removetickgads();
									lastsel=-1;
									makehelpname((char *)-1);
									doscreentitle(cfg_string[STR_OPERATION_SCREEN]);
									break;
								}
								lastsel=gadgetid;
								flag2=NULL;
								makehelpname(operationgads[gadgetid]);
								doscreentitle(currenthelpname);
								switch (gadgetid) {
									case OP_COPY: flag=&(config->copyflags); flag2=&(config->existflags); break;
									case OP_DATEFORMAT: flag=&(config->dateformat); break;
									case OP_DELETE: flag=&(config->deleteflags); break;
									case OP_ERRORCHECK: flag=&(config->errorflags); break;
									case OP_GENERAL: flag=&(config->generalflags); break;
									case OP_ICONS: flag=&(config->iconflags); break;
									case OP_UPDATE: flag=&(config->dynamicflags); break;
									case OP_FORMAT:
										removetickgads();
										setuplist(&listformatlists[0],181,38);
										setuplist(&listformatlists[1],181,112);
										listformatlists[1].itemselected=-1;
										for (a=0;a<16;a++) {
											if (config->displaypos[formatwin][a]==-1) break;
											if (config->displaypos[formatwin][a]==config->sortmethod[formatwin]) {
												listformatlists[1].itemselected=a;
												break;
											}
										}
										if (!(AddListView(listformatlists,2))) {
											lastsel=-1;
											break;
										}
										if (config->sortflags&(1<<formatwin)) formatgads[4].Flags|=GFLG_SELECTED;
										else formatgads[4].Flags&=~GFLG_SELECTED;
										SetAPen(rp,screen_pens[1].pen);

										AddGadgetBorders(&borderkey,
											&formatgads[0],
											1,
											screen_pens[(int)config->gadgettopcol].pen,screen_pens[(int)config->gadgetbotcol].pen);
										AddGadgetBorders(&borderkey,
											&formatgads[1],
											1,
											screen_pens[(int)config->gadgettopcol].pen,screen_pens[(int)config->gadgetbotcol].pen);
										AddGadgetBorders(&borderkey,
											&formatgads[3],
											1,
											screen_pens[(int)config->gadgettopcol].pen,screen_pens[(int)config->gadgetbotcol].pen);
										AddGadgets(Window,
											formatgads,
											formatgadgets,
											(network)?10:8,
											screen_pens[(int)config->gadgettopcol].pen,screen_pens[(int)config->gadgetbotcol].pen,1);
										fixrmbimage(&formatclearrmb,&formatgads[2],2,-1,-1);

										formatwin=0;
										DoCycleGadget(&formatgads[0],Window,windownames,formatwin);
										DoCycleGadget(&formatgads[3],Window,separatenames,config->separatemethod[formatwin]);
										disppos=fixformlist(formatwin);
										fixdisplaylengths(formatwin);
										UScoreText(rp,
											cfg_string[STR_AVAILABLE_DISPLAY_ITEMS],
											x_off+156,y_off+34,
											-1);
										UScoreText(rp,
											cfg_string[STR_SELECTED_DISPLAY_ITEMS],
											x_off+156,y_off+108,
											-1);
										UScoreText(rp,
											cfg_string[STR_DISPLAYED_LENGTHS],
											x_off+430,y_off+34,
											-1);
										UScoreText(rp,
											cfg_string[STR_CHECK_INDICATES_SORT],
											x_off+161,y_off+179,
											-1);
										UScoreText(rp,
											cfg_string[STR_ENTRY_SEPARATION],
											x_off+416,y_off+152,
											-1);
										BltTemplate((PLANEPTR)DOpusBase->pdb_check,0,2,rp,x_off+169,y_off+173,13,7);
										flag=NULL;
										break;
								}
								if (flag) inittickgads(operationgadgets[gadgetid],(int)*flag,(flag2)?(int)*flag2:0);
								break;
						}
						break;
				}
			}
			else if (view) {
				if (view->listid==1) {
					b=view->itemselected;
					if (disppos<FORMAT_MAXNUM) {
						for (a=0;a<disppos;a++) {
							if (config->displaypos[formatwin][a]==b) break;
						}
						if (a==disppos) {
							config->displaypos[formatwin][disppos]=b;
							disppos=fixformlist(formatwin);
						}
					}
				}
				else if (view->listid==2) {
					a=config->displaypos[formatwin][view->itemselected];
					if (config->sortmethod[formatwin]==a)
						config->sortmethod[formatwin]=-1;
					else config->sortmethod[formatwin]=a;
				}
			}
		}
	}
}

int dosystemconfig()
{
	ULONG class;
	USHORT code,gadgetid,qual,hotcode,hotqual;
	char *flag=NULL,*flag2=NULL,buf[80],buf1[80],buf2[80],*ptr;
	struct ConfigUndo *undo;
	struct Gadget *gad;
	struct ConfigGadget *congad;
	int a,lastsel=-1,tick,x,y;
	struct DOpusListView *view;
	struct dopusfiletype *type;
	char module_sel_array[NUM_MODULES];
	struct DOpusRemember *lang_key=NULL;
	char old_language[30];
	
kprintf("dosystemconfig()\n");

	showconfigscreen(CFG_SYSTEM);
	undo=makeundo(UNDO_MAIN|UNDO_FILETYPE);
	hotcode=config->hotkeycode; hotqual=config->hotkeyqual;
	strcpy(old_language,config->language);

	FOREVER {
		Wait(1<<Window->UserPort->mp_SigBit);
		while ((IMsg=getintuimsg())) {
			if (!(lastsel==SYS_ICONS &&
				(view=ListViewIDCMP(&iconlistview,IMsg))!=(struct DOpusListView *)-1) &&
				!(lastsel==SYS_MODULES &&
				(view=ListViewIDCMP(&modulelist,IMsg))!=(struct DOpusListView *)-1)) {
				class=IMsg->Class; code=IMsg->Code; qual=IMsg->Qualifier;
				x=IMsg->MouseX; y=IMsg->MouseY;
				if (class==IDCMP_GADGETUP || class==IDCMP_GADGETDOWN) {
					gad=(struct Gadget *)IMsg->IAddress;
					gadgetid=gad->GadgetID;
				}
				ReplyMsg((struct Message *)IMsg);
				switch (class) {
					case IDCMP_MOUSEBUTTONS:
						if (code==MENUDOWN && lastsel==SYS_HOTKEYS) {
							if (fixrmbimage(&sampleclearrmb,NULL,0,x,y)) {
								a=DoRMBGadget(&sampleclearrmb,Window);
								fixrmbimage(&sampleclearrmb,NULL,1,-1,-1);
								if (a==0) {
									hotcode=(USHORT)~0;
									hotqual=0;
									showkey(hotcode,hotqual,x_off+138,y_off+71,360,10);
								}
								hotkeygad.Flags&=~SELECTED;
								help_ok=1;
							}
						}
						break;

					case IDCMP_RAWKEY:
						if (lastsel!=SYS_HOTKEYS || code&0x80 || !(hotkeygad.Flags&SELECTED))
							break;
						qual&=IEQUALIFIER_LCOMMAND|IEQUALIFIER_RCOMMAND|
							IEQUALIFIER_CONTROL|IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT|
							IEQUALIFIER_LALT|IEQUALIFIER_RALT;
						if (code>=0x60 && code<=0x67) code=(USHORT)~0;
						showkey((hotcode=code),(hotqual=qual),x_off+138,y_off+71,360,10);
						break;

					case IDCMP_MENUPICK:
						if (code==MENUNULL) break;
						if (lastsel>-1) {
							maingad[lastsel].Flags&=~SELECTED;
							select_gadget(&maingad[lastsel],0);
							if (lastsel==SYS_HOTKEYS) {
								config->hotkeycode=hotcode; config->hotkeyqual=hotqual;
								RemoveGList(Window,&hotkeygad,-1);
							}
							else if (lastsel==SYS_ICONS) {
								RemoveGList(Window,icongads,4);
								RemoveListView(&iconlistview,1);
								system_makeiconlist(0);
							}
							else if (lastsel==SYS_MODULES) {
								RemoveListView(&modulelist,2);
								LFreeRemember(&lang_key);
							}
							removetickgads();
							lastsel=-1;
						}
						switch ((a=ITEMNUM(code))) {
							case 0:
							case 1:
								if (doload(CFG_SYSTEM,a))
									configmsg(CONFIG_NEW_HOTKEY);
								break;
						}
						break;

					case IDCMP_GADGETUP:
					case IDCMP_GADGETDOWN:
						if (gadgetid>=REQ_BASE) {
							congad=(struct ConfigGadget *)gad->UserData;
							ptr=((struct StringInfo *)congad->gad->SpecialInfo)->Buffer;
							if (system_requester(ptr,congad->buffer,congad->gad,congad->bit))
								processtickgad(systemgadgets[lastsel],0,-1,-1);
						}
						else if (gadgetid>=STRING_BASE) {
							processtickgad(systemgadgets[lastsel],0,-1,-1);
							switch (lastsel) {
								case SYS_DIRECTORIES:
									if ((a=atoi(buf))<1) a=1;
									else if (a>255) a=255;
									config->bufcount=a;
									break;
								case SYS_AMIGADOS:
									config->priority=atoi(buf);
									break;
								case SYS_VIEWPLAY:
									config->showdelay=atoi(buf);
									config->fadetime=atoi(buf1);
									config->tabsize=atoi(buf2);
									break;
							}
							if (code!=0x9) getnextgadget(gad);
						}
						else if (gadgetid>=GAD_BASE && flag) {
							if (gadgetid<5000) *flag=processtickgad(systemgadgets[lastsel],(int)*flag,gadgetid-GAD_BASE,0);
							else *flag2=processtickgad(systemgadgets[lastsel],(int)*flag2,gadgetid-5000,1);
						}
						else switch (gadgetid) {
							case SYS_CANCEL:
								doundo(undo,UNDO_MAIN|UNDO_FILETYPE);
							case SYS_OKAY:
								if (gadgetid==SYS_OKAY) {
									LParsePatternI(config->showpat,config->showpatparsed);
									LParsePatternI(config->hidepat,config->hidepatparsed);
									config->hotkeycode=hotcode; config->hotkeyqual=hotqual;
									configmsg(CONFIG_NEW_HOTKEY);
									if (strcmp(config->language,old_language)) {
										busy();
										read_strings();
										readhelp();
										unbusy();
									}
								}
								switch (lastsel) {
									case SYS_HOTKEYS:
										RemoveGList(Window,&hotkeygad,-1);
										help_ok=1;
										break;
									case SYS_ICONS:
										RemoveGList(Window,icongads,4);
										RemoveListView(&iconlistview,1);
										system_makeiconlist(0);
										break;
									case SYS_MODULES:
										RemoveListView(&modulelist,2);
										LFreeRemember(&lang_key);
										break;
								}
								doundo(undo,0);
								help_ok=1;
								return((gadgetid==SYS_OKAY));

							case HOTKEY_SAMPLE:
								if (hotkeygad.Flags&SELECTED) {
									config->hotkeycode=config->hotkeyqual=(USHORT)~0;
									help_ok=0;
								}
								else {
									config->hotkeycode=hotcode;
									config->hotkeyqual=hotqual;
									help_ok=1;
								}
								configmsg(CONFIG_NEW_HOTKEY);
								break;

							case HOTKEY_MMB:
								config->hotkeyflags^=1;
								configmsg(CONFIG_NEW_HOTKEY);
								break;

							case ICON_ICONREQ:
								if (iconlistview.itemselected<0) break;
								if (!system_requester(edit_funcbuf,NULL,&icongads[1],0)) break;

							case ICON_ICONPATH:
								if (iconlistview.itemselected<0) break;
								ptr=NULL;
								switch (iconlistview.itemselected) {
									case 0: ptr=config->drawericon; break;
									case 1: ptr=config->toolicon; break;
									case 2: ptr=config->projecticon; break;
									default:
										for (a=3,type=firsttype;a<iconlistview.itemselected && type;a++,type=type->next);
										if (type) {
											if (edit_funcbuf[0]) {
												if ((type->iconpath=LAllocRemember(&typekey,strlen(edit_funcbuf)+1,0)))
													strcpy(type->iconpath,edit_funcbuf);
											}
											else type->iconpath=NULL;
										}
										break;
								}
								if (ptr) LStrnCpy(ptr,edit_funcbuf,79);
								if (edit_funcbuf[0]) iconlistview.items[iconlistview.itemselected][1]='*';
								else iconlistview.items[iconlistview.itemselected][1]=' ';
								RefreshListView(&iconlistview,1);
								system_showicon(edit_funcbuf,iconlistview.itemselected);
								if (code!=0x9) getnextgadget(gad);
								break;

							case ICON_TOOLREQ:
								if (!system_requester(edit_pathbuf,NULL,&icongads[3],0)) break;
							case ICON_TOOLPATH:
								LStrnCpy(config->defaulttool,edit_pathbuf,79);
								if (code!=0x9) getnextgadget(gad);
								break;

							default:
								flag=NULL;
								if (lastsel>-1 && lastsel!=gadgetid) {
									maingad[lastsel].Flags&=~SELECTED;
									select_gadget(&maingad[lastsel],0);
									makehelpname((char *)-1);
									doscreentitle(cfg_string[STR_SYSTEM_SCREEN]);
								}
								switch (lastsel) {
									case SYS_HOTKEYS:
										config->hotkeycode=hotcode;
										config->hotkeyqual=hotqual;
										RemoveGList(Window,&hotkeygad,-1);
										help_ok=1;
										break;
									case SYS_ICONS:
										RemoveGList(Window,icongads,4);
										RemoveListView(&iconlistview,1);
										system_makeiconlist(0);
										break;
									case SYS_MODULES:
										RemoveListView(&modulelist,2);
										LFreeRemember(&lang_key);
										break;
								}
								if (lastsel==gadgetid) {
									removetickgads();
									lastsel=-1;
									makehelpname((char *)-1);
									doscreentitle(cfg_string[STR_SYSTEM_SCREEN]);
									break;
								}
								lastsel=gadgetid;
								flag2=NULL; tick=1;
								makehelpname(systemgads[gadgetid]);
								doscreentitle(currenthelpname);
								switch (gadgetid) {
									case SYS_CLOCK: flag=&(config->scrclktype); flag2=&(config->icontype); break;
									case SYS_DIRECTORIES: flag=&(config->dirflags); flag2=&(config->showfree); break;
									case SYS_SHOWPATTERN: flag=&(config->hiddenbit); break;
									case SYS_VIEWPLAY: flag=&(config->viewbits); break;
									case SYS_ICONS: tick=0; break;
								}
								if (tick) inittickgads(systemgadgets[gadgetid],(flag)?(int)*flag:0,(flag2)?(int)*flag2:0);
								switch (gadgetid) {
									case SYS_AMIGADOS:
										lsprintf(buf,"%ld",config->priority);
										makestring(config->outputcmd,config->output,config->shellstartup,
											buf,NULL);
										break;
									case SYS_STARTUP:
										makestring(config->autodirs[0],config->autodirs[1],
											config->startupscript,config->uniconscript,config->configreturnscript,NULL);
										break;
									case SYS_DIRECTORIES:
										lsprintf(buf,"%ld",config->bufcount);
										makestring(buf,NULL);
										break;
									case SYS_HOTKEYS:
										SetAPen(rp,screen_pens[1].pen);
										hotkeygad.Flags&=~GFLG_SELECTED;
										if (config->hotkeyflags&1) hotkeymmbgad.Flags|=GFLG_SELECTED;
										else hotkeymmbgad.Flags&=~GFLG_SELECTED;
										UScoreText(rp,
											cfg_string[STR_UNICONIFY_HOTKEY],
											x_off+166,y_off+64,
											-1);
										AddGadgets(Window,
											&hotkeygad,
											hotkeygadgets,
											2,
											screen_pens[(int)config->gadgettopcol].pen,screen_pens[(int)config->gadgetbotcol].pen,1);
										fixrmbimage(&sampleclearrmb,&hotkeygad,2,-1,-1);
										showkey((hotcode=config->hotkeycode),
											(hotqual=config->hotkeyqual),
											x_off+138,y_off+71,
											360,10);
										break;
									case SYS_MODULES:
										busy();
										for (a=0;a<NUM_MODULES;a++) {
											if (config->loadexternal&external_module_map[a])
												module_sel_array[a]=LVARRAY_SELECTED;
											else module_sel_array[a]=0;
											if (!(FindSystemFile(external_module_name[a],buf,80,SYSFILE_MODULE)))
												module_sel_array[a]|=2;
										}
										modulelist.items=external_module_list;
										modulelist.selectarray=module_sel_array;
										get_language_list(&lang_key);
										setuplist(&modulelist,200,18);
										setuplist(&languagelist,200,115);
										unbusy();
										if (!(AddListView(&modulelist,2))) {
											lastsel=-1;
											break;
										}
										SetAPen(rp,screen_pens[1].pen);
										UScoreText(rp,
											cfg_string[STR_MODULE_LIST_CHECK_STRING],
											x_off+200,y_off+83,
											-1);
										BltTemplate((PLANEPTR)DOpusBase->pdb_check,0,2,rp,x_off+208,y_off+77,13,7);
										UScoreText(rp,
											cfg_string[STR_LANGUAGE_LIST_CHECK_STRING],
											x_off+200,y_off+180,
											-1);
										BltTemplate((PLANEPTR)DOpusBase->pdb_check,0,2,rp,x_off+208,y_off+174,13,7);
										break;
									case SYS_VIEWPLAY:
										lsprintf(buf,"%ld",config->showdelay);
										lsprintf(buf1,"%ld",config->fadetime);
										lsprintf(buf2,"%ld",config->tabsize);
										makestring(buf,buf1,buf2,NULL);
										break;
									case SYS_SHOWPATTERN:
										makestring(config->showpat,config->hidepat,NULL);
										break;
									case SYS_ICONS:
										removetickgads();
										setuplist(&iconlistview,140,19);
										system_makeiconlist(1);
										if (!(AddListView(&iconlistview,1))) {
											system_makeiconlist(0);
											lastsel=-1;
											break;
										}
										SetAPen(rp,screen_pens[1].pen);
										UScoreText(rp,
											cfg_string[STR_PROJECTS_DEFAULT_TOOL],
											x_off+172,y_off+150,
											-1);
										strcpy(edit_pathbuf,config->defaulttool);
										edit_funcbuf[0]=0;
										AddGadgets(Window,
											icongads,
											NULL,
											4,
											screen_pens[(int)config->gadgettopcol].pen,screen_pens[(int)config->gadgetbotcol].pen,1);
										Do3DBox(rp,
											x_off+459,y_off+126,
											149,40,
											screen_pens[(int)config->gadgetbotcol].pen,screen_pens[(int)config->gadgettopcol].pen);
										doglassimage(&icongads[0]);
										doglassimage(&icongads[2]);
										flag=NULL;
										break;
								}
								break;
						}
						break;
				}
			}
			else if (view) {
				switch (view->listid) {
					case 3:
						ptr=NULL;
						switch (view->itemselected) {
							case -1: break;
							case 0: ptr=config->drawericon; break;
							case 1: ptr=config->toolicon; break;
							case 2: ptr=config->projecticon; break;
							default:
								for (a=3,type=firsttype;a<iconlistview.itemselected && type;a++,type=type->next);
								if (type && type->iconpath) ptr=type->iconpath;
								break;
						}
						if (ptr) strcpy(edit_funcbuf,ptr);
						else edit_funcbuf[0]=0;
						RefreshStrGad(&icongads[1],Window);
						ActivateStrGad(&icongads[1],Window);
						system_showicon(edit_funcbuf,view->itemselected);
						break;
					case 5:
						if (config->loadexternal&external_module_map[view->itemselected])
							config->loadexternal&=~external_module_map[view->itemselected];
						else config->loadexternal|=external_module_map[view->itemselected];
						break;
					case 6:
						if (strcmp(view->items[view->itemselected],"English (built-in)")==0)
							config->language[0]=0;
						else strcpy(config->language,view->items[view->itemselected]);
						break;
				}
			}
		}
	}
}

void getnextgadget(fgad)
struct Gadget *fgad;
{
	struct Gadget *gad;

	if (fgad->GadgetType==GTYP_STRGADGET) RefreshStrGad(fgad,Window);
	gad=fgad->NextGadget;
	while (gad) {
		if (gad->GadgetType==GTYP_STRGADGET && !(gad->Flags&GFLG_DISABLED)) {
			ActivateStrGad(gad,Window);
			return;
		}
		gad=gad->NextGadget;
	}
	gad=Window->FirstGadget;
	while (gad && gad!=fgad) {
		if (gad->GadgetType==GTYP_STRGADGET && !(gad->Flags&GFLG_DISABLED)) {
			ActivateStrGad(gad,Window);
			return;
		}
		gad=gad->NextGadget;
	}
}

int fixformlist(win)
int win;
{
	int a,b,d;

	for (a=0;a<FORMAT_MAXNUM;a++) {
		for (d=0;d<a;d++) {
			if (config->displaypos[win][a]==config->displaypos[win][d])
				config->displaypos[win][a]=-1;
		}
		if ((b=config->displaypos[win][a])<FORMAT_MAXNUM && b>-1)
			selectedformatnames[a]=formatnames[b];
		else break;
	}
	selectedformatnames[a]=NULL;
	RefreshListView(&listformatlists[1],1);
	return(a);
}

void fixdisplaylengths(win)
int win;
{
	int a,lim;

	lim=(network)?5:3;
	for (a=0;a<lim;a++) {
		if (config->displaylength[win][a]<5) config->displaylength[win][a]=5;
		else if (config->displaylength[win][a]>maxlength[a])
			config->displaylength[win][a]=maxlength[a];
		lsprintf(formatlen_buf[a],"%ld",config->displaylength[win][a]);
	}
	RefreshGList(&formatgads[5],Window,NULL,lim);
}

int system_requester(buf,buf2,gad,bit)
char *buf,*buf2;
struct Gadget *gad;
int bit;
{
	if (!buf[0]) {
		if (buf2 && CheckExist(buf2,NULL)) strcpy(dirbuf,buf2);
		else dirbuf[0]=0;
		filebuf[0]=0;
	}
	else makefilebuffer(buf);
	if (bit==0) {
		filereq.flags=0;
		filereq.title=cfg_string[STR_SELECT_DESIRED_FILE];
	}
	else {
		filereq.flags=DFRF_DIRREQ;
		filereq.title=cfg_string[STR_SELECT_DESIRED_DIRECTORY];
	}
	filereq.window=Window;
	if (FileRequest(&filereq)) {
		strcpy(buf,dirbuf);
		if (bit==0) TackOn(buf,filebuf,256);
		RefreshStrGad(gad,Window);
		return(1);
	}
	return(0);
}

void system_makeiconlist(make)
int make;
{
	static struct DOpusRemember *key=NULL;
	struct dopusfiletype *type;
	int a,count;

	LFreeRemember(&key);
	if (make) {
		for (count=3,type=firsttype;type;count++,type=type->next) {
			if (LStrCmpI(type->type,"Default")==0) --count;
		}
		if ((iconlistview.items=LAllocRemember(&key,(count+1)*4,MEMF_CLEAR))) {
			for (a=0;a<3;a++) iconlistview.items[a]=icontypes[a];
			if (config->drawericon[0]) icontypes[0][1]='*'; else icontypes[0][1]=' ';
			if (config->toolicon[0]) icontypes[1][1]='*'; else icontypes[1][1]=' ';
			if (config->projecticon[0]) icontypes[2][1]='*'; else icontypes[2][1]=' ';
			for (a=3,type=firsttype;a<count;a++,type=type->next) {
				if (LStrCmpI(type->type,"Default")==0) --a;
				else if ((iconlistview.items[a]=LAllocRemember(&key,strlen(type->type)+4,0))) {
					strcpy(iconlistview.items[a],"   ");
					strcat(iconlistview.items[a],type->type);
					if (type->iconpath) iconlistview.items[a][1]='*';
				}
			}
		}
		iconlistview.itemselected=-1; iconlistview.topitem=0;
	}
}

void system_showicon(iconfile,deftype)
char *iconfile;
int deftype;
{
	struct DiskObject *icon;
	char iconbuf[256],*ptr;
	int x,y;
	struct Image *image;
	struct Rectangle cliprec;
	struct Region *reg,*oldreg;
	
kprintf("system_showicon()\n");

	cliprec.MinX=x_off+459;
	cliprec.MinY=y_off+126;
	cliprec.MaxX=x_off+607;
	cliprec.MaxY=y_off+165;
	SetAPen(rp,screen_pens[0].pen);
	RectFill(rp,y_off+459,y_off+126,x_off+607,y_off+165);
	if (!IconBase) return;
	strcpy(iconbuf,iconfile);
	if ((ptr=strstri(iconbuf,".info"))) *ptr=0;
	if (!iconbuf[0] || !(icon=GetDiskObject(iconbuf))) {
		if (version2<OSVER_37 || deftype>2) return;
		icon=GetDefDiskObject((deftype==0)?WBDRAWER:((deftype==1)?WBTOOL:WBPROJECT));
		if (!icon) return;
	}
	if ((reg=NewRegion())) {
		OrRectRegion(reg,&cliprec);
		oldreg=InstallClipRegion(Window->WLayer,reg);
	}
	image=(struct Image *)icon->do_Gadget.GadgetRender;
	if ((x=x_off+459+((149-image->Width)/2))<(x_off+459)) x=x_off+456;
	if ((y=y_off+126+((40-image->Height)/2))<(y_off+126)) y=y_off+126;
	DrawImage(rp,image,x,y);
	FreeDiskObject(icon);
	if (reg) {
		InstallClipRegion(Window->WLayer,oldreg);
		DisposeRegion(reg);
	}
}

char *lock_dirs[]={
	"DOpus:S/",
	"S:"};

void get_language_list(key)
struct DOpusRemember **key;
{
	char *tempbuf;
	BPTR lock;
	
/* AROS: We have no __aligned keyword
	struct FileInfoBlock __aligned fib;
*/
	struct FileInfoBlock *fib;
	char pattern[30],buf[30];
	APTR wsave;
	int a,num,def;
	
	fib = AllocDosObject(DOS_FIB, NULL);
	assert(NULL != fib);

	languagelist.items=NULL;

	if (!(tempbuf=LAllocRemember(key,2500,MEMF_CLEAR))) goto exit;
	strcpy(tempbuf,"English (built-in)");
	num=1;
	LParsePatternI("DO_#?.STR",pattern);

	wsave=myproc->pr_WindowPtr;
	myproc->pr_WindowPtr=(APTR)-1;

	for (a=0;a<2;a++) {
		if ((lock=Lock(lock_dirs[a],ACCESS_READ))) {
			Examine(lock,fib);
			while (ExNext(lock,fib)) {
				if (fib->fib_DirEntryType<0 &&
					LMatchPatternI(pattern,fib->fib_FileName)) {
					strcpy(buf,&fib->fib_FileName[3]);
					buf[(strlen(buf)-4)]=0;
					strcpy(&tempbuf[num*25],buf);
					if ((++num)>99) break;
				}
			}
			UnLock(lock);
		}
		if (num>99) break;
	}

	myproc->pr_WindowPtr=wsave;

	if (!(languagelist.items=LAllocRemember(key,(num+1)*sizeof(char *),MEMF_CLEAR)))
		goto exit;

	for (a=0;a<num;a++) languagelist.items[a]=&tempbuf[a*25];
	quicksort(languagelist.items,0,num-1);

	languagelist.itemselected=-1;
	for (a=0;a<num;a++) {
		if (LStrCmpI(config->language,languagelist.items[a])==0)
			languagelist.itemselected=a;
		else if (strcmp(languagelist.items[a],"English (built-in)")==0)
			def=a;
	}
	if (languagelist.itemselected==-1) languagelist.itemselected=def;
	
exit:
	FreeDosObject(DOS_FIB, fib);	
}

void quicksort(array,left,right)
char **array;
int left,right;
{
	int i,last;

	if (left>=right) return;
	sortswap(array,left,(left+right)/2);
	last=left;
	for (i=left+1;i<=right;i++)
		if (LStrCmpI(array[i],array[left])<0)
			sortswap(array,++last,i);
	sortswap(array,left,last);
	quicksort(array,left,last-1);
	quicksort(array,last+1,right);
}

void sortswap(array,i,j)
char **array;
int i,j;
{
	char *temp;

	temp=array[i];
	array[i]=array[j];
	array[j]=temp;
}
