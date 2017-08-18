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

#include <rexx/storage.h>

/* assembly.c */

#if defined(__PPC__) || defined(__AROS__)
#define lsprintf sprintf
#else
void lsprintf (char *,char *,...);
#endif
/* void Keyhandler(void); */
int countlines(struct ViewData *);
int smartcountlines(struct ViewData *);
int ansicountlines(struct ViewData *);
void removetabs(struct ViewData *);
int getusage(void);
void dprintf (char *,char *,...);
int filteroff(void);
void filteron(void);

/* about.c */

void about(void);
void give_version_info(void);

/* arbiter.c */

int install_arbiter(void);
void remove_arbiter(void);
int arbiter_command(int,APTR,int);
void __saveds arbiter_process(void);
struct Screen *open_subprocess_screen(char *,struct TextFont *,struct DOpusRemember **,short *);

/* archive.c */

int readarchive(struct DirectoryWindow *,int);
void freearchive(struct DirectoryWindow *);
BOOL unarcfiledir(const struct DirectoryWindow *, const char *, char *, const char *);
BOOL getsourcefromarc(struct DirectoryWindow *, char *, char *);
void arcfillfib(struct FileInfoBlock *, struct Directory *);
void removetemparcfile(const char *);

/* buffers.c */

void allocdirbuffers(int);
int bringinbuffer(char *,int,int);
void findemptybuffer(int);
void startgetdir(int,int);
void incrementbuf(int,int,int);
void copydirwin(struct DirectoryWindow *,struct DirectoryWindow *,int);
void swapdirwin(void);
void advancebuf(int,int);
void clearbuffers(void);
struct DirectoryWindow *findbuffer(char *,int,int,int);
void renamebuffers(char *,char *);
int replacepart(char *,char *,char *);
void dolistbuffers(int);
int checkcurrentbuffer(int);
void userentrymessage(struct DirectoryWindow *,struct Directory *,int);
void makespecialdir(int,char *);
void check_old_buffer(int);
void refreshwindow(int,int);
void go_to_buffer(int,struct DirectoryWindow *);

/* 
main.c */

int main(int,char **);
int SetUp(int);
void setupdisplay(int);
void drawscreen(void);
struct TextFont *getfont(char *,int *,int);
void allocstrings(void);
char *astring(int);
void allocdragbuffers(void);
void freedragbuffers(void);
void load_palette(struct Screen *,ULONG *);
void get_palette(struct Screen *,ULONG *);
struct Library *open_dopus_library(char *,int);
void read_configuration(int);
void read_data_files(int);
int get_data_file(char *,char *,int);
void get_config_file(char *,char *);
void setup_draw_info(void);
struct Library *OpenXFDlib(void);
struct Library *OpenXADlib(void);

/* main1.c */

int readsetup(char *);
int savesetup(char *);
void getdefaultconfig(void);
void fixcstuff(struct ConfigStuff *);
void cstufffix(struct ConfigStuff *);

/* main2.c */

void freedir(struct DirectoryWindow *,int);
int getdir(struct DirectoryWindow *,int,int);
void displaydir(int);
void fixprop(int);
void fixvertprop(int);
void fixhorizprop(int);
int doposprop(int);
void doposhprop(int);
void checkdir(char *,struct Gadget *);
void verticalscroll(int,int);
void horizontalscroll(int,int);
void findfirstsel(int,int);
void findfirstchar(int,char);
void doinfodisplay(struct Directory *,int);
void nodayseedate(struct DateStamp *,char *);
void display_entry(struct Directory *,int,int,int);
void entry_text(int,struct Directory *,char *,int,int,int);
void builddisplaystring(struct Directory *,char *,int);
void setdispcol(struct Directory *,int);
void displaydirgiven(int,struct Directory *,char);
void endfollow(int);
void getprotdatelengths(struct RastPort *);
void setsizedisplen(struct RastPort *);
void buildkmgstring(char *,unsigned long long,int);

/* main3.c */

int doparent(char *);
int doroot(char *);
struct Directory *addfile(struct DirectoryWindow *,int,char *,long long,int,
          struct DateStamp *,char *,int,int,int,char *,struct Directory *,UWORD,UWORD);
