#!/bin/sh

if [ z$FEELINTOP == z ] ; then
 echo "FEELIN env variables are not setup. Do '. arosprepare.sh' in Feelin top level directory first!"
 exit 1
fi

echo -e "\n==[Making libfeelin_aros.a]==================\n"

cd $FEELINTOP/Include/lib
make -f makefile.gcc.aros || exit 1

echo -e "\n==[Making feelin.library]==================\n"

cd $FEELINTOP/Sources/feelin
make -f makefile.aros || exit 1

echo -e "\n==[Making Feelin Prefs program]==================\n"

cd $FEELINTOP
make -f makefile.prefs.aros || exit 1

echo -e "\n==[Making Feelin Demo programs]==================\n"

cd $FEELINTOP/Demos
make -f makefile.aros || exit 1

for i in `find $FEELINTOP/Sources -name makefile.aros`; do
 cd `dirname $i`
 if test -f makefile.aros; then
 	echo -e "\n==[Making " `dirname $i` "]==================\n"
 	make -f makefile.aros || exit 1
 fi
done
