BEGIN { FS=":" }
/^$/ { next }
/^#/ { next }
 {
    # name:passwd:user:name:email

    print $1":"$2":"$3 > "passwd"
    print $1":"$4":"$5":"$6 > "developers"
 }
