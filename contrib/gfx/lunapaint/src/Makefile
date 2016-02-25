CC=i386-aros-gcc
CFLAGS=-g -fno-strict-aliasing -funroll-loops -Wall -Wno-pointer-sign -I.
LDFLAGS=-g -lmui -lpng -lz -lfreetype2
RELEASEDIR=../../lunapaint.i386-aros

FLEXCAT=FlexCat
CP=cp
RM=rm

EXE=../Lunapaint

OBJS = \
main.o \
	core/drawtools.o \
	core/canvas.o \
	core/color.o \
	core/tools.o \
	core/effects.o \
	math/math.o \
	app/gui.o \
	app/about_window.o \
	app/toolbox.o \
	app/layers.o \
	app/animation.o \
	app/palette_editor.o \
	app/canvas.o \
	app/parts.o \
	app/new_image.o \
	app/export_import.o \
	app/project.o \
	app/preferences.o \
	app/text_to_brush.o \
	app/events.o \
	app/locale.o

SRCS= $(patsubst %.o,%.c,$(OBJS))

all: $(EXE)

$(EXE): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

app/strings.h : app/catalogs/lunapaint.cd
	$(FLEXCAT) $< $@=app/catalogs/C_h_orig.sd

clean:
	$(RM) -rf $(EXE) $(OBJS)

release:
	$(RM) -rf $(RELEASEDIR)
	$(MKDIR) $(RELEASEDIR)
	$(CP) ../BUGS ../Changelog ../COPYING ../default.palette $(RELEASEDIR)
	$(CP) ../Lunapaint.info ../Lunapaint ../lunapaint.prefs ../README $(RELEASEDIR)
	$(CP) -r ../data $(RELEASEDIR)

deps: $(SRCS) app/strings.h
	$(CC) -MM $(CFLAGS) $^


main.o: main.c math/math.h common.h core/definitions.h app/parts.h \
  config.h core/definitions.h core/canvas.h aros/debug.h \
  core/definitions.h core/color.h common.h math/math.h core/drawtools.h \
  app/toolbox.h app/canvas.h app/parts.h app/toolbox.h app/layers.h \
  config.h app/project.h app/gui.h app/events.h app/about_window.h \
  app/locale.h app/strings.h app/new_image.h app/export_import.h \
  core/canvas.h app/animation.h app/palette_editor.h app/preferences.h \
  app/text_to_brush.h app/../config.h core/definitions.h core/tools.h \
  common.h core/canvas.h app/canvas.h app/toolbox.h core/drawtools.h \
  math/math.h common.h core/effects.h core/definitions.h core/canvas.h \
  core/tools.h app/gui.h config.h app/locale.h
drawtools.o: core/drawtools.c core/drawtools.h common.h \
  core/definitions.h app/parts.h config.h core/definitions.h math/math.h \
  app/toolbox.h app/canvas.h aros/debug.h app/parts.h app/toolbox.h \
  app/layers.h app/project.h app/gui.h app/events.h app/about_window.h \
  app/locale.h app/strings.h app/new_image.h app/export_import.h \
  core/canvas.h core/definitions.h core/color.h common.h math/math.h \
  app/animation.h app/palette_editor.h app/preferences.h \
  app/text_to_brush.h app/../config.h core/tools.h core/canvas.h \
  app/canvas.h app/toolbox.h core/drawtools.h math/math.h common.h \
  core/effects.h core/canvas.h
canvas.o: core/canvas.c core/canvas.h aros/debug.h core/definitions.h \
  core/color.h common.h core/definitions.h app/parts.h config.h \
  core/definitions.h math/math.h
color.o: core/color.c core/color.h
tools.o: core/tools.c core/tools.h common.h core/definitions.h \
  app/parts.h config.h core/definitions.h core/canvas.h aros/debug.h \
  core/definitions.h core/color.h math/math.h app/canvas.h app/parts.h \
  app/toolbox.h app/canvas.h app/about_window.h app/locale.h \
  app/strings.h app/palette_editor.h app/gui.h app/events.h \
  app/new_image.h app/export_import.h core/canvas.h app/layers.h \
  app/animation.h app/preferences.h app/text_to_brush.h app/project.h \
  app/../config.h core/effects.h core/canvas.h core/definitions.h \
  core/tools.h math/math.h common.h app/toolbox.h core/drawtools.h
