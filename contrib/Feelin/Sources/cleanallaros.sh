#!/bin/sh

if [ z$FEELINTOP == z ] ; then
 echo "FEELIN env variables are not setup. Do '. arosprepare.sh' in Feelin top level directory first!"
 exit 1
fi

echo -e "\n==[Cleaning libfeelin_aros.a]==================\n"

cd $FEELINTOP/Include/lib
make -f makefile.gcc.aros clean || exit 1

echo -e "\n==[Cleaning feelin.library]==================\n"

cd $FEELINTOP/Sources/feelin
make -f makefile.aros clean || exit 1

echo -e "\n==[Cleaning Feelin Prefs program]==================\n"

cd $FEELINTOP
make -f makefile.prefs.aros clean || exit 1

echo -e "\n==[Cleaning Feelin Classes]==================\n"

echo -e "\n==[Cleaning Feelin Demo programs]==================\n"

cd $FEELINTOP/Demos
make -f makefile.aros clean || exit 1

cd $FEELINTOP
find . -name "*.o" | xargs rm
find . -name "*.fc" | xargs rm
