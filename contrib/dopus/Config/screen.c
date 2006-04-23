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

#include <diskfont/diskfont.h>

char old_general_font[30];

int doscreenconfig()
{
	ULONG class;
	USHORT code,gadgetid,qual;
	struct ConfigUndo *undo;
	struct Gadget *gad;
	int mode=-1,realpen=1,x,y,cursel=-1,a,b,fontnum=0,arrowact=0,c;
	char *fg=NULL,*bg=NULL,buf[256],*ptr;
	struct DOpusListView *view;
	struct Screen scrbuf;

	showconfigscreen(CFG_SCREEN);
	undo=makeundo(UNDO_MAIN);
	curmode=NULL;
	if (version2) fontsizelistview.h=52;

	FOREVER {
		Wait(1<<Window->UserPort->mp_SigBit);
		while ((IMsg=getintuimsg())) {
			if (!(mode==SCREEN_SCREENMODE &&
				(view=ListViewIDCMP(&screenmodeview,IMsg))!=(struct DOpusListView *)-1) &&
				!(mode==SCREEN_FONTS &&
				(view=ListViewIDCMP(&fontplacelist,IMsg))!=(struct DOpusListView *)-1) &&
				!(mode==SCREEN_PALETTE &&
				(view=ListViewIDCMP(&palettelist,IMsg))!=(struct DOpusListView *)-1)) {
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
						if (mode>-1) {
							select_gadget(&maingad[mode],0);
							removescreenedit(mode);
							mode=realpen=cursel=-1;
						}
						switch ((a=ITEMNUM(code))) {
							case 0:
							case 1:
								if (doload(CFG_SCREEN,a)) {
									if (config->scrdepth!=screen_depth)
										close_screen();
									open_screen();
									showconfigscreen(CFG_SCREEN);
								}
								break;
						}
						break;

					case IDCMP_MOUSEBUTTONS:
						if (code==SELECTDOWN) {
							if (mode==SCREEN_PALETTE) {
								if (x>x_off+297 && y>y_off+33 && x<x_off+514 && y<y_off+46) {
									for (a=0;a<(1<<screen_depth);a++) {
										if (x>=palettegad_xy[a][0] && x<=palettegad_xy[a][2] &&
											y>=palettegad_xy[a][1] && y<=palettegad_xy[a][3]) {
											realpen=screen_pens[a].pen;
											showcolourvals(realpen,1);
											showpalettegad(a);
											break;
										}
									}
								}
							}
							else if (mode==SCREEN_ARROWS) {
								for (a=0;a<3;a++) {
									if (x>x_off+182+(a*129) && x<x_off+307+(a*129) &&
										y>y_off+32 && y<y_off+89) {
										if (arrowact!=a) {
											showarrowborders(a,arrowact);
											arrowact=a;
											DoCycleGadget(&arrowgadgets[7],Window,arrowtypetxt,arrowact);
											fixarrowgads(arrowact);
											break;
										}
									}
								}
							}
							else if (mode==SCREEN_COLOURS && fg && bg) {
								a=*fg; b=*bg;
								if (docolourgad(rp,x_off+208,y_off+33,x,y,
									(cursel==COLBIT_3DBOXES)?NULL:cfg_string[STR_EXAMPLE],&a,&b,CFG_GADGET)) {
									*fg=a; *bg=b;
									if (cursel==COLBIT_3DBOXES) {
										showfuncob(rp,cfg_string[STR_EXAMPLE],1,0,CFG_GADGET,x_off+302,y_off+23);
										showscreencolours(COLOURS_ALL,NULL,NULL,4);
									}
									else showscreencolours((1<<cursel),NULL,NULL,0);
								}
								else {
									for (a=0;;a++) {
										if (coloursel[a].item==-1) break;
										if (x>=x_off+coloursel[a].x1 && x<=x_off+coloursel[a].x2 &&
											y>=y_off+coloursel[a].y1 && y<=y_off+coloursel[a].y2) {
											if (version2 || (a!=COLBIT_STRINGS && a!=COLBIT_SELSTRINGS)) {
												cursel=coloursel[a].item;
												showscreencolours((1<<cursel),&fg,&bg,0);
												showcolourscycle(*fg,*bg,cursel);
												setupcolourbox(rp,x_off+208,y_off+33,*fg,*bg);
											}
											break;
										}
									}
								}
							}
						}
						break;

					case IDCMP_RAWKEY:
						if ((code>=0x60 && code<=0x67) || code&0x80) break;
						buf[0]=0;
						RawkeyToStr(code,qual,NULL,buf,10);
						switch (LToUpper(buf[0])) {
							case 'R':
								if (mode==SCREEN_PALETTE) ActivateStrGad(&palettegads[3],Window);
								break;
							case 'G':
								if (mode==SCREEN_PALETTE) ActivateStrGad(&palettegads[4],Window);
								break;
							case 'B':
								if (mode==SCREEN_PALETTE) ActivateStrGad(&palettegads[5],Window);
								break;
							case 'W':
								if (mode==SCREEN_SCREENMODE) ActivateStrGad(&screenmodegads[0],Window);
								break;
							case 'H':
								if (mode==SCREEN_SCREENMODE) ActivateStrGad(&screenmodegads[1],Window);
								break;
							case 'C':
								if (mode==SCREEN_SCREENMODE) ActivateStrGad(&screenmodegads[2],Window);
								break;
						}
						break;

					case IDCMP_GADGETDOWN:
						if (gadgetid>=PALETTE_SLIDER && gadgetid<PALETTE_SLIDER+3) {
							updatecolourvals(realpen,0);
							FOREVER {
								Wait(1<<Window->UserPort->mp_SigBit);
								while ((IMsg=getintuimsg())) {
									class=IMsg->Class;
									ReplyMsg((struct Message *)IMsg);
									if (class==GADGETUP) break;
									if (class==MOUSEMOVE) updatecolourvals(realpen,0);
								}
								if (class==GADGETUP) {
									updatecolourvals(realpen,1);
									break;
								}
							}
						}
						else if (gadgetid>=ARROWBASE+3 && gadgetid<ARROWBASE+7) {
							gadgetid-=ARROWBASE+3; a=0;
							FOREVER {
								if (IntuitionBase->ActiveWindow!=Window ||
									!(gad->Flags&SELECTED)) break;
								if ((arrowact==0 && gadgetid>1) ||
									(arrowact>0 && gadgetid<2)) {
									b=config->arrowsize[arrowact];
									if ((arrowact==0 && gadgetid==2) ||
										(arrowact>0 && gadgetid==0))
										++config->arrowsize[arrowact];
									else --config->arrowsize[arrowact];
									switch (arrowact) {
										case 0:
											showupdownslider();
											break;
										case 1:
											showleftrightslider();
											break;
										case 2:
											showstringgadget();
											break;
									}
									if (b==config->arrowsize[arrowact]) a=-1;
								}
								else {
									switch (arrowact) {
										case 0:
											b=config->sliderwidth;
											if (gadgetid==1) config->sliderwidth-=2;
											else config->sliderwidth+=2;
											showupdownslider();
											if (b==config->sliderwidth) {
												a=-1;
												break;
											}
											break;
										case 1:
											b=config->sliderheight;
											if (gadgetid==3) config->sliderheight-=2;
											else config->sliderheight+=2;
											showleftrightslider();
											if (b==config->sliderheight) {
												a=-1;
												break;
											}
											break;
										case 2:
											b=config->stringheight;
											if (gadgetid==3) config->stringheight-=2;
											else config->stringheight+=2;
											showstringgadget();
											if (b==config->stringheight) {
												a=-1;
												break;
											}
											break;
									}
								}
								if (a==-1) break;
								else if (a==0) {
									Delay(5);
									a=1;
								}
								else Delay(1);
								while ((IMsg=getintuimsg())) {
									class=IMsg->Class;
									ReplyMsg((struct Message *)IMsg);
									if (class==GADGETUP) break;
								}
								if (class==GADGETUP) break;
							}
						}
						else if (gadgetid>=ARROWBASE && gadgetid<ARROWBASE+3) {
							gadgetid-=ARROWBASE;
							if (config->arrowpos[arrowact]!=gadgetid) {
								config->arrowpos[arrowact]=gadgetid;
								if (arrowact==0) showupdownslider();
								else if (arrowact==1) showleftrightslider();
								else showstringgadget();
							}
							fixarrowgads(arrowact);
						}
						break;

					case IDCMP_GADGETUP:
						if (mode==SCREEN_GENERAL && gadgetid>GAD_BASE) {
							config->generalscreenflags=processtickgad(scr_generalgadgets,config->generalscreenflags,gadgetid-GAD_BASE,-1);
							break;
						}
						if (gadgetid<SCREEN_OKAY) {
							if (mode==gadgetid) {
								removescreenedit(mode);
								mode=realpen=cursel=-1;
								makehelpname((char *)-1);
								doscreentitle(cfg_string[STR_SCREEN_SCREEN]);
								break;
							}
							if (mode>-1) {
								if (mode==SCREEN_SCREENMODE || mode==SCREEN_FONTS)
									select_gadget(&maingad[gadgetid],0);
								select_gadget(&maingad[mode],0);
								removescreenedit(mode);
								if (mode==SCREEN_SCREENMODE || mode==SCREEN_FONTS)
									select_gadget(&maingad[gadgetid],1);
								mode=realpen=cursel=-1;
								makehelpname((char *)-1);
							}
							makehelpname(screengadgets[gadgetid]);
							doscreentitle(currenthelpname);
						}
						if (gadgetid>=PALETTE_STRING && gadgetid<PALETTE_STRING+3) {
							updatecolourvals(realpen,-1);
							if (code!=0x9) getnextgadget(gad);
						}
						else switch (gadgetid) {
							case FONT_SIZE:
								if (!version2) break;
								RefreshStrGad(&fontsizegadget,Window);
								a=atoi(fontsize_buf);
								c=config->fontsizes[fontnum];
								config->fontsizes[fontnum]=a;
								for (b=0;b<fontsizelistview.count;b++)
									if (atoi(fontsizelistview.items[b])==a) break;
								if (b<fontsizelistview.count) {
									fontsizelistview.itemselected=b;
									if (b<fontsizelistview.topitem ||
										b>fontsizelistview.topitem+fontsizelistview.lines-1)
										fontsizelistview.topitem=b;
									RefreshListView(&fontsizelistview,1);
									fontsize_buf[0]=0;
									RefreshStrGad(&fontsizegadget,Window);
								}
								else {
									fontsizelistview.itemselected=-1;
									RefreshListView(&fontsizelistview,1);
								}
								if (c!=a) dofontdemo(config->fontbufs[fontnum],config->fontsizes[fontnum]);
								break;

							case ARROWCYCLE:
								a=arrowact;
								if (qual&IEQUALIFIER_LSHIFT || qual&IEQUALIFIER_RSHIFT) {
									if ((--arrowact)<0) arrowact=2;
								}
								else if ((++arrowact)==3) arrowact=0;
								showarrowborders(arrowact,a);
								DoCycleGadget(&arrowgadgets[7],Window,arrowtypetxt,arrowact);
								fixarrowgads(arrowact);
								break;

							case SCREEN_SLIDER_LEFTCYCLE:
								config->slider_pos^=1;
								show_slider_setup(182,32,0);
								break;

							case SCREEN_SLIDER_RIGHTCYCLE:
								config->slider_pos^=2;
								show_slider_setup(373,32,1);
								break;

							case SCREEN_PALETTE:
								realpen=screen_pens[1].pen;
							case SCREEN_FONTS:
								fontnum=FONT_CLOCK;
							case SCREEN_ARROWS:
								arrowact=0;
							case SCREEN_COLOURS:
							case SCREEN_GENERAL:
							case SCREEN_SCREENMODE:
							case SCREEN_SLIDERS:
								mode=gadgetid;
								makescreenedit(mode);
								if (gadgetid==SCREEN_COLOURS) {
									cursel=0;
									fg=&config->statusfg; bg=&config->statusbg;
								}
								break;

							case COLOURS_SELECT:
								if (qual&IEQUALIFIER_LSHIFT || qual&IEQUALIFIER_RSHIFT) {
									if ((--cursel)<0) cursel=(version2)?COLBIT_SELSTRINGS:COLBIT_3DBOXES;
								}
								else {
									if (!coloursgadgets[++cursel] ||
										(!version2 && cursel==COLBIT_STRINGS)) cursel=0;
								}
								showscreencolours((1<<cursel),&fg,&bg,0);
								showcolourscycle(*fg,*bg,cursel);
								setupcolourbox(rp,x_off+208,y_off+33,*fg,*bg);
								break;

							case SCREENMODE_WIDTH:
								a=atoi(screenwidth_buf);
								if (a%2) lsprintf(screenwidth_buf,"%ld",a+1);
								CheckNumGad(&screenmodegads[SCREENMODE_WIDTH-300],Window,curmode->minw,curmode->maxw);
								config->scrw=atoi(screenwidth_buf);
								config->scr_winw=config->scrw;
								if (code!=0x9) getnextgadget(gad);
								break;

							case SCREENMODE_HEIGHT:
								CheckNumGad(&screenmodegads[SCREENMODE_HEIGHT-300],Window,curmode->minh,curmode->maxh);
								config->scrh=atoi(screenheight_buf);
								config->scr_winh=config->scrh;
								if (code!=0x9) getnextgadget(gad);
								break;

							case SCREENMODE_DEPTH:
								a=atoi(screendepth_buf);
								for (b=curmode->maxdepth;b>=2;b--) {
									if (a>=(1<<b)) {
										a=b;
										break;
									}
								}
								if (b<=2) a=2;

							case SCREENMODE_SLIDER:
								if (gadgetid==SCREENMODE_SLIDER)
									a=GetSliderPos(&screenmodegads[SCREENMODE_SLIDER-300],curmode->maxdepth-1,1)+2;
								if (a>curmode->maxdepth) a=curmode->maxdepth;
								else if (a<2) a=2;
								lsprintf(screendepth_buf,"%ld",(1<<a));
								RefreshStrGad(&screenmodegads[SCREENMODE_DEPTH-300],Window);
								config->scrdepth=a;
								FixSliderPot(Window,&screenmodegads[SCREENMODE_SLIDER-300],config->scrdepth-2,curmode->maxdepth-1,1,2);
								if (gadgetid==SCREENMODE_DEPTH && code!=0x9) getnextgadget(gad);
								break;

							case SCREENMODE_DEFWIDTH:
								if (screenmodegads[SCREENMODE_DEFWIDTH-300].Flags&SELECTED) {
									config->screenflags|=SCRFLAGS_DEFWIDTH;
									EnableGadget(&screenmodegads[SCREENMODE_WIDTH-300],rp,0,0);
									config->scrw=curmode->defw;
									config->scr_winw=config->scrw;
									fixmodegads(curmode);
								}
								else config->screenflags&=~SCRFLAGS_DEFWIDTH;
								fixdefaultgads(curmode);
								fixmodegads(curmode);
								break;

							case SCREENMODE_DEFHEIGHT:
								if (screenmodegads[SCREENMODE_DEFHEIGHT-300].Flags&SELECTED) {
									config->screenflags|=SCRFLAGS_DEFHEIGHT;
									EnableGadget(&screenmodegads[SCREENMODE_HEIGHT-300],rp,0,0);
									config->scrh=curmode->defh;
									config->scr_winh=config->scrh;
									fixmodegads(curmode);
								}
								else config->screenflags&=~SCRFLAGS_DEFHEIGHT;
								fixdefaultgads(curmode);
								fixmodegads(curmode);
								break;

							case SCREENMODE_HALFHEIGHT:
								if (screenmodegads[SCREENMODE_HALFHEIGHT-300].Flags&SELECTED)
									config->screenflags|=SCRFLAGS_HALFHEIGHT;
								else config->screenflags&=~SCRFLAGS_HALFHEIGHT;
								break;

							case SCREEN_CANCEL:
								doundo(undo,UNDO_MAIN);
								if (config->scrdepth!=screen_depth) {
									if (mode>-1) {
										removescreenedit(mode);
										mode=-1;
									}
									close_screen();
								}
								open_screen();
							case SCREEN_OKAY:
								if (mode>-1) removescreenedit(mode);
								doundo(undo,0);
								return((gadgetid==SCREEN_OKAY));
						}
						break;
				}
			}
			else if (view) {
				if (mode==SCREEN_FONTS) {
					if (view->listid==1) {
						if (version2) {
							fontsize_buf[0]=0;
							RefreshStrGad(&fontsizegadget,Window);
						}
						fontnum=fontplacevals[view->itemselected];
						initfontlist(fontnum,1,fontplaceflags[fontnum]);
					}
					else if (fontnum>=0 && fontnum<NUMFONTS) {
						if (view->listid==2) {
							strcpy(buf,config->fontbufs[fontnum]);
							if ((ptr=strstri(buf,".font"))) *ptr=0;
							if (strcmp(buf,view->items[view->itemselected])) {
								if (version2) {
									fontsize_buf[0]=0;
									RefreshStrGad(&fontsizegadget,Window);
								}
								strcpy(config->fontbufs[fontnum],view->items[view->itemselected]);
								config->fontsizes[fontnum]=initfontlist(fontnum,0,fontplaceflags[fontnum]);
								StrConcat(config->fontbufs[fontnum],".font",40);
							}
						}
						else if (view->listid==3) {
							if (version2) {
								fontsize_buf[0]=0;
								RefreshStrGad(&fontsizegadget,Window);
							}
							a=config->fontsizes[fontnum];
							config->fontsizes[fontnum]=atoi(view->items[view->itemselected]);
							if (a!=config->fontsizes[fontnum])
								dofontdemo(config->fontbufs[fontnum],config->fontsizes[fontnum]);
						}
					}
				}
				else if (view->listid==1) {
					curmode=showdisplaydesc();
					fixdefaultgads(curmode);
					if (screenmodegads[SCREENMODE_DEFWIDTH-300].Flags&SELECTED) {
						config->scrw=curmode->defw;
						config->scr_winw=config->scrw;
						EnableGadget(&screenmodegads[SCREENMODE_WIDTH-300],rp,0,0);
					}
					if (screenmodegads[SCREENMODE_DEFHEIGHT-300].Flags&SELECTED) {
						config->scrh=curmode->defh;
						config->scr_winh=config->scrh;
						EnableGadget(&screenmodegads[SCREENMODE_HEIGHT-300],rp,0,0);
					}
					fixmodegads(curmode);
					if (config->screenflags&SCRFLAGS_DEFWIDTH)
						DisableGadget(&screenmodegads[SCREENMODE_WIDTH-300],rp,0,0);
					if (config->screenflags&SCRFLAGS_DEFHEIGHT)
						DisableGadget(&screenmodegads[SCREENMODE_HEIGHT-300],rp,0,0);
					config->screenmode=curmode->mode;

					if (curmode->mode==MODE_PUBLICSCREENUSE /* ||
						curmode->mode==MODE_PUBLICSCREENCLONE */ ) {
						for (a=0;a<80;a++) {
							if (!curmode->name[a]) break;
							if (a>0 && curmode->name[a-1]==':') {
								if ((strcmp(&curmode->name[a],cfg_string[STR_SCREEN_MODE_USE]))==0)
									break;
/*
								if ((strcmp(&curmode->name[a],cfg_string[STR_SCREEN_MODE_CLONE]))==0)
									break;
*/
							}
						}
						if (a>0 && curmode->name[a]) {
							strncpy(config->pubscreen_name,curmode->name,a);
							config->pubscreen_name[a-1]=0;
						}
					}
					else config->pubscreen_name[0]=0;

					if (version2<OSVER_39) {
						if (config->screenmode==MODE_WORKBENCHUSE ||
							curmode->mode==MODE_PUBLICSCREENUSE)
							DisableGadget(&maingad[SCREEN_PALETTE],rp,2,1);
						else EnableGadget(&maingad[SCREEN_PALETTE],rp,2,1);
					}
				}
				else if (view->listid==2) {
					if (view->itemselected>=0 && view->itemselected<13) {
						if (view->itemselected==0)
							load_palette(Screen,palette_backup,1<<screen_depth);
						else if (view->itemselected==1)
							loadrgb4(Screen,def_dopus_palette,1<<screen_depth);
						else if (view->itemselected==3) {
							ULONG *palbuf;
							int wbdepth,size;

							GetWBScreen(&scrbuf);
							wbdepth=1<<scrbuf.RastPort.BitMap->Depth;
							if (wbdepth > 256) wbdepth = 256; /* AROS FIX */
							
							size=(wbdepth*3)*sizeof(ULONG);
							if ((palbuf=AllocMem(size,MEMF_CLEAR))) {
								ULONG palette[48];

								get_palette(&scrbuf,palbuf,wbdepth);
								for (a=0;a<4;a++) {
									palette[(a*3)]=palbuf[(a*3)];
									palette[(a*3)+1]=palbuf[(a*3)+1];
									palette[(a*3)+2]=palbuf[(a*3)+2];
								}
								for (a=4;a<16;a++) {
									palette[(a*3)]=config->new_palette[(a*3)];
									palette[(a*3)+1]=config->new_palette[(a*3)+1];
									palette[(a*3)+2]=config->new_palette[(a*3)+2];
								}
								if (wbdepth>4 && screen_depth>2) {
									int a,endwb,endme;

									endwb=wbdepth-4;
									endme=(1<<screen_depth)-4;

									for (a=0;a<4;a++) {
										palette[((a+endme)*3)]=palbuf[((a+endwb)*3)];
										palette[((a+endme)*3)+1]=palbuf[((a+endwb)*3)+1];
										palette[((a+endme)*3)+2]=palbuf[((a+endwb)*3)+2];
									}
								}
								load_palette(Screen,palette,1<<screen_depth);
								FreeMem(palbuf,size);
							}
						}
						else loadrgb4(Screen,defpalettes[view->itemselected-2],4);
						get_palette(Screen,config->new_palette,1<<screen_depth);
						showcolourvals(realpen,1);
					}
				}
				break;
			}
		}
	}
}

