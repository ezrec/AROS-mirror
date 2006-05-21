/*
** $Id$
** Siamiga library
** See Copyright Notice in lua.h
*/

#define DEBUG 0
#include <aros/debug.h>

#include <stdlib.h>
#include <string.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/asl.h>
#include <proto/dos.h>
#include <proto/datatypes.h>
#include <proto/gadtools.h>

#include <datatypes/pictureclass.h>
#include <clib/alib_protos.h>

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

struct IntuitionBase *IntuitionBase = NULL;
struct GfxBase *GfxBase = NULL;
struct Library *AslBase = NULL;
struct Library *DataTypesBase = NULL;
struct Library *GadToolsBase = NULL;

static struct Screen *screen = NULL;
static void *vinfo = NULL;
static struct TextAttr gadfont = { "topaz.font", 8, 0, 0 }; // default values, will be overwritten by screen's font

//*****************************************************************
//                            Siamga
//*****************************************************************

#define SIAMIGA "Siamiga"
#define SIGADGET "Sigadget"
#define MAXMENUITEMS (60)
#define MAXLABELS (15)

typedef struct Sigadget
{
    struct Sigadget *next;
    struct Gadget *gad;
    ULONG type;
    char *txt;
    STRPTR labels[MAXLABELS + 1]; // must be 0 terminated
    int label_cnt;
    struct Window *win;
} Sigadget;

typedef struct Siamiga
{
    struct Window *win;
    int left, top, width, height;  // initial window size
    char *title;
    int resizeable;
    int smart;

    Sigadget *gadgets;
    struct Gadget *glist, *gad;

    struct NewMenu *newmenu;
    int menunr;
    struct Menu *menustrip;
} Siamiga;

static Siamiga *toSiamiga(lua_State *L, int index)
{
    Siamiga *siam = lua_touserdata(L, index);
    if (siam == NULL) luaL_typerror(L, index, SIAMIGA);
    return siam;
}

static Siamiga *checkSiamiga(lua_State *L, int index)
{
    Siamiga *siam;
    luaL_checktype(L, index, LUA_TUSERDATA);
    siam = luaL_checkudata(L, index, SIAMIGA);
    if (siam == NULL) luaL_typerror(L, index, SIAMIGA);
    return siam;
}

static Siamiga *pushSiamiga(lua_State *L)
{
    Siamiga *siam = lua_newuserdata(L, sizeof(Siamiga));
    luaL_getmetatable(L, SIAMIGA);
    lua_setmetatable(L, -2);
    return siam;
}

#if 0
// disabled to get rid of warning, enable me when you need me
static Sigadget *toSigadget(lua_State *L, int index)
{
    Sigadget **psg = (Sigadget **)lua_touserdata(L, index);
    if (psg == NULL) luaL_typerror(L, index, SIGADGET);
    return *psg;
}
#endif

static Sigadget *checkSigadget(lua_State *L, int index)
{
    Sigadget **psg, *sg;
    luaL_checktype(L, index, LUA_TUSERDATA);
    psg = (Sigadget **)luaL_checkudata(L, index, SIGADGET);
    if (psg == NULL) luaL_typerror(L, index, SIGADGET);
    sg = *psg;
    return sg;
}

static Sigadget **pushSigadget(lua_State *L, Sigadget *sg)
{
    Sigadget **psg = (Sigadget **)lua_newuserdata(L, sizeof( Sigadget *));
    *psg = sg;
    luaL_getmetatable(L, SIGADGET);
    lua_setmetatable(L, -2);
    return psg;
}

static int siamiga_createwindow(lua_State *L)
{
    const char *title = lua_tostring(L, 1);
    int left = lua_tonumber(L, 2);
    int top = lua_tonumber(L, 3);
    int width = lua_tonumber(L, 4);
    int height = lua_tonumber(L, 5);
    int resizeable = lua_toboolean(L, 6);
    int smart = lua_toboolean(L, 7);

    // It crashes when window size is too small
    if ((width != 0) && (width < 100)) width = 100;
    if ((height != 0) && (height < 50)) height = 50;

    Siamiga *siam = pushSiamiga(L);
    siam->left = left;
    siam->top = top;
    siam->width = width;
    siam->height = height;
    siam->resizeable = resizeable;
    siam->smart = smart;
    siam->title = NULL;
    if (title)
    {
        siam->title = strdup(title);
        if (siam->title == NULL) luaL_error(L, "Can't allocate RAM for title string");
    }
    siam->win = NULL;

    siam->glist = NULL;
    siam->gad = CreateContext(&(siam->glist));
    siam->gadgets = NULL;

    siam->newmenu = NULL;
    siam->menunr = 0;
    siam->menustrip = NULL;
    return 1;
}

