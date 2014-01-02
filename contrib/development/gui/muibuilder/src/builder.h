/***************************************************************************

  MUIBuilder - MUI interface builder
  Copyright (C) 1990-2009 by Eric Totel
  Copyright (C) 2010-2011 by MUIBuilder Open Source Team

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  MUIBuilder Support Site: http://sourceforge.net/projects/muibuilder/

  $Id$

***************************************************************************/

#ifndef BUILDER2_H
#define BUILDER2_H

/* MUI */
#include <libraries/mui.h>

/* Prototypes */
#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/asl.h>
#include <proto/icon.h>
#include <proto/dos.h>
#include <proto/gadtools.h>

/* ANSI C */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

/* locale */
#include "MUIBuilder_cat.h"

#include "SDI_compiler.h"
#include "SDI_hook.h"

#include "queue.h"
#include "file.h"
#include "grouparea.h"

#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif

/* message IDs */
#define ID_INFO         1
#define ID_END          2
#define ID_NEWWIN       3
#define ID_OKWIN        4
#define ID_NEWGRP       5
#define ID_TEST         6
#define ID_FINTEST      7
#define ID_NEWOBJ       8
#define ID_KEYBUTTON    9
#define ID_LISTVIEW     10
#define ID_STRING       11
#define ID_GROUP        12
#define ID_DBCLICK      13
#define ID_DBCLICK2     14
#define ID_DELETE       15
#define ID_DELETEGRP    16
#define ID_GENERATE     17
#define ID_GAUGE        18
#define ID_DOWN         19
#define ID_UP           20
#define ID_CYCLE        21
#define ID_GENERATEWIN  22
#define ID_RADIO        23
#define ID_LABEL        30
#define ID_SPACE        31
#define ID_SAVE         32
#define ID_LOAD         33
#define ID_CHECK        34
#define ID_EDIT         35
#define ID_CONFIG       36
#define ID_SCALE        37
#define ID_IMAGE        38
#define ID_SLIDER       39
#define ID_TEXT         40
#define ID_PROPG        41
#define ID_DIRLIST      42
#define ID_TOTMP        43
#define ID_TOCHILD      44
#define ID_TMPRM        45
#define ID_CATALOG      46
#define ID_POPUP1       47
#define ID_POPUP2       48
#define ID_NOTIFY       49
#define ID_EVT          50
#define ID_ACTION       51
#define ID_COPY         52
#define ID_COPYCHILD    53
#define ID_COPYTMP      54
#define ID_SEENOTIFY    55
#define ID_RECTANGLE    56
#define ID_COLORFIELD   57
#define ID_POPASL       58
#define ID_POPOBJECT    59
#define ID_DEPLIE       60
#define ID_MERGE        61
#define ID_FOLD         62
#define ID_UNFOLD       63
#define ID_UNDO         64
#define ID_CLOSE        65
#define ID_OPEN         66
#define ID_POPUP3       67
#define ID_POPUP4       68
#define ID_POPUP5       69
#define ID_NEW_MENUTITLE 70
#define ID_NEW_SUBMENU  71
#define ID_NEW_MENUITEM 72
#define ID_VALIDATE     73
#define ID_UPDATE       74
#define ID_NEWBARLABEL  75
#define ID_NEWAPPLI     76
#define ID_SAVEAS       77
#define ID_APPLI        78
#define ID_GUIDE        79

/* types of classes */
#define TY_APPLI        0
#define TY_WINDOW       1
#define TY_GROUP        2
#define TY_KEYBUTTON    3
#define TY_STRING       4
#define TY_LISTVIEW     5
#define TY_GAUGE        6
#define TY_CYCLE        7
#define TY_RADIO        8
#define TY_LABEL        9
#define TY_SPACE        10
#define TY_CHECK        11
#define TY_SCALE        12
#define TY_IMAGE        13
#define TY_SLIDER       14
#define TY_TEXT         15
#define TY_PROP         16
#define TY_DIRLIST      17
#define TY_RECTANGLE    18
#define TY_COLORFIELD   19
#define TY_POPASL       20
#define TY_POPOBJECT    21
#define TY_MENU         22
#define TY_SUBMENU      23
#define TY_MENUITEM     24

#define GR_ROOT         1
#define GR_NOROOT       2


/* variables types; must be in sync with libraries/muibuilder.h */
#define TYPEVAR_BOOL            1
#define TYPEVAR_INT             2
#define TYPEVAR_STRING          3
#define TYPEVAR_TABSTRING       4
#define TYPEVAR_PTR             5
#define TYPEVAR_HOOK            6
#define TYPEVAR_IDENT           7
#define TYPEVAR_EXTERNAL        8
#define TYPEVAR_LOCAL_PTR       9
#define TYPEVAR_EXTERNAL_PTR    10