void makescreenedit(mode)
int mode;
{
	int a,b,wclone,shortage,size,fnum,fontnum,nopropnum,only8num,num;
	ULONG modeid;
	char *ptr,buf[256];
	DisplayInfoHandle *handle;
	struct DisplayInfo *dispinfo;
	struct AvailFonts *avail;

	borderkey=NULL;

	SetAPen(rp,screen_pens[1].pen);
	switch (mode) {
		case SCREEN_PALETTE:
			bpg=4;
			if (version2>=OSVER_39) {
	(void)handle;
	(void)dispinfo;
	(void)modeid;
				modeid=GetVPModeID(vp);
				if ((handle=FindDisplayInfo(config->screenmode)) ||
					(handle=FindDisplayInfo(modeid))) {
					GetDisplayInfoData(handle,buf,256,DTAG_DISP,0);
					dispinfo=(struct DisplayInfo *)buf;
					bpg=(dispinfo->RedBits+dispinfo->GreenBits+dispinfo->BlueBits)/3;
				}
			}
			for (a=0;a<3;a++) {
				fix_slider(&palettegads[a]);
				FixSliderBody(Window,&palettegads[a],1<<bpg,1,2);
			}
			AddGadgets(Window,
				palettegads,
				palettegadgets,
				6,
				screen_pens[(int)config->gadgettopcol].pen,screen_pens[(int)config->gadgetbotcol].pen,1);
			do3dbox(x_off+296,y_off+33,220,14);
			do3dbox(x_off+250,y_off+33,40,14);
			setuplist(&palettelist,250,93);
			AddListView(&palettelist,1);
			showpalettegad(1);
			showcolourvals(screen_pens[1].pen,1);
			get_palette(Screen,palette_backup,1<<screen_depth);
			break;

		case SCREEN_COLOURS:
			if (config->stringfgcol==config->stringbgcol) ++config->stringfgcol;
			do3dbox(x_off+142,y_off+61,464,122);
			AddGadgetBorders(&borderkey,
				&coloursgad,
				1,
				screen_pens[(int)config->gadgettopcol].pen,screen_pens[(int)config->gadgetbotcol].pen);
			AddGadgets(Window,
				&coloursgad,
				NULL,
				1,
				screen_pens[(int)config->gadgettopcol].pen,screen_pens[(int)config->gadgetbotcol].pen,1);
			setupcolourbox(rp,x_off+208,y_off+33,config->statusfg,config->statusbg);
			showscreencolours(COLOURS_ALL,NULL,NULL,0);
			showcolourscycle(config->statusfg,config->statusbg,0);
			break;

		case SCREEN_SCREENMODE:
			screenkey=NULL; firstmode=NULL;
			screenmodeview.itemselected=-1; wclone=0;
			busy();
			if ((a=initscreenmodes()) &&
				(screenmodelist=LAllocRemember(&screenkey,(a+1)*4,MEMF_CLEAR))) {
				screenmode=firstmode;
				for (b=0;b<a;b++) {
					if ((screenmodelist[b]=LAllocRemember(&screenkey,DISPLAYNAMELEN,MEMF_CLEAR)))
						strcpy(screenmodelist[b],screenmode->name);
					if (screenmode->mode==MODE_WORKBENCHCLONE) wclone=b;
					if (screenmode->mode==config->screenmode) {
						if (screenmode->mode==MODE_PUBLICSCREENUSE) {
							lsprintf(buf,"%s:%s",config->pubscreen_name,cfg_string[STR_SCREEN_MODE_USE]);
							if (LStrCmpI(buf,screenmode->name)==0) screenmodeview.itemselected=b;
						}
/*
						else if (screenmode->mode==MODE_PUBLICSCREENCLONE) {
							lsprintf(buf,"%s:%s",config->pubscreen_name,cfg_string[STR_SCREEN_MODE_CLONE]);
							if (LStrCmpI(buf,screenmode->name)==0) screenmodeview.itemselected=b;
						}
*/
						else screenmodeview.itemselected=b;
					}
					if (!(screenmode=screenmode->next)) break;
				}
			}
			unbusy();
			if (screenmodeview.itemselected==-1) screenmodeview.itemselected=wclone;
			setuplist(&screenmodeview,238,17);
			screenmodeview.items=screenmodelist;
			screenmodeview.topitem=screenmodeview.itemselected;
			AddListView(&screenmodeview,1);
			screenmodegads[0].Flags&=~GFLG_DISABLED;
			screenmodegads[1].Flags&=~GFLG_DISABLED;
			for (a=SCREENMODE_DEFWIDTH-300;a<SCREENMODE_SLIDER-300;a++)
				screenmodegads[a].Flags&=~(GFLG_SELECTED|GFLG_DISABLED);
			if (config->screenflags&SCRFLAGS_HALFHEIGHT)
				screenmodegads[SCREENMODE_HALFHEIGHT-300].Flags|=GFLG_SELECTED;
			else screenmodegads[SCREENMODE_HALFHEIGHT-300].Flags&=~GFLG_SELECTED;
			fix_slider(&screenmodegads[SCREENMODE_SLIDER-300]);
			AddGadgets(Window,
				screenmodegads,
				screenmodegadgets,
				7,
				screen_pens[(int)config->gadgettopcol].pen,screen_pens[(int)config->gadgetbotcol].pen,1);
			Do3DBox(rp,
				x_off+238,y_off+140,
				272,40,
				screen_pens[(int)config->gadgetbotcol].pen,screen_pens[(int)config->gadgettopcol].pen);
			SetAPen(rp,screen_pens[1].pen);
			UScoreText(rp,
				cfg_string[STR_DISPLAY_MODE_DESCRIPTION],
				x_off+238,y_off+136,
				-1);
			fixmodegads((curmode=showdisplaydesc()));
			fixdefaultgads(curmode);
			break;

		case SCREEN_FONTS:
			if (!fontdatabuf) {
				busy();
				size=(sizeof(struct AvailFonts)*512)+sizeof(struct AvailFontsHeader);
				do {
					if (!(fontdatabuf=LAllocRemember(&fontkey,size,MEMF_CLEAR)))
						break;
					if ((shortage=AvailFonts(fontdatabuf,size,AFF_MEMORY|AFF_DISK))) {
						LFreeRemember(&fontkey);
						size+=shortage;
					}
					else break;
				} while (shortage);
				unbusy();
				if (!fontdatabuf) break;
				num=((struct AvailFontsHeader *)fontdatabuf)->afh_NumEntries;
				if (!num ||
					!(fontlist=LAllocRemember(&fontkey,(num+1)*4,MEMF_CLEAR)) ||
					!(fontsizelist=LAllocRemember(&fontkey,(num+1)*4,MEMF_CLEAR)))
					break;
				if (!(noproplist=LAllocRemember(&fontkey,(num+1)*4,MEMF_CLEAR)) ||
					!(nopropsizelist=LAllocRemember(&fontkey,(num+1)*4,MEMF_CLEAR))) {
					noproplist=fontlist;
					nopropsizelist=fontsizelist;
				}
				if (!(only8list=LAllocRemember(&fontkey,(num+1)*4,MEMF_CLEAR)))
					only8list=fontlist;
				sortfontlist((struct AvailFonts *)&fontdatabuf[sizeof(UWORD)],num,0);
				sortfontlist((struct AvailFonts *)&fontdatabuf[sizeof(UWORD)],num,1);
				avail=(struct AvailFonts *)&fontdatabuf[sizeof(UWORD)];
				for (fnum=0;fnum<num;fnum++) {
					if ((ptr=strstri(avail[fnum].af_Attr.ta_Name,".font"))) *ptr=0;
					avail[fnum].af_Attr.ta_Style=0;
					for (a=0;a<num;a++) {
						if (strcmp((char *)avail[a].af_Attr.ta_Name,(char *)avail[fnum].af_Attr.ta_Name))
							++avail[fnum].af_Attr.ta_Style;
					}
				}
				for (fnum=0,fontnum=0,nopropnum=0,only8num=0;fnum<num;fnum++) {
					for (a=0;a<fontnum;a++) {
						if (strcmp(fontlist[a],(char *)avail[fnum].af_Attr.ta_Name)==0) break;
					}
					if (a==fontnum) {
						fontlist[fontnum]=(char *)avail[fnum].af_Attr.ta_Name;
						fontsizelist[fontnum]=LAllocRemember(&fontkey,(avail[fnum].af_Attr.ta_Style+1)*4,MEMF_CLEAR);
						if (!(avail[fnum].af_Attr.ta_Flags&FPF_PROPORTIONAL)) {
							noproplist[nopropnum]=fontlist[fontnum];
							nopropsizelist[nopropnum]=fontsizelist[fontnum];
							++nopropnum;
							if (avail[fnum].af_Attr.ta_YSize==8) {
								only8list[only8num]=fontlist[fontnum];
								++only8num;
							}
						}
						++fontnum;
					}
					if (fontsizelist[a]) {
						for (b=0;;b++) {
							if (!fontsizelist[a][b]) break;
							if (atoi(fontsizelist[a][b])==avail[fnum].af_Attr.ta_YSize) {
								b=-1;
								break;
							}
						}
						if (b>-1 && (fontsizelist[a][b]=LAllocRemember(&fontkey,8,MEMF_CLEAR)))
							lsprintf(fontsizelist[a][b],"%4ld",avail[fnum].af_Attr.ta_YSize);
					}
				}
			}
			setuplist(&fontplacelist,146,18);
			setuplist(&fontlistview,360,18);
			setuplist(&fontsizelistview,566,18);
			fontplacelist.itemselected=fontplacelist.topitem=0;
			fontsizelistview.items=fontlistview.items=NULL;
			AddListView(&fontplacelist,3);
			SetAPen(rp,screen_pens[1].pen);
			UScoreText(rp,
				cfg_string[STR_DISPLAY_ITEM],
				x_off+146,y_off+14,
				-1);
			UScoreText(rp,
				cfg_string[STR_FONT],
				x_off+360,y_off+14,
				-1);
			UScoreText(rp,
				cfg_string[STR_FONT_SIZE],
				x_off+566,y_off+14,
				-1);
			if (version2) {
				fontsize_buf[0]=0;
				AddGadgets(Window,
					&fontsizegadget,
					NULL,
					1,
					screen_pens[(int)config->gadgettopcol].pen,screen_pens[(int)config->gadgetbotcol].pen,1);
			}
			initfontlist(FONT_CLOCK,1,0);
			strcpy(old_general_font,config->fontbufs[0]);
			break;

		case SCREEN_GENERAL:
			inittickgads(scr_generalgadgets,config->generalscreenflags,0);
			break;

		case SCREEN_ARROWS:
			for (a=0;a<3;a++) {
				arrowgadgets[a].GadgetRender=(APTR)buttonoffimage;
				arrowgadgets[a].SelectRender=(APTR)buttononimage;
				do3dbox(x_off+184+(a*129),y_off+33,121,55);
			}

			AddGadgetBorders(&borderkey,
				&arrowgadgets[3],
				2,
				screen_pens[(int)config->gadgettopcol].pen,screen_pens[(int)config->gadgetbotcol].pen);
			AddGadgetBorders(&borderkey,
				&arrowgadgets[5],
				2,
				screen_pens[(int)config->gadgettopcol].pen,screen_pens[(int)config->gadgetbotcol].pen);
			AddGadgetBorders(&borderkey,
				&arrowgadgets[7],
				1,
				screen_pens[(int)config->gadgettopcol].pen,screen_pens[(int)config->gadgetbotcol].pen);

			AddGadgets(Window,
				arrowgadgets,
				arrowgadtxt,
				8,
				screen_pens[(int)config->gadgettopcol].pen,screen_pens[(int)config->gadgetbotcol].pen,1);
			for (a=0;a<2;a++) {
				DoArrow(rp,x_off+420+(a*34),y_off+144,10,9,screen_pens[1].pen,screen_pens[0].pen,3-a);
				DoArrow(rp,x_off+436,y_off+138+(a*12),12,9,screen_pens[1].pen,screen_pens[0].pen,a);
			}
			showupdownslider();
			showleftrightslider();
			showstringgadget();
			showarrowborders(0,-1);
			fixarrowgads(0);
			SetAPen(rp,screen_pens[1].pen);
			UScoreText(rp,
				cfg_string[STR_MODIFY_SIZE],
				x_off+294,y_off+151,
				-1);
			DoCycleGadget(&arrowgadgets[7],Window,arrowtypetxt,0);
			break;

		case SCREEN_SLIDERS:
			for (a=0;a<2;a++) {
				AddGadgetBorders(&borderkey,
					&screen_sliders_gadgets[a],
					2,
					screen_pens[(int)config->gadgettopcol].pen,screen_pens[(int)config->gadgetbotcol].pen);
			}

			AddGadgets(Window,
				screen_sliders_gadgets,
				NULL,
				2,
				screen_pens[(int)config->gadgettopcol].pen,screen_pens[(int)config->gadgetbotcol].pen,1);

			for (a=0;a<2;a++) show_slider_setup(182+(a*191),32,a);
			break;
	}
}