static int siamiga_openwindow(lua_State *L)
{
    Siamiga *siam = checkSiamiga(L, 1);
    if (siam->win == NULL)
    {
        siam->win = OpenWindowTags(NULL,
            WA_Left, siam->left,
            WA_Top, siam->top,
            siam->width  ? WA_Width : TAG_IGNORE, siam->width,
            siam->height ? WA_Height: TAG_IGNORE, siam->height,
            siam->title  ? WA_Title : TAG_IGNORE, (IPTR)siam->title,
            WA_PubScreen, (IPTR)screen,
            WA_DragBar, TRUE,
            WA_CloseGadget, TRUE,
            WA_DepthGadget, TRUE,
            WA_Activate, TRUE,
            siam->newmenu ? TAG_IGNORE : WA_RMBTrap, TRUE,
            siam->smart ? WA_SmartRefresh : WA_SimpleRefresh, TRUE,
            WA_GimmeZeroZero, TRUE,
            siam->resizeable ? WA_SizeGadget : TAG_IGNORE, TRUE,
            WA_MinWidth, 100,
            WA_MinHeight, 50,
            WA_MaxWidth, ~0,
            WA_MaxHeight, ~0,
            WA_IDCMP, IDCMP_CLOSEWINDOW | IDCMP_VANILLAKEY | IDCMP_MOUSEBUTTONS | IDCMP_NEWSIZE | IDCMP_REFRESHWINDOW |
                BUTTONIDCMP | CHECKBOXIDCMP | INTEGERIDCMP | SLIDERIDCMP | MXIDCMP | CYCLEIDCMP | STRINGIDCMP | IDCMP_MENUPICK ,
            WA_Gadgets, (IPTR)siam->glist,
            WA_NewLookMenus, TRUE,
            TAG_END);

        if (siam->win == NULL) luaL_error(L, "Can't open window");

        GT_RefreshWindow(siam->win, NULL);

        // create menu bar
        if (siam->newmenu)
        {
            siam->newmenu[siam->menunr].nm_Type = NM_END;
            siam->menustrip = CreateMenus(siam->newmenu, TAG_END);
            if (siam->menustrip == NULL) luaL_error(L, "Can't create menus");
            if (LayoutMenus(siam->menustrip, vinfo, GTMN_NewLookMenus, TRUE, TAG_END) == FALSE)
                luaL_error(L, "Can't layout menus");
            if (SetMenuStrip(siam->win, siam->menustrip) == FALSE)
                luaL_error(L, "Can't set menu strip");
        }
        SetAPen(siam->win->RPort, 1);

        // store window address in all gadgets
        Sigadget *sig = siam->gadgets;
        while (sig)
        {
            sig->win = siam->win;
            sig = sig->next;
        }
    }
    return 0;
}

static int siamiga_querywindow(lua_State *L)
{
    Siamiga *siam = checkSiamiga(L, 1);
    struct Window *win = siam->win;
    if (win) // window is open
    {
        lua_pushnumber(L, win->LeftEdge);
        lua_pushnumber(L, win->TopEdge);
        lua_pushnumber(L, win->Width);
        lua_pushnumber(L, win->Height);
        lua_pushnumber(L, win->Width - win->BorderLeft - win->BorderRight);
        lua_pushnumber(L, win->Height - win->BorderTop - win->BorderBottom);
        return 6;
    }
    return 0;
}

static int siamiga_closewindow(lua_State *L)
{
    Siamiga *siam = checkSiamiga(L, 1);
    struct Window *win = siam->win;
    if (win)
    {
        // Remember current size for reopening
        siam->top = win->TopEdge;
        siam->left = win->LeftEdge;
        siam->width = win->Width;
        siam->height = win->Height;
        ClearMenuStrip(win);
        CloseWindow(win);
        siam->win = NULL;

        FreeMenus(siam->menustrip);
        siam->menustrip = NULL;

        Sigadget *sig = siam->gadgets;
        while (sig)
        {
            sig->win = NULL;
            sig = sig->next;
        }
    }
    return 0;
}

static int siamiga_pset(lua_State *L)
{
    Siamiga *siam = checkSiamiga(L, 1);
    struct Window *win = siam->win;
    if (win)
    {
        int x = luaL_checknumber(L, 2);
        int y = luaL_checknumber(L, 3);
        Move(win->RPort, x, y);
        WritePixel(win->RPort, x, y);
    }
    else
        luaL_error(L, "Can't draw to closed window");

    return 0;
}

static int siamiga_line(lua_State *L)
{
    Siamiga *siam = checkSiamiga(L, 1);
    struct Window *win = siam->win;
    if (win)
        {
        if (lua_gettop(L) == 3)
        {
            Draw(win->RPort, luaL_checknumber(L, 2), luaL_checknumber(L, 3));
        }
        else
        {
            Move(win->RPort, luaL_checknumber(L, 2), luaL_checknumber(L, 3));
            Draw(win->RPort, luaL_checknumber(L, 4), luaL_checknumber(L, 5));
        }
    }
    else
        luaL_error(L, "Can't draw to closed window");

    return 0;
}

static int siamiga_box(lua_State *L)
{
    Siamiga *siam = checkSiamiga(L, 1);
    struct Window *win = siam->win;
    if (win)
    {
        struct RastPort *rp = win->RPort;

        // clear window (fill it with color 0)
        if (lua_gettop(L) == 1)
        {
            SetRast(rp, 0);
            return 0;
        }

        int x1 = luaL_checknumber(L, 2);
        int y1 = luaL_checknumber(L, 3);
        int x2 = x1 + (int)luaL_checknumber(L, 4);
        int y2 = y1 + (int)luaL_checknumber(L, 5);
        if (lua_toboolean(L, 6)) // true means filled box
        {
            Move(rp, x1, y1); // ensure that 1st point is current point after drawing
            int tmp;
            if (x1 > x2)
            {
                tmp = x1;
                x1 = x2;
                x2 = tmp;
            }
            if (y1 > y2)
            {
                tmp = y1;
                y1 = y2;
                y2 = tmp;
            }
            RectFill(rp, x1, y1, x2, y2); // RectFill requieres x1<x2 and y1<y2
        }
        else
        {
            Move(rp, x1, y1);
            Draw(rp, x2, y1);
            Draw(rp, x2, y2);
            Draw(rp, x1, y2);
            Draw(rp, x1, y1);
        }
    }
    else
        luaL_error(L, "Can't draw to closed window");

    return 0;
}

