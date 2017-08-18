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

//void KPrintF __ARGS((char *,...));

/* main.c */

int main(int,char **);
void quit(void);
void getconfig(void);
void giveconfig(void);
void readconfig(void);
void configmsg(int);
void fixcstuff(struct ConfigStuff *);
void cstufffix(struct ConfigStuff *);
void cleanconfigscreen(void);
void showconfigscreen(int);
void initsidegads(char **,int,int);
void inittickgads(struct ConfigGadget *,int,int);
void removetickgads(void);
int processtickgad(struct ConfigGadget *,int,int,int);
struct ConfigUndo *makeundo(int);
void copygadgets(struct dopusgadgetbanks *,struct dopusgadgetbanks *,struct DOpusRemember **);
void copyfiletypes(struct dopusfiletype *,struct dopusfiletype **,struct DOpusRemember **);
void doundo(struct ConfigUndo *,int);
void freegadgets(struct dopusgadgetbanks *);
void freegadgetbank(struct dopusgadgetbanks *,int);
void freefunction(struct dopusfunction *);
void freenewfunction(struct newdopusfunction *);
char *getcopy(char *,int,struct DOpusRemember **);
void makestring(char *buf,...);
int getstring(char *,char *,int,int);
int request(char *);
int do_request(char *,char *,char *);
void doglassimage(struct Gadget *);
void freestring(char *);
void doscreentitle(char *);
struct IntuiMessage *getintuimsg(void);
struct TextFont *getfont(char *,int *,int);
void load_palette(struct Screen *,ULONG *,int);
void get_palette(struct Screen *,ULONG *,int);
void open_screen(void);
void close_screen(void);
void get_colour_table(void);
void free_colour_table(struct ColorMap *);
void loadrgb4(struct Screen *,UWORD *,int);
unsigned char getkeyshortcut(const char *str);

/* main1.c */

int dooperationconfig(void);
int dosystemconfig(void);
void getnextgadget(struct Gadget *);
int fixformlist(int);
void fixdisplaylengths(int);
int system_requester(char *,char *,struct Gadget *,int);
void system_makeiconlist(int);
void system_showicon(char *,int);
void get_language_list(struct DOpusRemember **);
void quicksort(char **,int,int);
void sortswap(char **,int,int);

/* main2.c */

int dogadgetconfig(void);
void showgadgets(struct dopusgadgetbanks *,int);
void showgadgetname(struct newdopusfunction *,struct Gadget *);
int makenewbank(int);
struct dopusgadgetbanks *lastbank(struct dopusgadgetbanks *);
void dogadgetinfo(char *);
int domenuconfig(void);
void showmenus(int);
void showmenuname(struct newdopusfunction *,struct Gadget *);
void showmenutext(char *,int,int,int,int);
void clearmenu(struct newdopusfunction *);
int doinitmenutext(int);
void sortmenustrip(int);
void do_menuscreen_title(int);

/* main3.c */

int editfunction(struct dopusfunction *func,int,APTR);
void setuplist(struct DOpusListView *,int,int);
void makefunclist(char *,char **,char *);
void makedispfunc(char *,char,char *);
char *compilefunclist(char **,char *,struct DOpusRemember **);
void freefunclist(char **);
void docolourbox(struct RastPort *,int,int,int,int);
void docoloursel(struct RastPort *,int,int,int);
void drawcompbox(struct RastPort *,int,int,int,int);
void showkey(UWORD,UWORD,int,int,int,int);
void showfuncob(struct RastPort *,char *,int,int,int,int,int);
void makeselflags(int,char *);
int getselflags(char *);
void checkmutflags(char *,int);
void editgadsoff(void);
void editgadson(int);
void deleteentry(char **,char *,char **,int);
void endedit(void);
void setupforedit(int,int);
void checkswapgad(void);
int funcrequester(int,char *,char *);
void makenewentry(char **,char *,char **,int);
void setupcolourbox(struct RastPort *,int,int,int,int);
void getfiletypefunc(struct dopusfunction *,struct dopusfiletype *,int);
void makefiletypefunc(struct dopusfunction *,struct dopusfiletype *,int);
void fixeditfunction(struct dopusfunction *,char *,char **,char *,int,int,APTR,int);
void setupeditdisplay(struct dopusfunction *,int,APTR,int,char **,char **,char *,char *,int);
void erasefunction(struct dopusfunction *,char **,char **,char *);
int fixfunctypelist(struct dopusfiletype *);
void add_appobject(int);
void rem_appobject(int);
void do_gad_label(char *,int,int);

/* main4.c */

int dodriveconfig(void);
void showdrives(void);
void showdrivename(struct dopusfunction *,struct Gadget *);
int doinitdrivetext(int);
int docolourgad(struct RastPort *,int,int,int,int,char *,int *,int *,int);
void driveeditoff(void);
void deletedrive(struct dopusfunction *);
void sortdrivebank(int);

