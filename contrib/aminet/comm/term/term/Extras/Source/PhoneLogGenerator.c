#ifndef _HEADERS_H
#include "Headers.h"
#endif	/* _HEADERS_H */

/* Copyright © 1996 Kai Hofmann. All rights reserved.
******* PhoneLog/--history-- ************************************************
*
*   NAME
*	history -- This is the development history of the PhoneLog library
*
*   VERSION
*	$VER: PhoneLog 1.1 (15.03.96)
*
*   HISTORY
*	18.02.1996 -	Concept
*	19.02.1996 -	Implementation
*	05.03.1996 -	Writing Autodocs
*	06.03.1996 -	Improving generator and parser
*	07.03.1996 -	Wrinting Autodocs and improvements for the parser
*	15.03.1996 -	Fixing bugs - now the date/time-format is in
*			correct ISO8601 format
*			closing SGML element now correctly using '/' instead
*			of '\'
*			Better scanner - now supports empty elements
*			Changed SGML DTD - HOSTNAME ist now optional
*			Parser now initialize and insert entry and mark
*			correctly
*			Parser now checks strings for length limitation
*
*****************************************************************************
*
*
*/

/*
******* PhoneLog/--release-- ************************************************
*
*   NAME
*	release -- This is the release history of the PhoneLog library
*
*   RELEASE
*	08.03.1996 : V1.0 -	First release on Aminet
*	15.03.1996 : v1.1 -	Second Aminet release
*
*****************************************************************************
*
*
*/

/*
******* PhoneLog/--todo-- ***************************************************
*
*   NAME
*	todo -- This is the 'To-Do' list of the PhoneLog library
*
*   TODO
*	Nothing at the moment
*
*****************************************************************************
*
*
*/

/*
******* PhoneLog/--announce-- ***********************************************
*
*   TITLE
*	PhoneLog Developer Kit
*
*   VERSION
*	1.1
*
*   RELEASE DATE
*	15.03.1996
*
*   AUTHOR
*	Kai Hofmann (i07m@zfn.uni-bremen.de)
*	            (http://www.informatik.uni-bremen.de/~i07m)
*
*   DESCRIPTION
*	Generator and parser for programs that want to write and read log
*	file entries about modem connections in a standard SGML format.
*	Full ANSI-C/C++ source code included.
*
*   NEW FEATURES
*	- Important bug fixes
*	- Improved code
*
*   SPECIAL REQUIREMENTS
*	ANSI-C or/and C++ Compiler.
*
*   AVAILABILITY
*	ftp://wuarchive.wustl.edu/pub/aminet/dev/c/PhoneLogDevKit.lha
*	And all other Aminet sites.
*
*   PRICE
*	For NON-COMMERCIAL USE this is Giftware!
*	(Non-commercial includes Giftware and Shareware!)
*
*	Permission for COMMERCIAL USE is only given by an extra available
*	commercial license that must be validated!
*	Contact me directly for this license, because it will be individually
*	handed out per your needs!
*
*	But in both cases you have to follow the two guidelines below:
*
*	1) You "must" send me a full version of your product at no cost
*	   including free updates!
*
*   DISTRIBUTION
*	You can copy and distribute this source code as long as you do not
*	take more than $5 for one disk or $40 for one CD!
*
*****************************************************************************
*
*
*/

/*
******* PhoneLog/--compiling-- **********************************************
*
*   NAME
*	compiling -- Specials for compiling the PhoneLog library
*
*   COMPILING
*	- You could compile this code as normal C or as C++
*	- You need only the follwoing files to include:
*	  PhoneLog.h, PhoneLogGenerator.h, PhoneLogParserInterface.h
*	  If you want to modifiy the scanner and/or parser, you need the
*	  following software: Aminet:dev/misc/Toolbox.lha to translate the
*	  .rex and .lalr file into C code!
*
*****************************************************************************
*
*
*/

