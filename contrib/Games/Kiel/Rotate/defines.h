/*  Layout   */
#define MAXCOLORS 8
#define MAXBITMAP 3
#define MAXX 22
#define MAXY 14
#define MINX 5
#define MINY 5
#define fwidth 30
#define fheight 30
#define bleft 30
#define bbottom 30
#define MENHI 12

#define MENUFLAGSN ITEMTEXT|ITEMENABLED|HIGHBOX|CHECKIT
#define MENUFLAGSC ITEMTEXT|ITEMENABLED|HIGHBOX|CHECKIT|CHECKED
#define MENUFLAGSE ITEMTEXT|ITEMENABLED|HIGHBOX
#define MENUFLAGSD ITEMTEXT|HIGHBOX


/* Constants */
#define EMPTY -1
#define MODE_NORMAL 0
#define MODE_REMOVE 1
#define MODE_ROTATE 2

/* Functions */
#define random(max) (rand()%((int)(max)))

#define StopMsg() ModifyIDCMP(Window,0L)
#define ContMsg() ModifyIDCMP(Window,iflags)
#define MenuOn()  menu1.Flags=MENUENABLED
#define MenuOff() menu1.Flags=0