/* main5.c */

int dofiletypeconfig(void);
char **makefiletypelist(struct DOpusRemember **);
void showtypelist(char **);
int doinitfiletypetext(int);
void filetypetitle(struct dopusfiletype *);
int editfiletype(struct dopusfiletype *,struct DOpusRemember **,int);
int editclass(struct fileclass *,int);
void addfiletype(struct dopusfiletype *);
void readfileclasses(void);
int importfileclasses(void);
int savefileclasses(void);
int addfileclass(char *,char *,char *);
void freefileclasses(void);
void removefileclass(struct fileclass *);
char **makeclasslist(struct DOpusRemember **);
int readline(char *,int,char *,int);
int editfileclass(struct fileclass *,int);
void makeeditclasslist(struct fileclass *,char **,char *);
void dispclasslist(char **,char *,char **);
void seteditclassgads(int);
void removeclassentry(char **,char *,int);
void insertnewclass(char **,char *,int,char **,char *,int);
void endclassedit(int,char **,char *,char **);
void makeclassrecog(struct fileclass *,unsigned char **,unsigned char *);
void checkclassswap(void);
void showclassop(int);
struct fileclass *getfileclasslist(int);
void draw_file_view(void);
void free_file_view(void);
void load_file_view(void);
void file_view_text(char *,int);
void show_file_view(void);
void show_view_number(int,int);
void cursor_fileview(int);

/* screen.c */

int doscreenconfig(void);
void makescreenedit(int);
void removescreenedit(int);

/* screen_colors.c */

void showcolourvals(int,int);
void updatecolourvals(int,int);
void showscreencolours(int,char **,char **,int);
void solidblock(struct RastPort *,char *,int,int,int,int,int,int,int);
void showcolourscycle(int,int,int);

/* screen_screenmode.c */

int initscreenmodes(void);
int addscreenmode(char *,UWORD,UWORD,UWORD,UWORD,UWORD,UWORD,UWORD,ULONG);
struct ScreenMode *showdisplaydesc(void);
void fixmodegads(struct ScreenMode *);
void fixdefaultgads(struct ScreenMode *);
void sortscreenmodes(int,int);
struct ScreenMode *getscreenmode(int);

/* screen_fonts.c */

int initfontlist(int,int,int);
void sortfontlist(struct AvailFonts *,int,int);
void dofontdemo(char *,int);

/* screen_palette.c */

void showpalettegad(int);
void scalecol(ULONG *);
void scalecolup(ULONG *);

/* screen_arrows.c */

void showupdownslider(void);
void showleftrightslider(void);
void showstringgadget(void);
void showarrowborders(int,int);
void drawarrowrec(int);
void fixarrowgads(int);

/* screen_sliders.c */

void show_slider_setup(int,int,int);

/* main7.c */

void busy(void);
void unbusy(void);
void dosave(int);
int doload(int,int);
void copyconfigonly(struct Config *,struct Config *);
int dolistwindow(char *,int,int,char **,int,char *,int *);
void copytoclip(struct dopusfunction *,char **,char *,char *);
int pasteclip(struct dopusfunction *,char **,char *,char **,char *);
void makefilebuffer(char *);
char *strstri(char *,char *);

/* main8.c */

int dohotkeysconfig(void);
void makehotkeylist(void);
int doedithotkey(struct dopushotkey *);
void loadpalette(UWORD *,ULONG *,int);
void loadrgbtable(struct ViewPort *,ULONG *,int,int);
void drawcornerimage(struct RastPort *,int,int,int,int);
int fixrmbimage(struct RMBGadget *,struct Gadget *,int,int,int);
void doradiobuttons(void);
void fix_gadget_border(struct Gadget *);
void select_gadget(struct Gadget *,int);
void fix_gadget_positions(void);
void fix_gad_pos(struct Gadget *,short *);

/* main9.c */

void readhelp(void);
void makehelpname(char *);
void doconfighelp(void);
void dohelpmsg(char *);
void load_clips(void);
void save_clips(void);
int readfile(char *,char **,int *);
void init_strings(void);
void setup_list_window(struct NewWindow *,struct DOpusListView *,struct Gadget *,int);

/* lsprintf.asm */

void SwapMem(register char *src __asm("a0"),register char *dst __asm("a1"),register int size __asm("d0"));
int lsprintf (char *,const char *,...);

/* strings.c */

void read_strings(void);

/* paint_mode.c */

int get_paint_colours(int *,int *,int);
void do3dbox(int,int,int,int);
void fix_slider(struct Gadget *);
struct Window *openwindow(struct NewWindow *);