void removescreenedit(mode)
int mode;
{
	SetAPen(rp,screen_pens[0].pen);
	RectFill(rp,
		x_off+120,y_off+4,
		x_bot,y_bot);
	SetAPen(rp,screen_pens[1].pen);
	switch (mode) {
		case SCREEN_PALETTE:
			RemoveGList(Window,palettegads,6);
			RemoveListView(&palettelist,1);
			break;

		case SCREEN_COLOURS:
			RemoveGList(Window,&coloursgad,1);
			doradiobuttons();
			break;

		case SCREEN_SCREENMODE:
			RemoveListView(&screenmodeview,1);
			RemoveGList(Window,screenmodegads,7);
			LFreeRemember(&screenkey);
			if (config->scrdepth!=screen_depth) {
				close_screen();
				open_screen();
				showconfigscreen(CFG_SCREEN);
			}
			break;

		case SCREEN_FONTS:
			RemoveListView(&fontplacelist,3);
			if (version2) RemoveGList(Window,&fontsizegadget,1);
			if (LStrCmpI(config->fontbufs[0],old_general_font)!=0) {
				close_screen();
				open_screen();
				showconfigscreen(CFG_SCREEN);
			}
			break;

		case SCREEN_ARROWS:
			RemoveGList(Window,arrowgadgets,8);
			break;

		case SCREEN_GENERAL:
			removetickgads();
			break;

		case SCREEN_SLIDERS:
			RemoveGList(Window,screen_sliders_gadgets,2);
			break;
	}
	LFreeRemember(&borderkey);
}
