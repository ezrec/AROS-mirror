#define ATTRLIST			(((struct opSet *)msg)->ops_AttrList)

#define MINMAXMSG			((struct MinMaxMsg *)msg)
#define SETUPMSG			((struct SetupMsg *)msg)
#define LAYOUTMSG			((struct LayoutMsg *)msg)
#define RENDERMSG			((struct gpRender *)msg)
#define INPUTMSG			((struct gpInput *)msg)
#define GOINACTIVEMSG	((struct gpGoInactive *)msg)
#define SETMSG				((struct opSet *)msg)
#define GETMSG				((struct opGet *)msg)
#define HITTESTMSG		((struct gpHitTest *)msg)

#define GAD					((struct Gadget *)obj)
#define WORDS2LONG(x,y) ( (((x) & 0xFFFF) << 16L) | ((y) & 0xFFFF))