/*
******* PhoneLog/--background-- *********************************************
*
*   NAME
*	PhoneLog -- Write and read entries to/from a log file (V33)
*
*   FUNCTION
*	This module has been designed to become a useful and portable library
*	and to help developers to write and read entries to/from a log file
*	in a standard SGML format.
*
*   NOTES
*	It seems that there was no standard for log files that are used
*	to log modem connections etc.
*	So I decided to create a standard based on SGML, because SGML
*	uses the ASCII characterset as base, so it can be used on every
*	platform. On the other hand SGML gives the log file a real structure
*	that is defined by the SGML DTD.
*	By supporting this standard with your software you give the user the
*	possibility to create or evaluate log files with software from
*	different platforms; and you get a standard that is easily extended
*	if a need occurs.
*
*	SGML : Standard Generalized Markup Language
*	DTD  : Document Type Definition
*
*	(English) Books which were consulted in creating this library:
*	    The SGML Handbook
*	    Charles F. Goldfarb
*	    First Edition
*	    Oxford University Press, Walton Street, Oxford (USA) 1990
*	    ISBN 0-19-853737-9
*
*   COPYRIGHT
*	This software is copyright 1996 by Kai Hofmann.
*	All rights reserved!
*
*	- Permission for COMMERCIAL USE is only given by an extra available
*	  commercial license that must be validated!
*	  Contact me directly for this license, because it will be
*	  individually handed out per your needs!
*
*	- Permission is hereby granted, without written agreement and without
*	  license, to USE this software and its documentation for any
*	  NON-COMMERCIAL purpose, provided that the above copyright notice
*	  and the following paragraph appear in all copies of this software
*	  (Non-commercial includes Giftware and Shareware!).
*
*	- THERE IS *NO* PERMISSION GIVEN TO REDISTRIBUTE THIS SOFTWARE IN A
*	  MODIFIED FORM!
*
*	  You "must" send me a full version of your product at no cost
*	  including free updates!
*	  Extra money is welcome (For Bank Account see below - but *ONLY*
*	  send in DM to this Bank Account!).
*
*   DISCLAIMER
*	THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY
*	APPLICABLE LAW. EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT
*	HOLDER AND/OR OTHER PARTIES PROVIDE THE PROGRAM "AS IS" WITHOUT
*	WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT
*	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*	A PARTICULAR PURPOSE. THE ENTIRE RISK AS TO THE QUALITY AND
*	PERFORMANCE OF THE PROGRAM IS WITH YOU. SHOULD THE PROGRAM PROVE
*	DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR
*	CORRECTION.
*
*	IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING
*	WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY REDISTRIBUTE
*	THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES,
*	INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES
*	ARISING OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING
*	BUT NOT LIMITED TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR
*	LOSSES SUSTAINED BY YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM
*	TO OPERATE WITH ANY OTHER PROGRAMS), EVEN IF SUCH HOLDER OR OTHER
*	PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
*
*	THE AUTHOR HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT,
*	UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
*
*   DISTRIBUTION
*	Permission is hereby granted, without written agreement and without
*	license or royalty fees, to copy and distribute this software and its
*	documentation for any purpose, provided that the above copyright
*	notice and the following paragraphs appear in all copies of this
*	software, to:
*	- All who will distribute this software for free!
*	- All free accessible INTERNET servers and PHONE boxes!
*	- All Aminet sites
*	- All SimTel sites
*	- Fred Fish for his great Amiga-Software-Library
*	- The German SAAR AG PD-Library
*	- All others who do NOT take more than $5.- for one disk that
*	  includes this software!
*	- ALL others who do NOT take more than $40.- for one CD that includes
*	  this software!
*
*   ADDITIONAL INFORMATIONS
*	I have tried to make portable/useful and I hope bugfree software
*	for eternity :)
*	So I hope you will pay a fee for this.
*
*	Kindly send US - dollars to a friend of mine in the USA who will
*	forward it to me in a timely manner.  Please send checks or money
*	orders only.
*	Contact me via email for more details!
*
*   AUTHOR
*	Kai Hofmann
*	Arberger Heerstraﬂe 92
*	28307 Bremen
*	Germany
*
*	Phone: (+49)-(0)421/480780
*	       (Remember that my parents don't speak english!)
*	EMail: i07m@zfn.uni-bremen.de
*	       i07m@informatik.uni-bremen.de
*	IRC  : PowerStat@#AmigaGer
*	WWW  : http://www.informatik.uni-bremen.de/~i07m
*
*	Bank account : 1203 7503
*	Account owner: Kai Hofmann
*	Bank code    : 290 501 01
*	Bank name    : Sparkasse in Bremen/Germany
*
*    THANKS
*	Thank you's are going to the following people:
*	Rita Reichl		- For correcting my English.
*	James Cooper and the
*	other Amiga people at
*	SAS Intitute		- For spending their unpaid free time with
*				  continuation of the Amiga SAS C/C++
*				  support :)
*
*****************************************************************************
*
*
*/


