BEGIN {
    FS=":"; names[""] = 1; 
    file="aros.org.mailheader";
    while ((getline line < file) > 0)
    {
	print line;
    }
    close(file);
}
/^[ \t]*(#|$|anon:)/ { next; }
 {
    # name:passwd:user:name:email
    name = $1
    if (name in names)
    {
	print "Duplicate name \""name"\"" > "/dev/stderr"
	exit (1);
    }
    names[name] = 1;

    print name": "$4
 }
