/**********************************************************************************
 * ini.h
 *
 * Include file for libini.a. Coding? Include this and link against libini.a.
 *
 *
 **************************************************
 * This code was created by Peter Harvey @ CodeByDesign.
 * Released under LGPL 28.JAN.99
 *
 * Contributions from...
 * -----------------------------------------------
 * Peter Harvey		- pharvey@codebydesign.com
 **************************************************/

#ifndef INCLUDED_INI_H
#define INCLUDED_INI_H

/*********[ CONSTANTS AND TYPES ]**************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

#define     INI_NO_DATA             2
#define     INI_SUCCESS             1
#define     INI_ERROR               0

#define     INI_MAX_LINE            1000
#define     INI_MAX_OBJECT_NAME     INI_MAX_LINE
#define     INI_MAX_PROPERTY_NAME   INI_MAX_LINE
#define     INI_MAX_PROPERTY_VALUE  INI_MAX_LINE

#if HAVE_LIMITS_H
#include <limits.h>
#endif

#ifdef PATH_MAX
#define ODBC_FILENAME_MAX         PATH_MAX
#elif MAXPATHLEN
#define ODBC_FILENAME_MAX         MAXPATHLEN
#else
#define ODBC_FILENAME_MAX         FILENAME_MAX
#endif

/********************************************
 * tINIPROPERTY
 *
 * Each property line has Name=Value pair.
 * They are stored in this structure and linked together to provide a list
 * of all properties for a given Object.
 ********************************************/

typedef struct	tINIPROPERTY
{
	struct	tINIPROPERTY	*pNext;
	struct	tINIPROPERTY	*pPrev;

	char	szName[INI_MAX_PROPERTY_NAME+1];
	char	szValue[INI_MAX_PROPERTY_VALUE+1];

} INIPROPERTY, *HINIPROPERTY;

/********************************************
 * tINIOBJECT
 *
 * Each object line has just an object name. This structure
 * stores the object name and its subordinate information.
 * The lines that follow are considered to be properties
 * and are stored in a list of tINIPROPERTY.
 ********************************************/

typedef struct	tINIOBJECT
{
	struct	tINIOBJECT	*pNext;
	struct	tINIOBJECT	*pPrev;

	char	szName[INI_MAX_OBJECT_NAME+1];

    HINIPROPERTY		hFirstProperty;
    HINIPROPERTY		hLastProperty;
	int					nProperties;

} INIOBJECT, *HINIOBJECT;

/********************************************
 * tINI
 *
 * Each INI file contains a list of objects. This
 * structure stores each object in a list of tINIOBJECT.
 ********************************************/

typedef struct     tINI
{
    char    szFileName[ODBC_FILENAME_MAX+1];             /* FULL INI FILE NAME                                                           */
    char    cComment;                               /* COMMENT CHAR MUST BE IN FIRST COLUMN                                         */
    char    cLeftBracket;                           /* BRACKETS DELIMIT THE OBJECT NAME, THE LEFT BRACKET MUST BE IN COLUMN ONE     */
    char    cRightBracket;
    char    cEqual;                                 /* SEPERATES THE PROPERTY NAME FROM ITS VALUE                                   */
	int		bChanged;								/* IF true, THEN THE WHOLE FILE IS OVERWRITTEN UPON iniCommit					*/
	int		bReadOnly;								/* TRUE IF AT LEAST ONE CALL HAS BEEN MADE TO iniAppend() 						*/

	HINIOBJECT		hFirstObject;
	HINIOBJECT		hLastObject;
	HINIOBJECT		hCurObject;
	int				nObjects;

	HINIPROPERTY	hCurProperty;

} INI, *HINI;

/********************************************
 * tINIBOOKMARK
 *
 * Used to store the current Object and Property pointers so
 * that the caller can quickly return to some point in the
 * INI data.
 ********************************************/

typedef struct tINIBOOKMARK
{
	HINI			hIni;
	HINIOBJECT		hCurObject;
    HINIPROPERTY	hCurProperty;

} INIBOOKMARK, *HINIBOOKMARK;

#if defined(__cplusplus)
         extern  "C" {
#endif

/*********[ PRIMARY INTERFACE ]*****************************************************/

#include "ini_stubs.h"

#if defined(__cplusplus)
         }
#endif

#endif
