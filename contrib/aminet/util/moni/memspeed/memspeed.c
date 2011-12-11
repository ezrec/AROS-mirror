/*
 *  Compilation:
 *
 *    gcc -O3 -fomit-frame-pointer MemSpeed.c -o MemSpeed
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <sys/types.h>

#define KB		(1024)
#define MB		(KB*KB)

#define TESTSIZE	(1*MB)
#define LOOPSIZE	(256*MB)

int main(int argc, char *argv[])
{
    unsigned long int mb = TESTSIZE;
    unsigned long int size, passes, i, j;
    volatile unsigned long int *mem;
    clock_t start, stop;

    switch (argc) {
	case 2:
	    mb = atol(argv[1])*MB;
	case 1:
	    break;

	default:
	    fprintf(stderr, "Usage: %s megabytes\n", argv[0]);
	    exit(1);
	    break;
    }

    mem = malloc(mb);
    if (mem == NULL)
    {
      fprintf (stderr, "Couldn't get %ld megabytes of memory.\n", mb / MB);
      return (10);
    }

    fprintf(stderr, "*** MEMORY WRITE PERFORMANCE (%d MB LOOP) ***\n",
	    LOOPSIZE/MB);
    for (size = 64; size <= mb; size <<= 1) {
	passes = LOOPSIZE/size;
	fprintf(stderr, "size = %9ld bytes: ", size);
        start = clock ();
	for (i = 0; i < passes; i++)
	    for (j = 0; j < size/sizeof(unsigned long int); j += 16) {
		mem[j] = 0; mem[j+1] = 0; mem[j+2] = 0; mem[j+3] = 0;
		mem[j+4] = 0; mem[j+5] = 0; mem[j+6] = 0; mem[j+7] = 0;
		mem[j+8] = 0; mem[j+9] = 0; mem[j+10] = 0; mem[j+11] = 0;
		mem[j+12] = 0; mem[j+13] = 0; mem[j+14] = 0; mem[j+15] = 0;
	    }
        stop = clock ();
	fprintf(stderr, "%5.3f MB/s\n",
		(double)(LOOPSIZE/MB)/(double)(stop-start)*(double)CLOCKS_PER_SEC);
    }
    fprintf(stderr, "*** MEMORY READ PERFORMANCE (%d MB LOOP) ***\n",
	    LOOPSIZE/MB);
    for (size = 64; size <= mb; size <<= 1) {
	passes = LOOPSIZE/size;
	fprintf(stderr, "size = %9ld bytes: ", size);
        start = clock ();
	for (i = 0; i < passes; i++)
	    for (j = 0; j < size/sizeof(unsigned long int); j += 16) {
	        volatile unsigned long int d;
		d = mem[j]; d = mem[j+1]; d = mem[j+2]; d = mem[j+3];
		d = mem[j+4]; d = mem[j+5]; d = mem[j+6]; d = mem[j+7];
		d = mem[j+8]; d = mem[j+9]; d = mem[j+10]; d = mem[j+11];
		d = mem[j+12]; d = mem[j+13]; d = mem[j+14]; d = mem[j+15];
		(void)d;
	    }
        stop = clock ();
	fprintf(stderr, "%5.3f MB/s\n",
		(double)(LOOPSIZE/MB)/(double)(stop-start)*(double)CLOCKS_PER_SEC);
    }
    free ((void *)mem);
    exit(0);
}