static int siamiga_ellipse(lua_State *L)
{
    Siamiga *siam = checkSiamiga(L, 1);
    struct Window *win = siam->win;
    if (win)
    {
        int x = luaL_checknumber(L, 2);
        int y = luaL_checknumber(L, 3);
        Move(win->RPort, x, y); // make x,y actual point
        DrawEllipse(win->RPort, x, y, luaL_checknumber(L, 4), luaL_checknumber(L, 5));
    }
    else
        luaL_error(L, "Can't draw to closed window");

    return 0;
}

static int siamiga_text(lua_State *L)
{
    Siamiga *siam = checkSiamiga(L, 1);
    struct Window *win = siam->win;
    if (win)
    {
        int x = luaL_checknumber(L, 2);
        int y = luaL_checknumber(L, 3);
        const char *txt = luaL_checkstring(L, 4);
        Move(win->RPort, x, y);
        if (txt)
            Text(win->RPort, txt, strlen(txt));
    }
    else
        luaL_error(L, "Can't draw to closed window");

    return 0;
}

static int siamiga_querytext(lua_State *L)
{
    Siamiga *siam = checkSiamiga(L, 1);
    struct Window *win = siam->win;
    if (win == NULL) luaL_error(L, "You can't query string for a closed window");
    const char *txt = luaL_checkstring(L, 2);
    struct TextExtent tex;
    TextExtent(win->RPort, txt, strlen(txt), &tex);
    lua_pushnumber(L, tex.te_Width);
    lua_pushnumber(L, tex.te_Height);
    lua_pushnumber(L, tex.te_Extent.MinX);
    lua_pushnumber(L, tex.te_Extent.MinY);
    lua_pushnumber(L, tex.te_Extent.MaxX);
    lua_pushnumber(L, tex.te_Extent.MaxY);
    return 6;
}

static int siamiga_createpen(lua_State *L)
{
    if (screen)
    {
        struct ColorMap * cm = screen->ViewPort.ColorMap;

        int r = luaL_checknumber(L, 1);
        luaL_argcheck(L, (r >= 0) && (r < 256), 1, "Parameter 'r' must be between 0 and 255");

        int g = luaL_checknumber(L, 2);
        luaL_argcheck(L, (g >= 0) && (g < 256), 2, "Parameter 'g' must be between 0 and 255");

        int b = luaL_checknumber(L, 3);
        luaL_argcheck(L, (b >= 0) && (b < 256), 3, "Parameter 'b' must be between 0 and 255");

        LONG pen = ObtainBestPen(cm, r * 0x01010101UL, g * 0x01010101UL, b * 0x01010101UL, TAG_END);
        if (pen < 0) luaL_error(L, "Can't create pen");
        lua_pushnumber(L, pen);
        return 1;
    }
    else
        luaL_error(L, "Can't create pen for unknown screen");

    return 0;
}

static int siamiga_releasepen(lua_State *L)
{
    if (screen)
        ReleasePen(screen->ViewPort.ColorMap, luaL_checknumber(L, 1));

    return 0;
}

static int siamiga_setpen(lua_State *L)
{
    Siamiga *siam = checkSiamiga(L, 1);
    struct Window *win = siam->win;
    if (win)
    {
        SetAPen(win->RPort, luaL_checknumber(L, 2));
    }
    else
        luaL_error(L, "Can't set pen of closed window");

    return 0;
}

static int siamiga_waitmessage(lua_State *L)
{
    Siamiga *siam;
    struct Window *win;
    ULONG sigset = 0;
    int openwindows = 0;
    int i;

    for(i=1 ; i <= lua_gettop(L) ; i++)
    {
        siam = toSiamiga(L, i);
        if (siam)
        {
            win = siam->win;
            if (win)
            {
                sigset |= (1L << win->UserPort->mp_SigBit);
                openwindows++;
            }
        }
    }
    if (sigset) Wait(sigset);
    lua_pushnumber(L, openwindows);
    return 1;
}

static int siamiga_getmessage(lua_State *L)
{
    int msg_cnt = 0;
    struct MenuItem *mitem = NULL;

    Siamiga *siam = toSiamiga(L, 1);
    struct Window *win = siam->win;
    if (win)
    {
        struct IntuiMessage *message = GT_GetIMsg(win->UserPort);
        if (message)
        {
	    switch(message->Class)
            {
		case IDCMP_GADGETDOWN:
		case IDCMP_GADGETUP:
                    ((struct Gadget*)(message->IAddress))->UserData = (APTR)(LONG)message->Code;  // active cycle gadget entry
		    lua_pushliteral(L, "gadget");
                    lua_pushnumber(L, ((struct Gadget*)message->IAddress)->GadgetID );
                    msg_cnt = 2;
                    break;
                case IDCMP_MOUSEBUTTONS:
                    lua_pushliteral(L, "mouse");
                    switch (message->Code)
                    {
                        case SELECTUP:
                            lua_pushliteral(L, "lu");
                            break;
                        case SELECTDOWN:
                            lua_pushliteral(L, "ld");
                            break;
                        case MIDDLEUP:
                            lua_pushliteral(L, "mu");
                            break;
                        case MIDDLEDOWN:
                            lua_pushliteral(L, "md");
                            break;
                        case MENUUP:
                            lua_pushliteral(L, "ru");
                            break;
                        case MENUDOWN:
                            lua_pushliteral(L, "rd");
                            break;
                        default:
                            lua_pushnil(L); // shouldn't happen; ensures order of return values
                    }
                    lua_pushnumber(L, message->MouseX);
                    lua_pushnumber(L, message->MouseY);
                    msg_cnt = 4;
                    break;
                case IDCMP_MENUPICK:
                    lua_pushliteral(L, "menu");
                    mitem = ItemAddress(siam->menustrip, message->Code);
                    if (mitem)
                    {
                        lua_pushnumber(L, (LONG)GTMENUITEM_USERDATA(mitem));
                    }
                    else
                    {
                        lua_pushnil(L);
                    }
                    msg_cnt = 2;
                    break;
                case IDCMP_VANILLAKEY:
                    lua_pushliteral(L, "key");
                    char keystring[1];
                    keystring[0] = message->Code;
                    lua_pushlstring(L, keystring, 1);
                    msg_cnt = 2;
                    break;
                case IDCMP_NEWSIZE:
                    lua_pushliteral(L, "size");
                    msg_cnt = 1;
                    break;
                case IDCMP_REFRESHWINDOW:
                    GT_BeginRefresh(win);
                    GT_EndRefresh(win, TRUE);
                    lua_pushliteral(L, "refresh");
                    msg_cnt = 1;
                    break;
                case IDCMP_CLOSEWINDOW:
                    lua_pushliteral(L, "close");
                    msg_cnt = 1;
                    break;
                default:
                    // always return something, or the message handling gets confused
                    lua_pushliteral(L, "dummy");
                    msg_cnt = 1;
                    break;    
            }
            GT_ReplyIMsg(message);
        }
    }
    return msg_cnt;
}

