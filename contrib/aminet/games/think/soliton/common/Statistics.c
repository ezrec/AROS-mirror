/* Soliton cardgames for Amiga computers
 * Copyright (C) 1997-2002 Kai Nickel and other authors
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <string.h>   /* strcpy */
#include <time.h>

#include <clib/alib_protos.h>
#include <exec/memory.h>
#include <libraries/iffparse.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/utility.h>

#include "CSolitaire.h"
#include "Locales.h"
#include "ProfileManager.h" /* getlin */
#include "Statistics.h"
#include "Soliton.h"

#include "IMG_Statistics.c"
#include "IMG_Winner.c"

#define MAXSCORES 30        /* maximum number of score entries */

struct Score
{
  char  name[20];
  long  points;
  long  time;
  char  date[20];
};

struct Statistics_Data
{
  Object  *TX_Time,                   // this game
  
          *TX_Time_Session, *TX_Wins_Session, *TX_Losses_Session, *TX_Quote_Session,  // this session
          *TX_Time_All    , *TX_Wins_All    , *TX_Losses_All    , *TX_Quote_All,      // all sessions

          *GR_Normal, *GR_Winner, *GR_Group, *GR_Mainpage,
          *LI_Highscores, *TX_Hightitle, *ST_Enter, *GR_Enter, *TX_Enter;

  char    scoresdate[20];       // Datum seit Beginn der Highscoreliste
  int     won_all, lost_all, won_session, lost_session;
  long    time_all, time_session, time_game;
  int     unfinished_game;
  Object *TX_Score;
  Object *TX_Turns;
  int     turn_game; /* Klondike */
  int     score_game;
};

static void SetDate(struct Statistics_Data *data, BOOL reset)
{
  char highscoretitle[40];

  if(reset)
  {
    time_t t = time(0);
    strftime(data->scoresdate, 20, "%Y-%m-%d", localtime(&t));
  }

  sprintf(highscoretitle, GetStr(MSG_STATS_DATE), data->scoresdate);
  setatt(data->TX_Hightitle, MUIA_Text_Contents, highscoretitle);
}

static const char HID[]   = "Soliton Statistics";
static const char FNAME[] = "PROGDIR:Soliton.scores";

static void SaveHighscores(struct Statistics_Data *data)
{
  BPTR f;
  LONG size;
  int i;

  data->won_all  += data->won_session;
  data->lost_all += data->lost_session + data->unfinished_game;
  data->time_all += data->time_session;

  if((f = Open(FNAME, MODE_NEWFILE)))
  {
    FPrintf(f, "%s\n%s\n", HID, data->scoresdate);
    Flush(f);

    size = xget(data->LI_Highscores, MUIA_List_Entries);
    Write(f, &size, sizeof(long));

    for(i = 0; i < size; i++)
    {
      struct Score* s;
      DoMethod(data->LI_Highscores, MUIM_List_GetEntry, i, &s);

      FPrintf(f, "%s\n%s\n", s->name, s->date);
      Flush(f);
      Write(f, &(s->points), sizeof(long));
      Write(f, &(s->time), sizeof(long));
    }

    Write(f, &data->won_all , sizeof(int));
    Write(f, &data->lost_all, sizeof(int));
    Write(f, &data->time_all, sizeof(long));

    Close(f);
  }
}

static void LoadHighscores(struct Statistics_Data *data)
{
  LONG size = -1;
  BPTR f;
  int i;

  data->won_all = 0;
  data->lost_all = 0;
  data->time_all = 0;

  if((f = Open(FNAME, MODE_OLDFILE))) 
  {
    char id[30];

    if(getlin(f, id, sizeof(id)) && !strcmp(id, HID))
    {
      if(getlin(f, data->scoresdate, sizeof(data->scoresdate)))
      {
        Flush(f);
        if(Read(f, &size, sizeof(long)) != sizeof(long))
          size = -1;

        for(i = 0; i < size; i++)
        {
          struct Score* s;

          if((s = (struct Score *) AllocVec(sizeof(struct Score), MEMF_PUBLIC)))
          {
            if(!getlin(f, s->name, sizeof(s->name)) ||
            !getlin(f, s->date, sizeof(s->date)))
              size = -2;
            Flush(f);
            if(Read(f, &(s->points), sizeof(long)) != sizeof(long))
              size = -2;
            else if(Read(f, &(s->time), sizeof(long)) != sizeof(long))
              size = -2;
            else
              DoMethod(data->LI_Highscores, MUIM_List_InsertSingle, s, MUIV_List_Insert_Bottom);
          }
        }

        if(size >= 0)
        {
          Read(f, &data->won_all, sizeof(int));
          Read(f, &data->lost_all, sizeof(int));
          Read(f, &data->time_all, sizeof(long));
        }
      }
    }
    Close(f);
  }

  SetDate(data, (size == -1));
}

