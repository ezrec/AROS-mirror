CXX      = mwcc
CXXFLAGS = -I- -I. -I../../../include -O7 -fp_contract on -inline auto -ansi strict
LIBS     = 
LFLAGS   = 
AR       = ar
RANLIB   = : No ranlib necessary for

.SUFFIXES: .cpp
%.o: %.cpp
	$(COMPILE.cc) $< $(OUTPUT_OPTION)

SRCS = XaoSApplication.cpp \
       XaoSSplashView.cpp \
       XaoSSplashWindow.cpp \
       XaoSView.cpp \
       XaoSWindow.cpp \
       ui_be.cpp \
       be_main.cpp

OBJS = $(SRCS:.cpp=.o)

TLIB = ../../../lib/libuibe.a


all:	$(TLIB) XaoS.rsrc

$(TLIB):$(OBJS)
	rm -f $@
	$(AR) rc $@ $^
	$(RANLIB) $@ 

XaoS.rsrc: XaoS.r XaoSResources SplashScreen
	mwbres -o $@ XaoS.r

clean:
	rm -f $(TLIB)
	rm -f *.[oas]
	rm -f *~
	rm -f core 

distclean:	clean
