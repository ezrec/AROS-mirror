BEGIN { FS=":"; names[""] = 1; }
/^[ \t]*(#|$)/ { next }
 {
    # name:passwd:user:name:email
    name = $1
    if (name in names)
    {
	print "Duplicate name \""name"\"" > "/dev/stderr"
	exit (1);
    }
    names[name] = 1;

    print name":"$2":cvs" > "passwd"
    print name":"$3":"$4":"$5 > "developers"
 }