static int siamiga_filebox(lua_State *L)
{
    const char *title = lua_tostring(L, 1);
    const char *dir = lua_tostring(L, 2);

    struct FileRequester *fr = AllocAslRequestTags(ASL_FileRequest,
        title ? ASLFR_TitleText : TAG_IGNORE, (IPTR)title,
        dir ? ASLFR_InitialDrawer : TAG_IGNORE, (IPTR)dir,
        screen ? ASLFR_Screen : TAG_IGNORE, (IPTR)screen,
        TAG_END);

    if (fr)
    {
        if (AslRequest(fr, NULL))
        {
            ULONG bufsize = strlen(fr->rf_Dir) + strlen(fr->rf_File) + 10;
            char *path = malloc(bufsize);
            if (path)
            {
                strcpy(path, fr->rf_Dir);
                if (AddPart(path, fr->rf_File, bufsize))
                {
                    lua_pushstring(L, path);
                }
                else
                {
                    free(path);
                    FreeAslRequest(fr);
                    luaL_error(L, "AddPart failed");
                }
                free(path);
            }
            else
            {
                FreeAslRequest(fr);
                luaL_error(L, "Can't allocate RAM");
            }
        }
        else
        {
            // Filereq canceled
            lua_pushnil(L);
        }
        FreeAslRequest(fr);
    }
    else
    {
        luaL_error(L, "Can't allocate file requester");
    }
    return 1;
}

static int siamiga_messagebox(lua_State *L)
{
    Siamiga *siam = toSiamiga(L, 1);

    struct EasyStruct es;
    es.es_StructSize = sizeof(struct EasyStruct);
    es.es_Flags = 0;
    es.es_Title = (char *)luaL_checkstring(L, 2);
    es.es_TextFormat = (char *)luaL_checkstring(L, 3);
    es.es_GadgetFormat = (char *)luaL_checkstring(L, 4);

    LONG res = EasyRequest(siam->win, &es, NULL);
    lua_pushnumber(L, res);
    return 1;
}

static int siamiga_queryscreen(lua_State *L)
{
    if (screen == NULL) luaL_error(L, "Screen address is NULL");
    lua_pushnumber(L, screen->Width);
    lua_pushnumber(L, screen->Height);
    lua_pushnumber(L, GetBitMapAttr(screen->RastPort.BitMap, BMA_DEPTH));
    return 3;
}

static int siamiga_addmenu(lua_State *L)
{
    Siamiga *siam = checkSiamiga(L, 1);
    if (siam->menunr >= MAXMENUITEMS) luaL_error(L, "Maximum number of menu entries reached");

    struct Window *win = siam->win;
    if (win) luaL_error(L, "You can't add a menu to an open window");

    int id = luaL_checknumber(L, 2);
    const char *type = luaL_checkstring(L, 3);
    const char *title = luaL_checkstring(L, 4);
    const char *key = lua_tostring(L, 5);

    if (siam->newmenu == NULL)
    {
        siam->newmenu = calloc(sizeof(struct NewMenu), MAXMENUITEMS + 1); //additional entry for menu end
        if (siam->newmenu == NULL) luaL_error(L, "Can't allocate RAM for menus");
        if (strcmp(type, "menu") != 0) luaL_error(L, "First entry must be of type 'menu'");
    }

    struct NewMenu *menu = &siam->newmenu[siam->menunr];

    if (strcmp(type, "menu") == 0)
    {
        menu->nm_Type = NM_TITLE;
    }
    else if (strcmp(type, "item") == 0)
    {
        menu->nm_Type = NM_ITEM;
    }
    else if (strcmp(type, "sub") == 0)
    {
        menu->nm_Type = NM_SUB;
    }
    else
    {
        luaL_error(L, "Unknown menu type");
    }

    if (strcmp(title, "barlabel") == 0)
    {
        menu->nm_Label = NM_BARLABEL;
    }
    else
    {
        menu->nm_Label = strdup(title);
        if (menu->nm_Label == NULL) luaL_error(L, "Can't allocate RAM for title");
    }

    if (key)
    {
         menu->nm_CommKey = strdup(key);
         if (menu->nm_CommKey == NULL) luaL_error(L, "Can't allocate RAM for comm key");
    }

    menu->nm_UserData = (APTR)id;
    siam->menunr++;
    return 0;
}

