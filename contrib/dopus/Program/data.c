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
#ifndef __AROS__
#include <libraries/screennotify.h>
#include <libraries/pm.h>
#include <libraries/SysInfo.h>
#endif

/* SAS Detach information */

#ifndef __AROS__
unsigned long __stack=16384;               /* Stack size needed */
//long __priority=0;                         /* Default task priority */
long __BackGroundIO=0;                     /* IO off for non-beta */
char *__procname="dopus_task";             /* Task name */
char __stdiowin[] = "";
#endif

/* Library bases */

struct DOpusBase *DOpusBase/* = NULL*/;         /* dopus.library */
struct IntuitionBase *IntuitionBase = NULL; /* intuition.library */
struct GfxBase *GfxBase = NULL;             /* graphics.library */
struct Library *IconBase = NULL;            /* icon.library */
struct Library *DiskfontBase = NULL;        /* diskfont.library */
#ifdef USE_POWERPACKER
struct PPBase *PPBase = NULL;               /* powerpacker.library */
#endif
struct Library *LayersBase = NULL;          /* layers.library */
struct Library *WorkbenchBase = NULL;       /* workbench.library */
struct MusicBase *MUSICBase/* = NULL*/;         /* music.library */
struct Library *CxBase = NULL;              /* commodities.library */
struct Library *RexxSysBase = NULL;         /* rexxsyslib.library */
struct Library *AccountsBase/* = NULL*/;        /* accounts.library */
struct Library *UtilityBase = NULL;         /* utility.library */
struct Library *ScreenNotifyBase/* = NULL*/;    /* screennotify.library */
struct Library *CyberGfxBase/* = NULL*/;        /* cybergraphics.library */
struct PopupMenuBase *PopupMenuBase/* = NULL*/; /* popupmenu.library */
struct xfdMasterBase *xfdMasterBase/* = NULL*/; /* xfdmaster.library */
#ifdef USE_XADMASTER
struct xadMasterBase *xadMasterBase/* = NULL*/; /* xadmaster.library */
#endif
struct Library *SysInfoBase/* = NULL*/;         /* sysinfo.library */
struct Library *LocaleBase = NULL;          /* locale.library */
struct muBase *muBase/* = NULL*/;               /* multiuser.library */
struct Library *DataTypesBase=NULL;         /* datatypes.library */
struct Library *AmigaGuideBase=NULL;        /* amigaguide.library */
struct Library *GadToolsBase=NULL;          /* gadtools.library */

/* Intuition objects */

struct IntuiMessage *IMsg/*=NULL*/;           /* Main IntuiMessage pointer */
struct Screen *MainScreen/*=NULL*/;           /* Main screen */
struct Screen *iffscreen/*=NULL*/;            /* IFF display screen */
struct Screen *fontscreen/*=NULL*/;           /* Font display screen */
struct Screen *blankscreen/*=NULL*/;          /* Blank screen for IFF viewer */
struct Window *Window/*=NULL*/;               /* Main window */
struct Window *fontwindow/*=NULL*/;           /* Window for font display */
struct Window *ansiread_window/*=NULL*/;      /* Window for rendering ANSI text */
struct Window *reqoverride/*=NULL*/;          /* Pointer for SimpleRequest window */
struct PopupMenu *sortmenu/*=NULL*/;          /* Sort pop-up menu */

/* Graphics objects */

//struct ViewPort *main_vp=NULL;            /* Main screen ViewPort */
struct RastPort *main_rp/*=NULL*/;            /* Main window RastPort */
struct RastPort dir_rp[2]/*={{0},{0}}*/;      /* Directory window RastPorts */

struct TextFont *scr_font[NUMFONTS]/*={0}*/;  /* Fonts used in screen display */

/* Exec objects */

struct Process *main_proc/*=NULL*/;           /* Main Process */
struct Task *hotkey_task/*=NULL*/;            /* HotKey Task */
struct Task *clock_task/*=NULL*/;             /* Clock Task */

struct MsgPort *count_port/*=NULL*/;          /* Port to establish run count */
struct MsgPort *general_port/*=NULL*/;        /* General Message Port */
struct MsgPort *appmsg_port/*=NULL*/;         /* AppObject Message Port */
struct MsgPort *arexx_port/*=NULL*/;          /* ARexx Port */
struct MsgPort *snm_port/*=NULL*/;            /* ScreenNotify Port */

struct MsgPort *hotkeymsg_port/*=NULL*/;      /* Messages to the hotkey task */
struct MsgPort *clockmsg_port/*=NULL*/;       /* Messages to the clock task */

//struct IOStdReq *keyboard_req/*=NULL*/;       /* Keyboard IO Request block */
struct IOStdReq *input_req/*=NULL*/;          /* Input IO Request block */

/* DOS objects */

BPTR ramdisk_lock/*=0*/;                      /* Lock on RAM: */
APTR windowptr_save/*=NULL*/;                 /* Saved Window Pointer */
BPTR nil_file_handle/*=0*/;                   /* Handle on the NIL: device */