/* types of code; must be in sync with libraries/muibuilder.h */
#define TC_CREATEOBJ            1
#define TC_ATTRIBUT             2
#define TC_END                  3
#define TC_FUNCTION             4
#define TC_STRING               5
#define TC_INTEGER              6
#define TC_CHAR                 7
#define TC_VAR_AFFECT           8
#define TC_VAR_ARG              9
#define TC_END_FUNCTION         10
#define TC_BOOL                 11
#define TC_MUIARG               12
#define TC_OBJFUNCTION          13
#define TC_LOCALESTRING         14
#define TC_EXTERNAL_CONSTANT    15
#define TC_EXTERNAL_FUNCTION    16
#define TC_MUIARG_ATTRIBUT      17
#define TC_MUIARG_FUNCTION      18
#define TC_MUIARG_OBJ           19
#define TC_BEGIN_NOTIFICATION   20
#define TC_END_NOTIFICATION     21
#define TC_EXTERNAL_VARIABLE    22
#define TC_MUIARG_OBJFUNCTION   23
#define TC_OBJ_ARG              24
#define TC_LOCALECHAR           25

/* structures */
typedef struct help1
{
    int nb_char;
    char title[80];
    APTR content;
    BOOL generated;
} help;

typedef struct conf1
{
    BOOL phantom;
    BOOL request;
    char langage[512];
    char editor[128];
    BOOL icons;
    BOOL depth;
    char tree_char;
    int columns;
    BOOL labels;
} conf;

typedef struct area1
{
    BOOL Hide;
    BOOL Disable;
    BOOL InputMode;
    BOOL Phantom;
    int Weight;
    unsigned Background;
    int Frame;
    char key;
    char TitleFrame[80];
} area;

typedef struct menu1
{
    int id;
    char label[80];
    APTR father;
    BOOL generated;
    help Help;
    queue *notify;
    queue *notifysource;
    APTR muiobj;
    char name[80];
    char key;
    BOOL menu_enable;
    BOOL check_enable;
    BOOL check_state;
    BOOL Toggle;
    queue *childs;
    BOOL fold;
} menu;

typedef struct appli1
{
    int id;
    char label[80];
    APTR father;
    BOOL generated;
    help Help;
    queue *notify;
    queue *notifysource;
    APTR muiobj;
    char author[80];
    char base[80];
    char title[80];
    char version[80];
    char copyright[80];
    char description[80];
    char helpfile[256];
    queue *Idents;
    queue *Functions;
    queue *Constants;
    queue *Variables;
    menu *appmenu;
} appli;

typedef struct bouton1
{
    int id;
    char label[80];
    APTR father;
    BOOL generated;
    help Help;
    queue *notify;
    queue *notifysource;
    APTR muiobj;
    area Area;
    char title[80];
} bouton;

typedef struct object1
{
    int id;
    char label[80];
    APTR father;
    BOOL generated;
    help Help;
    queue *notify;
    queue *notifysource;
    APTR muiobj;
} object;

typedef struct event1
{
    object *destination;        /* vers quel objet ? */
    char label_cible[80];       /* nom objet cible   */
    int id_cible;               /* son type ?        */
    char description[120];      /* la description de la notification */
    int src_type;               /* type d'evenement  */
    int dest_type;              /* type d'action     */
    APTR arguments;             /* un ptr sur des arguments */
    char argstring[80];         /* un argument sous forme de chaine */
} event;

typedef struct text1
{
    int id;
    char label[80];
    APTR father;
    BOOL generated;
    help Help;
    queue *notify;
    queue *notifysource;
    APTR muiobj;
    area Area;
    BOOL max;
    BOOL min;
    char content[256];
    char preparse[80];
    char testpreparse[80];
} texte;

typedef struct space1
{
    int id;
    char label[80];
    APTR father;
    BOOL generated;
    help Help;
    queue *notify;
    queue *notifysource;
    APTR muiobj;
    int type;
    int spacing;
} space;

typedef struct gauge1
{
    int id;
    char label[80];
    APTR father;
    BOOL generated;
    help Help;
    queue *notify;
    queue *notifysource;
    APTR muiobj;
    area Area;
    int divide;
    BOOL horizontal;
    BOOL fixheight;
    BOOL fixwidth;
    int max;
    int height;
    int width;
    char infotext[80];
} gauge;

