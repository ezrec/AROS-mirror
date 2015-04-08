/*************************************************************************\
 *                                                                       *
 * prefs.h                                                               *
 *                                                                       *
 * Copyright 1999-2001 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

#ifndef PREFS_H
#define PREFS_H

#ifndef MYTAG_START
#   define MYTAG_START       TAG_USER
#endif

#define MYATTR_Prefs_Plugin          (MYTAG_START+801) /* I.. */
#define MYATTR_Prefs_Page            (MYTAG_START+802) /* I.. */

//#define MYM_Prefs_Save               (MYTAG_START+1801)

#define MYCFG_Apdf_GzipCmd           (MYTAG_START+1)
#define MYCFG_Apdf_URLCmd            (MYTAG_START+2)
#define MYCFG_Apdf_PrintDev          (MYTAG_START+3)
#define MYCFG_Apdf_DefIcon           (MYTAG_START+4)
#define MYCFG_Apdf_DiskObj           (MYTAG_START+5)
#define MYCFG_Apdf_Cache             (MYTAG_START+6)
#define MYCFG_Apdf_Colors            (MYTAG_START+7)
#define MYCFG_Apdf_Zoom              (MYTAG_START+8)
#define MYCFG_Apdf_SelectColor       (MYTAG_START+9)
#define MYCFG_Apdf_FontPath          (MYTAG_START+10)
#define MYCFG_Apdf_Module            (MYTAG_START+11)
#define MYCFG_Apdf_TmpDir            (MYTAG_START+12)
#define MYCFG_Apdf_AntiAliasing      (MYTAG_START+13)
#define MYCFG_Apdf_AutoOpenLog       (MYTAG_START+14)
#define MYCFG_Apdf_NoPowerUp         (MYTAG_START+15)
#define MYCFG_Apdf_FontMap           (MYTAG_START+16)
#define MYCFG_Apdf_ModulesDir        (MYTAG_START+17)
#define MYCFG_Apdf_Continuous        (MYTAG_START+18)
#define MYCFG_Apdf_DisplayMode       (MYTAG_START+19)
#define MYCFG_Apdf_Columns           (MYTAG_START+20)
#define MYCFG_Apdf_Rows              (MYTAG_START+21)
#define MYCFG_Apdf_MakeBitMap        (MYTAG_START+22)
#define MYCFG_Apdf_SpareMem          (MYTAG_START+23)
#define MYCFG_Apdf_UpdateTime        (MYTAG_START+24)
#define MYCFG_Apdf_FullScreen        (MYTAG_START+25)
#define MYCFG_Apdf_ExtOutlines       (MYTAG_START+26)
#define MYCFG_Apdf_OpenOutlines      (MYTAG_START+27)


enum DrawMode {
    DrAllAtOnce,
    DrTextFirst,
    DrPartialUpdates
};
extern enum DrawMode draw_mode;
extern int update_time;
extern int make_bitmap;
extern int spare_mem;

extern struct MUI_CustomClass *prefs_mcc;

#define PrefsObject NewObject(prefs_mcc->mcc_Class,NULL

BOOL createPrefsClass(void);
BOOL deletePrefsClass(void);

struct CacheInfo {
    size_t size;
    size_t bloc_size;
};

BOOL save_prefs(Object *,const char *);
BOOL load_prefs(Object *,const char *);
void use_prefs(struct comm_info *,Object *);

#endif