static void copylabels(lua_State *L, Sigadget *sig)
{
    int i;
    int argcnt = lua_gettop(L);
    if ((argcnt < 9) || (argcnt > MAXLABELS + 8)) luaL_error(L, "Wrong number of labels");
    STRPTR *label_ptr = sig->labels;
    for (i = 9; i <= argcnt; i++)
    {
        const char *newlabel = lua_tostring(L, i);
        if (newlabel == NULL) luaL_error(L, "You can't use 'nil' for gadget label");
        *label_ptr = strdup(newlabel);
        if (*label_ptr == NULL) luaL_error(L, "Can't allocate RAM for label");
        label_ptr++;
        sig->label_cnt++;
    }
}

static int siamiga_addgadget(lua_State *L)
{
    Siamiga *siam = checkSiamiga(L, 1);
    if (siam->win)
        luaL_error(L, "You can't add gadgets to an open window");

    int id = luaL_checknumber(L, 2);
    const char *type = luaL_checkstring(L, 3);
    int x = luaL_checknumber(L, 4);
    int y = luaL_checknumber(L, 5);
    int w = luaL_checknumber(L, 6);
    luaL_argcheck(L, w>0 ,6 , "width must be greater than 0");
    int h = luaL_checknumber(L, 7);
    luaL_argcheck(L, h>0 ,7 , "height must be greater than 0");
    const char *txt = luaL_checkstring(L, 8);

    Sigadget *sig = calloc(sizeof(Sigadget), 1);
    if (sig == NULL) luaL_error(L, "Can't allocate RAM for gadget");

    if (txt)
    {
        sig->txt = strdup(txt);
    }

    struct NewGadget ng;
    ng.ng_TextAttr  = &gadfont;
    ng.ng_VisualInfo = vinfo;
    ng.ng_LeftEdge = x;
    ng.ng_TopEdge = y;
    ng.ng_Width = w;
    ng.ng_Height = h;
    ng.ng_GadgetText = sig->txt;
    ng.ng_GadgetID = id;
    ng.ng_Flags = 0;
    ng.ng_UserData = NULL;

    if (strcmp(type, "button") == 0)
    {
        siam->gad = CreateGadget(BUTTON_KIND, siam->gad ,&ng, TAG_END);
        sig->type = BUTTON_KIND;
    }
    else if (strcmp(type, "check") == 0)
    {
        int state = lua_toboolean(L, 9);
        siam->gad = CreateGadget(CHECKBOX_KIND, siam->gad, &ng, GTCB_Checked, state, TAG_END);
        sig->type = CHECKBOX_KIND;
    }
    else if (strcmp(type, "integer") == 0)
    {
        long def = lua_tonumber(L, 9);
        siam->gad = CreateGadget(INTEGER_KIND, siam->gad, &ng,
            GTIN_Number, def, GA_TabCycle, TRUE, TAG_END);
        sig->type = INTEGER_KIND;
    }
    else if (strcmp(type, "string") == 0)
    {
        const char *def = lua_tostring(L, 9); // can be NULL
        siam->gad = CreateGadget(STRING_KIND, siam->gad, &ng,
            GTST_String, (IPTR)def, GA_TabCycle, TRUE, TAG_END);
        sig->type = STRING_KIND;
    }
    else if (strcmp(type, "hslider") == 0)
    {
        WORD def = lua_tonumber(L, 9);
        WORD min = lua_tonumber(L, 10);
        WORD max = lua_tonumber(L, 11);
        if (min>max) luaL_error(L, "min is greater than max");
        siam->gad = CreateGadget(SLIDER_KIND, siam->gad, &ng,
            GTSL_Min, min,
            GTSL_Max, max,
            GTSL_Level, def,
            GA_RelVerify, TRUE,
            PGA_Freedom, LORIENT_HORIZ,
            TAG_END);
        sig->type = SLIDER_KIND;
    }
    else if (strcmp(type, "vslider") == 0)
    {
        WORD def = lua_tonumber(L, 9);
        WORD min = lua_tonumber(L, 10);
        WORD max = lua_tonumber(L, 11);
        if (min>max) luaL_error(L, "min is greater than max");
        siam->gad = CreateGadget(SLIDER_KIND, siam->gad, &ng,
            GTSL_Min, min,
            GTSL_Max, max,
            GTSL_Level, def,
            GA_RelVerify, TRUE,
            PGA_Freedom, LORIENT_VERT,
            TAG_END);
        sig->type = SLIDER_KIND;
    }
    else if (strcmp(type, "radio") == 0)
    {
        copylabels(L, sig);
        siam->gad = CreateGadget(MX_KIND, siam->gad, &ng,
            GTMX_Labels, sig->labels, 
            GA_RelVerify, TRUE,
            TAG_END);
        sig->type = MX_KIND;
    }
    else if (strcmp(type, "cycle") == 0)
    {
        copylabels(L, sig);
        siam->gad = CreateGadget(CYCLE_KIND, siam->gad, &ng,
            GTCY_Labels, sig->labels, 
            GA_RelVerify, TRUE,
            TAG_END);
        sig->type = CYCLE_KIND;
    }
    else
    {
        luaL_error(L, "Unknown gadget type");
    }

    if (siam->gad == NULL) luaL_error(L, "Can't create gadget");

    sig->gad = siam->gad;
    if (siam->gadgets != NULL)
    {
        sig->next = siam->gadgets;
        siam->gadgets = sig;
    }
    else
    {
        siam->gadgets = sig;
    }
    pushSigadget(L, sig);
    return 1;
}