static void updateDisplay(struct Statistics_Data *data)
{
  static char str[20];
  LONG s;
  int games;

  // this game
  sprintf(str, "%d", data->turn_game);
  setatt(data->TX_Turns, MUIA_Text_Contents, str);

  sprintf(str, "%d", data->score_game);
  setatt(data->TX_Score, MUIA_Text_Contents, str);

  s = data->time_game;
  sprintf(str, "%d:%02d min", (int)s / 60, (int)s % 60);
  setatt(data->TX_Time, MUIA_Text_Contents, str);

  // this session
  games = data->lost_session + data->won_session;
  sprintf(str, "%.0f %%", (games ? ((double)data->won_session / (double)games)*100.0 : 0.0));
  setatt(data->TX_Quote_Session, MUIA_Text_Contents, str);

  sprintf(str, "%d", (int)data->won_session);
  setatt(data->TX_Wins_Session, MUIA_Text_Contents, str);

  sprintf(str, "%d", (int)data->lost_session);
  setatt(data->TX_Losses_Session, MUIA_Text_Contents, str);

  s = data->time_session;
  sprintf(str, "%d:%02d h", (int)s / 3600, (int)(s % 3600) / 60);
  setatt(data->TX_Time_Session, MUIA_Text_Contents, str);

  // all session
  games += data->lost_all + data->won_all;
  sprintf(str, "%.0f %%", (games ? ((double)(data->won_session + data->won_all) / (double)games)*100.0 : 0.0));
  setatt(data->TX_Quote_All, MUIA_Text_Contents, str);

  sprintf(str, "%d", data->won_session + data->won_all);
  setatt(data->TX_Wins_All, MUIA_Text_Contents, str);

  sprintf(str, "%d", data->lost_session + data->lost_all);
  setatt(data->TX_Losses_All, MUIA_Text_Contents, str);

  s = data->time_session + data->time_all;
  sprintf(str, "%d:%02d h", (int)s / 3600, (int)(s % 3600) / 60);
  setatt(data->TX_Time_All, MUIA_Text_Contents, str);
}


/****************************************************************************************
  ClearScores()
****************************************************************************************/

static IPTR Statistics_ClearScores(struct IClass* cl, Object* obj, Msg msg)
{
  struct Statistics_Data* data = (struct Statistics_Data*)INST_DATA(cl, obj);
  if ( MUI_RequestA(_app(obj), obj, 0, "Soliton",
       GetStr(MSG_REQ_GADGETS), GetStr(MSG_STATS_CLEARREQ), NULL) == 1)
  {
    data->won_all = 0;
    data->lost_all = 0;
    data->time_all = 0;
    SetDate(data, TRUE);
    DoMethod(data->LI_Highscores, MUIM_List_Clear);
    updateDisplay(data);
  }
  return FALSE;
}


/****************************************************************************************
  Enter()
****************************************************************************************/

static IPTR Statistics_Enter(struct IClass* cl, Object* obj, Msg msg)
{
  struct Statistics_Data* data = (struct Statistics_Data*)INST_DATA(cl, obj);
  struct Score* s;

  DoMethod(data->LI_Highscores, MUIM_List_GetEntry, MUIV_List_GetEntry_Active, &s);
  strncpy(s->name, (char*)xget(data->ST_Enter, MUIA_String_Contents), sizeof(s->name));
  DoMethod(data->LI_Highscores, MUIM_List_Redraw, MUIV_List_Redraw_Active);

  return FALSE;
}


