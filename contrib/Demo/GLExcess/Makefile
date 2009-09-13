# ----------------------------------------------------------------------------
# Special flags for the compiler/linker
# ----------------------------------------------------------------------------

CXXFLAGS=-Wall -O2
LDFLAGS= -lGL -lGLU -lglut

# ----------------------------------------------------------------------------
# Source listing and derived objects
# ----------------------------------------------------------------------------

SOURCES= \
	glxsglut.cpp \
	scene01.cpp \
	scene02.cpp \
	scene03.cpp \
	scene04.cpp \
	scene05.cpp \
	scene06.cpp \
	scene07.cpp \
	scene08.cpp \
	scene09.cpp \
	scene10.cpp \
	scene11.cpp \
	scene12.cpp \
	Texture.cpp \
	utils.cpp

OBJECTS=$(SOURCES:.cpp=.o)

# ----------------------------------------------------------------------------
# Main target definition
# ----------------------------------------------------------------------------

MAIN_TARGET=glxs

# ----------------------------------------------------------------------------
# Rules
# ----------------------------------------------------------------------------

all: $(MAIN_TARGET)

.PHONY: clean

$(MAIN_TARGET): $(OBJECTS)
	@echo "  L: $(@D)/$(@F)"
	@$(CXX) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	@echo "  C: $(@D)/$(<F)"
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	@echo "  Cl: Object files and target binary"
	@rm -rf $(OBJECTS) $(MAIN_TARGET)
