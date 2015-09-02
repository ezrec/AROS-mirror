CC     := ppc-amigaos-gcc
STRIP  := ppc-amigaos-strip
AR     := ppc-amigaos-ar
RANLIB := ppc-amigaos-ranlib
RM     := rm -f

CFLAGS  := -std=gnu99 -O3 -g -ffast-math -Wall -I. -Iinclude -IOpenAL32/Include -IAlc \
           -DAL_ALEXT_PROTOTYPES
LDFLAGS := -use-dynld
LIBS    := -lpthread
PIC     := -fPIC -DPIC

STATIC_OBJS := \
	OpenAL32/alAuxEffectSlot.o \
	OpenAL32/alBuffer.o \
	OpenAL32/alEffect.o \
	OpenAL32/alError.o \
	OpenAL32/alExtension.o \
	OpenAL32/alFilter.o \
	OpenAL32/alFontsound.o \
	OpenAL32/alListener.o \
	OpenAL32/alMidi.o \
	OpenAL32/alPreset.o \
	OpenAL32/alSoundfont.o \
	OpenAL32/alSource.o \
	OpenAL32/alState.o \
	OpenAL32/alThunk.o \
	OpenAL32/sample_cvt.o \
	Alc/ALc.o \
	Alc/ALu.o \
	Alc/alcConfig.o \
	Alc/bs2b.o \
	Alc/helpers.o \
	Alc/hrtf.o \
	Alc/mixer.o \
	Alc/mixer_c.o \
	Alc/panning.o \
	Alc/backends/ahi.o \
	Alc/backends/base.o \
	Alc/backends/loopback.o \
	Alc/backends/null.o \
	Alc/backends/wave.o \
	Alc/effects/autowah.o \
	Alc/effects/chorus.o \
	Alc/effects/compressor.o \
	Alc/effects/dedicated.o \
	Alc/effects/distortion.o \
	Alc/effects/echo.o \
	Alc/effects/equalizer.o \
	Alc/effects/flanger.o \
	Alc/effects/modulator.o \
	Alc/effects/null.o \
	Alc/effects/reverb.o \
	Alc/midi/base.o \
	Alc/midi/dummy.o \
	Alc/midi/fluidsynth.o \
	Alc/midi/sf2load.o \
	Alc/midi/soft.o \
	common/threads.o \
	common/rwlock.o \
	common/uintmap.o

SHARED_OBJS := $(STATIC_OBJS:.o=.lo)

all: libopenal.a libopenal.so utils/openal-info

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.lo: %.c
	$(CC) $(CFLAGS) $(PIC) -c -o $@ $<

libopenal.a: $(STATIC_OBJS)
	$(AR) -crv $@ $^
	$(RANLIB) $@
	
libopenal.so: $(SHARED_OBJS)
	$(CC) -shared -Wl,-soname,$@ -o $@.debug $^ -lpthread
	$(STRIP) -R.comment -o $@ $@.debug

utils/openal-info: utils/openal-info.o libopenal.so
	$(CC) $(LDFLAGS) -o $@.debug $^ $(LIBS)
	$(STRIP) -R.comment -o $@ $@.debug

clean:
	$(RM) libopenal.a libopenal.so utils/openal-info *.debug */*.debug */*.o */*/*.o */*.lo */*/*.lo