static const luaL_reg siamiga_methods[] =
{
    {"createwindow",siamiga_createwindow},
    {"openwindow",  siamiga_openwindow},
    {"querywindow", siamiga_querywindow},
    {"closewindow", siamiga_closewindow},
    {"pset",        siamiga_pset},
    {"line",        siamiga_line},
    {"box",         siamiga_box},
    {"ellipse",     siamiga_ellipse},
    {"text",        siamiga_text},
    {"querytext",   siamiga_querytext},
    {"waitmessage", siamiga_waitmessage},
    {"getmessage",  siamiga_getmessage},
    {"createpen",   siamiga_createpen},
    {"releasepen",  siamiga_releasepen},
    {"setpen",      siamiga_setpen},
    {"filebox",     siamiga_filebox},
    {"messagebox",  siamiga_messagebox},
    {"queryscreen", siamiga_queryscreen},
    {"addmenu",     siamiga_addmenu},
    {"addgadget",   siamiga_addgadget},
    {NULL, NULL}
};

static int siamiga_gc(lua_State *L)
{
    int i;
    Siamiga *siam = checkSiamiga(L, 1);
    siamiga_closewindow(L);
    free(siam->title);

    FreeGadgets(siam->glist);

    Sigadget *sig = siam->gadgets;
    Sigadget *next = NULL;
    while (sig)
    {
        next = sig->next;
        free(sig->txt);
        free(sig);
        for (i = 0; i < sig->label_cnt; i++)
        {
            D(bug("Free label %s\n", sig->labels[i]));
            free(sig->labels[i]);
        }
        sig = next;
    }

    if (siam->newmenu)
    {
        struct NewMenu *menu;
        for(i=0; i < siam->menunr; i++)
        {
            menu = &siam->newmenu[i];
            if (menu->nm_Label != NM_BARLABEL) free((STRPTR)menu->nm_Label);
            free((STRPTR)menu->nm_CommKey);
        }
        free(siam->newmenu);
    }
    return 0;
}

static const luaL_reg siamiga_meta[] = {
    {"__gc",    siamiga_gc},
    {NULL, NULL}
};


//*****************************************************************
//                            Sigadget
//*****************************************************************

// the functions toSigadget, checkSigadget and pushSigadget are defined above

static int sigadget_set(lua_State *L)
{
    Sigadget *sig = checkSigadget(L, 1);
    struct Window *win = sig->win;
    LONG value;
    char *strvalue;
    switch(sig->type)
    {
        case CHECKBOX_KIND:
            value = lua_toboolean(L, 2);
            GT_SetGadgetAttrs(sig->gad, win, NULL, GTCB_Checked, value, TAG_END);
            break;
        case SLIDER_KIND:
            value = lua_tonumber(L, 2);
            GT_SetGadgetAttrs(sig->gad, win, NULL, GTSL_Level, value, TAG_END);
            break;
        case INTEGER_KIND:
            value = lua_tonumber(L, 2);
            GT_SetGadgetAttrs(sig->gad, win, NULL, GTIN_Number, value, TAG_END);
            break;
        case STRING_KIND:
            strvalue = (char *)lua_tostring(L, 2);
            GT_SetGadgetAttrs(sig->gad, win, NULL, GTST_String, (IPTR)strvalue, TAG_END);
            break;
        case MX_KIND:
            value = lua_tonumber(L, 2) - 1; // Lua indices start with 1, gadtools with 0
            luaL_argcheck(L, (value >= 0) && (value < sig->label_cnt), 2, "Parameter must be between 1 and label count");
            sig->gad->UserData = (APTR)value;
            GT_SetGadgetAttrs(sig->gad, win, NULL, GTMX_Active, value, TAG_END);
            break;
        case CYCLE_KIND:
            value = lua_tonumber(L, 2) - 1;
            luaL_argcheck(L, (value >= 0) && (value < sig->label_cnt), 2, "Parameter must be between 1 and label count");
            sig->gad->UserData = (APTR)value;
            GT_SetGadgetAttrs(sig->gad, win, NULL, GTCY_Active, value, TAG_END);
            break;
        default:
            luaL_error(L, "Wrong gadget type");
    }
    return 0;
}

static int sigadget_get(lua_State *L)
{
    Sigadget *sig = checkSigadget(L, 1);
    struct Window *win = sig->win;
    if (win == NULL)
        luaL_error(L, "Gadget must be in an open window");

    LONG result = 0;
    LONG changed = 0;
    switch (sig->type)
    {
        case CHECKBOX_KIND:
            changed = GT_GetGadgetAttrs(sig->gad, win, NULL, GTCB_Checked, (IPTR)&result, TAG_END);
            lua_pushboolean(L, result);
            break;
        case SLIDER_KIND:
            changed = GT_GetGadgetAttrs(sig->gad, win, NULL, GTSL_Level, (IPTR)&result, TAG_END);
            lua_pushnumber(L, result);
            break;
        case INTEGER_KIND:
            changed = GT_GetGadgetAttrs(sig->gad, win, NULL, GTIN_Number, (IPTR)&result, TAG_END);
            lua_pushnumber(L, result);
            break;
        case STRING_KIND:
            changed = GT_GetGadgetAttrs(sig->gad, win, NULL, GTST_String, (IPTR)&result, TAG_END);
            lua_pushstring(L, (char *)result);
            break;
        case MX_KIND:
            changed = 1;
	    lua_pushnumber(L, (int)sig->gad->UserData + 1); // Lua indices start with 1
            break;
        case CYCLE_KIND:
            changed = 1;
            lua_pushnumber(L, (int)sig->gad->UserData + 1);
            break;
	default:
            luaL_error(L, "Wrong gadget type");
    }
    if (changed != 1)
        luaL_error(L, "Gadget can't be queried");

    return 1;
}

static const luaL_reg sigadget_methods[] =
{
    {"set",     sigadget_set},
    {"get",     sigadget_get},
    {NULL, NULL}
};

static const luaL_reg sigadget_meta[] = {
    {NULL, NULL}
};


//*****************************************************************
//                            Sipicture
//*****************************************************************

#define SIPICTURE "Sipicture"