typedef struct prop1
{
    int id;
    char label[80];
    APTR father;
    BOOL generated;
    help Help;
    queue *notify;
    queue *notifysource;
    APTR muiobj;
    area Area;
    BOOL horizontal;
    int entries;
    int first;
    int visible;
    BOOL fixheight;
    BOOL fixwidth;
    int height;
    int width;
} prop;

typedef struct group1
{
    int id;
    char label[80];
    APTR father;
    BOOL generated;
    help Help;
    queue *notify;
    queue *notifysource;
    APTR muiobj;
    area Area;
    BOOL root;
    BOOL horizontal;
    BOOL registermode;
    BOOL sameheight;
    BOOL samesize;
    BOOL samewidth;
    BOOL virtual;
    BOOL columns;
    BOOL rows;
    BOOL horizspacing;
    BOOL vertspacing;
    int number;
    queue *child;
    int horizspace;
    int vertspace;
    queue *entries;
    char *registertitles[200];
    BOOL deplie;
} group;

typedef struct listview1
{
    int id;
    char label[80];
    APTR father;
    BOOL generated;
    help Help;
    queue *notify;
    queue *notifysource;
    APTR muiobj;
    area Area;
    char title[80];
    int type;
    int select;
    BOOL multiselect;
    BOOL doubleclick;
    BOOL adjustheight;
    BOOL adjustwidth;
    BOOL inputmode;
    char *comparehook;
    char *constructhook;
    char *displayhook;
    char *destructhook;
    char *multitesthook;
    char format[80];
    char content[256];
} listview;

typedef struct dirlist1
{
    int id;
    char label[80];
    APTR father;
    BOOL generated;
    help Help;
    queue *notify;
    queue *notifysource;
    APTR muiobj;
    area Area;
    int weight;
    BOOL drawers;
    BOOL files;
    BOOL filter;
    BOOL multi;
    BOOL icons;
    BOOL highlow;
    int sorttype;
    char accept[80];
    char directory[80];
    char reject[80];
    char *filterhook;
    int sortdirs;
} dirlist;

typedef struct check1
{
    int id;
    char label[80];
    APTR father;
    BOOL generated;
    help Help;
    queue *notify;
    queue *notifysource;
    APTR muiobj;
    area Area;
    char title[80];
    BOOL title_exist;
    BOOL init_state;
} check;

typedef struct slider1
{
    int id;
    char label[80];
    APTR father;
    BOOL generated;
    help Help;
    queue *notify;
    queue *notifysource;
    APTR muiobj;
    area Area;
    char title[80];
    BOOL title_exist;
    int max;
    int min;
    BOOL quiet;
    int init;
    BOOL reverse;
    BOOL horizontal;
} slider;

typedef struct cycle1
{
    int id;
    char label[80];
    APTR father;
    BOOL generated;
    help Help;
    queue *notify;
    queue *notifysource;
    APTR muiobj;
    area Area;
    char title[80];
    queue *entries;
    char *STRA_cycle[200];
} cycle;

typedef struct radio1
{
    int id;
    char label[80];
    APTR father;
    BOOL generated;
    help Help;
    queue *notify;
    queue *notifysource;
    APTR muiobj;
    area Area;
    queue *entries;
    char *STRA_radio[200];
} radio;

typedef struct chaine1
{
    int id;
    char label[80];
    APTR father;
    BOOL generated;
    help Help;
    queue *notify;
    queue *notifysource;
    APTR muiobj;
    area Area;
    char title[80];
    char content[80];
    BOOL title_exist;
    char accept[80];
    char reject[80];
    int format;
    BOOL integer;
    int maxlen;
    BOOL secret;
} chaine;

typedef struct label1
{
    int id;
    char label[80];
    APTR father;
    BOOL generated;
    help Help;
    queue *notify;
    queue *notifysource;
    APTR muiobj;
    area Area;
    char title[80];
} label;

typedef struct scale1
{
    int id;
    char label[80];
    APTR father;
    BOOL generated;
    help Help;
    queue *notify;
    queue *notifysource;
    APTR muiobj;
    area Area;
    BOOL horiz;
} scale;

typedef struct image1
{
    int id;
    char label[80];
    APTR father;
    BOOL generated;
    help Help;
    queue *notify;
    queue *notifysource;
    APTR muiobj;
    area Area;
    int type;
    BOOL freevert;
    BOOL freehoriz;
    BOOL fixheight;
    BOOL fixwidth;
    int height;
    int width;
    char spec[128];
} image;

