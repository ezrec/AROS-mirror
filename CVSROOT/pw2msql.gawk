BEGIN { 
    FS=":" 
    print "delete from users where login <> ''"
    print "\\g"
}
/^$/ { next }
/^#/ { next }
 {
    if ($1!="")
    {
	print "insert into users (login,name,email,comment)"
	print "values('"$1"','"$3"','"$4"','"$5"')"
	print "\\g"
    }
}
