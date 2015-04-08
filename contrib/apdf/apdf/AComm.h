//========================================================================
//
// AComm.h
//
// Copyright 1999-2002 Emmanuel Lesueur
//
//========================================================================

#ifndef ACOMM_H
#define ACOMM_H

#include <exec/nodes.h>

#define VMAGIC       308

/*#define st_init      0
#define st_newdoc    1
#define st_url       2
#define st_run       3
#define st_goto      4
#define st_newdocnw  5
#define st_goback    6
#define st_prevpage  7
#define st_nextpage  8*/

struct PDFDoc;
struct AOutputDev;
struct OutputDev;
struct FontOutputDev;
struct Annot;
struct Action;
struct TextField;
struct ChoiceField;

struct OutlinesInfo {
    const char **titles;
    struct Action **actions;
    char *buf;
};

#define actionEnd           0
#define actionUpdateRect    1
#define actionShowText      2
#define actionChangePage    3
#define actionLaunch        4
#define actionURI           5
#define actionNamed         6
#define actionGoTo          7
#define actionOpenDoc       8
#define actionEditText      9
#define actionEditChoice    10
#define actionRefresh       11
#define actionUpdateAnnots  12

struct ActionInfo {
    int id;
    void* action;
};

struct mActionOpenDoc {
    int new_window;
    char file[1];
};

struct mActionGoTo {
    int page;
    int x1;
    int y1;
    int x2;
    int y2;
    int mode;
};

struct mActionShowText {
    char msg[1];
};

struct mActionUpdateRect {
    int x;
    int y;
    int width;
    int height;
};

struct mActionLaunch {
    int new_window;
    char cmd[1];
};

struct mActionURI {
    int new_window;
    char uri[1];
};

struct mActionEditText {
    struct TextField *field;
    int x;
    int y;
    int width;
    int height;
    int multiline;
    int password;
    int maxlen;
    int quadding;
    char text[1];
};

struct mActionEditChoice {
    struct ChoiceField *field;
    int x;
    int y;
    int width;
    int height;
    int quadding;
    char **entries;
    int active;
    int editflag;
    int popupflag;
    char value[1];
};

#define actionFirstPage     100
#define actionLastPage      101
#define actionNextPage      102
#define actionPrevPage      103
#define actionGoBack        104

struct mActionNamed {
    int id;
};

struct AnnotInfo {
    struct Annot *annot;
    int x;
    int y;
    int width;
    int height;
    char *title;
};


struct StartupData {
    void* port;
    int in;
    int out;
    int err;
    int status;
    int flags;
    int dir;
    void *task;
};

struct msg_base {
    int success;
    int error;
};

struct msg_init {
    struct msg_base base;
    int version;
    int errors;
    volatile int *abort_flag_ptr;
    volatile int *draw_state_ptr;
    void *io_server;
};

struct msg_prefs {
    struct msg_base base;
    int colors;
    size_t cachesz;
    size_t cacheblocsz;
    char gzipcmd[256];
};

struct msg_version {
    struct msg_base base;
    char version[1024];
};

struct msg_create_output_dev {
    struct msg_base base;
    struct AOutputDev *out;
    void* port;
    int depth;
    int map_mode;
    unsigned char map[256];
};

struct msg_delete_output_dev {
    struct msg_base base;
    struct AOutputDev *out;
};

struct msg_setaalevels {
    struct msg_base base;
    struct AOutputDev *out;
    int text;
    int stroke;
    int fill;
};

struct msg_pagesize {
    struct msg_base base;
    struct PDFDoc* doc;
    int page;
    int width;
    int height;
    int rotate;
};

struct msg_page {
    struct msg_base base;
    struct AOutputDev* aout;
    struct PDFDoc* doc;
    int page_num;
    int zoom;
    int rotate;
    int xoffset;
    int yoffset;
    int width;
    int height;
    int modulo;
    int bpp;
    int pixfmt;
    int level;
    int returned_level;
};

struct msg_createdoc {
    struct msg_base base;
    struct PDFDoc* doc;
    int num_pages;
    long dir;
    char filename[256];
    char ownerpw[36];
    char userpw[36];
};

struct msg_deletedoc {
    struct msg_base base;
    struct PDFDoc* doc;
};

struct msg_find {
    struct msg_base base;
    struct PDFDoc *doc;
    struct AOutputDev *aout;
    int xmin;
    int xmax;
    int ymin;
    int ymax;
    int top;
    int bottom;
    int page;
    char str[100];
};

