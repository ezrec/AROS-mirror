#include <stdio.h>
#include <string.h>

#include <proto/exec.h>

void perror(char *msg)
{
    printf("AROS: perror: %s\n", msg);
}

void raise(void)
{
    printf("AROS: raise: I don't know what do to! Freezing!\n");
    Wait(0);
}

int clock(void)
{
    static int counter = 1;
 
    printf("AROS: clock\n");   
    counter += 100000;
    
    return counter;
}


#ifndef AROSMAME_USEZLIB

int compress(void)
{
    printf("AROS: compress: I cannot zip compress!\n");
    return 0;
}

int uncompress(void)
{
    printf("AROS: uncompress: I cannot zip uncompress!\n");
    return 0;
}


int gzopen(void)
{
    printf("AROS: gzopen: Who needs this anyway!\n");

    return 0;
}

void gzclose(void)
{
    printf("AROS: gzclose: Shit, what now?\n");
}

int gzgets(void)
{
    printf("AROS: gzgets: You can get noooo ...\n");
    return 0;
}

int gzprintf(void)
{
    printf("AROS: gzprintf: No way! It's really getting silly.\n");     
    return 0;
}

int inflate(void)
{
    printf("AROS: inflate: can't do that, sorry!\n");
    return 0;
}

int inflateEnd(void)
{
    printf("AROS: inflateEnd: can't do that, sorry!\n"); 
    return 0;
}

int inflateInit2_(void)
{
    printf("AROS: inflateInit2: can't do that, sorry!\n"); 
    return 0;
}

#endif