#ifndef _PHONELOGGENERATOR_H
#include "PhoneLogGenerator.h"
#endif	/* _PHONELOGGENERATOR_H */

#ifndef _PHONELOG_H
#include "PhoneLog.h"
#endif	/* _PHONELOG_H */

#include <stdarg.h>

#ifndef EXEC_MEMORY_H
#include <exec/memory.h>
#endif

#ifndef DOS_DOS_H
#include <dos/dos.h>
#endif

#ifndef CLIB_DOS_PROTOS_H
#include <clib/dos_protos.h>
#endif

#ifndef CLIB_EXEC_PROTOS_H
#include <clib/exec_protos.h>
#endif

extern struct ExecBase		*SysBase;
extern struct DosLibrary	*DOSBase;

#ifdef __AROS__
#define LogPrintf(h,f,...) \
	do { IPTR args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
	_LogPrintf(h,f,args); } while (0)
STATIC void
_LogPrintf(PhoneLogHandle *handle,char *format,IPTR *args)
{
	if(handle->error == 0)
		if(VFPrintf(handle->file,format,(APTR)args) == -1)
			handle->error = IoErr();
}
#else
STATIC void
LogPrintf(PhoneLogHandle *handle,char *format,...)
{
	if(handle->error == 0)
	{
		va_list args;

		va_start(args,format);

		if(VFPrintf(handle->file,format,(APTR)args) == -1)
			handle->error = IoErr();

		va_end(args);
	}
}
#endif

/*
******* PhoneLogGenerator/OpenPhoneLog **************************************
*
*   NAME
*	OpenPhoneLog -- Opens a log file for write operations (V33)
*
*   SYNOPSIS
*	file = OpenPhoneLog(name);
*
*	BPTR OpenPhoneLog(char *name);
*
*   FUNCTION
*	Opens a log file for appending new entries. If the log file doesn't
*	exist, a new one will be created.
*
*   INPUTS
*	name - The name of the log file.
*
*   RESULT
*	file - stdio.h file descriptor for a level 2 file or NULL if an error
*	    occurs.
*
*   EXAMPLE
*	...
*	BPTR file;
*
*	file = OpenPhoneLog("AmiTCP:log/AmiLog.log");
*	...
*	ClosePhoneLog(file);
*	...
*
*   NOTES
*	None
*
*   BUGS
*	No known bugs.
*
*   SEE ALSO
*	ClosePhoneLog(),WritePhoneLogStartEntry(),WritePhoneLogEndEntry(),
*	WritePhoneLogEntry(),WritePhoneLogMark()
*
*****************************************************************************
*
*
*/

PhoneLogHandle *
OpenPhoneLog(char *name)
{
	PhoneLogHandle *handle;
	BPTR fileLock;
	BPTR file;

	if(handle = AllocVec(sizeof(PhoneLogHandle),MEMF_ANY|MEMF_CLEAR))
	{
		if(fileLock = Lock(name,EXCLUSIVE_LOCK))
		{
			if(!(file = OpenFromLock(fileLock)))
			{
				UnLock(fileLock);

				file = Open(name,MODE_READWRITE);
			}

			if(file)
			{
				if(Seek(file,0,OFFSET_END) != -1)
				{
					handle->file = file;

					return(handle);
				}

				Close(file);
			}
		}
		else
		{
			if(file = Open(name,MODE_NEWFILE))
			{
				handle->file = file;

				LogPrintf(handle,"<PHONELOG");
				LogPrintf(handle," version=1 revision=1");
				LogPrintf(handle,">\n");

				if(handle->error != 0)
				{
					Close(file);
					DeleteFile(name);

					SetIoErr(handle->error);

					FreeVec(handle);

					return(NULL);
				}

				return(handle);
			}
		}

		FreeVec(handle);
	}

	return(NULL);
}


