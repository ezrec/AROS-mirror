/*

Directory Opus 4
Original GPL release version 4.12
Copyright 1993-2000 Jonathan Potter

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

All users of Directory Opus 4 (including versions distributed
under the GPL) are entitled to upgrade to the latest version of
Directory Opus version 5 at a reduced price. Please see
http://www.gpsoft.com.au for more information.

The release of Directory Opus 4 under the GPL in NO WAY affects
the existing commercial status of Directory Opus 5.

*/

/* Structures and Tags for font-sensitive layout routines */

/* A requester object; normally text, a gadget, a border or a listview */

struct RequesterObject {
	struct RequesterObject *ro_next;     /* Next object */
	ULONG ro_type;                       /* Object type */
	APTR ro_data;                        /* Object data */
};

/* The main definition structure of a requester */

struct RequesterBase {
	short rb_width,rb_height;            /* Width, Height in characters    */
	short rb_widthfine,rb_heightfine;    /* Width, Height fine positioning */

	short rb_leftoffset,rb_topoffset;    /* Left, Top object offsets       */

	char rb_fg,rb_bg;                    /* Foreground, Background pens    */
	char rb_shine,rb_shadow;             /* Shine, Shadow pens             */
	struct TextFont *rb_font;            /* Requester font                 */
	char *rb_screenname;                 /* Screen name to open on         */
	struct Screen *rb_screen;            /* Screen pointer to open on      */
	char *rb_title;                      /* Window title (RBF_BORDERS)     */

	struct DOpusRemember *rb_memory;     /* Memory key                     */
	struct RequesterObject *rb_objects;  /* Requester objects              */
	struct Window *rb_window;            /* Window pointer                 */

	ULONG rb_flags;                      /* Requester flags                */
	ULONG rb_privateflags;               /* Private requester flags        */
	struct StringExtend *rb_extend;      /* String Extension if needed     */
	ULONG rb_idcmpflags;                 /* IDCMP flags or 0 for standard  */

	APTR rb_privatedata;                 /* Private requester data         */
	char **rb_string_table;              /* String table (RBF_STRINGS)     */
};

/* rb_flags */

#define RBF_BORDERS         1          /* Standard Intuition borders     */
#define RBF_CLOSEGAD        2          /* Close gadget with borders      */
#define RBF_STRINGS         4          /* Custom string table supplied   */
#define RBF_WINDOWCENTER    8          /* Center within provided window  */

/* rb_privateflags */

#define RBP_CLOSEFONT       1          /* rb_font must be closed         */


/* Requester Text object */

typedef struct RequesterObject_Text {
	short ot_left,ot_top;                /* Left edge, Top edge */
	char *ot_text;                       /* Text string */
} Object_Text;


/* Requester Border object */

typedef struct RequesterObject_Border {
	short ob_left,ob_top;                /* Left edge, Top edge */
	short ob_width,ob_height;            /* Width, Height */
	short ob_type;                       /* Border type */
	short ob_textpos;                    /* Text positioning */
	char *ob_text;                       /* Text string */
} Object_Border;


/* Requester Gadget object */

typedef struct RequesterObject_Gadget {
	struct Gadget og_gadget;             /* Gadget structure */
	short og_textpos;                    /* Text positioning */
	char *og_text;                       /* Text string */
	char og_type;                        /* Private type identifier */
} Object_Gadget;


/* Requester ListView object */

typedef struct RequesterObject_ListView {
	struct DOpusListView ol_listview;    /* List view structure */
} Object_ListView;


/* Object Tag definitions */

#define RO_Dummy      (TAG_USER + 99)

#define RO_Type       (RO_Dummy + 0x01)    /* Object type */

#define RO_Left       (RO_Dummy + 0x02)    /* Left edge in characters */
#define RO_Top        (RO_Dummy + 0x03)    /* Top edge in characters */
#define RO_LeftFine   (RO_Dummy + 0x04)    /* Left edge fine positioning */
#define RO_TopFine    (RO_Dummy + 0x05)    /* Top edge fine positioning */