int namesort(char *,char *);
int inter_sort(char *,char *);
char *getstrafternum(char *);
void removefile(struct Directory *,struct DirectoryWindow *,int,int);
void sortdir(struct DirectoryWindow *, int);
int dorun(char *,int,int);
void busy(void);
void unbusy(void);
void setnullpointer(struct Window *);
void free_file_memory(struct Directory *);

/* main4.c */

void makedir(int);
int iconwrite(int,char *);
int copyicon(char *,char *,int *);
char *isicon(char *);
char *getarexxpath(int,int,int,int);
int readfile(char *,char **,int *);
int getnewprot(int,int,int);
int dateformat(int);
int checkscreenmode(int);
void doarrowgadgets(struct Gadget *,int);
void makermbgad(struct RMBGadget *,struct Gadget *,int);
void dolittlegads(struct Gadget *,char *,int);
void rectfill(struct RastPort *,int,int,int,int);
int isvalidwindow(int);
int _isdigit(unsigned char);
int _isxdigit(unsigned char);
int _isprint(unsigned char);
int _isspace(unsigned char);
int _isupper(unsigned char);
int _ispunct(unsigned char);

/* main5.c */

int copyfile(char *,char *,int *,/*int,*/char *,int);
struct Directory *checktot(struct DirectoryWindow *);
struct Directory *checkdirtot(struct DirectoryWindow *);
struct Directory *checkdevtot(struct DirectoryWindow *);
struct Directory *checkalltot(struct DirectoryWindow *);
struct Directory *findfile(struct DirectoryWindow *,char *,int *);
int delfile(char *,char *,char *,int,int);
int getwildrename(char *,char *,char *,char *);
void filloutcopydata(struct Directory *);
void filloutcopydatafile(char *);
void update_buffer_stamp(int,int);
int check_key_press(struct dopusfunction *,UWORD,UWORD);

/* main6.c */

void get_printdir_data(struct PrintDirData *);
void startnotify(int);
void endnotify(int);
void startnotifies(void);
void endnotifies(void);
int dormbgadget(int,int,struct newdopusfunction *,struct newdopusfunction *);
int gadgetfrompos(int,int);
int isvalidgad(struct newdopusfunction *);
int getpal(void);
void quickfixmenus(void);
int getdummypath(char *,int);
char *getfiledescription(char *,int);
void fixhlen(int);
char *get_our_pubscreen(void);
void change_port_name(char *);

/* main7.c */

int showpic(char *,int);
int readicon(char *,int);
void drawrecaround(struct RastPort *,/*int,int,*/int,int,int,int,int,int);
int doplay8svx(char *,int);
void kill8svx(void);
void handle8svxerror(int);
void dosound(int);
int playmod(char *);
char DUnpack(char *,int,char *,char);
int check_is_module(char *);
int EnvoyPacket(char *,ULONG,ULONG,UWORD,APTR);

/* main8.c */

int checkexec(char *);
void newcli(char *);
void getprot(int,char *);
int getprotval(char *);
int checkexistreplace(char *,char *,struct DateStamp *,int,int);
int lockandexamine(char *,struct FileInfoBlock *);
void layoutcenter(int);
void fix_rmb_gadgets(void);
void dosizedirwindows(int);
void sizedirwindow_gfx(int);
void fix_requester(struct RequesterBase *,char *);
void fix_stringex(struct StringExtend *);
struct Gadget *addreqgadgets(struct RequesterBase *,struct TagItem **,int,int *);
void set_reqobject_text(struct TagItem *,char *);
void set_reqobject(struct TagItem *,ULONG,ULONG);
void FadeRGB32(struct Screen *,ULONG *,int,int,int);
void FadeRGB4(struct Screen *,UWORD *,int,int,int);
void removewindowgadgets(struct Window *);

/* main9.c */

#ifdef __SASC__
bigint bytes(char *,bigint *,int *);
#else
long long bytes(char *,long long *,int *);
#endif
void get_colour_table(void);
void free_colour_table(void);
void SetDrawModes(struct RastPort *,UBYTE,UBYTE,UBYTE);
void do3dbox(struct RastPort *,int,int,int,int);

/* main10.c */