BPTR old_pr_cis/*=0*/;                        /* Old pr_CIS pointer */
BPTR old_pr_cos/*=0*/;                        /* Old pr_COS pointer */
void *old_pr_consoletask/*=NULL*/;            /* Old pr_ConsoleTask pointer */

/* General objects */

struct DOpusRemember *general_key/*=NULL*/;   /* General memory key */
struct DOpusRemember *help_key/*=NULL*/;      /* Memory key for help files */
struct DOpusRemember *filetype_key/*=NULL*/;  /* Memory key for filetype list */
struct DOpusRemember *menu_key/*=NULL*/;      /* Memory key for menus */

UWORD *nullpalette/*=NULL*/;                 /* Pointer to a black palette */

struct UserInfo *user_info/*=NULL*/;          /* UserInfo for envoy */
struct GroupInfo *group_info/*=NULL*/;        /* GroupInfo for envoy */
struct muUserInfo *mu_userinfo/*=NULL*/;          /* UserInfo for multiuser */
struct muGroupInfo *mu_groupinfo/*=NULL*/;        /* GroupInfo for multiuser */

struct ScreenNotifyMessage *snm/*=NULL*/;
struct Locale *locale/*=NULL*/;

/* Directory Opus Internal objects */

struct Directory
    *last_selected_entry/*=NULL*/;              /* The last entry selected */
struct Help *dopus_firsthelp/*=NULL*/;        /* The first help message */
struct dopusfiletype
    *dopus_firsttype/*=NULL*/;                  /* The first filetype */
struct dopusgadgetbanks
    *dopus_firstgadbank/*=NULL*/;               /* The first gadget bank */
struct dopusgadgetbanks
    *dopus_curgadbank/*=NULL*/;                 /* The current gadget bank */
struct dopushotkey
    *dopus_firsthotkey/*=NULL*/;                /* The first hotkey */
struct dopushotkey
    *dopus_globalhotkey/*=NULL*/;               /* The last hotkey function */

struct DirectoryWindow *dopus_curwin[2]/*={0}*/;  /* Currently displayed buffer */
struct DirectoryWindow
    *dopus_firstwin[2]/*={0}*/;                 /* First buffer in each window */
struct DirectoryWindow
    *dopus_specialwin[2]/*={0}*/;               /* Special buffer for each window */
struct DirectoryWindow *dopus_oldwin[2]/*={0}*/;  /* Last buffer displayed */

void *dir_memory_pool/*=NULL*/;               /* Memory pool for allocations */


/* String data */

char str_config_basename[256]/*={0}*/;        /* Configuration base name */
char str_config_file[256]/*={0}*/;            /* Configuration file */
char str_dopusrt_path[256]/*={0}*/;           /* DOpusRT path name */

char str_select_info[128]/*={0}*/;            /* Status bar text for selection info */
char *str_select_pattern[4]/*={0}*/;          /* Four different Select patterns */
char *str_arexx_command/*=NULL*/;             /* Entered ARexx command string */
char *str_space_string/*=NULL*/;              /* 255 spaces in a string */
char str_hunt_name[80]/*={0}*/;               /* File name to hunt for */
char str_hunt_name_parsed[164]/*={0}*/;       /* Hunt file parsed */
char str_search_string[80]/*={0}*/;           /* String to search for */
char str_filter[40]/*={0}*/;                  /* File filter for recursive ops */
char str_filter_parsed[84]/*={0}*/;           /* File filter parsed */
char str_arcorgname[FILEBUF_SIZE]/*={0}*/;    /* Original name of temporarily unarchived file */
char str_version_string[30]/*={0}*/;          /* Current version string */
char str_arexx_portname[30]/*={0}*/;          /* ARexx Port name */

//UBYTE var_key_matrix[13]/*={0}*/;             /* Keyboard matrix data area */
char *str_last_statustext/*=NULL*/;           /* Last status text displayed */

char *str_last_rexx_result/*=NULL*/;          /* Last ARexx result */

const char *str_protbits="HSPARWED";

/* Various data */

BYTE status_iconified/*=0*/;                  /* Current iconified status */
BYTE status_configuring=-1;               /* Configuring status */
BYTE status_justabort/*=0*/;                  /* Have we just aborted? */
BYTE status_haveaborted/*=0*/;                /* The abort sequence has been hit */
BYTE status_rexxabort/*=0*/;                  /* Abort flag for ARexx scripts */
BYTE status_previousabort/*=0*/;              /* Did we abort a while ago ? */
BYTE status_publicscreen/*=0*/;               /* Are we on a public screen? */
ULONG status_flags/*=0*/;                     /* Various status bits */