/*struct msg_chklink {
    struct msg_base base;
    struct PDFDoc *doc;
    struct AOutputDev *aout;
    int x;
    int y;
    struct Action *action;
    char str[256];
};

struct msg_doaction {
    struct msg_base base;
    struct PDFDoc *doc;
    struct Action *action;
    struct AOutputDev *aout;
    int state;
    int page;
    int x;
    int y;
    char str[256];
};*/

struct msg_write {
    struct msg_base base;
    struct PDFDoc *doc;
    struct OutputDev *savedev;
    int type;
    int first_page;
    int last_page;
    int zoom;
    int rotate;
    char filename[256];
};

struct msg_gettext {
    struct msg_base base;
    struct AOutputDev *aout;
    int xmin;
    int xmax;
    int ymin;
    int ymax;
    int size;
};

struct msg_getdocinfo {
    struct msg_base base;
    struct PDFDoc *doc;
    int size;
    int linearized;
    int encrypted;
    int oktoprint;
    int oktocopy;

};

struct msg_showfonts {
    struct msg_base base;
    int flag;
};

struct msg_fontmap {
    struct msg_base base;
    struct FontOutputDev *out;
    int m;
    int type;
    int flags;
    char pdffont[128];
    char font[128];
};

struct msg_addfontmap {
    struct msg_base base;
    struct PDFDoc *pdfdoc;
    int type;
    char pdffont[128];
    char font[128];
};

struct msg_scan_fonts {
    struct msg_base base;
    struct PDFDoc *doc;
    struct FontOutputDev *out;
    int first_page;
    int last_page;
};

struct msg_get_outlines_info {
    struct msg_base base;
    struct PDFDoc *doc;
    int num;
    size_t bufsize;
};

struct msg_get_outlines {
    struct msg_base base;
    struct PDFDoc *doc;
};


struct msg_startdoc {
    struct msg_base base;
    struct AOutputDev *aout;
};

struct msg_addfontpath {
    struct msg_base base;
    char path[256];
};

struct msg_pagelabel {
    struct msg_base base;
    struct PDFDoc *doc;
    int index;
    char label[32];
};

struct msg_pagelabels {
    struct msg_base base;
    struct PDFDoc *doc;
    size_t size;
};

struct msg_error {
    struct msg_base base;
    char msg[256];
};

struct msg_getannots {
    struct msg_base base;
    struct PDFDoc *doc;
    struct AOutputDev *aout;
    int page;
    int num;
    size_t size;
};

struct msg_setannotstate {
    struct msg_base base;
    struct PDFDoc *pdfdoc;
    struct Annot *annot;
    struct Action *action;
    int state;
};

struct msg_doaction {
    struct msg_base base;
    struct PDFDoc *pdfdoc;
    struct AOutputDev *aout;
    struct Action *action;
    size_t size;
};

struct msg_getevents {
    struct msg_base base;
    struct AOutputDev *aout;
};

struct msg_settextfield {
    struct msg_base base;
    struct TextField *field;
    size_t size;
    char txt[1024];
};

struct msg_setchoicefield {
    struct msg_base base;
    struct ChoiceField *field;
    size_t size;
    int num;
    char txt[1024];
};

struct msg_fdf {
    struct msg_base base;
    struct PDFDoc *pdfdoc;
    char filename[256];
};

union msg_max {
    struct msg_init m0;
    struct msg_create_output_dev m1;
    struct msg_delete_output_dev m2;
    struct msg_page m3;
    struct msg_createdoc m4;
    struct msg_find m5;
    struct msg_doaction m6;
    //struct msg_chklink m7;
    struct msg_write m8;
    struct msg_gettext m9;
    struct msg_showfonts m10;
    struct msg_fontmap m11;
    struct msg_scan_fonts m12;
    struct msg_deletedoc m13;
    struct msg_get_outlines m14;
    struct msg_get_outlines_info m15;
    struct msg_getdocinfo m16;
    struct msg_startdoc m17;
    struct msg_addfontmap m18;
    struct msg_version m19;
    struct msg_addfontpath m20;
    struct msg_pagelabel m21;
    struct msg_pagelabels m22;
    struct msg_error m23;
    struct msg_getannots m24;
    struct msg_setannotstate m25;
    struct msg_getevents m26;
    struct msg_settextfield m27;
    struct msg_setchoicefield m28;
    struct msg_fdf m29;
};


struct msg_getcolor {
    struct msg_base base;
    unsigned long r;
    unsigned long g;
    unsigned long b;
};

struct msg_getcolors {
    struct msg_base base;
    int num;
    unsigned long table[1];
};