typedef struct window1
{
    int id;
    char label[80];
    APTR father;
    BOOL generated;
    help Help;
    queue *notify;
    queue *notifysource;
    APTR muiobj;
    char title[80];
    group root;
    queue *groups;
    BOOL appwindow;
    BOOL backdrop;
    BOOL borderless;
    BOOL closegadget;
    BOOL depthgadget;
    BOOL dragbar;
    BOOL sizegadget;
    BOOL initopen;
    BOOL deplie;
    queue *chain;
    menu *menu;
    APTR muichain;
    BOOL nomenu;
    BOOL needmouse;
} window;

typedef struct rectangle1
{
    int id;
    char label[80];
    APTR father;
    BOOL generated;
    help Help;
    queue *notify;
    queue *notifysource;
    APTR muiobj;
    area Area;
    int type;
    BOOL fixheight;
    BOOL fixwidth;
    int height;
    int width;
} rectangle;

typedef struct colorfield1
{
    int id;
    char label[80];
    APTR father;
    BOOL generated;
    help Help;
    queue *notify;
    queue *notifysource;
    APTR muiobj;
    area Area;
    BOOL fixheight;
    BOOL fixwidth;
    int height;
    int width;
    BOOL frame;
    ULONG red;
    ULONG green;
    ULONG blue;
} colorfield;

typedef struct popaslobj1
{
    int id;
    char label[80];
    APTR father;
    BOOL generated;
    help Help;
    queue *notify;
    queue *notifysource;
    APTR muiobj;
    area Area;
    char *starthook;
    char *stophook;
    int type;
    int image;
} popaslobj;

typedef struct popobject1
{
    int id;
    char label[80];
    APTR father;
    BOOL generated;
    help Help;
    queue *notify;
    queue *notifysource;
    APTR muiobj;
    area Area;
    char *openhook;
    char *closehook;
    object *obj;
    int image;
    BOOL follow;
    BOOL light;
    BOOL Volatile;
} popobject;


/* global functions */
extern CONST_STRPTR GetStr(APTR);
extern BOOL NewGroup(group *, int, BOOL);
extern BOOL NewWindow(window *, BOOL, object *);
extern APTR NewChild(APTR, BOOL);
extern BOOL NewButton(bouton *, BOOL);
extern BOOL NewListView(listview *, BOOL);
extern BOOL NewString(chaine *, BOOL);
extern BOOL NewGauge(gauge *, BOOL);
extern BOOL NewCycle(cycle *, BOOL);
extern BOOL NewRadio(radio *, BOOL);
extern BOOL NewLabel(label *, BOOL);
extern BOOL NewSpace(space *, BOOL);
extern BOOL NewCheckMark(check *, BOOL);
extern BOOL NewScale(scale *, BOOL);
extern BOOL NewImage(image *, BOOL);
extern BOOL NewSlider(slider *, BOOL);
extern BOOL NewText(texte *, BOOL);
extern BOOL NewProp(prop *, BOOL);
extern BOOL NewDirList(dirlist *, BOOL);
extern BOOL NewRectangle(rectangle *, BOOL);
extern BOOL NewColorField(colorfield *, BOOL);
extern BOOL NewPopAsl(popaslobj *, BOOL);
extern BOOL NewPopObject(popobject *, BOOL);
extern BOOL NewMenu(menu *, BOOL);
extern void TestWindow(window *);
extern APTR CreateWindow(APTR);
extern void DeleteObject(APTR);
extern void DeleteLabel(queue *, APTR);
extern void ErrorMessage(CONST_STRPTR);
extern void CodeCreate(object *, FILE *);
extern void GenerateCode(queue *);
extern void GenerateCodeObject(APTR);
extern void EXIT_PRG(void);
extern void SaveObject(APTR, FILE *);
extern void SaveApplication(BOOL);
extern void LoadApplication(void);
extern APTR LoadObject(FILE *, APTR, int);
extern void LoadFile(char *);
extern BOOL RequestMessage(CONST_STRPTR);
extern void Code(void);
extern void GenerateLabels(APTR, FILE *);
extern void Guide(void);
extern char GetMUIBuilderChar(int);
extern void CreateCatalog(void);
extern BOOL LoadRequester(CONST_STRPTR, char *, char *);
extern BOOL SaveRequester(CONST_STRPTR, char *, char *);
extern void InitGroup(group *);
extern void InitListview(listview *);
extern void InitWindow(window *);
extern void InitAppli(void);
extern void InitButton(bouton *);
extern void InitString(chaine *);
extern void InitGauge(gauge *);
extern void InitCycle(cycle *);
extern void InitRadio(radio *);
extern void InitLabel(label *);
extern void InitSpace(space *);
extern void InitCheckMark(check *);
extern void InitScale(scale *);
extern void InitImage(image *);
extern void InitSlider(slider *);
extern void InitText(texte *);
extern void InitProp(prop *);
extern void InitDirList(dirlist *);
extern void InitRectangle(rectangle *);
extern void InitColorField(colorfield *);
extern void InitPopAsl(popaslobj *);
extern void InitPopObject(popobject *);
extern void InitMenu(menu *);
extern menu *CreateMenu(BOOL);
extern BOOL CreateNotify(object *, window *);
extern APTR Duplicate(APTR, APTR);
extern void CreateChain(object *, window *);
extern BOOL GetMUIarg(object *, APTR *, char *, queue *, BOOL,
                      CONST_STRPTR);
