#ifndef CLIB_WORKBENCH_PROTOS_H
#define CLIB_WORKBENCH_PROTOS_H

/*
    *** Automatically generated from 'workbench.conf'. Edits will be lost. ***
    Copyright © 1995-2008, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>
#include <workbench/workbench.h>
#include <dos/bptr.h>

/* Prototypes for stubs in amiga.lib */
struct AppIcon * AddAppIcon( ULONG id, ULONG userdata, STRPTR text, struct MsgPort * msgport, BPTR lock, struct DiskObject * diskobj, Tag tag1, ... ) __stackparm;
struct AppMenuItem * AddAppMenuItem( ULONG id, ULONG userdata, STRPTR text, struct MsgPort * msgport, Tag tag1, ... ) __stackparm;
struct AppWindow * AddAppWindow( ULONG id, ULONG userdata, struct Window * window, struct MsgPort * msgport, Tag tag1, ... ) __stackparm;
struct AppWindowDropZone * AddAppWindowDropZone( struct AppWindow * aw, ULONG id, ULONG userdata, Tag tag1, ... ) __stackparm;
BOOL CloseWorkbenchObject( STRPTR name, Tag tag1, ... ) __stackparm;
BOOL MakeWorkbenchObjectVisible( STRPTR name, Tag tag1, ... ) __stackparm;
BOOL OpenWorkbenchObject( STRPTR name, Tag tag1, ... ) __stackparm;
BOOL WorkbenchControl( STRPTR name, Tag tag1, ... ) __stackparm;
AROS_LP5(struct AppWindow *, AddAppWindowA,
         AROS_LPA(ULONG, id, D0),
         AROS_LPA(ULONG, userdata, D1),
         AROS_LPA(struct Window *, window, A0),
         AROS_LPA(struct MsgPort *, msgport, A1),
         AROS_LPA(struct TagItem *, taglist, A2),
         LIBBASETYPEPTR, WorkbenchBase, 8, Workbench
);
AROS_LP1(BOOL, RemoveAppWindow,
         AROS_LPA(struct AppWindow *, appWindow, A0),
         LIBBASETYPEPTR, WorkbenchBase, 9, Workbench
);
AROS_LP7(struct AppIcon *, AddAppIconA,
         AROS_LPA(ULONG, id, D0),
         AROS_LPA(ULONG, userdata, D1),
         AROS_LPA(char *, text, A0),
         AROS_LPA(struct MsgPort *, msgport, A1),
         AROS_LPA(BPTR, lock, A2),
         AROS_LPA(struct DiskObject *, diskobj, A3),
         AROS_LPA(struct TagItem *, taglist, A4),
         LIBBASETYPEPTR, WorkbenchBase, 10, Workbench
);
AROS_LP1(BOOL, RemoveAppIcon,
         AROS_LPA(struct AppIcon *, appIcon, A0),
         LIBBASETYPEPTR, WorkbenchBase, 11, Workbench
);
AROS_LP5(struct AppMenuItem *, AddAppMenuItemA,
         AROS_LPA(ULONG, id, D0),
         AROS_LPA(ULONG, userdata, D1),
         AROS_LPA(APTR, text, A0),
         AROS_LPA(struct MsgPort *, msgport, A1),
         AROS_LPA(struct TagItem *, taglist, A3),
         LIBBASETYPEPTR, WorkbenchBase, 12, Workbench
);
AROS_LP1(BOOL, RemoveAppMenuItem,
         AROS_LPA(struct AppMenuItem *, appMenuItem, A0),
         LIBBASETYPEPTR, WorkbenchBase, 13, Workbench
);
AROS_LP3(BOOL, WBInfo,
         AROS_LPA(BPTR, lock, A0),
         AROS_LPA(CONST_STRPTR, name, A1),
         AROS_LPA(struct Screen *, screen, A2),
         LIBBASETYPEPTR, WorkbenchBase, 15, Workbench
);
AROS_LP2(BOOL, OpenWorkbenchObjectA,
         AROS_LPA(STRPTR, name, A0),
         AROS_LPA(struct TagItem *, tags, A1),
         LIBBASETYPEPTR, WorkbenchBase, 16, Workbench
);
AROS_LP2(BOOL, CloseWorkbenchObjectA,
         AROS_LPA(STRPTR, name, A0),
         AROS_LPA(struct TagItem *, tags, A1),
         LIBBASETYPEPTR, WorkbenchBase, 17, Workbench
);
AROS_LP2(BOOL, WorkbenchControlA,
         AROS_LPA(STRPTR, name, A0),
         AROS_LPA(struct TagItem *, tags, A1),
         LIBBASETYPEPTR, WorkbenchBase, 18, Workbench
);
AROS_LP4(struct AppWindowDropZone *, AddAppWindowDropZoneA,
         AROS_LPA(struct AppWindow *, aw, A0),
         AROS_LPA(ULONG, id, D0),
         AROS_LPA(ULONG, userdata, D1),
         AROS_LPA(struct TagItem *, tags, A1),
         LIBBASETYPEPTR, WorkbenchBase, 19, Workbench
);
AROS_LP2(BOOL, RemoveAppWindowDropZone,
         AROS_LPA(struct AppWindow *, aw, A0),
         AROS_LPA(struct AppWindowDropZone *, dropZone, A1),
         LIBBASETYPEPTR, WorkbenchBase, 20, Workbench
);
AROS_LP3(BOOL, ChangeWorkbenchSelectionA,
         AROS_LPA(STRPTR, name, A0),
         AROS_LPA(struct Hook *, hook, A1),
         AROS_LPA(struct TagItem *, tags, A2),
         LIBBASETYPEPTR, WorkbenchBase, 21, Workbench
);
AROS_LP2(BOOL, MakeWorkbenchObjectVisibleA,
         AROS_LPA(STRPTR, name, A0),
         AROS_LPA(struct TagItem *, tags, A1),
         LIBBASETYPEPTR, WorkbenchBase, 22, Workbench
);
AROS_LP1(BOOL, RegisterWorkbench,
         AROS_LPA(struct MsgPort *, messageport, A0),
         LIBBASETYPEPTR, WorkbenchBase, 23, Workbench
);
AROS_LP1(BOOL, UnregisterWorkbench,
         AROS_LPA(struct MsgPort *, messageport, A0),
         LIBBASETYPEPTR, WorkbenchBase, 24, Workbench
);
AROS_LP3(BOOL, UpdateWorkbenchObjectA,
         AROS_LPA(STRPTR, name, A0),
         AROS_LPA(LONG, type, D1),
         AROS_LPA(struct TagItem *, tags, A1),
         LIBBASETYPEPTR, WorkbenchBase, 25, Workbench
);
AROS_LP8(BOOL, SendAppWindowMessage,
         AROS_LPA(struct Window *, win, A0),
         AROS_LPA(ULONG, numfiles, D0),
         AROS_LPA(char **, files, A1),
         AROS_LPA(UWORD, class, D1),
         AROS_LPA(WORD, mousex, D2),
         AROS_LPA(WORD, mousey, D3),
         AROS_LPA(ULONG, seconds, D4),
         AROS_LPA(ULONG, micros, D5),
         LIBBASETYPEPTR, WorkbenchBase, 26, Workbench
);
AROS_LP2(struct DiskObject *, GetNextAppIcon,
         AROS_LPA(struct DiskObject *, lastdiskobj, A0),
         AROS_LPA(char*, text, A1),
         LIBBASETYPEPTR, WorkbenchBase, 27, Workbench
);

#endif /* CLIB_WORKBENCH_PROTOS_H */
