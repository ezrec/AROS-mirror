/*************************************************************************\
 *                                                                       *
 * comm.h                                                                *
 *                                                                       *
 * Copyright 2000-2001 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

#ifndef COMM_H
#define COMM_H

#ifndef MYTAG_START
#   define MYTAG_START       TAG_USER
#endif

#define MYATTR_Comm_Application      (MYTAG_START+101) /* I.. */

#define MYM_Comm_Handle              (MYTAG_START+1101)
#define MYM_Comm_StartNext           (MYTAG_START+1102)

Object *initComm(Object *app);
void cleanupComm(void);

extern Object *comm;

#endif