/******************************************************************************
  DisplayFunc / CompareFunc / DestructFunc
******************************************************************************/

HOOKPROTONH(Highscore_DisplayFunc, LONG, char** array, struct Score* entry)
{
  static char buf1[25];
  static char buf2[25];
  static char buf3[25];
  static char buf4[25];
  if(entry)
  {
    sprintf(buf1, "%d", (int)entry->points);
    sprintf(buf3, "%d:%02d min", (int)entry->time/60, (int)entry->time % 60);
    *array++ = entry->name;
    *array++ = buf1;
    *array++ = buf3;
    *array   = entry->date;
  }
  else /* title */
  {
    sprintf(buf1  , "\033c\033b%s", GetStr(MSG_STATS_LNAME));
    *array++ = buf1;
    sprintf(buf2  , "\033c\033b%s", GetStr(MSG_STATS_LSCORE));
    *array++ = buf2;
    sprintf(buf3  , "\033c\033b%s", GetStr(MSG_STATS_TIME));
    *array++ = buf3;
    sprintf(buf4  , "\033c\033b%s", GetStr(MSG_STATS_LDATE));
    *array = buf4;
  }
  return 0;
}

HOOKPROTONH(Highscore_CompareFunc, LONG, struct Score* s2, struct Score* s1)
{
  if (s1->points < s2->points) return 1;
  if (s1->points > s2->points) return -1;
  return 0;
}

HOOKPROTONHNO(Highscore_DestructFunc, LONG, struct Score* entry)
{
  FreeVec(entry);
  return 0;
}

MakeStaticHook(DispHook, Highscore_DisplayFunc);
MakeStaticHook(CompHook, Highscore_CompareFunc);
MakeStaticHook(DestHook, Highscore_DestructFunc);

/****************************************************************************************
  New / Dispose
****************************************************************************************/