effects.o: core/effects.c core/effects.h core/canvas.h aros/debug.h \
  core/definitions.h core/color.h common.h core/definitions.h app/parts.h \
  config.h core/definitions.h math/math.h
math.o: math/math.c math/math.h
gui.o: app/gui.c app/gui.h app/events.h aros/debug.h app/canvas.h \
  app/parts.h config.h core/definitions.h app/toolbox.h \
  app/about_window.h app/locale.h app/strings.h app/palette_editor.h \
  app/export_import.h core/canvas.h core/definitions.h core/color.h \
  common.h core/definitions.h app/parts.h math/math.h app/new_image.h \
  app/project.h app/layers.h app/preferences.h core/effects.h \
  core/canvas.h core/tools.h app/canvas.h app/toolbox.h core/drawtools.h \
  math/math.h common.h app/animation.h app/text_to_brush.h \
  app/../config.h
about_window.o: app/about_window.c app/about_window.h app/locale.h \
  app/strings.h config.h app/parts.h core/definitions.h
toolbox.o: app/toolbox.c SDI_hook.h SDI_compiler.h app/toolbox.h \
  app/canvas.h aros/debug.h app/parts.h config.h core/definitions.h \
  app/layers.h app/project.h app/gui.h app/events.h app/about_window.h \
  app/locale.h app/strings.h app/new_image.h app/export_import.h \
  core/canvas.h core/definitions.h core/color.h common.h \
  core/definitions.h app/parts.h math/math.h app/animation.h \
  app/palette_editor.h app/preferences.h app/text_to_brush.h \
  app/../config.h core/tools.h core/canvas.h app/canvas.h app/toolbox.h \
  core/drawtools.h math/math.h common.h core/effects.h
layers.o: app/layers.c SDI_hook.h SDI_compiler.h app/layers.h \
  app/canvas.h aros/debug.h app/parts.h config.h core/definitions.h \
  app/toolbox.h app/about_window.h app/locale.h app/strings.h \
  app/palette_editor.h app/gui.h app/events.h app/new_image.h \
  app/export_import.h core/canvas.h core/definitions.h core/color.h \
  common.h core/definitions.h app/parts.h math/math.h app/animation.h \
  app/preferences.h app/text_to_brush.h app/project.h app/../config.h \
  core/effects.h core/canvas.h core/tools.h app/canvas.h app/toolbox.h \
  core/drawtools.h math/math.h common.h
animation.o: app/animation.c app/animation.h config.h app/canvas.h \
  aros/debug.h app/parts.h core/definitions.h app/toolbox.h \
  app/about_window.h app/locale.h app/strings.h app/palette_editor.h \
  app/gui.h app/events.h app/new_image.h app/export_import.h \
  core/canvas.h core/definitions.h core/color.h common.h \
  core/definitions.h app/parts.h math/math.h app/layers.h \
  app/preferences.h app/text_to_brush.h app/project.h app/../config.h \
  core/effects.h core/canvas.h core/tools.h app/canvas.h app/toolbox.h \
  core/drawtools.h math/math.h common.h
palette_editor.o: app/palette_editor.c SDI_hook.h SDI_compiler.h \
  app/palette_editor.h app/gui.h app/events.h aros/debug.h app/canvas.h \
  app/parts.h config.h core/definitions.h app/toolbox.h \
  app/about_window.h app/locale.h app/strings.h app/export_import.h \
  core/canvas.h core/definitions.h core/color.h common.h \
  core/definitions.h app/parts.h math/math.h app/new_image.h \
  app/project.h app/layers.h app/preferences.h core/effects.h \
  core/canvas.h core/tools.h app/canvas.h app/toolbox.h core/drawtools.h \
  math/math.h common.h app/animation.h app/text_to_brush.h \
  app/../config.h
canvas.o: app/canvas.c SDI_hook.h SDI_compiler.h app/canvas.h \
  aros/debug.h app/parts.h config.h core/definitions.h app/toolbox.h \
  app/about_window.h app/locale.h app/strings.h app/palette_editor.h \
  app/gui.h app/events.h app/new_image.h app/export_import.h \
  core/canvas.h core/definitions.h core/color.h common.h \
  core/definitions.h app/parts.h math/math.h app/layers.h app/animation.h \
  app/preferences.h app/text_to_brush.h app/project.h app/../config.h \
  core/effects.h core/canvas.h core/tools.h app/canvas.h app/toolbox.h \
  core/drawtools.h math/math.h common.h
