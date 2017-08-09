// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id$
//
// Cyril Deble 1998 (cyril.deble@inforoute.cgs.fr)
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// DESCRIPTION:
//      Doom internal dehacked file loader for amiga only :)
//      0.1 (11/02/98) First internal release
//      0.2 (15/02/98) Currently only a few options supported
//      0.3 (08/03/98) Added much more (almost all) options support
//                     and corrected some pointless bugs
//
//-----------------------------------------------------------------------------

static const char
rcsid[] = "$Id$";

#include <ctype.h>
#include <stdlib.h>

// Data.
#include "doomdef.h"
#include "sounds.h"
#include "m_fixed.h"
#include "doomstat.h"
#include "info.h"
#include "m_cheat.h"

#ifdef __GNUG__
#pragma implementation "info.h"
#endif
#include "info.h"

#include "p_mobj.h"

/* for ammo */
extern int clipammo[];

/* for misc */
extern int idfa_armor;
extern int idfa_armor_class;
extern int idkfa_armor;
extern int idkfa_armor_class;
extern int god_health;
extern int initial_health;
extern int initial_bullets;
extern int MAXHEALTH;
extern int max_armor;
extern int green_armor_class;
extern int blue_armor_class;
extern int maxsoul;
extern int soul_health;
extern int mega_health;
extern int BFGCELLS;

/* for cheat */
extern char cheat_mus_seq[];
extern char cheat_choppers_seq[];
extern char cheat_god_seq[];
extern char cheat_ammo_seq[];
extern char cheat_ammonokey_seq[];
extern char cheat_noclip_seq[];
extern char cheat_commercial_noclip_seq[];
extern char cheat_powerup_seq[7][10];
extern char cheat_clev_seq[];
extern char cheat_mypos_seq[];
extern char cheat_amap_seq[];

enum
{
  THING_ID = 0,
  THING_HIT_POINTS,
  THING_SPEED,
  THING_WIDTH,
  THING_HEIGHT,
  THING_MISSILE_DAMAGE,
  THING_REACTION_TIME,
  THING_PAIN_CHANCE,
  THING_MASS,
  THING_BITS,
  THING_ALERT_SOUND,
  THING_ATTACK_SOUND,
  THING_PAIN_SOUND,
  THING_DEATH_SOUND,
  THING_ACTION_SOUND,
  THING_INITIAL_FRAME,
  THING_FIRST_MOVING_FRAME,
  THING_INJURY_FRAME,
  THING_CLOSE_ATTACK_FRAME,
  THING_FAR_ATTACK_FRAME,
  THING_DEATH_FRAME,
  THING_EXPLODING_FRAME,
  THING_RESPAWN_FRAME,

  FRAME_SPRITE_NUMBER,
  FRAME_SPRITE_SUBNUMBER,
  FRAME_DURATION,
  FRAME_NEXT_FRAME,
  FRAME_UNKNOWN_1,
  FRAME_UNKNOWN_2,

  SPRITE_OFFSET,

  AMMO_MAX_AMMO,
  AMMO_PER_AMMO,

  WEAPON_AMMO_TYPE,
  WEAPON_DESELECT_FRAME,
  WEAPON_SELECT_FRAME,
  WEAPON_BOBBING_FRAME,
  WEAPON_SHOOTING_FRAME,
  WEAPON_FIRING_FRAME,

  POINTER_CODEP_FRAME,

  CHEAT_CHANGE_MUSIC,
  CHEAT_CHAINSAW,
  CHEAT_GOD_MODE,
  CHEAT_AMMO_KEYS,
  CHEAT_AMMO,
  CHEAT_NO_CLIPPING_1,
  CHEAT_NO_CLIPPING_2,
  CHEAT_INVINCIBILITY,
  CHEAT_BERSERK,
  CHEAT_INVISIBILITY,
  CHEAT_RADIATION_SUIT,
  CHEAT_AUTO_MAP,
  CHEAT_LITE_AMP_GOGGLES,
  CHEAT_BEHOLD_MENU,
  CHEAT_LEVEL_WARP,
  CHEAT_PLAYER_POSITION,
  CHEAT_MAP_CHEAT,

  MISC_INITIAL_HEALTH,
  MISC_INITIAL_BULLETS,
  MISC_MAX_HEALTH,
  MISC_MAX_ARMOR,
  MISC_GREEN_ARMOR_CLASS,
  MISC_BLUE_ARMOR_CLASS,
  MISC_MAX_SOULSPHERE,
  MISC_SOULSPHERE_HEALTH,
  MISC_MEGASPHERE_HEALTH,
  MISC_GOD_MODE_HEALTH,
  MISC_IDFA_ARMOR,
  MISC_IDFA_ARMOR_CLASS,
  MISC_IDKFA_ARMOR,
  MISC_IDKFA_ARMOR_CLASS,
  MISC_BFG_CELLS_SHOT,
  MISC_MONSTERS_INFIGHT,

