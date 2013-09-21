/* AnnPEGCC (02-09-2012) by Chris Handley.
   Feel free to modify & distribute this script as you wish!
*/
OPTIONS RESULTS
CALL Addlib('rexxsupport.library',0,-30,0)

/* parse parameters Run/S,Compile/S,Options/F */
argRun = 0
argCompile = 0
argOptions = ''
args = Arg(1)
DO UNTIL args = ''
	PARSE VAR args next ' ' args
	IF Upper(next) = 'RUN' THEN
		argRun = 1
	ELSE IF Upper(next) = 'COMPILE' THEN
		argCompile = 1
	ELSE
		argOptions = argOptions||' '||next
END
/*Say '# argRun='||argRun||', argCompile='||argCompile||', argOptions="'||argOptions||'"'*/

/* get file currently being editied */
ADDRESS "Annotate_Rexx"
GET WIN_NAME 0
path = RESULT
IF Right(path, 2) ~= '.e' THEN DO
	Say 'ERROR: Your filename must end in .e for PEGCC to compile it!'
	EXIT
END
path = Left(path, Length(path)-2)
/*SAY '# "'||path||'"'*/

/* split path into file & dir */
PARSE VALUE Reverse(path) WITH file '/' dir
IF dir = '' THEN DO
	/* handle the case where there is no directory */
	PARSE VALUE Reverse(path) WITH file ':' dir
	IF dir ~= '' THEN dir = ':'||dir
END
dir  = Reverse(dir)
file = Reverse(file)
/*SAY '# dir="'||dir||'", file="'||file||'"'*/

/* add escaped quotes if file & dir contain spaces */
fileNoQuotes = file
 dirNoQuotes = dir
IF Index(file, ' ') ~= 0 THEN file = '*"'||file||'*"'
IF Index( dir, ' ') ~= 0 THEN  dir = '*"'|| dir||'*"'
/*SAY '# dir='||dir||', file='||file||''*/

/* sanity check request */
IF ~argCompile THEN DO
	IF ~Exists(path) THEN DO
		Say 'ERROR: You must compile the program (with PEGCC) before you can Run it!'
		EXIT
	END
END

/* perform request */
temp = 'T:AnnPEGCC'
IF ~Open(.file, temp, 'WRITE') THEN DO
	Say 'ERROR: Failed to create temporary file "'||temp||'".'
	EXIT
END
IF dir ~= '' THEN DO
	CALL WriteLn(.file, 'Echo "> CD '||dir||'"')
	CALL WriteLn(.file, 'CD "'||dirNoQuotes||'"')
END
IF argCompile THEN DO
	CALL WriteLn(.file, 'Stack 100000')
	CALL WriteLn(.file, 'Echo "> PEGCC '||file||argOptions||'"')
	CALL WriteLn(.file, 'PEGCC "'||fileNoQuotes||'"'||argOptions)
	CALL WriteLn(.file, 'IF NOT WARN')
END
IF argRun THEN DO
	CALL WriteLn(.file, '	Echo "> '||file||'"')
	CALL WriteLn(.file, '	"'||path||'"') /*the use of path instead of file is intentional, to ensure it fails if it trys to execute after a failed compilation, rather than searching the command path*/
END
IF argCompile THEN DO
	CALL WriteLn(.file, 'ENDIF')
END
CALL Close(.file)
ADDRESS COMMAND 'Execute "'||temp||'"'
CALL Delete(temp)

/*SAY '# DONE'*/
