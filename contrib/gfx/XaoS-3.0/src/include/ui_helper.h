#ifndef UI_HELPER_H
#define UI_HELPER_H
#include <timers.h>
#include "xio.h"
#define MAXFILTERS 10
#define AVRGSIZE 50
struct uih_savedcontext
  {
    xio_file file;
    int mode;
    number_t speedup, maxstep;
    number_t xcenter, ycenter;
    tl_timer *timer;
    tl_timer *synctimer;
    struct fractal_context *fcontext;
    int displaytext;
    int clearscreen;
    char *text;
    int fastmode, juliamode, fastrotate, autorotate;
    number_t rotationspeed;
    int firsttime;
    int filter[MAXFILTERS];
    int pressed;
    int rotatepressed;
    int cycling;
    int direction;
    int cyclingspeed;
    int zoomactive;
    int xtextpos, ytextpos;
    int writefailed;
    int nonfractalscreen;
  };
#define MAXLEVEL 10		/*Maximal include level */
struct uih_playcontext
  {
    xio_file file;
    xio_file prevfiles[MAXLEVEL + 1];
    int level;
    xio_path directory;
    tl_timer *timer;
    int waittime;
    int playframe;
    int timerin;
    int frametime, starttime;
    int morph;
    int morphjulia;
    int morphangle;
    vinfo destination;
    vinfo source;
    number_t srcangle, destangle;
    number_t sr, si, dr, di;
    int readfailed;
    int line;
  };
struct uih_context
  {
    /*stuff that should be visible from outside */
    number_t speedup, maxstep;	/*zooming speed */
    /*readonly */
    struct image *image;
    struct palette *palette;
    struct fractal_context *fcontext;	/*fractal informations */
    struct queue *queue;
    struct filter *zengine;	/*zooming filter */
    struct filter *uifilter;	/*user interface layer */
    double mul;			/*speed of last iteration */
    int rotatemode;		/*ROTATE_NONE, ROTATE_CONTINUOUS or ROTATE_NONE */
    number_t rotationspeed;	/*speed of continuous rotation */
    int fastmode;		/*when draw in fast mode */
    int juliamode;
    int fastrotate;
    int uncomplette;		/*1 if image is not complettly caluclated or in animation */
    int dirty;			/*1 if image is inexact */
    int inanimation;		/*1 if uih_update wants to be called soon */
    int fastanimation;		/*1 if animation needs to be fast */
    int incalculation;		/*1 if calulcation is currently in process */
    int cycling;		/*1 if cycling is enabled */
    int save;			/*1 if save animation is enabled */
    int play;			/*1 if animation replay is active */
    int palettetype, paletteseed;	/*0 for default palette,1,2 for random */
    int display;		/*1 when ui wants to display something */
    int displaytext;		/*1 when ui wants to display text */
    int clearscreen;		/*1 when ui want to clear screen */
    int xtextpos, ytextpos;	/*possitioning of text */
    int recalculatemode;	/*informations for user interface */
    int stoppedtimers;
    int nletters;		/*Number of letters displayed at screen */
    int letterspersec;		/*Number of letters per second user should read */
    char *text;
    char *errstring;
    /*hidden thinks */
    /*Save variables */
    int todisplayletters;
    struct uih_savedcontext *savec;
    int viewchanged;		/*When to generate setview commands */
    int palettechanged;		/*When to generate setpalette commands */
    int nonfractalscreen;
    /*waiting variables */
    void (*complettehandler) (void *);	/*Handler to be activated when calculation is complette */
    void *handlerdata;
    /*replay variables */
    struct uih_playcontext *playc;

    struct filter *rotatef;

    /*Fixedcolor stuff */
    struct filter *fixedcolor;

    /*Julia/filter mechanizm */
    struct filter *subwindow, *julia, *smalliter;
    struct filter *filter[MAXFILTERS];

    /*zoom/unzoom */
    number_t speed, step;
    number_t xcenter, ycenter;
    int xcenterm, ycenterm;
    int zoomactive;

    /*drag&drop move */
    int pressed;
    number_t oldx, oldy;
    int moved;

    /*drag&drop rotate */
    int rotatepressed;
    number_t oldangle;

    int ddatalost;
    int flags;
    int interrupt;

    int tbreak;

    int autopilot;		/*for uih side of autopilot */
    int autopilotx, autopiloty, autopilotbuttons;

    /*calculation time variables */
    int interruptiblemode;
    int starttime, endtime;
    int maxtime;

    /*dynamical timeout measuring */
    int times[2][AVRGSIZE];	/*for statistics */
    int timespos, count[2];
    double lastspeed, lasttime;



    number_t xsize, ysize;
    tl_timer *maintimer, *cyclingtimer, *autopilottimer, *calculatetimer,
     *doittimer;
    tl_group *autopilotgroup;
    /*color cycling values */
    int direction;
    int stopped;
    int cyclingspeed;

    /*autopilot internal values */
    int x1, y1, c1;
    number_t minsize;
    number_t maxsize;
    int autopilotversion;
    int autime;
    int minlong;
    int interlevel;

    /*server's callbacks */
    int (*passfunc) (struct uih_context *, int, char *, float);
    void (*longwait) (struct uih_context *);

    /*saved palettes */
    struct palette *palette2;

    int paletteshift;
    int indofractal;

  };
