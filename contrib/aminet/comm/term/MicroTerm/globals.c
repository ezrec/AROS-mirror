#include "symbols.y"

/** Globals.c

            Written by Stephen Vermeulen (403) 282-7990

            PO Box 3295, Station B,
            Calgary, Alberta, CANADA, T2M 4L8

    Here is where the various globals are located, most of this
    is just structures for the menus...
 **/

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;

struct NewWindow new_cw =
{
  0, 0, 640, 50,
  -1, -1,
  MENUPICK,
  WINDOWSIZING | WINDOWDEPTH | WINDOWDRAG | SMART_REFRESH | ACTIVATE,
  NULL,
  NULL,
  (UBYTE *) "BBS Term",
  NULL,
  NULL,
  50, 20,
  -1, -1,
  WBENCHSCREEN
};

struct IntuiText help_text4 =
{
  0, 1, JAM1, 0, 31, NULL, (UBYTE *) "some more help", NULL
};

struct IntuiText help_text3 =
{
  0, 1, JAM1, 0, 21, NULL, (UBYTE *) "some help", &help_text4
};

struct IntuiText help_text2 =
{
  0, 1, JAM1, 0, 11, NULL, (UBYTE *) "another line of help", &help_text3
};


struct IntuiText help_text1 =
{
  0, 1, JAM1, 0, 1, NULL, (UBYTE *) "The first line of help", &help_text2
};

struct IntuiText reader_text2 =
{
  0, 1, JAM1, 0, 21, NULL, (UBYTE *) "(403) 282-7990", NULL
};

struct IntuiText reader_text1 =
{
  0, 1, JAM1, 0, 11, NULL, (UBYTE *) "Stephen Vermeulen", &reader_text2
};

struct IntuiText sub_version_text =
{
  0, 1, JAM1, 0, 1, NULL,  (UBYTE *) "MicroTerm: 0.1 Written by:", &reader_text1
};


struct MenuItem sub_help =
{
  NULL,
  60, 0, 264, 80,
  (short) (ITEMTEXT | ITEMENABLED | HIGHNONE),
  NULL,
  (APTR) &help_text1,
  NULL,
  0,
  NULL,
  0
};

struct MenuItem sub_version =
{
  NULL,
  60, 0, 264, 60,
  (short) (ITEMTEXT | ITEMENABLED | HIGHNONE),
  NULL,
  (APTR) &sub_version_text,
  NULL,
  0,
  NULL,
  0
};

struct IntuiText quit_text =
{
  0, 1, JAM1, 0, 1, NULL, (UBYTE *) "Quit", NULL
};

struct MenuItem quit_item =
{
  NULL,
  0, 40, 150, 10,
  (short) (ITEMTEXT | ITEMENABLED | HIGHCOMP | COMMSEQ),
  NULL,
  (APTR) &quit_text,
  NULL,
  'Q',
  NULL,
  0
};

struct IntuiText help_text =
{
  0, 1, JAM1, 0, 1, NULL, (UBYTE *) "Help...", NULL
};

struct MenuItem help_item =
{
  &quit_item,
  0, 20, 150, 10,
  (short) (ITEMTEXT | ITEMENABLED | HIGHNONE),
  NULL,
  (APTR) &help_text,
  NULL,
  0,
  &sub_help,
  0
};

struct IntuiText about_text =
{
  0, 1, JAM1, 0, 1, NULL, (UBYTE *) "About...", NULL
};

struct MenuItem about_item =
{
  &help_item,
  0, 10, 150, 10,
  (short) (ITEMTEXT | ITEMENABLED | HIGHNONE),
  NULL,
  (APTR) &about_text,
  NULL,
  0,
  &sub_version,
  0
};

struct IntuiText nothing_text =
{
  0, 1, JAM1, 0, 1, NULL, (UBYTE *) "Nothing", NULL
};

struct MenuItem nothing_item =
{
  &about_item,
  0, 0, 150, 10,
  (short) (ITEMTEXT | ITEMENABLED | HIGHCOMP | COMMSEQ),
  NULL,
  (APTR) &nothing_text,
  NULL,
  'P',
  NULL,
  0
};

struct Menu term_menu =
{
  NULL,
  0, 0, 70, 10,
  (short) MENUENABLED,
  (BYTE *) "Project",
  &nothing_item
};
