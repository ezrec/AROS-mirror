/*************************************************************************\
 *                                                                       *
 * Apdf.h                                                                *
 *                                                                       *
 * Copyright 1999-2006 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

#ifndef APDF_H
#define APDF_H

#define apdfVersion "3.4"
#define apdfVerString "Apdf 3.4 (30.10.2010)"
#define vpdfVersion "2.4"
#define vpdfVerString "Vpdf.VPlug 2.4 (30.10.2010)"
#define LIB_VERSION  2
#define LIB_REVISION 4

#define ID_ABORT          1000

void* MyAllocMem(size_t);
void MyFreeMem(void*);
void CopyStr(char**,const char*);

void sleep(Object*,BOOL);
Object *create_gauge(Object *app,const char *title,int);
void delete_gauge(Object *);

extern BOOL is38;
extern Object *config;

#endif