extern BOOL EditChild(object *);
extern void Config(void);
extern CONST_STRPTR GetMenuChar(int);
extern CONST_STRPTR GetMenuString(int);
void InitArea(area *);
void ValidateArea(struct ObjGR_AreaGroup *, area *);
extern struct ObjGR_AreaGroup *CreateGR_AreaGroup(area *, BOOL, BOOL, BOOL,
                                                  BOOL, BOOL);
extern void DisposeGR_AreaGroup(struct ObjGR_AreaGroup *);
extern APTR LitChaine(FILE *);
extern int ReadInt(FILE *);
extern void ReadHelp(FILE *, APTR);
extern void ReadNotify(FILE *, APTR);
extern void ReadArea(FILE *, area *);
extern void SaveArea(FILE *, area *);
extern void ReadFunctionHook(FILE *, char **);
extern queue *CopyStringsList(queue *);

extern APTR LoadWindow(FILE *, int);
extern void SaveWindow(FILE *, window *);
extern APTR LoadText(FILE *, APTR, int);
extern void SaveText(FILE *, texte *);
extern APTR LoadCycle(FILE *, APTR, int);
extern void SaveCycle(FILE *, cycle *);
extern APTR LoadRadio(FILE *, APTR, int);
extern void SaveRadio(FILE *, radio *);
extern APTR LoadGroup(FILE *, APTR, int);
extern void SaveGroup(FILE *, group *);
extern APTR LoadDirList(FILE *, APTR, int);
extern void SaveDirList(FILE *, dirlist *);
extern APTR LoadListview(FILE *, APTR, int);
extern void SaveListview(FILE *, listview *);
extern APTR LoadPopAsl(FILE *, APTR, int);
extern void SavePopAsl(FILE *, popaslobj *);
extern APTR LoadImage(FILE *, APTR, int);
extern void SaveImage(FILE *, image *);
extern APTR LoadMenu(FILE *, APTR, int, int);
extern void SaveMenu(FILE *, menu *);
extern void ErrorMessageEasy(CONST_STRPTR);
extern void CreateLabels(APTR, window *);
extern APTR PointerOnString(char *, queue *);
extern void LinkNotify(int);
extern void DeleteApplication(void);
extern window *ParentWindow(APTR obj);
extern BOOL IsMenuParent(menu *, menu *);
extern BOOL MakeTest;
extern void TestChain(window *);
extern BOOL CopyObject(object *, object *, int, APTR);
extern BOOL IsParent(object * obj, group * parent);
extern void ShowObject(APTR);
extern void EditNode(APTR);
extern menu *ParentMenu(menu *);
extern void CreateLocaleString(char *, char);
extern void TestNotify(object *);
extern void DeleteObjNotify(object *obj);
extern void DeleteSourceNotify(object *obj);
extern void CloseLocale(void);
extern void Load(APTR lv_window, char *file, BOOL request);
extern void MergeFile(void);
extern void InitLocale(void);
extern void ScanAvailableModules(void);
extern void InitNotifyArrays(void);


/* global variables */
extern char *PopModules[50];
extern conf config;
extern appli application;
extern window *current_window;
extern char genfile[512];
extern char catfile[512];
extern BOOL code, env, declarations, local, notifications, generate_all;
extern char real_getstring[100];
extern char catprepend[5];
extern char ImageDir[256];
extern char MBDir[512];
extern char locale_string[82];
extern char nospace[80];
extern APTR AppMenu;
extern int nb_modules;
extern APTR WI_current;
extern APTR app;
extern queue *windows;
extern queue *labels;
extern queue *tmp;
extern struct DiskObject *dobj;
extern int nb_window, nb_listview, nb_group, nb_button, nb_string;
extern int nb_gauge, nb_cycle, nb_radio, nb_label, nb_check, nb_image;
extern int nb_slider, nb_text, nb_prop, nb_rectangle, nb_colorfield;
extern int nb_popasl, nb_popobject, nb_menu, nb_space, nb_scale,
    nb_barlabel;
extern char savefile[256], savedir[256];
extern BOOL modify;
extern char get_string[80];

#endif