BYTE data_active_window/*=0*/;                /* Active directory window */
UBYTE data_buffer_count[2]/*={0}*/;           /* Number of buffers in each window */
BYTE data_gadgetrow_offset/*=0*/;             /* Gadget row offset */
WORD data_drive_offset/*=0*/;                /* Current drive offset */
ULONG data_colorclock/*=0*/;                  /* Color clock value for this system */

ULONG time_current_sec/*=0*/;                 /* Current Second (from Intuition) */
ULONG time_current_micro/*=0*/;               /* Current Micro (from Intuition) */
ULONG time_previous_sec/*=0*/;                /* Previous Second (from Intuition) */
ULONG time_previous_micro/*=0*/;              /* Previous Micro (from Intuition) */

UBYTE system_version2/*=0*/;                  /* OS version 2 or greater */
int system_dopus_runcount/*=0*/;              /* Run count of this copy */

UWORD scr_gadget_rows/*=0*/;                 /* Number of button rows */
UWORD scr_gadget_bank_count/*=0*/;           /* Number of button banks */

UWORD scr_min_width/*=0*/;                   /* Minimum window width */
UWORD scr_min_height/*=0*/;                  /* Minimum window height */

APTR snm_handle/*=NULL*/;                     /* ScreenNotify handle */
struct SysInfo *sysinfo/*=NULL*/;             /* SysInfo.library */

int askeach/* = 0*/;                          /* Should replace requester be displayed */
int autoskip/* = 0*/;                         /* Should files be automatically owerwritten */

/* Configuration data */

struct Config *config/*=NULL*/;               /* Configuration structure */
BPTR configopus_segment/*=NULL*/;             /* Segment pointer for ConfigOpus */
BPTR external_mod_segment[3]/*={0}*/;         /* External module segments */
BYTE config_changed/*=0*/;                    /* Configuration has been changed */

const char *config_replyport_basename="dopus4_config_reply";

/* Disk and DOS operations data */

UBYTE disk_change_state/*=0*/;                /* Lower bits indicate change state */

#ifdef __GNUC__
unsigned long long dos_global_bytecount/*=0*/;             /* Global byte count from recursive op */
unsigned long long dos_global_copiedbytes/*=0*/;           /* Bytes copied in a recursive op */
unsigned long long dos_global_deletedbytes/*=0*/;          /* Bytes copied in a recursive op */
#else
ULONG dos_global_bytecount/*=0*/;             /* Global byte count from recursive op */
ULONG dos_global_copiedbytes/*=0*/;           /* Bytes copied in a recursive op */
ULONG dos_global_deletedbytes/*=0*/;          /* Bytes copied in a recursive op */
#endif
ULONG dos_global_blocksneeded/*=0*/;          /* Number of blocks needed to copy */
ULONG dos_global_files/*=0*/;		          /* Number of files found in a recursive op */

struct DateStamp dos_copy_date/*={0}*/;       /* DateStamp of file last copied */
ULONG dos_copy_protection/*=0*/;              /* Protection of file last copied */
char dos_copy_comment[80]/*={0}*/;            /* Comment of file last copied */

BOOL glob_unprotect_all/*=0*/;                /* Unprotect all files */

BOOL global_swap_window=FALSE;            /* Active window changed */

struct Directory dos_global_entry/*={0}*/;    /* Global single file entry */

struct NotifyRequest *dos_notify_req[2]/*={0}*/;  /* Notify requests for both windows */
char dos_notify_names[2][256]/*={{0},{0}}*/;  /* Notify requests name buffers */

struct recpath *rec_firstpath/*=NULL*/;       /* Directory path list pointer */
struct DOpusRemember *rec_pathkey/*=NULL*/;   /* Memory key for dir list */

char *icon_type_names[]={                 /* Names of different icon types */
    "Disk",
    "Drawer",
    "Tool",
    "Project",
    "Trash",
    "Device",
    "Kick",
    "AppIcon"};

struct DOpusRemember *recurse_dir_key/*=NULL*/;    /* Memory key for RecursiveDirectory's */
struct RecursiveDirectory *first_recurse/*=NULL*/; /* First recursive directory pointer */
int recurse_max_depth/*=0*/   ;                    /* Maximum depth reached */

struct path_complete completion[2]/*={{0},{0}}*/;  /* Path-name completion data */

/* DateTime stuff */

//char *date_months[12]/*={0}*/;                    /* Months */
//char *date_shortmonths[12]/*={0}*/;               /* Short months */
//char *date_weekdays[7]/*={0}*/;                   /* Weekdays */
//char *date_shortweekdays[7]/*={0}*/;              /* Short weekdays */
//char *date_special[4]/*={0}*/;                    /* Special days */

struct StringData stringdata/*={            // String data
    NULL,
    0,
    NULL,
    NULL,0,
    0,
    NULL,NULL}*/; // JRZ

extern UWORD __chip appicon_data[];
struct Image appicon_image = { 0,0,80,40,2,appicon_data,3,0,0};
struct DiskObject *user_appicon/* = NULL*/; // HUX: User selected app icon
