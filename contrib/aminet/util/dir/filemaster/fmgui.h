/*
     Filemaster - Multitasking directory utility.
     Copyright (C) 2000  Toni Wilen
     
     This program is free software; you can redistribute it and/or
     modify it under the terms of the GNU General Public License
     as published by the Free Software Foundation; either version 2
     of the License, or (at your option) any later version.
     
     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.
     
     You should have received a copy of the GNU General Public License
     along with this program; if not, write to the Free Software
     Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include <utility/tagitem.h>

#include <libraries/bgui.h>
#include <proto/bgui.h>
#include <libraries/bgui_macros.h>

extern struct Library *BGUIBase; 
#define GUIGROUPS 16
#define GUISLOTS 64

#ifndef GUIDEF
#define GUIDEF

struct GUISlot {
	Object *obj;
	void *data;
	WORD type;
	WORD id;
	UBYTE shortcut[2];
};

struct GUIGroup {
	WORD alignment;
	WORD used;
	struct GUISlot slots[GUISLOTS];
};
struct GUIBase {
	Object *obj;
	struct Window *win;
	UBYTE *title;
	WORD currentgroup;
	struct Screen *screen;
	struct GUISlot *lastslot;
	struct GUIGroup *groups[GUIGROUPS];
};
#endif

WORD reqinfowindow(UBYTE*,UBYTE*,UWORD,SIPTR,...);

struct GUISlot *getguislot(struct GUIBase*,Object*,WORD,WORD,void*);

struct GUIBase *getguibase(UBYTE*);
void setguigroup(struct GUIBase*,WORD,WORD);
void connectgroups(struct GUIBase*,WORD,WORD);
void setconnectgroup(struct GUIBase*,WORD,WORD,WORD);
void denaobject(struct GUIBase*,WORD,BOOL);
LONG getobject(struct GUIBase*,WORD);
void setobject(struct GUIBase*,WORD,LONG);

WORD quickreq(struct GUIBase*);
BOOL initreq(struct GUIBase*);
void freereq(struct GUIBase*);
BOOL openreq(struct GUIBase*);
void closereq(struct GUIBase*);
BOOL openinitreq(struct GUIBase*);
WORD reqmsghandler(struct GUIBase*);
void sleepreq(struct GUIBase*);
void wakereq(struct GUIBase*);

struct GUISlot *reqtoggle(struct GUIBase*,UBYTE*,WORD,UWORD,BOOL*);
struct GUISlot *reqtogglemsg(struct GUIBase*,WORD,WORD,UWORD,BOOL*);

struct GUISlot *reqbuttonmsg(struct GUIBase*,WORD,WORD,UWORD);
struct GUISlot *reqbutton(struct GUIBase*,UBYTE*,WORD,UWORD);

void buttonbarmsg(struct GUIBase*,SIPTR,...);
void buttonbar(struct GUIBase*,void*,...);
void startbuttonbar(struct GUIBase*);

struct GUISlot *reqinfo(struct GUIBase*,UBYTE*,WORD,UWORD);
struct GUISlot *reqinfomsg(struct GUIBase*,WORD,WORD,UWORD);

struct GUISlot *reqstring(struct GUIBase*,WORD,UBYTE*,WORD);
struct GUISlot *reqinteger(struct GUIBase*,WORD,LONG*,LONG,LONG);

struct GUISlot *reqcyclemsg(struct GUIBase*,WORD,WORD*,SIPTR,...);
struct GUISlot *reqcycle(struct GUIBase*,WORD,WORD*,UBYTE*,...);
struct GUISlot *reqcycle2msg(struct GUIBase*,WORD,WORD*);

struct GUISlot *reqpalette(struct GUIBase*,WORD,UBYTE*);

#define guiUC		0x0001
#define guiCENTER	0x0002
#define guiLEFT		0x0004
#define guiRIGHT	0x0008
