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
int _iniObjectRead( HINI hIni, char *szLine, char *pszObjectName );

/******************************
 * _iniPropertyRead
 *
 ******************************/
int _iniPropertyRead( HINI hIni, char *szLine, char *pszPropertyName, char *pszPropertyValue );

/******************************
 * _iniDump
 *
 ******************************/
int _iniDump( HINI hIni, FILE *hStream );

/******************************
 * _iniScanUntilObject
 *
 ******************************/
int _iniScanUntilObject( HINI hIni, FILE *hFile, char *pszLine );
int _iniScanUntilNextObject( HINI hIni, FILE *hFile, char *pszLine );

#if defined(__cplusplus)
         }
#endif

#endif
