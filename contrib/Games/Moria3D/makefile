
TOP = ../..

include $(TOP)/config/make.cfg

prefix = $(BINDIR)/Games/Moria3D

all :
	cd Source ; make all TOP="$(TOP)/.."

install :
	$(MKDIR) $(prefix)/bin
	$(CP) Levels $(prefix)/
	$(CP) Textures $(prefix)/
	$(CP) Source/Moria3D.Linux $(prefix)/Moria3D
	$(CP) Missions $(prefix)/

