/*************************************************************************\
 *                                                                       *
 * notifier.h                                                            *
 *                                                                       *
 * Copyright 2001 Emmanuel Lesueur                                       *
 *                                                                       *
\*************************************************************************/

#ifndef NOTIFIER_H
#define NOTIFIER_H

#ifndef MYTAG_START
#   define MYTAG_START       TAG_USER
#endif

#define MYATTR_Notifier_Name            (MYTAG_START+190) /* I.. */
#define MYATTR_Notifier_Changed         (MYTAG_START+191) /* ..G */

#define MYM_NotifierBase_Handle         (MYTAG_START+1190) /* private */

extern struct MUI_CustomClass *notifier_mcc;

#define NotifierObject NewObject(notifier_mcc->mcc_Class,NULL

BOOL initNotifier(Object *app);
void cleanupNotifier(void);

#endif

