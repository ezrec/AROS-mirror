/* Prototypes for functions defined in
filexwindow.c
 */

struct WindowInfo;
struct WindowData;

extern struct MsgPort * GadgetReplyPort;

void DoGadgetReplyPortMsg(void);

BOOL InitReplyPort(void);

void FreeReplyPort(void);

void NewCalcCleanup(struct WindowInfo * );

struct WindowInfo * NewCalcSetup(struct Screen * );

void SetzeMuster(struct Window * );

void MyFilledDrawBevelBox(struct RastPort * , long , long , long , long , BOOL );

long MyTextLength(struct RastPort * , char * );

UBYTE GetUnderlinedKey(UBYTE * );

int NewOpenAWindow(struct WindowData * , CONST_STRPTR );

void NewCloseAWindow(struct WindowData * );

void KeySelect(struct Gadget ** , struct IntuiMessage * );

