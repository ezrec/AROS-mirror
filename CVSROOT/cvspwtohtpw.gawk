BEGIN {
    FS=":";
    file=ARGV[1];
    out_pw=ARGV[2];
    out_acl=ARGV[3];

    user="";

    printf ("") > out_pw;

    while ((getline < file) > 0)
    {
	if (substr($1,1,1)=="#" || $1=="")
	    continue;

	print $1":"$2 >> out_pw;

	user=user" "$1;
    }


    close (file);
    close (out_pw);

    n=0;
    FS=" ";

    while ((getline < out_acl) > 0)
    {
	if ($1 == "require" && $2 == "user")
	{
	    #print user
	    line[n++] = $1 " " $2 " " user;
	}
	else
	    line[n++] = $0;
    }

    close (out_acl);

    printf ("") > out_acl;

    for (t=0; t<n; t++)
	print line[t] >> out_acl;

    close (out_acl);
}