union ppcmsg_max {
    struct msg_getcolor getcolor;
    struct msg_getcolors getcolors;
};

#define MSGID_ABORTED         -1
#define MSGID_INIT            0
#define MSGID_QUIT            1
#define MSGID_GFX             2
#define MSGID_INFO            3
#define MSGID_PAGE            4
#define MSGID_CREATEOUTPUTDEV 5
#define MSGID_DELETEOUTPUTDEV 6
#define MSGID_CREATEDOC       7
#define MSGID_DELETEDOC       8
#define MSGID_FIND            9
#define MSGID_CHKLINK         10
#define MSGID_DOACTION        11
#define MSGID_SIMPLEFIND      12
#define MSGID_WRITE           13
#define MSGID_VERSION         14
#define MSGID_SETAALEVELS     15
#define MSGID_GETTEXT         16
#define MSGID_SHOWFONTS       17
#define MSGID_ADDFONTMAP      18
#define MSGID_CLEARFONTMAP    19
#define MSGID_CLEARFONTPATH   20
#define MSGID_ADDFONTPATH     21
#define MSGID_SCANFONTS       22
#define MSGID_ENDSCANFONTS    23
#define MSGID_GETFONT         24
#define MSGID_DEFAULTFONTS    25
#define MSGID_PAGESIZE        26
#define MSGID_INITFIND        27
#define MSGID_ENDFIND         28
#define MSGID_INITWRITE       29
#define MSGID_ENDWRITE        30
#define MSGID_INITSCAN        31
#define MSGID_GETOUTLINESINFO 32
#define MSGID_GETOUTLINES     33
#define MSGID_GETDOCINFO      34
#define MSGID_STARTDOC        35
#define MSGID_USEPREFS        36
#define MSGID_GETPAGELABEL    37
#define MSGID_GETPAGEINDEX    38
#define MSGID_GETERROR        39
#define MSGID_HASOUTLINES     40
#define MSGID_GETPAGELABELS   41
#define MSGID_GETANNOTS       42
#define MSGID_DISPLAYANNOT    43
#define MSGID_SETANNOTSTATE   44
#define MSGID_GETEVENTS       45
#define MSGID_SETTEXTFIELD    46
#define MSGID_SETCHOICEFIELD  47
#define MSGID_IMPORTFDF       48
#define MSGID_EXPORTFDF       49

#define MSGID_GETCOLOR        100
#define MSGID_GETCOLORS       101

struct comm_info;

struct server_info {
    void *port;
    void *reply_port;
    void *process;
    void *pool;
    void *startupmsg;
    void *extra;
    int sigbit;
    volatile int *abort_flag_ptr;
    volatile int *draw_state_ptr;

    void (*destroy)(struct server_info *);
    void *(*MyAllocSharedMem)(struct server_info *,size_t);
    void (*MyFreeSharedMem)(struct server_info *,void *);
    struct comm_info *(*create_comm_info)(struct server_info*,size_t);
    void (*delete_comm_info)(struct comm_info *);
    int (*exchange_msg)(struct comm_info *,struct msg_base *,size_t,int);
    void (*exchange_raw_msg)(struct comm_info *,void *,size_t,int);
    //extern void (*init_exchange)(struct comm_info *);
    void (*send_msg)(struct comm_info *,void *,size_t,int);
    void *(*get_msg)(struct server_info *,void **,size_t *,int *);
    void (*reply_msg)(void *);
};

struct comm_info {
    struct server_info *sv;
    void *msg;
    void *cmddata;
};

struct async_msg {
    struct MinNode node;
    struct comm_info *ci;
    int replied;
    void *obj;
    int init_methodid;
    int reply_methodid;
    void *msg;
    void *data;
    size_t sz;
    int id;
};

void server(struct comm_info *);

extern struct server_info sv;

BOOL is_sync(void);
BOOL has_error(void);
void sync(void);
void handle_async_msgs(void);
void send_async_msg(struct async_msg *);
void abort_async_msg(struct async_msg *);
void reply_async_msg(struct async_msg *,void*);
void abort_msg(void);

//extern void (*end_exchange)(struct comm_info *);
//extern void (*delete_server)(struct server_info*);

extern struct comm_info *(*create_comm_info)(size_t);
extern void (*delete_comm_info)(struct comm_info *);
extern int (*exchange_msg)(struct comm_info *,struct msg_base *,size_t,int);
extern void (*send_msg)(struct comm_info *,void *,size_t,int);
extern void *(*get_msg)(struct server_info *,void **,size_t *,int *);
extern void (*reply_msg)(void *);

#define LOCK
#define UNLOCK

#endif

