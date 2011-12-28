/*------------------------------------------------------------------------
  Function protos
------------------------------------------------------------------------*/

struct MUI_CustomClass *HTMLGadClInit(void);
struct MUI_CustomClass *NewGroupClInit(void);

Class *BoingTransferClInit(void);
Class *SpinnerClInit(void);

BOOL HTMLGadClFree(struct MUI_CustomClass *);
BOOL NewGroupClFree(struct MUI_CustomClass *);
BOOL BoingTransferClFree(Class *);
BOOL SpinnerClFree(Class *);


/* main.c */

int main(int, char **);
int get_filename(char **);
void setup_win(mo_window *);
char *gui_whichscreen(void);
int LoadInlinedImage(char *img);
int LoadInlinedImages(void);
void ResetAmigaGadgets(void);

/* mui.c */

void mui_init(void);
void fail(APTR, char *);
void init(void);
void SetImageButtons(long);
void SetSearchIndex(long);

/* HTMLfonts.c */

void open_fonts(void);
void close_fonts(void);


/* gui.c */

void amiga_load_prefs(void);

/* rexx.c */
ULONG RexxLaunch(int, mo_window *) ;
void RexxDispatch(void) ;
void RexxQuit(void) ;
void RexxMenu(int, char *) ;

/* to_html.c */

ULONG TO_HTMLGetScreenDepth(void);
void TO_HTML_reset_search(mo_window *win);
char **TO_HTMLGetImageSrcs (mo_window *win,int *num);
void TO_HTML_SetSearch(void);
void TO_HTML_reset_search(mo_window *win);
void TO_HTML_FlushImageCache(void);
void TO_HTML_LoadInlinedImage(void);
void *TO_HTMLMakeSpaceForElementRef(void);
ULONG TO_HTMLGetScreenDepth(void);
struct mark_up *TO_HTML_GetHTMLObjects(void);

/* HTMLimages.c */
Pixmap CreateBitMapFromXBMData(UBYTE *bits, int width, int height, int depth);

/* picread */

unsigned char *ReadXbmBitmap(FILE *fp, char *datafile, int *w, int *h,
			     int *colrs);

/* html.c */
void ViewRedisplay(APTR, int, int, int, int);

/* HTMLParse.c */
struct mark_up *HTMLParse(struct mark_up *old_list, char *str);
char *ParseMarkTag(char *text, char *mtext, char *mtag);

/* HTMLAmiga.c */
WidgetInfo *
MakeWidget(HTMLGadClData *HTML_Data, char *edata,
				long x,long y,long WidgetId,FormInfo *Form);
void AddNewForm(HTMLGadClData *,FormInfo *);
void DisposeForms(FormInfo *Form);

/* HTMLformat.c */
void ReformatWindow(HTMLGadClData *);

/* misc */
//void kprintf(char *,...);
