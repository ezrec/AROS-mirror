CC = m68k-amigaos-gcc

CPU = -m68020

All: ../../DOpusRT

../../DOpusRT: DOpusRT.o
	$(CC) -o ../../DOpusRT DOpusRT.o $(CPU) -noixemul -resident -ldebug

DOpusRT.o: DOpusRT.c
	$(CC) -c DOpusRT.c -O3 -Wall -I../include $(CPU) -fomit-frame-pointer -fbaserel -msmall-code

