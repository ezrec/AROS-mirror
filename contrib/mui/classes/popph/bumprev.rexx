/*
 *      $VER: BumpRev.rexx 1.1 (26.04.98) © Sylvain Rougier
 *
 *
 *      Example:
 *      1>      rx bumprev.rexx Name prog Version 1 Revision 0 Copyright "© by Sylvain Rougier" IncRev Verbose
 *
 */


PARSE ARG ArgLine

call addlib("RexxDosSupport.library",0,-30,2)

Template = "Name/A,Version,Revision,File/K,Copyright/K,IncRev/K/S,Verbose/K/S,Asm/K/S"
IF ~ReadArgs(ArgLine, Template, "Args.") THEN DO
	Echo "BumpRev: " Fault(RC)
	Echo "Usage: "Template
	EXIT
END

IF Args.File = 'ARGS.FILE' THEN
	FileName = Args.Name"_rev.h"
ELSE
	FileName = Args.File

IF Args.IncRev THEN DO
	IF OPEN(FH, FileName, 'R') THEN DO
		s = ReadLn(FH)  /* VERSION */
		OldVersion = WORD(s, 3)
		IF OldVersion ~= Args.Version THEN
			Args.Revision = 0
		ELSE
			DO
			s = ReadLn(FH)  /* REVISION */
			Args.Revision = WORD(s, 3) + 1
			END
		CLOSE(FH)
	END
	ELSE
		Args.Revision = 0;
END

VersionRevision = Args.Version'.'Args.Revision
DateVer = SUBSTR(DATE(SORTED), 7, 2)'.'SUBSTR(DATE(SORTED), 5, 2)'.'SUBSTR(DATE(SORTED), 3, 2)
VersTag = '$VER: 'Args.Name' 'VersionRevision' ('DateVer')"'

/*
ExpireDay     = SUBSTR(DATE(SORTED), 7, 2)
ExpireMonth   = RIGHT(SUBSTR(DATE(SORTED), 5, 2)+1, 2, "0")
ExpireYear    = SUBSTR(DATE(SORTED), 1, 4)
ExpireDateStr = ExpireDay'.'ExpireMonth'.'ExpireYear'"'
*/


say '.revinfo: 'Args.Version'.'Args.Revision
IF OPEN(FH, '.revinfo', 'W') THEN DO
	Writeln(FH, Args.Version'.'Args.Revision)
	CLOSE(FH)
END


IF OPEN(FH, FileName, 'W') THEN DO
	WriteLn(FH, '#define VERSION 'Args.Version)
	WriteLn(FH, '#define REVISION 'Args.Revision)
	WriteLn(FH, '#define VERSIONREVISION "'VersionRevision'"')      /* asked by Pierre Carrette */
	WriteLn(FH, '#define DATE "'DateVer'"')
	WriteLn(FH, '#define VERS "'Args.Name' 'VersionRevision'"')
	WriteLn(FH, '#define VSTRING "'Args.Name' 'VersionRevision' ('DateVer')\r\n"')
	WriteLn(FH, '#define VERSTAG "'VersTag)

/*
	WriteLn(FH, '#define EXPIREDATE "'ExpireDateStr)
	WriteLn(FH, '#define EXPIREDAY 'ExpireDay)
	WriteLn(FH, '#define EXPIREDMONTH 'ExpireMonth)
	WriteLn(FH, '#define EXPIREDYEAR 'ExpireYear)
*/

	CLOSE(FH)
END
ELSE
	EXIT

IF Args.Asm THEN DO
	IF OPEN(FH, Args.Name"_rev.i", 'W') THEN DO
		WriteLn(FH, "VERSION EQU "Args.Version)
		WriteLn(FH, "REVISION EQU "Args.Revision)
		WriteLn(FH, "DATE       MACRO")
		WriteLn(FH, "           dc.b  '"DateVer"'")
		WriteLn(FH, "   ENDM")
		WriteLn(FH, "VERS       MACRO")
		WriteLn(FH, "           dc.b  '"Args.Name" "VersionRevision"'")
		WriteLn(FH, "   ENDM")
		WriteLn(FH, "VSTRING    MACRO")
		WriteLn(FH, "           dc.b  '"Args.Name" "VersionRevision" ("DateVer")',13,10,0")
		WriteLn(FH, "   ENDM")
		WriteLn(FH, "VERSTAG    MACRO")
		WriteLn(FH, "           dc.b  '$VER: "Args.Name" "VersionRevision" ("DateVer") "TIME(NORMAL)" "Args.Copyright"',0") 
		WriteLn(FH, "   ENDM")
		CLOSE(FH)
	END
	ELSE
		EXIT
END

IF Args.Verbose THEN DO
	Echo VersTag
END