static IPTR Statistics_New(struct IClass* cl, Object* obj, struct opSet* msg)
{
  Object *BT_Close, *BT_Clear;
  struct Statistics_Data tmp;

  static char* PG_Pages[4];
  PG_Pages[0] = GetStr(MSG_STATS_SESSIONS);
  PG_Pages[1] = GetStr(MSG_STATS_THISGAME);
  PG_Pages[2] = GetStr(MSG_STATS_HIGHSCORES);
  PG_Pages[3] = NULL;

  obj = (Object*)DoSuperNew(cl, obj,
    MUIA_Window_Title      , GetStr(MSG_STATS_TITLE),
    MUIA_Window_ID         , MAKE_ID('S','K','S','T'),
    MUIA_HelpNode          , "STATISTICS",
    WindowContents, VGroup,

      /*
      **
      **  Header
      **
      */

      Child, tmp.GR_Group = VGroup,
        Child, tmp.GR_Normal = HGroup,
          Child, MakeImage(IMG_Statistics_body, 
                           IMG_STATISTICS_WIDTH, IMG_STATISTICS_HEIGHT, 
                           IMG_STATISTICS_DEPTH, IMG_STATISTICS_COMPRESSION, 
                           IMG_STATISTICS_MASKING, IMG_Statistics_colors),
          Child, VGroup,
            Child, TextObject,
              MUIA_Text_Contents, GetStr(MSG_STATS_TOPIC),
              MUIA_Text_SetMin  , TRUE,
              MUIA_Font         , MUIV_Font_Big,
              End,
            Child, tmp.TX_Hightitle = TextObject,
              MUIA_Text_SetMin  , TRUE,
              MUIA_Font         , MUIV_Font_Tiny,
              MUIA_Text_PreParse, "\33b",
              End,
            End,
          Child, HVSpace,
          Child, VGroup,
            Child, HVSpace,
            Child, BT_Clear = MakeButton(MSG_STATS_CLEARSCORES, MSG_STATS_CLEARSCORES_HELP),
            End,
          End,
        End,  // Header

      /*
      **
      **  Pages
      **
      */

      Child, tmp.GR_Mainpage = RegisterGroup(PG_Pages),
        MUIA_Register_Frame, TRUE,

        /*
        **  Sessions
        */

          Child, ColGroup(3), 

            Child, HVSpace,

            Child, MUI_MakeObject(MUIO_Label, GetStr(MSG_STATS_THISSESSION), MUIO_Label_Centered|MUIO_Label_DoubleFrame),
            Child, MUI_MakeObject(MUIO_Label, GetStr(MSG_STATS_ALLSESSION ), MUIO_Label_Centered|MUIO_Label_DoubleFrame),

            Child, MakeLabel2(MSG_STATS_WINS),
            Child, tmp.TX_Wins_Session = TextObject, TextFrame,
              MUIA_ShortHelp, GetStr(MSG_STATS_WINS_HELP),
              End,
            Child, tmp.TX_Wins_All = TextObject, TextFrame,
              MUIA_ShortHelp, GetStr(MSG_STATS_WINS_HELP),
              End,

            Child, MakeLabel2(MSG_STATS_LOSSES),
            Child, tmp.TX_Losses_Session = TextObject, TextFrame,
              MUIA_ShortHelp, GetStr(MSG_STATS_LOSSES_HELP),
              End,
            Child, tmp.TX_Losses_All = TextObject, TextFrame,
              MUIA_ShortHelp, GetStr(MSG_STATS_LOSSES_HELP),
              End,

            Child, MakeLabel2(MSG_STATS_QUOTA),
            Child, tmp.TX_Quote_Session = TextObject, TextFrame,
              MUIA_ShortHelp, GetStr(MSG_STATS_QUOTA_HELP),
              End,
            Child, tmp.TX_Quote_All = TextObject, TextFrame,
              MUIA_ShortHelp, GetStr(MSG_STATS_QUOTA_HELP),
              End,

            Child, MakeLabel2(MSG_STATS_TIME),
            Child, tmp.TX_Time_Session = TextObject, TextFrame,
              MUIA_ShortHelp, GetStr(MSG_STATS_TIME_HELP),
              End,
            Child, tmp.TX_Time_All = TextObject, TextFrame,
              MUIA_ShortHelp, GetStr(MSG_STATS_TIME_HELP),
              End,

            End,

        /*
        **  This game
        */

            Child, ColGroup(2), 
              Child, MakeLabel2(MSG_STATS_SCORE),
              Child, tmp.TX_Score = TextObject, TextFrame,
                MUIA_ShortHelp, GetStr(MSG_STATS_SCORE_HELP),
                End,
  
              Child, MakeLabel2(MSG_STATS_TURNS),
              Child, tmp.TX_Turns = TextObject, TextFrame,
                MUIA_ShortHelp, GetStr(MSG_STATS_TURNS_HELP),
                End,

              Child, MakeLabel2(MSG_STATS_TIME),
              Child, tmp.TX_Time = TextObject, TextFrame,
                MUIA_ShortHelp, GetStr(MSG_STATS_TIME_HELP),
                End,

              End,


        /*
        **  Highscores
        */

        Child, VGroup,

          Child, ListviewObject,
            MUIA_Listview_Input, FALSE,
            MUIA_Listview_List , tmp.LI_Highscores = ListObject,
              ReadListFrame,
              MUIA_List_Format     , "D=8 MAW=40 BAR,D=8 MAW=20 P=\033r BAR,MAW=20 P=\033r BAR,MAW=20 P=\033c",
              MUIA_List_Title      , TRUE,
              MUIA_List_AutoVisible, TRUE,
              End,
            End,

          // Eingabepage

          Child, tmp.GR_Enter = VGroup,
            MUIA_Group_PageMode, TRUE,

            Child, HVSpace,

            Child, VGroup, 
              Child, tmp.TX_Enter = TextObject,
                MUIA_Text_Contents, GetStr(MSG_STATS_ENTERNAME),
                MUIA_Text_PreParse, "\033c",
                MUIA_Text_SetMin  , TRUE,
                End,
              Child, tmp.ST_Enter = MakeString(19, 0, "", 0),
              End,

            End,  // Eingabepage

          End,  // Highscores

        End,  // Pages

      Child, HGroup,
        Child, HVSpace,
        Child, BT_Close = MakeButton(MSG_STATS_CLOSE, 0),
        Child, HVSpace,
        End,

      End,  // Rootgroup

    TAG_MORE, msg->ops_AttrList);

  if (obj)
  {
    DoMethod(BT_Close, MUIM_Notify, MUIA_Pressed            , FALSE, obj, 3, MUIM_Set, MUIA_Window_Open, FALSE);
    DoMethod(obj     , MUIM_Notify, MUIA_Window_CloseRequest, TRUE , obj, 3, MUIM_Set, MUIA_Window_Open, FALSE);

    DoMethod(BT_Close, MUIM_Notify, MUIA_Pressed, FALSE, 
             MUIV_Notify_Application, 2, MUIM_Application_ReturnID, MUIV_Statistics_Close);
    DoMethod(obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, 
             MUIV_Notify_Application, 2, MUIM_Application_ReturnID, MUIV_Statistics_Close);

    DoMethod(BT_Clear, MUIM_Notify, MUIA_Pressed            , FALSE, obj, 1, MUIM_Statistics_ClearScores);
    DoMethod(tmp.ST_Enter, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, obj, 1, MUIM_Statistics_Enter);

    tmp.GR_Winner = HGroup, //GroupFrame,
        Child, MakeImage(IMG_Winner_body, IMG_WINNER_WIDTH, IMG_WINNER_HEIGHT, 
                         IMG_WINNER_DEPTH, IMG_WINNER_COMPRESSION, IMG_WINNER_MASKING, 
                         IMG_Winner_colors),
        Child, VGroup,
          Child, TextObject,
            MUIA_Text_Contents, GetStr(MSG_WINNER_TITLE),
            MUIA_Text_PreParse, "\033c",
            MUIA_Text_SetMin  , TRUE,
            MUIA_Font         , MUIV_Font_Big,
            End,
          Child, TextObject,
            MUIA_Text_Contents, GetStr(MSG_WINNER_TEXT),
            MUIA_Text_PreParse, "\033c",
            MUIA_Text_SetMin  , TRUE,
            End,
          End,
        Child, MakeImage(IMG_Winner_body, IMG_WINNER_WIDTH, IMG_WINNER_HEIGHT, 
                         IMG_WINNER_DEPTH, IMG_WINNER_COMPRESSION, IMG_WINNER_MASKING, 
                         IMG_Winner_colors),
        End;

    setatt(tmp.LI_Highscores, MUIA_List_DisplayHook, &DispHook);
    setatt(tmp.LI_Highscores, MUIA_List_CompareHook, &CompHook);
    setatt(tmp.LI_Highscores, MUIA_List_DestructHook, &DestHook);

    LoadHighscores(&tmp);
    tmp.won_session     = 0;
    tmp.lost_session    = 0;
    tmp.time_session    = 0;
    tmp.turn_game       = 0;
    tmp.score_game      = 0;
    tmp.time_game       = 0;
    tmp.unfinished_game = 0;
    updateDisplay(&tmp);

    *((struct Statistics_Data*)INST_DATA(cl, obj)) = tmp;

    return((IPTR)obj);
  }
  return 0;
}

