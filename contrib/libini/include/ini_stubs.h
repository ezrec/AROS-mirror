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

#ifndef INCLUDED_INI_STUBS_H
#define INCLUDED_INI_STUBS_H

#include <aros/libcall.h>

#include <proto/inifile.h>

/******************************
 * _iniObjectRead
 *
 ******************************/
int _iniObjectRead( HINI hIni, char *szLine, char *pszObjectName, struct Library *inifileBase );
#define _iniObjectRead(x...) _iniObjectRead(x, inifileBase)

/******************************
 * _iniPropertyRead
 *
 ******************************/
int _iniPropertyRead( HINI hIni, char *szLine, char *pszPropertyName, char *pszPropertyValue, struct Library *inifileBase );
#define _iniPropertyRead(x...) _iniPropertyRead(x, inifileBase)

/******************************
 * _iniDump
 *
 ******************************/
int _iniDump( HINI hIni, FILE *hStream, struct Library *inifileBase );
#define _iniDump(x...) _iniDump(x, inifileBase ) 

/******************************
 * _iniScanUntilObject
 *
 ******************************/
int _iniScanUntilObject( HINI hIni, FILE *hFile, char *pszLine, struct Library *inifileBase );
int _iniScanUntilNextObject( HINI hIni, FILE *hFile, char *pszLine);

#define _iniScanUntilObject(x...) _iniScanUntilObject(x, inifileBase)

#if defined(__cplusplus)
         }
#endif

#endif