void initdatetime(struct DOpusDateTime *,char *,char *,int);
void doarexx(int);
void setcurdir(int);
void dodevicelist(int);
int huntfile(char *,char *,int *);
void getcurdir(char *);
void checksize(int);
void centerwindow(struct NewWindow *);
char *parsedatetime(char *,char *,char *,int *);
void copy_datestamp(struct DateStamp *,struct DateStamp *);
//void readkeys(APTR);
ULONG clone_screen(struct Screen *,struct ExtNewScreen *);
int copy_string(char *,char **,struct DOpusRemember **);
char *strstri(char *,char *);
struct MsgPort *CreateUniquePort(char *,char *,int *);
int identify_and_load(int,int);

/* main11.c */

void iconify(int,int,int);
void remiclock(void);
int getmaxmem(ULONG/*,ULONG*/);
void iconstatustext(char *,int);
void cleanupiconify(void);

/* main12.c */

void setupchangestate(void);
void dodiskinfo(char *);
void get_device_task(BPTR,char *,struct MsgPort *);
#ifdef __SASC__
void getsizestring(char *,bigint);
#else
void getsizestring(char *,unsigned long long);
#endif
void getfloatstr(double,char *);

/* main13.c */

void seedate(struct DateStamp *,char *,int);
int setdate(char *,struct DateStamp *);
void seename(int);
void displayname(int,int);
void relabel_disk(int,char *);
int getroot(char *,struct DateStamp *);
BPTR getrootlock(BPTR);
void strtostamp(char *,char *,struct DateStamp *);

/* main14.c */

void doassign(int);
int checkdest(int);
int checksame(char *,char *,int);
int expand_path(char *,char *);

/* main15.c */

void ftype_doubleclick(char *,char *,int);
int filesearch(char *,int *,int);
int checkisfont(char *,char *);
void initclock(void);
int internal_function(int,int,char *,char *);
void do_parent_root(int);
int checklastsaved(void);
int checkdefaults(void);
int checknewconfig(void);
void dragcompgad(int,int);

/* main16.c */

int showfont(char *,int,int);
int setupfontdisplay(int,UWORD *);
void cleanup_fontdisplay(void);
void readhelp(char *);
void doreadhelp(char *);
void dohelp(char *,char *,int,int,char *);
void checkstringgads(int);
void setdirsize(struct Directory *,long long,int);

/* main17.c */

int rexxdisp(struct RexxMsg *,struct CommandList *,char *);
int parse(unsigned char *);
void changebuffer(int);
char *dosstatus(int,char *,char *);
void dopustofront(void);
void dopustoback(void);
int checkkeyword(char **,int,int);
void removeargstring(int);
void modify(unsigned char *);
void rexx_return(struct RexxMsg *,long long);

/* main18.c */

int recursedir(char *,char *,int,int);
int addrecurse(struct DOpusRemember **,char *,char *,int,APTR,APTR,BPTR,struct FileInfoBlock *,struct Directory *,struct DirectoryWindow *);
int getdircontentsinfo(char *, unsigned long long *, ULONG *);

/* main19.c */

int checkshift(void);
void makereselect(struct DirWindowPars *,int);
void doreselect(struct DirWindowPars *,int);
void shutthingsdown(int);
void setupwindreq(struct Window *);
void hilite_req_gadget(struct Window *,UWORD);
int simplerequest (char *,...);
int whatsit(char *,int,char *,char *);
struct dopusfiletype *checkfiletype(char *,int,int);
int checkfiletypefunc(char *,int);
int dochecktype(struct dopusfiletype *,char *,BPTR,struct FileInfoBlock *);
int checktypechars(int,unsigned char *,int);
int typesearch(int,char *,int,char *,int);
int dorequest(struct DOpusSimpleRequest *,char *,char **,int *,struct Window *);
int searchbuffer(char *,int,char *,int,int);

/* main20.c */

void quit(void);
void remclock(void);
void removehotkeys(void);
void freefont(int);
void freehelp(void);
void freedynamiccfg(void);
int checkwindowquit(void);
void dotaskmsg(struct MsgPort *,int,int,int,char *,int);
void closedisplay(void);
void testcommand(void);

/* main21.c */