static IPTR Statistics_Dispose(struct IClass* cl, Object* obj, Msg msg)
{
  struct Statistics_Data* data = (struct Statistics_Data*)INST_DATA(cl,obj);
  SaveHighscores(data);
  return DoSuperMethodA(cl, obj, msg);
}


/****************************************************************************************
  Set
****************************************************************************************/

static IPTR Statistics_Set(struct IClass* cl, Object* obj, struct opSet* msg)
{
  struct Statistics_Data* data = (struct Statistics_Data*)INST_DATA(cl, obj);
  struct TagItem *tag;

  /*
  **  MUIA_Soliton_Stats
  */

  if((tag = FindTagItem(MUIA_Soliton_Stats, msg->ops_AttrList)))
  {
    struct Stats * stats = (struct Stats*)(tag->ti_Data);

    data->won_session  = stats->all_won;
    data->lost_session = stats->all_lost;
    data->time_session = stats->all_seconds;
    data->time_game    = stats->game_seconds;
    data->score_game   = stats->game_score;
    data->turn_game    = stats->game_turns;

    if(stats->finished)
      data->unfinished_game = 0;
    else
      data->unfinished_game = 1;

    updateDisplay(data);
    return TRUE;
  }

  /*
  **  MUIA_Statistics_Winner
  */

  if((tag = FindTagItem(MUIA_Statistics_Winner, msg->ops_AttrList)))
  {
    if(DoMethod(data->GR_Group, MUIM_Group_InitChange))
    {
      if(tag->ti_Data)
      {
        LONG pos;
        struct Score *s;

        DoMethod(data->GR_Group, OM_ADDMEMBER, data->GR_Winner);
        DoMethod(data->GR_Group, OM_REMMEMBER, data->GR_Normal);

        pos = xget(data->LI_Highscores, MUIA_List_Entries);
        DoMethod(data->LI_Highscores, MUIM_List_GetEntry, pos, &s);

        if(!s || s->points <= data->score_game)
        {
          /*
          **  In Highscores aufnehmen
          */

          if (pos >= MAXSCORES) // ggf. Liste kürzen
            DoMethod(data->LI_Highscores, MUIM_List_Remove, MUIV_List_Remove_Last);

          setatt(data->GR_Mainpage, MUIA_Group_ActivePage, 2);

          if((s = AllocVec(sizeof(struct Score), MEMF_PUBLIC)))
          {
            char* name;
            time_t t;

            if((name = (char*)xget(data->ST_Enter, MUIA_String_Contents)))
              strcpy(s->name, name);
            else
              strcpy(s->name, "");
            t = time(0);
            strftime(s->date, 20, "%Y-%m-%d", localtime(&t));

            s->points = data->score_game;
            s->time = data->time_game;

            DoMethod(data->LI_Highscores, MUIM_List_InsertSingle, s, MUIV_List_Insert_Sorted);
            pos = xget(data->LI_Highscores, MUIA_List_InsertPosition);
            setatt(data->LI_Highscores, MUIA_List_Active, pos);
            DoMethod(data->LI_Highscores, MUIM_List_Select, pos, MUIV_List_Select_On, NULL);
            setatt(data->LI_Highscores, MUIM_List_Jump  , pos);

            setatt(data->GR_Enter, MUIA_Group_ActivePage, 1);
            setatt(obj, MUIA_Window_ActiveObject, data->ST_Enter);
          }
        } // In Highscores aufnehmen
      }
      else
      {
        DoMethod(data->GR_Group, OM_ADDMEMBER, data->GR_Normal);
        DoMethod(data->GR_Group, OM_REMMEMBER, data->GR_Winner);

        setatt(data->GR_Enter, MUIA_Group_ActivePage, 0);
        DoMethod(data->LI_Highscores, MUIM_List_Select, MUIV_List_Select_All, MUIV_List_Select_Off, NULL);
      }
      DoMethod(data->GR_Group, MUIM_Group_ExitChange);

      setatt(obj, MUIA_Window_ActiveObject, data->ST_Enter);  // unschön
    }
    return TRUE;
  }
  return DoSuperMethodA(cl, obj, (Msg)msg);
}

