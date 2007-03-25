#include <libraries/mui.h>
#include <strings.h>

struct stream
{
    UBYTE   *buf;
    int     size;
    int     counter;
    int     stop;
};

/***********************************************************************/

#if !defined(__MORPHOS____) && !defined(__AROS__)
ULONG STDARGS DoSuperNew(struct IClass *cl,Object *obj,ULONG tag1,...);
#endif

Object *ovfixspace(void);
Object *ohfixspace(void);
Object *otextitem(void);
Object *obartitle(ULONG id);
Object *owspace(ULONG weight);
Object *olabel(ULONG id);
Object *olabel1(ULONG id);
Object *ollabel1(ULONG id);
Object *olabel2(ULONG id);
Object *obutton(ULONG label,ULONG help);
Object *ourlText(UBYTE *url,UBYTE *text);
Object *ocheck(ULONG key,ULONG help);
Object *ocycle(ULONG key,UBYTE **entries,ULONG help);
ULONG openWindow(Object *app,Object *win);
void grouping(UBYTE *source, struct MiamiPanelBase_intern *MiamiPanelBaseIntern);
#if !defined(__AROS__)
void sprintf(UBYTE *to,UBYTE *fmt,...);
#endif

#ifdef __MORPHOS__
static void snprintfStuff(void);
#elseif !defined(__AROS__)
static void ASM snprintfStuff(REG(d0,UBYTE c),REG(a3,struct stream *s));
#endif

#if !defined(__AROS__)
int STDARGS snprintf(UBYTE *buf,int size,UBYTE *fmt,...);
#endif

ULONG IDToValue(Tag tag);
Tag valueToID(ULONG val);
struct ifnode *createIFNode(struct MPS_Prefs *prefs,UBYTE *name,ULONG scale, struct MiamiPanelBase_intern *MiamiPanelBaseIntern);
struct ifnode *findIFNode(struct MPS_Prefs *prefs,UBYTE *name);
void freeIFList(struct MPS_Prefs *prefs, struct MiamiPanelBase_intern *MiamiPanelBaseIntern);
void moveMinList(struct MinList *to,struct MinList *from, struct MiamiPanelBase_intern *MiamiPanelBaseIntern);
#ifdef __MORPHOS__
void *memcpy(void *to,const void *from,size_t len);
#endif

/****************************************************************************/

