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

#include "dopus.h"

/* SAS Detach information */

long __stack=16384;                        /* Stack size needed */
long __priority=0;                         /* Default task priority */
long __BackGroundIO=0;                     /* IO off for non-beta */
char *__procname="dopus_task";             /* Task name */

/* Library bases */

struct DOpusBase *DOpusBase;              /* dopus.library */
struct IntuitionBase *IntuitionBase;      /* intuition.library */
struct GfxBase *GfxBase;                  /* graphics.library */
struct Library *IconBase;                 /* icon.library */
struct Library *DiskfontBase;             /* diskfont.library */
struct Library *PPBase;                   /* powerpacker.library */
struct Library *LayersBase;               /* layers.library */
struct Library *WorkbenchBase;            /* workbench.library */
struct Library *MUSICBase;                /* music.library */
struct Library *CxBase;                   /* commodities.library */
struct Library *AccountsBase;             /* accounts.library */
struct UtilityBase *UtilityBase;              /* utility.library */


/* Intuition objects */

struct IntuiMessage *IMsg;                /* Main IntuiMessage pointer */
struct Screen *MainScreen=NULL;           /* Main screen */
struct Screen *iffscreen=NULL;            /* IFF display screen */
struct Screen *fontscreen=NULL;           /* Font display screen */
struct Screen *blankscreen=NULL;          /* Blank screen for IFF viewer */
struct Window *Window=NULL;               /* Main window */
struct Window *fontwindow=NULL;           /* Window for font display */
struct Window *ansiread_window;           /* Window for rendering ANSI text */
struct Window *reqoverride=NULL;          /* Pointer for SimpleRequest window */

/* Graphics objects */

struct ViewPort *main_vp;                 /* Main screen ViewPort */

struct RastPort *main_rp;                 /* Main window RastPort */
struct RastPort *font_rp;                 /* Font window RastPort */
struct RastPort dir_rp[2];                /* Directory window RastPorts */

struct TextFont *scr_font[NUMFONTS];      /* Fonts used in screen display */

/* Exec objects */

struct Process *main_proc;                /* Main Process */
struct Task *hotkey_task=NULL;            /* HotKey Task */
struct Task *clock_task=NULL;             /* Clock Task */

struct MsgPort *count_port=NULL;          /* Port to establish run count */
struct MsgPort *general_port=NULL;        /* General Message Port */
struct MsgPort *audio_port[2];            /* 8SVX Sound player Ports */
struct MsgPort *appmsg_port=NULL;         /* AppObject Message Port */
struct MsgPort *arexx_port=NULL;          /* ARexx Port */

struct MsgPort *hotkeymsg_port=NULL;      /* Messages to the hotkey task */
struct MsgPort *clockmsg_port=NULL;       /* Messages to the clock task */

struct IOStdReq *keyboard_req=NULL;       /* Keyboard IO Request block */
struct IOStdReq *input_req=NULL;          /* Input IO Request block */
struct IOAudio *audio_req1[2];            /* Audio IO Request block #1 */
struct IOAudio *audio_req2[2];            /* Audio IO Request block #2 */

/* DOS objects */

BPTR ramdisk_lock=0;                      /* Lock on RAM: */
APTR windowptr_save;                      /* Saved Window Pointer */
BPTR nil_file_handle;                     /* Handle on the NIL: device */

BPTR old_pr_cis;                          /* Old pr_CIS pointer */
BPTR old_pr_cos;                          /* Old pr_COS pointer */
void *old_pr_consoletask;                 /* Old pr_ConsoleTask pointer */

/* General objects */

struct DOpusRemember *general_key=NULL;   /* General memory key */
struct DOpusRemember *help_key=NULL;      /* Memory key for help files */
struct DOpusRemember *filetype_key=NULL;  /* Memory key for filetype list */
struct DOpusRemember *audio_key=NULL;     /* Memory key for 8SVX player */
struct DOpusRemember *menu_key=NULL;      /* Memory key for menus */

USHORT *nullpalette;                      /* Pointer to a black palette */
UBYTE *audiodata;                         /* Audio data to play */
ULONG audio_size;                           /* Size of audio data */

struct UserInfo *user_info;                 /* UserInfo for envoy */
struct GroupInfo *group_info;               /* GroupInfo for envoy */


/* Directory Opus Internal objects */

struct Directory
	*last_selected_entry=NULL;              /* The last entry selected */
struct Help *dopus_firsthelp=NULL;        /* The first help message */
struct dopusfiletype
	*dopus_firsttype=NULL;                  /* The first filetype */
struct dopusgadgetbanks
	*dopus_firstgadbank=NULL;               /* The first gadget bank */
struct dopusgadgetbanks
	*dopus_curgadbank=NULL;                 /* The current gadget bank */
struct dopushotkey
	*dopus_firsthotkey=NULL;                /* The first hotkey */
struct dopushotkey
	*dopus_globalhotkey=NULL;               /* The last hotkey function */

struct DirectoryWindow *dopus_curwin[2];  /* Currently displayed buffer */
struct DirectoryWindow
	*dopus_firstwin[2];                     /* First buffer in each window */
struct DirectoryWindow
	*dopus_specialwin[2];                   /* Special buffer for each window */