/*
******* PhoneLogGenerator/ClosePhoneLog *************************************
*
*   NAME
*	ClosePhoneLog -- Close a log file (V33)
*
*   SYNOPSIS
*	ClosePhoneLog(file);
*
*	void ClosePhoneLog(BPTR file);
*
*   FUNCTION
*	Close a log file opened by OpenPhoneLog().
*
*   INPUTS
*	file - The stdio.h file descriptor for a level 2 file that you got
*	    from OpenPhoneLog().
*
*   RESULT
*	None
*
*   EXAMPLE
*	...
*	BPTR file;
*
*	file = OpenPhoneLog("AmiTCP:log/AmiLog.log");
*	...
*	ClosePhoneLog(file);
*	...
*
*   NOTES
*	None
*
*   BUGS
*	No known bugs.
*
*   SEE ALSO
*	OpenPhoneLog(),WritePhoneLogStartEntry(),WritePhoneLogEndEntry(),
*	WritePhoneLogEntry(),WritePhoneLogMark()
*
*****************************************************************************
*
*
*/

void
ClosePhoneLog(PhoneLogHandle *handle)
{
	if(handle)
	{
		Close(handle->file);
		FreeVec(handle);
	}
}


/*
******* PhoneLogGenerator/WritePhoneLogStartEntry ***************************
*
*   NAME
*	WritePhoneLogStartEntry -- Write the start to a log file (V33)
*
*   SYNOPSIS
*	WritePhoneLogStartEntry(file, item);
*
*	void WritePhoneLogStartEntry(BPTR file,
*	    struct PhoneLogEntry *item);
*
*   FUNCTION
*	Write the start data for a connection to a log file opened by
*	OpenPhoneLog().
*
*   INPUTS
*	file - The stdio.h file descriptor for a level 2 file that you got
*	    from OpenPhoneLog().
*	item - PhoneLogEntry structure. You must fill in following fields:
*	    Number, StartDay, StartMonth, StartYear, StartHour,
*	    StartMin, StartSec.
*	    The fields Name and Reason are optional and will only be written
*	    if the string length is greater than 0.
*
*   RESULT
*	None
*
*   EXAMPLE
*	...
*	BPTR file;
*	struct PhoneLogEntry item;
*
*	file = OpenPhoneLog("AmiTCP:log/AmiLog.log");
*	...
*	strcpy(item.Number,"1234567890");
*	strcpy(item.Name,"University");
*	item.StartDay = 5;
*	item.StartMonth = 3;
*	item.StartYear = 1996; \* NOT 96! *\
*	item.StartHour = 12;
*	item.StartMin = 3;
*	item.StartSec = 0;
*	WritePhoneLogStartEntry(file,item);
*	...
*	ClosePhoneLog(file);
*	...
*
*   NOTES
*	Set Reason[0] = '\0' if you not want that this will be written to
*	the log file.
*
*   BUGS
*	No known bugs.
*
*   SEE ALSO
*	OpenPhoneLog(),ClosePhoneLog(),WritePhoneLogEndEntry(),
*	WritePhoneLogEntry(),WritePhoneLogMark()
*
*****************************************************************************
*
*
*/

void
WritePhoneLogStartEntry(PhoneLogHandle *handle, struct PhoneLogEntry *item)
{
	if(handle != NULL && item != NULL)
	{
		LogPrintf(handle,"\n<ENTRY>\n");
		LogPrintf(handle,"  <HOST>\n");
		LogPrintf(handle,"    <NUMBER>");
		LogPrintf(handle,"%s",item->Number);
		LogPrintf(handle,"</NUMBER>\n");

		if (item->Name[0] != '\0')
		{
			LogPrintf(handle,"    <HOSTNAME>");
			LogPrintf(handle,"%s",item->Name);
			LogPrintf(handle,"</HOSTNAME>\n");
		}

		if (item->Reason[0] != '\0')
		{
			LogPrintf(handle,"    <REASON>");
			LogPrintf(handle,"%s",item->Reason);
			LogPrintf(handle,"</REASON>\n");
		}

		LogPrintf(handle,"  </HOST>\n");
		LogPrintf(handle,"  <START>\n");
		LogPrintf(handle,"    <DATE>");
		LogPrintf(handle,"%04ld-%02ld-%02ld",item->StartYear,item->StartMonth,item->StartDay);
		LogPrintf(handle,"</DATE>\n");
		LogPrintf(handle,"    <TIME>");
		LogPrintf(handle,"%02ld:%02ld:%02ld",item->StartHour,item->StartMin,item->StartSec);
		LogPrintf(handle,"</TIME>\n");
		LogPrintf(handle,"  </START>\n");
	}
}