void fixdrivegadgets(void);
void nextdrives(void);
void setupgadgets(void);
void allocate_borders(void);
void add_gadget_borders(struct DOpusRemember **,struct Gadget *,int);
void create_gadget_borders(struct DOpusRemember **,int,int,struct Border **,struct Border **,int);
void fill_out_border_data(short *,short *,int,int);
void fill_out_border(struct Border *,int,int,short *,short *);
void drawgadgets(int,int);
void fix_gadget_highlight(struct newdopusfunction *,struct Gadget *,int);
void init_menus(void);
void layout_menus(void);
int maxgadwidth(struct TextFont *,char **,int);
int gettextlength(struct TextFont *,char *,int *,int);
int dotextlength(struct RastPort *,char *,int *,int);
void doposdriveprop(void);
int getgadbankcount(void);
void doposgadgetprop(int);
void fixgadgetprop(void);
int makeusstring(char *,char *,int *);
void drawgadgetcorner(struct RastPort *,int,int);
void drawcornerimage(struct RastPort *,int,int,int,int,int);
void get_bar_item(struct MenuItem *,struct MenuItem *,struct Image *);
struct Image *get_bar_image(int,int,int);

/* main22.c */

int dofilefunction(int,int,char *,char *,int,int,int);

/* custom.c */

void defaultpar(struct dopusfuncpar *);
void dofunctionstring(char *,char *,char *,struct dopusfuncpar *);
int handlefunctionss(char *,char *,char *,struct dopusfuncpar *,struct function_data *);
int getfunction(char *,char **);
int customthing(char *,char *,char *,struct dopusfuncpar *,struct function_data *);
int buildcustfunc(unsigned char *,int,char *,int *,int,int,int,struct function_data *);
int addfilename(char *,char *,char *,int);
void parserunline(char *,unsigned char *);
void custunselect(struct Directory *,int,struct function_data *);
void doreloadfiles(struct function_data *);
struct Directory *reload_file(int,char *);
int openscriptfile(struct dopusfuncpar *,struct function_data *);
int closescriptfile(struct dopusfuncpar *,int,struct function_data *);
int getdummyfile(struct Directory *,char *,struct DOpusFileReq *);
int filloutdummy(char *,struct Directory *);
int dirrequester(struct DOpusFileReq *,char *,char *);
void do_title_string(char *,char *,int,char *);
void addreqfilename(char *,int);
struct Directory *custgetfirst(struct function_data *);
void custnextsel(struct Directory **);
int check_dest_path(struct function_data *);
void build_default_string(char *,char *,char *,char *,char *);

/* cycling.c */

void __saveds colourcycle(void);
int initcycle(struct ViewPort *,UWORD *,int,struct C_Range *,int);
void stopcycle(void);
void togglecycle(void);
int checkcycling(void);

/* doerror.c */

int doerror(int);
void geterrorstring(char *,int);
void dostatustext(char *);
void okay(void);
void myabort(void);
void dofilename(char *);
void geterrorhelp(int);
int checkerror(char *,char *,int);

/* doidcmp.c */

void doidcmp(void);
struct IntuiMessage *getintuimsg(void);
void flushidcmp(void);
int isinwindow(int,int);

/* iffload.c */

int LoadPic(char *);
int checkiff(void);
void rletobuffer(unsigned char *,int,int,struct BitMap *,char,char);
void readpic(struct BitMapHeader *,unsigned char *,struct BitMap *);
void decoderle(struct RLEinfo *);
void doanimframe(void);
void doriff(unsigned char *,struct BitMap *,int,int,int);
void doriff7(unsigned char *,struct BitMap *,int,int);
void decode_riff_xor(unsigned char *,char *,int,int);
void decode_riff_set(unsigned char *,char *,int,int);
void decode_riff_short(UWORD *,unsigned char *,int,int);
void decode_riff_long(ULONG *,unsigned char *,int,int);
void decode_riff7_short(UWORD *,unsigned char *,unsigned char *,int,int);
void decode_riff7_long(ULONG *,unsigned char *,unsigned char *,int,int);
int WaitForMouseClick(int,struct Window *);
int chunkread(void *,ULONG);
void getcolstring(char *);
void gfxprint(struct Window *,struct RastPort *,int,int,int,int,int);
int InitDHIRES(int);
void getviewmodes(char *);
int initsham(void);
void iffinfotxt(struct RastPort *,char *,int,int);
void build_palettes(unsigned char *,int,UWORD *,ULONG *);