typedef struct Picture
{
    struct BitMap *bm;
    int width, height, depth;
} Picture;

#if 0
// disabled to get rid of warning, enable me when you need me
static Picture *toPicture(lua_State *L, int index)
{
    Picture *pi = (Picture *)lua_touserdata(L, index);
    if (pi == NULL) luaL_typerror(L, index, SIPICTURE);
    return pi;
}
#endif

static Picture *checkPicture(lua_State *L, int index)
{
    Picture *pi;
    luaL_checktype(L, index, LUA_TUSERDATA);
    pi = (Picture *)luaL_checkudata(L, index, SIPICTURE);
    if (pi == NULL) luaL_typerror(L, index, SIPICTURE);
    return pi;
}

static Picture *pushPicture(lua_State *L)
{
    Picture *pi = (Picture *)lua_newuserdata(L, sizeof( Picture ));
    luaL_getmetatable(L, SIPICTURE);
    lua_setmetatable(L, -2);
    return pi;
}

static int sipicture_load(lua_State *L)
{
    const char *filename = luaL_checkstring(L, 1);

    Object *o = NewDTObject((APTR)filename,
        DTA_GroupID, GID_PICTURE,
        PDTA_Remap, TRUE,
        PDTA_Screen, (IPTR)screen,
        PDTA_DestMode, PMODE_V43,
        TAG_END
    );
    if (o == NULL) luaL_error(L, "Can't create datatype object");

    DoDTMethod(o, NULL, NULL, DTM_PROCLAYOUT, 0, 1);

    struct BitMap *dbm = NULL;
    struct BitMapHeader *bmh = NULL;
    GetDTAttrs(o, PDTA_DestBitMap, (IPTR)&dbm, PDTA_BitMapHeader, (IPTR)&bmh, TAG_END);

    if (bmh == NULL)
    {
        DisposeDTObject(o);
        luaL_error(L, "Bitmapheader is Null");
    }   

    int width = bmh->bmh_Width;
    int height = bmh->bmh_Height;
    int depth = bmh->bmh_Depth;
    printf("width %d height %d depth %d\n", width, height, depth);    
    if (dbm == NULL)
    {
        DisposeDTObject(o);
        luaL_error(L, "Destbitmap is Null");
    }


    struct BitMap *newbm = AllocBitMap(width, height, depth, 0, screen->RastPort.BitMap);
    if (newbm == NULL)
    {
        DisposeDTObject(o);
        luaL_error(L, "Can't allocate Bitmap");
    }
    BltBitMap(dbm, 0, 0, newbm, 0, 0 , width, height, 0xC0, 0xFFFFFFFF, NULL);
    DisposeDTObject(o);

    Picture *pic = pushPicture(L);
    pic->bm = newbm;
    pic->width = width;
    pic->height = height;
    pic->depth = depth;

    return 1;
}

#if 0
// saving didn't work. Still have to find the reason

static int sipicture_save(lua_State *L)
{
    UBYTE *cmap;
    ULONG *cregs;

    Picture *pi = checkPicture(L, 1);
    const char *filename = luaL_checkstring(L, 2);

    int w = pi->width;
    int h = pi->height;
    int d = pi->depth;

    struct BitMap *newbm = AllocBitMap(w, h, d, 0, screen->RastPort.BitMap );
    if (newbm == NULL) luaL_error(L, "Can't allocate bitmap");

    BltBitMap(pi->bm, 0, 0, newbm, 0, 0, w, h, 0xC0, 0xFFFFFFFF, NULL);

    int ncols = (d > 8 ? 0 : 1 << d);

    struct ViewPort *vp = &(screen->ViewPort);

    Object *o = NewDTObject(NULL,
        DTA_SourceType, DTST_RAM,
        DTA_GroupID, GID_PICTURE,
        PDTA_BitMap, (ULONG)newbm,
        PDTA_ModeID, GetVPModeID(vp),
        (ncols ? PDTA_NumColors : PDTA_DestMode),(ncols ? ncols : PMODE_V43),
        TAG_END);
    if (o == NULL)
    {
        WaitBlit();
        FreeBitMap(newbm);
        luaL_error(L, "Can't create datatype object");
    }

    struct BitMapHeader *bmhd = NULL;
    GetDTAttrs(o, PDTA_BitMapHeader, (IPTR)&bmhd, TAG_END);

    bmhd->bmh_Width  = w;
    bmhd->bmh_Height = h;
    bmhd->bmh_Depth  = d;
    bmhd->bmh_XAspect = 22;
    bmhd->bmh_YAspect = 22;
    bmhd->bmh_PageWidth = (w <= 320 ? 320 : w <= 640 ? 640 : w <= 1024 ? 1024 : w <= 1280 ? 1280 : 1600);
    bmhd->bmh_PageHeight = bmhd->bmh_PageWidth * 3 / 4;

    if (ncols)
    {
        GetDTAttrs(o, PDTA_ColorRegisters, (IPTR)&cmap, PDTA_CRegs, (IPTR)&cregs, TAG_END);
        GetRGB32(vp->ColorMap, 0, ncols, cregs);
        int i;
        for (i = 3 * ncols; i; i--)
            *cmap++ = (*cregs++) >> 24;
    }

    BPTR fhand = Open(filename, MODE_NEWFILE);
    if (fhand)
    {
        ULONG res = DoDTMethod(o, NULL, NULL, DTM_WRITE, 0, (ULONG)fhand, DTWM_IFF, 0);
        Close(fhand);
        if (!res)
            DeleteFile(filename);
    }
    else
    {
        DisposeDTObject( o );
        luaL_error(L, "Can't open output file");
    }


    DisposeDTObject( o );
    return 0;
}
#endif