  NUM_KEYWORD
};


/* All keywords supported */
char *Keywords[] = {
/* Things */
"id # = ",                  /* mobjinfo.doomednum    */
"hit points = ",            /* mobjinfo.spawnhealth  */
"speed = ",                 /* mobjinfo.speed        */
"width = ",                 /* mobjinfo.radius       */
"height = ",                /* mobjinfo.height       */
"missile damage = ",        /* mobjinfo.damage       */
"reaction time = ",         /* mobjinfo.reactiontime */
"pain chance = ",           /* mobjinfo.painchance   */
"mass = ",                  /* mobjinfo.mass         */
"bits = ",                  /* mobjinfo.flags        */
"alert sound = ",           /* mobjinfo.seesound     */
"attack sound = ",          /* mobjinfo.attacksound  */
"pain sound = ",            /* mobjinfo.painsound    */
"death sound = ",           /* mobjinfo.deathsound   */
"action sound = ",          /* mobjinfo.activesound  */
"initial frame = ",         /* mobjinfo.spawnstate   */
"first moving frame = ",    /* mobjinfo.seestate     */
"injury frame = ",          /* mobjinfo.painstate    */
"close attack frame = ",    /* mobjinfo.meleestate   */
"far attack frame = ",      /* mobjinfo.missilestate */
"death frame = ",           /* mobjinfo.deathstate   */
"exploding frame = ",       /* mobjinfo.xdeathstate  */
"respawn frame = ",         /* mobjinfo.raisestate   */

/* Frame */

"sprite number = ",         /* states.sprite    */
"sprite subnumber = ",      /* states.frame     */
"duration = ",              /* states.tics      */
"next frame = ",            /* states.nextstate */
"unknown 1 = ",             /* states.misc1     */
"unknown 2 = ",             /* states.misc2     */

/* Sprite */

"offset =",                 /* supported */

/* Ammo */
                           
"max ammo = ",              /* maxammo  */
"per ammo = ",              /* clipammo */

/* Weapon */

"ammo type = ",             /* weaponinfo.ammo       */
"deselect frame = ",        /* weaponinfo.upstate    */
"select frame = ",          /* weaponinfo.downstate  */
"bobbing frame = ",         /* weaponinfo.readystate */
"shooting frame = ",        /* weaponinfo.atkstate   */
"firing frame = ",          /* weaponinfo.flashstate */

/* Pointer */

"codep frame = ",           /* supported */

/* Cheat */

"change music = ",          /* supported */
"chainsaw = ",              /* supported */
"god mode = ",              /* supported */
"ammo & keys = ",           /* supported */
"ammo = ",                  /* supported */
"no clipping 1 = ",         /* supported */
"no clipping 2 = ",         /* supported */
"invincibility = ",         /* supported */
"berserk = ",               /* supported */
"invisibility = ",          /* supported */
"radiation suit = ",        /* supported */
"auto-map = ",              /* supported */
"lite-amp goggles = ",      /* supported */
"behold menu = ",           /* supported */
"level warp = ",            /* supported */
"player position = ",       /* supported */
"map cheat = ",             /* supported */

/* Misc */

"initial health = ",        /* initial_health    */
"initial bullets = ",       /* initial_bullets   */
"max health = ",            /* maxhealth         */
"max armor = ",             /* max_armor         */
"green armor class = ",     /* green_armor_class */
"blue armor class = ",      /* blue_armor_class  */
"max soulsphere = ",        /* maxsoul           */
"soulsphere health = ",     /* soul_health       */
"megasphere health = ",     /* mega_health       */
"god mode health = ",       /* god_health        */
"idfa armor = ",            /* idfa_armor        */
"idfa armor class = ",      /* idfa_armor_class  */
"idkfa armor = ",           /* idkfa_armor       */
"idkfa armor class = ",     /* idfa_armor_class  */
"bfg cells/shot = ",        /* bfgcells          */
"monsters infight = ",      /* not supported     */


/* Others to follow :) */
"\n"                     /* The last              */
};


/* Backup of action pointer */
actionf_t actions[NUMSTATES];
/* Backup of sprites names */
char * oldsprnames[NUMSPRITES];

