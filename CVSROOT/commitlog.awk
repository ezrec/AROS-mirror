BEGIN {
	User=ENVIRON["USER"];
	cmd="date \"+%d.%m.%Y %X\"";
	cmd | getline;
	Date=$0;
	close (cmd);
	CVSROOT=ENVIRON["CVSROOT"];

	pout("* " User " " Date "\n\n");

	while (getline > 0)
	{
	    Dir=$3
	    gsub(CVSROOT "/","",Dir);

	    while (getline > 0)
	    {
		if ($0 == "Modified Files:")
		    break;
	    }

	    nfiles=0;

	    while (getline > 0)
	    {
		if ($0 == "Log Message:")
		    break;

		for (t=1; t<=NF; t++)
		    file[nfiles++] = $t;
	    }

	    pout("    " Dir "/: ");
	    for (t=0; t<nfiles; t++)
	    {
		pout(file[t]);

		if (t!=nfiles-1)
		    pout(", ");
		else
		    pout(":\n");
	    }

#	     logmsg="";
	    ende=0;
	    while (getline > 0)
	    {
		if ($0 == "")
		{
		    ende ++;

		    if (ende==2)
			break;
		}
		else
		    ende=0;

		pout("\t" $0 "\n");
#		 logmsg=logmsg $0 " ";
	    }

#print "1"
#	     wrap(logmsg,70);
#print "2"
#	     gsub ("\n","\n\t",logmsg);
#print "3"
#	     pout(logmsg);
#print "4"
	}

	close (Out);
	close (Out ".new");
}

function pout(str) {
    printf ("%s", str) >> Out;
    printf ("%s", str) >> Out ".new";
}

function wrap(str,width         ,line,rest,pos) {
    rest=str;
    str="";

    while (rest!="")
    {
#print "5" rest
	line=substr(rest,1,width);
	pos=rindex(line," ");
	if (pos)
	{
	    line=substr(rest,1,pos);
	    rest=substr(rest,pos+1);
	}
	else
	    rest=substr(rest,width+1);

	str=str line "\n";
#print "6" str
    }
#print "7" str
}

function rindex(s,t         ,pos,nextpos) {
    pos=0;
    nextpos=index(s,t);
    while (nextpos)
    {
#print "8 " pos "\"" substr(s,pos+1) "\""
	pos+=nextpos;
	nextpos=index(substr(s,pos+1),t);
    }
#print "9 " pos
    return pos;
}

