#!/bin/sh -
# This script generates ed test scripts, with extensions .ed and .red, from 
# .t and .err files, respectively.

PATH="/bin:/usr/bin:/usr/local/bin/:."
ED=$1
TMP=$2
TESTDIR=`pwd`

if test  ! -f $ED; then
	echo "$ED: cannot execute"
	exit 1
fi

for i in *.t; do
#	base=${i%.*}
#	base=`echo $i | sed 's/\..*//'`
	base=`expr $i : '\([^.]*\)'`
	(
	echo "#!/bin/sh -"
	echo "$ED - <<'EOT'"
	echo H
	echo "r ${TESTDIR}/$base.d"
	cat $i
	echo "w $base.o"
	echo EOT
	) >${TMP}$base.ed
	chmod +x ${TMP}$base.ed
# The following is pretty ugly way of doing the above, and not appropriate 
# use of ed  but the point is that it can be done...
#	base=`$ED - \!"echo $i" <<-EOF
#		s/\..*
#	EOF`
#	$ED - <<-EOF
#		a
#		#!/bin/sh -
#		$ED - <<'EOT'
#		H
#		r ${TESTDIR}/$base.d
#		w $base.o
#		EOT
#		.
#		-2r $i
#		w ${TMP}$base.ed
#		!chmod +x ${TMP}$base.ed
#	EOF
done

for i in *.err; do
#	base=${i%.*}
#	base=`echo $i | sed 's/\..*//'`
	base=`expr $i : '\([^.]*\)'`
	(
	echo "#!/bin/sh -"
	echo "$ED - <<'EOT'"
	echo H
	echo "r ${TESTDIR}/$base.err"
	cat $i
	echo "w $base.ro"
	echo EOT
	) >${TMP}$base.red
	chmod +x ${TMP}$base.red
# The following is pretty ugly way of doing the above, and not appropriate 
# use of ed  but the point is that it can be done...
#	base=`$ED - \!"echo $i" <<-EOF
#		s/\..*
#	EOF`
#	$ED - <<-EOF
#		a
#		#!/bin/sh -
#		$ED - <<'EOT'
#		H
#		r ${TESTDIR}/$base.err
#		w $base.ro
#		EOT
#		.
#		-2r $i
#		w ${TMP}${base}.red
#		!chmod +x ${TMP}${base}.red
#	EOF
done
