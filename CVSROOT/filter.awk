BEGIN { FS=":" }
/^$/ { next }
/^#/ { next }
 {
    # name:passwd:user:name:email

    print $1":"$2":aros" > "passwd"
    print $1":"$3":"$4":"$5 > "developers"
 }