void DE_Cheat(char *cheatseq, char* newcheat)
{
  unsigned char *i,*j;

  for(i=newcheat;*i!='\0';i++)
      *i=SCRAMBLE(*i);

  for(i=cheatseq,j=newcheat;*j!='\0' && *j!=0xff;i++,j++)
      if(*i==1 || *i==0xff) // no more place in the cheat
         return;
      else
         *i=*j;

  j=i;

  for(;*i!=0xff;i++)
      if(*i==1)
      {
         *j++=1;
         *j++=0;
         *j++=0;
         break;
      }
  *j=0xff;

  return;
}


void DE_Text(FILE* f,int len1,int len2)
{
  char s[50];
  int i;

  // it is hard to change all the text in doom
  // here i implement only vital things
  // yes text change somes tables like music, sound and sprite name
  if(len1>6)
    return;

  if(fread(s,len1+len2,1,f))
  {
    for(i=0;i<108;i++)
      if(!strncmp(S_sfx[i].name,s,len1))
      {
        strncpy(S_sfx[i].name,&(s[len1]),len2);
        return;
      }
    for(i=0;i<NUMSPRITES;i++)
      if(!strncmp(sprnames[i],s,len1))
      {
        strncpy(sprnames[i],&(s[len1]),len2);
        return;
      }
    for(i=0;i<NUMSPRITES;i++)
      if(!strncmp(sprnames[i],s,len1))
      {
        strncpy(sprnames[i],&(s[len1]),len2);
        return;
      }
    for(i=1;i<68;i++)
      if(!strncmp(S_music[i].name,s,len1))
      {
        strncpy(S_music[i].name,&(s[len1]),len2);
        return;
      }
      /* more text to follow */


  }
}


int
DE_Search(char *arg, char **list)
{
  int i, l;

  /* Make into lower case, and get length of string */
  for (l = 0; *(arg + l); l++)
    *(arg + l) = tolower(*(arg + l));
  /*printf("arg = '%s'\n", arg);*/

  for (i = 0; **(list + i) != '\n'; i++) {
    /*printf("arg = '%s' list = '%s'\n", arg, *(list + i));*/
    if (strstr(arg,*(list + i)))
        return(i);
  }
  return(-1);
}