static int sipicture_put(lua_State *L)
{
    Picture *pi = checkPicture(L, 1);
    if (pi->bm == NULL) luaL_error(L, "Picture object doesn't contain bitmap");

    Siamiga *siam = checkSiamiga(L, 2);
    struct Window *win = siam->win;
    if (win == NULL) luaL_error(L, "You can't put to a closed window");

    int x = luaL_checknumber(L, 3);
    int y = luaL_checknumber(L, 4);

    struct RastPort rp;
    memset(&rp, 0, sizeof(struct RastPort));
    InitRastPort(&rp);
    rp.BitMap = pi->bm;
    ClipBlit(&rp, 0 , 0 , win->RPort , x, y, pi->width, pi->height, 0xC0);
    return 0;
}

static int sipicture_get(lua_State *L)
{
    Siamiga *siam = checkSiamiga(L, 1);
    struct Window *win = siam->win;
    if (win == NULL) luaL_error(L, "You can't get picture from closed window");

    int x = luaL_checknumber(L, 2);
    int y = luaL_checknumber(L, 3);
    int w = luaL_checknumber(L, 4);
    int h = luaL_checknumber(L, 5);

    int d = GetBitMapAttr(win->RPort->BitMap , BMA_DEPTH);
    struct BitMap *bm = AllocBitMap(w, h, d, BMF_CLEAR , win->RPort->BitMap);
    if (bm == NULL) luaL_error(L, "Can't allocate bitmap");

    struct RastPort rp;
    memset(&rp, 0, sizeof(struct RastPort));
    InitRastPort(&rp);
    rp.BitMap = bm;
    ClipBlit(win->RPort, x, y, &rp, 0, 0, w, h, 0xC0);

    Picture *pi = pushPicture(L);
    pi->bm = bm;
    pi->width = w;
    pi->height = h;
    pi->depth = d;
    return 1;
}

static int sipicture_free(lua_State *L)
{
    Picture *pi = checkPicture(L, 1);
    if (pi)
    {
        WaitBlit();
        FreeBitMap(pi->bm);
        pi->bm = NULL;
    }
    return 0;
}

static const luaL_reg sipicture_methods[] =
{
    {"load",    sipicture_load},
    // {"save",    sipicture_save},
    {"put",     sipicture_put},
    {"get",     sipicture_get},
    {"free",    sipicture_free},

    {NULL, NULL}
};

static int sipicture_gc(lua_State *L)
{
    sipicture_free(L);
    return 0;
}

static const luaL_reg sipicture_meta[] = {
    {"__gc",    sipicture_gc},
    {NULL, NULL}
};


//*****************************************************************
//                            Library
//*****************************************************************

int luaopen_siamigalib(lua_State *L)
{
    IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 36);
    if (IntuitionBase == NULL) luaL_error(L, "Can't open intuition.library V 36");

    GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 36);
    if (GfxBase == NULL) luaL_error(L, "Can't open graphics.library V 36");

    AslBase = OpenLibrary("asl.library", 36);
    if (AslBase == NULL) luaL_error(L, "Can't open asl.library V 36");

    DataTypesBase = OpenLibrary("datatypes.library", 43);
    if (DataTypesBase == NULL) luaL_error(L, "Can't open datatypes.library V 43");

    GadToolsBase = OpenLibrary("gadtools.library", 39);
    if (GadToolsBase == NULL) luaL_error(L, "Can't open gadtools.library V39");

    screen = LockPubScreen(NULL);
    if (screen == NULL) luaL_error(L, "Can't lock pubscreen");

    if ((screen->Font) && (screen->Font->ta_Name))
    {
        memcpy(&gadfont, screen->Font, sizeof(gadfont));
    }
    else
        luaL_error(L, "Screen has no font attribute");

    vinfo = GetVisualInfo(screen, TAG_END);
    if (vinfo == NULL) luaL_error(L, "Can't get visualinfo");

    luaL_openlib(L, SIAMIGA, siamiga_methods, 0);
    luaL_newmetatable(L, SIAMIGA);
    luaL_openlib(L, 0, siamiga_meta, 0);
    lua_pushliteral(L, "__index");
    lua_pushvalue(L, -3);
    lua_rawset(L, -3);
    lua_pushliteral(L, "__metatable");
    lua_pushvalue(L, -3);
    lua_rawset(L, -3);
    lua_pop(L, 1);

    luaL_openlib(L, SIGADGET, sigadget_methods, 0);
    luaL_newmetatable(L, SIGADGET);
    luaL_openlib(L, 0, sigadget_meta, 0);
    lua_pushliteral(L, "__index");
    lua_pushvalue(L, -3);
    lua_rawset(L, -3);
    lua_pushliteral(L, "__metatable");
    lua_pushvalue(L, -3);
    lua_rawset(L, -3);
    lua_pop(L, 1);

    luaL_openlib(L, SIPICTURE, sipicture_methods, 0);
    luaL_newmetatable(L, SIPICTURE);
    luaL_openlib(L, 0, sipicture_meta, 0);
    lua_pushliteral(L, "__index");
    lua_pushvalue(L, -3);
    lua_rawset(L, -3);
    lua_pushliteral(L, "__metatable");
    lua_pushvalue(L, -3);
    lua_rawset(L, -3);
    lua_pop(L, 1);

    return 1;
}

void siamiga_close_libraries(void)
{
    if (screen) UnlockPubScreen(NULL, screen);
    if (vinfo) FreeVisualInfo(vinfo);
    CloseLibrary(GadToolsBase);
    CloseLibrary(DataTypesBase);
    CloseLibrary(AslBase);
    CloseLibrary((struct Library *)IntuitionBase);
    CloseLibrary((struct Library *)GfxBase);
}