typedef struct uih_context uih_context;
#define UIH_SAVEALL 2
#define UIH_SAVEANIMATION 1
#define UIH_SAVEPOS 0

#define UIH_PALETTEDRAW -1
#define UIH_INTERRUPTIBLE 0
#define UIH_ANIMATION 1
#define UIH_NEW_IMAGE 2
#define UIH_UNINTERRUPTIBLE 3
#define FRAMETIME (1000000/FRAMERATE)

#define UIH_TEXTTOP 0
#define UIH_TEXTMIDDLE 1
#define UIH_TEXTBOTTOM 2

#define UIH_TEXTLEFT 0
#define UIH_TEXTCENTER 1
#define UIH_TEXTRIGHT 2

#define RANDOM_PALETTE_SIZE 1
#define FULLSCREEN 2
#define UPDATE_AFTER_PALETTE 4
#define UPDATE_AFTER_RESIZE 8
#define PALETTE_ROTATION 16
#define ASYNC_PALETTE 32
#define ROTATE_INSIDE_CALCULATION 64
#define PALETTE_REDISPLAYS 128
#define SCREENSIZE 256
#define PIXELSIZE 512
#define RESOLUTION 1024

#define BUTTON1 256
#define BUTTON2 512
#define BUTTON3 1024

#define ROTATE_NONE 0
#define ROTATE_MOUSE 1
#define ROTATE_CONTINUOUS 2

#define uih_needrecalculate(context) ((context)->recalculatemode)
#define uih_needdisplay(context) ((context)->display)
#define MAX(a,b) ((a)>(b)?(a):(b))
#define uih_newimage(c) (((c)->recalculatemode=MAX((c)->recalculatemode,UIH_NEW_IMAGE)))
#define uih_animate_image(c) ((c)->recalculatemode=MAX((c)->recalculatemode,UIH_ANIMATION))
#define uih_cleared(c) ((c)->clearscreen=0,(c)->nonfractalscreen=1,uih_callcomplette(c),(c)->nletters=0)
#define uih_textdisplayed(c) ((c)->displaytext=0,free((c)->text),(c)->nonfractalscreen=1,uih_callcomplette(c),(c)->nletters+=(c)->todisplayletters,(c)->todisplayletters=0)
extern struct filteraction *uih_filters[MAXFILTERS];
extern int uih_nfilters;

struct uih_context *uih_mkcontext (int flags, struct image *image, int (*passfunc) (struct uih_context *, int, char *, float), void (*longwait) (struct uih_context *));
int uih_updateimage (uih_context * c, struct image *img);
void uih_freecontext (uih_context * c);