#define RO_Width      (RO_Dummy + 0x06)    /* Width in characters */
#define RO_Height     (RO_Dummy + 0x07)    /* Height in characters */
#define RO_WidthFine  (RO_Dummy + 0x08)    /* Width fine adjustment */
#define RO_HeightFine (RO_Dummy + 0x09)    /* Height fine adjustment */

#define RO_Text       (RO_Dummy + 0x0a)    /* Object text */
#define RO_TextPos    (RO_Dummy + 0x0b)    /* Text positioning */

#define RO_GadgetID   (RO_Dummy + 0x0c)    /* Gadget ID */
#define RO_GadgetType (RO_Dummy + 0x0d)    /* Gadget type */

#define RO_BorderType (RO_Dummy + 0x0e)    /* Border type */

#define RO_ListViewID (RO_Dummy + 0x0f)    /* ListView ID */

#define RO_StringBuf  (RO_Dummy + 0x10)    /* String gadget buffer */
#define RO_StringLen  (RO_Dummy + 0x11)    /* Size of string gadget buffer */
#define RO_StringUndo (RO_Dummy + 0x12)    /* String undo buffer */

#define RO_BoolOn     (RO_Dummy + 0x13)    /* Boolean gadget is on by default */

#define RO_ChkCenter  (RO_Dummy + 0x14)    /* Check gadgets are font-centered */

#define RO_TextNum    (RO_Dummy + 0x15)    /* Use string from string-table */

#define RO_HighRecess (RO_Dummy + 0x16)    /* Highlight by recessing borders */


/* RO_ObjectType */

#define OBJECT_TEXT      1 /* Text object */
#define OBJECT_BORDER    2 /* Border object */
#define OBJECT_GADGET    3 /* Gadget object */
#define OBJECT_LISTVIEW  4 /* ListView object */


/* RO_TextPos */

#define TEXTPOS_ABSOLUTE 1 /* Absolute coordinates */
#define TEXTPOS_CENTER   2 /* Center within object */
#define TEXTPOS_LEFT     3 /* Position to left of object */
#define TEXTPOS_RIGHT    4 /* Position to right of object */
#define TEXTPOS_ABOVE    5 /* Position above object */
#define TEXTPOS_BELOW    6 /* Position below object */

#define TEXTPOS_F_NOUSCORE 32768  /* Do not render underscores */

/* RO_BorderType */

#define BORDER_NONE      0 /* No border */
#define BORDER_NORMAL    1 /* Normal border */
#define BORDER_RECESSED  2 /* Recessed border */
#define BORDER_GROUP     3 /* Group border */
#define BORDER_STRING    4 /* String border */


/* RO_GadgetType */

#define GADGET_BOOLEAN   0 /* Boolean gadget */
#define GADGET_STRING    1 /* String gadget */
#define GADGET_PROP      2 /* Proportional gadget */
#define GADGET_CHECK     3 /* Check-mark gadget */
#define GADGET_CYCLE     4 /* Cycle gadget */
#define GADGET_RADIO     5 /* Radio button */
#define GADGET_GLASS     6 /* Magnifying-glass gadget */
#define GADGET_INTEGER   7 /* Integer string gadget */
#define GADGET_TINYGLASS 8 /* Smaller, centered glass gadget */

/* Requester opening routine */

struct Window *OpenRequester(struct RequesterBase *Requester);

/* Requester closing routine */

void CloseRequester(struct RequesterBase *Requester);

/* Object adding routine */

APTR AddRequesterObject(struct RequesterBase *Requester,
	struct TagItem *ObjectTags);

/* Requester refreshing routine */

void RefreshRequesterObject(struct RequesterBase *Requester,
	struct RequesterObject *FirstObject);

/* Routine to render text within an area */

void ObjectText(struct RequesterBase *Requester,
	short Left,short Top,
	short Width,short Height,
	char *Text,
	short TextPos);
