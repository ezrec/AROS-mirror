/* Quit.rexx */

options results;
address command;

selection = Request("Quit Scalos", "What would you like to do now?", "Quit|Reboot|Shutdown|Cancel")

select
	when selection = '1' then do
		/* say "Quit" */
		address workbench 'menu invoke "WORKBENCH.QUIT"';
		end;
	when selection = '2' then do
		doReboot = Request("Reboot Machine", "Are you sure you want to reboot now?", "Reboot|Cancel")
		/* say "Reboot" doReboot */
		if doReboot = '1' then do
			/* say "Reboot now!" */
			address command "c:reboot"
			end;
		end;
	when selection = '3' then do
		doShutdown = Request("Shutdown Machine", "Are you sure you want to shutdown now?", "Shutdown|Cancel")
		/* say "Shutdown" doShutdown */
		if doShutdown = '1' then do
			/* say "Shutdown now!" */
			address command "c:shutdown"
			end;
		end;
	otherwise do
		say "Cancel"
		end;
end

exit;


Request : procedure
do
	parse arg title, body, gadgets

	tmpfname = "t:xyzzy"

	/* say "title=" title */
	address command;

	'requestchoice > ' tmpfname 'title "' title '" body "' body '" gadgets "' gadgets '"';

	rc = open( 'tmpfile', tmpfname, 'R' )
	instring = readln( 'tmpfile' )
	rc = close( 'tmpfile' )

	return instring;
end;