int
DE_Hack( char * line)
{
    static int thing = 0;
    static int frame = 0;
    static int ammo = 0;
    static int weapon = 0;
    static int sprite = 0;
    static int pointer = 0;

    int choice;
    char * arg;

    /* scanf variables */
    int n1;
    char buf1[255];

    /* first check for main keywords */
    if(sscanf(line, "Thing %d (%s)", &n1, buf1) == 2)
    {
         thing = n1-1;
         //buf1[strlen(buf1)-1] = '\0';
         //printf("-> Changing Thing %d (%s)\n", n1, buf1);
         return(1);
    }

    if(thing >= NUMMOBJTYPES) return(0);

    if(sscanf(line, "Frame %d", &n1) == 1)
    {
         frame = n1;
         //printf("-> Changing Frame %d\n", n1);
         return(1);
    }

    if(frame >= NUMSTATES) return(0);

    if(sscanf(line, "Pointer %d", &n1) == 1)
    {
         pointer = n1;
         //printf("-> Changing Pointer %d\n", n1);
         return(1);
    }

    if(pointer >= NUMSTATES) return(0);

    if(sscanf(line, "Ammo %d", &n1) == 1)
    {
         ammo = n1;
         //printf("-> Changing Ammo %d\n", n1);
         return(1);
    }

    if(ammo >= NUMAMMO) return(0);

    if(sscanf(line, "Cheat %d", &n1) == 1)
    {
         //printf("-> Changing Cheat\n");
         return(1);
    }

    if(sscanf(line, "Weapon %d", &n1) == 1)
    {
         weapon = n1;
         //printf("-> Changing Weapon %d\n", n1);
         return(1);
    }

    if(weapon >= NUMWEAPONS) return(0);

    if(sscanf(line, "Sprite %d", &n1) == 1)
    {
         sprite = n1;
         //printf("-> Changing Sprite %d\n", n1);
         return(1);
    }

    if(sprite >= NUMSPRITES) return(0);

    if(sscanf(line, "Misc %d", &n1) == 1)
    {
         //printf("-> Changing Misc\n");
         return(1);
    }

    /* search for option */
    choice = DE_Search(line, Keywords);

/*    if(choice < 0 || choice > NUM_KEYWORD-1) return(0);*/
    if(choice < 0 || choice > NUM_KEYWORD-1) return(0);
    arg = strstr(line, Keywords[choice]) +
          strlen(Keywords[choice]);

    /*printf("Choice = %d, Arg = %s\n", choice, arg);*/

    switch(choice)
    {
      case THING_ID:
          mobjinfo[thing].doomednum = atoi(arg);
          break;
      case THING_HIT_POINTS:
          mobjinfo[thing].spawnhealth = atoi(arg);
          break;
      case THING_SPEED:
          mobjinfo[thing].speed = atoi(arg);
          break;
      case THING_WIDTH:
          mobjinfo[thing].radius = atoi(arg);
          break;
      case THING_HEIGHT:
          mobjinfo[thing].height = atoi(arg);
          break;
      case THING_MISSILE_DAMAGE:
          mobjinfo[thing].damage = atoi(arg);
          break;
      case THING_REACTION_TIME:
          mobjinfo[thing].reactiontime = atoi(arg);
          break;
      case THING_PAIN_CHANCE:
          mobjinfo[thing].painchance = atoi(arg);
          break;
      case THING_MASS:
          mobjinfo[thing].mass = atoi(arg);
          break;
      case THING_BITS:
          mobjinfo[thing].flags = atoi(arg);
          break;
      case THING_ALERT_SOUND:
          mobjinfo[thing].seesound = atoi(arg);
          break;
      case THING_ATTACK_SOUND:
          mobjinfo[thing].attacksound = atoi(arg);
          break;
      case THING_PAIN_SOUND:
          mobjinfo[thing].painsound = atoi(arg);
          break;
      case THING_DEATH_SOUND:
          mobjinfo[thing].deathsound = atoi(arg);
          break;
      case THING_ACTION_SOUND:
          mobjinfo[thing].activesound = atoi(arg);
          break;
      case THING_INITIAL_FRAME:
          mobjinfo[thing].spawnstate = atoi(arg);
          break;
      case THING_FIRST_MOVING_FRAME:
          mobjinfo[thing].seestate = atoi(arg);
          break;
      case THING_INJURY_FRAME:
          mobjinfo[thing].painstate = atoi(arg);
          break;
      case THING_CLOSE_ATTACK_FRAME:
          mobjinfo[thing].meleestate = atoi(arg);
          break;
      case THING_FAR_ATTACK_FRAME:
          mobjinfo[thing].missilestate = atoi(arg);
          break;
      case THING_DEATH_FRAME:
          mobjinfo[thing].deathstate = atoi(arg);
          break;
      case THING_EXPLODING_FRAME:
          mobjinfo[thing].xdeathstate = atoi(arg);
          break;
      case THING_RESPAWN_FRAME:
          mobjinfo[thing].raisestate = atoi(arg);
          break;
      case FRAME_SPRITE_NUMBER:
          states[frame].sprite = atoi(arg);
          break;
      case FRAME_SPRITE_SUBNUMBER:
          states[frame].frame = atoi(arg);
          break;
      case FRAME_DURATION:
          states[frame].tics = atoi(arg);
          break;
      case FRAME_NEXT_FRAME:
          states[frame].nextstate = atoi(arg);
          break;
      case FRAME_UNKNOWN_1:
          states[frame].misc1 = atoi(arg);
          break;
      case FRAME_UNKNOWN_2:
          states[frame].misc2 = atoi(arg);
          break;
      case WEAPON_AMMO_TYPE:
          weaponinfo[weapon].ammo = atoi(arg);
          break;
      case WEAPON_DESELECT_FRAME:
          weaponinfo[weapon].upstate = atoi(arg);
          break;
      case WEAPON_SELECT_FRAME:
          weaponinfo[weapon].downstate = atoi(arg);
          break;
      case WEAPON_BOBBING_FRAME:
          weaponinfo[weapon].readystate = atoi(arg);
          break;
      case WEAPON_SHOOTING_FRAME:
          weaponinfo[weapon].atkstate = atoi(arg);
          break;
      case WEAPON_FIRING_FRAME:
          weaponinfo[weapon].flashstate = atoi(arg);
          break;
      case AMMO_MAX_AMMO:
          maxammo[ammo] = atoi(arg);
          break;
      case AMMO_PER_AMMO:
          clipammo[ammo] = atoi(arg);
          break;
      case CHEAT_CHANGE_MUSIC:
          DE_Cheat(cheat_mus_seq,arg);
          break;
      case CHEAT_CHAINSAW:
          DE_Cheat(cheat_choppers_seq,arg);
          break;
      case CHEAT_GOD_MODE:
          DE_Cheat(cheat_god_seq,arg);
          break;
      case CHEAT_AMMO_KEYS:
          DE_Cheat(cheat_ammonokey_seq,arg);
          break;
      case CHEAT_AMMO:
          DE_Cheat(cheat_ammo_seq,arg);
          break;
      case CHEAT_NO_CLIPPING_1:
          DE_Cheat(cheat_noclip_seq,arg);
          break;
      case CHEAT_NO_CLIPPING_2:
          DE_Cheat(cheat_commercial_noclip_seq,arg);
          break;
      case CHEAT_INVINCIBILITY:
          DE_Cheat(cheat_powerup_seq[0],arg);
          break;
      case CHEAT_BERSERK:
          DE_Cheat(cheat_powerup_seq[1],arg);
          break;
      case CHEAT_INVISIBILITY:
          DE_Cheat(cheat_powerup_seq[2],arg);
          break;
      case CHEAT_RADIATION_SUIT:
          DE_Cheat(cheat_powerup_seq[3],arg);
          break;
      case CHEAT_AUTO_MAP:
          DE_Cheat(cheat_powerup_seq[4],arg);
          break;
      case CHEAT_LITE_AMP_GOGGLES:
          DE_Cheat(cheat_powerup_seq[5],arg);
          break;
      case CHEAT_BEHOLD_MENU:
          DE_Cheat(cheat_powerup_seq[6],arg);
          break;
      case CHEAT_LEVEL_WARP:
          DE_Cheat(cheat_clev_seq,arg);
          break;
      case CHEAT_PLAYER_POSITION:
          DE_Cheat(cheat_mypos_seq,arg);
          break;
      case CHEAT_MAP_CHEAT:
          DE_Cheat(cheat_amap_seq,arg);
          break;
      case MISC_INITIAL_HEALTH:
          initial_health = atoi(arg);
          break;
      case MISC_INITIAL_BULLETS:
          initial_bullets = atoi(arg);
          break;
      case MISC_MAX_HEALTH:
          MAXHEALTH = atoi(arg);
          break;
      case MISC_MAX_ARMOR:
          max_armor = atoi(arg);
          break;
      case MISC_GREEN_ARMOR_CLASS:
          green_armor_class = atoi(arg);
          break;
      case MISC_BLUE_ARMOR_CLASS:
          blue_armor_class = atoi(arg);
          break;
      case MISC_MAX_SOULSPHERE:
          maxsoul = atoi(arg);
          break;
      case MISC_SOULSPHERE_HEALTH:
          soul_health = atoi(arg);
          break;
      case MISC_MEGASPHERE_HEALTH:
          mega_health = atoi(arg);
          break;
      case MISC_GOD_MODE_HEALTH:
          god_health = atoi(arg);
          break;
      case MISC_IDFA_ARMOR:
          idfa_armor = atoi(arg);
          break;
      case MISC_IDFA_ARMOR_CLASS:
          idfa_armor_class = atoi(arg);
          break;
      case MISC_IDKFA_ARMOR:
          idkfa_armor = atoi(arg);
          break;
      case MISC_IDKFA_ARMOR_CLASS:
          idfa_armor_class = atoi(arg);
          break;
      case MISC_BFG_CELLS_SHOT:
          BFGCELLS = atoi(arg);
          break;
      case SPRITE_OFFSET:
          sprnames[sprite]=oldsprnames[(atoi(arg)-151328)/8];

      case POINTER_CODEP_FRAME:
          states[frame].action = actions[atoi(arg)];

      default:
        return(0);
    }

    return(1);
}


void
DE_AddDeh(char * name)
{
  FILE *fp;
  char text[255];

  printf("DE_AddDeh: Adding %s\n", name);

  if((fp = fopen(name,"r")))
  {
    int i;

    /* Backup all action pointer */
    for(i=0;i<NUMSTATES;i++)
        actions[i] = states[i].action;

    /* Backup all sprite name pointer */
    for(i=0;i<NUMSPRITES;i++)
        oldsprnames[i] = sprnames[i];

    /* Start parsing the file */
    while(fgets(text, 255, fp))
    {
        int len1 = 0, len2 = 0;
        text[strlen(text)-1] = '\0';

        if(text[0] == '#') continue;
        if(text[0] == '\n') continue;

        /* Check for text or hack */
        if(sscanf(text, "Text %d %d", &len1, &len2) == 2)
          DE_Text(fp, len1, len2);
        else
          DE_Hack(text);
    }

    fclose(fp);
  }
  else
  {
    printf("Dehacked file '%s' not found\nPress ENTER to continue\n", name);
    getchar();
  }
}
