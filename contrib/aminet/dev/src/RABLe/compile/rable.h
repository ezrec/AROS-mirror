/* function prototypes */

void setup_display(void);
void process_events(void);
BOOL handleIDCMP(struct Window *);
void InitLibs(void);
BOOL setup_menu(void);
void about_window(void);
struct ScreenModeRequester *openASL(void);
void LoadPic(char *filename);
void SavePic(char *filename);
void clean_exit(TEXT *str, int rcode);
void decode_gadget(struct Gadget *, struct IntuiMessage *);
void fill_handler(struct Gadget *);
void tool_handler(struct Gadget *);
void palette_handler(struct Gadget *);
BOOL menu_handler(UWORD, BOOL);
struct Gadget *get_gadgets(void);
struct Gadget *get_pal(void);

void canvas_size(void);
void resize_handler (void);
void slider_handler (void);
void slider_refresh (void);



