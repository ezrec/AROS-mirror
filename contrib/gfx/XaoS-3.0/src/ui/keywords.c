/* C code produced by gperf version 2.5 (GNU C++ version) */
/* Command-line: gperf -t -p -D -C -a keywords.gperf  */
#ifdef _plan9_
#include <u.h>
#include <libc.h>
#endif
#include <filter.h>
#include <fractal.h>
#include <ui_helper.h>
#include "play.h"
struct keyword1 {char *name;int type;funcptr callback;void *userdata;};

#define TOTAL_KEYWORDS 47
#define MIN_WORD_LENGTH 4
#define MAX_WORD_LENGTH 14
#define MIN_HASH_VALUE 10
#define MAX_HASH_VALUE 118
/* maximum key range = 109, duplicates = 0 */

static unsigned int
hash (register const char *str, register int len)
{
  static const unsigned char asso_values[] =
    {
     119, 119, 119, 119, 119, 119, 119, 119, 119, 119,
     119, 119, 119, 119, 119, 119, 119, 119, 119, 119,
     119, 119, 119, 119, 119, 119, 119, 119, 119, 119,
     119, 119, 119, 119, 119, 119, 119, 119, 119, 119,
     119, 119, 119, 119, 119, 119, 119, 119, 119, 119,
     119, 119, 119, 119, 119, 119, 119, 119, 119, 119,
     119, 119, 119, 119, 119, 119, 119, 119, 119, 119,
     119, 119, 119, 119, 119, 119, 119, 119, 119, 119,
     119, 119, 119, 119, 119, 119, 119, 119, 119, 119,
     119, 119, 119, 119, 119, 119, 119,  50, 119,  30,
       0,   0,  35,   0, 119,  15,  10, 119,  30,  40,
      10,  10,  61, 119,  15,  50,   6,   0,  40,   0,
     119,   5,  15, 119, 119, 119, 119, 119,
    };
  return len + asso_values[(unsigned int)str[len - 1]]
    + asso_values[(unsigned int)str[0]];
}

const struct keyword1 *
in_word_set (register const char *str, register int len)
{
  static const struct keyword1 wordlist[] =
    {
      {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, 
      {"",}, 
      {"wait",  GENERIC, (funcptr) uih_playwait, NULL},
      {"",}, 
      {"display",  GENERIC, (funcptr) uih_display, NULL},
      {"",}, 
      {"defaultpalette",  PARAM_INTEGER, (funcptr) uih_playdefpalette, NULL},
      {"",}, 
      {"text",  PARAM_STRING, (funcptr) uih_text, NULL},
      {"",}, {"",}, 
      {"juliaseed",  PARAM_COORD, (funcptr) uih_setjuliaseed, NULL},
      {"range",  PARAM_INTEGER, (funcptr) uih_setguessing, NULL},
      {"outcoloring",  PARAM_INTEGER, (funcptr) uih_setoutcoloringmode, NULL},
      {"outtcoloring",  PARAM_INTEGER, (funcptr) uih_setouttcolor, NULL},
      {"",}, 
      {"initstate",  GENERIC, (funcptr) uih_playinit, NULL},
      {"incoloring",  PARAM_INTEGER, (funcptr) uih_setincoloringmode, NULL},
      {"intcoloring",  PARAM_INTEGER, (funcptr) uih_setintcolor, NULL},
      {"",}, 
      {"rotationspeed",  PARAM_FLOAT, (funcptr) uih_rotationspeed, NULL},
      {"textpossition",  GENERIC, (funcptr) uih_playtextpos, NULL},
      {"",}, {"",}, {"",}, {"",}, 
      {"load",  PARAM_STRING, (funcptr) uih_playload, NULL},
      {"",}, {"",}, 
      {"cycling",  PARAM_BOOL, (funcptr) uih_cycling, NULL},
      {"",}, 
      {"calculate",  GENERIC, (funcptr) uih_playcalculate, NULL},
      {"zoomcenter",  PARAM_COORD, (funcptr) uih_zoomcenter, NULL},
      {"",}, 
      {"cyclingspeed",  PARAM_INTEGER, (funcptr) uih_setcycling, NULL},
      {"fastmode",  PARAM_NSTRING, (funcptr) uih_setfastmode, save_fastmode},
      {"view",  GENERIC, (funcptr) uih_play_setview, NULL},
      {"fastrotate",  PARAM_BOOL, (funcptr) uih_fastrotate, NULL},
      {"unzoom",  GENERIC, (funcptr) uih_playunzoom, NULL},
      {"message",  PARAM_STRING, (funcptr) uih_playmessage, NULL},
      {"moveview",  GENERIC, (funcptr) uih_playmove, NULL},
      {"morphview",  GENERIC, (funcptr) uih_playmorph, NULL},
      {"morphangle",  PARAM_FLOAT, (funcptr) uih_playmorphangle, NULL},
      {"clearscreen",  GENERIC, (funcptr) uih_clearscreen, NULL},
      {"",}, {"",}, {"",}, 
      {"angle",  PARAM_FLOAT, (funcptr) uih_angle, NULL},
      {"filter",  GENERIC, (funcptr) uih_playfilter, NULL},
      {"",}, {"",}, 
      {"zoom",  GENERIC, (funcptr) uih_playzoom, NULL},
      {"autorotate",  PARAM_BOOL, (funcptr) uih_playautorotate, NULL},
      {"animateview",  GENERIC, (funcptr) uih_play_setview2, NULL},
      {"maxiter",  PARAM_INTEGER, (funcptr) uih_setmaxiter, NULL},
      {"",}, {"",}, 
      {"julia",  PARAM_BOOL, (funcptr) uih_playjulia, NULL},
      {"plane",  PARAM_INTEGER, (funcptr) uih_setplane, NULL},
      {"usleep",  PARAM_INTEGER, (funcptr) uih_playusleep, NULL},
      {"palette",  GENERIC, (funcptr) uih_playpalette, NULL},
      {"",}, {"",}, {"",}, {"",}, 
      {"letterspersec",  PARAM_INTEGER, (funcptr) uih_letterspersec, NULL},
      {"",}, {"",}, 
      {"textsleep",  GENERIC, (funcptr) uih_playtextsleep, NULL},
      {"periodicity",  PARAM_BOOL, (funcptr) uih_setperiodicity, NULL},
      {"",}, {"",}, {"",}, {"",}, {"",}, 
      {"perturbation",  PARAM_COORD, (funcptr) uih_setperbutation, NULL},
      {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, 
      {"formula",  GENERIC, (funcptr) uih_play_formula, NULL},
      {"",}, 
      {"fastjulia",  PARAM_BOOL, (funcptr) uih_setjuliamode, NULL},
      {"",}, {"",}, {"",}, {"",}, {"",}, 
      {"morphjulia",  PARAM_COORD, (funcptr) uih_playmorphjulia, NULL},
      {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, 
      {"maxstep",  PARAM_FLOAT, (funcptr) uih_setmaxstep, NULL},
      {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, 
      {"stop",  GENERIC, (funcptr) uih_playstop, NULL},
      {"",}, {"",}, 
      {"speedup",  PARAM_FLOAT, (funcptr) uih_setspeedup, NULL},
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].name;

          if (*s == *str && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}