parts.o: app/parts.c app/parts.h config.h core/definitions.h
new_image.o: app/new_image.c SDI_hook.h SDI_compiler.h app/new_image.h \
  app/parts.h config.h core/definitions.h app/toolbox.h app/canvas.h \
  aros/debug.h app/layers.h app/project.h app/gui.h app/events.h \
  app/about_window.h app/locale.h app/strings.h app/export_import.h \
  core/canvas.h core/definitions.h core/color.h common.h \
  core/definitions.h app/parts.h math/math.h app/animation.h \
  app/palette_editor.h app/preferences.h app/text_to_brush.h \
  app/../config.h core/tools.h core/canvas.h app/canvas.h app/toolbox.h \
  core/drawtools.h math/math.h common.h core/effects.h
export_import.o: app/export_import.c SDI_hook.h SDI_compiler.h \
  app/export_import.h app/parts.h config.h core/definitions.h \
  app/canvas.h aros/debug.h app/toolbox.h app/about_window.h app/locale.h \
  app/strings.h app/palette_editor.h app/gui.h app/events.h \
  app/new_image.h app/layers.h app/animation.h app/preferences.h \
  app/text_to_brush.h app/project.h core/canvas.h core/definitions.h \
  core/color.h common.h core/definitions.h app/parts.h math/math.h \
  app/../config.h core/effects.h core/canvas.h core/tools.h app/canvas.h \
  app/toolbox.h core/drawtools.h math/math.h common.h
project.o: app/project.c app/project.h aros/debug.h app/gui.h \
  app/events.h app/canvas.h app/parts.h config.h core/definitions.h \
  app/toolbox.h app/about_window.h app/locale.h app/strings.h \
  app/palette_editor.h app/export_import.h core/canvas.h \
  core/definitions.h core/color.h common.h core/definitions.h app/parts.h \
  math/math.h app/new_image.h app/layers.h app/preferences.h \
  core/effects.h core/canvas.h core/tools.h app/canvas.h app/toolbox.h \
  core/drawtools.h math/math.h common.h app/animation.h \
  app/text_to_brush.h app/../config.h
preferences.o: app/preferences.c SDI_hook.h SDI_compiler.h \
  app/preferences.h aros/debug.h app/gui.h app/events.h app/canvas.h \
  app/parts.h config.h core/definitions.h app/toolbox.h \
  app/about_window.h app/locale.h app/strings.h app/palette_editor.h \
  app/export_import.h core/canvas.h core/definitions.h core/color.h \
  common.h core/definitions.h app/parts.h math/math.h app/new_image.h \
  app/project.h app/layers.h core/effects.h core/canvas.h core/tools.h \
  app/canvas.h app/toolbox.h core/drawtools.h math/math.h common.h \
  app/animation.h app/text_to_brush.h app/../config.h
text_to_brush.o: app/text_to_brush.c SDI_hook.h SDI_compiler.h \
  app/text_to_brush.h aros/debug.h app/parts.h config.h \
  core/definitions.h app/toolbox.h app/canvas.h app/layers.h \
  app/project.h app/gui.h app/events.h app/about_window.h app/locale.h \
  app/strings.h app/new_image.h app/export_import.h core/canvas.h \
  core/definitions.h core/color.h common.h core/definitions.h app/parts.h \
  math/math.h app/animation.h app/palette_editor.h app/preferences.h \
  app/../config.h core/tools.h core/canvas.h app/canvas.h app/toolbox.h \
  core/drawtools.h math/math.h common.h core/effects.h
events.o: app/events.c SDI_hook.h SDI_compiler.h app/events.h \
  aros/debug.h app/canvas.h app/parts.h config.h core/definitions.h \
  app/toolbox.h app/about_window.h app/locale.h app/strings.h \
  app/palette_editor.h app/gui.h app/new_image.h app/export_import.h \
  core/canvas.h core/definitions.h core/color.h common.h \
  core/definitions.h app/parts.h math/math.h app/layers.h app/animation.h \
  app/preferences.h app/text_to_brush.h app/project.h app/../config.h \
  core/effects.h core/canvas.h core/tools.h app/canvas.h app/toolbox.h \
  core/drawtools.h math/math.h common.h
locale.o: app/locale.c app/strings.h
strings.o: app/strings.h