void uih_callcomplette (uih_context * c);
/*palette functions */
void uih_mkdefaultpalette (uih_context * c);
void uih_mkpalette (uih_context * c);
void uih_savepalette (uih_context * c);
void uih_restorepalette (uih_context * c);

/*autopilot handling */
void uih_autopilot_on (uih_context * c);
void uih_autopilot_off (uih_context * c);

/*misc functions */
int uih_update (uih_context * c, int mousex, int mousey, int mousebuttons);
char *uih_save (struct uih_context *c, xio_path filename);
void uih_tbreak (uih_context * c);
double uih_displayed (uih_context * c);
void uih_do_fractal (uih_context * c);
void uih_interrupt (uih_context * c);
void uih_stopzooming (uih_context * c);
void uih_setspeedup (uih_context * c, number_t speed);
void uih_setmaxstep (uih_context * c, number_t speed);
void uih_setcomplettehandler (uih_context * c, void (h) (void *), void *d);

/*cycling functions */
void uih_cycling_off (struct uih_context *c);
void uih_cycling_stop (struct uih_context *c);
void uih_cycling_continue (struct uih_context *c);
void uih_setcycling (struct uih_context *c, int speed);
int uih_cycling_on (struct uih_context *c);
int uih_cycling (struct uih_context *c, int mode);

/*fractal context manipulation routines */
void uih_setperbutation (uih_context * c, number_t re, number_t im);
void uih_perbutation (uih_context * c, int mousex, int mousey);
void uih_setmaxiter (uih_context * c, int maxiter);
void uih_setincoloringmode (uih_context * c, int mode);
void uih_setoutcoloringmode (uih_context * c, int mode);
void uih_setintcolor (uih_context * c, int mode);
void uih_setouttcolor (uih_context * c, int mode);
void uih_setplane (uih_context * c, int mode);
void uih_setmandelbrot (uih_context * c, int mode, int mousex, int mousey);
void uih_setfastmode (uih_context * c, int mode);
void uih_setguessing (uih_context * c, int range);
void uih_setperiodicity (uih_context * c, int periodicity);
void uih_display (uih_context * c);
void uih_disablejulia (uih_context * c);
int uih_enablejulia (uih_context * c);
int uih_setjuliamode (uih_context * c, int mode);
void uih_setjuliaseed (uih_context * c, number_t zre, number_t zim);

/*filter manipulation */
int uih_enablefilter (uih_context * c, int n);
void uih_disablefilter (uih_context * c, int n);

/*Animation save routines */
int uih_save_enable (struct uih_context *uih, xio_file f, int mode);
void uih_save_disable (struct uih_context *uih);
void uih_saveframe (struct uih_context *uih);
void uih_save_possition (struct uih_context *uih, xio_file f, int mode);

void uih_load (struct uih_context *uih, xio_file f, xio_path name);
void uih_playupdate (struct uih_context *uih);
void uih_replaydisable (struct uih_context *uih);
void uih_skipframe (struct uih_context *uih);
int uih_replayenable (struct uih_context *uih, xio_file f, xio_path filename);

/*timer functions */
void uih_stoptimers (uih_context * c);
void uih_resumetimers (uih_context * c);
void uih_slowdowntimers (uih_context * c, int time);

/*text output functions */
void uih_clearscreen (uih_context * c);
void uih_settextpos (uih_context * c, int x, int y);
void uih_text (uih_context * c, char *text);
void uih_letterspersec (uih_context * c, int n);

/*image rotation functions */
int uih_fastrotate (uih_context * c, int mode);
int uih_fastrotateenable (uih_context * c);
void uih_fastrotatedisable (uih_context * c);
void uih_angle (uih_context * c, number_t angle);
void uih_rotatemode (uih_context * c, int mode);
void uih_rotationspeed (uih_context * c, number_t speed);

/*Catalog functions */
int uih_loadcatalog (uih_context * c, char *name);
void uih_freecatalog (uih_context * c);

#endif
