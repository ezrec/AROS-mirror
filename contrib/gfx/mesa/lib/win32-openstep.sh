# This file should be run from the Bourne shell
# provided for OpenStep implementations under WIN32

mkdir lib
( cd src; make -f Makefile.OpenStep openstep )
( cd src-glu; make -f Makefile.OpenStep openstep )
( cd OpenStep/MesaView; make )