/* rexx.c */

void rexx_dispatch(int);
struct RexxMsg *send_rexx_command(char *,int (*)(),struct RexxMsg *);
void rexx_msg_reply(struct RexxMsg *);
struct RexxMsg *rexx_command(char *,struct RexxMsg *);
void rexx_set_return(struct RexxMsg *,int,char *);
void close_rexx_port(void);

/* protectdata.c */

int getprotectdata(int *,int *);
void do_prot_display(int,Object_Border *,struct Gadget *);

/* renamedata.c */

int getrenamedata(char *,char *);

/* select.c */

void doselection(int,int);
void dormbscroll(int);
int select(int,int);
int unselect(int,struct Directory *);
void defselect(int,int,int);
void globalselect(int,int);
void globaltoggle(int);
void doselect(int);
void wildselect(char *,int,int,int);
void dateselect(struct DateStamp *,struct DateStamp *,int,int);
void getprotselvals(char *,int *);
void getseldatestamps(char *,struct DateStamp *,struct DateStamp *);
void protselect(int,int,int,int);
void wildselectthisone(struct Directory *,int,int);
void doselinfo(int);
void updateselectinfo(struct Directory *,int,int);
int makeactive(int,int);
int doactive(int,int);
void unbyte(int);
void checkselection(struct Directory *);

/* selectdata.c */

int getselectdata(char **,int *);
void makemultigad(struct Gadget *,char **,int);
void makeseldobuffer(char **);
void centergadtext(struct Window *,struct Gadget *,char *);

/* tasks.c */

void __saveds hotkeytaskcode(void);
void add_hotkey_objects(CxObj *,struct MsgPort *,int);
CxObj *set_dopus_filter(CxObj *,struct MsgPort *,char *,UWORD,UWORD,int,int);
void set_hotkey(CxObj *,UWORD,UWORD);
void dummy_idcmp(struct MsgPort *,ULONG,UWORD,APTR,int,int);
void __saveds clocktask(void);
#ifdef __AROS__
AROS_UFP2(struct InputEvent *, keyhandler,
    AROS_UFPA(struct InputEvent *, oldevent, A0),
    AROS_UFPA(APTR, userdata, A1)
);
#elif defined(__MORPHOS__)
struct InputEvent * keyhandler(void);
#else
struct InputEvent *__saveds keyhandler(register struct InputEvent * __asm("a0"),register APTR __asm("a1"));
#endif
void openprogresswindow(char *,int,int,int);
void progresstext(int,int,int,char *);

/* view.c */

int viewfile(char *,char *,int,char *,struct ViewData *,int,int);
void cleanupviewfile(struct ViewData *);


/* launchexternal.c */

int start_external(struct dopus_func_start *);
int close_external(struct dopus_func_start *,int);
int set_wb_arg(struct WBArg *,char *,BPTR,int);
void doconfig(void);
void dopus_diskop(int,int,int);
void __saveds launch_diskop(void);
void dopus_print(int,struct DOpusArgsList *,int,char *,struct ViewData *);
int dopus_iconinfo(char *);
void setup_externals(void);
void fill_out_visinfo(struct VisInfo *,struct Screen *);

/* localefunc.c */

void readstrings(char *);
int getkeyshortcut(const char *);

/* searchdata.c */

int get_search_data(char *,int *,struct Window *,struct TextFont *);

/* complete.c */

void do_path_completion(int,UWORD);

/* remember.c */

void do_remember_config(struct RememberData *);
void do_restore_config(struct RememberData *);

/* parentmulti.c */

int do_parent_multi(char *);
int get_multi_volume(BPTR,char **,struct DOpusRemember **);

/* dirtree.c */

void dotree(int);
int build_tree(struct RecursiveDirectory *);
void draw_dirtree_gfx(struct RastPort *,int,int,int);

/* popupmenu.c */

void handlelistermenu(int);
void initlistermenu(void);

/* makelinkdata.c */

int makelink(int);