/****************************************************************************************
  Dispatcher / Init / Exit
****************************************************************************************/

DISPATCHER(Statistics_Dispatcher)
{
  switch(msg->MethodID)
  {
    case OM_NEW                     : return(Statistics_New        (cl, obj, (struct opSet*)msg));
    case OM_DISPOSE                 : return(Statistics_Dispose    (cl, obj, msg));
    case OM_SET                     : return(Statistics_Set        (cl, obj, (struct opSet*)msg));
    case MUIM_Statistics_ClearScores: return(Statistics_ClearScores(cl, obj, msg));
    case MUIM_Statistics_Enter      : return(Statistics_Enter      (cl, obj, msg));
  }
  return DoSuperMethodA(cl, obj, msg);
}

struct MUI_CustomClass *CL_Statistics = NULL;

BOOL Statistics_Init(void)
{
  if(!(CL_Statistics = MUI_CreateCustomClass(NULL, MUIC_Window, NULL,
  sizeof(struct Statistics_Data), Statistics_Dispatcher)))
  {
    ErrorReq(MSG_CREATE_STATISTICSCLASS);
    return FALSE;
  }
  return TRUE;
}

void Statistics_Exit(void)
{
  if(CL_Statistics)
    MUI_DeleteCustomClass(CL_Statistics);
}

