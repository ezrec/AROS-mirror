# Makefile for gcc version of SDL

BASEDIR = $(TOP)/bin/$(AROS_TARGET_ARCH)-$(AROS_TARGET_CPU)

CC = $(BASEDIR)/tools/$(AROS_TARGET_CPU)-$(AROS_TARGET_ARCH)-aros-gcc

GCCFLAGS = -O2 -fomit-frame-pointer -I$(BASEDIR)/AROS/Include

GCCDEFINES = -DENABLE_CYBERGRAPHICS -DNO_SIGNAL_H -DAROS
# -DHAVE_OPENGL -DMORPHOS -DENABLE_AHI 

LFLAGS =  -nostdlib -nostartfiles -L. -lSDL -lamiga -larosc -larosm -lautoinit -larossupport -larosc
STARTUP = $(BASEDIR)/AROS/lib/startup.o

OBJS = SDL_error.o SDL_fatal.o SDL_RLEaccel.o SDL_blit.o SDL_blit_0.o  \
   SDL_blit_1.o SDL_blit_A.o SDL_blit_N.o \
   SDL_bmp.o SDL_cursor.o SDL_pixels.o SDL_surface.o SDL_stretch.o \
   SDL_yuv.o SDL_yuv_sw.o SDL_yuv_mmx.o SDL_video.o SDL_audio.o \
   SDL_audiocvt.o SDL_audiomem.o SDL_mixer.o SDL_wave.o \
   SDL_systimer.o SDL_timer.o SDL_joystick.o \
   SDL_sysjoystick.o SDL_cdrom.o SDL_syscdrom.o SDL_quit.o SDL_active.o \
   SDL_keyboard.o SDL_mouse.o SDL_resize.o SDL_rwops.o SDL_endian.o SDL.o \
   SDL_events.o SDL_sysmutex.o SDL_syssem.o SDL_systhread.o SDL_thread.o \
   SDL_syscond.o SDL_cgxvideo.o SDL_cgxmodes.o SDL_cgximage.o SDL_amigaevents.o \
   SDL_amigamouse.o SDL_cgxgl.o SDL_cgxwm.o SDL_cgxyuv.o SDL_cgxaccel.o \
   SDL_cgxgl_wrapper.o SDL_gamma.o 
   
%.o: %.c
	$(CC) $(GCCFLAGS) $(GCCDEFINES) -o $@ -c $*.c

all: libSDL.a 

EXAMPLES = testsprite testwin testbitmap testver testhread testsem

examples: $(EXAMPLES)
	list='$(EXAMPLES)'; for example in $$list; do \
		echo "Making $$example"; \
	    $(CC) -o $$example $(STARTUP) $$example.c $(LFLAGS) ; \
	done;
    
install: examples
	list='$(EXAMPLES)'; for subdir in $$list; do \
		echo "Installing $$subdir"; \
        mv $$subdir $(BASEDIR)/AROS/test/ ; \
	done;
 
libSDL.a: $(OBJS)
	-rm -f libSDL.a
	ar cru libSDL.a $(OBJS)
	ranlib libSDL.a

testsprite: testsprite.c libSDL.a
	$(CC) -o $@ $(STARTUP) testsprite.c $(LFLAGS) 
    
testwin: testwin.c libSDL.a
	$(CC) -o $@ $(STARTUP) testwin.c $(LFLAGS) 

testbitmap: testbitmap.c libSDL.a
	$(CC) -o $@ $(STARTUP) testbitmap.c $(LFLAGS) 

testhread: testhread.c libSDL.a
	$(CC) -o $@ $(STARTUP) testhread.c $(LFLAGS) 

testsem: testsem.c libSDL.a
	$(CC) -o $@ $(STARTUP) testsem.c $(LFLAGS) 

testver: testver.c libSDL.a
	$(CC) -o $@ $(STARTUP) testver.c $(LFLAGS) 

clean:
	rm -f *.o