/*
******* PhoneLogGenerator/WritePhoneLogEndEntry *****************************
*
*   NAME
*	WritePhoneLogEndEntry -- Write the end to a log file (V33)
*
*   SYNOPSIS
*	WritePhoneLogEndEntry(file, item);
*
*	void WritePhoneLogEndEntry(BPTR file,
*	    struct PhoneLogEntry *item);
*
*   FUNCTION
*	Write the end data for a connection to a log file opened by
*	OpenPhoneLog() after you have used WritePhoneLogStartEntry()!
*
*   INPUTS
*	file - The stdio.h file descriptor for a level 2 file that you got
*	    from OpenPhoneLog().
*	item - PhoneLogEntry structure. You must fill in following fields:
*	    EndDay, EndMonth, EndYear, EndHour, EndMin, EndSec
*	    If you want you can optionally fill in the fields:
*	    Hours, Mins, Secs
*
*   RESULT
*	None
*
*   EXAMPLE
*	...
*	BPTR file;
*	struct PhoneLogEntry item;
*
*	file = OpenPhoneLog("AmiTCP:log/AmiLog.log");
*	...
*	item.EndDay = 5;
*	item.EndMonth = 3;
*	item.EndYear = 1996; \* NOT 96! *\
*	item.EndHour = 12;
*	item.EndMin = 17;
*	item.EndSec = 0;
*	WritePhoneLogEndEntry(file,item);
*	...
*	ClosePhoneLog(file);
*	...
*
*   NOTES
*	If you want to write the optional fields Hours, Mins, Secs to the log
*	file, please remove the second comment from the source code!
*	If you want that EndDay, EndMonth and EndYear will only be written
*	to the log file if they are different to the start date, then please
*	remove the first comment from the source code, but keep in mind
*	that you now must fill the structure with StartDay, StartMonth and
*	StartYear too!
*
*   BUGS
*	No known bugs.
*
*   SEE ALSO
*	OpenPhoneLog(),ClosePhoneLog(),WritePhoneLogStartEntry(),
*	WritePhoneLogEntry(),WritePhoneLogMark()
*
*****************************************************************************
*
*
*/

void
WritePhoneLogEndEntry(PhoneLogHandle *handle, struct PhoneLogEntry *item)
{
	if(handle != NULL && item != NULL)
	{
		LogPrintf(handle,"  <END>\n");
		/*if (StartDate != EndDate)*/
		{
			LogPrintf(handle,"    <DATE>");
			LogPrintf(handle,"%04ld-%02ld-%02ld",item->EndYear,item->EndMonth,item->EndDay);
			LogPrintf(handle,"</DATE>\n");
		}
		LogPrintf(handle,"    <TIME>");
		LogPrintf(handle,"%02ld:%02ld:%02ld",item->EndHour,item->EndMin,item->EndSec);
		LogPrintf(handle,"</TIME>\n");
		LogPrintf(handle,"  </END>\n");
		/*
		if ((item->Hours > 0) || (item->Mins > 0) || (item->Secs > 0))
		{
			LogPrintf(handle,"  <PERIOD>");
			if (item->Hours > 0)
			{
				LogPrintf(handle,"%ldH",item->Hours);
			}
			if (item->Mins > 0)
			{
				LogPrintf(handle,"%02ldM",item->Mins);
			}
			if (item->Secs > 0)
			{
				LogPrintf(handle,"%02ldS",item->Secs);
			}
			LogPrintf(handle,"</PERIOD>\n");
		}
		*/
		LogPrintf(handle,"</ENTRY>\n");
	}
}
