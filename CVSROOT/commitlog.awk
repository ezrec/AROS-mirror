BEGIN {
	# Who did it ?
	#User=ENVIRON["USER"];
	# Where is CVSROOT ?
	CVSROOT=ENVIRON["CVSROOT"];
	gsub(/:[a-z]+:.+@.+:/,"",CVSROOT);

	# Get the current date
	cmd="date \"+%d.%m.%Y %X\"";
	cmd | getline;
	Date=$0;
	close (cmd);

	# First message: Who did it when ?
	pout("* " User " " Date "\n\n");

	# Read the file which CVS generates
	Dir="";
	while (getline > 0)
	{
	    # If there was only a single line after the last CVS header,
	    # then Dir is already set
	    if (Dir=="")
	    {
		# The first thing is "Update of x/y/z"
		Dir=$3
	    }

	    # Get rid of the CVSROOT in Dir
	    gsub(CVSROOT "/","",Dir);

	    # Skip until the filenames
	    while (getline > 0)
	    {
		if ($0 == "Modified Files:" || $0 == "Added Files:" || $0 == "Removed Files:")
		    break;
	    }

	    # Read in all filenames
	    nfiles=0;
	    if ($0 == "Modified Files:")
		app="(M)";
	    else if ($0 == "Added Files:")
		app="(A)";
	    else
		app="(R)";

	    while (getline > 0)
	    {
		# If we have reached the logmessages, terminate the loop
		if ($0 == "Log Message:")
		    break;

		# Don't add "Added" and "Files:" to the list of files :-)
		if ($0 == "Added Files:")
		{
		    app="(A)";
		    continue;
		}
		if ($0 == "Removed Files:")
		{
		    app="(R)";
		    continue;
		}

		# Add all filenames in the line
		for (t=1; t<=NF; t++)
		    file[nfiles++] = $t app;
	    }

	    # Now show the directory and all files
	    flist="    " Dir "/: ";
	    for (t=0; t<nfiles; t++)
	    {
		if (t!=nfiles-1)
		    flist=flist file[t] ", ";
		else
		    flist=flist file[t] ":";
	    }
	    flist=wrap(flist,70);
	    gsub ("\n","\n\t",flist);
	    gsub ("\t\n","\n",flist);
	    pout(flist);
	    pout("\n");

	    # Add the logmessage
	    ende=0;
	    logmsg="";
	    Dir="";
	    while (getline > 0)
	    {
		if ($0 == "")
		{
		    ende ++;

		    if (ende==2)
			break;

		    logmsg=logmsg $0 "\n";
		}
		else
		{
		    if (ende)
		    {
			if ($1 == "Update" && $2 == "of" && match ($3,/(\/[-a-zA-Z0-9_.,]+)+/))
			{
			    Dir = $3;
			    break;
			}
		    }

		    ende=0;
		    logmsg=logmsg $0 " ";
		}

		#pout("\t" $0 "\n");
	    }

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

	close (Out);
	close (Out ".new");
}

function pout(str) {
    # Add a string to the file "Out" and "Out.new"
#print str
    printf ("%s", str) >> Out;
    printf ("%s", str) >> Out ".new";
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

