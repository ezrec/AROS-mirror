BEGIN {
    stderr = "/dev/stderr";

    # Who did it ?
    #system ("printenv > /tmp/cvs.env");
    User=ENVIRON["CVSLOGINNAME"];
    if (User == "")
	User=ENVIRON["LOGNAME"];
    if (User == "")
	User=ENVIRON["USER"];
    # Where is CVSROOT ?
    CVSROOT=ENVIRON["CVSROOT"];
    gsub(/:[a-z]+:.+@.+:/,"",CVSROOT);

    # Get the current date
    cmd="date \"+%d.%m.%Y %X\"";
    cmd | getline;
    Date=$0;
    close (cmd);

    updatePattern = "Update of "CVSROOT"/";
    uplen = length (updatePattern);
}
/^Modified Files:$/ {
    suffix = "(M)";
    mode = "files";
    next;
}
/^Added Files:$/ {
    suffix = "(A)";
    mode = "files";
    next;
}
/^Removed Files:$/ {
    suffix = "(R)";
    mode = "files";
    next;
}
/^Log Message:$/ {
    msg = "";
    mode = "logmsg";
    next;
}
/^Update of / {
    if (logmsg)
	dump();

    mode = "";
    nfiles = 0;
    logmsg = "";

    Dir = $3;

    # Remove CVSROOT from Dir
    gsub (CVSROOT"/", "", Dir);

    # Find the first name in the path
    Area = Dir;
    sub (/\/.*/, "", Area);
#print Dir,":",Area

    next;
}
 { 
    if (mode == "files")
    {
	for (t=1; t<=NF; t++)
	{
	    file[nfiles++] = $t suffix;
	}
    }
    else if (mode == "logmsg")
    {
	logmsg=logmsg $0 " ";
    }
}
END {
    if (logmsg)
	dump();
}

function dump(t,flist)
{
    # First: Who did it when ?
    pout("* " User " " Date " " Dir "/\n\n");

    # Now show the directory and all files
    flist="";
    for (t=0; t<nfiles; t++)
    {
	if (t!=nfiles-1)
	    flist=flist file[t] ", ";
	else
	    flist=flist file[t] ":";
    }
    flist=wrap(flist,70);
    gsub ("\n","\n    ",flist);
    gsub (" +\n","\n",flist);
    pout("    "flist);
    pout("\n");

    gsub (/\n*$/,"",logmsg);
    gsub (/^\n*/,"",logmsg);
    logmsg=wrap(logmsg,70);
#print "2" logmsg
    gsub ("\n","\n\t",logmsg);
    gsub ("\t\n","\n",logmsg);
#print "3" logmsg
    pout("\t" logmsg);
    pout("\n");
}

function pout(str) {
    # Add a string to the file "Out" and "Out.new"
#printf ("%s", str);
    printf ("%s", str) >> Out;
    printf ("%s", str) >> Out ".new." Area;
}

function wrap(str,width         ,line,rest,pos) {
    # wordwrap a string
    rest=str;
    str="";

    while (rest!="")
    {
	# Estimate one line
	line=substr(rest,1,width);

	# if there is a CR in the line, wrap there
	pos = index(line,"\n");

	if (pos)
	{
	    # Now cut the line at the CR (the CR is taken away by this)
	    line=substr(rest,1,pos);
	    rest=substr(rest,pos+1);
	}
	else if (length (rest) > width)
	{
	    # Find the last whitespace in the estimate
	    pos=rindex(line," ");
	    if (pos)
	    {
		# Now cut the real thing
		line=substr(rest,1,pos);
		rest=substr(rest,pos+1);
	    }
	    else
		rest=substr(rest,width+1);

	}
	else
	    rest="";

	# Add the wrapped line and a CR to the result
	str=str line "\n";
    }
    return str;
}

function rindex(s,t         ,pos,nextpos) {
    # If there is no string "t" in "s", we return 0
    pos=0;

    # Find the next position which matches the searchstring
    nextpos=index(s,t);

    # As long as there any
    while (nextpos)
    {
	# Advance pos
	pos+=nextpos;

	# Find the next "t" beginning at pos+1 (because there is one at pos)
	nextpos=index(substr(s,pos+1),t);
    }

    return pos;
}

