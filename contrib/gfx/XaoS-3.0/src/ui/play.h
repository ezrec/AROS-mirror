#ifndef PLAY_H
#define PLAY_H
struct keyword
  {
    char *name;
    int type;
    void (*callback) (void);
    void *userdata;
  };
typedef void (*funcptr) (void);
#define GENERIC 0
#define PARAM_INTEGER 1
#define PARAM_BOOL 2
#define PARAM_NSTRING 3
#define PARAM_FLOAT 4
#define PARAM_KEYSTRING 5
#define PARAM_COORD 6
#define PARAM_STRING 7
extern char *save_fastmode[];
void uih_play_setview (struct uih_context *uih);
void uih_play_setview2 (struct uih_context *uih);
void uih_play_formula (struct uih_context *uih);
void uih_playfilter (struct uih_context *uih);
void uih_zoomcenter (struct uih_context *uih, number_t p1, number_t p2);
void uih_playpalette (struct uih_context *uih);
void uih_playdefpalette (struct uih_context *uih, int shift);
void uih_playusleep (struct uih_context *uih, int time);
void uih_playtextsleep (struct uih_context *uih);
void uih_playwait (struct uih_context *uih);
void uih_playjulia (struct uih_context *uih, int julia);
void uih_playzoom (struct uih_context *uih);
void uih_playunzoom (struct uih_context *uih);
void uih_playstop (struct uih_context *uih);
void uih_playmorph (struct uih_context *uih);
void uih_playmove (struct uih_context *uih);
void uih_playtextpos (struct uih_context *uih);
void uih_playcalculate (struct uih_context *uih);
void uih_playmorphjulia (struct uih_context *uih, number_t p1, number_t p2);
void uih_playmorphangle (struct uih_context *uih, number_t angle);
void uih_playautorotate (struct uih_context *uih, int mode);
void uih_playmessage (struct uih_context *uih, char *message);
void uih_playload (struct uih_context *uih, char *message);
void uih_playinit (struct uih_context *uih);


#endif