struct DirectoryWindow *dopus_oldwin[2];  /* Last buffer displayed */

void *dir_memory_pool=NULL;               /* Memory pool for allocations */


/* String data */

char str_config_basename[256];            /* Configuration base name */
char str_config_file[256];                /* Configuration file */
char str_dopusrt_path[256];               /* DOpusRT path name */

char str_select_info[80];                 /* Status bar text for selection info */
char *str_select_pattern[3];              /* Three different Select patterns */
char *str_arexx_command;                  /* Entered ARexx command string */
char *str_space_string;                   /* 255 spaces in a string */
char str_hunt_name[80];                   /* File name to hunt for */
char str_hunt_name_parsed[164];           /* Hunt file parsed */
char str_search_string[80];               /* String to search for */
char str_filter[40];                      /* File filter for recursive ops */
char str_filter_parsed[84];               /* File filter parsed */

char str_version_string[30];              /* Current version string */
char str_arexx_portname[30];              /* ARexx Port name */

UBYTE var_key_matrix[13];                 /* Keyboard matrix data area */
char *str_last_statustext;                /* Last status text displayed */

char str_last_rexx_result[256];           /* Last ARexx result */

/* Various data */

BYTE status_iconified=0;                  /* Current iconified status */
BYTE status_configuring=-1;               /* Configuring status */
BYTE status_justabort=0;                  /* Have we just aborted? */
BYTE status_haveaborted=0;                /* The abort sequence has been hit */
BYTE status_rexxabort=0;                  /* Abort flag for ARexx scripts */
BYTE status_previousabort;                /* Did we abort a while ago ? */
BYTE status_publicscreen;                 /* Are we on a public screen? */
ULONG status_flags;                       /* Various status bits */

BYTE data_active_window=0;                /* Active directory window */
UBYTE data_buffer_count[2];               /* Number of buffers in each window */
BYTE data_gadgetrow_offset=0;             /* Gadget row offset */
SHORT data_drive_offset=0;                /* Current drive offset */
ULONG data_colorclock;                    /* Color clock value for this system */

ULONG time_current_sec;                   /* Current Second (from Intuition) */
ULONG time_current_micro;                 /* Current Micro (from Intuition) */
ULONG time_previous_sec;                  /* Previous Second (from Intuition) */
ULONG time_previous_micro;                /* Previous Micro (from Intuition) */

UBYTE system_version2;                    /* OS version 2 or greater */
int system_dopus_runcount;                /* Run count of this copy */

ULONG data_window_refresh;                /* Windows that need refreshing */

USHORT scr_gadget_rows;                   /* Number of button rows */
USHORT scr_gadget_bank_count;             /* Number of button banks */

USHORT scr_min_width;                     /* Minimum window width */
USHORT scr_min_height;                    /* Minimum window height */

/* Configuration data */

struct Config *config;                    /* Configuration structure */
BPTR configopus_segment=NULL;             /* Segment pointer for ConfigOpus */
BPTR external_mod_segment[3];             /* External module segments */
BYTE config_changed=0;                    /* Configuration has been changed */

/* Disk and DOS operations data */

UBYTE disk_change_state;                  /* Lower bits indicate change state */

ULONG dos_global_bytecount;               /* Global byte count from recursive op */
ULONG dos_global_copiedbytes;             /* Bytes copied in a recursive op */
ULONG dos_global_deletedbytes;            /* Bytes copied in a recursive op */
ULONG dos_global_blocksneeded;            /* Number of blocks needed to copy */

struct DateStamp dos_copy_date;           /* DateStamp of file last copied */
ULONG dos_copy_protection;                /* Protection of file last copied */
char dos_copy_comment[80];                /* Comment of file last copied */

BOOL glob_unprotect_all;                  /* Unprotect all files */

BOOL global_swap_window=FALSE;            /* Active window changed */

struct Directory dos_global_entry;        /* Global single file entry */

struct NotifyRequest *dos_notify_req[2];  /* Notify requests for both windows */
char dos_notify_names[2][256];            /* Notify requests name buffers */

struct recpath *rec_firstpath;            /* Directory path list pointer */
struct DOpusRemember *rec_pathkey=NULL;   /* Memory key for dir list */

struct Requester dopus_busy_requester;    /* Busy requester */

char *icon_type_names[]={                 /* Names of different icon types */
	"Disk",
	"Drawer",
	"Tool",
	"Project",
	"Trash",
	"Device",
	"Kick",
	"AppIcon"};

struct DOpusRemember *recurse_dir_key;    /* Memory key for RecursiveDirectory's */
struct RecursiveDirectory *first_recurse; /* First recursive directory pointer */
int recurse_max_depth;                    /* Maximum depth reached */
struct RecursiveDirectory
	**recurse_parent_array;                 /* Array of parent directories */

struct path_complete completion[2];       /* Path-name completion data */

/* DateTime stuff */

char *date_months[12];                    /* Months */
char *date_shortmonths[12];               /* Short months */
char *date_weekdays[7];                   /* Weekdays */
char *date_shortweekdays[7];              /* Short weekdays */
char *date_special[4];                    /* Special days */

struct StringData stringdata={            /* String data */
	NULL,
	0,
	NULL,
	NULL,0,
	0};

