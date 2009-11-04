CC = gcc -mcrt=newlib
AR = ar
RM = delete
CFLAGS = -O3 -Wall -DPIC -fPIC -I. -Iinclude -IOpenAL32/Include
LIBS = -L. -lopenal -lpthread

OBJS = OpenAL32/alAuxEffectSlot.o \
	OpenAL32/alBuffer.o \
	OpenAL32/alEffect.o \
	OpenAL32/alError.o \
	OpenAL32/alExtension.o \
	OpenAL32/alFilter.o \
	OpenAL32/alListener.o \
	OpenAL32/alSource.o \
	OpenAL32/alState.o \
	OpenAL32/alThunk.o \
	Alc/ALc.o Alc/ALu.o Alc/ahi.o Alc/wave.o \
	Alc/alcConfig.o Alc/alcReverb.o \
	Alc/alcThread.o Alc/bs2b.o

all: libopenal.a libopenal.so examples/openal-info

libopenal.a: $(OBJS)
	$(AR) -crv $@ $^

libopenal.so: $(OBJS)
	$(CC) -shared -nostartfiles -o $@ $^ -lpthread

examples/openal-info: examples/openal-info.o libopenal.so
	$(CC) -use-dynld -o $@ examples/openal-info.o $(LIBS)

clean:
	$(RM) $(OBJS)
